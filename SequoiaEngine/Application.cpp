#include "Application.h"

namespace seq {

	void Application::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
		this->hInstance = hInstance;
		this->hPrevInstance = hPrevInstance;
		this->lpCmdLine = lpCmdLine;
		this->nCmdShow = nCmdShow;
		Init();
		Tick();
		Destroy();
	}

	void Application::Init() {
		// right
		msg = {};
		device = NULL;
		deviceContext = NULL;
		swapChain = NULL;
		renderTargetView = NULL;
		dxgiFactory = NULL;
		dxgiAdapter = NULL;
		dxgiDevice = NULL;
		clearColor[0] = 0.25f; clearColor[1] = 0.45f; clearColor[2] = 0.65f; clearColor[3] = 1.00f;
		// Some class bs for Windows
		WNDPROC wndProc = WndProc;
		wc = { sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, wndProc, 0, 0, hInstance, (HICON)NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, windowClass, NULL };
		RegisterClassEx(&wc);

		// Set DPI Awareness 
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		screenSettings = {};

		screenSettings.x = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		screenSettings.y = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		DEVMODE devmodea;
		devmodea.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devmodea);
		screenSettings.refreshRate = devmodea.dmDisplayFrequency;

		// Basic Window
		hwnd = CreateWindow(wc.lpszClassName, L"SequoiaEngine", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);
		
		// Create flags for Device
		UINT createFlags = 0;
#ifdef DEBUG
		createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create DX11 Device 
		auto hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createFlags, 0, 0, D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);
		//statics.device = device;
		//statics.deviceContext = deviceContext;

		if (FAILED(hr)) {
			MessageBox(hwnd, L"Failed to create DX11 Device", L"Troubleshooter", MB_OK);
			return;
		}

		if (featureLevel < D3D_FEATURE_LEVEL_11_0) {
			MessageBox(hwnd, L"Your hardware is too old! You should get some GPU with DirectX 11 support ASAP.", L"Troubleshooter", MB_OK);
			return;
		}

		// MSAA and Texture AA for 2x 4x 8x 16x 32x
		// device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 32, &msaa8Quality);
		// msaa32Quality--;
		// no need
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 16, &msaa16Quality);
		msaa16Quality--;
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 8, &msaa8Quality);
		msaa8Quality--;
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa4Quality);
		msaa4Quality--;
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 2, &msaa2Quality);
		msaa2Quality--;

		// Create Swap Chain
		graphicsSettings.msaa = 2;
		swapChainDesc = {};
		swapChainDesc.Windowed = FALSE;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = screenSettings.x;
		swapChainDesc.BufferDesc.Height = screenSettings.y;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = screenSettings.refreshRate;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		if (graphicsSettings.msaa == 1) {
			swapChainDesc.SampleDesc.Count = 2;
			swapChainDesc.SampleDesc.Quality = msaa2Quality;
		}
		if (graphicsSettings.msaa == 2) {
			swapChainDesc.SampleDesc.Count = 4;
			swapChainDesc.SampleDesc.Quality = msaa4Quality;
		}
		if (graphicsSettings.msaa == 3) {
			swapChainDesc.SampleDesc.Count = 8;
			swapChainDesc.SampleDesc.Quality = msaa8Quality;
		}
		if (graphicsSettings.msaa == 4) {
			swapChainDesc.SampleDesc.Count = 16;
			swapChainDesc.SampleDesc.Quality = msaa16Quality;
		}
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		swapChainDesc.Flags = 0;

		if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice))) {
			MessageBox(hwnd, L"Failed to get DXGIDevice from ID3D11Device", L"Troubleshooter", MB_OK);
			return;
		}
		if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter))) {
			MessageBox(hwnd, L"Failed to get DXGIAdapter from DXGIDevice", L"Troubleshooter", MB_OK);
			return;
		}
		if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory))) {
			MessageBox(hwnd, L"Failed to get DXGIFactory from DXGIAdapter", L"Troubleshooter", MB_OK);
			return;
		}
		if (FAILED(dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain))) {
			MessageBox(hwnd, L"Failed to create swap chain", L"Troubleshooter", MB_OK);
			return;
		}

		// Release DXGI
		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();

		// Create Render Target View
		if (FAILED(swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
			MessageBox(hwnd, L"Failed to get back buffer", L"Troubleshooter", MB_OK);
			return;
		}
		
		if (FAILED(device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView))) {
			MessageBox(hwnd, L"Failed to create render target", L"Troubleshooter", MB_OK);
			return;
		}
		//statics.renderTargetView = renderTargetView;

		backBuffer->Release();
		// We can make the buffer getting method later

		// Create Texture for depth and buffer...
		// ...
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = screenSettings.x;
		depthStencilDesc.Height = screenSettings.y;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		if (graphicsSettings.msaa == 1) {
			depthStencilDesc.SampleDesc.Count = 2;
			depthStencilDesc.SampleDesc.Quality = msaa2Quality;
		}
		if (graphicsSettings.msaa == 2) {
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = msaa4Quality;
		}
		if (graphicsSettings.msaa == 3) {
			depthStencilDesc.SampleDesc.Count = 8;
			depthStencilDesc.SampleDesc.Quality = msaa8Quality;
		}
		if (graphicsSettings.msaa == 4) {
			depthStencilDesc.SampleDesc.Count = 16;
			depthStencilDesc.SampleDesc.Quality = msaa16Quality;
		}
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
		device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 0.0f;
		viewport.Width = screenSettings.x;
		viewport.Height = screenSettings.y;

		deviceContext->RSSetViewports(1, &viewport);

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		// draw the initial gray window
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
		// Set Topology
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
		// Cube
		Vertex cube[]{
			{ -1.0f, 1.0f, -1.0f },    // vertex 0
			{ 1.0f, 1.0f, -1.0f },     // vertex 1
			{ -1.0f, -1.0f, -1.0f },   // 2
			{ 1.0f, -1.0f, -1.0f },  // 3
			{ -1.0f, 1.0f, 1.0f },     // ...
			{ 1.0f, 1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f }
		};
		Indice indices[]{
			0, 1, 2,    // side 1
			2, 1, 3,
			4, 0, 6,    // side 2
			6, 0, 2,
			7, 5, 6,    // side 3
			6, 5, 4,
			3, 1, 7,    // side 4
			7, 1, 5,
			4, 5, 0,    // side 5
			0, 5, 1,
			3, 7, 2,    // side 6
			2, 7, 6,
		};

		// DirectX::XMMatrixLookAtRH()
		// shaderVertDesc = NULL;
		shaderVertDesc.push_back(std::vector<D3D11_INPUT_ELEMENT_DESC>{});
		shaderVertDesc[0].push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		shaderVertDesc[0].push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 12 });
		shaderVertDesc[0].push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 24 });
		shaderVertDesc[0].push_back({ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 32 });
		
		//device->CreateInputLayout(shaderVertDesc[0].data(), 4, )
		swapChain->Present(1, 0);
	}

	void Application::Tick() {
		// message loop
		while (GetMessage(&msg, NULL, 0, 0))
		{
			__int64 fps;
			QueryPerformanceFrequency((LARGE_INTEGER*)&fps);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Application::Destroy() {
		renderTargetView->Release();
		renderTargetView = nullptr;
		swapChain->Release();
		// swapChain = nullptr;
		deviceContext->Release();
		// deviceContext = nullptr;
		device->Release();
		// device = nullptr;

		DestroyWindow(hwnd);
		UnregisterClass(windowClass, wc.hInstance);
	}

	LRESULT WINAPI Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			if (MessageBox(hwnd, L"Are you sure you want to close?", L"Confirm Close", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
				DestroyWindow(hwnd);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
	}
}