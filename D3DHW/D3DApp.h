#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <string>
#include "Timer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class GameApp {
public:
	GameApp(HINSTANCE hInst);
	virtual ~GameApp();

	HINSTANCE GetInstance()const;
	HWND GetWindow()const;
	float GetAspectRatio()const;

	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float timeInterval) = 0;
	virtual void Draw() = 0;
	//virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	bool InitWindow();
	bool InitDirect3D();
protected:
	HINSTANCE _AppInstance;
	HWND _MainWindow;
	bool _IsAppPaused;
	bool _IsResizing;
	bool _IsMinimized;
	bool _IsMaximized;
	bool _Enable4xMsaa;
	UINT _4xMsaaQuality;

	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D11Device> _pDevice;
	ComPtr<ID3D11DeviceContext> _pDeviceContext;
	ComPtr<IDXGISwapChain> _pSwapChain;

	ComPtr<ID3D11Texture2D> _pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> _pDepthStencilView;
	D3D11_VIEWPORT _viewport;

	std::wstring _windowTitle;

	int _width;
	int _height;

	// timer
	Timer _gameTimer;
	// keyboard
	// mouse
};