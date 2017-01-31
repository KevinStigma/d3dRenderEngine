#include "NormalDisplacementMapApp.h"
#include <iostream>
NormalDisplacementMapApp::NormalDisplacementMapApp() :m_renderOption(RENDER_BASIC)
{

}

void NormalDisplacementMapApp::loadTextures()
{
	AltarApp::loadTextures();
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/floor_nmap.dds", 0, 0, &m_stoneNormalTexSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/bricks_nmap.dds", 0, 0, &m_brickNormalTexSRV, 0));

}

void NormalDisplacementMapApp::initScene(int width, int height)
{
	AltarApp::initScene(width, height);
	m_sky = new Sky(m_d3dDevice, L"./Data/Images/snowcube1024.dds", 5000.0f);

}
void NormalDisplacementMapApp::updateScene(GameTimer*gameTimer)
{

}
void NormalDisplacementMapApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();

	// Set per frame constants.
	XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(eyePosW);

	Effects::NormalMapFX->SetDirLights(&m_dirLight[0]);
	Effects::NormalMapFX->SetEyePosW(eyePosW);

	Effects::DisplacementMapFX->SetDirLights(&m_dirLight[0]);
	Effects::DisplacementMapFX->SetEyePosW(eyePosW);

	// These properties could be set per object if needed.
	Effects::DisplacementMapFX->SetHeightScale(0.07f);
	Effects::DisplacementMapFX->SetMaxTessDistance(1.0f);
	Effects::DisplacementMapFX->SetMinTessDistance(25.0f);
	Effects::DisplacementMapFX->SetMinTessFactor(1.0f);
	Effects::DisplacementMapFX->SetMaxTessFactor(5.0f);

	// Figure out which technique to use.  Skull does not have texture coordinates,
	// so we need a separate technique for it.
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3Tech;
	ID3DX11EffectTechnique* activeSphereTech = Effects::BasicFX->Light3TexTech;

	switch (m_renderOption)
	{
	case RENDER_BASIC:
		activeTexTech = Effects::BasicFX->Light3TexTech;
		m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RENDER_NORMALMAP:
		activeTexTech = Effects::NormalMapFX->Light3TexTech;
		m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RENDER_DISPLACEMENTMAP:
		activeTexTech = Effects::DisplacementMapFX->Light3TexTech;
		m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
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

		switch (m_renderOption)
		{
		case RENDER_BASIC:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::BasicFX->SetMaterial(m_materials[0].data);
			Effects::BasicFX->SetDiffuseMap(m_floorTexSRV);
			break;
		case RENDER_NORMALMAP:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(m_materials[0].data);
			Effects::NormalMapFX->SetDiffuseMap(m_floorTexSRV);
			Effects::NormalMapFX->SetNormalMap(m_stoneNormalTexSRV);
			break;
		case RENDER_DISPLACEMENTMAP:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(m_camera->getViewProjMatrix());
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(m_materials[0].data);
			Effects::DisplacementMapFX->SetDiffuseMap(m_floorTexSRV);
			Effects::DisplacementMapFX->SetNormalMap(m_stoneNormalTexSRV);
			break;
		}
		activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_gridIndexCount, m_gridIndexOffset, m_gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&m_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		switch (m_renderOption)
		{
		case RENDER_BASIC:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::BasicFX->SetMaterial(m_materials[3].data);
			Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);
			break;
		case RENDER_NORMALMAP:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(m_materials[3].data);
			Effects::NormalMapFX->SetDiffuseMap(m_brickTexSRV);
			Effects::NormalMapFX->SetNormalMap(m_brickNormalTexSRV);
			break;
		case RENDER_DISPLACEMENTMAP:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(m_camera->getViewProjMatrix());
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(m_materials[3].data);
			Effects::DisplacementMapFX->SetDiffuseMap(m_brickTexSRV);
			Effects::DisplacementMapFX->SetNormalMap(m_brickNormalTexSRV);
			break;
		}
		activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			switch (m_renderOption)
			{
			case RENDER_BASIC:
				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::BasicFX->SetMaterial(m_materials[1].data);
				Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);
				break;
			case RENDER_NORMALMAP:
				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::NormalMapFX->SetMaterial(m_materials[1].data);
				Effects::NormalMapFX->SetDiffuseMap(m_brickTexSRV);
				Effects::NormalMapFX->SetNormalMap(m_brickNormalTexSRV);
				break;
			case RENDER_DISPLACEMENTMAP:
				Effects::DisplacementMapFX->SetWorld(world);
				Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::DisplacementMapFX->SetViewProj(m_camera->getViewProjMatrix());
				Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);
				Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::DisplacementMapFX->SetMaterial(m_materials[1].data);
				Effects::DisplacementMapFX->SetDiffuseMap(m_brickTexSRV);
				Effects::DisplacementMapFX->SetNormalMap(m_brickNormalTexSRV);
				break;
			}
			activeTexTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.

	//I can't understand this code completely now
	m_d3dDevContext->HSSetShader(0, 0, 0);
	m_d3dDevContext->DSSetShader(0, 0, 0);

	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Draw the spheres.
	activeSphereTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (int i = 0; i < 10; ++i)
		{
			XMMATRIX world = XMLoadFloat4x4(&m_sphereWorld[i]);
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_materials[2].data);
			Effects::BasicFX->SetDiffuseMap(m_stoneTexSRV);

			activeSphereTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	stride = sizeof(Vertex::Basic32);
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

	m_sky->Draw(m_d3dDevContext, *m_camera);

	HR(m_swapChain->Present(0, 0));
}

void NormalDisplacementMapApp::cleanUp()
{
	AltarApp::cleanUp();
	SafeDelete(m_sky);
	ReleaseCOM(m_stoneNormalTexSRV);
	ReleaseCOM(m_brickNormalTexSRV);
}

void NormalDisplacementMapApp::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_1:
		m_renderOption = RENDER_BASIC;
		break;
	case Qt::Key_2:
		m_renderOption = RENDER_NORMALMAP;
		break;
	case Qt::Key_3:
		m_renderOption = RENDER_DISPLACEMENTMAP;
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