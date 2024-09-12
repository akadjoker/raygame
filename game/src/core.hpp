#pragma once


#include <raylib.h>
#include <rlgl.h>

#include "Config.hpp"
#include "Arena.hpp"
#include "Utils.hpp"





template<typename T>
T ConvertFromDouble(u32 ptr)
{
   T obj = reinterpret_cast<T>(static_cast<intptr_t>(ptr));

   if (obj == NULL)
      ERROR("Invalid object ID passed from script!");

   return obj;
}

template<typename T>
u32 ConvertToDouble(T ptr)
{
   return static_cast<u32>(reinterpret_cast<intptr_t>(ptr));
}
#define PI_TIMES_TWO 6.28318530718f
#define PI2 PI * 2
#define DEG -180 / PI
#define RAD PI / -180

namespace Math
{
    const float RECIPROCAL_PI = 1.0f / PI;
    const float HALF_PI = PI / 2.0f;
    const float DEGTORAD = PI / 180.0f;
    const float RADTODEG = 180.0f / PI;
    const float MinValue = -3.40282347E+38;
    const float MaxValue = 3.40282347E+38;
    const float Epsilon = 1.401298E-45;




    int sign(double value);
    double clip(double value, double min, double max);
    double clamp(double value, double min, double max);
    double lerp(double a, double b, double t);
    double normalize_angle(double angle);
    double clamp_angle(double angle, double min, double max);
    double hermite(double value1, double tangent1, double value2, double tangent2, double amount);
    double smooth_step(double value1, double value2, double amount);
    double  repeat(double t, double length);
    double ping_pong(double t, double length);
    double get_distx(double a, double d);
    double get_disty(double a, double d);
    double range();
    double range(double min, double max);

    double Sin( double n );
    double Cos( double n );
    double Tan( double n );
    double ASin( double n );
    double ACos( double n );
    double ATan( double n );
    double ATan2( double n,double t );
    double Sqr( double n );
    double Floor( double n );
    double Ceil( double n );
    double Exp( double n );
    double Log( double n );
    double Log10( double n );

    double Random();
    double Random(double from, double to);

} // namespace Math

struct CoreImage
{
    u32 id;
    Texture2D texture;
    int width;
    int height;
    Rectangle source;

    double rotation;
    Vector2 position;
    Vector2 origin;
    Vector2 scale;
    Color  color;
};

struct CoreSound
{
    Sound sound;
    u32 id;
};

struct CoreMusic
{
    Music music;
    u32 id;
};

struct CoreFont
{
    Font font;
    u32 id;
    Vector2 origin;
    float fontSize;
    float spacing;
    Color color;
};

class Core 
{
    public:
        static Core &as();
        void clear();

        void SoundInit();
        void SoundClose();

        void CreateWindow(int width, int height, const std::string &title);
        void WindowClose();
        bool Update();
        void SetBackgroundColor(double r, double g, double b, double a);
        void Cls();

        void Begin_Drawing();
        void End_Drawing();

        void SetAudioVolume(double volume);


        u32 SoundLoad(const std::string &path);
        void cSoundPlay(u32 id);
        void cSoundStop(u32 id);
        void cSoundPause(u32 id);
        void cSoundSetVolume(u32 id, double volume);
        void cSoundSetPitch(u32 id, double pitch);
        void cSetSoundPan(u32 id, double pan);
        bool cSoundIsPlaying(u32 id);

        u32 MusicLoad(const std::string &path);
        void cMusicPlay(u32 id);
        void cMusicStop(u32 id);
        void cMusicUpdate(u32 id);
        void cMusicPause(u32 id);
        void cMusicResume(u32 id);
        void cMusicSetVolume(u32 id, double volume);
        void cMusicSetPitch(u32 id, double pitch);
        void cSetMusicPan(u32 id, double pan); 
        bool cMusicIsPlaying(u32 id);
        void cMusicSetPosition(u32 id, double position);
        double cMusicTimeLength(u32 id);
        double cMusicTimePlayed(u32 id);





        u32 ImageLoad(const std::string &path);
        void   ImageDraw(u32 id, double x, double y);
        void   ImageDrawRect(u32 id, double x, double y, double src_x, double src_y, double src_width, double src_height);


        void SetImageRotation(u32 id, double rotation);
        void SetImageScale(u32 id, double x, double y); 
        void SetImageOrigin(u32 id, double x, double y);
        void SetImageColor(u32 id, double r, double g, double b);
        void SetImageAlpha(u32 id, double a);

        double GetImageRotation(u32 id);
        double GetImageWidth(u32 id);
        double GetImageHeight(u32 id);

        void Point(double x, double y);
        void Ring(double x, double y, double innerRadius, double outerRadius, double startAngle, double endAngle, int segments, bool fill);
        void Ellipse(double x, double y, double width, double height, bool fill);
        void Circle(double x, double y, double radius,bool fill);
        void Line(double x1, double y1, double x2, double y2);
        void Rect(double x, double y, double width, double height, bool fill);
        void SetDrawColor(double r, double g, double b);
        void SetDrawAlpha(double a);
        void Text(const std::string &text, double x, double y, double size);


        u32 cLoadFont(const std::string &path);

        void  cDrawText(u32 font,const std::string &text, double x, double y);
        void  cDrawRotateText(u32 font,const std::string &text, double x, double y, double angle);
        int   GetFontTextWidth(u32 font,const std::string &text);
        void SetFontSpacing(u32 font, double spacing);
        void SetFontColor(u32 font, double r, double g, double b);
        void SetFontAlpha(u32 font, double a);
        void SetFontOrigin(u32 font, double x, double y);




    private:
        BlockArena arena;
        std::vector<CoreImage*> images;
        std::vector<CoreSound*> sounds;
        std::vector<CoreMusic*> musics;
        std::vector<CoreFont*> fonts;
        bool isReady = false;
        bool isSoundsReady = false;
        Color  backgroundColor = { 0, 0, 0, 255 };
        Color  drawColor = { 255, 255, 255, 255 };

        CoreImage *loadImage(const std::string &path);
        CoreFont *loadFont(const std::string &path);



        CoreSound * loadSound(const std::string &path);
        CoreMusic * loadMusic(const std::string &path);
        



        Core ();
        ~Core ();
    
};
