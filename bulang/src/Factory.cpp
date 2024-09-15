#include "pch.h"

#include "Interpreter.hpp"
#include "Utils.hpp"


constexpr size_t POOL_NUMBERS = 1024*2;
constexpr size_t POOL_STRINGS = 1024*2;

Factory &Factory::as()
{
    static Factory Factory;
    return Factory;
}

std::shared_ptr<NumberLiteral> Factory::getNumber()
{

    // std::shared_ptr<NumberLiteral> result = std::make_shared<NumberLiteral>();
    // return result;

    std::shared_ptr<NumberLiteral> result (newNumber(),
                                         [this](NumberLiteral* num)
                                         {
                                            Factory::as().recycle(num);
                                         });

    //   return std::make_shared<NumberLiteral>();

    // if (numbers.empty() && numbers.size() < POOL_NUMBERS)
    // {

    //     std::shared_ptr<NumberLiteral> result (newNumber(),
    //                                              [this](NumberLiteral* num) 
    //                                              {
    //                                                  recycle(num); 
    //                                              });

    //     return result;
    // }
    // std::shared_ptr<NumberLiteral> result = numbers.back();
    // numbers.pop_back();
     return result;
}

std::shared_ptr<StringLiteral> Factory::getString()
{

    // std::shared_ptr<StringLiteral> result = std::make_shared<StringLiteral>();
    // return result;


         std::shared_ptr<StringLiteral> result(newString(),
                                                 [this](StringLiteral* num) 
                                                 {
                                                     Factory::as().recycle(num);
                                                 });
        return result;
    
}

std::shared_ptr<CallExpr> Factory::getCall()
{

    std::shared_ptr<CallExpr> result(newCall(),
                                                 [this](CallExpr* num) 
                                                 {
                                                     Factory::as().freeCall(num);
                                                 });
    return result;
}

std::shared_ptr<GetExpr> Factory::getGet()
{
    std::shared_ptr<GetExpr> result(newGet(),
                                                 [this](GetExpr* num) 
                                                 {
                                                     Factory::as().freeGet(num);
                                                 });
    return result;
}

std::shared_ptr<Assign> Factory::getAssign()
{
    std::shared_ptr<Assign> result(newAssign(),
                                                 [this](Assign* num) 
                                                 {
                                                     Factory::as().freeAssign(num);
                                                 });
    return result;
}

std::shared_ptr<SetExpr> Factory::getSet()
{
    std::shared_ptr<SetExpr> result(newSet(),
                                                 [this](SetExpr* num) 
                                                 {
                                                     Factory::as().freeSet(num);
                                                 });
    return result;
}



void Factory::recycle(StringLiteral* ptr)
{

    strings.push_back(ptr);


    
    // strings.emplace_back(ptr, [this](StringLiteral* p) 
    // {

    //     Factory::as().freeString(p);
    // });
}

void Factory::recycle(NumberLiteral* number)
{
   numbers.push_back(number);
    // numbers.emplace_back(number, [this](NumberLiteral* p)
    // {
    //       Factory::as().freeNumber(p);
    // });
}

u32 Factory::size() const
{
    return arena.size();
}

Factory::Factory()
{
}

Factory::~Factory()
{
    for (u32 i = 0; i < numbers.size(); i++)
    {
        Factory::as().freeNumber(numbers[i]);
    }
    for (u32 i = 0; i < strings.size(); i++)
    {
        Factory::as().freeString(strings[i]);
    }
 

}

NumberLiteral *Factory::newNumber()
{

    if (numbers.empty() && numbers.size() < POOL_NUMBERS)
    {
        void* ptr = arena.Allocate(sizeof(NumberLiteral));
        return new (ptr) NumberLiteral();
    }
    NumberLiteral *result = numbers.back();
    numbers.pop_back();
    return result;

    
}

StringLiteral *Factory::newString()
{

    if (strings.empty() && strings.size() < POOL_STRINGS)
    {
        void* ptr = arena.Allocate(sizeof(StringLiteral));
        return new (ptr) StringLiteral();
    }
    StringLiteral *result = strings.back();
    strings.pop_back();
    return result;
  
}

CallExpr *Factory::newCall()
{

    void* ptr = arena.Allocate(sizeof(CallExpr));
    return new (ptr) CallExpr();
}

GetExpr *Factory::newGet()
{
    void* ptr = arena.Allocate(sizeof(GetExpr));
    return new (ptr) GetExpr();
}

Assign *Factory::newAssign()
{
    void* ptr = arena.Allocate(sizeof(Assign));
    return new (ptr) Assign();
}

SetExpr *Factory::newSet()
{
    void* ptr = arena.Allocate(sizeof(SetExpr));
    return new (ptr) SetExpr();
}


void Factory::freeCall(CallExpr *call)
{
    call->~CallExpr();
    arena.Free(call, sizeof(CallExpr));
}

void Factory::freeGet(GetExpr *get)
{
    get->~GetExpr();
    arena.Free(get, sizeof(GetExpr));
}

void Factory::freeAssign(Assign *assign)
{
    assign->~Assign();
    arena.Free(assign, sizeof(Assign));
}

void Factory::freeSet(SetExpr *set)
{
    set->~SetExpr();
    arena.Free(set, sizeof(SetExpr));
}



void Factory::freeNumber(NumberLiteral *number)
{
    number->~NumberLiteral();
    arena.Free(number, sizeof(NumberLiteral));
}

void Factory::freeString(StringLiteral *string)
{
    string->~StringLiteral();
    arena.Free(string, sizeof(StringLiteral));
}

Environment *Factory::newEnvironment(Environment *parent)
{
    void* ptr = stack.Allocate(sizeof(Environment)); 
    return new (ptr) Environment(parent);
}

std::shared_ptr<Environment> Factory::createEnvironment(Environment *parent )
{
     std::shared_ptr<Environment> result(newEnvironment(parent),
                                                 [this](Environment *environment) 
                                                 {
                                                     Factory::as().freeEnvironment(environment);
                                                 });

    return result;
}

void Factory::freeEnvironment(Environment *environment)
{

    environment->~Environment();
    stack.Free(environment);

}
