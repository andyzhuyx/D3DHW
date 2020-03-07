cbuffer cBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix proj;
}

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