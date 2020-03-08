cbuffer CBWorld : register(b0) {
	matrix world;
	matrix worldInv;
}

cbuffer CBCamera : register(b1) {
	matrix view;
	matrix eye;
}

cbuffer CBProj : register(b2) {
	matrix proj;
}

SamplerState _Sampler : register(s0);
Texture2D _Texture : register(t0);
// for original input
struct VertexPosTex
{
	float3 pos: POSITION;
	float2 tex: TEXCOORD;
};

struct VertexPosNormalTex
{
	float3 pos: POSITION;
	float3 normal: NORMAL;
	float2 tex: TEXCOORD;
};

// for system view
struct VertexPosHTex {
	float4 posH: SV_POSITION;
	float2 tex: TEXCOORD;
};

struct VertexPosHNormalTex {
	float4 posH: SV_POSITION;
	float3 normal: NORMAL;
	float2 tex: TEXCOORD;
};