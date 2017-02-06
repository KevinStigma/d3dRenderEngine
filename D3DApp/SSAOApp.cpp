#include "SSAOApp.h"
SSAOApp::SSAOApp() :m_ssao(NULL), m_equalsDSS(NULL)
{
}

void SSAOApp::initScene(int width, int height)
{
	ShadowApp::initScene(width, height);
	m_ssao = new Ssao(m_d3dDevice, m_d3dDevContext, m_screenViewport.Width, m_screenViewport.Height, m_camera->fov, m_camera->zFar);
	
	buildScreenQuadGeometryBuffers();

	D3D11_DEPTH_STENCIL_DESC equalsDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalsDesc.DepthEnable = true;
	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equalsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;

	HR(m_d3dDevice->CreateDepthStencilState(&equalsDesc, &m_equalsDSS));
}
void SSAOApp::updateScene(GameTimer*gameTimer)
{
	ShadowApp::updateScene(gameTimer);
}

void SSAOApp::drawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormalDepthFX->NormalDepthTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, spheres and box.

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_shapesVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_shapesIB, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&m_gridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_gridIndexCount, m_gridIndexOffset, m_gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&m_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_cylinderIndexCount, m_cylinderIndexOffset, m_cylinderVertexOffset);
		}

		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&m_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

			tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	m_d3dDevContext->RSSetState(0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_skullVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_skullIB, DXGI_FORMAT_R32_UINT, 0);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&m_skullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(m_skullIndexCount, 0, 0);
	}
}

void SSAOApp::renderScene()
{
	m_smap->BindDsvAndSetNullRenderTarget(m_d3dDevContext);

	drawSceneToShadowMap();

	m_d3dDevContext->RSSetState(0);

	m_d3dDevContext->RSSetViewports(1, &m_screenViewport);
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	m_ssao->SetNormalDepthRenderTarget(m_depthStencilView);
	drawSceneToSsaoNormalDepthMap();

	m_ssao->ComputeSsao(*m_camera);
	m_ssao->BlurAmbientMap(4);

	m_d3dDevContext->RSSetViewports(1, &m_screenViewport);
	ID3D11RenderTargetView* renderTargets[1] = { m_renderTargetView };
	m_d3dDevContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView);
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));

	// We already laid down scene depth to the depth buffer in the Normal/Depth map pass,
	// so we can set the depth comparison test to EQUAL
	m_d3dDevContext->OMSetDepthStencilState(m_equalsDSS, 0);

	XMMATRIX view = m_camera->getViewMatrix();
	XMMATRIX proj = m_camera->getProjMatrix();
	XMMATRIX viewProj = m_camera->getViewProjMatrix();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(m_camera->position);
	Effects::BasicFX->SetCubeMap(m_sky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(m_smap->DepthMapSRV());
	Effects::BasicFX->SetSsaoMap(m_ssao->AmbientSRV());

	// Figure out which technique to use for different geometry.

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSphereTech = Effects::BasicFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3RefTech;
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

	m_d3dDevContext->OMSetDepthStencilState(0, 0);
	
	drawScreenQuad(m_ssao->AmbientSRV());
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

void SSAOApp::drawScreenQuad(ID3D11ShaderResourceView* srv)
{
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dDevContext->IASetVertexBuffers(0, 1, &m_screenQuadVB, &stride, &offset);
	m_d3dDevContext->IASetIndexBuffer(m_screenQuadIB, DXGI_FORMAT_R32_UINT, 0);

	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ID3DX11EffectTechnique* tech = Effects::DebugTexFX->ViewRedTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::DebugTexFX->SetWorldViewProj(world);
		Effects::DebugTexFX->SetTexture(srv);

		tech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
		m_d3dDevContext->DrawIndexed(6, 0, 0);
	}
}


void SSAOApp::cleanUp()
{
	ShadowApp::cleanUp();
	SafeDelete(m_ssao);
	ReleaseCOM(m_equalsDSS);
	ReleaseCOM(m_screenQuadIB);
	ReleaseCOM(m_screenQuadVB);
}