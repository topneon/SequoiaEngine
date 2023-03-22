#pragma once
#include "framework.h"

namespace seq {
	namespace shortcut {

		template<class T>
		ID3D11Buffer* CreateBufferHR(T* v, ID3D11Device* device) {
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.ByteWidth = sizeof(T) * 8;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = NULL;
			desc.MiscFlags = NULL;
			desc.StructureByteStride = NULL;
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = v;
			ID3D11Buffer* buffer;
			HR(device->CreateBuffer(&desc, &data, &buffer));
			return buffer;
		}

		template<class T>
		ID3D11Buffer* CreateBuffer(T* v, ID3D11Device* device) {
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.ByteWidth = sizeof(T) * 8;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = NULL;
			desc.MiscFlags = NULL;
			desc.StructureByteStride = NULL;
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = v;
			ID3D11Buffer* buffer;
			device->CreateBuffer(&desc, &data, &buffer);
			return buffer;
		}

		template<class T>
		ID3D11Buffer* CreateBufferDynamic(T* v, ID3D11Device* device, uint8_t cpuMode = 0) {
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(T) * 8;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = (D3D11_CPU_ACCESS_WRITE * (cpuMode & 0x1)) | (D3D11_CPU_ACCESS_READ * ((cpuMode & 0x2) >> 1));
			desc.MiscFlags = NULL;
			desc.StructureByteStride = NULL;
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = v;
			ID3D11Buffer* buffer;
			device->CreateBuffer(&desc, &data, &buffer);
			return buffer;
		}

		
	}
}