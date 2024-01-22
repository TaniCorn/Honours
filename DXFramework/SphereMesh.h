// Sphere Mesh
// Generated sphere mesh with texture coordinates and normals.
// Uses the cube sphere normalisation method. First a cube is generated,
// then the vertices are normalised creating a sphere.
// Resolution specifies the number of segments in the sphere (top and bottom, matches equator).

#ifndef _SPHEREMESH_H_
#define _SPHEREMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class SphereMesh : public BaseMesh
{

public:
	SphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~SphereMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

#endif