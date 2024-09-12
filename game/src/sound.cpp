#include "pch.h"
#include "core.hpp"
#include "Interpreter.hpp" 


u32 Core::SoundLoad(const std::string &path)
{
    if (!isReady)
        return -1;
    CoreSound *sound = loadSound(path);
    u32 id = sounds.size() - 1;
    return static_cast<u32>(id);
}

void Core::cSoundPlay(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    PlaySound(sound->sound);
}

void Core::cSoundStop(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    StopSound(sound->sound);
}

void Core::cSoundPause(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    PauseSound(sound->sound);
}

void Core::cSoundSetVolume(u32 id, double volume)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    SetSoundVolume(sound->sound, volume);
}

void Core::cSoundSetPitch(u32 id, double pitch)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    SetSoundPitch(sound->sound, pitch);
}

void Core::cSetSoundPan(u32 id, double pan)
{
    if (!isReady)
        return;
    if (id < 0 || id >= sounds.size())
        return;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return;
    SetSoundPan(sound->sound, pan);
}

bool Core::cSoundIsPlaying(u32 id)
{
    if (!isReady)
        return false;
    if (id < 0 || id >= sounds.size())
        return false;
    CoreSound *sound = sounds[id];
    if (sound == nullptr)
        return false;
    return IsSoundPlaying(sound->sound);
}

u32 Core::MusicLoad(const std::string &path)
{
    if (!isReady)
        return -1;
    CoreMusic *music = loadMusic(path);
    u32 id = musics.size() ;
    return static_cast<u32>(id);
}

void Core::cMusicPlay(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    PlayMusicStream(music->music);
}

void Core::cMusicStop(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    StopMusicStream(music->music);
}

void Core::cMusicUpdate(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    UpdateMusicStream(music->music);
}

void Core::cMusicPause(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    PauseMusicStream(music->music);
}

void Core::cMusicResume(u32 id)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    ResumeMusicStream(music->music);
}

void Core::cMusicSetVolume(u32 id, double volume)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    SetMusicVolume(music->music, volume);
}

void Core::cMusicSetPitch(u32 id, double pitch)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    SetMusicPitch(music->music, pitch);
}

void Core::cSetMusicPan(u32 id, double pan)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    SetMusicPan(music->music, pan);
}

bool Core::cMusicIsPlaying(u32 id)
{
    if (!isReady)
        return false;
    if (id < 0 || id >= musics.size())
        return false;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return false;
    return IsMusicStreamPlaying(music->music);
}

void Core::cMusicSetPosition(u32 id, double position)
{
    if (!isReady)
        return;
    if (id < 0 || id >= musics.size())
        return;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return;
    SeekMusicStream(music->music, position);
}

double Core::cMusicTimeLength(u32 id)
{
    if (!isReady)
        return 0;
    if (id < 0 || id >= musics.size())
        return 0;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return 0;
    return GetMusicTimeLength(music->music);
}

double Core::cMusicTimePlayed(u32 id)
{
    if (!isReady)
        return 0;
    if (id < 0 || id >= musics.size())
        return 0;
    CoreMusic *music = musics[id];
    if (music == nullptr)
        return 0;
    return GetMusicTimePlayed(music->music);
}







ExprPtr native_sound_load(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("LoadSound requires (fileName) argument");
        return ctx->asNil();
    }

    std::string path = ctx->getString(0);
    long id = Core::as().SoundLoad(path);
    return ctx->asLong(id);
    
}

ExprPtr native_sound_play(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("SoundPlay requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cSoundPlay(id);
    return ctx->asNil();
}

ExprPtr native_sound_stop(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("SoundStop requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cSoundStop(id);
    return ctx->asNil();
}

ExprPtr native_sound_pause(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("SoundPause requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cSoundPause(id);
    return ctx->asNil();
}

ExprPtr native_sound_set_volume(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetSoundVolume requires (id,volume) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double volume = ctx->getDouble(1);
    Core::as().cSoundSetVolume(id, volume);
    return ctx->asNil();
}

ExprPtr native_sound_set_pitch(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetSoundPitch requires (id,pitch) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double pitch = ctx->getDouble(1);
    Core::as().cSoundSetPitch(id, pitch);
    return ctx->asNil();
}

ExprPtr native_set_sound_pan(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetSoundPan requires (id,pan) argument");
        return ctx->asNil();
    }   

    long id = ctx->getLong(0);
    double pan = ctx->getDouble(1);
    Core::as().cSetSoundPan(id, pan);
    return ctx->asNil();
}

ExprPtr native_sound_is_playing(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("SoundIsPlaying requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    bool playing = Core::as().cSoundIsPlaying(id);
    return ctx->asBoolean(playing);
}



ExprPtr native_music_load(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("LoadMusic requires (fileName) argument");
        return ctx->asNil();
    }

    std::string path = ctx->getString(0);
    long id = Core::as().MusicLoad(path);
    return ctx->asLong(id);
}

ExprPtr native_music_play(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("PlayMusic requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cMusicPlay(id);
    return ctx->asNil();
}

ExprPtr native_music_stop(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("StopMusic requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cMusicStop(id);
    return ctx->asNil();
}

ExprPtr native_music_pause(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("PauseMusic requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cMusicPause(id);
    return ctx->asNil();
}

ExprPtr native_music_resume(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("ResumeMusic requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cMusicResume(id);
    return ctx->asNil();
}

ExprPtr native_music_set_volume(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetMusicVolume requires (id,volume) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double volume = ctx->getDouble(1);
    Core::as().cMusicSetVolume(id, volume);
    return ctx->asNil();
}

ExprPtr native_music_set_pitch(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetMusicPitch requires (id,pitch) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double pitch = ctx->getDouble(1);
    Core::as().cMusicSetPitch(id, pitch);
    return ctx->asNil();
}

ExprPtr native_music_update(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("UpdateMusic requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    Core::as().cMusicUpdate(id);
    return ctx->asNil();
}

ExprPtr native_music_is_playing(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("MusicIsPlaying requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    bool playing = Core::as().cMusicIsPlaying(id);
    return ctx->asBoolean(playing);
}

ExprPtr native_music_set_position(Context* ctx, int argc)
{
    if (argc != 2)
    {
        ERROR("SetMusicPosition requires (id,position) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double position = ctx->getDouble(1);
    Core::as().cMusicSetPosition(id, position);
    return ctx->asNil();
}

ExprPtr native_music_time_length(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("GetMusicLength requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double length = Core::as().cMusicTimeLength(id);
    return ctx->asDouble(length);
}

ExprPtr native_music_time_played(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("GetMusicPosition requires (id) argument");
        return ctx->asNil();
    }

    long id = ctx->getLong(0);
    double length = Core::as().cMusicTimePlayed(id);
    return ctx->asDouble(length);
}

ExprPtr native_audio_set_volume(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("SetAudioVolume requires (volume) argument");
        return ctx->asNil();
    }

    double volume = ctx->getDouble(0);
    Core::as().SetAudioVolume( volume);
    return ctx->asNil();
}

ExprPtr native_audio_init(Context* ctx, int argc)
{
    if (argc != 0)
    {
        ERROR("InitAudio requires no arguments");
        return ctx->asNil();
    }
    Core::as().SoundInit();
    return ctx->asNil();
    
}


ExprPtr native_audio_deinit(Context* ctx, int argc)
{
    if (argc != 0)
    {
        ERROR("CloseAudio requires no arguments");
        return ctx->asNil();
    }
    Core::as().SoundClose();
    return ctx->asNil();
}

void RegisterAudio(Interpreter& interpreter)
{

    interpreter.registerFunction("LoadSound", native_sound_load);
    interpreter.registerFunction("PlaySound", native_sound_play);
    interpreter.registerFunction("StopSound", native_sound_stop);
    interpreter.registerFunction("PauseSound", native_sound_pause);
    interpreter.registerFunction("SetSoundVolume", native_sound_set_volume);
    interpreter.registerFunction("SetSoundPitch", native_sound_set_pitch);
    interpreter.registerFunction("SetSoundPan", native_set_sound_pan);
    interpreter.registerFunction("IsPlayingSound", native_sound_is_playing);

    interpreter.registerFunction("LoadMusic", native_music_load);
    interpreter.registerFunction("PlayMusic", native_music_play);
    interpreter.registerFunction("UpdateMusic", native_music_update);
    interpreter.registerFunction("StopMusic", native_music_stop);
    interpreter.registerFunction("PauseMusic", native_music_pause);
    interpreter.registerFunction("ResumeMusic", native_music_resume);
    interpreter.registerFunction("SetMusicVolume", native_music_set_volume);
    interpreter.registerFunction("SetMusicPitch", native_music_set_pitch);
    interpreter.registerFunction("IsPlayingMusic", native_music_is_playing);
    interpreter.registerFunction("SetMusicPosition", native_music_set_position);
    interpreter.registerFunction("GetMusicPosition", native_music_time_played);
    interpreter.registerFunction("GetMusicLength", native_music_time_length);

    interpreter.registerFunction("SetAudioVolume", native_audio_set_volume);
    interpreter.registerFunction("InitAudio", native_audio_init);
    interpreter.registerFunction("CloseAudio", native_audio_deinit);
    

    

   

}

