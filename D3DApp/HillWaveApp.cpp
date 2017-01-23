#include "HillWaveApp.h"
#include "../GameTimer.h"
#include <GeometryGenerator.h>
#include <iostream>

HillWaveApp::HillWaveApp()
{
	m_waves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);
	XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&m_grassTexTransform, grassTexScale);
	m_waterTexOffset = XMFLOAT2(0.0f, 0.0f);
}

void HillWaveApp::updateScene(GameTimer* gameTimer)
{
	float dt = gameTimer->DeltaTime()*2;
	float total_time = gameTimer->TotalTime();
	
	static float t_base = 0.0f;
	float timeSpan = 0.5f;
	if ((total_time - t_base) >= timeSpan)
	{
		t_base += timeSpan;

		DWORD i = 5 + rand() % (m_waves.RowCount() - 10);
		DWORD j = 5 + rand() % (m_waves.ColumnCount() - 10);

		float r = MathHelper::RandF(1.0f, 2.0f);

		m_waves.Disturb(i, j, r);
	}
	m_waves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_d3dDevContext->Map(m_wavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < m_waves.VertexCount(); ++i)
	{
		v[i].pos = m_waves[i];
		v[i].normal = m_waves.Normal(i);

		// Derive tex-coords in [0,1] from position.
		v[i].tex.x = 0.5f + m_waves[i].x / m_waves.Width();
		v[i].tex.y = 0.5f - m_waves[i].z / m_waves.Depth();
	}

	m_d3dDevContext->Unmap(m_wavesVB, 0);

	//
	// Animate water texture coordinates.
	//

	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

	// Translate texture over time.
	m_waterTexOffset.y += 0.05f*dt;
	m_waterTexOffset.x += 0.1f*dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(m_waterTexOffset.x, m_waterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&m_waterTexTransform, wavesScale*wavesOffset);
}

void HillWaveApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);
	buildLandGeometryBuffers();
	buildWaveGeometryBuffers();
}

void HillWaveApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = view*proj;

	XMMATRIX I = XMMatrixIdentity();

	XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
	// Set per frame constants.
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(eyePosW);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light1TexAlphaClipFogTech;

	XMMATRIX rotScaleMat = XMLoadFloat4x4(&m_transformMat);
	XMMATRIX translateMat = XMMatrixTranslation(m_translateX, m_translateY, 0.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_landVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_landIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = I;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*rotScaleMat* translateMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&m_grassTexTransform));
		Effects::BasicFX->SetMaterial(m_materials[1].data);
		Effects::BasicFX->SetDiffuseMap(m_grassMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_landIndexCount, 0, 0);

		// Draw the waves.
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_wavesVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_wavesIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		world = I;
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*rotScaleMat* translateMat*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&m_waterTexTransform));
		Effects::BasicFX->SetMaterial(m_materials[2].data);
		Effects::BasicFX->SetDiffuseMap(m_wavesMapSRV);

		m_d3dDevContext->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);
		activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(3 * m_waves.TriangleCount(), 0, 0);
		m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	HR(m_swapChain->Present(0, 0));
}

void HillWaveApp::loadTextures()
{
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"../Data/Images/grass.dds", 0, 0, &m_grassMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"../Data/Images/water2.dds", 0, 0, &m_wavesMapSRV, 0));
}

float GetHillHeight(float x, float z)
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

XMFLOAT3 GetHillNormal(float x, float z)
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}


void HillWaveApp::buildLandGeometryBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	m_landIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  
	//

	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHillHeight(p.x, p.z);

		vertices[i].pos = p;
		vertices[i].normal = GetHillNormal(p.x, p.z);
		vertices[i].tex = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_landVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* m_landIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_landIB));
}
void HillWaveApp::buildWaveGeometryBuffers()
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * m_waves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(m_d3dDevice->CreateBuffer(&vbd, 0, &m_wavesVB));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3 * m_waves.TriangleCount()); // 3 indices per face

	// Iterate over each quad.
	UINT m = m_waves.RowCount();
	UINT n = m_waves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_wavesIB));
}

void HillWaveApp::cleanUp()
{
	D3DApp::cleanUp();
	ReleaseCOM(m_grassMapSRV);
	ReleaseCOM(m_wavesMapSRV);
	ReleaseCOM(m_landIB);
	ReleaseCOM(m_landVB);
	ReleaseCOM(m_wavesIB);
	ReleaseCOM(m_wavesVB);
}