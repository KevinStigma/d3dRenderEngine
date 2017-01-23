#include "TessellationApp.h"
#include <MathHelper.cpp>
#include <qwidget.h>

TessellationApp::TessellationApp() :m_quadPatchVB(NULL), m_wireFrameRS(NULL), m_lastPoint(0,0)
{
	m_theta = 1.3f*MathHelper::Pi;
	m_phi = 0.2f*MathHelper::Pi;
	m_radius = 80.0f;
}

bool TessellationApp::initD3D(HWND windowId, int width, int height)
{
	if (!D3DApp::initD3D(windowId, width, height))
		return false;

	return true;
}

void TessellationApp::updateScene(GameTimer*gameTimer)
{
	float x = m_radius*sinf(m_phi)*cosf(m_theta);
	float z = m_radius*sinf(m_phi)*sinf(m_theta);
	float y = m_radius*cosf(m_phi);

	m_camera->calViewMatrix(XMFLOAT3(x, y, z), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
}

void TessellationApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_BACK;
	HRESULT	hr = m_d3dDevice->CreateRasterizerState(&wfdesc, &m_wireFrameRS);

	m_d3dDevContext->RSSetState(m_wireFrameRS);
	buildQuadPatchBuffer();
}

void TessellationApp::buildQuadPatchBuffer()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3)* 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	XMFLOAT3 vertices[4] =
	{
		XMFLOAT3(-10.0f, 0.0f, +10.0f),
		XMFLOAT3(+10.0f, 0.0f, +10.0f),
		XMFLOAT3(-10.0f, 0.0f, -10.0f),
		XMFLOAT3(+10.0f, 0.0f, -10.0f)
	};

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_quadPatchVB));
}
void TessellationApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::Pos);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_d3dDevContext->RSSetState(m_wireFrameRS);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = view*proj;

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;

	XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);

	// Set per frame constants.
	Effects::TessellationFX->SetEyePosW(eyePosW);
	Effects::TessellationFX->SetFogColor(Colors::Silver);
	Effects::TessellationFX->SetFogStart(15.0f);
	Effects::TessellationFX->SetFogRange(175.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::TessellationFX->TessTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_quadPatchVB, &stride, &offset);

		// Set per object constants.
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::TessellationFX->SetWorld(world);
		Effects::TessellationFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::TessellationFX->SetWorldViewProj(worldViewProj);
		Effects::TessellationFX->SetTexTransform(XMMatrixIdentity());
		Effects::TessellationFX->SetDiffuseMap(0);
		
		Effects::TessellationFX->TessTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);

		m_d3dDevContext->Draw(4, 0);
	}

	HR(m_swapChain->Present(0, 0));
}

void TessellationApp::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
		m_radius += 0.5f;
		break;
	case Qt::Key_S:
		m_radius -= 0.5f;
		break;
	}
}
void TessellationApp::cleanUp()
{
	ReleaseCOM(m_quadPatchVB);
}