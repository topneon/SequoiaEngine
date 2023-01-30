#pragma once

namespace seq {
	class Object {
	public:
		virtual ~Object() = default;
		virtual void Init() = 0;
		virtual void Tick() = 0;
		virtual void Destroy() = 0;
	protected:
		Object() = default;
	};
}