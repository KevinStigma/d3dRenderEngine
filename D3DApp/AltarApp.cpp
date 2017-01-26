#include "AltarApp.h"
#include "GeometryGenerator.h"
#include "GeometryGenerator.cpp"

AltarApp::AltarApp() :m_boxVertexOffset(0), m_gridVertexOffset(0), m_cylinderVertexOffset(0), m_sphereVertexOffset(0),
m_boxIndexCount(0), m_gridIndexCount(0), m_sphereIndexCount(0), m_cylinderIndexCount(0), m_boxIndexOffset(0), m_gridIndexOffset(0),
m_sphereIndexOffset(0), m_cylinderIndexOffset(0), m_shapesVB(NULL), m_shapesIB(NULL), m_skullVB(NULL), m_skullIB(NULL), m_skullIndexCount(0),
m_lightCount(1), m_floorTexSRV(NULL), m_stoneTexSRV(NULL), m_brickTexSRV(NULL)
{

}
void AltarApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);
	buildShapeGeometryBuffers();
	buildSkullGeometryBuffers();
	initWorldMatrix();
	initMaterials();
}

void AltarApp::loadTextures()
{
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/floor.dds", 0, 0, &m_floorTexSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/stone.dds", 0, 0, &m_stoneTexSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/bricks.dds", 0, 0, &m_brickTexSRV, 0));
}
void AltarApp::initMaterials()
{
	m_materials.resize(6);
	m_materials[0].name = "grid";
	m_materials[0].data.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_materials[0].data.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_materials[0].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	m_materials[1].name = "cylinder";
	m_materials[1].data.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[1].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[1].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	m_materials[2].name = "sphere";
	m_materials[2].data.Ambient = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);
	m_materials[2].data.Diffuse = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);
	m_materials[2].data.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

	m_materials[3].name = "box";
	m_materials[3].data.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[3].data.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials[3].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	m_materials[4].name = "skull";
	m_materials[4].data.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_materials[4].data.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_materials[4].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	m_materials[4].data.Reflect = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	m_materials[5].name = "reflect";
	m_materials[5].data.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_materials[5].data.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_materials[5].data.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	m_materials[5].data.Reflect = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
}

void AltarApp::updateScene(GameTimer*gameTimer)
{

}
void AltarApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();

	// Set per frame constants.
	XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(eyePosW);

	// Figure out which technique to use.  Skull does not have texture coordinates,
	// so we need a separate technique for it.
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light1Tech;

	switch (m_lightCount)
	{
	case 1:
		activeTexTech = Effects::BasicFX->Light1TexTech;
		activeSkullTech = Effects::BasicFX->Light1Tech;
		break;
	case 2:
		activeTexTech = Effects::BasicFX->Light2TexTech;
		activeSkullTech = Effects::BasicFX->Light2Tech;
		break;
	case 3:
		activeTexTech = Effects::BasicFX->Light3TexTech;
		activeSkullTech = Effects::BasicFX->Light3Tech;
		break;
	}

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTexTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_shapesVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_shapesIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the grid.
		XMMATRIX world = XMLoadFloat4x4(&m_gridWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(6.0f, 8.0f, 1.0f));
		Effects::BasicFX->SetMaterial(m_materials[0].data);
		Effects::BasicFX->SetDiffuseMap(m_floorTexSRV);

		activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_gridIndexCount, m_gridIndexOffset, m_gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&m_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(m_materials[3].data);
		Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

		activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_materials[1].data);
			Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}

		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_materials[2].data);
			Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

			activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}

	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.

		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&m_skullWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[4].data);

		activeSkullTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}

	HR(m_swapChain->Present(0, 0));
}

void AltarApp::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_1:
		m_lightCount = 1;
		break;
	case Qt::Key_2:
		m_lightCount = 2;
		break;
	case Qt::Key_3:
		m_lightCount = 3;
		break;
	case Qt::Key_Up:
		m_camera->walkForward(0.5f);
		break;
	case Qt::Key_Down:
		m_camera->walkForward(-0.5f);
		break;
	case Qt::Key_Right:
		m_camera->walkRight(0.2f);
		break;
	case Qt::Key_Left:
		m_camera->walkRight(-0.2f);
		break;
	case Qt::Key_W:
		m_camera->rotateRight(-MathHelper::Pi / 72.0f);
		break;
	case Qt::Key_S:
		m_camera->rotateRight(MathHelper::Pi / 72.0f);
		break;
	case Qt::Key_A:
		m_camera->rotateY(-MathHelper::Pi / 72.0f);
		break;
	case Qt::Key_D:
		m_camera->rotateY(MathHelper::Pi / 72.0f);
		break;
	case Qt::Key_Q:
		m_camera->walkUp(0.2f);
		break;
	case Qt::Key_E:
		m_camera->walkUp(-0.2f);
		break;
	default:
		break;
	}
}
void AltarApp::cleanUp()
{
	ReleaseCOM(m_floorTexSRV);
	ReleaseCOM(m_stoneTexSRV);
	ReleaseCOM(m_brickTexSRV);

	ReleaseCOM(m_shapesVB);
	ReleaseCOM(m_shapesIB);
	ReleaseCOM(m_skullVB);
	ReleaseCOM(m_skullIB);
}

void AltarApp::initWorldMatrix()
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_gridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&m_boxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&m_skullWorld, XMMatrixMultiply(skullScale, skullOffset));

	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&m_cylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&m_cylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&m_sphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&m_sphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}
}

void AltarApp::buildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	m_boxVertexOffset = 0;
	m_gridVertexOffset = box.Vertices.size();
	m_sphereVertexOffset = m_gridVertexOffset + grid.Vertices.size();
	m_cylinderVertexOffset = m_sphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	m_boxIndexCount = box.Indices.size();
	m_gridIndexCount = grid.Indices.size();
	m_sphereIndexCount = sphere.Indices.size();
	m_cylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	m_boxIndexOffset = 0;
	m_gridIndexOffset;
	m_sphereIndexOffset = m_gridIndexOffset + m_gridIndexCount;
	m_cylinderIndexOffset = m_sphereIndexOffset + m_sphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		m_boxIndexCount +
		m_gridIndexCount +
		m_sphereIndexCount +
		m_cylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].pos = box.Vertices[i].Position;
		vertices[k].normal = box.Vertices[i].Normal;
		vertices[k].tex = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].pos = grid.Vertices[i].Position;
		vertices[k].normal = grid.Vertices[i].Normal;
		vertices[k].tex = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].pos = sphere.Vertices[i].Position;
		vertices[k].normal = sphere.Vertices[i].Normal;
		vertices[k].tex = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].pos = cylinder.Vertices[i].Position;
		vertices[k].normal = cylinder.Vertices[i].Normal;
		vertices[k].tex = cylinder.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_shapesVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_shapesIB));
}

void AltarApp::buildSkullGeometryBuffers()
{
	std::ifstream fin("./Data/Mesh/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"./Data/Mesh/skull.txt not found.", 0, 0);
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
	vbd.ByteWidth = sizeof(Vertex)* vcount;
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