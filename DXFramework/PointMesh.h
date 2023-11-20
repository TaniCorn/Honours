// PointMesh.h
// A simple mesh for demostrating the geometry shader
// Instead producing a triangle list, produce a point list.
// This point list is three points of a triangle and can be used by the geometry shader to generate geometry.
// Therefore has a modified sendData() function.

#ifndef _POINTMESH_H_
#define _POINTMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class PointMesh : public BaseMesh
{

public:
	PointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~PointMesh();

	//void sendData(ID3D11DeviceContext*);
	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST) override;

protected:
	void initBuffers(ID3D11Device* device);

};

#endif