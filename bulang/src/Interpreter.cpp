#include "pch.h"

#include "Interpreter.hpp"
#include "Utils.hpp"



class BreakException : public std::runtime_error
{
public:
    explicit BreakException(std::string message) : std::runtime_error(message) {}
};

class ContinueException : public std::runtime_error
{
public:
    explicit ContinueException(std::string message) : std::runtime_error(message) {}
};



class ReturnException : public std::runtime_error
{
public:
    explicit ReturnException(ExprPtr value) : std::runtime_error("return"), value(value) {}
    ExprPtr value {nullptr};
};


ExprPtr Compiler::visit(ExprPtr node)
{
    if (!node) std::make_shared<Literal>();
    return node->accept(*this);
}

ExprPtr Compiler::visit_assign(Assign *node)
{
    if (!node) return std::make_shared<Literal>();
    ExprPtr value = evaluate(node->value);

    

    if (!environment->assign(node->name.lexeme, std::move(value)))
    {

      
       throw FatalException("Undefined variable: '" + node->name.lexeme +"' at line  "+ std::to_string(node->name.line )+" or mixe types.");
    }


    return value;
}

ExprPtr Compiler::evaluate(ExprPtr node)
{
   
    if (!node)
    {
        WARNING("Evaluation error: Unknown expression type");
        return std::make_shared<Literal>();
    }
    std::shared_ptr<Expr>  result = visit(node);
    return result;
}


ExprPtr Compiler::visit_call_native(CallExpr *node)
{
 //   INFO("CALL: %s ",node->name.lexeme.c_str());

    interpreter->context->clear();
    for (u32 i = 0; i < node->args.size(); i++)
    {
        ExprPtr arg = evaluate(node->args[i]);
        Literal *l = static_cast<Literal *>(arg.get());
        interpreter->context->add(std::move(arg),l);
        
    }
    ExprPtr  result = interpreter->CallNativeFunction(node->name.lexeme,(int) node->args.size());

    return result;
}
ExprPtr Compiler::visit_call_struct(const ExprPtr &var,CallExpr *node,Expr *expr) 
{

    StructLiteral *original = static_cast<StructLiteral *>(var.get());
    std::shared_ptr<StructLiteral> result = std::make_shared<StructLiteral>();
    result->name = node->name.lexeme;
     if (!node->args.empty())
     {
         if (node->args.size() > original->members.size())
         {
             WARNING("Too many arguments in struct call: '%s' (pass %d / %d have) ", node->name.lexeme.c_str(), node->args.size(), original->members.size());
         }
    }
    u32 index = 0;
    for (auto it = original->members.begin(); it != original->members.end(); it++)
    {
         result->members[it->first] =  it->second->clone();
        if (!node->args.empty())
        {
            if (index < node->args.size())
            {
                ExprPtr arg = evaluate(node->args[index]);
                std::string name = it->first;
                result->members[name] = std::move(arg);
            }
            index++;
        } 
    } 

    return result;


}

ExprPtr Compiler::visit_call_function(CallExpr *node,Expr *callee) 
{
    auto previousEnvironment = environment;

    Function *function = static_cast<Function *>(callee);
    if (function->arity != node->args.size())
    {
        throw FatalException("Incorrect number of arguments in call to '" + node->name.lexeme +"' at line "+ std::to_string(node->name.line )+ " expected " + std::to_string(function->arity) + " but got " + std::to_string(node->args.size()));
    }

    std::shared_ptr<Environment>  local = std::make_shared<Environment>(environment);

    for (u32 i = 0; i < node->args.size(); i++)
    {
        ExprPtr arg = evaluate(node->args[i]);
        local->define(function->args[i], std::move(arg));
    }
    ExprPtr result = nullptr;
    try  
    {
        BlockStmt *body = static_cast<BlockStmt *>(function->body.get());
        execte_block(body, local.get());
        
    }
    catch (ReturnException &e)
    {
        result = e.value;
    }  
    


    if (result == nullptr)
    {
        result = std::make_shared<Literal>();
    }

    environment = previousEnvironment;

    return result;
}
ExprPtr Compiler::visit_call_class(const ExprPtr &result,CallExpr *node, Expr *callee) //contructor
{
  //  INFO("CREATE CLASS: %s %d", node->name.lexeme.c_str(), node->args.size());

   
    ClassLiteral *main = static_cast<ClassLiteral *>(result.get());

    ExprPtr parent = nullptr;
    if (main->isChild)
    {
        parent = environment->get(main->parentName);
        if (!parent)
        {
            WARNING("Undefined parent class: '%s'", main->parentName.c_str());
            return  std::make_shared<Literal>();
        }
    }

    std::shared_ptr<ClassLiteral> s = std::make_shared<ClassLiteral>(); 
    s->name        = node->name.lexeme;
    
 


    if (main->isChild)
    {       
        ClassLiteral *p = static_cast<ClassLiteral *>(parent.get());
        s->environment= new Environment(p->environment);
    } else 
    {
        s->environment= new Environment(environment);
        
    }

    s->environment->copy(main->environment);

  //  s->environment->define("self", s);





   // std::shared_ptr<SelfExpr> self = std::make_shared<SelfExpr>();
   // self->parent = s;

 //   s->environment->define("self", s);

    if (main->isChild)
    {       
        s->environment->define("super", parent);
    } 


    if (!node->args.empty())
    {   

         if (s->environment->contains("init"))
         {
            ExprPtr value = s->environment->get("init");
            if (value->type == ExprType::L_FUNCTION)
            {

                instance= s;    

                prefEnv = s->environment;
                std::shared_ptr<CallExpr> call = std::make_shared<CallExpr>();
                call->name = node->name;
                call->callee = value;
                call->args = std::move(node->args);
                try
                {
                     visit_call_function_member(call.get(), value.get(), s.get());
                }
                catch(const std::exception& e)
                {
                    ERROR("Fail call init ");
                    
                }
                
                instance= nullptr;
                   
                prefEnv = nullptr;

            } else 
            {
                ERROR("Function constructor init not found in class");
            }
        }

    }
    return s;
}

ExprPtr Compiler::visit_call(CallExpr *node)
{

    if (!node)   return  std::make_shared<Literal>();
          




    ExprPtr callee = evaluate(node->callee);


    ExprPtr var = environment->get(node->name.lexeme);
    if (var->type == ExprType::L_STRUCT)
    {
        return visit_call_struct(var,node, callee.get());
    }

    if (var->type == ExprType::L_NATIVE)
    {
        return visit_call_native(node);
    }
    if (var->type == ExprType::L_CLASS)
    {
        return visit_call_class(var,node, callee.get());
    }

 

    if (var->type != ExprType::L_FUNCTION)
        return callee;

    return visit_call_function(node, callee.get());
} 

std::string toLower(const std::string &str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

ExprPtr Compiler::ProcessString(Expr *var, GetDefinitionExpr *node)
{

    StringLiteral *estring = static_cast<StringLiteral *>(var);
    std::string action = node->name.lexeme;
    if (action == "length")
    {
        std::shared_ptr<NumberLiteral> number = std::make_shared<NumberLiteral>();
        number->value = static_cast<double>(estring->value.length());
        return number;
    } else if (action == "asInt") 
    {
        if (node->values.size() < 1)
        {
            throw FatalException("String 'asInt' requires an argument");
        }

        ExprPtr value = evaluate(node->values[0]);
        if (value->type != ExprType::L_NUMBER)
        {
            throw FatalException("String 'asInt' requires a number argument");
        }
        NumberLiteral *number = static_cast<NumberLiteral *>(value.get());
        std::shared_ptr<StringLiteral> result = std::make_shared<StringLiteral>();
        long numberValue = static_cast<long>(number->value);
        result->value = std::to_string(numberValue);
        return result;
    }else 
    {
        
         throw FatalException("Unknown string function '" + action + "'");
    }
    return std::make_shared<Literal>();
}

ExprPtr Compiler::ProcessArray(Expr *var, GetDefinitionExpr *node)
{
        ArrayLiteral *earray = static_cast<ArrayLiteral *>(var);
        
        ExprPtr expr    = environment->get(earray->name);
        if (!expr)  
        {
           
           throw FatalException("Array not found: " + earray->name);
           
        }
        ArrayLiteral *array = static_cast<ArrayLiteral *>(expr.get());
        std::string action = toLower(node->name.lexeme);


        if (action == "push")
        {
                if (node->values.size() < 1)
                {
                    throw FatalException("Array 'push' requires 1 or more argument");
                }
                for (u32 i = 0; i < node->values.size(); i++)
                {
                    ExprPtr value = evaluate(node->values[i]);
                    ExprPtr clone = value->clone();
                    array->values.push_back(std::move(clone));
                }
                return expr;
  
        } else if (action == "pop")
        {
                ExprPtr value = std::move(array->values.back());
                array->values.pop_back();
                return value;
        } else if (action == "size")
        {
            std::shared_ptr<NumberLiteral> size = std::make_shared<NumberLiteral>();
            size->value = array->values.size();
            return size;
        }
        else if (action == "at")
        {
            if (node->values.size() != 1)
            {
                ERROR("Array 'at' requires 1 argument");
                return expr;
            }
            ExprPtr value = evaluate(node->values[0]);
            if (value->type != ExprType::L_NUMBER)
            {
                ERROR("Array index must be a number");
                return expr;
            }
            NumberLiteral *nl = static_cast<NumberLiteral *>(value.get());
            if (nl->value < 0 || nl->value >= array->values.size())
            {
                ERROR("Array index out of bounds");
                return expr;
            }
            u32 index = (u32) static_cast<int>(nl->value);
            return array->values[index];
        }else  if (action == "set")
        {
            if (node->values.size() != 2)
            {
                throw FatalException("Array 'set' requires 2 arguments");
            }
            ExprPtr value = evaluate(node->values[0]);
            if (value->type != ExprType::L_NUMBER)
            {
                throw FatalException("Array index must be a number");
            }
            NumberLiteral *nl = static_cast<NumberLiteral *>(value.get());
            if (nl->value < 0 || nl->value >= array->values.size())
            {
                throw FatalException("Array index out of bounds");
            }

            int index = nl->value;

            array->values[index] = std::move(evaluate(node->values[1]));

            array->values[index];
        }
        else if (action == "last")
        {
            return array->values.back();
        }
        else if (action == "remove")
        {
            if (node->values.size() != 1)
            {
                
                throw FatalException("Array 'remove' requires 1 argument");
            }
            ExprPtr value = evaluate(node->values[0]);
            if (value->type != ExprType::L_NUMBER)
            {
                throw FatalException("Array index must be a number");
            }
            NumberLiteral *nl = static_cast<NumberLiteral *>(value.get());
            if (nl->value < 0 || nl->value >= array->values.size())
            {

                throw FatalException("Array index out of bounds");
            }

            int index = nl->value;
            auto v =    array->values.erase(array->values.begin() + index);
            ExprPtr item = *v;
            return item;
            
        } else if (action == "clear")
        {
            
            array->values.clear();
            
            return expr;
        }
        else if (action == "foreach")
        {
                if (node->values.size() < 1)
                {
                    throw FatalException("Array 'foreach' requires 1 function argument");
                }
                ExprPtr value = evaluate(node->values[0]);
                if (value->type != ExprType::L_FUNCTION)
                {
                    throw FatalException("Array 'foreach' requires 1 function argument");
                }
                std::vector<std::shared_ptr<CallExpr>> calls;     
                for (u32 i = 0; i < array->values.size(); i++)
                {
                      std::shared_ptr<CallExpr> call = std::make_shared<CallExpr>();
                      calls.push_back(call);
                      call->name = node->name;
                      call->callee = value;
                      call->args.push_back(array->values[i]);
                      visit_call_function(call.get(), value.get());
                }
                calls.clear();
            return expr;
        }
        else 
        {
             
             throw FatalException("Unknown array function: "+ node->name.lexeme);
        }
    
    return expr;
}

ExprPtr Compiler::ProcessMap(Expr *var, GetDefinitionExpr *node)
{
        MapLiteral *emap = static_cast<MapLiteral *>(var);
 
        ExprPtr expr    = environment->get(emap->name);
        if (!expr)  
        {
           
           throw FatalException("Dictionary " +emap->name+ " not found");
        }

        MapLiteral *map = static_cast<MapLiteral *>(expr.get());
        std::string action = toLower(node->name.lexeme);


        if (action == "erase")
        {
            if (node->values.size() != 1)
            {
                
                throw FatalException("Dictionary 'erase' requires 1 arguments");
                
            }
            ExprPtr find = evaluate(node->values[0]);
            if (find->type==ExprType::L_STRING)
            {
                StringLiteral *sl = static_cast<StringLiteral *>(find.get());
                    auto it = map->values.begin();
                    for (; it != map->values.end(); it++)
                    {
                        ExprPtr first = it->first;
                        if (first->type == ExprType::L_STRING)
                        {
                            StringLiteral *key = static_cast<StringLiteral *>(first.get());
                            if (key->value == sl->value)
                            {
                                ExprPtr value = it->second;
                                map->values.erase(it);
                                return value;
                            }
                        }
                    }
                
                WARNING("Key not found: %s", sl->value.c_str());

            } else if (find->type == ExprType::L_NUMBER)
            {
                NumberLiteral *nl = static_cast<NumberLiteral *>(find.get());

                auto it = map->values.begin();
                for (; it != map->values.end(); it++)   
                {
                    ExprPtr first = it->first;
                    if (first->type == ExprType::L_NUMBER)
                    {
                        NumberLiteral *key = static_cast<NumberLiteral *>(first.get());
                        if (key->value == nl->value)
                        {
                            ExprPtr value = it->second;
                            map->values.erase(it);
                            return value;
                        }
                    }
                }
                WARNING("Key not found: %f", nl->value);
            }
           

            
            return  std::make_shared<Literal>();
        } else if (action == "size")
        {
            std::shared_ptr<NumberLiteral> result = std::make_shared<NumberLiteral>();
            result->value = map->values.size();
            return result;
        } else if (action == "set")
        {
            if (node->values.size() != 2)
            {
                
                
                throw FatalException("Dictionary 'set' requires 2 arguments");
            }
            ExprPtr key   = evaluate(node->values[0]);
            ExprPtr value = evaluate(node->values[1]);
            ExprPtr remove = map->values[key];
            map->values[key] = value->clone();
            return value;
        }
        else if (action == "find")
        {
            if (node->values.size() != 1)
            {
                
                throw FatalException("Dictionary 'find' requires 1 arguments");
            }
            ExprPtr find   = evaluate(node->values[0]);


            if (find->type==ExprType::L_STRING)
            {
                StringLiteral *sl = static_cast<StringLiteral *>(find.get(  ));
                    auto it = map->values.begin();
                    for (; it != map->values.end(); it++)
                    {
                        ExprPtr first = it->first;
                        if (first->type == ExprType::L_STRING)
                        {
                            StringLiteral *key = static_cast<StringLiteral *>(first.get());
                            if (key->value == sl->value)
                            {
                                ExprPtr value = it->second;
                                return value;
                            }
                        }
                    }
                
                WARNING("Key not found: %s", sl->value.c_str());
            } else if (find->type == ExprType::L_NUMBER)
            {
                NumberLiteral *nl = static_cast<NumberLiteral *>(find.get());

                auto it = map->values.begin();
                for (; it != map->values.end(); it++)   
                {
                    ExprPtr first = it->first;
                    if (first->type == ExprType::L_NUMBER)
                    {
                        NumberLiteral *key = static_cast<NumberLiteral *>(first.get());
                        if (key->value == nl->value)
                        {
                            ExprPtr value = it->second;
                            return value;
                        }
                    }
                }
                WARNING("Key not found: %f", nl->value);
            }

            return  std::make_shared<Literal>();
        }
        else if (action == "clear")
        {
            map->values.clear();
            return std::make_shared<Literal>();
        }
        else if (action == "foreach")
        {
                if (node->values.size() < 1)
                {
                    throw FatalException("Dictionary 'foreach' requires 1 function argument");
                }
                ExprPtr value = evaluate(node->values[0]);
                if (value->type != ExprType::L_FUNCTION)
                {
                    throw FatalException("Dictionary 'foreach' requires 1 function argument");
                }
                
              
               std::vector<ExprPtr> calls;
                auto it = map->values.begin();
            for (; it != map->values.end(); it++)
            {
                ExprPtr key   = it->first;
                ExprPtr v = it->second;

              
                std::shared_ptr<CallExpr>call = std::shared_ptr<CallExpr>();
                calls.push_back(call);
                call->name = node->name;
                call->callee = value;
                call->args.push_back(key);
                call->args.push_back(v);
                visit_call_function(call.get(), value.get());
                    
            }
            calls.clear();
            return std::make_shared<Literal>();
        }
        else 
        {
             
              throw FatalException("Unknown dictionary function: " + node->name.lexeme);
        }
    
    
    return std::make_shared<Literal>();
}


ExprPtr Compiler::visit_call_function_member(CallExpr *node,Expr *callee, ClassLiteral *main) 
{
    Function *function = static_cast<Function *>(callee);
    if (function->arity != node->args.size())
    {
        throw FatalException("Incorrect number of arguments in call to '" + node->name.lexeme +"' at line "+ std::to_string(node->name.line )+ " expected " + std::to_string(function->arity) + " but got " + std::to_string(node->args.size()));
    }


    std::shared_ptr<Environment>  local = std::make_shared<Environment>(main->environment);
    local->define("self", instance);
    
    for (u32 i = 0; i < node->args.size(); i++)
    {
        ExprPtr arg = evaluate(node->args[i]);
        if (!local->define(function->args[i], std::move(arg)))
        {
            throw FatalException("Duplicate identifier from argumnts");
        }
    }

    ExprPtr result = nullptr;
    try  
    {
        BlockStmt *body = static_cast<BlockStmt *>(function->body.get());
        execte_block(body, local.get());
    }
    catch (const ReturnException &e)
    {
        result = e.value;
    }  
    catch (const std::exception &e)
    {
        throw e;
    }



    if (result == nullptr)
    {
        result =  std::make_shared<Literal>();
    }
     return result;
}

ExprPtr Compiler::ProcessClass(Expr *var, GetDefinitionExpr *node)//call_function member
{
        ClassLiteral *classl = static_cast<ClassLiteral *>(var);
        if (!classl)
        {
            ERROR("Class '%s' not found: " ,node->name.lexeme.c_str());
            return  std::make_shared<Literal>();
        }

        std::string action = node->name.lexeme;

     //   INFO("Get Class: %s function %s", classl->name.c_str(), action.c_str());


        ExprPtr value = classl->environment->get(action);
        if (!value)  
        {
           ERROR("Function '%s' not found in class" ,action.c_str());
           return  std::make_shared<Literal>();
        }
        if (value->type == ExprType::L_FUNCTION)
        {
            ExprPtr result = nullptr;
            prefEnv = classl->environment;
            std::shared_ptr<CallExpr> call = std::make_shared<CallExpr>();
            call->name = node->name;
            call->callee = value;
            call->args = node->values;

          

            try
            {
                result = visit_call_function_member(call.get(), value.get(), classl);
            }
            catch (const std::exception &e)
            {
                ERROR("Fail  to execute '%s' function", action.c_str());
                return  std::make_shared<Literal>();
            }


            instance= nullptr;
            prefEnv = nullptr;
            return result;
        } else 
        {
            ERROR("Function '%s' not found in class" ,action.c_str());
        }


        return  std::make_shared<Literal>();
}
    

ExprPtr Compiler::visit_get_definition(GetDefinitionExpr *node)
{

    //  INFO("GET Built int defenition: %s ", node->name.lexeme.c_str());
 

    ExprPtr var     = evaluate(node->variable);    

    if (var->type == ExprType::L_ARRAY)
    {
       return ProcessArray(var.get(), node);
    } else if (var->type == ExprType::L_MAP)
    {
        return ProcessMap(var.get(), node);
    } else if (var->type == ExprType::L_CLASS)
    {


        return ProcessClass(var.get(), node);
    } else if (var->type == ExprType::L_STRING)
    {
        return ProcessString(var.get(), node);
    } else if (var->type == ExprType::L_NUMBER)
    {
     //   return ProcessNumber(var.get(), node);
    }


    
    return var;
}

ExprPtr Compiler::visit_get(GetExpr *node)
{
 //   INFO("GET arg: %s  ", node->name.lexeme.c_str());

    
    ExprPtr object = evaluate(node->object);
    if (object->type==ExprType::L_STRUCT)
    {
        StructLiteral *sl = static_cast<StructLiteral *>(object.get());
        if (sl->members.find(node->name.lexeme) != sl->members.end())
        {
            ExprPtr value = sl->members[node->name.lexeme];
            return value;
        } else 
        {
            ERROR("Member not found: %s", node->name.lexeme.c_str());
            return  std::make_shared<Literal>();
        }
    } else if (object->type == ExprType::L_ARRAY)
    {
        WARNING("TODO Array GET: %s", node->name.lexeme.c_str());
    } else if (object->type == ExprType::L_MAP)
    {
        WARNING("TODO Map GET: %s", node->name.lexeme.c_str());
        
    } else if (object->type == ExprType::L_CLASS)
    {
        std::string action = node->name.lexeme;
        ClassLiteral *cl = static_cast<ClassLiteral *>(object.get());
        ExprPtr key = cl->environment->get(action);
        if (key)
        {
            return key;
           
        }   else 
        {
            WARNING("Class member not found: %s", action.c_str());
            return  std::make_shared<Literal>();
        }
    } else if (object->type == ExprType::L_STRING)
    {
        INFO("TODO String GET: %s", node->name.lexeme.c_str());
        
    } else if (object->type == ExprType::L_NUMBER)
    {
        INFO("TODO Number GET: %s", node->name.lexeme.c_str());   
    }
    return  object;
}

ExprPtr Compiler::visit_self(SelfExpr *node)
{
    if (instance==nullptr)
    {
        ERROR("Self must be call from a class");
        return  std::make_shared<Literal>();
    }
    return instance;
}

ExprPtr Compiler::visit_super(SuperExpr *node)
{

    if (instance==nullptr)
    {
        ERROR("Super must be call from a child class");
        return  std::make_shared<Literal>();
    }
    if(!instance->isChild)
    {
        ERROR("Super must be call from a child class");
        return  std::make_shared<Literal>();
    }
    return instance;
}

ExprPtr Compiler::visit_set(SetExpr *node)
{

    
    ExprPtr object = evaluate(node->object);
    if (object->type==ExprType::L_STRUCT)
    {
        StructLiteral *sl = static_cast<StructLiteral *>(object.get());

       // INFO("SET arg: %s", node->name.lexeme.c_str());

        if (sl->members.find(node->name.lexeme) != sl->members.end())
        {
            ExprPtr value = evaluate(node->value);
            sl->members[node->name.lexeme] = std::move(value);
        }




    } else if (object->type == ExprType::L_ARRAY)
    {
        WARNING("TODO Array SET: %s", node->name.lexeme.c_str());
    } else if (object->type == ExprType::L_MAP)
    {
        WARNING("TODO Map SET: %s", node->name.lexeme.c_str());
    } else if (object->type == ExprType::L_CLASS)
    {
        std::string action = node->name.lexeme;
       // WARNING("TODO Class SET: %s", action.c_str());
        
        ClassLiteral *cl = static_cast<ClassLiteral *>(object.get());
        ExprPtr key = cl->environment->get(action);

        if (key)
        {
               ExprPtr value = evaluate(node->value);
               cl->environment->set(action, std::move(value));
           
        }   else 
        {
            WARNING("Class member not found: %s", action.c_str());
        }


    } else 
    {
        ERROR("SET not implemented for %s", object->toString().c_str());
    }
    return object;
}

ExprPtr Compiler::visit_now_expression(NowExpr *node)
{
    std::shared_ptr<NumberLiteral> result = std::make_shared<NumberLiteral>();
    
    result->value = time_now();
    return result;
}



u8 Compiler::execute(Stmt *stmt)
{
    if (!stmt)
    {
        return 0;
    }
    return stmt->visit(*this);
}



void Compiler::clear()
{

}

u8 Compiler::execte_block(BlockStmt *node, Environment *env)
{
    u8 result = 0;
    auto previousEnvironment = environment;
    environment = env;
    try
    {
        for (auto &s : node->statements)
        {
            result |=  execute(s.get());
        }
    } 
    catch (const FatalException &e)
    {
        environment = previousEnvironment;
        throw e;
    }

    environment = previousEnvironment;
    return result;
}

u8 Compiler::visit_block_smt(BlockStmt *node)
{
    if (!node) return  0;

    Environment * prev = environment;
 
    std::shared_ptr<Environment> env = std::make_shared<Environment>(environment);
    

   
    u8 result = execte_block(node, env.get());

    
    environment = prev;

    return result;
}

u8 Compiler::visit_expression_smt(ExpressionStmt *node)
{
    if (node->expression == nullptr)
    {
        throw FatalException("[EXPRESSION STATEMENT] Unknown expression type");
    }
    evaluate(node->expression);

    return 0;
}



u8 Compiler::visit_print_smt(PrintStmt *node)
{
    if (!node) return  0;

 

    ExprPtr result = evaluate(node->expression);
    if (result == nullptr)
    {
        WARNING("[PRINT] Unknown expression type");
        return 0;
    }





    if (result->type == ExprType::L_NUMBER)
    {
        NumberLiteral *nl = static_cast<NumberLiteral *>(result.get());
        nl->print();
      //  printf("Variable: %p print\n", (void*)&node->expression);
         
    }
    else if (result->type == ExprType::L_STRING)
    {
        StringLiteral *sl = static_cast<StringLiteral *>(result.get());
        sl->print();
    } else if (result->type == ExprType::LITERAL)
    {
        PRINT("nil");
    } else if (result->type == ExprType::L_STRUCT)
    {
        StructLiteral *sl = static_cast<StructLiteral *>(result.get());
        sl->print();

    } else if (result->type == ExprType::L_ARRAY)
    {
        ArrayLiteral *al = static_cast<ArrayLiteral *>(result.get());
        al->print();
    } else if (result->type == ExprType::L_MAP)
    {
        MapLiteral *ml = static_cast<MapLiteral *>(result.get());
        ml->print();
    } else if (result->type == ExprType::L_CLASS)
    {
        ClassLiteral *cl = static_cast<ClassLiteral *>(result.get());
        cl->print();
    }
    
    else 
    {
       WARNING("[PRINT] Unknown literal type %s", result->toString().c_str());
    }
   
  return 0;
}


ExprPtr Compiler::visit_read_variable(Variable *node)
{


    ExprPtr result = environment->get(node->name.lexeme);
    if (result == nullptr)
    {
        if (prefEnv != nullptr)
        {
            result= prefEnv->get(node->name.lexeme);
            if (result != nullptr) return result;
        } 
    

        throw FatalException("Undefined variable: '" + node->name.lexeme +"' at line "+ std::to_string(node->name.line ));
    }
    // if (result->type == ExprType::LITERAL)
    // {
    //   WARNING("Variable: %s is not initialized", node->name.lexeme.c_str());
    // }


  //  printf("read Variable: %x get\n", &result);

    return result;
}

u8 Compiler::visit_declaration(Declaration *node)
{

     
   
       


        Token name = node->names[0];

        //INFO("Variable: %s", name.lexeme.c_str());


        ExprPtr  value = evaluate(node->initializer);
        if (node->names.size() == 1)
        {
            if (!environment->define(name.lexeme, value))
            {
               // WARNING("Variable already defined: %s at line %d", name.lexeme.c_str() ,name.line );
            }
        } else
        {
            for (u32 i = 1; 0 < node->names.size(); i++)
            {
                Token name = node->names[i];
                if (environment->define(node->names[i].lexeme, value))
                {
                    WARNING("Variable already defined: %s at line %d", name.lexeme.c_str() ,name.line );
                }
            }

        }
        

   // environment->print();

    return 0;
}

static bool is_truthy(ExprPtr expr)
{
    if (expr == nullptr)
    {
        return false;
    }
    if (expr->type == ExprType::L_NUMBER)
    {
        NumberLiteral *nl = static_cast<NumberLiteral *>(expr.get());
        return nl->value != 0;
    }
    return true;
}

static bool is_equal(ExprPtr a, ExprPtr b)
{
    if (a == nullptr && b == nullptr) return true;
    if (a->type != b->type) return false;
    if (a->type == ExprType::L_NUMBER && b->type == ExprType::L_NUMBER)
    {
        NumberLiteral *nl = static_cast<NumberLiteral *>(a.get());
        NumberLiteral *nl2 = static_cast<NumberLiteral *>(b.get());
        return nl->value == nl2->value;
    }
    if (a->type == ExprType::L_STRING && b->type == ExprType::L_STRING)
    {
        StringLiteral *sl = static_cast<StringLiteral *>(a.get());
        StringLiteral *sl2 = static_cast<StringLiteral *>(b.get());
        return sl->value == sl2->value;
    }
    return true;
}

u8 Compiler::visit_if(IFStmt *node)
{

       auto previousEnvironment = environment;

    //INFO("Visit if: %s", node->condition->toString().c_str());
    ExprPtr condition = evaluate(node->condition);
    u8 result = 0;

    
    if (is_truthy(condition))
    {
        result = execute(node->then_branch.get());
        
    }
    
    for (auto elif : node->elifBranch)
    {
        condition = evaluate(elif->condition);
        if (is_truthy(condition))
        {
            result =  execute(elif->then_branch.get());
            break;
        }
    }
    
    if (node->else_branch != nullptr)
    {
        result = execute(node->else_branch.get());
    }


    environment = previousEnvironment;


    return result;

}

u8 Compiler::visit_while(WhileStmt *node)
{
    auto previousEnvironment = environment;

    loop_count++;

    ExprPtr condition =nullptr;
    
    while (true)
    {

        
            condition = evaluate(node->condition);
           
            if (!is_truthy(condition))
            {
                break;
            }
           


        try  
        {

        
                execute(node->body.get());

            
        }
        catch (const BreakException &e)
        {
            break;
        }

        catch (const ContinueException &e)
        {
        }
      
    }

    loop_count--;

    environment = previousEnvironment;
   
  
    return 0;
}


u8 Compiler::visit_do(DoStmt *node)
{
    loop_count++;

    do 
    {
        try 
        {
            execute(node->body.get());
        }
        catch (const BreakException &e)
        {
            break;
        }
        catch (const ContinueException &e)
        {
        }
    } while (is_truthy(evaluate(node->condition)));
    loop_count--;



    return 0;
}


u8 Compiler::visit_program(Program *node)
{
    auto previousEnvironment = environment;

    for (auto &s : node->statements)
    {
        execute(s.get());
    }


    environment = previousEnvironment;
    clear();
    return 0;
}

u8 Compiler::visit_function(FunctionStmt *node)
{
    std::shared_ptr<Function> function = std::make_shared<Function>();

    function->name = node->name;
    function->arity = node->args.size();

    for (u32 i = 0; i < node->args.size(); i++)
    {
        function->args[i]=std::move(node->args[i]);
    }
    function->body = std::move(node->body);
    

    environment->define(function->name.lexeme,function);


    return 0;
}

u8 Compiler::visit_struct(StructStmt *node)
{

    if (environment->contains(node->name.lexeme))
    {
        throw FatalException("Struct '" + node->name.lexeme + "' already defined .");
    }
    
   
    std::shared_ptr<StructLiteral> sl = std::make_shared<StructLiteral>();
    sl->name = node->name.lexeme;
    Environment *local = new Environment(environment);
    auto previousEnvironment = environment;
    environment = local;

     u32 index = node->values.size() - 1;
     for (u32 i = 0; i < node->values.size(); i++)
     {
         execute(node->values[index].get());
         index--;
    }

    std::unordered_map<std::string, ExprPtr> values = environment->values();
    sl->members = std::move(values);


    environment = previousEnvironment;
 
    environment->define(node->name.lexeme, std::move(sl));
    delete local;

  


    return 0;
}

u8 Compiler::visit_class(ClassStmt *node)
{
   // INFO("DECLARE class: %s", node->name.lexeme.c_str());



    
    auto previousEnvironment = environment;

    if (environment->contains(node->name.lexeme))
    {
        throw FatalException("Class '" + node->name.lexeme + "' already defined .");
    }


    
    std::shared_ptr<ClassLiteral> cl = std::make_shared<ClassLiteral>();
    cl->environment= new Environment(environment);
    
    cl->name = node->name.lexeme;

    environment = cl->environment;

     if (node->superClass != nullptr)
     {
         cl->isChild = true;
         Variable *superName = static_cast<Variable *>(node->superClass.get());
         cl->parentName = superName->name.lexeme;
        
    } else 
    {
        cl->isChild = false;
        cl->parentName = "";
    }

    try
    {


        for (u32 i = 0; i < node->fields.size(); i++)
        {
            execute(node->fields[i].get());
        }
        for (u32 i = 0; i < node->methods.size(); i++)
        {
            execute(node->methods[i].get());
        }
        
    }
    
    catch (const FatalException &e)
    {
        environment = previousEnvironment;
        ERROR("Error: %s", e.what());
        return 0;
    }

    
   // cl->environment->copy(std::move(local->values()));




    environment = previousEnvironment;
    environment->define(node->name.lexeme, cl);

   


    return 0;
}

u8 Compiler::visit_array(ArrayStmt *node)
{
  //  INFO("Visit array: %s", node->name.lexeme.c_str());
    
    std::shared_ptr<ArrayLiteral> al = std::make_shared<ArrayLiteral>();
    al->name = node->name.lexeme;
    if (environment->define(node->name.lexeme, std::move(al)))
    {
        for (u32 i = 0; i < node->values.size(); i++)
        {
            ExprPtr expr = evaluate(node->values[i]);
            al->values.push_back(std::move(expr));
        }
    }
    return 0;
}

u8 Compiler::visit_map(MapStmt *node)
{
  //  INFO("Visit map: %s", node->name.lexeme.c_str());
    std::shared_ptr<MapLiteral> ml = std::make_shared<MapLiteral>();
    
    ml->name = node->name.lexeme;
   

    if (environment->define(node->name.lexeme, std::move(ml)))
    {
        auto it = node->values.begin();
        for (; it != node->values.end(); it++)
        {
            ExprPtr key  = evaluate(it->first);
            if (key->type != ExprType::L_STRING && key->type != ExprType::L_NUMBER)
            {
                ERROR("Map key must be a string or number.");
                return 0;
            }

            ExprPtr expr = evaluate(it->second);
            ml->values[key] = std::move(expr);
        }
    }
    return 0;
}

u8 Compiler::visit_for(ForStmt *node)
{

   
    std::shared_ptr<Environment> envInit = std::make_shared<Environment>(environment); 
    auto previousEnvironment = environment;
    environment = envInit.get();


    try
    {
        execute(node->initializer.get());
    } 
    catch (const FatalException &e)
    {
        throw e;
    }




    loop_count++;
    ExprPtr condition = nullptr;
    while (true)
    {
        
            
            std::shared_ptr<Environment> local = std::make_shared<Environment>(envInit.get());
            environment = local.get();
            condition = evaluate(node->condition);
            if (!is_truthy(condition))
            {
                
                break;
            }

        try 
        {
           BlockStmt *block = static_cast<BlockStmt *>(node->body.get());
           execte_block(block, local.get());
        }
        catch (const BreakException &e)
        {
            break;
        }

        catch (const ContinueException &e)
        {
            continue;
        }
         evaluate(node->increment);
        
      }

    loop_count--;
    environment = previousEnvironment;

    return 0;
}

u8 Compiler::visit_from(FromStmt *node)
{

    auto previousEnvironment = environment;
    loop_count++;
    ExprPtr  array = evaluate(node->array);
    if (!array || array->type != ExprType::L_ARRAY)
    {
        ERROR("Expected array to iterate");
        return 0;
    }
    ArrayLiteral *al = static_cast<ArrayLiteral *>(array.get());
    if (al->values.size() == 0)
    {
        return 0;
    }
    if (node->variable->type != StmtType::DECLARATION)
    {
        ERROR("Expected variable declaration to iterate");
        return 0;
    }


    std::shared_ptr<Environment> envInit = std::make_shared<Environment>(environment); 
    environment = envInit.get();


    
    Declaration *decl = static_cast<Declaration *>(node->variable.get());
    decl->initializer = al->values[0];
    std::string name = decl->names[0].lexeme; 

    execute(node->variable.get());//define the variable in the environment from






    for (u32 i = 0; i < al->values.size(); i++)
    {
      
        std::shared_ptr<Environment> env = std::make_shared<Environment>(envInit.get());
        environment = env.get();
        ExprPtr value = al->values[i];
        env->set(name, value);
      
        
            
        try 
        {
            execute(node->body.get());
        }
        catch (const BreakException &e)
        {
            break;
        }

        catch (const ContinueException &e)
        {
        }
        

      
         
        
      }

    loop_count--;



    environment = previousEnvironment;



    return 0;
}

u8 Compiler::visit_return(ReturnStmt *node)
{
    ExprPtr value = evaluate(node->value);
    throw ReturnException(value);
    return 3;
}

u8 Compiler::visit_break(BreakStmt *node)
{
    if (loop_count == 0)
    {
       WARNING("BREAK outside of loop");
       return 0;
    }
    throw BreakException("BREAK");
    return 1;
}

u8 Compiler::visit_continue(ContinueStmt *node)
{
    if (loop_count == 0)
    {
       WARNING("CONTINUE outside of loop");
       return 0;
    }
   // Factory::as().free_continue(node);
    throw ContinueException("CONTINUE");
    return 2;
}

u8 Compiler::visit_switch(SwitchStmt *stmt)
{
    auto previousEnvironment = environment;

    auto condition = evaluate(stmt->condition);
    if (!condition)
    {
           throw FatalException("invalid switch condition");
    }
 

    for (const auto &caseStmt : stmt->cases)
    {

        auto result = evaluate(caseStmt->condition);
        if (!result)
        {
               throw FatalException("invalid case condition");
        }
      
        if (is_equal(condition, result))
        {
            return execute(caseStmt->body.get());
        }
    }
    if (stmt->defaultBranch != nullptr)
    {
       return  execute(stmt->defaultBranch.get());
    }

    environment = previousEnvironment;

    return 0;
}

Compiler::Compiler(Interpreter *i, Compiler *c)
{
    interpreter = i;
    parent = c;

    global = std::make_shared<Environment>(nullptr);
    global->define("string", std::make_shared<StringLiteral>());
    global->define("number", std::make_shared<NumberLiteral>());
    environment= global.get();
    prefEnv = nullptr;
    instance = nullptr;

}

void Compiler::init()
{
    
}

Compiler::~Compiler()
{
 
    environment = nullptr;
    instance = nullptr;
    prefEnv = nullptr;
   
}



ExprPtr Compiler::visit_empty_expression(EmptyExpr *node)
{
    return std::make_shared<Literal>();
}


ExprPtr Compiler::visit_binary(BinaryExpr *node)
{
    ExprPtr left  = evaluate(node->left);
    if(!left)
    {
        throw FatalException("Invalid binary expression left");
        
    }
    ExprPtr right = evaluate(node->right);
    if (!right)
    {
        throw FatalException("Invalid binary expression right");
    }

    if (left->type == ExprType::LITERAL || right->type == ExprType::LITERAL)
    {
        throw FatalException("Invalid binary expression. '"+ node->op.lexeme +"' Literals are not allowed at line "+ std::to_string(node->op.line));
    }

    switch (node->op.type)
    {
        case TokenType::GREATER:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value > r->value ? 1 : 0;

                return result;
            } 

            break;
        }

        case TokenType::GREATER_EQUAL:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value >= r->value ? 1 : 0;
                return result;
            }

            break;
        }

        case TokenType::LESS:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value < r->value ? 1 : 0;
                return result;
            }

            break;
        }

        case TokenType::LESS_EQUAL:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = (l->value <= r->value) ? 1 : 0;
                
                return result;
            }

            break;
        }
        case TokenType::PLUS:
        {
           if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value + r->value;
                return result;
            } else if (left->type == ExprType::L_STRING && right->type == ExprType::L_STRING)
            {
                StringLiteral *l = static_cast<StringLiteral *>(left.get());
                StringLiteral *r = static_cast<StringLiteral *>(right.get());
                std::shared_ptr<StringLiteral> result =  std::make_shared<StringLiteral>();
                result->value = l->value + r->value;
                return result;
            } else if (left->type == ExprType::L_STRING && right->type == ExprType::L_NUMBER)
            {
                StringLiteral *l = static_cast<StringLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<StringLiteral> result =  std::make_shared<StringLiteral>();
                result->value = l->value + std::to_string(r->value);

                return result;
            } else if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_STRING)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                StringLiteral *r = static_cast<StringLiteral *>(right.get());
                std::shared_ptr<StringLiteral> result =  std::make_shared<StringLiteral>();
                result->value = std::to_string(l->value) + r->value;
                return result;
            }
            break;
        }
        case TokenType::MINUS:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value - r->value;

                return result;
            }
            break;
        }
        case TokenType::SLASH:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                if (r->value == 0)
                {

                    throw FatalException("Division by zero");
                }
                result->value = l->value / r->value;


                return result;
            }
            break;
        }
        case TokenType::STAR:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
               NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();   
                result->value = l->value * r->value;

                return result;
            }
            break;
        }
        case TokenType::MOD:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
               NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value =std::fmod(l->value, r->value);
   
   
                return result;
            }
            break;
        }
        case TokenType::BANG_EQUAL:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
               NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value != r->value ? 1 : 0;
   
                return result;
            } else if (left->type == ExprType::L_STRING && right->type == ExprType::L_STRING)
            {
                StringLiteral *l = static_cast<StringLiteral *>(left.get());
                StringLiteral *r = static_cast<StringLiteral *>(right.get());
               std::shared_ptr< NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value != r->value ? 1 : 0;
                return result;
            }
            break;
        }

        case TokenType::EQUAL_EQUAL:
        {
            if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value == r->value ? 1 : 0;

                return result;
            } else if (left->type == ExprType::L_STRING && right->type == ExprType::L_STRING)
            {
                StringLiteral *l = static_cast<StringLiteral *>(left.get());
                StringLiteral *r = static_cast<StringLiteral *>(right.get());
                std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                result->value = l->value == r->value ? 1 : 0;

                return result;
            }
            break;
        }
        case TokenType::PLUS_EQUAL://+=
        {
              if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
               // std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
               // result->value = l->value += r->value;

                l->value += r->value;

                return left;
            }
            break;
        }
        case TokenType::MINUS_EQUAL:// -=
        {
             if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
             
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                //std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();

              //  INFO("l: %f, r: %f", l->value, r->value);

                l->value -= r->value;

                return left;
                

                //result->value = l->value -= r->value;

             //   return result;
            }
            break;
        }
        case TokenType::STAR_EQUAL:// *=
        {
             if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                //std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                //result->value = l->value *= r->value;
                //return result;

                l->value *= r->value;
                return left;
            }
            break;
        }
        case TokenType::SLASH_EQUAL:// /=
        {
             if (left->type == ExprType::L_NUMBER && right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
                NumberLiteral *r = static_cast<NumberLiteral *>(right.get());
                //std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
                //result->value = l->value /= r->value;
                if (r->value == 0)
                {
                     throw FatalException("Division by zero");
                }

                l->value /= r->value;
                return left;

                //return result;
            }
            break;
        }
        default:
            break;
    }

    
    throw FatalException("Invalid binary expression, With operator '"+node->op.lexeme+"'");

    
}

ExprPtr Compiler::visit_unary(UnaryExpr *expr)
{
    ExprPtr right = evaluate(expr->right);
    if (!right)
    { 
        throw FatalException("Unknown expression type for UnaryExpr: "+ expr->toString());
    }
    if ( right->type == ExprType::LITERAL)
    {
        throw FatalException("Invalid unary expression. '"+ expr->op.lexeme +"' Literals are not allowed at line "+ std::to_string(expr->op.line));
    }
    


    switch (expr->op.type)
    {

        case TokenType::MINUS:
        {
            if (right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *num = static_cast<NumberLiteral *>(right.get());   
                double value = num->value;
                num->value = -value;
                return right;
            }
            break;
        }
        case TokenType::BANG:
        {
            if (right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *num = static_cast<NumberLiteral *>(right.get());
                double value = num->value;
                num->value = (value==0) ? 0 : 1;
                return right;
            }
            break;
        }
        case TokenType::INC:
        {
            if (right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *num = static_cast<NumberLiteral *>(right.get());
                if (expr->isPrefix)
                {
                   ++num->value;
                }
                else
                {
                    num->value++;
                }

                return right;
            }
            break;
        }
        case TokenType::DEC:
        {
            if (right->type == ExprType::L_NUMBER)
            {
                NumberLiteral *num = static_cast<NumberLiteral *>(right.get());
                
                if (expr->isPrefix)
                {
                     --num->value;
                }
                else
                {
                    
                    num->value--;
                }
                return right;
            }
            break;
        }
        default:
            break;
       
    
    }
  
    

    throw FatalException("Invalid unary expression, With operator '"+expr->op.lexeme+"'");
   
    return nullptr;
}

ExprPtr Compiler::visit_logical(LogicalExpr *node)
{
    ExprPtr left = evaluate(node->left);
    if (!left) 
    {
        throw FatalException("Unknown expression type for Logical avaliation: "+ node->toString());
    }
    if ( left->type == ExprType::LITERAL)
    {
        throw FatalException("Invalid logical expression. '"+ node->op.lexeme +"' Literals are not allowed at line "+ std::to_string(node->op.line));
    }


    if (left->type == ExprType::L_NUMBER)
    {
        NumberLiteral *l = static_cast<NumberLiteral *>(left.get());
       
        if (node->op.type == TokenType::OR)
        {
            if (l->value != 0)
            {
                return left;
            }
        }else  if (node->op.type == TokenType::AND)
        {
            if (l->value == 0)
            {
                return left;
            }
        } else if (node->op.type == TokenType::XOR)
        {
            if (l->value != 0)
            {
                return left;
            }
        } 
    }
    return evaluate(node->right);
}

ExprPtr Compiler::visit_grouping(GroupingExpr *node)
{
    return evaluate(node->expr);
}

ExprPtr Compiler::visit_literal(Literal *node)
{
    std::shared_ptr<Literal> result =  std::make_shared<Literal>();
    return result;
}
std::shared_ptr<Expr> Compiler::visit_number_literal(NumberLiteral *node)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = node->value;
    return result;
}

std::shared_ptr<Expr> Compiler::visit_string_literal(StringLiteral *node)
{
    std::shared_ptr<StringLiteral> result =  std::make_shared<StringLiteral>();
    result->value = node->value;
    return result;
   
}

Interpreter::~Interpreter()
{
     INFO("Interpreter destroyed");
     nativeFunctions.clear();


    compiler=nullptr;
    context = nullptr;

}   

Interpreter::Interpreter()
{


    currentCompiler = std::make_shared<Compiler>(this);
    currentContext  = std::make_shared<Context>(this);

    compiler = currentCompiler.get();
    context = currentContext.get();
    compiler->init();
}

bool Interpreter::compile(const std::string &source)
{
    Lexer lexer;
    lexer.initialize();
    lexer.Load(source);
    std::vector<Token> tokens =  lexer.GetTokens();
    if (tokens.size() == 0)
    {
        return false;
    }

    // for (auto token : tokens)
    // {
    //     INFO("Token: %s" ,token.toString().c_str());
    // }

    std::shared_ptr<Program> program=nullptr;
    Parser parser;

   
        
        parser.Load(tokens);

        program =  parser.parse();
       
        

      
        if (program == nullptr)
        {
            return false;
        }
        compiler->execute(program.get());
        parser.clear();
 

    
   
    return false;
}

void Interpreter::collect()
{
   
}

void Interpreter::clear()
{
   
    //natives.clear();

  

   

}

void Interpreter::registerFunction(const std::string &name, NativeFunction function)
{
    if (nativeFunctions.find(name) != nativeFunctions.end())
    {
        throw FatalException("Native function already defined: " + name);
    }
    std::shared_ptr<Native> native = std::make_shared<Native>();
    if (!compiler->environment->define(name, native))
    {
           throw FatalException("Native function already defined: " + name);
    }
    nativeFunctions[name] = function;
}

bool Interpreter::registerInteger(const std::string &name, int value)
{
    std::shared_ptr<NumberLiteral> num = std::make_shared<NumberLiteral>();
    num->value = static_cast<double>(value);
    return compiler->environment->define(name, num);
    
}

bool Interpreter::registerBoolean(const std::string &name, bool value)
{
    std::shared_ptr<NumberLiteral> num = std::make_shared<NumberLiteral>();
    num->value = static_cast<double>(value);
    return compiler->environment->define(name, num);
}

bool Interpreter::registerDouble(const std::string &name, double value)
{
    std::shared_ptr<NumberLiteral> num = std::make_shared<NumberLiteral>();
    num->value = value;
    return compiler->environment->define(name, num);
}

bool Interpreter::registerString(const std::string &name, std::string value)
{
    std::shared_ptr<StringLiteral> str = std::make_shared<StringLiteral>();
    str->value = value;
    return compiler->environment->define(name, str);
}

bool Interpreter::isnative(const std::string &name)
{
     return nativeFunctions.find(name) != nativeFunctions.end();
}

ExprPtr Interpreter::CallNativeFunction(const std::string &name, int argc)
{
    

    auto function = nativeFunctions[name];
    
    

    ExprPtr result = function(context, argc);
    
    return result;
}

bool Interpreter::registerGlobal(const std::string &name, ExprPtr value)
{

    return compiler->environment->define(name, value);
}

void Interpreter::Error(const Token &token, const std::string &message)
{
    int line = token.line;
    std::string text =message+ " at line: " +std::to_string(line)+" '"+ token.lexeme+"'";
    Log(2, text.c_str());
    throw FatalException(text);
}
void Interpreter::Error(const std::string &message)
{
    Log(2, message.c_str());
    throw FatalException(message);
}
void Interpreter::Warning(const Token &token,const std::string &message)
{
    int line = token.line;
    std::string text =message+ " at line: " +std::to_string(line);
    Log(1, text.c_str());
}

void Interpreter::Warning(const std::string &message)
{
   Log(1, message.c_str());
}

void Interpreter::Info(const std::string &message)
{
   Log(0, message.c_str());
}



Function::Function():Literal()
{
    type = ExprType::L_FUNCTION;
    body = nullptr;
}

ExprPtr Function::clone()
{
    std::shared_ptr<Function> f = std::make_shared<Function>();
    f->name = name;
    f->body = body;
    return f;
}



ClassLiteral::ClassLiteral()
{
    type = ExprType::L_CLASS;
    name = "";
    parentName = "";
    isChild = false;
   
    environment = nullptr;
}

ClassLiteral::~ClassLiteral()
{
   INFO("Class deleted: %s", name.c_str());
   environment->remove("self");
   delete environment;
   environment = nullptr;
}




std::string BuildClass(ClassLiteral *cl);

void ClassLiteral::print()
{
    std::string data = BuildClass(this);
    INFO("%s",data.c_str());
}

ExprPtr ClassLiteral::clone()
{
    std::shared_ptr<ClassLiteral> cl = std::make_shared<ClassLiteral>();
    cl->name = name;
    
    if (this->environment)
    {
        cl->environment =  new Environment(this->environment->getParent());
        
        const std::unordered_map<std::string, ExprPtr > values = this->environment->values();
        for (auto it = values.begin(); it != values.end(); it++)
        {
          cl->environment->define(it->first, it->second);
        }
    }
    return cl;
}

StructLiteral::StructLiteral()
{
    type = ExprType::L_STRUCT;
    name = "";
}
StructLiteral::~StructLiteral()
{
   // INFO("Struct deleted: %s", name.c_str());
}
std::string BuilArray(ArrayLiteral *al);

std::string BuilMap(MapLiteral *ml)
{
    std::string s;
    auto it = ml->values.begin();
    u32 size = ml->values.size();
    for (u32 i = 0; it != ml->values.end(); it++, i++)
    {
        s +="{";
        ExprPtr key = it->first;
        if (key->type == ExprType::LITERAL)
        {
            s += "nil";
        }else  if (key->type == ExprType::L_NUMBER)
        {
            NumberLiteral *nl = static_cast<NumberLiteral *>(key.get());
            s += std::to_string(nl->value);
        } else if (key->type == ExprType::L_STRING)
        {
            StringLiteral *sl = static_cast<StringLiteral *>(key.get());
            s += sl->value;
        }
        s += ":";
        ExprPtr value = it->second;
        if (value->type == ExprType::L_NUMBER)
        {
            NumberLiteral *nl = static_cast<NumberLiteral *>(value.get());
            s += std::to_string(nl->value);
        } else if (value->type == ExprType::L_STRING)
        {
            StringLiteral *sl = static_cast<StringLiteral *>(value.get());
            s += sl->value;
        } else if (value->type == ExprType::L_ARRAY)
        {
            ArrayLiteral *al = static_cast<ArrayLiteral *>(value.get());
            s += BuilArray(al);
        } else if (value->type == ExprType::L_MAP)
        {
            MapLiteral *ml = static_cast<MapLiteral *>(value.get());
            s += BuilMap(ml);
        } else if (value->type == ExprType::L_CLASS)
        {
            ClassLiteral *cl = static_cast<ClassLiteral *>(value.get());
            s += BuildClass(cl);
        }
        s += "}";
        if (i < size - 1)
        {
            s += ",";
        }

       
    }
    return s;
} 

std::string BuilStruct(StructLiteral *sl)
{
    std::string s=sl->name+ " ";
    auto it = sl->members.begin();
    while (it != sl->members.end())
    {
        std::string name = it->first;
        std::string value;
        ExprPtr expr = it->second;
        if (expr->type == ExprType::LITERAL)
        {
                value ="nil";
        }else if (expr->type == ExprType::L_NUMBER)
        {
            NumberLiteral *nl = static_cast<NumberLiteral *>(expr.get());
            value = std::to_string(nl->value);
        } else if (expr->type == ExprType::L_STRING)
        {
            StringLiteral *sl = static_cast<StringLiteral *>(expr.get());
            value = sl->value;
        } else if (expr->type == ExprType::L_STRUCT)
        {
            StructLiteral *sl = static_cast<StructLiteral *>(expr.get());
            value = BuilStruct(sl);            
        }else if (expr->type == ExprType::L_ARRAY)
        {
            ArrayLiteral *al = static_cast<ArrayLiteral *>(expr.get());
            value = BuilArray(al);
        } else if (expr->type == ExprType::L_MAP)
        {
            MapLiteral *ml = static_cast<MapLiteral *>(expr.get());
            value = BuilMap(ml);
        } else if (expr->type == ExprType::L_CLASS)
        {
            ClassLiteral *cl = static_cast<ClassLiteral *>(expr.get());
            value = BuildClass(cl);
        }
        s  += "("+ name+ ":" + value+")";
        
        it++;
    }    
    return s;
}

std::string BuilArray(ArrayLiteral *al)
{
    std::string s = "[";
    for (u32 i = 0; i < al->values.size(); i++)
    {
        if (i > 0)
        {
            s += ", ";
        }
        if (al->values[i]->type == ExprType::LITERAL)
        {
                s+="nil";
        }
        else if (al->values[i]->type == ExprType::L_NUMBER)
        {
            NumberLiteral *nl = static_cast<NumberLiteral *>(al->values[i].get());
            s +=  std::to_string(nl->value);
        } else if (al->values[i]->type == ExprType::L_STRING)
        {
            StringLiteral *sl = static_cast<StringLiteral *>(al->values[i].get());
            s += sl->value;
        } else if (al->values[i]->type == ExprType::L_STRUCT)
        {
            StructLiteral *sl = static_cast<StructLiteral *>(al->values[i].get());
            s += BuilStruct(sl);
        } else if (al->values[i]->type == ExprType::L_ARRAY)
        {
            ArrayLiteral *al = static_cast<ArrayLiteral *>(al->values[i].get());
            s += BuilArray(al);
        } else if (al->values[i]->type == ExprType::L_MAP)
        {
            MapLiteral *ml = static_cast<MapLiteral *>(al->values[i].get());
            s += BuilMap(ml);
        } else if (al->values[i]->type == ExprType::L_CLASS)
        {
            ClassLiteral *cl = static_cast<ClassLiteral *>(al->values[i].get());
            s += BuildClass(cl);
        }

    }
    s += "]";
    return s;
}

std::string BuildClass(ClassLiteral *cl)
{
    std::string s ="Class :"+ cl->name;
    // auto list  =    cl->environment->values();
    // auto it = list.begin();
    // while (it != list.end())
    // {
    //     std::string name = it->first;
    //     s += "("+ name +")";
    //     // ExprPtr expr = it->second;
    //     // if (expr->type == ExprType::L_STRUCT)
    //     // {
    //     //     StructLiteral *sl = static_cast<StructLiteral *>(expr);
    //     //     s += BuilStruct(sl);
    //     // } else if (expr->type == ExprType::L_ARRAY)
    //     // {
    //     //     ArrayLiteral *al = static_cast<ArrayLiteral *>(expr);
    //     //     s += BuilArray(al);
    //     // } else if (expr->type == ExprType::L_MAP)
    //     // {
    //     //     MapLiteral *ml = static_cast<MapLiteral *>(expr);
    //     //     s += BuilMap(ml);
    //     // }   else 
    //     // if (expr->type == ExprType::L_NUMBER)
    //     // {
    //     //     NumberLiteral *nl = static_cast<NumberLiteral *>(expr);
    //     //     s +=  std::to_string(nl->value) + ")";
    //     // } else if (expr->type == ExprType::L_STRING)
    //     //  {
    //     //      StringLiteral *sl = static_cast<StringLiteral *>(expr);
    //     //      s += sl->value + ")";
    //     //  }
    //      // else if (expr->type == ExprType::L_CLASS)
    //     // {
    //     //     ClassLiteral *cl = static_cast<ClassLiteral *>(expr);
    //     //     s += BuildClass(cl);
    //     // }

    //     it++;
    // }
    return s;
}

void StructLiteral::print()
{
        std::string s=BuilStruct(this);
        PRINT("%s", s.c_str());
}

ExprPtr StructLiteral::clone()
{
    
    std::shared_ptr<StructLiteral> l = std::make_shared<StructLiteral>();

    l->name = name;
    for (auto it = members.begin(); it != members.end(); it++)
    {
        l->members[it->first] = it->second->clone();
    }
    return l;
}

ArrayLiteral::ArrayLiteral()
{
    type = ExprType::L_ARRAY;

}

void ArrayLiteral::print()
{
    std::string str = BuilArray(this);
    PRINT("Array %s", str.c_str());
    
}

ExprPtr ArrayLiteral::clone()
{
    std::shared_ptr<ArrayLiteral> l = std::make_shared<ArrayLiteral>();
    l->name = name;
    for (u32 i = 0; i < values.size(); i++)
    {
        l->values.push_back(values[i]->clone());
    }
    return l;
}

MapLiteral::MapLiteral()
{
    type = ExprType::L_MAP;
}

void MapLiteral::print()
{
    std::string str = BuilMap(this);
    PRINT("Map [%s]", str.c_str());
}

ExprPtr MapLiteral::clone()
{
    std::shared_ptr<MapLiteral> l = std::make_shared<MapLiteral>();
    for (auto it = values.begin(); it != values.end(); it++)
    {
        l->values[it->first] = it->second->clone();
    }
    return l;
}

Native::Native()
{
    type = ExprType::L_NATIVE;
}

Context::Context(Interpreter *interpreter)
{
    this->interpreter = interpreter;
    NIL = std::make_shared<Literal>();
}

Context::~Context()
{
    NIL = nullptr;
    clear();
}

void Context::add(ExprPtr value,Literal *literal)
{
    expressions.push_back(std::move(value));
    literals.push_back(literal);
}

void Context::clear()
{
    values.clear();
    literals.clear();
    expressions.clear();
}

long Context::getLong(u8 index)
{
    NumberLiteral *l = static_cast<NumberLiteral *>(literals[index]);
    return static_cast<long>(l->value);
}

int Context::getInt(u8 index)
{
    NumberLiteral *l = static_cast<NumberLiteral *>(literals[index]);
    return static_cast<int>(l->value);
}

double Context::getDouble(u8 index)
{
    NumberLiteral *l = static_cast<NumberLiteral *>(literals[index]);
    return l->value;
}

float Context::getFloat(u8 index)
{
    NumberLiteral *l = static_cast<NumberLiteral *>(literals[index]);
    return static_cast<float>(l->value);
}

std::string Context::getString(u8 index)
{
    return static_cast<StringLiteral *>(literals[index])->value;
}

bool Context::getBoolean(u8 index)
{
    NumberLiteral *l = static_cast<NumberLiteral *>(literals[index]);
    return l->value != 0;
}
ExprPtr Context::asFloat(float value)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = static_cast<double>(value);
    values.push_back(result);
    return result;
}

ExprPtr Context::asDouble(double value)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = value;
    values.push_back(result);
    return result;
}

ExprPtr Context::asInt(int value)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = static_cast<double>(value);
    values.push_back(result);
    return result;
}

ExprPtr Context::asLong(long value)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = static_cast<double>(value);
    values.push_back(result);
    return result;
}

ExprPtr Context::asString(std::string value)
{
    std::shared_ptr<StringLiteral> result =  std::make_shared<StringLiteral>();
    result->value = value;
    values.push_back(result);
    return result;
}

ExprPtr Context::asBoolean(bool value)
{
    std::shared_ptr<NumberLiteral> result =  std::make_shared<NumberLiteral>();
    result->value = value ? 1 : 0;
    values.push_back(result);
    return result;
}

ExprPtr Context::asNil()
{
    return  NIL;
}

bool Context::isNumber(u8 index)
{
    if (index >= literals.size())
    {
        return false;
    }
    return literals[index]->type == ExprType::L_NUMBER;
}

bool Context::isString(u8 index)
{
    if (index >= literals.size())
    {
        return false;
    }
    return literals[index]->type == ExprType::L_STRING;
}
