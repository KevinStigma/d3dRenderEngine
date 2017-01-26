#include "CubeMapApp.h"
CubeMapApp::CubeMapApp() :m_sky(NULL)
{

}
CubeMapApp::~CubeMapApp()
{

}

void CubeMapApp::cleanUp()
{
	AltarApp::cleanUp();
	SafeDelete(m_sky);
}

void CubeMapApp::initScene(int width, int height)
{
	AltarApp::initScene(width, height);
	m_sky = new Sky(m_d3dDevice, L"./Data/Images/desertcube1024.dds", 5000.0f);

}

void CubeMapApp::renderScene()
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
		activeSkullTech = Effects::BasicFX->Light1RefTech;
		break;
	case 2:
		activeTexTech = Effects::BasicFX->Light2TexTech;
		activeSkullTech = Effects::BasicFX->Light2RefTech;
		break;
	case 3:
		activeTexTech = Effects::BasicFX->Light3TexTech;
		activeSkullTech = Effects::BasicFX->Light3RefTech;
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
		Effects::BasicFX->SetMaterial(m_materials[5].data);
		Effects::BasicFX->SetCubeMap(m_sky->CubeMapSRV());

		activeSkullTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}
	m_sky->Draw(m_d3dDevContext, *m_camera);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dDevContext->RSSetState(0);
	m_d3dDevContext->OMSetDepthStencilState(0, 0);
	HR(m_swapChain->Present(0, 0));
}