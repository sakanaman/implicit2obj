#include "marching_cube.hpp"

#include <fstream>

namespace mc
{
void Mesh::add_triangle(const iVec3& ivec)
{
    triangles.push_back(ivec);
}

void Mesh::add_vertex(const Vec3& v)
{
    vertices.push_back(v);
}

size_t Mesh::vertex_size() const
{
    return vertices.size();
}

void Mesh::write_obj(const std::string& filename) const
{
    std::ofstream file(filename);

    //vertex
    for(auto v : vertices)
    {
        file << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    }

    //face(without uv-coordinate and vertex-normal)
    for(auto f : triangles)
    {
        file << "f " << f[0] + 1 << " " << f[1] + 1 << " " << f[2] + 1 << std::endl;
    }

    file.close();
}
} // namespace mc
