// D3D.cpp
// Direct3D setup
#include "d3d.h"
#include <string>

// Configures and initilises a DirectX renderer.
// Including render states for wireframe, alpha blending and orthographics rendering.
D3D::D3D(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	// Store rendering control variables.
	vsync_enabled = vsync;
	wireframeState = false;
	zbufferState = true;
	alphaBlendState = false;

	wnd = &hwnd;
	isFullscreen = fullscreen;
	screenheight = screenHeight;
	screenwidth = screenWidth;

	nearPlane = screenNear;
	farPlane = screenDepth;

	// Configure and create DirectX 11 renderer
	// include z buffer for 2D rendering and alpha blend state.
	createDevice();
	createSwapchain();
	createRenderTargetView();
	createDepthBuffer();
	createStencilBuffer();
	createDefaultRasterState();
	createDepthlDisableState();
	createBlendState();

}

// Create a Direct3D11 rendering device. Chooses the best gfx card available.
void D3D::createDevice()
{

	// Set the feature level to DirectX 11. Other feature levels 11.1 and 11.2 require Win8+ to support it.
	//featureLevel = D3D_FEATURE_LEVEL_11_1;
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	UINT creationFlags = NULL;
//#if defined(_DEBUG)
//	creationFlags = D3D11_CREATE_DEVICE_DEBUG;
//#endif

	// Create a list of every GFX adapter available
	//IDXGIFactory1* pFactory;
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));

	UINT i = 0;
	IDXGIAdapter1* pAdapter;
	std::vector <IDXGIAdapter1*> vAdapters;
	while (pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pAdapter);
		++i;
	}

	// Find best GFX adapter
	SIZE_T MaxSize = 0;
	for (int ap = 0; ap < vAdapters.size(); ap++)
	{
		DXGI_ADAPTER_DESC1 desc;
		pAdapter = vAdapters[ap];
		pAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if ((desc.DedicatedVideoMemory > MaxSize) && (D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, creationFlags, featureLevels, 2, D3D11_SDK_VERSION, &device, NULL, &deviceContext) == S_OK))
		{
			//pAdapter->GetDesc1(&desc);
			//printf(L"D3D11-capable hardware found:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
			std::wstring outputS = L"D3D11 capable HW: ";
			outputS += desc.Description;
			outputS += '\n';
			OutputDebugStringW(outputS.c_str());
			MaxSize = desc.DedicatedVideoMemory;
		}
	}
	
}	

// Create the swap chain.
void D3D::createSwapchain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// Initialise the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenwidth;
	swapChainDesc.BufferDesc.Height = screenheight;
	//swapChainDesc.BufferDesc.Width = 0;	// auto sizing
	//swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *wnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (isFullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// swap chain
	HRESULT h = pFactory->CreateSwapChain(device, &swapChainDesc, &swapChain);

}

// Creates the default render target view. (back buffer).
void D3D::createRenderTargetView()
{
	ID3D11Texture2D* backBufferPtr;
	// Configure back buffer
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	backBufferPtr->Release();
	backBufferPtr = 0;
}

// Creates a default depth buffer
void D3D::createDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenwidth;
	depthBufferDesc.Height = screenheight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

}

// Creates a default stencil buffer.
void D3D::createStencilBuffer()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Initialise the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

}

// Creates the default reaster state/view
void D3D::createDefaultRasterState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	device->CreateRasterizerState(&rasterDesc, &rasterState);
	deviceContext->RSSetState(rasterState);

	//create raster state with wireframe enabled
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	device->CreateRasterizerState(&rasterDesc, &rasterStateWF);

	// Setup the viewport for rendering.
	viewport.Width = (float)screenwidth;
	viewport.Height = (float)screenheight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenwidth / (float)screenheight;

	// Create the projection matrix for 3D rendering.
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, nearPlane, farPlane);

	// Initialize the world matrix to the identity matrix.
	worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	orthoMatrix = XMMatrixOrthographicLH((float)screenwidth, (float)screenheight, nearPlane, farPlane);

}

// Creates additional raster state, in this case for depth disabled 2D rendering.
void D3D::createDepthlDisableState()
{
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState);

}

// Creates additional raster state, in this case a aplha blending capable state.
void D3D::createBlendState()
{
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState);
	
	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	device->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState);
}

// Releases swap chain. Failure to do so will throw an exception.
D3D::~D3D()
{
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}

	if (alphaEnableBlendingState)
	{
		alphaEnableBlendingState->Release();
		alphaEnableBlendingState = 0;
	}

	if (alphaDisableBlendingState)
	{
		alphaDisableBlendingState->Release();
		alphaDisableBlendingState = 0;
	}

	if (depthDisabledStencilState)
	{
		depthDisabledStencilState->Release();
		depthDisabledStencilState = 0;
	}

	if (rasterState)
	{
		rasterState->Release();
		rasterState = 0;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = 0;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}

	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}
}

// Prepares the back buffer. Clears it and depth buffer, ready for rendering. Clears buffer to specified colour.
void D3D::beginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

// Present the back buffer to the screen now rendering is complete (based on vsync switch)
void D3D::endScene()
{
	if (vsync_enabled)
	{
		swapChain->Present(1, 0);
	}
	else
	{
		swapChain->Present(0, 0);
	}

	return;
}

// Get 3D device
ID3D11Device* D3D::getDevice()
{
	return device;
}


ID3D11DeviceContext* D3D::getDeviceContext()
{
	return deviceContext;
}


XMMATRIX D3D::getProjectionMatrix()
{
	return projectionMatrix;
}


XMMATRIX D3D::getWorldMatrix()
{
	return worldMatrix;
}


XMMATRIX D3D::getOrthoMatrix()
{
	return orthoMatrix;
}

// Enable/disable the ZBuffer. Uses previously created depth states.
void D3D::setZBuffer(bool b)
{
	zbufferState = b;
	if (zbufferState)
	{
		deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	}
	else
	{
		deviceContext->OMSetDepthStencilState(depthDisabledStencilState, 1);
	}
}

bool D3D::getZBufferState()
{
	return zbufferState;
}

// Sets the blending state, to enable/disable alphablending
void D3D::setAlphaBlending(bool b)
{
	alphaBlendState = b;

	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	if (alphaBlendState)
	{
		// Turn on the alpha blending.
		deviceContext->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);
	}
	else
	{
		// Turn off the alpha blending.
		deviceContext->OMSetBlendState(alphaDisableBlendingState, blendFactor, 0xffffffff);
	}
}

bool D3D::getAlphaBlendingState()
{
	return alphaBlendState;
}

// Set the back buffer as the render target
void D3D::setBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	return;
}

// Your initialise will create a local viewport variable, and you can swap it to this one
void D3D::resetViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
	return;
}

// Enable/disable wireframe rendering. Uses previously created raster states.
void D3D::setWireframeMode(bool b)
{
	wireframeState = b;
	if (wireframeState)
	{
		deviceContext->RSSetState(rasterStateWF);
	}
	else
	{
		deviceContext->RSSetState(rasterState);
	}
}

bool D3D::getWireframeState() 
{
	return wireframeState;
}