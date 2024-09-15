#include "pch.h"
#include "core.hpp"
#include "Interpreter.hpp" 

Core &Core::as()
{
    static Core core;
    return core;
}

void Core::clear()
{
        
    if (!isReady)
        return;



    for (u32 i = 0; i < images.size(); i++)
    {
        CoreImage *image = images[i];
        INFO("Core remove image [%d]" , image->id);
        UnloadTexture(image->texture);
        image->~CoreImage();
        arena.Free(image, sizeof(CoreImage));
    }
    for (CoreSound *sound : sounds)
    {
        INFO("Core remove sound [%d]" , sound->id);
        UnloadSound(sound->sound);
        sound->~CoreSound();
        arena.Free(sound, sizeof(CoreSound));
    }
    for (CoreMusic *music : musics)
    {
        INFO("Core remove music [%d]" , music->id);
        UnloadMusicStream(music->music);
        music->~CoreMusic();
        arena.Free(music, sizeof(CoreMusic));
    }
    for (CoreFont *font : fonts)
    {
        INFO("Core remove font [%d]" , font->id);
        UnloadFont(font->font);
        font->~CoreFont();
        arena.Free(font, sizeof(CoreFont));
    }

    musics.clear();
    fonts.clear();
    sounds.clear();
    images.clear();
    
 //   arena.Clear();
        
    if (isSoundsReady)
    {
        isSoundsReady = false;
        CloseAudioDevice();
    }


    if (isReady)
    {
        isReady = false;
        CloseWindow();
    }
}

void Core::SoundInit()
{
    if (isSoundsReady) return;
    InitAudioDevice(); 
    isSoundsReady = true;
}

void Core::SoundClose()
{
    if (!isSoundsReady) return;
    CloseAudioDevice();
    isSoundsReady = false;
}

void Native_TraceLog(int logLevel, const char *text, va_list args)
{
}


void Core::CreateWindow(int width, int height, const std::string &title)
{

    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    SetExitKey(0);
    isReady = true;
}

void Core::WindowClose()
{
    if (!isReady)
        return;

    clear();
    CloseWindow();
    isReady = false;
}

bool Core::Update()
{
    if (!isReady)
        return false;
    return WindowShouldClose();
}

void Core::SetBackgroundColor(double r, double g, double b, double a)
{
    if (!isReady)
        return;
    backgroundColor.r = static_cast<unsigned char>(r * 255);
    backgroundColor.g = static_cast<unsigned char>(g * 255);
    backgroundColor.b = static_cast<unsigned char>(b * 255);
    backgroundColor.a = static_cast<unsigned char>(a * 255);
}
void Core::Cls()
{
    if (!isReady)
        return;
    ClearBackground(backgroundColor);
}

void Core::Begin_Drawing()
{
    if (!isReady)
        return;
    BeginDrawing();
}

static float memoryInMB(size_t bytes)
{
    return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

static float memoryInKB(size_t bytes)
{
    return static_cast<float>(bytes) / 1024.0f;
}

static const char *memoryIn(size_t bytes)
{
    if (bytes >= 1.0e6)
    {
        return FormatText("%.2f MB", memoryInMB(bytes));
    }
    else if (bytes >= 1.0e3)
    {
        return FormatText("%.2f KB", memoryInKB(bytes));
    }
    return FormatText("%zu bytes", bytes);
}

void Core::End_Drawing()
{
    if (!isReady)
        return;
    DrawFPS(10, 10);
    DrawText(memoryIn(Factory::as().size()), 10, 40, 20, WHITE);
    EndDrawing();
}

void Core::SetAudioVolume(double volume)
{
    if (!isSoundsReady)
        return;
    SetMasterVolume(volume);
}

CoreImage *Core::loadImage(const std::string &path)
{
    void *p = arena.Allocate(sizeof(CoreImage));
    CoreImage *image = new (p) CoreImage();
    image->id = images.size();
    if (FileExists(path.c_str()))
    {
        image->texture = LoadTexture(path.c_str());
        image->width = image->texture.width;
        image->height = image->texture.height;
        INFO("Image loaded: %s width: %d height: %d   id: %d ", path.c_str(), image->texture.width, image->texture.height, image->id);
    }
    else
    {
        Image img = GenImageColor(32, 32, RED);
        image->texture = LoadTextureFromImage(img);
        image->width = image->texture.width;
        image->height = image->texture.height;
        WARNING("Image not found: %s width: %d height: %d  id: %d", path.c_str(), image->texture.width, image->texture.height, image->id);
        UnloadImage(img);
    }

    image->rotation = 0;
    image->position.x = 0;
    image->position.y = 0;
    image->origin.x = image->texture.width / 2;
    image->origin.y = image->texture.height / 2;
    image->scale.x = 1;
    image->scale.y = 1;
    image->color = WHITE;
    image->source.x = 0;
    image->source.y = 0;
    image->source.width = image->texture.width;
    image->source.height = image->texture.height;
    image->width = image->texture.width;
    image->height = image->texture.height;
    images.push_back(image);
    return image;
}

CoreFont *Core::loadFont(const std::string &path)
{
    if (!FileExists(path.c_str()))
    {
        WARNING("Font not found: %s", path.c_str());
        return nullptr;
    }
    void *p = arena.Allocate(sizeof(CoreFont));
    CoreFont *font = new (p) CoreFont();
    font->font = LoadFont(path.c_str());
    font->id = fonts.size();
    INFO("Font loaded: %s id: %d", path.c_str(), font->id);
    fonts.push_back(font);
    return font;
}

CoreSound *Core::loadSound(const std::string &path)
{
    if (!FileExists(path.c_str()))
    {
        WARNING("Sound not found: %s", path.c_str());
        return nullptr;
    }

    void *p = arena.Allocate(sizeof(CoreSound));
    CoreSound *sound = new (p) CoreSound();
    sound->id = sounds.size();
    sound->sound = LoadSound(path.c_str());
    INFO("Sound loaded: %s id: %d", path.c_str(), sound->id);
    sounds.push_back(sound);

    return sound;
}

CoreMusic *Core::loadMusic(const std::string &path)
{
    if (!FileExists(path.c_str()))
    {   
        WARNING("Music not found: %s", path.c_str());
        return nullptr;
    }
    void *p = arena.Allocate(sizeof(CoreMusic));
    CoreMusic *music = new (p) CoreMusic();
    music->id = musics.size();
    music->music = LoadMusicStream(path.c_str());
    INFO("Music loaded: %s id: %d", path.c_str(), music->id);
    musics.push_back(music);

    return music;
}



Core::Core()
{
}

Core::~Core()
{
    clear();
}

#define REGISTER_CORE_FUNCTION(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 0) { \
            ERROR(name " requires none argument"); \
            return ctx->asBoolean(false); \
        } \
        return ctx->asDouble(func()); \
    });


#define REGISTER_CORE_FUNCTION1(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 1) { \
            ERROR(name " requires 1 argument"); \
            return ctx->asBoolean(false); \
        } \
        double n = ctx->getDouble(0); \
        return ctx->asDouble(func(n)); \
    });

#define REGISTER_CORE_FUNCTION2(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 2) { \
            ERROR(name " requires 2 arguments"); \
            return ctx->asBoolean(false); \
        } \
        double n1 = ctx->getDouble(0); \
        double n2 = ctx->getDouble(1); \
        return ctx->asDouble(func(n1, n2)); \
    });

#define REGISTER_CORE_FUNCTION3(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 3) { \
            ERROR(name " requires 3 arguments"); \
            return ctx->asBoolean(false); \
        } \
        double n1 = ctx->getDouble(0); \
        double n2 = ctx->getDouble(1); \
        double n3 = ctx->getDouble(2); \
        return ctx->asDouble(func(n1, n2, n3)); \
    });


ExprPtr native_graphics(Context* ctx, int argc) 
{
    if (argc != 3)
    {
        ERROR("CreateWindow requires (width,height,title) arguments");
        return ctx->asBoolean(false);
    }
    int width = ctx->getInt(0);
    int height = ctx->getInt(1);
    std::string title = ctx->getString(2);


    
    Core::as().CreateWindow(width, height, title);
   return ctx->asNil();
}

ExprPtr native_close_graphics(Context* ctx, int argc) 
{
    Core::as().WindowClose();
    return ctx->asNil();
}

ExprPtr native_update(Context* ctx, int argc) 
{
    bool result = Core::as().Update();
    return ctx->asBoolean(result);
}

ExprPtr native_set_fps(Context* ctx, int argc) 
{
    if (argc != 1)
    {
        ERROR("SetTargetFPS requires (fps) argument");
        return ctx->asBoolean(false);
    }
    SetTargetFPS(ctx->getInt(0));
    return ctx->asNil();
}


void RegisterCore(Interpreter& interpreter)
{
    interpreter.registerFunction("CreateWindow", native_graphics);
    interpreter.registerFunction("CloseWindow", native_close_graphics);
    interpreter.registerFunction("WindowShouldClose", native_update);
    interpreter.registerFunction("SetTargetFPS", native_set_fps);

    REGISTER_CORE_FUNCTION(interpreter, "GetFrameTime", GetFrameTime);
    REGISTER_CORE_FUNCTION(interpreter, "GetTime", GetTime);
    REGISTER_CORE_FUNCTION(interpreter, "GetFPS", GetFPS);
    
    


    
}