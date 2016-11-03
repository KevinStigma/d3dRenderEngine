#include "Vertex.h"
ID3D11InputLayout* InputLayouts::PosNormal = 0;
void InputLayouts::initAll(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	
	D3DX11_PASS_DESC passDesc;
	technique->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosNormal));
}

void InputLayouts::destroyAll()
{
	ReleaseCOM(PosNormal);
}