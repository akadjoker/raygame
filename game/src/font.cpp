#include "pch.h"
#include "core.hpp"
#include "Interpreter.hpp" 

//RLAPI void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint); // Draw text using font and additional parameters
//RLAPI void DrawTextPro(Font font, const char *text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint); // Draw text using Font and pro parameters (rotation)



u32 Core::cLoadFont(const std::string &path)
{
    if (!isReady)
        return -1;
    if (!FileExists(path.c_str()))
        return -1;
    CoreFont *font = loadFont(path);
    u32 id = fonts.size() - 1;
    return static_cast<u32>(id);
}

void Core::cDrawText(u32 font, const std::string &text, double x, double y)
{
    if (!isReady)
        return;
    if (font < 0 || font >= fonts.size())
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;

        DrawTextEx(f->font, text.c_str(), 
        {static_cast<float>(x), static_cast<float>(y)}, 
        static_cast<float>(f->fontSize),static_cast<float>(f->fontSize), f->color);

}

void Core::cDrawRotateText(u32 font, const std::string &text, double x, double y, double angle)
{
    if (!isReady)
        return; 
    if (font < 0 || font >= fonts.size())
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;

    Vector2 position = {static_cast<float>(x), static_cast<float>(y)};
    Vector2 origin = {static_cast<float>(f->origin.x), static_cast<float>(f->origin.y)};

    DrawTextPro(f->font, text.c_str(), 
        position, origin, angle, static_cast<float>(f->fontSize), static_cast<float>(f->spacing), f->color);
}

int Core::GetFontTextWidth(u32 font, const std::string &text)
{
    if (!isReady)
        return 0;
    if (font < 0 || font >= fonts.size())   
        return 0;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return 0;
    return MeasureText(text.c_str(), static_cast<int>(f->fontSize));
}

void Core::SetFontSpacing(u32 font, double spacing)
{
    if (!isReady)
        return;
    if (font < 0 || font >= fonts.size())
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;
    f->spacing = static_cast<float>(spacing);
}

void Core::SetFontColor(u32 font, double r, double g, double b)
{
    if (!isReady)
        return;
    if (font < 0 || font >= fonts.size())   
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;
    f->color.r = static_cast<unsigned char>(r * 255);
    f->color.g = static_cast<unsigned char>(g * 255);
    f->color.b = static_cast<unsigned char>(b * 255);
}

void Core::SetFontAlpha(u32 font, double a)
{
    if (!isReady)
        return;
    if (font < 0 || font >= fonts.size())
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;
    f->color.a = static_cast<unsigned char>(a * 255);
}

void Core::SetFontOrigin(u32 font, double x, double y)
{
    if (!isReady)
        return;
    if (font < 0 || font >= fonts.size())
        return;
    CoreFont *f = fonts[font];
    if (f == nullptr)
        return;
    f->origin.x = static_cast<float>(x);
    f->origin.y = static_cast<float>(y);
}

