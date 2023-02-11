#pragma once
#include <d3d11_1.h>
class CRender
{
public:
	HWND					 windowHandle = (HWND)0;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;

	ID3D11Device* pDevice = nullptr;
	ID3D11BlendState* pBlender = nullptr;
	ID3D11SamplerState* pSampler = nullptr;
public:

	~CRender();
	CRender();


	void CleanupRenderTarget();


	void CleanupDeviceD3D();

	void CreateRenderTarget();

	void presentCallback(void* chain);


};

extern void PresentCallBack(void* chain);

