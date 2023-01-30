#pragma once

namespace seq {
	struct ScreenSettings {
		uint16_t x;
		uint16_t y;
		uint8_t refreshRate;
		uint8_t format; // 0x00 = FULLSCREEN, 0x01 = WINDOWED, 0x11 = BORDERLESS WINDOWED
	};

	struct GraphicsSettings {
		uint8_t msaa; // MSAA
		uint8_t aa2d; // aa for textures
	};
}