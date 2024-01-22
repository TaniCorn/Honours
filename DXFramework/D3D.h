/**
* \class Direct 3D renderer
*
* \brief Defines and initialises a DirectX 11 renderer
*
* Creates a DX11 renderer, creating the required depth and stencil buffers.
* Additionally, creating render states for alpha blended rendering.
* Provided functions for controlling begin/end frame rendering, wireframe rendering, orthographic and alpha blended rendering.
* Member variables for projection, ortho and identity world matrices.
*
* \author Paul Robertson
*/


#ifndef _D3D_H_
#define _D3D_H_

// Includes
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <dxgi.h>
#include <string>
//#include <winerror.h>

using namespace DirectX;

class D3D
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
	/** \brief Creates and initialises a DX11 renderer
	* @param screenWidth
	* @param screenHeight
	* @param vsync is a boolean for if vsync is dis/enabled
	* @param hwnd is a handle to the window, that the renderer will sit inside
	* @param fullscreen is a boolean if fullscreen is dis/enabled
	* @param screenDepth is the distance of the far plane for projection matrix generation
	* @param screenNear is the near plane for projection matrix generation
	*/
	D3D(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	~D3D();

	/// Begin rendering frame, set background colour
	void beginScene(float r, float g, float b, float a);
	/// end scene rendering, do frame buffer swap
	void endScene();

	ID3D11Device* getDevice();	///< Returns render device
	ID3D11DeviceContext* getDeviceContext(); ///< Returns renderer device context

	XMMATRIX getProjectionMatrix();	///< Returns default projection matrix
	XMMATRIX getWorldMatrix();		///< Returns identity world matrix
	XMMATRIX getOrthoMatrix();		///< Returns default orthographic matrix

	// Control render states
	void setZBuffer(bool b);	///< Sets z-buffer on/off for orthographic rendering
	bool getZBufferState();		///< Returns if the z-buffer is on/off


	void setAlphaBlending(bool b);	///< Sets the alpha blending state on/off for transparent rendering
	bool getAlphaBlendingState();	///< Returns alphab blending state, if on/off
	
	void setWireframeMode(bool b);	///< Set wireframe render mode on/off
	bool getWireframeState();		///< Returns currect wireframe state on/off

	void setBackBufferRenderTarget();	///< Sets the back buffer as the render target
	void resetViewport();				///< Restores viewport if dimensions of render target were different

private:
	void createDevice();
	void createSwapchain();
	void createRenderTargetView();
	void createDepthBuffer();
	void createStencilBuffer();
	void createDefaultRasterState();
	void createDepthlDisableState();
	void createBlendState();


protected:
	bool vsync_enabled;	
	bool isWirefameEnabled;
	
	bool zbufferState;		///< Variable tracks z-buffer state
	bool wireframeState;	///< Variable tracks wireframe state
	bool alphaBlendState;	///< Variable tracks alpha blending state

	bool isFullscreen;
	HWND* wnd;
	int screenheight;
	int screenwidth;
	float fieldOfView;
	float screenAspect;
	float nearPlane;
	float farPlane;

	IDXGIFactory1* pFactory;
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;	///< Default render target
	ID3D11Texture2D* depthStencilBuffer;		///< Depth and stencil buffer
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;			///< Default FILL raster state
	ID3D11RasterizerState* rasterStateWF;		///< Wireframe raster state
	XMMATRIX projectionMatrix;					///< Identity projection matrix
	XMMATRIX worldMatrix;						///< Identity world matrix
	XMMATRIX orthoMatrix;						///< Identity orthographic matrix
	ID3D11DepthStencilState* depthDisabledStencilState;
	ID3D11BlendState* alphaEnableBlendingState;	///< Alpha blend enabled state
	ID3D11BlendState* alphaDisableBlendingState;///< Alpha blend disabled state
	D3D11_VIEWPORT viewport;					///< Default viewport object
};

#endif