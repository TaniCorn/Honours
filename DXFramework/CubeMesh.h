/**
* \class Cube Mesh
*
* \brief Simple cube mesh object
*
* Inherits from Base Mesh, Builds a simple cube with texture coordinates and normals.
*
* \author Paul Robertson
*/

#ifndef _CUBEMESH_H_
#define _CUBEMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class CubeMesh : public BaseMesh
{

public:
	/** \brief Initialises and builds a cube mesh
	*
	* Can specify resolution of cube, this deteremines how many subdivisions are on each side of the cube.
	* @param device is the renderer device
	* @param device context is the renderer device context
	* @param resolution is a int for subdivision of the cube. Default is 20.
	*/
	CubeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~CubeMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

#endif