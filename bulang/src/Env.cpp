#include "pch.h"

#include "Interpreter.hpp"
#include "Utils.hpp"

//***************************************************************************************** */

static u32 env_depth = 0;

Environment::Environment()
{
    depth = ++env_depth;
    parent = nullptr;
   // INFO("Environment created %d", depth);
}

Environment::Environment(Environment *parent)
    : parent(parent)
{

    depth = ++env_depth;
 //   INFO("Environment created %d", depth);
}

Environment::~Environment()
{
    parent = nullptr;
  //  INFO("Environment destroyed %d", depth);

//    m_values.clear();
    env_depth--;
}

void Environment::print()
{

    for (auto it = m_values.begin(); it != m_values.end(); it++)
    {
        ExprPtr l = it->second;
        if (l != nullptr)
        {
            INFO("%s: %s", it->first.c_str(), l->toString().c_str());
        }
    }
}

bool Environment::define(const std::string &name, ExprPtr value)
{
    if (m_values.find(name) != m_values.end())
    {
        m_values[name] = value;
        return false;
    }
    
    m_values[name] =std::move(value);
    return true;
}

ExprPtr Environment::get(const std::string &name)
{
    if (m_values.find(name) != m_values.end())
    {
        return m_values[name];
    }
    if (parent != nullptr)
    {
        return parent->get(name);
    }
    return nullptr;
}

bool Environment::set(const std::string &name, ExprPtr value)
{
    if (m_values.find(name) != m_values.end())
    {
        ExprPtr expr = m_values[name];
        m_values[name] = value;
        return true;
    }
    if (parent != nullptr)
    {
        return parent->set(name, value);
    }
    return false;
}

bool Environment::contains(const std::string &name)
{
    if (m_values.find(name) != m_values.end())
    {
        return true;
    }
    if (parent != nullptr)
    {
        return parent->contains(name);
    }
    return false;
}

void Environment::remove(const std::string &name)
{
    if (m_values.find(name) != m_values.end())
    {
        m_values.erase(name);
    }
}

bool Environment::assign(const std::string &name, ExprPtr value)
{
    if (value == nullptr)
    {
        ERROR("Cannot assign variable to undefined value: %s", name.c_str());
        return false;
    }

    if (m_values.find(name) != m_values.end())
    {
            ExprPtr expr = m_values[name];
            if (expr == nullptr)
            {
                ERROR("Cannot assign variable to undefined value: %s", name.c_str());
                return false;
            }

            m_values[name] = std::move(value);

            // if (expr->type == ExprType::LITERAL)
            // {
            //     WARNING("Variable: %s is not initialized", name.c_str());
            //     return replace(name, value);
            // } 


            // if (expr->type == ExprType::L_NUMBER && value->type == ExprType::L_NUMBER)
            // {
            //     NumberLiteral *l = static_cast<NumberLiteral *>(expr.get());
            //     NumberLiteral *r = static_cast<NumberLiteral *>(value.get());
            //     l->value = r->value;
            //     return true;
            // } else if (expr->type == ExprType::L_STRING && value->type == ExprType::L_STRING)
            // {
            //     StringLiteral *l = static_cast<StringLiteral *>(expr.get());
            //     StringLiteral *r = static_cast<StringLiteral *>(value.get());
            //     l->value = r->value;
            //     return true;
            // }
            

            // if (expr->type == ExprType::L_NUMBER && value->type == ExprType::L_STRING)
            // {
            //     ERROR("Cannot assign string to number");
            //     return false;
            // } else if (expr->type == ExprType::L_STRING && value->type == ExprType::L_NUMBER)
            // {
            //     ERROR("Cannot assign number to string");
            //     return false;
            // }

            return true;
    }


    if (parent != nullptr)
    {
        return parent->assign(name, value);
    }

 

   
    return false;
}

bool Environment::replace(const std::string &name, ExprPtr value)
{
    if (m_values.find(name) != m_values.end())
    {
        m_values[name] = value;
        return true;
    }
    if (parent != nullptr)
    {
        return parent->replace(name, value);
    }
    return false;
}

bool Environment::addInteger(const std::string &name, int value)
{
    std::shared_ptr<NumberLiteral> nl =  std::make_shared<NumberLiteral>();
    nl->value = value;
    return define(name, nl);
}

bool Environment::addDouble(const std::string &name, double value)
{
    std::shared_ptr<NumberLiteral> nl =  std::make_shared<NumberLiteral>();
    nl->value = value;
    return define(name, nl);
}

bool Environment::addString(const std::string &name, std::string value)
{
    std::shared_ptr<StringLiteral> sl =  std::make_shared<StringLiteral>();
    sl->value = value;
    return define(name, sl);
}

bool Environment::addBoolean(const std::string &name, bool value)
{
    std::shared_ptr<NumberLiteral> bl =  std::make_shared<NumberLiteral>();
    bl->value = value ? 1 : 0;
    return define(name, bl);
}
bool Environment::copy(std::unordered_map<std::string, ExprPtr> values)
{
    this->m_values=std::move(values);
    return true;
}
bool Environment::copy(Environment *env)
{
    if (env == nullptr)
        return false;
    this->m_values = env->m_values;
    return true;
    
}
std::shared_ptr<Environment> Environment::clone()
{
    std::shared_ptr<Environment>  env = std::make_shared<Environment>(parent);
    for (auto it = m_values.begin(); it != m_values.end(); it++)
    {
        if (it->second->type == ExprType::L_FUNCTION)
            env->define(it->first, it->second);
        else 
            env->define(it->first, it->second->clone());
    }

    return env;
}
