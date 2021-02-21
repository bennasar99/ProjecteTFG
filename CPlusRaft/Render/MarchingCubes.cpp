#include "MarchingCubes.h"

float GetOffset(float v1, float v2)
{
    float surface = 1;
    float delta = v2 - v1;
    return (delta == 0.0f) ? surface : (surface - v1) / delta;
};

bool MarchingCubes::apply(float surface, std::array<float, 8>& density, std::vector< Vector3<float> > &vertexs, std::vector< Vector3<float> > &normals) {
    unsigned char bitfield = {};
	for (int i = 0; i < 8; i++) {
		if (density[i] < surface) {
			bitfield |= 1UL << i;
		}
	}
    //printf("%d\n", int(bitfield));

    int edgeFlags = cubeEdgeFlags[bitfield];

    //If the cube is entirely inside or outside of the surface, then there will be no intersections
    if (edgeFlags == 0) return false;

    Vector3<float> EdgeVertex[12];
    //Find the point of intersection of the surface with each edge
    for (int i = 0; i < 12; i++)
    {
        //if there is an intersection on this edge
        if ((edgeFlags & (1 << i)) != 0)
        {
            float offset = GetOffset(density[EdgeConnection[i][0]], density[EdgeConnection[i][1]]);

            EdgeVertex[i].x = toCheck[EdgeConnection[i][0]][0] + offset * EdgeDirection[i][0];
            EdgeVertex[i].y = toCheck[EdgeConnection[i][0]][1] + offset * EdgeDirection[i][1];
            EdgeVertex[i].z = toCheck[EdgeConnection[i][0]][2] + offset * EdgeDirection[i][2];

        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (triangleTable[bitfield][3 * i] < 0) break;

        Vector3<float> triangle[3];
        for (int j = 0; j < 3; j++)
        {
            int vert = triangleTable[bitfield][3 * i + j];
            triangle[j] = EdgeVertex[vert];
            vertexs.push_back(EdgeVertex[vert]);
        }
        //Solució alternativa a decidir tipus bloc: mirar centre de cada triangle i bloc sòlid més proper a aquest
        Vector3<float> a = triangle[1] - triangle[0];
        Vector3<float> b = triangle[2] - triangle[0];
        Vector3<float> normal = Vector3<float>::normalize(Vector3<float>::cross(a, b));
        normals.push_back(normal);
    }
}