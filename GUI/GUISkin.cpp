#include "GUI.h"
#include "GUIReader.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUISkin::Create(const std::string_view &directory, const std::string &fileName) {
		Destroy();

		m_SkinDirectory = directory;

		GUIReader skinFile;
		if (!skinFile.Create(m_SkinDirectory + "/" + fileName)) {
			return false;
		}
		GUIProperties *currentSection = nullptr;

		while (!skinFile.GetStream()->eof()) {
			std::string line = skinFile.ReadLine();
			if (!line.empty()) {
				if (line.front() == '[' && line.back() == ']') {
					std::string sectionName = line.substr(1, line.size() - 2);
					GUIProperties *section = new GUIProperties(sectionName);
					m_Properties.try_emplace(sectionName, section);
					currentSection = section;
				} else {
					size_t equalPos = line.find('=');
					if (currentSection && equalPos != std::string::npos) {
						std::string propName = skinFile.TrimString(line.substr(0, equalPos));
						std::string propValue = skinFile.TrimString(line.substr(equalPos + 1, std::string::npos));
						currentSection->AddProperty(propName, propValue);
					}
				}
			}
		}
		for (auto &[cursorType, cursorData] : m_MouseCursors) {
			std::string bitmapFileName;
			int maskColor;
			if (GetValue(cursorData.first, "Filename", &bitmapFileName) && GetValue(cursorData.first, "ColorKeyIndex", &maskColor)) {
				if (GUIBitmap *bitmap = CreateBitmap(bitmapFileName)) {
					bitmap->SetColorKey(/*ConvertColor(ColorKey)*/);
					cursorData.second = bitmap;
				}
			}
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISkin::Destroy() {
		for (const auto &[propName, property] : m_Properties) {
			delete property;
		}
		m_Properties.clear();

		for (const auto &[fontName, font] : m_FontCache) {
			font->Destroy();
			delete font;
		}
		m_FontCache.clear();

		for (const auto &[bitmapFileName, bitmap] : m_BitmapCache) {
			delete bitmap;
		}
		m_BitmapCache.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIFont * GUISkin::GetFont(const std::string &fontName) {
		if (std::unordered_map<std::string, GUIFont *>::iterator foundFont = m_FontCache.find(fontName); foundFont != m_FontCache.end()) {
			return (*foundFont).second;
		} else {
			GUIFont *newFont = new GUIFont(fontName);
			if (!newFont->Load(m_Screen, m_SkinDirectory + "/" + fontName)) {
				delete newFont;
				return nullptr;
			}
			m_FontCache.try_emplace(fontName, newFont);
			return newFont;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUISkin::GetValue(const std::string &sectionName, const std::string &propName, std::string *propValue) {
		if (std::unordered_map<std::string, GUIProperties *>::iterator foundProp = m_Properties.find(sectionName); foundProp != m_Properties.end()) {
			const GUIProperties *property = (*foundProp).second;
			if (property->GetName() == sectionName && property->GetPropertyValue(propName, propValue)) {
				return true;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUISkin::GetValue(const std::string &sectionName, const std::string &propName, int *propValue) {
		if (std::unordered_map<std::string, GUIProperties *>::iterator foundProp = m_Properties.find(sectionName); foundProp != m_Properties.end()) {
			const GUIProperties *property = (*foundProp).second;
			if (property->GetName() == sectionName && property->GetPropertyValue(propName, propValue)) {
				return true;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUISkin::GetValue(const std::string &sectionName, const std::string &propName, int *propValueArray, int arraySize) {
		if (std::unordered_map<std::string, GUIProperties *>::iterator foundProp = m_Properties.find(sectionName); foundProp != m_Properties.end()) {
			GUIProperties *property = (*foundProp).second;
			if (property->GetName() == sectionName && property->GetPropertyValue(propName, propValueArray, arraySize)) {
				return true;
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIBitmap * GUISkin::CreateBitmap(const std::string &fileName) {
		if (std::unordered_map<std::string, GUIBitmap *>::iterator foundBitmap = m_BitmapCache.find(fileName); foundBitmap != m_BitmapCache.end()) {
			return (*foundBitmap).second;
		} else {
			if (GUIBitmap *newBitmap = m_Screen->CreateBitmap(m_SkinDirectory + "/" + fileName)) {
				m_BitmapCache.try_emplace(fileName, newBitmap);
				return newBitmap;
			}
			return nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUISkin::BuildStandardRect(GUIBitmap *destBitmap, const std::string &sectionName, int posX, int posY, int width, int height, bool buildBG, bool buildFrame, GUIRect *borderSizes) {
		// TODO: Figure out how to refactor and break this down. Also figure out a better name.
		// Note: For a control to use a 'Standard Rect' it must use the 8 side names, a filler name and a filename property.

		std::string bitmapFileName;
		GetValue(sectionName, "Filename", &bitmapFileName);
		GUIBitmap *srcBitmap = CreateBitmap(bitmapFileName);

		// Set the color key to be the same color as the Top-Right hand corner pixel
		srcBitmap->SetColorKey(srcBitmap->GetPixel(srcBitmap->GetWidth() - 1, 0));
		destBitmap->DrawRectangle(posX, posY, width, height, srcBitmap->GetPixel(srcBitmap->GetWidth() - 1, 0), true);
		destBitmap->SetColorKey(srcBitmap->GetPixel(srcBitmap->GetWidth() - 1, 0));

		std::array<int, 4> top = { 0, 0, 0, 0 };
		GetValue(sectionName, "Top", top.data(), top.size());

		std::array<int, 4> right = { 0, 0, 0, 0 };
		GetValue(sectionName, "Right", right.data(), right.size());

		std::array<int, 4> bottom = { 0, 0, 0, 0 };
		GetValue(sectionName, "Bottom", bottom.data(), bottom.size());

		std::array<int, 4> left = { 0, 0, 0, 0 };
		GetValue(sectionName, "Left", left.data(), left.size());

		int i = 0;
		int j = 0;
		GUIRect rect;

		if (buildBG) {
			std::array<int, 4> filler = { 0, 0, 0, 0 };
			GetValue(sectionName, "Filler", filler.data(), filler.size());

			// Use the filler first
			SetRect(&rect, filler[0], filler[1], filler[0] + filler[2], filler[1] + filler[3]);
			// Tile the filler across
			for (j = posY + top[3]; j < posY + height - bottom[3]; j += filler[3]) {
				for (i = posX + left[2]; i < posX + width - right[2]; i += filler[2]) {
					srcBitmap->DrawTrans(destBitmap, i, j, &rect);
				}
			}
		}

		if (buildFrame) {
			std::array<int, 4> topLeft = { 0, 0, 0, 0 };
			GetValue(sectionName, "TopLeft", topLeft.data(), topLeft.size());

			std::array<int, 4> topRight = { 0, 0, 0, 0 };
			GetValue(sectionName, "TopRight", topRight.data(), topRight.size());

			std::array<int, 4> bottomRight = { 0, 0, 0, 0 };
			GetValue(sectionName, "BottomRight", bottomRight.data(), bottomRight.size());

			std::array<int, 4> bottomLeft = { 0, 0, 0, 0 };
			GetValue(sectionName, "BottomLeft", bottomLeft.data(), bottomLeft.size());

			// Tile the four sides first, then place the four corners last

			// Tile the Top side
			SetRect(&rect, top[0], top[1], top[0] + top[2], top[1] + top[3]);
			for (i = posX + topLeft[2]; i <= posX + width - topRight[2]; i += top[2]) {
				srcBitmap->DrawTrans(destBitmap, i, posY, &rect);
			}

			// Tile the Right side
			SetRect(&rect, right[0], right[1], right[0] + right[2], right[1] + right[3]);
			for (j = posY + topRight[3]; j < posY + height - bottomRight[3]; j += right[3]) {
				srcBitmap->DrawTrans(destBitmap, posX + width - right[2], j, &rect);
			}

			// Tile the Bottom side
			SetRect(&rect, bottom[0], bottom[1], bottom[0] + bottom[2], bottom[1] + bottom[3]);
			for (i = posX + bottomLeft[2]; i < posX + width - bottomRight[2]; i += bottom[2]) {
				srcBitmap->DrawTrans(destBitmap, i, posY + height - bottom[3], &rect);
			}

			// Tile the Left side
			SetRect(&rect, left[0], left[1], left[0] + left[2], left[1] + left[3]);
			for (j = posY + topLeft[3]; j < posY + height - bottomLeft[3]; j += left[3]) {
				srcBitmap->DrawTrans(destBitmap, posX, j, &rect);
			}

			// Top-Left Corner
			SetRect(&rect, topLeft[0], topLeft[1], topLeft[0] + topLeft[2], topLeft[1] + topLeft[3]);
			srcBitmap->DrawTrans(destBitmap, posX, posY, &rect);

			// Top-Right Corner
			SetRect(&rect, topRight[0], topRight[1], topRight[0] + topRight[2], topRight[1] + topRight[3]);
			srcBitmap->DrawTrans(destBitmap, posX + width - topRight[2], posY, &rect);

			// Bottom-Right Corner
			SetRect(&rect, bottomRight[0], bottomRight[1], bottomRight[0] + bottomRight[2], bottomRight[1] + bottomRight[3]);
			srcBitmap->DrawTrans(destBitmap, posX + width - bottomRight[2], posY + height - bottomRight[3], &rect);

			// Bottom-Left Corner
			SetRect(&rect, bottomLeft[0], bottomLeft[1], bottomLeft[0] + bottomLeft[2], bottomLeft[1] + bottomLeft[3]);
			srcBitmap->DrawTrans(destBitmap, posX, posY + height - bottomLeft[3], &rect);
		}

		if (borderSizes) { SetRect(borderSizes, left[2], top[3], right[2], bottom[3]); }
	}
}