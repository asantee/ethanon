#ifndef GS2D_MATH_H_
#define GS2D_MATH_H_

namespace gs2d {
namespace math {

namespace constant {
const float PI = 3.141592654f;
const float PI2 = PI * 2;
const float PIb = 1.570796327f;
} // namespace constant

class Util
{
public:
	static float RadianToDegree(const float angle);
	static float DegreeToRadian(const float angle);
    static bool IsPowerOfTwo(const unsigned int n);
    static unsigned int FindNextPowerOfTwoValue(const unsigned int n);
};

template <class T>
T Max(const T &a, const T &b)
{
    return (a > b) ? a : b;
}

template <class T>
T Min(const T &a, const T &b)
{
    return (a < b) ? a : b;
}

template <class T>
T Abs(const T &n)
{
    return (n < 0) ? -n : n;
}

template <class T>
T Clamp(const T &n, const T &min, const T &max)
{
    return (n < min) ? min : (n > max) ? max : n;
}

template <class T>
T Square(const T &a)
{
    return a*a;
}

template <class T>
T Sign(const T &n)
{
    return (n < static_cast<T>(0)) ? static_cast<T>(-1) : (n > static_cast<T>(0)) ? static_cast<T>(1) : static_cast<T>(0);
}

} // namespace math
} // namespace gs2d

#endif
