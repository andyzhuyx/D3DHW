#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
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

struct PTMeshData {
	std::vector<VertexPosTex> vertices;
	std::vector<DWORD> indices;
};

struct PNTMeshData {
	std::vector<VertexPosNormalTex> vertices;
	std::vector<DWORD> indices;
};

PTMeshData CreatePlane(DirectX::XMFLOAT3* vpos, DirectX::XMFLOAT2* vtext);