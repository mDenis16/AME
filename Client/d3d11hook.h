#pragma once


#define D3D11_HOOK_API

struct ID3D11Device; // from d3d11.h
struct ID3D11DeviceContext; // from d3d11.h
struct IDXGISwapChain; // from d3d11.h

// Use for rendering graphical user interfaces (for example: ImGui) or other.
extern D3D11_HOOK_API void ImplHookDX11_Present(ID3D11Device* device, ID3D11DeviceContext* ctx, IDXGISwapChain* swap_chain);

// Use for initialize hook.
D3D11_HOOK_API void	       ImplHookDX11_Init(HMODULE hModule, void* hwnd);

// Use for untialize hook (ONLY AFTER INITIALIZE).
D3D11_HOOK_API void	       ImplHookDX11_Shutdown();

CFactory(CD3DRender)
{
	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	
	static HRESULT Present(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags);
	static void  DrawIndexedHook(ID3D11DeviceContext * pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	static void  D3D11CreateQuery(ID3D11Device * pDevice, const D3D11_QUERY_DESC * pQueryDesc, ID3D11Query * *ppQuery);
	static void  ClearRenderTargetViewHook(ID3D11DeviceContext * pContext, ID3D11RenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4]);
	static void D3D11PSSetShaderResources(ID3D11DeviceContext * pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
};
CEnd(CD3DRender)