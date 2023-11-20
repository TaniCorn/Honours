/**
* \class Base Shader
*
* \brief Loads shaders CSOs and sets parameters
*
* Base shader class to be inherited. Provides default functionality of loading and sending to GPU
* TODO: Add compute shader to set
* Base shader is the parent for other custom shader objects. Offers required functions and a standard Matrix buffer.
* 
* \author Paul Robertson
*/


#ifndef _BASESHADER_H_
#define _BASESHADER_H_

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <fstream>
#include "imGUI/imgui.h"

using namespace std;
using namespace DirectX;


class BaseShader
{
protected:
	/** Default world, view, projection matrix buffer struct */
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	BaseShader(ID3D11Device* device, HWND hwnd);
	~BaseShader();

	/** \Brief render function
	* Sets shader stages and draws the indexed data
	*/
	virtual void render(ID3D11DeviceContext* deviceContext, int vertexCount);
	void compute(ID3D11DeviceContext* dc, int x, int y, int z);

protected:
	virtual void initShader(const wchar_t*, const wchar_t*) = 0;
	void loadVertexShader(const wchar_t* filename);		///< Load Vertex shader, for stand position, tex, normal geomtry
	void loadColourVertexShader(const wchar_t* filename);		///< Load Vertex shader, pre-made for position and colour only
	void loadTextureVertexShader(const wchar_t* filename);		///< Load Vertex shader, pre-made for position and tex only
	void loadHullShader(const wchar_t* filename);		///< Load Hull shader
	void loadDomainShader(const wchar_t* filename);		///< Load Domain shader
	void loadGeometryShader(const wchar_t* filename);	///< Load Geometry shader
	void loadPixelShader(const wchar_t* filename);		///< Load Pixel shader
	void loadComputeShader(const wchar_t* filename);	///< Load computer shader

protected:
	ID3D11Device* renderer;
	HWND hwnd;
	
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11ComputeShader* computeShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

#endif