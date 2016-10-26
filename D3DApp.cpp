#include "D3DApp.h"
#include <d3dUtil.h>
#include <iostream>

XMMATRIX cube1WorldMat;
XMMATRIX cube2WorldMat;

XMMATRIX rotation;
XMMATRIX scale;
XMMATRIX translation;
float rotAngle = 0.1f;
float rotateSpeed = 1.0f;

XMMATRIX camView;
XMMATRIX camProjection;


D3DApp::D3DApp() :m_d3dDevice(0), m_d3dDevContext(0), m_swapChain(0),m_depthStencilBuffer(0),m_depthStencilView(0),
m_renderTargetView(0), m_rasterizeState(0), m_squareVertexBuffer(0),m_squareIndiceBuffer(0),m_inputLayout(0), m_fx(0)
{
	m_camera.init();
}

D3DApp::~D3DApp()
{
	cleanUp();
}

bool D3DApp::initD3D(HWND windowId, int width, int height)
{
	HRESULT hr;
	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = windowId;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_d3dDevice, NULL, &m_d3dDevContext);

	if (FAILED(hr))
	{
		std::cout<<"Can't create device and swap chain!";
		return false;
	}

	//Create our BackBuffer
	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	//Create our Render Target
	hr = m_d3dDevice->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	ReleaseCOM(backBuffer);

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	m_d3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer);
	m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);

	//Set our Render Target
	m_d3dDevContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return true;
}

void D3DApp::loadData()
{
	Vertex v[] =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	};
	//create index 
	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)* 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_squareVertexBuffer));


	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD)* 12*3;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_squareIndiceBuffer));
}

void D3DApp::loadShaders()
{
	DWORD shaderFlags = 0;
#ifdef DEBUG||__DEBUG
	shaderFlags|=D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob*compiledShader = 0;
	ID3D10Blob*compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"Effect.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs,0);
	if (compilationMsgs != 0)
	{
		std::cout << (char*)compilationMsgs->GetBufferPointer() << std::endl;
		ReleaseCOM(compilationMsgs);
		return;
	}
	if (FAILED(hr))
	{
		std::cout << "compile effect file error!" << std::endl;
		return;
	}

	D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),compiledShader->GetBufferSize(),0,m_d3dDevice,&m_fx);

	ReleaseCOM(compiledShader);

	m_fxWVPVar = m_fx->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_tech = m_fx->GetTechniqueByName("ColorTech");
}

void D3DApp::createViewport(int width, int height)
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	m_d3dDevContext->RSSetViewports(1, &viewport);
}

void D3DApp::buildVertexLayout()
{
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3DX11_PASS_DESC passDesc;
	m_tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(m_d3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}

void D3DApp::setRasterizationState()
{
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID; 
	wfdesc.CullMode = D3D11_CULL_BACK;
	HRESULT	hr = m_d3dDevice->CreateRasterizerState(&wfdesc, &m_rasterizeState);

	m_d3dDevContext->RSSetState(m_rasterizeState);
}

void D3DApp::initScene(int width,int height)
{
	loadShaders();
	loadData();
	buildVertexLayout();
	createViewport(width, height);

	camView = XMMatrixLookAtLH(XMLoadFloat4(&m_camera.position), XMLoadFloat4(&m_camera.target), XMLoadFloat4(&m_camera.up));
	camProjection = XMMatrixPerspectiveFovLH(m_camera.fov, (float)width / height, m_camera.zNear, m_camera.zFar);
	setRasterizationState();
}

void D3DApp::renderScene()
{
	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, bgColor);

	//Refresh the Depth/Stencil view
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_squareVertexBuffer, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_squareIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_d3dDevContext->IASetInputLayout(m_inputLayout);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{	
		XMMATRIX WVP;
		WVP = cube1WorldMat * camView * camProjection;
		m_fxWVPVar->SetMatrix(reinterpret_cast<float*>(&WVP));
		m_tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(36, 0, 0);

		WVP = cube2WorldMat * camView * camProjection;
		m_fxWVPVar->SetMatrix(reinterpret_cast<float*>(&WVP));
		m_tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		//Draw the second cube
		m_d3dDevContext->DrawIndexed(36, 0, 0);
	}

	//Present the backbuffer to the screen
	m_swapChain->Present(0, 0);
}

void D3DApp::updateScene(double deltaTime)
{
	rotAngle += rotateSpeed*deltaTime;
	if (rotAngle >= 6.28f)
		rotAngle = 0.0f;

	//Reset cube1World
	cube1WorldMat = XMMatrixIdentity();

	//Define cube1's world space matrix
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	rotation = XMMatrixRotationAxis(rotaxis, rotAngle);
	translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	//Set cube1's world space using the transformations
	cube1WorldMat = translation * rotation; 

	//Reset cube2World
	cube2WorldMat = XMMatrixIdentity();

	//Define cube2's world space matrix
	rotation = XMMatrixRotationAxis(rotaxis, -rotAngle);
	scale = XMMatrixScaling(1.2f, 1.2f, 1.2f);

	//Set cube2's world space matrix
	cube2WorldMat = rotation * scale;
}
void D3DApp::resizeD3D(int width,int height)
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	HRESULT hr;
	m_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	//Create our BackBuffer
	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	//Create our Render Target
	hr = m_d3dDevice->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	ReleaseCOM(backBuffer);

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	m_d3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer);
	m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);

	//Set our Render Target
	m_d3dDevContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	m_d3dDevContext->RSSetViewports(1, &viewport);
}

void D3DApp::cleanUp()
{
	ReleaseCOM(m_d3dDevice);
	ReleaseCOM(m_d3dDevContext);
	ReleaseCOM(m_swapChain);
	ReleaseCOM(m_depthStencilBuffer);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_rasterizeState);
	ReleaseCOM(m_squareVertexBuffer);
	ReleaseCOM(m_squareIndiceBuffer);
	ReleaseCOM(m_inputLayout);
	ReleaseCOM(m_fx);
}