#include "Vertex.h"
#include "Effects.h"
ID3D11InputLayout* InputLayouts::PosNorTex = 0;
ID3D11InputLayout* InputLayouts::Pos = 0;
void InputLayouts::initAll(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	
	D3DX11_PASS_DESC passDesc;
	Effects::BasicFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosNorTex));


	const D3D11_INPUT_ELEMENT_DESC vertexDesc2[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Effects::TessellationFX->TessTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc2, 1, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Pos));
}

void InputLayouts::destroyAll()
{
	ReleaseCOM(PosNorTex);
	ReleaseCOM(Pos);
}