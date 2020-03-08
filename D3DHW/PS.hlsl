#include "Shader.hlsli"

float4 main(VertexPosHTex v) : SV_TARGET{
	float4 texColor = _Texture.Sample(_Sampler, v.tex);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return texColor;
}