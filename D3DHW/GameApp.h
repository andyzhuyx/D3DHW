#pragma once

#include "D3DApp.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <D3DCompiler.h>
#include "Vertex.h"
#include "Camera.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
class Game : public GameApp{
public:
	class DrawObject {
	public:
		DrawObject();

		DirectX::XMFLOAT3 GetPos() const;
		bool SetBufferPT(ID3D11Device* device, const PTMeshData& dataset);
		void SetTexture(ID3D11ShaderResourceView* texture);
		void SetObjWorld(const DirectX::XMFLOAT4X4& world);
		void Draw(ID3D11DeviceContext* deviceContext);
	private:
		ComPtr<ID3D11Buffer> _pVertexBuffer;
		ComPtr<ID3D11Buffer> _pIndexBuffer;
		ComPtr<ID3D11ShaderResourceView> _pTexture;
		UINT _Stride; // Vertex Byte Size				
		UINT _NumOfIndex;

		DirectX::XMFLOAT4X4 _ObjWorld;
	};

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
	std::shared_ptr<Camera> _pFPCamera;

	// World Picture
	DrawObject _pWallsArr[6];

};
