#include "marching_cube.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

int main()
{
    double isovalue = 0.0;
    double radius = M_PI/4;
    int n = 512;
    double L = 2.0 * M_PI;
    double h = L/n;
    mc::Vec3 gridsize{h, h, h};
    mc::Vec3 origin{-M_PI, -M_PI, -M_PI};
    mc::iVec3 dim{n, n, n};

    auto imp = [&](double x, double y, double z)
    {
        // return x*x + y*y + z*z - radius*radius;
        return sin(x)*sin(y)*sin(z);
    };

    auto acc = [&](int i, int j, int k)
    {
        double x = origin[0] + gridsize[0] * i;
        double y = origin[1] + gridsize[1] * j;
        double z = origin[2] + gridsize[2] * k;

        return imp(x, y, z);
    };

    mc::MarchingCube march(acc, dim, origin, gridsize, isovalue);
    march.compute();
    march.write_obj("output.obj");
}