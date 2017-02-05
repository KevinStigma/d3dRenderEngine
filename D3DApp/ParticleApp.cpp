#include "ParticleApp.h"
#include "GameTimer.h"
ParticleApp::ParticleApp() :m_sky(NULL), m_flareTexSRV(NULL), m_randomTexSRV(NULL)
{

}
void ParticleApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);
	m_camera->setPosition(0.0f, 2.0f, 100.0f);
	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"./Data/Images/terrain.raw";
	tii.LayerMapFilename0 = L"./Data/Images/grass.dds";
	tii.LayerMapFilename1 = L"./Data/Images/darkdirt.dds";
	tii.LayerMapFilename2 = L"./Data/Images/stone.dds";
	tii.LayerMapFilename3 = L"./Data/Images/lightdirt.dds";
	tii.LayerMapFilename4 = L"./Data/Images/snow.dds";
	tii.BlendMapFilename = L"./Data/Images/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	m_terrain.Init(m_d3dDevice, m_d3dDevContext, tii);

	m_randomTexSRV = d3dHelper::CreateRandomTexture1DSRV(m_d3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"./Data/Images/flare0.dds");
	m_flareTexSRV = d3dHelper::CreateTexture2DArraySRV(m_d3dDevice, m_d3dDevContext, flares);

	m_fire.Init(m_d3dDevice, Effects::FireFX, m_flareTexSRV, m_randomTexSRV, 500);
	m_fire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	m_sky = new Sky(m_d3dDevice, L"./Data/Images/grasscube1024.dds", 5000.0f);
}
void ParticleApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_terrain.Draw(m_d3dDevContext, *m_camera,m_dirLight.data());

	m_d3dDevContext->RSSetState(0);

	m_sky->Draw(m_d3dDevContext, *m_camera);

	// Draw particle systems last so it is blended with scene.
	m_fire.SetEyePos(m_camera->position);
	m_fire.Draw(m_d3dDevContext, *m_camera);
	m_d3dDevContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default

	// restore default states.
	m_d3dDevContext->RSSetState(0);
	m_d3dDevContext->OMSetDepthStencilState(0, 0);

	HR(m_swapChain->Present(0, 0));
}


void ParticleApp::cleanUp()
{
	SafeDelete(m_sky);
	ReleaseCOM(m_flareTexSRV);
	ReleaseCOM(m_randomTexSRV);
}
void ParticleApp::updateScene(GameTimer*gameTimer)
{
	m_fire.Update(gameTimer->DeltaTime(), gameTimer->TotalTime());
}
void ParticleApp::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Up:
		m_camera->walkForward(1.0f);
		break;
	case Qt::Key_Down:
		m_camera->walkForward(-1.0f);
		break;
	case Qt::Key_Right:
		m_camera->walkRight(1.0f);
		break;
	case Qt::Key_Left:
		m_camera->walkRight(-1.0f);
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
		m_camera->walkUp(1.0f);
		break;
	case Qt::Key_E:
		m_camera->walkUp(-1.0f);
		break;
	case Qt::Key_R:
		m_fire.Reset();
		break;
	default:
		break;
	}
}