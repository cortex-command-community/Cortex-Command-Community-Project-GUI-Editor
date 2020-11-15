#include "ContentFile.h"
#include "RTEError.h"

namespace RTE {

	const string ContentFile::m_ClassName = "ContentFile";
	std::unordered_map<std::string, BITMAP *> ContentFile::m_LoadedBitmaps;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const char *filePath) {
		m_DataPath = filePath;
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const ContentFile &reference) {
		m_DataPath = reference.m_DataPath;
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::FreeAllLoaded() {
		for (const std::pair<std::string, BITMAP *> &loadedBitmap : m_LoadedBitmaps) {
			destroy_bitmap(loadedBitmap.second);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP * ContentFile::GetAsBitmap(int conversionMode) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		BITMAP *returnBitmap = nullptr;

		std::unordered_map<std::string, BITMAP *>::iterator foundBitmap = m_LoadedBitmaps.find(m_DataPath);
		if (foundBitmap != m_LoadedBitmaps.end()) {
			returnBitmap = (*foundBitmap).second;
		} else {
			returnBitmap = LoadAndReleaseBitmap(conversionMode);
			m_LoadedBitmaps.insert({ m_DataPath, returnBitmap });
		}

		return returnBitmap;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP * ContentFile::LoadAndReleaseBitmap(int conversionMode) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		set_color_conversion(conversionMode == 0 ? COLORCONV_MOST : conversionMode);

		PALETTE currentPalette;
		get_palette(currentPalette);
		return load_bmp(m_DataPath.c_str(), currentPalette);
	}
}