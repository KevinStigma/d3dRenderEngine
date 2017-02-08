#include "AnimationApp.h"
#include "GameTimer.h"
AnimationApp::AnimationApp() :m_characterModel(NULL)
{

}

void AnimationApp::initScene(int width, int height)
{
	ShadowApp::initScene(width, height);

	m_texMgr.Init(m_d3dDevice);
	m_characterModel = new SkinnedModel(m_d3dDevice, m_texMgr, "./Data/Mesh/soldier.m3d", L"./Data/Images/");
	m_characterInstance1.Model = m_characterModel;
	m_characterInstance2.Model = m_characterModel;
	m_characterInstance1.TimePos = 0.0f;
	m_characterInstance2.TimePos = 0.0f;
	m_characterInstance1.ClipName = "Take1";
	m_characterInstance2.ClipName = "Take1";
	m_characterInstance1.FinalTransforms.resize(m_characterModel->SkinnedData.BoneCount());
	m_characterInstance2.FinalTransforms.resize(m_characterModel->SkinnedData.BoneCount());

	// Reflect to change coordinate system from the RHS the data was exported out as.
	XMMATRIX modelScale = XMMatrixScaling(0.05f, 0.05f, -0.05f);
	XMMATRIX modelRot = XMMatrixRotationY(MathHelper::Pi);
	XMMATRIX modelOffset = XMMatrixTranslation(-2.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&m_characterInstance1.World, modelScale*modelRot*modelOffset);

	modelOffset = XMMatrixTranslation(2.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&m_characterInstance2.World, modelScale*modelRot*modelOffset);

	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/head_diff.dds", 0, 0, &m_headTexSRV, 0));

}
void AnimationApp::updateScene(GameTimer*gameTimer)
{
	//ShadowApp::updateScene(gameTimer);
	m_characterInstance1.Update(gameTimer->DeltaTime());
	m_characterInstance2.Update(gameTimer->DeltaTime());
	buildShadowTransform();
}

void AnimationApp::drawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&m_lightView);
	XMMATRIX proj = XMLoadFloat4x4(&m_lightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(m_camera->position);
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ID3DX11EffectTechnique* tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_shapesVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_shapesIB, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	tessSmapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&m_gridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		tessSmapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_gridIndexCount, m_gridIndexOffset, m_gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&m_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

		tessSmapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			tessSmapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	m_d3dDevContext->HSSetShader(0, 0, 0);
	m_d3dDevContext->DSSetShader(0, 0, 0);

	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Draw the spheres with cubemap reflection.
	//

	smapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

			smapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	m_d3dDevContext->RSSetState(0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

	smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	smapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&m_skullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		smapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	smapTech = Effects::BuildShadowMapFX->BuildShadowMapSkinnedTech;
	smapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Instance 1

		world = XMLoadFloat4x4(&m_characterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		Effects::BuildShadowMapFX->SetBoneTransforms(
			&m_characterInstance1.FinalTransforms[0],
			m_characterInstance1.FinalTransforms.size());

		for (UINT subset = 0; subset < m_characterInstance1.Model->SubsetCount; ++subset)
		{
			smapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_characterInstance1.Model->ModelMesh.Draw(m_d3dDevContext, subset);
		}

		// Instance 2

		world = XMLoadFloat4x4(&m_characterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		Effects::BuildShadowMapFX->SetBoneTransforms(
			&m_characterInstance2.FinalTransforms[0],
			m_characterInstance2.FinalTransforms.size());

		for (UINT subset = 0; subset < m_characterInstance1.Model->SubsetCount; ++subset)
		{
			smapTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_characterInstance2.Model->ModelMesh.Draw(m_d3dDevContext, subset);
		}
	}

}

void AnimationApp::renderScene()
{
	m_smap->BindDsvAndSetNullRenderTarget(m_d3dDevContext);

	drawSceneToShadowMap();

	m_d3dDevContext->RSSetState(0);

	//
	// Restore the back and depth buffer to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { m_renderTargetView };
	m_d3dDevContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView);
	m_d3dDevContext->RSSetViewports(1, &m_screenViewport);

	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = m_camera->getViewProjMatrix();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(m_camera->position);
	Effects::BasicFX->SetCubeMap(m_sky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(m_smap->DepthMapSRV());

	// Figure out which technique to use for different geometry.

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexShadowTech;
	ID3DX11EffectTechnique* activeSphereTech = Effects::BasicFX->Light3TexShadowTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3RefTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::BasicFX->Light3TexShadowSkinedTech;
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX shadowTransform = XMLoadFloat4x4(&m_shadowTransform);

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_shapesVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_shapesIB, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&m_gridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
		Effects::BasicFX->SetMaterial(m_materials[0].data);
		Effects::BasicFX->SetDiffuseMap(m_floorTexSRV);

		activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_gridIndexCount, m_gridIndexOffset, m_gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&m_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
		Effects::BasicFX->SetMaterial(m_materials[3].data);
		Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);

		activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
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
			Effects::BasicFX->SetShadowTransform(world*shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
			Effects::BasicFX->SetMaterial(m_materials[1].data);
			Effects::BasicFX->SetDiffuseMap(m_brickTexSRV);

			activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	m_d3dDevContext->HSSetShader(0, 0, 0);
	m_d3dDevContext->DSSetShader(0, 0, 0);

	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Draw the spheres with cubemap reflection.
	//

	activeSphereTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetShadowTransform(world*shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(m_materials[2].data);

			activeSphereTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	m_d3dDevContext->RSSetState(0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&m_skullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(m_materials[4].data);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetCubeMap(m_sky->CubeMapSRV());

		activeSkullTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}

	//
	// Draw the animated characters.
	//

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	activeSkinnedTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Instance 1

		world = XMLoadFloat4x4(&m_characterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		Effects::BasicFX->SetBoneTransforms(
			&m_characterInstance1.FinalTransforms[0],
			m_characterInstance1.FinalTransforms.size());

		for (UINT subset = 0; subset < m_characterInstance1.Model->SubsetCount; ++subset)
		{
			Effects::BasicFX->SetMaterial(m_characterInstance1.Model->Mat[subset]);
			Effects::BasicFX->SetDiffuseMap(m_characterInstance1.Model->DiffuseMapSRV[subset]);

			activeSkinnedTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_characterInstance1.Model->ModelMesh.Draw(m_d3dDevContext, subset);
		}

		// Instance 2

		world = XMLoadFloat4x4(&m_characterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		Effects::BasicFX->SetBoneTransforms(
			&m_characterInstance2.FinalTransforms[0],
			m_characterInstance2.FinalTransforms.size());

		for (UINT subset = 0; subset < m_characterInstance1.Model->SubsetCount; ++subset)
		{
			Effects::BasicFX->SetMaterial(m_characterInstance2.Model->Mat[subset]);
			Effects::BasicFX->SetDiffuseMap(m_characterInstance2.Model->DiffuseMapSRV[subset]);

			activeSkinnedTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_characterInstance2.Model->ModelMesh.Draw(m_d3dDevContext, subset);
		}
	}


	m_sky->Draw(m_d3dDevContext, *m_camera);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dDevContext->RSSetState(0);
	m_d3dDevContext->OMSetDepthStencilState(0, 0);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	m_d3dDevContext->PSSetShaderResources(0, 16, nullSRV);

	HR(m_swapChain->Present(0, 0));
}
void AnimationApp::cleanUp()
{
	ShadowApp::cleanUp();
	SafeDelete(m_characterModel);
}
