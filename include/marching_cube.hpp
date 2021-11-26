// This code is based on
// - "https://github.com/doyubkim/fluid-engine-dev"
// - "https://fluidenginedevelopment.org/"


#ifndef __MARCHING_CUBE_HPP__
#define __MARCHING_CUBE_HPP__

#include "marching_cubes_table.h"
#include "marching_squares_table.h"

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

namespace mc
{

using Vec3 = std::array<double, 3>;
using iVec3 = std::array<size_t, 3>;

// function : (i, j, k) |--> griddata at (i, j, k)
using Accessor = std::function<double(size_t, size_t, size_t)>;

// hash map for preventing from vertex duplication
using VertexMap = std::unordered_map<size_t, size_t>;

class Mesh
{
public:
    Mesh(){};
    // void add_normal();
    void add_triangle(const iVec3& i_tri);
    void add_vertex(const Vec3& v);
    size_t vertex_size() const;
    void write_obj(const std::string& filename) const;
private:
    std::vector<Vec3> normals;
    std::vector<Vec3> vertices;
    std::vector<iVec3> triangles;
};

class MarchingCube
{
public:
    MarchingCube(const Accessor& _acc, const iVec3& _dim, const Vec3& _origin,
                 const Vec3& _gridsize, double isovalue);
    void compute();
    void write_obj(const std::string& filename) const;
private:
    Accessor acc;
    iVec3 dim;
    Vec3 origin;
    Vec3 gridsize;
    double isovalue;
    Mesh meshdata;
};

} // namespace mc

#endif
