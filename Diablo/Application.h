#pragma once
#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>
#endif

#include"GameTimer.h"
#include"AppUtility.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;


class Application
{
protected:
	static Application* mApp;
	HINSTANCE mApphInstance=nullptr;
	HWND mMainWnd = nullptr;

	bool mAppPaused=false;
	bool mMinimized = false;
	bool mMaxmized = false;
	bool mResizing = false;
	bool mFullScreenState = false;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;

	GameTimer mTimer;
	
	D3D_FEATURE_LEVEL mFeatureLevel;
	D3D_FEATURE_LEVEL mMinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	Microsoft::WRL::ComPtr<ID3D12Device>m3dDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain>mSwapChain;
	Microsoft::WRL::ComPtr<IDXGIFactory4>mdxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12Fence>mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>mCommandQueue;  //명령 대기열
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>mDirectCmdListAlloc; //명령 할당자 
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>mCommandList;  //명령 목록

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource>mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>mDsvHeap;
	
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	
	
	wstring mMainWndCaption = L"Diablo App";

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
protected:
	Application(HINSTANCE hInstance);
	virtual ~Application();
protected:
	virtual void OnResize();
	virtual void Update(const GameTimer& dt) = 0;
	virtual void Draw(const GameTimer& dt) = 0;

	virtual void CreateRtvAndDsvDescriptorHeaps();
protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandQueue();
	void CreateSwapChain();

	void FlushCommandQueue();

	void CalculateFrameState();
public:
	static Application* GetApp();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();

};

