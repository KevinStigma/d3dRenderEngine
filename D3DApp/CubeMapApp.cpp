#include "CubeMapApp.h"
CubeMapApp::CubeMapApp() :m_sky(NULL)
{
	for (int i = 0; i < 6; i++)
		m_dynamicCubeMapRTV[i] = NULL;
	m_dynamicCubeMapSRV = NULL;
	m_dynamicCubeMapDSV = NULL;
}
CubeMapApp::~CubeMapApp()
{

}

void CubeMapApp::cleanUp()
{
	AltarApp::cleanUp();
	SafeDelete(m_sky);
	for (int i = 0; i < 6; i++)
		ReleaseCOM(m_dynamicCubeMapRTV[i]);
	ReleaseCOM(m_dynamicCubeMapSRV);
	ReleaseCOM(m_dynamicCubeMapDSV);
}

bool CubeMapApp::initD3D(HWND windowId, int width, int height)
{
	if (!AltarApp::initD3D(windowId, width, height))
		return false;
	buildDynamicCubeMapViews();
	return true;
}

void CubeMapApp::buildCubeFaceCamera(float x, float y, float z)
{
	// Generate the cube map about the given position.
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		m_cubeMapCamera[i].lookAt(XMLoadFloat3(&center), XMLoadFloat3(&targets[i]), XMLoadFloat3(&ups[i]));
		m_cubeMapCamera[i].setLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
		m_cubeMapCamera[i].updateViewMatrix();
	}
}

void CubeMapApp::buildDynamicCubeMapViews()
{
	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HR(m_d3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;

		HR(m_d3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &m_dynamicCubeMapRTV[i]));
	}

	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(m_d3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &m_dynamicCubeMapSRV));

	ReleaseCOM(cubeTex);

	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	HR(m_d3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(m_d3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &m_dynamicCubeMapDSV));

	ReleaseCOM(depthTex);

	//
	// Viewport for drawing into cubemap.
	// 

	m_cubeMapViewport.TopLeftX = 0.0f;
	m_cubeMapViewport.TopLeftY = 0.0f;
	m_cubeMapViewport.Width = (float)CubeMapSize;
	m_cubeMapViewport.Height = (float)CubeMapSize;
	m_cubeMapViewport.MinDepth = 0.0f;
	m_cubeMapViewport.MaxDepth = 1.0f;
}

void CubeMapApp::initScene(int width, int height)
{
	AltarApp::initScene(width, height);
	m_sky = new Sky(m_d3dDevice, L"./Data/Images/desertcube1024.dds", 5000.0f);

	buildCubeFaceCamera(0.0f, 2.0f, 0.0f);

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&m_centerSphereWorld, XMMatrixMultiply(centerSphereScale, centerSphereOffset));
}

void CubeMapApp::renderScene()
{
	ID3D11RenderTargetView* renderTargets[1];

	// Generate the cube map.
	m_d3dDevContext->RSSetViewports(1, &m_cubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		// Clear cube map face and depth buffer.
		m_d3dDevContext->ClearRenderTargetView(m_dynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Silver));
		m_d3dDevContext->ClearDepthStencilView(m_dynamicCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		renderTargets[0] = m_dynamicCubeMapRTV[i];
		m_d3dDevContext->OMSetRenderTargets(1, renderTargets, m_dynamicCubeMapDSV);

		// Draw the scene with the exception of the center sphere to this cube map face.
		drawScene(m_cubeMapCamera[i], false);
	}

	// Restore old viewport and render targets.
	m_d3dDevContext->RSSetViewports(1,&m_screenViewport);
	renderTargets[0] = m_renderTargetView;
	m_d3dDevContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView);

	// Have hardware generate lower mipmap levels of cube map.
	m_d3dDevContext->GenerateMips(m_dynamicCubeMapSRV);

	// Now draw the scene as normal, but with the center sphere.
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	drawScene(*m_camera, true);
	HR(m_swapChain->Present(0, 0));
}


void CubeMapApp::drawScene(const Camera& camera, bool drawCenterSphere)
{
	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX view = camera.getViewMatrix();
	XMMATRIX proj = camera.getProjMatrix();

	// Set per frame constants.
	XMFLOAT3 eyePosW(camera.position.x, camera.position.y, camera.position.z);
	Effects::BasicFX->SetDirLights(&m_dirLight[0]);
	Effects::BasicFX->SetEyePosW(eyePosW);

	// Figure out which technique to use.  Skull does not have texture coordinates,
	// so we need a separate technique for it.
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light1Tech;
	ID3DX11EffectTechnique* activeCenterSphereTech = Effects::BasicFX->Light1RefTech;

	switch (m_lightCount)
	{
	case 1:
		activeTexTech = Effects::BasicFX->Light1TexTech;
		activeSkullTech = Effects::BasicFX->Light1Tech;
		activeCenterSphereTech = Effects::BasicFX->Light1RefTech;
		break;
	case 2:
		activeTexTech = Effects::BasicFX->Light2TexTech;
		activeSkullTech = Effects::BasicFX->Light2Tech;
		activeCenterSphereTech = Effects::BasicFX->Light2RefTech;
		break;
	case 3:
		activeTexTech = Effects::BasicFX->Light3TexTech;
		activeSkullTech = Effects::BasicFX->Light3Tech;
		activeCenterSphereTech = Effects::BasicFX->Light3RefTech;
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

	if (drawCenterSphere)
	{
		activeCenterSphereTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			// Draw the center sphere.

			m_d3dDevContext->IASetVertexBuffers(0, 1, &m_shapesVB, &stride, &offset);
			m_d3dDevContext->IASetIndexBuffer(m_shapesIB, DXGI_FORMAT_R32_UINT, 0);

			XMMATRIX world = XMLoadFloat4x4(&m_centerSphereWorld);
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(m_materials[5].data);
			Effects::BasicFX->SetCubeMap(m_dynamicCubeMapSRV);

			activeCenterSphereTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(m_sphereIndexCount, m_sphereIndexOffset, m_sphereVertexOffset);
		}
	}
	m_sky->Draw(m_d3dDevContext, camera);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dDevContext->RSSetState(0);
	m_d3dDevContext->OMSetDepthStencilState(0, 0);
}