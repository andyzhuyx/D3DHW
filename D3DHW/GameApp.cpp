#include "GameApp.h"
#include <sstream>
Game::Game(HINSTANCE hInst)
	:GameApp(hInst)
{}

bool  Game::Init() {
	if (!GameApp::Init())
		return false;
	if (!InitShader())
		return false;
	if (!InitResource())
		return false;
	return true;
}

Game::~Game() {

}

void Game::UpdateScene(float dt) {
	using namespace DirectX;
	auto cam1 = std::dynamic_pointer_cast<FirstPersonCamera>(_pFPCamera);
	// Keyboard test
	if (kbd.IsKeyPressed(static_cast<unsigned char>('W'))) {
		cam1->Walk(dt * 10);
	}
	if (kbd.IsKeyPressed(static_cast<unsigned char>('A'))) {
		cam1->LeftRightRotate(-dt);
	}
	if (kbd.IsKeyPressed(static_cast<unsigned char>('D'))) {
		cam1->LeftRightRotate(dt);
	}
	if (kbd.IsKeyPressed(static_cast<unsigned char>('S'))) {
		cam1->Walk(-dt * 10);
	}
	// Keyboard test


	cam1->UpdateViewMatrix();
	XMFLOAT4X4 tempView = cam1->GetViewMatrix();
	_CCamera.view = XMMatrixTranspose(DirectX::XMLoadFloat4x4(&tempView));
	XMFLOAT3 tempEye = cam1->GetPos();
	_CCamera.eye = XMFLOAT4(tempEye.x, tempEye.y, tempEye.z, 0.0f);

	HRESULT hr = S_OK;
	// Update Camera Buffer and Map it
	D3D11_MAPPED_SUBRESOURCE mappedCamera;
	hr = _pDeviceContext->Map(_pCCamera.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCamera);
	if (FAILED(hr)) {
		MessageBox(0, L"Update Camera ERROR", 0, 0);
	}
	memcpy_s(mappedCamera.pData, sizeof(CBufferCamera), &_CCamera, sizeof(CBufferCamera));
	_pDeviceContext->Unmap(_pCCamera.Get(), 0);
}

void Game::Draw() {
	static float blue[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,255,255)
	_pDeviceContext->ClearRenderTargetView(_pRenderTargetView.Get(), blue);
	_pDeviceContext->ClearDepthStencilView(_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (int i = 0; i < 6; i++) {
		_pWallsArr[i].Draw(_pDeviceContext.Get());
	}
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
	flag = _pDevice->CreateInputLayout(VertexPosTex::inputLayout,
		ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(),
		_pInputLayout.GetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"Init Inputlayout ERROR", 0, 0);
		return false;
	}
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
	using namespace DirectX;
	HRESULT hr = S_OK;
	// Constant Buffer Set Up
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// Const Buffer World
	cbd.ByteWidth = sizeof(CBufferWorld);
	hr = _pDevice->CreateBuffer(&cbd, nullptr, _pCWorld.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Const World Buffer ERROR", 0, 0);
		return false;
	}
	// Const Buffer Camera
	cbd.ByteWidth = sizeof(CBufferCamera);
	hr = _pDevice->CreateBuffer(&cbd, nullptr, _pCCamera.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Const Camera Buffer ERROR", 0, 0);
		return false;
	}
	// Const Buffer Projection
	cbd.ByteWidth = sizeof(CBufferCamera);
	hr = _pDevice->CreateBuffer(&cbd, nullptr, _pCProj.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Const Projection Buffer ERROR", 0, 0);
		return false;
	}

	/*----------------Init Vertex and Index-----------------------*/
	// Topology
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Input Layout
	_pDeviceContext->IASetInputLayout(_pInputLayout.Get());

	// Vertex Shader
	_pDeviceContext->VSSetShader(_pVertexShader.Get(), nullptr, 0);
	// Pixel Shader
	_pDeviceContext->PSSetShader(_pPixelShader.Get(), nullptr, 0);

	// Constant Buffer
		auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
		_pFPCamera = camera;
		camera->SetViewPort(0, 0, (float)_width, (float)_height);
		camera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		// Project Matrix
		_pFPCamera->SetFrustum(XM_PI / 3, GetAspectRatio(), 0.5f, 1000.0f);
		XMFLOAT4X4 tempProj = _pFPCamera->GetProjMatrix();
		_CProj.proj = XMMatrixTranspose(XMLoadFloat4x4(&tempProj));
		// Update Project Matrix to the pointer
		D3D11_MAPPED_SUBRESOURCE mappedProj;
		hr = _pDeviceContext->Map(_pCProj.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedProj);
		if (FAILED(hr)) {
			MessageBox(0, L"Map Proj ERROR", 0, 0);
			return false;
		}
		memcpy_s(mappedProj.pData, sizeof(CBufferProj), &_CProj, sizeof(CBufferProj));
		_pDeviceContext->Unmap(_pCProj.Get(), 0);

	/*----------------Init Vertex and Index-----------------------*/
		// Bind Constant Buffer
	_pDeviceContext->VSSetConstantBuffers(0, 1, _pCWorld.GetAddressOf());
	_pDeviceContext->VSSetConstantBuffers(1, 1, _pCCamera.GetAddressOf());
	_pDeviceContext->VSSetConstantBuffers(2, 1, _pCProj.GetAddressOf());



	// Initial Wall World Picture
	XMFLOAT3 floor_v[4] = {
		XMFLOAT3(-200.0f, -20.0f, -200.0f),
		XMFLOAT3(-200.0f, -20.0f, 200.0f),
		XMFLOAT3(200.0f, -20.0f, 200.0f),
		XMFLOAT3(200.0f, -20.0f, -200.0f)
	};

	XMFLOAT3 left_v[4] = {
		XMFLOAT3(-200.0f, -20.0f, -200.0f),
		XMFLOAT3(-200.0f, 80.0f, -200.0f),
		XMFLOAT3(-200.0f, 80.0f, 200.0f),
		XMFLOAT3(-200.0f, -20.0f, 200.0f)
	};

	XMFLOAT3 front_v[4] = {
		XMFLOAT3(-200.0f, -20.0f, 200.0f),
		XMFLOAT3(-200.0f, 80.0f, 200.0f),
		XMFLOAT3(200.0f, 80.0f, 200.0f),
		XMFLOAT3(200.0f, -20.0f, 200.0f)
	};

	XMFLOAT3 right_v[4] = {
		XMFLOAT3(200.0f, -20.0f, 200.0f),
		XMFLOAT3(200.0f, 80.0f, 200.0f),
		XMFLOAT3(200.0f, 80.0f, -200.0f),
		XMFLOAT3(200.0f, -20.0f, -200.0f)
	};

	XMFLOAT3 back_v[4] = {
		XMFLOAT3(200.0f, -20.0f, -200.0f),
		XMFLOAT3(200.0f, 80.0f, -200.0f),
		XMFLOAT3(-200.0f, 80.0f, -200.0f),
		XMFLOAT3(-200.0f, -20.0f, -200.0f)
	};

	XMFLOAT3 top_v[4] = {
		XMFLOAT3(-200.0f, 80.0f, 200.0f),
		XMFLOAT3(-200.0f, 80.0f, -200.0f),
		XMFLOAT3(200.0f, 80.0f, -200.0f),
		XMFLOAT3(200.0f, 80.0f, 200.0f)
	};
	/*
	XMFLOAT2 floor_t[4] = {
		XMFLOAT2(45.0f / 178.0f, 1.0f),
		XMFLOAT2(45.0f / 178.0f, 89.0f / 135.0f),
		XMFLOAT2(89.0f / 178.0f, 89.0f / 135.0f),
		XMFLOAT2(89.0f / 178.0f, 1.0f)
	};
	XMFLOAT2 left_t[4] = {
		XMFLOAT2(0.0f, 89.0f / 135.0f),
		XMFLOAT2(0.0f, 45.0f / 135.0f),
		XMFLOAT2(45.0f / 178.0f, 45.0f / 135.0f),
		XMFLOAT2(45.0f / 178.0f, 89.0f / 135.0f)
	};
	XMFLOAT2 front_t[4] = {
		XMFLOAT2(45.0f / 178.0f, 89.0f / 135.0f),
		XMFLOAT2(45.0f / 178.0f, 45.0f / 135.0f),
		XMFLOAT2(89.0f / 178.0f,45.0f / 135.0f),
		XMFLOAT2(89.0f / 178.0f, 89.0f / 135.0f)
	};
	XMFLOAT2 right_t[4] = {
		XMFLOAT2(89.0f / 178.0f,89.0f / 135.0f),
		XMFLOAT2(89.0f / 178.0f,45.0f / 135.0f),
		XMFLOAT2(133.5f / 178.0f,45.0f / 135.0f),
		XMFLOAT2(133.5f / 178.0f,89.0f / 135.0f)
	};
	XMFLOAT2 back_t[4] = {
		XMFLOAT2(133.5f / 178.0f,89.0f / 135.0f),
		XMFLOAT2(133.5f / 178.0f,45.0f / 135.0f),
		XMFLOAT2(1.0f,45.0f / 135.0f),
		XMFLOAT2(1.0f,89.0f / 135.0f)
	};
	XMFLOAT2 top_t[4] = {
		XMFLOAT2(45.0f / 178.0f, 45.0f / 135.0f),
		XMFLOAT2(45.0f / 178.0f,0.0f),
		XMFLOAT2(89.0f / 178.0f,0.0f),
		XMFLOAT2(89.0f / 178.0f,45.0f / 135.0f)
	};
	*/
	XMFLOAT2 floor_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
	XMFLOAT2 left_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
	XMFLOAT2 front_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
	XMFLOAT2 right_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
	XMFLOAT2 back_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
	XMFLOAT2 top_t[4] = {
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};
		// Position and Texture
	XMFLOAT4X4 iden;
	XMStoreFloat4x4(&iden, XMMatrixIdentity());
	/*
	ComPtr<ID3D11ShaderResourceView> texture;
	hr = CreateWICTextureFromFile(_pDevice.Get(), L"world.png", nullptr, texture.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Create Texture ERROR", 0, 0);
		return false;
	}*/
	ComPtr<ID3D11ShaderResourceView> t1;
	hr = CreateWICTextureFromFile(_pDevice.Get(), L"wall.png", nullptr, t1.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Create Texture ERROR1", 0, 0);
		return false;
	}
	ComPtr<ID3D11ShaderResourceView> t2;
	hr = CreateWICTextureFromFile(_pDevice.Get(), L"floor.png", nullptr, t2.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Create Texture ERROR2", 0, 0);
		return false;
	}
	ComPtr<ID3D11ShaderResourceView> t3;
	hr = CreateWICTextureFromFile(_pDevice.Get(), L"sky.jpg", nullptr, t3.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Create Texture ERROR23", 0, 0);
		return false;
	}
		// Set Buffer
	bool create_result = true;
	create_result = _pWallsArr[0].SetBufferPT(_pDevice.Get(), CreatePlane(floor_v, floor_t));
	if (!create_result) {
		MessageBox(0, L"Front Wall ERROR", 0, 0);
		return false;
	}
	create_result = _pWallsArr[1].SetBufferPT(_pDevice.Get(), CreatePlane(left_v, left_t));
	if (!create_result) {
		MessageBox(0, L"Left Wall ERROR", 0, 0);
		return false;
	}
	create_result = _pWallsArr[2].SetBufferPT(_pDevice.Get(), CreatePlane(front_v, front_t));
	if (!create_result) {
		MessageBox(0, L"Front Wall ERROR", 0, 0);
		return false;
	}
	create_result = _pWallsArr[3].SetBufferPT(_pDevice.Get(), CreatePlane(right_v, right_t));
	if (!create_result) {
		MessageBox(0, L"Right Wall ERROR", 0, 0);
		return false;
	}
	create_result = _pWallsArr[4].SetBufferPT(_pDevice.Get(), CreatePlane(back_v, back_t));
	if (!create_result) {
		MessageBox(0, L"Back Wall ERROR", 0, 0);
		return false;
	}
	create_result = _pWallsArr[5].SetBufferPT(_pDevice.Get(), CreatePlane(top_v, top_t));
	if (!create_result) {
		MessageBox(0, L"Top Wall ERROR", 0, 0);
		return false;
	}
		// Set Pos
	_pWallsArr[0].SetObjWorld(iden);
	_pWallsArr[1].SetObjWorld(iden);
	_pWallsArr[2].SetObjWorld(iden);
	_pWallsArr[3].SetObjWorld(iden);
	_pWallsArr[4].SetObjWorld(iden);
	_pWallsArr[5].SetObjWorld(iden);
		// Set Texture
	_pWallsArr[0].SetTexture(t2.Get());
	_pWallsArr[1].SetTexture(t1.Get());
	_pWallsArr[2].SetTexture(t1.Get());
	_pWallsArr[3].SetTexture(t1.Get());
	_pWallsArr[4].SetTexture(t1.Get());
	_pWallsArr[5].SetTexture(t3.Get());


	// Sampler
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	hr =_pDevice->CreateSamplerState(&sd, _pSamplerState.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Sampler State ERROR", 0, 0);
	}
	return true;
}


/*
	DrawObject functions
*/
Game::DrawObject::DrawObject()
	:_Stride(), 
	_NumOfIndex()
{
	using namespace DirectX;
	XMStoreFloat4x4(&_ObjWorld, XMMatrixIdentity());
}

DirectX::XMFLOAT3 Game::DrawObject::GetPos() const {
	return DirectX::XMFLOAT3(_ObjWorld(3, 0), _ObjWorld(3, 1), _ObjWorld(3, 2));
}

bool Game::DrawObject::SetBufferPT(ID3D11Device* device, const PTMeshData& dataset) {
	HRESULT hr = S_OK;

	_pVertexBuffer.Reset();
	_pIndexBuffer.Reset();
	// Vertex info
	_Stride = sizeof(VertexPosTex);
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.ByteWidth = (UINT)dataset.vertices.size() * _Stride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA arrData = { 0 };
	arrData.pSysMem = dataset.vertices.data();
	hr = device->CreateBuffer(&vbd, &arrData, _pVertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Vertex DrawObj ERROR", 0, 0);
		return false;
	}

	// Index info
	_NumOfIndex = (UINT)dataset.indices.size();
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = _NumOfIndex * sizeof(DWORD);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	arrData.pSysMem = dataset.indices.data();
	hr = device->CreateBuffer(&ibd, &arrData, _pIndexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(0, L"Index DrawObj ERROR", 0, 0);
		return false;
	}
	return true;
}

void Game::DrawObject::SetTexture(ID3D11ShaderResourceView* texture) {
	_pTexture = texture;
}

void Game::DrawObject::SetObjWorld(const DirectX::XMFLOAT4X4& world) {
	_ObjWorld = world;
}

void Game::DrawObject::Draw(ID3D11DeviceContext* deviceContext) {
	using namespace DirectX;
	// Set Vertex Buffer
	//UINT stride = _Stride;
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, _pVertexBuffer.GetAddressOf(), &_Stride, &offset);
	// Set Index Buffer
	deviceContext->IASetIndexBuffer(_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	// Set World Contant Buffer
	ComPtr<ID3D11Buffer> p_ConstantB = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, p_ConstantB.GetAddressOf());
	CBufferWorld constantB;
	
	constantB.world = XMMatrixTranspose(XMLoadFloat4x4(&_ObjWorld));
	constantB.worldInverseTranspose = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_ObjWorld));
	D3D11_MAPPED_SUBRESOURCE mapData;
	deviceContext->Map(p_ConstantB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapData);
	memcpy_s(mapData.pData, sizeof(constantB), &constantB, sizeof(constantB));
	deviceContext->Unmap(p_ConstantB.Get(), 0);

	// Texture
	deviceContext->PSSetShaderResources(0, 1, _pTexture.GetAddressOf());
	// Draw
	deviceContext->DrawIndexed(_NumOfIndex, 0, 0);
}