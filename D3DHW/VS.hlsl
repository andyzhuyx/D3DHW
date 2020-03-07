#include "Shader.hlsli"

VertexPosHTex main(VertexPosTex vin) {
	VertexPosHTex vout;
	vout.posH = float4(vin.pos, 1.0f);
	vout.posH = mul(vout.posH, world);
	vout.posH = mul(vout.posH, view);
	vout.posH = mul(vout.posH, proj);
	vout.tex = vin.tex;
	return vout;
}