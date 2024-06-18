#include <string>
#include <sstream>
#include <math.h>

template <typename T>
class Vec2
{
public:
    Vec2() : x(0), y(0) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    
    T dot(Vec2<T> vec) const { return (this->x * vec.x) + (this->y * vec.y); }

    double magnitude() const 
    {
        double X = (double)(this->x);
        double Y = (double)(this->y);

        return sqrt(X*X + Y*Y);
    }

    // Unit Vector version of this
    Vec2<T> unit() const { return (*this) / this->magnitude(); }

    Vec2<T> operator+ (const Vec2<T> vec) const { return Vec2<T>(this->x + vec.x, this->y + vec.y); }
    Vec2<T> operator- (const Vec2<T> vec) const { return Vec2<T>(this->x - vec.x, this->y - vec.y); }
    Vec2<T> operator* (const T scalar) const { return Vec2<T>(this->x * scalar, this->y * scalar); }
    Vec2<T> operator/ (const T scalar) const { return Vec2<T>(this->x / scalar, this->y / scalar); }

    const std::string str() const 
    {
        std::ostringstream oss;
        oss << "<" << this->x << ", " << this->y << ">";
        return oss.str();
    }

    T x, y;
};
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

template <typename T>
class Vec3
{
public:
    Vec3() : x(0), y(0), z(0) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    
    T dot(Vec3<T> vec) const { return (this->x * vec.x) + (this->y * vec.y) + (this->z * vec.z); }
    
    // this crossed with vec
    Vec3<T> cross(Vec3<T> vec) const { return Vec3<T>(this->y * vec.z - vec.y * this->z, this->z * vec.x - this->x * vec.z, this->x * vec.y - vec.x * this->y); }

    double magnitude() const 
    {
        double X = (double)(this->x);
        double Y = (double)(this->y);
        double Z = (double)(this->z);

        return sqrt(X*X + Y*Y + Z*Z);
    }

    // Unit Vector version of this
    Vec3<T> unit() const { return (*this) / this->magnitude(); }

    Vec3<T> operator+ (const Vec2<T> vec) const { return Vec2<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z); }
    Vec3<T> operator- (const Vec2<T> vec) const { return Vec2<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z); }
    Vec3<T> operator* (const T scalar) const { return Vec2<T>(this->x * scalar, this->y * scalar, this->z * scalar); }
    Vec3<T> operator/ (const T scalar) const { return Vec2<T>(this->x / scalar, this->y / scalar, this->z / scalar); }

    const std::string str() const 
    {
        std::ostringstream oss;
        oss << "<" << this->x << ", " << this->y << ", " << this->z << ">";
        return oss.str();
    }

    T x, y, z;
};
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;
