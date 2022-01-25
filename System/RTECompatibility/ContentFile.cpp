#include "ContentFile.h"
#include "RTEError.h"

namespace RTE {

	std::array<std::unordered_map<std::string, BITMAP *>, ContentFile::BitDepths::BitDepthCount> ContentFile::s_LoadedBitmaps;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::Clear() {
		m_DataPath.clear();
		m_DataPathExtension.clear();
		m_DataPathWithoutExtension.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const char *filePath) {
		SetDataPath(filePath);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const ContentFile &reference) {
		m_DataPath = reference.m_DataPath;
		m_DataPathExtension = reference.m_DataPathExtension;
		m_DataPathWithoutExtension = reference.m_DataPathWithoutExtension;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::FreeAllLoaded() {
		for (int depth = BitDepths::Eight; depth < BitDepths::BitDepthCount; ++depth) {
			for (const auto &[bitmapPath, bitmapPtr] : s_LoadedBitmaps.at(depth)) {
				destroy_bitmap(bitmapPtr);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::SetDataPath(const std::string &newDataPath) {
		m_DataPath = std::filesystem::path(newDataPath).generic_string();
		m_DataPathExtension = std::filesystem::path(m_DataPath).extension().string();

		RTEAssert(!m_DataPathExtension.empty(), "Failed to find file extension when trying to find file with path and name:\n" + m_DataPath);

		m_DataPathWithoutExtension = m_DataPath.substr(0, m_DataPath.length() - m_DataPathExtension.length());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP * ContentFile::GetAsBitmap(int conversionMode, bool storeBitmap, const std::string &dataPathToSpecificFrame) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		BITMAP *returnBitmap = nullptr;
		const int bitDepth = (conversionMode == COLORCONV_8_TO_32) ? BitDepths::ThirtyTwo : BitDepths::Eight;
		std::string dataPathToLoad = dataPathToSpecificFrame.empty() ? m_DataPath : dataPathToSpecificFrame;

		// Check if the file has already been read and loaded from the disk and, if so, use that data.
		std::unordered_map<std::string, BITMAP *>::iterator foundBitmap = s_LoadedBitmaps.at(bitDepth).find(dataPathToLoad);
		if (foundBitmap != s_LoadedBitmaps.at(bitDepth).end()) {
			returnBitmap = (*foundBitmap).second;
		} else {
			if (!std::filesystem::exists(dataPathToLoad)) {
				const std::string dataPathWithoutExtension = dataPathToLoad.substr(0, dataPathToLoad.length() - m_DataPathExtension.length());
				const std::string altFileExtension = (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (std::filesystem::exists(dataPathWithoutExtension + altFileExtension)) {
					SetDataPath(m_DataPathWithoutExtension + altFileExtension);
					dataPathToLoad = dataPathWithoutExtension + altFileExtension;
				} else {
					RTEAbort("Failed to find image file with following path and name:\n\n" + m_DataPath + " or " + altFileExtension);
				}
			}
			returnBitmap = LoadAndReleaseBitmap(conversionMode, dataPathToLoad); // NOTE: This takes ownership of the bitmap file

			// Insert the bitmap into the map, PASSING OVER OWNERSHIP OF THE LOADED DATAFILE
			if (storeBitmap) { s_LoadedBitmaps.at(bitDepth).insert({ dataPathToLoad, returnBitmap }); }
		}
		return returnBitmap;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP ** ContentFile::GetAsAnimation(int frameCount, int conversionMode) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		// Create the array of as many BITMAP pointers as requested frames
		BITMAP **returnBitmaps = new BITMAP *[frameCount];

		// Don't try to append numbers if there's only one frame
		if (frameCount == 1) {
			// Check for 000 in the file name in case it is part of an animation but the FrameCount was set to 1. Do not warn about this because it's normal operation, but warn about incorrect extension.
			if (!std::filesystem::exists(m_DataPath)) {
				const std::string altFileExtension = (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (std::filesystem::exists(m_DataPathWithoutExtension + "000" + m_DataPathExtension)) {
					SetDataPath(m_DataPathWithoutExtension + "000" + m_DataPathExtension);
				} else if (std::filesystem::exists(m_DataPathWithoutExtension + "000" + altFileExtension)) {
					SetDataPath(m_DataPathWithoutExtension + "000" + altFileExtension);
				}
			}
			returnBitmaps[0] = GetAsBitmap(conversionMode);
			return returnBitmaps;
		}
		char framePath[1024];
		for (int frameNum = 0; frameNum < frameCount; frameNum++) {
			std::snprintf(framePath, sizeof(framePath), "%s%03i%s", m_DataPathWithoutExtension.c_str(), frameNum, m_DataPathExtension.c_str());
			returnBitmaps[frameNum] = GetAsBitmap(conversionMode, true, framePath);
		}
		return returnBitmaps;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP * ContentFile::LoadAndReleaseBitmap(int conversionMode, const std::string &dataPathToSpecificFrame) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		const std::string dataPathToLoad = dataPathToSpecificFrame.empty() ? m_DataPath : dataPathToSpecificFrame;

		BITMAP *returnBitmap = nullptr;

		PALETTE currentPalette;
		get_palette(currentPalette);

		set_color_conversion((conversionMode == 0) ? COLORCONV_MOST : conversionMode);
		returnBitmap = load_bitmap(dataPathToLoad.c_str(), currentPalette);
		RTEAssert(returnBitmap, "Failed to load image file with path and name:\n" + m_DataPath);

		return returnBitmap;
	}
}