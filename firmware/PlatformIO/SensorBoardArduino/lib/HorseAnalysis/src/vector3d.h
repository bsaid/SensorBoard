#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <math.h>

class Vector3D
{
public:
    double x = 0;
    double y = 0;
    double z = 0;

    Vector3D(double x = 0, double y = 0, double z = 0)
        : x(x), y(y), z(z) {}

    double size()
    {
        return sqrt(x*x+y*y+z*z);
    }

    double axisValue(Vector3D base)
    {
        return x*base.x + y*base.y + z*base.z;
    }

    void operator+=(Vector3D v)
    {
        x += v.x; y += v.y; z += v.z;
    }

    Vector3D operator/(double i)
    {
        return Vector3D(x/i, y/i, z/i);
    }

    void operator=(double i)
    {
        x = i; y = i; z = i;
    }

    Vector3D operator-(Vector3D v)
    {
        return Vector3D(x-v.x, y-v.y, z-v.z);
    }
};

#endif // VECTOR3D_H
