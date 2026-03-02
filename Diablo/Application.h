#pragma once
#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>
#endif

#include"GameTimer.h"
#include<windows.h>
#include<cassert>
#include<wrl.h>
#include<DirectXMath.h>
#include<d3d12.h>
#include<string>
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
	GameTimer mTimer;

	Microsoft::WRL::ComPtr<ID3D12Device>m3dDevice;
	
	wstring mMainWndCaption = L"Diablo App";
	int mClientWidth = 800;
	int mClientHeight = 600;
protected:
	Application(HINSTANCE hInstance);
	bool InitMainWindow();
	
public:
	static Application* GetApp();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();

};

