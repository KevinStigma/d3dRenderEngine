#include "MirrorApp.h"
MirrorApp::MirrorApp() :m_floorDiffuseMapSRV(false), m_wallDiffuseMapSRV(false), m_mirrorDiffuseMapSRV(false),
m_roomVB(false), m_skullIB(false), m_skullVB(false), m_skullTranslation(0,0,-2)
{

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
}

//void MirrorApp::initMaterials()
//{
//	m_materials.resize(4);
//	m_materials[0].name = "room";
//	m_materials[0].data.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
//	m_materials[0].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//	m_materials[0].data.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
//
//	m_materials[1].name = "skull";
//	m_materials[1].data.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
//	m_materials[1].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//	m_materials[1].data.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
//
//	m_materials[2].name = "mirror";
//	m_materials[2].data.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
//	m_materials[2].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f);
//	m_materials[2].data.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
//
//	m_materials[3].name = "shadow";
//	m_materials[3].data.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
//	m_materials[3].data.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
//	m_materials[3].data.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
//}

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

	activeTech = Effects::BasicFX->Light2TexTech;
	activeSkullTech = Effects::BasicFX->Light2Tech;
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
		XMMATRIX worldViewProj = world*rotScaleMat* translateMat*view*proj;

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
		XMMATRIX worldViewProj = world*rotScaleMat* translateMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[0].data);

		pass->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}

	m_swapChain->Present(0, 0);
}