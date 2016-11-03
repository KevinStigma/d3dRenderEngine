#include "D3DApp.h"
#include "GlobalSys.h"
#include "../GameTimer.h"
#include <d3dUtil.h>
#include <iostream>
#include <MathHelper.h>

D3DApp::D3DApp() :m_d3dDevice(0), m_d3dDevContext(0), m_swapChain(0),m_depthStencilBuffer(0),m_depthStencilView(0),
m_renderTargetView(0), m_rasterizeState(0), m_squareVertexBuffer(0),m_squareIndiceBuffer(0),m_translateX(0), m_translateY(0),
m_hasTex(false)
{
	m_camera = new Camera;
	XMStoreFloat4x4(&m_transformMat, XMMatrixIdentity());
	initLight();
	initMaterials();
}

D3DApp::~D3DApp()
{
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
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
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

void D3DApp::initMaterials()
{
	float r=0.73725f, g=0.741176f, b=0.74902f;
	m_materials.resize(3);
	m_materials[0].name = "gray";
	m_materials[0].data.Ambient = XMFLOAT4(r*0.2f,g*0.2f,b*0.2f,1.0f);
	m_materials[0].data.Diffuse = XMFLOAT4(r*0.4f,g*0.4f,b*0.4f, 1.0f);
	m_materials[0].data.Specular = XMFLOAT4(r*0.5f,g*0.5f,b*0.5f,16.0f);

	m_materials[1].name = "land";
	m_materials[1].data.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materials[1].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[1].data.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	m_materials[2].name = "wave";
	m_materials[2].data.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materials[2].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[2].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
}

void D3DApp::initLight()
{
	m_dirLight.resize(3);
	m_dirLight[0].Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); 
	m_dirLight[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[0].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_dirLight[1].Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[1].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[1].Direction = XMFLOAT3(-0.57735f, 0.57735f, -0.57735f);

	m_dirLight[2].Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[2].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_dirLight[2].Direction = XMFLOAT3(-0.57735f, 0.57735f, -0.57735f);

	m_spotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_spotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	m_spotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_spotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_spotLight.Spot = 96.0f;
	m_spotLight.Range = 10000.0f;

	m_pointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_pointLight.Range = 25.0f;
}

//now for convenient, we only use the first object to generate buffers
void D3DApp::loadObjData()
{
	std::vector<Vertex> vertices;
	const std::vector<float>& positions = g_pGlobalSys->objects[0].mesh.positions;
	const std::vector<float>& normals = g_pGlobalSys->objects[0].mesh.normals;
	const std::vector<float>& texs = g_pGlobalSys->objects[0].mesh.texcoords;
	if (texs.size())
		m_hasTex = true;

	int num_vertex = positions.size() / 3;
	for (int i = 0; i < num_vertex; i++)
	{
		Vertex v(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2],
			normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2],0,0);
		if (m_hasTex)
		{
			v.tex.x = texs[i * 2];
			v.tex.y = texs[i * 2+1];
		}
		vertices.push_back(v);
	}
	
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)* vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_squareVertexBuffer));

	const std::vector<unsigned int>& indices = g_pGlobalSys->objects[0].mesh.indices;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int)* indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_squareIndiceBuffer));
}

void D3DApp::loadShaders()
{
	Effects::InitAll(m_d3dDevice);
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
	InputLayouts::initAll(m_d3dDevice,Effects::BasicFX->Light1Tech);
}

void D3DApp::loadTextures()
{

}

void D3DApp::setRasterizationState()
{
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	//wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_BACK;
	HRESULT	hr = m_d3dDevice->CreateRasterizerState(&wfdesc, &m_rasterizeState);

	m_d3dDevContext->RSSetState(m_rasterizeState);
}

void D3DApp::initScene(int width,int height)
{
	m_camera->init(width, height);
	loadShaders();
	loadTextures();
	buildVertexLayout();
	createViewport(width, height);
	setRasterizationState();
}

void D3DApp::renderScene()
{
	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, bgColor);

	//Refresh the Depth/Stencil view
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_squareVertexBuffer&&m_squareIndiceBuffer)
	{
		BasicEffect*basicEffect = Effects::BasicFX;
		
		XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
		basicEffect->SetDirLights(&m_dirLight[0]);
		basicEffect->SetEyePosW(eyePosW);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_squareVertexBuffer, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_squareIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormal);
		m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX rotScaleMat = XMLoadFloat4x4(&m_transformMat);
		XMMATRIX translateMat = XMMatrixTranslation(m_translateX, m_translateY, 0.0f);
		XMMATRIX worldMat = XMMatrixIdentity();

		ID3DX11EffectTechnique* activeTech = basicEffect->Light2Tech;
		D3DX11_TECHNIQUE_DESC techDesc;
		activeTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			XMMATRIX WVP;
			WVP = worldMat* rotScaleMat* translateMat* m_camera->getViewMatrix() * m_camera->getProjMatrix();
			basicEffect->SetWorld(worldMat);
			basicEffect->SetWorldInvTranspose(worldMat);
			basicEffect->SetWorldViewProj(WVP);
			basicEffect->SetMaterial(m_materials[0].data);

			activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(g_pGlobalSys->objects[0].mesh.indices.size(), 0, 0);
		}
	}
	//Present the backbuffer to the screen
	m_swapChain->Present(0, 0);
}

void D3DApp::updateScene(GameTimer*gameTimer)
{
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

void D3DApp::setTransMat(float*data)
{
	for (int i = 0; i < 4;i++)
	for (int j = 0; j < 4; j++)
	{
		int baseInd = i * 4;
		m_transformMat(j,i) = data[baseInd + j];
	}
}

void D3DApp::setTranslate(float x, float y)
{
	m_translateX = x;
	m_translateY = y;
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
	Effects::DestroyAll();
	InputLayouts::destroyAll();
	SafeDelete(m_camera);
}