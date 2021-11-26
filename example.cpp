#include "marching_cube.hpp"

int main()
{
    double isovalue = 0.0;
    double radius = 0.3;
    mc::Vec3 gridsize{0.02, 0.02, 0.02};
    mc::Vec3 origin{-1.0, -1.0, -1.0};
    mc::iVec3 dim{100, 100, 100};

    auto imp = [&](double x, double y, double z)
    {
        return x*x + y*y + z*z - radius*radius;
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