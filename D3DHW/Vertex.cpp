#include "Vertex.h"

const D3D11_INPUT_ELEMENT_DESC VertexPosTex::inputLayout[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC VertexPosNormalTex::inputLayout[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

PTMeshData CreatePlane(DirectX::XMFLOAT3* vpos, DirectX::XMFLOAT2* vtext) {
	using namespace DirectX;
	PTMeshData dataset;
	dataset.vertices.resize(4);
	dataset.indices.resize(6);

	// Vertices
	for (int i = 0; i < 4; i++) {
		dataset.vertices[i].pos = vpos[i];
		dataset.vertices[i].tex = vtext[i];
	}

	dataset.indices[0] = 0;
	dataset.indices[1] = 1;
	dataset.indices[2] = 2;
	dataset.indices[3] = 0;
	dataset.indices[4] = 2;
	dataset.indices[5] = 3;

	return dataset;
}