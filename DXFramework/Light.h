/**
* \class Light source
*
* \brief Represents a single light source
*
* Stores ambient, diffuse, specular colour, specular power. Also stores direction and position
* Additionally, generates view, projectiong and orthographics matrices for use in shadow mapping.
*
* \author Paul Robertson
*/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

using namespace DirectX;

class Light
{

public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void generateViewMatrix();			///< Generates and upto date view matrix, based on current rotation
	void generateProjectionMatrix(float screenNear, float screenFar);			///< Generate project matrix based on current rotation and provided near & far plane
	void generateOrthoMatrix(float screenWidth, float screenHeight, float near, float far);		///< Generates orthographic matrix based on supplied screen dimensions and near & far plane.

	// Setters
	void setAmbientColour(float red, float green, float blue, float alpha);		///< Set ambient colour RGBA
	void setDiffuseColour(float red, float green, float blue, float alpha);		///< Set diffuse colour RGBA
	void setDirection(float x, float y, float z);								///< Set light direction (for directional lights)
	void setSpecularColour(float red, float green, float blue, float alpha);	///< set specular colour RGBA
	void setSpecularPower(float power);											///< Set specular power
	void setPosition(float x, float y, float z);								///< Set light position (for point lights)
	void setLookAt(float x, float y, float z);									///< Set light lookAt (near deprecation)

	// Getters
	XMFLOAT4 getAmbientColour();		///< Get ambient colour, returns float4
	XMFLOAT4 getDiffuseColour();		///< Get diffuse colour, returns float4
	XMFLOAT3 getDirection();			///< Get light direction, returns float3
	XMFLOAT4 getSpecularColour();		///< Get specular colour, returns float4
	float getSpecularPower();			///< Get specular power, returns float
	XMFLOAT3 getPosition();				///< Get light position, returns float3
	XMMATRIX getViewMatrix();			///< Get light view matrix for shadow mapping, returns XMMATRIX
	XMMATRIX getProjectionMatrix();		///< Get light projection matrix for shadow mapping, returns XMMATRIX
	XMMATRIX getOrthoMatrix();			///< Get light orthographic matrix for shadow mapping, returns XMMATRIX


protected:
	XMFLOAT4 ambientColour;
	XMFLOAT4 diffuseColour;
	XMFLOAT3 direction;
	XMFLOAT4 specularColour;
	float specularPower;
	XMVECTOR position;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX orthoMatrix;
	XMVECTOR lookAt; 
};

#endif