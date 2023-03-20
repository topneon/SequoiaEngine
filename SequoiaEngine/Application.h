#pragma once
#include "framework.h"
#include "structs.h"
#include "statics.h"
#include "Object.h"
#include "Entity.h"

namespace seq {
	class Application : public Object
	{
	private:
		// WinMain
		HINSTANCE hInstance;
		HINSTANCE hPrevInstance;
		LPSTR lpCmdLine;
		int nCmdShow;
		//
		MSG msg;
		HWND hwnd;
		WNDCLASSEX wc;
		D3D11_VIEWPORT viewport;
		IDXGIDevice* dxgiDevice;
		IDXGIAdapter* dxgiAdapter;
		IDXGIFactory* dxgiFactory;
		IDXGISwapChain* swapChain;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11Texture2D* backBuffer, *depthStencilBuffer;
		ID3D11DepthStencilView* depthStencilView;
		D3D_FEATURE_LEVEL featureLevel;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		uint32_t msaa2Quality, msaa4Quality, msaa8Quality, msaa16Quality, msaa32Quality;
		ScreenSettings screenSettings;
		GraphicsSettings graphicsSettings;
		
		static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		void Init() override;
		void Tick() override;
		void Destroy() override;
		void Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		const wchar_t* windowClass = L"SequoiaEngine";
		float clearColor[4];
		//static Statics statics;
	};
}