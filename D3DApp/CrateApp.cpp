#include "CrateApp.h"
#include "GlobalSys.h"

void CrateApp::cleanUp()
{
	D3DApp::cleanUp();
	ReleaseCOM(m_boxTexView);
}

void CrateApp::loadTextures()
{
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		L"./Data/Images/box1.jpg",0,0,&m_boxTexView,0));
}

void CrateApp::renderScene()
{
	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, bgColor);

	//Refresh the Depth/Stencil view
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_squareVertexBuffer&&m_squareIndiceBuffer)
	{
		BasicEffect*basicEffect = Effects::BasicFX;

		XMFLOAT3 eyePosW(m_camera->position.x, m_camera->position.y, m_camera->position.z);
		basicEffect->SetDirLights(&m_dirLight[0]);
		basicEffect->SetEyePosW(eyePosW);

		UINT stride = sizeof(Vertex::Basic32);
		UINT offset = 0;
		m_d3dDevContext->IASetVertexBuffers(0, 1, &m_squareVertexBuffer, &stride, &offset);
		m_d3dDevContext->IASetIndexBuffer(m_squareIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
		m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX rotScaleMat = XMLoadFloat4x4(&m_transformMat);
		XMMATRIX translateMat = XMMatrixTranslation(m_translateX, m_translateY, 0.0f);
		XMMATRIX I = XMMatrixIdentity();
		XMMATRIX worldMat = I;

		ID3DX11EffectTechnique* activeTech = basicEffect->Light2Tech;
		if (m_hasTex)
			activeTech = basicEffect->Light0TexTech;
		D3DX11_TECHNIQUE_DESC techDesc;
		activeTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			XMMATRIX WVP;
			WVP = worldMat* rotScaleMat* translateMat* m_camera->getViewMatrix() * m_camera->getProjMatrix();
			basicEffect->SetWorld(worldMat);
			basicEffect->SetWorldInvTranspose(worldMat);
			basicEffect->SetWorldViewProj(WVP);
			basicEffect->SetMaterial(m_materials[0].data);
			basicEffect->SetTexTransform(I);
			basicEffect->SetDiffuseMap(m_boxTexView);

			activeTech->GetPassByIndex(p)->Apply(0, m_d3dDevContext);
			m_d3dDevContext->DrawIndexed(g_pGlobalSys->objects[0].mesh.indices.size(), 0, 0);
		}
	}
	//Present the backbuffer to the screen
	m_swapChain->Present(0, 0);
}