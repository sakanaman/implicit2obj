#include "marching_cube.hpp"

#include <cmath>
#include <iostream>
#include <bitset>

namespace mc
{

MarchingCube::MarchingCube(const Accessor& _acc, const iVec3& _dim, const Vec3& _origin,
                           const Vec3& _gridsize, double _isovalue)
: acc(_acc), dim(_dim), gridsize(_gridsize), isovalue(_isovalue), origin(_origin)
{}

size_t globalid_edge(size_t i, size_t j, size_t k, 
                     const iVec3& localid, const iVec3& dim)
{
    return    (2 * k + localid[2]) * 4 * dim[0] * dim[1]
            + (2 * j + localid[1]) * 2 * dim[0]
            + (2 * i + localid[0]);
}

void eval_inside_cube(const double* data, const double isovalue, int* insidebits)
{
    for(size_t i = 0; i < 8; ++i)
    {
        if(data[i] <= isovalue) *insidebits |= (1 << i);
    }
}

// :NOTATION: 
//  I use y-up & right-hand coordinate system.
//  If you wanna use left-hand, swap the factor of vertex.  
void MarchingCube::compute()
{
    auto pos_x = [&](int i, int j, int k)
    {
        return origin[0] + i * gridsize[0];
    };
    auto pos_y = [&](int i, int j, int k)
    {
        return origin[1] + j * gridsize[1];
    };
    auto pos_z = [&](int i, int j, int k)
    {
        return origin[2] + k * gridsize[2];
    };



    size_t dimX = dim[0];
    size_t dimY = dim[1];
    size_t dimZ = dim[2];

    VertexMap vmap;


    // for each voxel
    for(size_t k = 0; k < dimZ - 1; ++k)
    {
        for(size_t j = 0; j < dimY - 1; ++j)
        {
            for(size_t i = 0; i < dimX - 1; ++i)
            {
                double data[8] = {//lower
                                  acc(i, j, k),
                                  acc(i+1, j, k),
                                  acc(i+1, j, k+1),
                                  acc(i, j, k+1),
                                  //upper
                                  acc(i, j+1, k),
                                  acc(i+1, j+1, k),
                                  acc(i+1, j+1, k+1),
                                  acc(i, j+1, k+1)
                                 };

                size_t globaledgeID[12] = {//lower
                                            globalid_edge(i, j, k, {1, 0, 0}, dim),
                                            globalid_edge(i, j, k, {2, 0, 1}, dim),
                                            globalid_edge(i, j, k, {1, 0, 2}, dim),
                                            globalid_edge(i, j, k, {0, 0, 1}, dim),

                                            //upper
                                            globalid_edge(i, j, k, {1, 2, 0}, dim),
                                            globalid_edge(i, j, k, {2, 2, 1}, dim),
                                            globalid_edge(i, j, k, {1, 2, 2}, dim),
                                            globalid_edge(i, j, k, {0, 2, 1}, dim),

                                            //middle
                                            globalid_edge(i, j, k, {0, 1, 0}, dim),
                                            globalid_edge(i, j, k, {2, 1, 0}, dim),
                                            globalid_edge(i, j, k, {2, 1, 2}, dim),
                                            globalid_edge(i, j, k, {0, 1, 2}, dim)
                                          };

                double voxel_pos[8][3] = {//lower
                                          {pos_x(i,j,k),       pos_y(i,j,k),       pos_z(i,j,k)},
                                          {pos_x(i+1, j, k),   pos_y(i+1, j, k),   pos_z(i+1, j, k)},
                                          {pos_x(i+1, j, k+1), pos_y(i+1, j, k+1), pos_z(i+1, j, k+1)},
                                          {pos_x(i, j, k+1),   pos_y(i, j, k+1),   pos_z(i, j, k+1)},
                                          
                                          //upper
                                          {pos_x(i, j+1, k),     pos_y(i, j+1, k),     pos_z(i, j+1, k)},
                                          {pos_x(i+1, j+1, k),   pos_y(i+1, j+1, k),   pos_z(i+1, j+1, k)},
                                          {pos_x(i+1, j+1, k+1), pos_y(i+1, j+1, k+1), pos_z(i+1, j+1, k+1)},
                                          {pos_x(i, j+1, k+1),   pos_y(i, j+1, k+1),   pos_z(i, j+1, k+1)}
                                          };


                // evaluate whether voxel's each vertex is inside or outside.
                int insidebits = 0;
                eval_inside_cube(data, isovalue, &insidebits);

                // int edgeType = jet::cubeEdgeFlags[insidebits];

                // cube is entirely inside or outside.
                if(insidebits == 0 || insidebits == 255) continue;


                // make triangle
                for(size_t tri = 0; tri < 5; ++tri)
                {
                    if(jet::triangleConnectionTable3D[insidebits][3 * tri] == -1) break;

                    iVec3 v_index;
                    for(size_t v = 0; v < 3; ++v)
                    {
                        // which edge intersect?
                        int point = jet::triangleConnectionTable3D[insidebits][3 * tri + v];
                        
                        size_t globalID = globaledgeID[point];

                        auto v_iter = vmap.find(globalID);

                        //duplication of vertex
                        if(v_iter !=  vmap.end())
                        {
                            v_index[v] = v_iter->second;
                            continue;
                        }

                        //coordinate:    pos0      p       pos1
                        //                |--------x--------|
                        //     index:   end0               end1
                        int end0 = jet::edgeConnection[point][0];
                        int end1 = jet::edgeConnection[point][1];

                        // calculate coefficient of interpolation
                        double phi0 = std::abs(data[end0] - isovalue);
                        double phi1 = std::abs(data[end1] - isovalue);
                        double alpha = phi0 / (phi0 + phi1);


                        double pos0[3] = {voxel_pos[end0][0],
                                          voxel_pos[end0][1],
                                          voxel_pos[end0][2]
                                          };
                        double pos1[3] = {voxel_pos[end1][0],
                                          voxel_pos[end1][1],
                                          voxel_pos[end1][2]
                                          };

                        // calculate vertex by interpolation
                        double px = (1 - alpha) * pos0[0] + alpha * pos1[0];
                        double py = (1 - alpha) * pos0[1] + alpha * pos1[1];
                        double pz = (1 - alpha) * pos0[2] + alpha * pos1[2];


                        size_t vsize = meshdata.vertex_size();
                        vmap.insert(std::make_pair(globalID, vsize));
                        meshdata.add_vertex({px, py, pz});
                        v_index[v] = vsize;
                    }
                    meshdata.add_triangle(v_index);
                }

            }//i
        }//j
    }//k
}


void MarchingCube::write_obj(const std::string& filename) const
{
    meshdata.write_obj(filename);
}

} // namespace mc
