#pragma once

#include "D3DApp.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <D3DCompiler.h>
#include "Vertex.h"
#include "Camera.h"

class Game : public GameApp{
public:
	Game(HINSTANCE hInst);
	~Game();
	virtual void UpdateScene(float dt);
	virtual void Draw();
	virtual bool Init();
	virtual void OnResize();

public:
	// Constant Buffer
	// World Buffer is changed every time we want to draw a obj
	struct CBufferWorld {
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInverseTranspose;
	};

	// Camera Buffer is changed every frame due to busy waiting
	struct CBufferCamera {
		DirectX::XMMATRIX view;
		DirectX::XMFLOAT4 eye;
	};

	// Projection Buffer is changed only if the view port changes
	struct CBufferProj {
		DirectX::XMMATRIX proj;
	};
private:
	bool InitResource();
	bool InitShader();
private:
	ComPtr<ID3D11InputLayout> _pInputLayout;
	ComPtr<ID3D11VertexShader> _pVertexShader;
	ComPtr<ID3D11Buffer> _pVertexBuffer;
	
	ComPtr<ID3D11Buffer> _pIndexBuffer;
	ComPtr<ID3D11PixelShader> _pPixelShader;

	ComPtr<ID3D11SamplerState> _pSamplerState;

	// Constant Buffer
	ComPtr<ID3D11Buffer> _pCWorld;
	ComPtr<ID3D11Buffer> _pCCamera;
	ComPtr<ID3D11Buffer> _pCProj;
	CBufferWorld _CWorld;
	CBufferCamera _CCamera;
	CBufferProj _CProj;
	// Camera
	FirstPersonCamera _FPCamera;
};
