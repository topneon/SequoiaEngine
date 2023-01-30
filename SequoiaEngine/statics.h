#pragma once
#include "framework.h"

namespace seq {
	struct Statics {
		IDXGISwapChain* swapChain;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		ID3D11RenderTargetView* renderTargetView;
	};
}