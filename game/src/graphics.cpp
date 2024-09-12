#include "pch.h"
#include "core.hpp"
#include "Interpreter.hpp" 




u32 Core::ImageLoad(const std::string &path)
{
    if (!isReady)
        return -1;
    CoreImage *image = loadImage(path);
    u32 id = images.size() - 1;
    return static_cast<u32>(id);
}

void Core::ImageDraw(u32 id, double x, double y)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;

    Rectangle source = image->source;

    Rectangle dest;
    dest.x = static_cast<float>(x);
    dest.y = static_cast<float>(y);

    dest.width = static_cast<float>(image->width   * image->scale.x);
    dest.height = static_cast<float>(image->height * image->scale.y);

    // = {x, y, image->width * image->scale.x, image->height * image->scale.y};
    DrawTexturePro(image->texture, image->source, dest, image->origin, image->rotation , image->color);

    // DrawTexturePro(image->texture, image->texture, image->position, image->origin, image->rotation, image->color);
    //  DrawTexture(image->texture, x, y, image->color);
}

void Core::ImageDrawRect(u32 id, double x, double y, double src_x, double src_y, double src_width, double src_height)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;
    image->source.x = static_cast<float>(src_x);
    image->source.y = static_cast<float>(src_y);
    image->source.width = static_cast<float>(src_width);
    image->source.height = static_cast<float>(src_height);

    ImageDraw(id, x, y);
}

void Core::SetImageRotation(u32 id, double rotation)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;
    image->rotation = rotation;
}



void Core::SetImageScale(u32 id, double x, double y)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;
    image->scale.x = static_cast<float>(x);
    image->scale.y = static_cast<float>(y);
}

void Core::SetImageOrigin(u32 id, double x, double y)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;
    image->origin.x = static_cast<float>(x);
    image->origin.y = static_cast<float>(y);
}

void Core::SetImageColor(u32 id, double r, double g, double b)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;
    image->color.r = static_cast<unsigned char>(r * 255);
    image->color.g = static_cast<unsigned char>(g * 255);
    image->color.b = static_cast<unsigned char>(b * 255);


}
void Core::SetImageAlpha(u32 id ,double a)
{
    if (!isReady)
        return;
    if (id < 0 || id >= images.size())
        return;
    CoreImage *image = images[id];
    if (image == nullptr)
        return;

    image->color.a = static_cast<unsigned char>(a * 255);

}

double Core::GetImageRotation(u32 id)
{
    if (!isReady)
        return 0;
    if (id < 0 || id >= images.size())
        return 0;
    CoreImage *image = images[id];
    if (image == nullptr)
        return 0;
    return image->rotation;
}

double Core::GetImageWidth(u32 id)
{
  
    if (!isReady)
        return 0;
    if (id < 0 || id >= images.size())
        return 0;
    CoreImage *image = images[id];
    if (image == nullptr)
        return 0;
    return image->width;
}

double Core::GetImageHeight(u32 id)
{
    if (!isReady)
        return 0;
    if (id < 0 || id >= images.size())
        return 0;
    CoreImage *image = images[id];
    if (image == nullptr)
        return 0;
    return image->height;
}

void Core::Point(double x, double y)
{
    if (!isReady)
        return;
    DrawPixel(static_cast<int>(x), static_cast<int>(y), drawColor);
}

void Core::Ring(double x, double y, double innerRadius, double outerRadius, double startAngle, double endAngle, int segments, bool fill)
{
    if (!isReady)
        return;
    Vector2 position = { static_cast<float>(x), static_cast<float>(y) };
    if (fill)   
        DrawRing(position, static_cast<float>(innerRadius), static_cast<float>(outerRadius), static_cast<float>(startAngle), static_cast<float>(endAngle), static_cast<int>(segments), drawColor);
    else
        DrawRingLines(position, static_cast<float>(innerRadius), static_cast<float>(outerRadius), static_cast<float>(startAngle), static_cast<float>(endAngle), static_cast<int>(segments), drawColor);

}

void Core::Ellipse(double x, double y, double width, double height, bool fill)
{
    if (!isReady)
        return;
    if (fill)
        DrawEllipse(static_cast<int>(x), static_cast<int>(y), static_cast<float>(width), static_cast<float>(height), drawColor);
    else
        DrawEllipseLines(static_cast<int>(x), static_cast<int>(y), static_cast<float>(width), static_cast<float>(height), drawColor);
}

void Core::Circle(double x, double y, double radius, bool fill)
{
    if (!isReady)
        return;
    if (fill)
        DrawCircle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(radius), drawColor);
    else
        DrawCircleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(radius), drawColor);
}

void Core::Line(double x1, double y1, double x2, double y2)
{
    if (!isReady)
        return;
    DrawLine(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2), drawColor);
}

void Core::Rect(double x, double y, double width, double height, bool fill)
{
    if (!isReady)
        return;
    if (fill)
        DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height), drawColor);
    else
        DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height), drawColor);
}

void Core::SetDrawAlpha(double a)
{
    if (!isReady)
        return;
    drawColor.a = static_cast<unsigned char>(a * 255);
}

void Core::Text(const std::string &text, double x, double y, double size)
{
    if (!isReady)
        return;
    DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(size), drawColor);
}



void Core::SetDrawColor(double r, double g, double b)
{
    if (!isReady)
        return;
    drawColor.r = static_cast<unsigned char>(r * 255);
    drawColor.g = static_cast<unsigned char>(g * 255);
    drawColor.b = static_cast<unsigned char>(b * 255);
}




ExprPtr native_begin_draw(Context* ctx, int argc) 
{
    Core::as().Begin_Drawing();
    return ctx->asNil();
}

ExprPtr native_end_draw(Context* ctx, int argc) 
{
    Core::as().End_Drawing();
    return ctx->asNil();
}

ExprPtr native_clear(Context* ctx, int argc) 
{
    Core::as().Cls();
    return ctx->asNil();
}
//SetBackgroundColor

ExprPtr native_set_background_color(Context* ctx, int argc)
{
    if (argc != 3)
    {
        ERROR("SetBackgroundColor requires (r,g,b) arguments");
        return ctx->asNil();
    }
    double r = ctx->getDouble(0);
    double g = ctx->getDouble(1);
    double b = ctx->getDouble(2);
    double a = 1;
    Core::as().SetBackgroundColor(r, g, b, a);
    return ctx->asNil();
}

//Imgae 


ExprPtr native_image_load(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("LoadImage requires (file name) argument");
        return ctx->asBoolean(false);
    }
    std::string path = ctx->getString(0);
    u32 id = Core::as().ImageLoad(path);
 return ctx->asNil();
}


ExprPtr native_image_draw(Context* ctx, int argc) 
{
    if (argc != 3)
    {
        ERROR("DrawImage requires (id,x,y) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double x = ctx->getDouble(1);
    double y = ctx->getDouble(2);

   // INFO("DrawImage %ld %f %f", id, x, y);

    Core::as().ImageDraw(id, x, y);
    return ctx->asNil();
}

ExprPtr native_image_draw_rect(Context* ctx, int argc)
{
    if (argc != 7)
    {
        ERROR("DrawRect requires (id,x,y,src_x,src_y,src_width,src_height) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double x = ctx->getDouble(1);
    double y = ctx->getDouble(2);
    
    double src_x = ctx->getDouble(3);
    double src_y = ctx->getDouble(4);

    double src_w = ctx->getDouble(5);
    double src_h = ctx->getDouble(6);

    Core::as().ImageDrawRect(id, x, y, src_x, src_y, src_w, src_h);
    return ctx->asNil();
}


ExprPtr native_image_set_rotate(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("RotateImage requires (id,rotation) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double rotation = ctx->getDouble(1);
    Core::as().SetImageRotation(id, rotation);
    return ctx->asNil();
}

ExprPtr native_image_get_rotate(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("GetImageRotation requires (id) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double rotation = Core::as().GetImageRotation(id);
    return ctx->asDouble(rotation);
}


ExprPtr native_image_set_scale(Context* ctx, int argc)
{
    if (argc != 3)
    {
        ERROR("SetImageScale requires (id,x,y) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double x = ctx->getDouble(1);
    double y = ctx->getDouble(2);
    Core::as().SetImageScale(id, x, y);
    return ctx->asNil();
}


ExprPtr native_image_set_origin(Context* ctx, int argc)
{
    if (argc != 3)
    {
        ERROR("SetImageOrigin requires (id,x,y) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double x = ctx->getDouble(1);
    double y = ctx->getDouble(2);
    Core::as().SetImageOrigin(id, x, y);
    return ctx->asNil();
}


ExprPtr native_image_set_color(Context* ctx, int argc)
{
    if (argc != 4)
    {
        ERROR("SetImageColor requires (id,r,g,b) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double r = ctx->getDouble(1);
    double g = ctx->getDouble(2);
    double b = ctx->getDouble(3);

    Core::as().SetImageColor(id, r, g, b);
    return ctx->asNil();
}


ExprPtr native_image_set_alpha(Context* ctx, int argc)  
{
    if (argc != 2)
    {
        ERROR("SetImageAlpha requires (id,alpha) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double alpha = ctx->getDouble(1);
    Core::as().SetImageAlpha(id, alpha);
    return ctx->asNil();
}


ExprPtr native_image_get_width(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("GetImageWidth requires (id) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double w = Core::as().GetImageWidth(id);
    return ctx->asDouble(w);
}


ExprPtr native_image_get_height(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("GetImageHeight requires (id) arguments");
        return ctx->asBoolean(false);
    }
    u32 id = ctx->getLong(0);
    double h = Core::as().GetImageHeight(id);
    return ctx->asDouble(h);
}



//Draw


ExprPtr native_draw_text(Context* ctx, int argc) 
{
    std::string text;
    if (argc < 4)
    {
        ERROR("DrawText requires (x,y,size,....) arguments");
        return ctx->asBoolean(false);
    }

    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    double size = ctx->getDouble(2);
    
    for (int i = 3; i < argc; i++)
    {
            if (ctx->isString(i))
            {
                
                text += ctx->getString(i);
            }
            else if (ctx->isNumber(i))
            {
        
                text += std::to_string(ctx->getDouble(i));
            }
    }
    Core::as().Text(text, x, y, size);
    return ctx->asNil();
}

ExprPtr native_draw_color(Context* ctx, int argc) 
{
    if (argc != 3)
    {
        ERROR("SetDrawColor requires (r,g,b) arguments");
        return ctx->asBoolean(false);
    }
    double r = ctx->getDouble(0);
    double g = ctx->getDouble(1);
    double b = ctx->getDouble(2);

    Core::as().SetDrawColor(r, g, b);
    return ctx->asNil();
}

ExprPtr native_draw_alpha(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("SetDrawAlpha requires (alpha) argument");
        return ctx->asBoolean(false);
    }
    double a = ctx->getDouble(0);
    Core::as().SetDrawAlpha(a);
    return ctx->asNil();
}

ExprPtr native_draw_rect(Context* ctx, int argc) 
{
    if (argc != 5 && argc != 4)
    {
        ERROR("DrawRect requires ( x,y,w,h,[fill]) arguments");
        return ctx->asBoolean(false);
    }
    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    double w = ctx->getDouble(2);
    double h = ctx->getDouble(3);
    bool fill = false;
    if (argc == 5)
    {
        fill = ctx->getBoolean(4);
    }
    Core::as().Rect(x, y, w, h, fill);
    return ctx->asNil();
}

//void Core::Ring(double x, double y, double innerRadius, double outerRadius, double startAngle, double endAngle, int segments, bool fill)


ExprPtr native_draw_ring(Context* ctx, int argc)
{
    if (argc != 7 && argc != 6)
    {
        ERROR("DrawRing requires ( x,y,innerRadius,outerRadius,startAngle,endAngle,segments,[fill]) arguments");
        return ctx->asBoolean(false);
    }
    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    double innerRadius = ctx->getDouble(2);
    double outerRadius = ctx->getDouble(3);
    double startAngle = ctx->getDouble(4);
    double endAngle = ctx->getDouble(5);
    int segments = ctx->getInt(6);
    bool fill = false;
    if (argc == 7)
    {
        fill = ctx->getBoolean(7);
    }
    Core::as().Ring(x, y, innerRadius, outerRadius, startAngle, endAngle, segments, fill);
    return ctx->asNil();
}

ExprPtr native_draw_ellipse(Context* ctx, int argc)
{
    if (argc != 5 && argc != 4)
    {
        ERROR("DrawEllipse requires ( x,y,w,h,[fill]) arguments");
        return ctx->asBoolean(false);
    }
    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    double w = ctx->getDouble(2);
    double h = ctx->getDouble(3);
    bool fill = false;
    if (argc == 5)
    {
        fill = ctx->getBoolean(4);
    }
    Core::as().Ellipse(x, y, w, h, fill);
    return ctx->asNil();
}

ExprPtr native_draw_circle(Context* ctx, int argc) 
{
    if (argc != 3 && argc != 4)
    {
        ERROR("DrawCircle requires (x,y,radius,[fill]) arguments");
        return ctx->asBoolean(false);
    }
    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    double r = ctx->getDouble(2);
 //   INFO("DrawCircle %f %f %f", x, y, r);
    bool fill = false;
    if (argc == 4)
    {
        fill = ctx->getBoolean(3);
    }
    Core::as().Circle(x, y, r, fill);
    return ctx->asNil();
}

ExprPtr native_draw_line(Context* ctx, int argc)
{
    if (argc != 4)
    {
        ERROR("DrawLine requires (x1,y1,x2,y2) arguments");
        return ctx->asBoolean(false);
    }
    double x1 = ctx->getDouble(0);
    double y1 = ctx->getDouble(1);
    double x2 = ctx->getDouble(2);
    double y2 = ctx->getDouble(3);
    Core::as().Line(x1, y1, x2, y2);
    return ctx->asNil();
}

ExprPtr native_draw_point(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("DrawPoint requires (x,y) arguments");
        return ctx->asBoolean(false);
    }
    double x = ctx->getDouble(0);
    double y = ctx->getDouble(1);
    Core::as().Point(x, y);
    return ctx->asNil();
}

ExprPtr native_load_font(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("LoadFont requires (name) as argument");
        return ctx->asBoolean(false);
    }   
    std::string name = ctx->getString(0);
    long id = Core::as().cLoadFont(name);
    return ctx->asLong(id);
}

ExprPtr native_draw_font(Context* ctx, int argc)
{
    if (argc != 4)
    {
        ERROR("DrawFont requires (id,text,x,y) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    std::string text = ctx->getString(1);
    double x = ctx->getDouble(2);
    double y = ctx->getDouble(3);
    Core::as().cDrawText(id,text, x, y);
    return ctx->asNil();
}

ExprPtr native_draw_text_rotate(Context* ctx, int argc)
{
    if (argc != 5)
    {
        ERROR("DrawTextRotate requires (id,text,x,y,angle) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    std::string text = ctx->getString(1);
    double x = ctx->getDouble(2);
    double y = ctx->getDouble(3);
    double angle = ctx->getDouble(4);
    Core::as().cDrawRotateText(id,text, x, y, angle);
    return ctx->asNil();
}

ExprPtr native_get_font_text_width(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("GetFontTextWidth requires (id,text) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    std::string text = ctx->getString(1);
    return ctx->asDouble(Core::as().GetFontTextWidth(id,text));
}

ExprPtr native_set_font_spacing(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetFontSpacing requires (id,spacing) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    double spacing = ctx->getDouble(1);
    Core::as().SetFontSpacing(id,spacing);
    return ctx->asNil();
}

ExprPtr native_set_font_color(Context* ctx, int argc)
{
    if (argc != 4)
    {
        ERROR("SetFontColor requires (id,r,g,b) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    double r = ctx->getDouble(1);
    double g = ctx->getDouble(2);
    double b = ctx->getDouble(3);
    Core::as().SetFontColor(id,r,g,b);
    return ctx->asNil();
}

ExprPtr native_set_font_alpha(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetFontAlpha requires (id,a) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    double a = ctx->getDouble(1);
    Core::as().SetFontAlpha(id,a);
    return ctx->asNil();
}

ExprPtr native_set_font_origin(Context* ctx, int argc)
{
    if (argc != 3)
    {
        ERROR("SetFontOrigin requires (id,x,y) arguments");
        return ctx->asNil();
    }
    u32 id = ctx->getLong(0);
    double x = ctx->getDouble(1);
    double y = ctx->getDouble(2);
    Core::as().SetFontOrigin(id,x,y);
    return ctx->asNil();
}









void RegisterGraphics(Interpreter& interpreter)
{

    
    //Graphics
     interpreter.registerFunction("BeginDraw", native_begin_draw);
    interpreter.registerFunction("EndDraw", native_end_draw);
    interpreter.registerFunction("Clear", native_clear);
    interpreter.registerFunction("SetClearColor", native_set_background_color);
    
    interpreter.registerFunction("SetDrawColor", native_draw_color);
    interpreter.registerFunction("SetDrawAlpha", native_draw_alpha);



    //Image
    interpreter.registerFunction("LoadImage", native_image_load);
    interpreter.registerFunction("DrawImage", native_image_draw);
    interpreter.registerFunction("DrawImageRect", native_image_draw_rect);
    interpreter.registerFunction("SetImageRotation", native_image_set_rotate);
    interpreter.registerFunction("SetImageColor", native_image_set_color);
    interpreter.registerFunction("SetImageAlpha", native_image_set_alpha);
    interpreter.registerFunction("SetImageOrigin", native_image_set_origin);
    interpreter.registerFunction("SetImageScale", native_image_set_scale);

    interpreter.registerFunction("GetImageRotation", native_image_get_rotate);
    interpreter.registerFunction("GetImageWidth", native_image_get_width);
    interpreter.registerFunction("GetImageHeight", native_image_get_height);
    



    



    //Draw
    interpreter.registerFunction("DrawColor", native_draw_color);
    interpreter.registerFunction("DrawAlpha", native_draw_alpha);
    interpreter.registerFunction("DrawRect", native_draw_rect);
    interpreter.registerFunction("DrawCircle", native_draw_circle);
    interpreter.registerFunction("DrawEllipse", native_draw_ellipse);
    interpreter.registerFunction("DrawRing", native_draw_ring);
    interpreter.registerFunction("DrawLine", native_draw_line);
    interpreter.registerFunction("DrawPoint", native_draw_point);
    interpreter.registerFunction("DrawText", native_draw_text);

    //font

    interpreter.registerFunction("LoadFont", native_load_font);
    interpreter.registerFunction("DrawFontRotate", native_draw_text_rotate);
    interpreter.registerFunction("DrawFont", native_draw_text);
    interpreter.registerFunction("GetFontWidth", native_get_font_text_width);
    interpreter.registerFunction("SetFontSpacing", native_set_font_spacing);
    interpreter.registerFunction("SetFontColor", native_set_font_color);
    interpreter.registerFunction("SetFontAlpha", native_set_font_alpha);
    interpreter.registerFunction("SetFontOrigin", native_set_font_origin);

}
