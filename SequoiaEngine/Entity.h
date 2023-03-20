#pragma once
#include "Object.h"
#include "Transformer.h"
#include "Mesh.h"

namespace seq { 
	class Entity : public Object
	{
		Transformer* transform;
		Mesh* mesh;
	public:
		Entity();
		~Entity();
		void Init() override;
		void Tick() override;
		void Destroy() override;
		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMFLOAT3 GetRotation();
		DirectX::XMFLOAT3 GetScale();
		void SetPosition(DirectX::XMFLOAT3);
		void SetRotation(DirectX::XMFLOAT3);
		void SetScale(DirectX::XMFLOAT3);
	};
}