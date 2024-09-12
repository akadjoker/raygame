#include "pch.h"

#include "core.hpp"
#include "Interpreter.hpp" 






namespace Math
{

    int sign(double value) { return value < 0 ? -1 : (value > 0 ? 1 : 0); }

    static const double rand_scale = 1.0 / (1 << 16) / (1 << 16);
    double range()
    {
        unsigned int lo = rand() & 0xfff;
        unsigned int mid = rand() & 0xfff;
        unsigned int hi = rand() & 0xff;
        double result = (lo | (mid << 12) | (hi << 24)) * rand_scale;
        return result;
    }
    double range(double min, double max)
    {

        unsigned int lo = rand() & 0xfff;  // 12 bits
        unsigned int mid = rand() & 0xfff; // 12 bits
        unsigned int hi = rand() & 0xff;   // 8 bits

        double normalized = (lo | (mid << 12) | (hi << 24)) * rand_scale;

        return min + normalized * (max - min);
    }

    double clip(double value, double min, double max)
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    }

    double clamp(double value, double min, double max)
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    }

    double lerp(double a, double b, double t)
    {
        return a + t * (b - a);
    }

    double normalize_angle(double angle)
    {
        while (angle > 360)
            angle -= 360;
        while (angle < 0)
            angle += 360;
        return angle;
    }

    double clamp_angle(double angle, double min, double max)
    {

        angle = normalize_angle(angle);
        if (angle > 180)
        {
            angle -= 360;
        }
        else if (angle < -180)
        {
            angle += 360;
        }

        min = normalize_angle(min);
        if (min > 180)
        {
            min -= 360;
        }
        else if (min < -180)
        {
            min += 360;
        }

        max = normalize_angle(max);
        if (max > 180)
        {
            max -= 360;
        }
        else if (max < -180)
        {
            max += 360;
        }

        return clamp(angle, min, max);
    }

    double hermite(double value1, double tangent1, double value2, double tangent2, double amount)
    {
        double v1 = value1;
        double v2 = value2;
        double t1 = tangent1;
        double t2 = tangent2;
        double s = amount;
        double result;
        double sCubed = s * s * s;
        double sSquared = s * s;

        if (amount == 0)
            result = value1;
        else if (amount == 1)
            result = value2;
        else
            result = (2 * v1 - 2 * v2 + t2 + t1) * sCubed +
                     (3 * v2 - 3 * v1 - 2 * t1 - t2) * sSquared +
                     t1 * s +
                     v1;
        return result;
    }

    double smooth_step(double value1, double value2, double amount)
    {

        double result = clamp(amount, 0, 1);
        result = hermite(value1, 0, value2, 0, result);

        return result;
    }

    double repeat(double t, double length)
    {
        return clamp(t - floor(t / length) * length, 0.0f, length);
    }
    double ping_pong(double t, double length)
    {
        t = repeat(t, length * 2.0f);
        return length - abs(t - length);
    }

    double get_distx(double a, double d)
    {
        double angulo = a * RAD;
        return ((double)(cos(angulo) * d));
    }

    double get_disty(double a, double d)
    {
        double angulo = (double)a * RAD;
        return (-(double)(sin(angulo) * d));
    }

    static const double dtor = 0.0174532925199432957692369076848861f;
    static const double rtod = 57.2957795130823208767981548141052f;
    static int rnd_state = 0x1234;
    static const int RND_A = 48271;
    static const int RND_M = 2147483647;
    static const int RND_Q = 44488;
    static const int RND_R = 3399;

    double Sin(double n) { return sin(n * dtor); }
    double Cos(double n) { return cos(n * dtor); }
    double Tan(double n) { return tan(n * dtor); }
    double ASin(double n) { return asin(n) * rtod; }
    double ACos(double n) { return acos(n) * rtod; }
    double ATan(double n) { return atan(n) * rtod; }
    double ATan2(double n, double t) { return atan2(n, t) * rtod; }
    double Sqr(double n) { return sqrt(n); }
    double Floor(double n) { return floor(n); }
    double Ceil(double n) { return ceil(n); }
    double Exp(double n) { return exp(n); }
    double Log(double n) { return log(n); }
    double Log10(double n) { return log10(n); }

#define FLT_MIN 1.175494351e-38

    static double rnd()
    {
        rnd_state = RND_A * (rnd_state % RND_Q) - RND_R * (rnd_state / RND_Q);
        if (rnd_state < 0)
            rnd_state += RND_M;

        return (rnd_state & 0xffffff0) / 268435456.0; // divide by 2^28
    }

    static double rnd2()
    {
        const double TWO27 = 134217728.0; //  2^27
        const double TWO29 = 536870912.0;

        double rHi, rLo; // two 'halves' of the fraction

        rnd_state = RND_A * (rnd_state % RND_Q) - RND_R * (rnd_state / RND_Q);
        if (rnd_state < 0)
            rnd_state += RND_M;

        rHi = (double)(rnd_state & 0x1ffffffc); // 27 bits

        rnd_state = RND_A * (rnd_state % RND_Q) - RND_R * (rnd_state / RND_Q);
        if (rnd_state < 0)
            rnd_state += RND_M;

        rLo = (double)(rnd_state & 0x1ffffff8); // 26 bits

        return (rHi + rLo / TWO27) / TWO29; // 27+29 = 53 + 3
    }

    double Random(double from, double to)
    {
        return rnd() * (to - from) + from;
    }

    double Random()
    {
        return rnd();
    }


    void SeedRnd(int seed)
    {
        seed &= 0x7fffffff;
        if (seed == 0 || seed == RND_M)
            seed = 0x1234;
        rnd_state = seed;
    }

    int RndSeed()
    {
        return rnd_state;
    }

} // namespace Math

ExprPtr native_rand_seed(Context* ctx, int argc)
{
    if (argc != 1)
    {
        ERROR("Seed requires 1 argument");
        return ctx->asBoolean(false);
    }
    int seed = ctx->getInt(0);
    Math::SeedRnd(seed);
    return ctx->asNil();
}


ExprPtr native_rand(Context* ctx, int argc)
{
     if (argc != 0 && argc != 2)
    {
        ERROR("Rnd requires 0 or (min/max) arguments");
        return ctx->asDouble(0);
    }

    if (argc == 0)
    {
        double value = ctx->getDouble(0);

        return ctx->asDouble(Math::Random());
    }
    if (argc == 2)
    {
        double min = ctx->getDouble(0);
        double max = ctx->getDouble(1);
        return ctx->asDouble(Math::Random(min, max));
    }
   
    return ctx->asDouble(0);    
}




#define REGISTER_MATH_FUNCTION(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 1) { \
            ERROR(name " requires 1 argument"); \
            return ctx->asBoolean(false); \
        } \
        double n = ctx->getDouble(0); \
        return ctx->asDouble(func(n)); \
    });

#define REGISTER_MATH_FUNCTION2(interpreter, name, func) \
    interpreter.registerFunction(name, [](Context* ctx, int argc) -> ExprPtr { \
        if (argc != 2) { \
            ERROR(name " requires 2 arguments"); \
            return ctx->asBoolean(false); \
        } \
        double n1 = ctx->getDouble(0); \
        double n2 = ctx->getDouble(1); \
        return ctx->asDouble(func(n1, n2)); \
    });

#define REGISTER_MATH_FUNCTION3(interpreter, name, func) \
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

void RegisterMath(Interpreter& interpreter)
{
    
    REGISTER_MATH_FUNCTION(interpreter, "Sin", Math::Sin);
    REGISTER_MATH_FUNCTION(interpreter, "Cos", Math::Cos);
    REGISTER_MATH_FUNCTION(interpreter, "Tan", Math::Tan);
    REGISTER_MATH_FUNCTION(interpreter, "ASin", Math::ASin);
    REGISTER_MATH_FUNCTION(interpreter, "ACos", Math::ACos);
    REGISTER_MATH_FUNCTION(interpreter, "ATan", Math::ATan);
    REGISTER_MATH_FUNCTION2(interpreter, "ATan2", Math::ATan2);
    REGISTER_MATH_FUNCTION(interpreter, "Sqr", Math::Sqr);
    REGISTER_MATH_FUNCTION(interpreter, "Floor", Math::Floor);
    REGISTER_MATH_FUNCTION(interpreter, "Ceil", Math::Ceil);
    REGISTER_MATH_FUNCTION(interpreter, "Exp", Math::Exp);
    REGISTER_MATH_FUNCTION(interpreter, "Log", Math::Log);
    REGISTER_MATH_FUNCTION(interpreter, "Log10", Math::Log10);
    REGISTER_MATH_FUNCTION(interpreter, "NormalizeAngle", Math::normalize_angle);
  


    REGISTER_MATH_FUNCTION2(interpreter, "Repeat", Math::repeat);
    REGISTER_MATH_FUNCTION2(interpreter, "PingPong", Math::ping_pong);
    
    REGISTER_MATH_FUNCTION2(interpreter, "GetDistX", Math::get_distx);
    REGISTER_MATH_FUNCTION2(interpreter, "GetDistY", Math::get_disty);
    

    REGISTER_MATH_FUNCTION3(interpreter, "Clip", Math::clip);
    REGISTER_MATH_FUNCTION3(interpreter, "Clamp", Math::clamp);
    REGISTER_MATH_FUNCTION3(interpreter, "Lerp", Math::lerp);
    REGISTER_MATH_FUNCTION3(interpreter, "ClampAngle", Math::clamp_angle);
    REGISTER_MATH_FUNCTION3(interpreter, "SmoothStep", Math::smooth_step);


    interpreter.registerFunction("Rnd",  native_rand);
    interpreter.registerFunction("Seed", native_rand_seed);
    interpreter.registerDouble("PI", PI);
    interpreter.registerDouble("RAD", RAD);
    interpreter.registerDouble("DEG", DEG);
    
    

    

   

}