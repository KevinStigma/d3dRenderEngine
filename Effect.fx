cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};
struct VertexIn
{
	float3 Pos :POSITION;
	float3 Normal:NORMAL;
};

struct VertexOut
{
	float4 PosH:SV_POSITION;
	float4 Color:COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	//Transform to homogeneous clip space.
	vout.PosH=mul(float4(vin.Pos,1.0f),gWorldViewProj);
	
	//Just pass vertex color into the pixel shader.
	vout.Color=float4(1.0f,1.0f,1.0f,1.0f);
	return vout;
}

float4 PS(VertexOut pin):SV_Target
{
	return pin.Color;
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0,VS()));
		SetPixelShader(CompileShader(ps_5_0,PS()));
	}
}