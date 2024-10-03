
#include "pch.h" 

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <utility>

#include <vector>
#include <string>



#include <iostream>
#include <fstream>
#include <sstream>

#include "Utils.hpp" 
#include "Interpreter.hpp" 
#include "core.hpp" 

void RegisterGraphics(Interpreter &interpreter);
void RegisterInput(Interpreter &interpreter);
void RegisterMath(Interpreter &interpreter);
void RegisterCore(Interpreter &interpreter);
void RegisterAudio(Interpreter &interpreter);

ExprPtr native_writeln(Context* ctx, int argc) 
{
    for (int i = 0; i < argc; i++)
    {
            if (ctx->isString(i))
            {
                std::cout <<ctx->getString(i);
            }
            else if (ctx->isNumber(i))
            {
                std::cout << ctx->getDouble(i);
            }
    }
    std::cout<< std::endl;
    return ctx->asNil();
}

ExprPtr native_write(Context* ctx, int argc) 
{
    for (int i = 0; i < argc; i++)
    {
            if (ctx->isString(i))
            {
                std::cout <<ctx->getString(i);
            }
            else if (ctx->isNumber(i))
            {
                std::cout << ctx->getDouble(i);
            }
    }
    return ctx->asNil();
}


ExprPtr native_random(Context* ctx, int argc)
{
    if (argc != 0 && argc != 2)
    {
        ERROR("Random requires 0 or (min/max) arguments");
        return ctx->asDouble(0);
    }

    if (argc == 0)
    {
        double value = ctx->getDouble(0);

        return ctx->asDouble(Math::range());
    }
    if (argc == 2)
    {
        double min = ctx->getDouble(0);
        double max = ctx->getDouble(1);
        return ctx->asDouble(Math::range(min, max));
    }
   
    return ctx->asDouble(0);    
    
}








std::string readFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file)
    {
        throw std::runtime_error("Could not open file.");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}




int main() 
{


     std::string code;
   
   #if defined(_WIN32)
        ChangeDirectory("../");
       //code= readFile("../game.bu");
   #else
       
   #endif
   
   code= readFile("game.bu");
    if (code.length() == 0)
    {
        return 0;
    } 

    Interpreter interpreter;


    interpreter.registerFunction("Random", native_random);
    interpreter.registerFunction("writeln", native_writeln);
    interpreter.registerFunction("write", native_write);

    RegisterCore(interpreter);
    RegisterGraphics(interpreter);
    RegisterInput(interpreter);
    RegisterMath(interpreter);
    RegisterAudio(interpreter);
    
    


    try 
    {
        interpreter.compile(code);
    }
    catch (const  std::exception &e)
    {
        ERROR("%s",e.what());
        
    }



   // Core::as().clear();
    interpreter.clear();
   // std::cout <<"Exit " << std::endl;
  

    return 0;

  
}

