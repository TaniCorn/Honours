/**
* \class Orthodgraphic mesh
*
* \brief Quad mesh designed for 2D Orthographic rendering
*
* Added functionality to be position with onscreen coordinates.
* While normals are define for shape, they are not expected to be used.
*
* \author Paul Robertson
*/

#ifndef _ORTHOMESH_H_
#define _ORTHOMESH_H_

#include "basemesh.h"

using namespace DirectX;

class OrthoMesh : public BaseMesh
{

public:
	/** \brief Initialises the mesh and vertex list, requires size and position values
	* Provide filename to OBJ object, will be loaded and store like other mesh objects.
	* @param device is the renderer device
	* @param device context is the renderer device context
	* @param width is the required width in pixels
	* @param height is the required height in pixels
	* @param x position is the x-axis offset, default is zero for centre screen
	* @param y position is the y-axis offset, default is zero for centre screen
	*/
	OrthoMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, int xPosition = 0, int yPosition = 0);
	~OrthoMesh();

protected:
	void initBuffers(ID3D11Device*);
	int width, height, xPosition, yPosition;
};

#endif