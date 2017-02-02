#include "TerrainApp.h"
TerrainApp::TerrainApp() :m_sky(NULL)
{

}
bool TerrainApp::initD3D(HWND windowId, int width, int height)
{
	if (!D3DApp::initD3D(windowId,width, height))
		return false;
	return true;
}
void TerrainApp::initScene(int width, int height)
{
	D3DApp::initScene(width, height);
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

	m_sky = new Sky(m_d3dDevice, L"./Data/Images/grasscube1024.dds", 5000.0f);
}
void TerrainApp::renderScene()
{
	m_d3dDevContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dDevContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDevContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_d3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_terrain.Draw(m_d3dDevContext, *m_camera, m_dirLight.data());

	m_d3dDevContext->RSSetState(0);

	m_sky->Draw(m_d3dDevContext, *m_camera);

	// restore default states, as the SkyFX changes them in the effect file.
	m_d3dDevContext->RSSetState(0);
	m_d3dDevContext->OMSetDepthStencilState(0, 0);

	HR(m_swapChain->Present(0, 0));
}
void TerrainApp::cleanUp()
{
	SafeDelete(m_sky);
}
void TerrainApp::updateScene(GameTimer*gameTimer)
{
}

void TerrainApp::keyPressEvent(QKeyEvent*event)
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
	default:
		break;
	}
}
