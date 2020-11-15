#ifndef _RTECONSTANTS_
#define _RTECONSTANTS_

namespace RTE {

#pragma region Graphics Constants
	enum ColorKeys {
		g_MaskColor = 0, //!< Mask color for all 8bpp bitmaps (palette index 0 (255,0,255)). This color is fully transparent.
		//g_MOIDMaskColor = 0, //!< Mask color for 8bpp MOID layer bitmaps (palette index 0 (255,0,255)). This color is fully transparent.
		g_MOIDMaskColor = 0xF81F, //!< Mask color for 16bpp MOID layer bitmaps (255,0,255). This color is fully transparent.
		//g_MOIDMaskColor = 0xFF00FF, //!< Mask color for 32bpp MOID layer bitmaps (255,0,255). This color is fully transparent.
		g_BlackColor = 245,
		g_WhiteColor = 254,
		g_RedColor = 13,
		g_YellowGlowColor = 117,
		g_NoMOID = 255
	};

	enum TransparencyPreset { LessTrans = 0, HalfTrans, MoreTrans };

	// GUI colors
	#define c_GUIColorWhite makecol(255, 255, 255)
	#define c_GUIColorYellow makecol(255, 255, 128)
	#define c_GUIColorRed makecol(255, 100, 100)
	#define c_GUIColorGreen makecol(128, 255, 128)
	#define c_GUIColorLightBlue makecol(109, 117, 170)
	#define c_GUIColorBlue makecol(59, 65, 83)
	#define c_GUIColorDarkBlue makecol(12, 20, 39)
	#define c_GUIColorGray makecol(232, 232, 248)
#pragma endregion

#pragma region Math Constants
	static constexpr float c_TwoPI = 6.28318531F;
	static constexpr float c_PI = 3.14159265F;
	static constexpr float c_HalfPI = 1.57079633F;
	static constexpr float c_QuarterPI = 0.78539816F;
	static constexpr float c_EighthPI = 0.39269908F;
	static constexpr float c_SixteenthPI = 0.19634954F;
#pragma endregion

#pragma region Un-Definitions
	// Allegro defines these via define in astdint.h and Boost with stdlib go crazy so we need to undefine them manually.
	#undef int8_t
	#undef uint8_t
	#undef int16_t
	#undef uint16_t
	#undef int32_t
	#undef uint32_t
	#undef intptr_t
	#undef uintptr_t
	#undef LONG_LONG
	#undef int64_t
	#undef uint64_t
#pragma endregion
}
#endif