#include <Windows.h>
#include "CRender.h"

CRender::~CRender()
{


}

CRender::CRender()
{
}
void CRender::CleanupRenderTarget()
{
	if (pRenderTargetView) { pRenderTargetView->Release(); pRenderTargetView = NULL; }
}
void CRender::CleanupDeviceD3D()
{

	CleanupRenderTarget();
	if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
	if (pDeviceContext) { pDeviceContext->Release(); pDeviceContext = nullptr; }
	if (pDevice) { pDevice->Release(); pDevice = nullptr; }
}
void CRender::CreateRenderTarget()
{
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);

	// Create the render target
	ID3D11Texture2D* pBackBuffer;
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	render_target_view_desc.Format = sd.BufferDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &pRenderTargetView);
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	pBackBuffer->Release();
}
