// Orthographic mesh
// 2D quad mesh for post processing, should render a quad to match window size

#include "orthomesh.h"

// Store geometry dimensions, initialise buffers and loadTexture (null as texture is provided from a rendertarget).
OrthoMesh::OrthoMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lwidth, int lheight, int lxPosition, int lyPosition)
{
	width = lwidth;
	height = lheight;
	xPosition = lxPosition;
	yPosition = lyPosition;

	initBuffers(device);
}

// Release resources
OrthoMesh::~OrthoMesh()
{
	BaseMesh::~BaseMesh();
}

// Based on provide dimensions and position, generate quad for orthographics rendering.
void OrthoMesh::initBuffers(ID3D11Device* device)
{
	float left, right, top, bottom;
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Calculate the screen coordinates of the left side of the window.
	left = (float)((width / 2) * -1) + xPosition;

	// Calculate the screen coordinates of the right side of the window.
	right = left + (float)width;

	// Calculate the screen coordinates of the top of the window.
	top = (float)(height / 2) + yPosition;

	// Calculate the screen coordinates of the bottom of the window.
	bottom = top - (float)height;

	vertexCount = 6;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	
	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[3].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 2;  // Top right.
	indices[2] = 1;  // Top left.

	indices[3] = 0;	// bottom left
	indices[4] = 3;	// bottom right
	indices[5] = 2;	// top right

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	
	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}
