#include "D3DApp.h"

namespace {
	GameApp* msgptr;
}

LRESULT CALLBACK WindowMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return msgptr->MsgProc(hwnd, msg, wParam, lParam);
}

GameApp::GameApp(HINSTANCE hInst)
	:
	_AppInstance(hInst),
	_MainWindow(nullptr),
	_IsAppPaused(false),
	_IsResizing(false),
	_IsMinimized(false),
	_IsMaximized(false),
	_Enable4xMsaa(true),
	_4xMsaaQuality(0),
	_pDevice(nullptr),
	_pDeviceContext(nullptr),
	_pSwapChain(nullptr),
	_pDepthStencilBuffer(nullptr),
	_pRenderTargetView(nullptr),
	_pDepthStencilView(nullptr),
	_windowTitle(L"D3DAPP"),
	_width(800),
	_height(600),
	kbd()
{
	ZeroMemory(&_viewport, sizeof(_viewport));
	msgptr = this;
}

GameApp::~GameApp(){
	if (!_pDeviceContext)
		_pDeviceContext->ClearState();
}

HINSTANCE GameApp::GetInstance()const {
	return _AppInstance;
}

HWND GameApp::GetWindow()const {
	return _MainWindow;
}

float GameApp::GetAspectRatio()const {
	return (float)(_width) / (float)(_height);
}

int GameApp::Run() {
	MSG msg = { 0 };
	_gameTimer.Reset();
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			_gameTimer.Tick();
			if (_IsAppPaused) {
				Sleep(100);
			}
			else {
				UpdateScene(_gameTimer.GetDeltaTime());
				Draw();
			}
		}
	}
	return (int)msg.wParam;
}

bool GameApp::Init() {
	if (!InitWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

void GameApp::OnResize() {
	_pDepthStencilBuffer.Reset();
	_pRenderTargetView.Reset();
	_pDepthStencilView.Reset();

	// Reset BackBuffer
	// Reset render target view
	ComPtr<ID3D11Texture2D> backBuffer;
	_pSwapChain->ResizeBuffers(1, _width, _height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, _pRenderTargetView.GetAddressOf());
	backBuffer.Reset();

	// Reset Depth buffer and view
	D3D11_TEXTURE2D_DESC dbd = { 0 };
	dbd.Usage = D3D11_USAGE_DEFAULT;
	dbd.Width = _width;
	dbd.Height = _height;
	dbd.MipLevels = 1;
	dbd.ArraySize = 1;
	dbd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (_Enable4xMsaa) {
		dbd.SampleDesc.Count = 4;
		dbd.SampleDesc.Quality = _Enable4xMsaa - 1;
	}
	else {
		dbd.SampleDesc.Count = 1;
		dbd.SampleDesc.Quality = 0;
	}
	dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dbd.CPUAccessFlags = 0;
	dbd.MiscFlags = 0;

	_pDevice->CreateTexture2D(&dbd, nullptr, _pDepthStencilBuffer.GetAddressOf());
	_pDevice->CreateDepthStencilView(_pDepthStencilBuffer.Get(), nullptr, _pDepthStencilView.GetAddressOf());

	_pDeviceContext->OMSetRenderTargets(1, _pRenderTargetView.GetAddressOf(), _pDepthStencilView.Get());

	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width = (float)_width;
	_viewport.Height = (float)_height;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	_pDeviceContext->RSSetViewports(1, &_viewport);
}

LRESULT GameApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			_IsAppPaused = true;
			_gameTimer.Stop();
		}
		else {
			_IsAppPaused = false;
			_gameTimer.Start();
		}
		return 0;

	case WM_SIZE:
		_width = LOWORD(lParam);
		_height = LOWORD(lParam);
		if (_pDevice) {
			if(wParam == SIZE_MINIMIZED) {
				_IsAppPaused = false;
				_IsMinimized = true;
				_IsMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				_IsMaximized = true;
				_IsAppPaused = false;
				_IsMinimized = false;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED) {
				if (_IsMinimized) {
					_IsAppPaused = false;
					_IsMinimized = false;
					OnResize();
				}
				else if(_IsMaximized) {
					_IsAppPaused = false;
					_IsMaximized = false;
					OnResize();
				}
				else if(_IsResizing){}
				else {
					OnResize();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		_IsAppPaused = true;
		_IsResizing = true;
		_gameTimer.Stop();
		OnResize();
		return 0;

	case WM_EXITSIZEMOVE:
		_IsAppPaused = false;
		_IsResizing = false;
		_gameTimer.Start();
		OnResize();
		return 0;


	/*Keyboard Event*/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool GameApp::InitWindow() {
	// Register
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _AppInstance;
	wc.hIcon = NULL;
	wc.hbrBackground = NULL;
	wc.hCursor = NULL;
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DAPP";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"Register Window ERROR", 0, 0);
		return false;
	}

	// Create
	RECT rt = { 0 };
	rt.top = 0;
	rt.left = 0;
	rt.bottom = rt.top + _height;
	rt.right = rt.left + _width;
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	int width = rt.right - rt.left;
	int depth = rt.bottom - rt.top;

	_MainWindow = CreateWindow(L"D3DApp", _windowTitle.c_str(), WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN)- _width) / 2, 
		(GetSystemMetrics(SM_CYSCREEN) - _height) / 2, 
		width, depth, 0, 0, _AppInstance, 0);

	if (!_MainWindow) {
		MessageBox(0, L"Create Window ERROR", 0, 0);
		return false;
	}
	ShowWindow(_MainWindow, SW_SHOW);
	UpdateWindow(_MainWindow);
	return true;

}

bool GameApp::InitDirect3D() {
	HRESULT flag = S_OK;
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;	// Direct2D需要支持BGRA格式
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	flag = D3D11CreateDevice(
		0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags,
		0, 0, D3D11_SDK_VERSION, _pDevice.GetAddressOf(),
		0, _pDeviceContext.GetAddressOf()
	);

	if (FAILED(flag)) {
		MessageBox(0, L"Create Device and Context ERROR", 0, 0);
		return false;
	}

	_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);
	if (_4xMsaaQuality == 4) {
		_Enable4xMsaa = true;
	}
	else {
		_Enable4xMsaa = false;
	}
	// Find Factory to create swapchain
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ComPtr<IDXGIFactory> dxgiFactory = nullptr;
	_pDevice.As(&dxgiDevice);
	dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
	
	// Swap chain desc
	DXGI_SWAP_CHAIN_DESC sw = { 0 };
	sw.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sw.BufferDesc.Width = _width;
	sw.BufferDesc.Height = _height;
	sw.BufferDesc.RefreshRate.Numerator = 60;
	sw.BufferDesc.RefreshRate.Denominator = 1;
	sw.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sw.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	if (_Enable4xMsaa) {
		sw.SampleDesc.Count = _4xMsaaQuality;
		sw.SampleDesc.Quality = _4xMsaaQuality - 1;
	}
	else {
		sw.SampleDesc.Count = 1;
		sw.SampleDesc.Quality = 0;
	}

	sw.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sw.BufferCount = 1;
	sw.OutputWindow = _MainWindow;
	sw.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sw.Flags = 0;
	sw.Windowed = true;

	flag = dxgiFactory->CreateSwapChain(_pDevice.Get(), &sw, _pSwapChain.GetAddressOf());
	if (FAILED(flag)) {
		MessageBox(0, L"Create Swapchain ERROR", 0, 0);
		return 0;
	}

	OnResize();
	return true;
}