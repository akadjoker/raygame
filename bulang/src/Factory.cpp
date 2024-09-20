#include "pch.h"

#include "Interpreter.hpp"
#include "Utils.hpp"


constexpr size_t POOL_NUMBERS = 1024*2;
constexpr size_t POOL_STRINGS = 1024*2;
constexpr size_t THRESHOLD = 1024*2;

Factory &Factory::as()
{
    static Factory Factory;
    return Factory;
}

std::shared_ptr<NumberLiteral> Factory::getNumber()
{

  return std::make_shared<NumberLiteral>();

}

std::shared_ptr<StringLiteral> Factory::getString()
{

    return std::make_shared<StringLiteral>();

    
}

std::shared_ptr<CallExpr> Factory::getCall()
{

    return std::make_shared<CallExpr>();

   
}

std::shared_ptr<GetExpr> Factory::getGet()
{

  return std::make_shared<GetExpr>();

}

std::shared_ptr<Assign> Factory::getAssign()
{
     return std::make_shared<Assign>();
   
}

std::shared_ptr<SetExpr> Factory::getSet()
{

    return std::make_shared<SetExpr>();
  
  
}

std::shared_ptr<Environment> Factory::createEnvironment(Environment *parent )
{

    return std::make_shared<Environment>(parent);

   
}


u32 Factory::size() const
{
    return 0;
}

Factory::Factory()
{
   

}

Factory::~Factory()
{
   
 

}
