#include "MirrorApp.h"
#include "../GameTimer.h"

MirrorApp::MirrorApp() :m_floorDiffuseMapSRV(false), m_wallDiffuseMapSRV(false), m_mirrorDiffuseMapSRV(false),
m_roomVB(false), m_skullIB(false), m_skullVB(false), m_noRenderTargetWritesBS(false),m_skullTranslation(0,0,-2),
m_cullClockwiseRS(false), m_drawReflectionDSS(false), m_noDoubleBlendDSS(false)
{
}

bool MirrorApp::initD3D(HWND windowId, int width, int height)
{
	if (!D3DApp::initD3D(windowId, width, height))
		return false;
	D3D11_BLEND_DESC noRenderTargetWritesDesc = { 0 };
	noRenderTargetWritesDesc.AlphaToCoverageEnable = false;
	noRenderTargetWritesDesc.IndependentBlendEnable = false;

	noRenderTargetWritesDesc.RenderTarget[0].BlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].RenderTargetWriteMask = 0;

	HR(m_d3dDevice->CreateBlendState(&noRenderTargetWritesDesc, &m_noRenderTargetWritesBS));


	D3D11_DEPTH_STENCIL_DESC mirrorDesc;
	mirrorDesc.DepthEnable = true;
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.StencilEnable = true;
	mirrorDesc.StencilReadMask = 0xff;
	mirrorDesc.StencilWriteMask = 0xff;

	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(m_d3dDevice->CreateDepthStencilState(&mirrorDesc, &m_markMirrorDSS));

	D3D11_RASTERIZER_DESC cullClockwiseDesc;
	ZeroMemory(&cullClockwiseDesc, sizeof(D3D11_RASTERIZER_DESC));
	cullClockwiseDesc.FillMode = D3D11_FILL_SOLID;
	cullClockwiseDesc.CullMode = D3D11_CULL_BACK;
	cullClockwiseDesc.FrontCounterClockwise = true;
	cullClockwiseDesc.DepthClipEnable = true;

	HR(m_d3dDevice->CreateRasterizerState(&cullClockwiseDesc, &m_cullClockwiseRS)); 

	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc;
	drawReflectionDesc.DepthEnable = true;
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawReflectionDesc.StencilEnable = true;
	drawReflectionDesc.StencilReadMask = 0xff;
	drawReflectionDesc.StencilWriteMask = 0xff;

	drawReflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	drawReflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	HR(m_d3dDevice->CreateDepthStencilState(&drawReflectionDesc, &m_drawReflectionDSS));

	//
	// NoDoubleBlendDSS
	//

	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;

	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	HR(m_d3dDevice->CreateDepthStencilState(&noDoubleBlendDesc, &m_noDoubleBlendDSS));
	return true;
}


void MirrorApp::cleanUp()
{
	D3DApp::cleanUp();
	ReleaseCOM(m_roomVB);
	ReleaseCOM(m_skullVB);
	ReleaseCOM(m_skullIB);
	ReleaseCOM(m_floorDiffuseMapSRV);
	ReleaseCOM(m_wallDiffuseMapSRV);
	ReleaseCOM(m_mirrorDiffuseMapSRV);
	ReleaseCOM(m_noRenderTargetWritesBS);

	ReleaseCOM(m_markMirrorDSS);
	ReleaseCOM(m_cullClockwiseRS);
	ReleaseCOM(m_drawReflectionDSS);
	ReleaseCOM(m_noDoubleBlendDSS);
}

void MirrorApp::loadTextures()
{
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"../Data/Images/checkboard.dds", 0, 0, &m_floorDiffuseMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"../Data/Images/brick01.dds", 0, 0, &m_wallDiffuseMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"../Data/Images/ice.dds", 0, 0, &m_mirrorDiffuseMapSRV, 0));
}

void MirrorApp::buildRoomGeometryBuffers()
{
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |--------------|
	//   |              |
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/


	Vertex v[30];

	// Floor: Observe we tile texture coordinates.
	v[0] = Vertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	v[1] = Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);

	v[3] = Vertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f);

	// Wall: Observe we tile texture coordinates, and that we
	// leave a gap in the middle for the mirror.
	v[6] = Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[7] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);

	v[9] = Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[10] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f);

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[13] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[16] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	v[18] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);

	v[21] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
	v[23] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f);

	// Mirror
	v[24] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[25] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[26] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[27] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[28] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[29] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 30;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_roomVB));
}
void MirrorApp::buildSkullGeometryBuffers()
{
	std::ifstream fin("../Data/Mesh/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Mesh/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
		fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	m_skullIndexCount = 3 * tcount;
	std::vector<UINT> indices(m_skullIndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_skullVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* m_skullIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_skullIB));
}

void MirrorApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);

	loadTextures();
	buildRoomGeometryBuffers();
	buildSkullGeometryBuffers();
}

void MirrorApp::updateScene(GameTimer*gameTimer)
{	
	float pi = 3.1415926f;
	XMMATRIX skullRotate = XMMatrixRotationY(0.5f*pi);
	XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
	XMMATRIX skullOffset = XMMatrixTranslation(m_skullTranslation.x, m_skullTranslation.y, m_skullTranslation.z);
	XMStoreFloat4x4(&m_skullWorld, skullRotate*skullScale*skullOffset);
}

void MirrorApp::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
	case Qt::Key_W:
		m_skullTranslation.y += 0.1f;
		break;
	case Qt::Key_S:
		m_skullTranslation.y -= 0.1f;
		break;
	case Qt::Key_A:
		m_skullTranslation.x -= 0.1f;
		break;
	case Qt::Key_D:
		m_skullTranslation.x += 0.1f;
		break;
	}
}

void MirrorApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormal);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = view*proj;

	XMMATRIX rotScaleMat = XMLoadFloat4x4(&m_transformMat);
	XMMATRIX translateMat = XMMatrixTranslation(m_translateX, m_translateY, 0.0f);
	XMMATRIX arcballTransMat = rotScaleMat*translateMat;

	// Set per frame constants.
	XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(eyePosW);
	Effects::BasicFX->SetFogColor(Colors::Black);
	Effects::BasicFX->SetFogStart(2.0f);
	Effects::BasicFX->SetFogRange(40.0f);

	// Skull doesn't have texture coordinates, so we can't texture it.
	ID3DX11EffectTechnique* activeTech;
	ID3DX11EffectTechnique* activeSkullTech;
	ID3DX11EffectTechnique* activeSkullShadowTech;

	activeTech = Effects::BasicFX->Light1TexTech;
	activeSkullTech = Effects::BasicFX->Light1Tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	//
	// Draw the floor and walls to the back buffer as normal.
	//

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_roomVB, &stride, &offset);

		// Set per object constants.
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*arcballTransMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_materials[3].data);

		// Floor
		Effects::BasicFX->SetDiffuseMap(m_floorDiffuseMapSRV);
		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->Draw(6, 0);

		// Wall
		Effects::BasicFX->SetDiffuseMap(m_wallDiffuseMapSRV);
		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->Draw(18, 6);
	}

	//
	// Draw the skull to the back buffer as normal.
	//

	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeSkullTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&m_skullWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*arcballTransMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[0].data);

		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}

	//
	// Draw the mirror to stencil buffer only.
	//

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_roomVB, &stride, &offset);

		// Set per object constants.
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*rotScaleMat* translateMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());

		// Do not write to render target.
		m_d3dDevContext->OMSetBlendState(m_noRenderTargetWritesBS, blendFactor, 0xffffffff);

		// Render visible mirror pixels to stencil buffer.
		// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
		m_d3dDevContext->OMSetDepthStencilState(m_markMirrorDSS, 1);

		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->Draw(6, 24);

		// Restore states.
		m_d3dDevContext->OMSetDepthStencilState(0, 0);
		m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	//
	// Draw the skull reflection.
	//
	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeSkullTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

		XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
		XMMATRIX R = XMMatrixReflect(mirrorPlane);
		XMMATRIX world = XMLoadFloat4x4(&m_skullWorld) * R;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*arcballTransMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[0].data);

		// Cache the old light directions, and reflect the light directions.
		XMFLOAT3 oldLightDirections[3];
		for (int i = 0; i < 3; ++i)
		{
			oldLightDirections[i] = m_dirLight[i].Direction;

			XMVECTOR lightDir = XMLoadFloat3(&m_dirLight[i].Direction);
			XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
			XMStoreFloat3(&m_dirLight[i].Direction, reflectedLightDir);
		}

		Effects::BasicFX->SetDirLights(&m_dirLight[0]);

		// Cull clockwise triangles for reflection.
		m_d3dDevContext->RSSetState(m_cullClockwiseRS);

		// Only draw reflection into visible mirror pixels as marked by the stencil buffer. 
		m_d3dDevContext->OMSetDepthStencilState(m_drawReflectionDSS, 1);
		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);

		// Restore default states.
		m_d3dDevContext->RSSetState(0);
		m_d3dDevContext->OMSetDepthStencilState(0, 0);

		// Restore light directions.
		for (int i = 0; i < 3; ++i)
			m_dirLight[i].Direction = oldLightDirections[i];

		Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	}

	//
	// Draw the mirror to the back buffer as usual but with transparency
	// blending so the reflection shows through.
	// 

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_roomVB, &stride, &offset);

		// Set per object constants.
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*arcballTransMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_materials[4].data);
		Effects::BasicFX->SetDiffuseMap(m_mirrorDiffuseMapSRV);

		// Mirror
		m_d3dDevContext->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);
		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->Draw(6, 24);
		m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	//
	// Draw the skull shadow.
	//
	
	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = activeSkullTech->GetPassByIndex(p);

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
		XMVECTOR toMainLight = -XMLoadFloat3(&m_dirLight[0].Direction);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.05f, 0.0f);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&m_skullWorld)*S*shadowOffsetY;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*arcballTransMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[5].data);

		m_d3dDevContext->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);
		m_d3dDevContext->OMSetDepthStencilState(m_noDoubleBlendDSS, 0);
		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);

		// Restore default states.
		m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff);
		m_d3dDevContext->OMSetDepthStencilState(0, 0);
		m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}
	m_swapChain->Present(0, 0);
}