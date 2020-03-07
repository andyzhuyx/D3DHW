#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
struct VertexPosTex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;

	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosNormalTex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;

	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};