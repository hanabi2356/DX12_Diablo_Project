#include "Application.h"
#include<windowsx.h>
#include<minwindef.h>

using Microsoft::WRL::ComPtr;  //wrl.h include해야 사용가능
using namespace DirectX;  //DirectXMath.h include 해야 사용가능

LRESULT CALLBACK 
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Application::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}
Application* Application::mApp = nullptr;
Application::Application(HINSTANCE hInstance)
	:mApphInstance(hInstance)
{
	assert(mApp == nullptr);
	mApp = this;
}
void Application::CalculateFrameState()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;  //Milli-Second Per Frame
		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = mMainWndCaption + L"fps : " + fpsStr + L"mspf" + mspfStr;

		SetWindowText(mMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
Application* Application::GetApp()
{
	return mApp;
}

LRESULT Application::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:  //윈도우의 활성화 상태 또는 비활성화 상태의 메시지를 넘겨준다
		if (LOWORD(wParam) == WA_INACTIVE)  //윈도우 비활성화 여부 확인
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;
	case WM_SIZE:  //윈도우의 사이즈를 조정하거나 MoveWindow를 호출 할 때 호출 된다
		mClientWidth = LOWORD(lParam);  //새로운 width를 가져온다
		mClientHeight = HIWORD(lParam);  //새로운 height를 가져온다
		if (m3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)  //윈도우를 작업 표시줄로 내렸을 때
			{
				mAppPaused = true;
				mMinimized = true;
				mMaxmized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)  //원도우를 최대화 했을 때
			{
				mAppPaused = false;
				mMinimized = false;
				mMaxmized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)  //윈도우가 원래대로 돌아왔을 때 이전 상태에 따라 처리가 달라진다
			{
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();

				}
				else if (mMaxmized)
				{
					mAppPaused = false;
					mMaxmized = false;
					OnResize();

				}
				else
				{
					OnResize();

				}
			}
			return 0;

		}
	case WM_ENTERSIZEMOVE:  //사용자가 윈도우 크기를 조절하거나 이동하기 시작할 때 전송되는 메시지
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;
	case WM_MENUCHAR:  //alt + enter 입력 시 beep 음을 내지 않기 위한 코드
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:  //윈도우의 최소 크기를 x=200, y=200으로 고정
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
int Application::Run()
{
	MSG msg = { 0 };
	mTimer.Rest();  //루프 시작전에 타이머를 초기화 한다

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))  //데드 타임(Dead-Time) 루프 기법을 사용하여 메시지 큐에 메시지가 없을 때도 작업을 한다
		{
			TranslateMessage(&msg);  //전달된 메시지를 문자 메시지로 바꾼다
			DispatchMessage(&msg);  //전달된 메시지를 MsgProc로 넘겨준다
		}
		else
		{
			mTimer.Tick();  //매 프레임 마다 DeltaTime 계산
			if (!mAppPaused)  //애플리케이션이 Pause 상태가 아닐 때 렌더, 업데이트, 프레임 계산을 실행한다
			{
				CalculateFrameState();
				Update(mTimer);
				Draw(mTimer);
			}
			else  //Pause 상태일 경우 Sleep을 호출하여 스레드를 대기상태로 전환한다
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}
void Application::CreateRtvAndDsvDescriptorHeaps()  //서술자 힙 생성
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}
bool Application::InitMainWindow()
{
	WNDCLASS wc;  //명세서
	wc.style = CS_HREDRAW | CS_VREDRAW;  //윈도우의 크기가 변하면 화면을 다시 그리도록 설정한다
	wc.lpfnWndProc = MainWndProc;  //이 윈도우에서 발생하는 모든 메시지를 처리할 함수를 등록한다
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mApphInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";
	if (!RegisterClass(&wc))  //설정한 명세서를 등록한다. 등록에 성공해야만 창을 띄울 수 있다
	{
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return false;
	}

	RECT r = { 0,mClientWidth, mClientHeight };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	mMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0,0,mApphInstance,0);

	if (!mMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed", 0, 0);
		return false;
	}

	ShowWindow(mMainWnd, SW_SHOW);  //윈도우를 화면에 나타나게 해줌
	UpdateWindow(mMainWnd);  //윈도우가 처음 그려질 때 WM_PAINT 메시지 발생
	return false;
}

bool Application::InitDirect3D()
{
#if defined(DEBUG) | defined(_DEBUG)
{
	ComPtr<ID3D12Debug>debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
}
#endif
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));
	D3D_FEATURE_LEVEL featurLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,  //최신
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0  //최소 마지노선
	};
	HRESULT hr = E_FAIL;
	for (auto level : featurLevels)
	{
		hr = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&m3dDevice));
		if (SUCCEEDED(hr))
		{
			mFeatureLevel = level;
			break;
		}
	}

	if (FAILED(hr))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter>pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), mMinimumFeatureLevel, IID_PPV_ARGS(&m3dDevice)));
		mFeatureLevel = mMinimumFeatureLevel;
	}
	
	ThrowIfFailed(m3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	
	mRtvDescriptorSize = m3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = m3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = m3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level");

	//여기 부터 작업 진행


	return true;
}

void Application::CreateSwapChain()
{
	mSwapChain.Reset();
	
	Microsoft::WRL::ComPtr<IDXGIAdapter>adapter;
	Microsoft::WRL::ComPtr<IDXGIOutput>output;

	mdxgiFactory->EnumAdapters(0, &adapter);
	adapter->EnumOutputs(0, &output);

	Microsoft::WRL::ComPtr<IDXGIOutput1>output1;
	ThrowIfFailed(output.As(&output1));

	
	DXGI_MODE_DESC1 targetDesc;
	targetDesc.RefreshRate.Numerator = 60;
	targetDesc.RefreshRate.Denominator = 1;
	targetDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	targetDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width = mClientWidth;
	sd.Height = mClientHeight;
	sd.Format = mBackBufferFormat;
	sd.Stereo = false;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_MODE_DESC1 closestMatch;
	ThrowIfFailed(output1->FindClosestMatchingMode1(&targetDesc, &closestMatch, m3dDevice.Get()));

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd;
	fsd.RefreshRate = closestMatch.RefreshRate;
	fsd.ScanlineOrdering = closestMatch.ScanlineOrdering;
	fsd.Scaling = closestMatch.Scaling;
	fsd.Windowed = true;
	
	Microsoft::WRL::ComPtr<IDXGISwapChain1>swapChain1;
	ThrowIfFailed(mdxgiFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), mMainWnd, &sd, &fsd, nullptr, &swapChain1));

	ThrowIfFailed(swapChain1.As(&mSwapChain));  //보관은 SwapChain에한다 
}

void Application::OnResize()
{
	assert(m3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; i++)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	ThrowIfFailed(mSwapChain->ResizeBuffers(SwapChainBufferCount, mClientWidth, mClientHeight, mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	
	mCurrBackBuffer = 0;
	
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

}
