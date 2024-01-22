// TriangleMesh.h
// Simple single triangle mesh (for example purposes).
// Mesh contains texture coordinates and normals.

#ifndef _TRIMESH_H_
#define _TRIMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class TriangleMesh : public BaseMesh
{

public:
	TriangleMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TriangleMesh();

protected:
	void initBuffers(ID3D11Device* device);
	
};

#endif