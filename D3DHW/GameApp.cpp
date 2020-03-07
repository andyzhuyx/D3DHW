#include "GameApp.h"

Game::Game(HINSTANCE hInst)
	:GameApp(hInst)
{}

bool  Game::Init() {
	if (!GameApp::Init())
		return false;
	if (!InitShader())
		return false;
	return true;
}

Game::~Game() {

}

void Game::UpdateScene(float dt) {

}

void Game::Draw() {
	static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)
	_pDeviceContext->ClearRenderTargetView(_pRenderTargetView.Get(), blue);
	_pDeviceContext->ClearDepthStencilView(_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_pSwapChain->Present(0, 0);
}

void Game::OnResize(){
	GameApp::OnResize();
}

bool Game::InitShader() {
	HRESULT flag = S_OK;
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> err;
	// Compile and Create VS
	flag = D3DCompileFromFile(L"VS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, blob.ReleaseAndGetAddressOf(), err.ReleaseAndGetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"VS Compile ERROR", 0, 0);
		return false;
	}
	flag = _pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, _pVertexShader.GetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"Create VS ERROR", 0, 0);
		return false;
	}
	// Input Layout
	_pDevice->CreateInputLayout(VertexPosTex::inputLayout,
		ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(),
		_pInputLayout.GetAddressOf());

	// Compile and Create PS
	flag = D3DCompileFromFile(L"PS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, blob.ReleaseAndGetAddressOf(), err.ReleaseAndGetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"Compile PS ERROR", 0, 0);
		return false;
	}
	flag = _pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, _pPixelShader.GetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"Create PS ERROR", 0, 0);
		return false;
	}

	return true;
}

bool Game::InitResource() {
	// Constant Buffer Set Up
	D3D11_BUFFER_DESC cbd = { 0 };

	return true;
}