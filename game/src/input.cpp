#include "pch.h"
#include "core.hpp"
#include "Interpreter.hpp" 


//Input

ExprPtr native_key_down(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("KeyDown requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsKeyDown(key);
    return ctx->asBoolean(result);
}

ExprPtr native_key_up(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("KeyUp requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsKeyUp(key);
    return ctx->asBoolean(result);
}

ExprPtr native_key_press(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("KeyPress requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsKeyPressed(key);
    return ctx->asBoolean(result);
}

ExprPtr native_mouse_down(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("MouseDown requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsMouseButtonDown(key);
    return ctx->asBoolean(result);
}

ExprPtr native_mouse_up(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("MouseUp requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsMouseButtonUp(key);
    return ctx->asBoolean(result);
}

ExprPtr native_mouse_press(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("MousePress requires (key) argument");
        return ctx->asBoolean(false);
    }
    int key = ctx->getInt(0);
    bool result = IsMouseButtonPressed(key);
    return ctx->asBoolean(result);
}

ExprPtr native_mouse_x(Context* ctx, int argc)
{
    double result = GetMouseX();
    return ctx->asDouble(result);
}

ExprPtr native_mouse_y(Context* ctx, int argc)
{
    double result = GetMouseY();
    return ctx->asDouble(result);
}

void RegisterInput(Interpreter &interpreter)
{


    //Input
    interpreter.registerFunction("KeyDown", native_key_down);
    interpreter.registerFunction("KeyUp", native_key_up);
    interpreter.registerFunction("KeyPress", native_key_press);
    interpreter.registerFunction("MouseDown", native_mouse_down);
    interpreter.registerFunction("MouseUp", native_mouse_up);
    interpreter.registerFunction("MousePress", native_mouse_press);
    interpreter.registerFunction("GetMouseX", native_mouse_x);
    interpreter.registerFunction("GetMouseY", native_mouse_y);

    interpreter.registerInteger("KEY_SPACE", KEY_SPACE);
    interpreter.registerInteger("KEY_ENTER", KEY_ENTER);
    interpreter.registerInteger("KEY_ESCAPE", KEY_ESCAPE);
    interpreter.registerInteger("KEY_BACKSPACE", KEY_BACKSPACE);
    interpreter.registerInteger("KEY_TAB", KEY_TAB);

    interpreter.registerInteger("KEY_LEFT", KEY_LEFT);
    interpreter.registerInteger("KEY_RIGHT", KEY_RIGHT);
    interpreter.registerInteger("KEY_UP", KEY_UP);
    interpreter.registerInteger("KEY_DOWN", KEY_DOWN);
    
    
    interpreter.registerInteger("KEY_APOSTROPHE", KEY_APOSTROPHE);
    interpreter.registerInteger("KEY_COMMA", KEY_COMMA);
    interpreter.registerInteger("KEY_MINUS", KEY_MINUS);
    interpreter.registerInteger("KEY_PERIOD", KEY_PERIOD);
    interpreter.registerInteger("KEY_SLASH", KEY_SLASH);


    interpreter.registerInteger("KEY_Q", KEY_Q);
    interpreter.registerInteger("KEY_W", KEY_W);
    interpreter.registerInteger("KEY_E", KEY_E);
    interpreter.registerInteger("KEY_R", KEY_R);
    interpreter.registerInteger("KEY_T", KEY_T);
    interpreter.registerInteger("KEY_Y", KEY_Y);
    interpreter.registerInteger("KEY_U", KEY_U);
    interpreter.registerInteger("KEY_I", KEY_I);
    interpreter.registerInteger("KEY_O", KEY_O);
    interpreter.registerInteger("KEY_P", KEY_P);

    interpreter.registerInteger("KEY_A", KEY_A);
    interpreter.registerInteger("KEY_S", KEY_S);
    interpreter.registerInteger("KEY_D", KEY_D);
    interpreter.registerInteger("KEY_F", KEY_F);
    interpreter.registerInteger("KEY_G", KEY_G);
    interpreter.registerInteger("KEY_H", KEY_H);
    interpreter.registerInteger("KEY_J", KEY_J);
    interpreter.registerInteger("KEY_K", KEY_K);
    interpreter.registerInteger("KEY_L", KEY_L);

    interpreter.registerInteger("KEY_Z", KEY_Z);
    interpreter.registerInteger("KEY_X", KEY_X);
    interpreter.registerInteger("KEY_C", KEY_C);
    interpreter.registerInteger("KEY_V", KEY_V);
    interpreter.registerInteger("KEY_B", KEY_B);
    interpreter.registerInteger("KEY_N", KEY_N);
    interpreter.registerInteger("KEY_M", KEY_M);

    interpreter.registerInteger("KEY_0", KEY_ZERO);
    interpreter.registerInteger("KEY_1", KEY_ONE);
    interpreter.registerInteger("KEY_2", KEY_TWO);
    interpreter.registerInteger("KEY_3", KEY_THREE);
    interpreter.registerInteger("KEY_4", KEY_FOUR);
    interpreter.registerInteger("KEY_5", KEY_FIVE);
    interpreter.registerInteger("KEY_6", KEY_SIX);
    interpreter.registerInteger("KEY_7", KEY_SEVEN);
    interpreter.registerInteger("KEY_8", KEY_EIGHT);
    interpreter.registerInteger("KEY_9", KEY_NINE);

    interpreter.registerInteger("MOUSE_LEFT", MOUSE_BUTTON_LEFT);
    interpreter.registerInteger("MOUSE_RIGHT", MOUSE_BUTTON_RIGHT);
    interpreter.registerInteger("MOUSE_MIDDLE", MOUSE_BUTTON_MIDDLE);



}