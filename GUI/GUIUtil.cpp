#include "GUIUtil.h"

#ifdef _WIN32
#include "Windows.h"
#endif

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	char * GUIUtil::TrimString(char *String) {
		char *ptr = String;
		// Find the first non-space character
		while (*ptr) {
			if (*ptr != ' ') {
				break;
			}
			ptr++;
		}
		// Add a null terminator after the last character
		for (int i = strlen(ptr) - 1; i >= 0; i--) {
			if (ptr[i] != ' ') {
				ptr[i + 1] = '\0';
				break;
			}
		}
		return ptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIUtil::GetClipboardText() {
#ifdef _WIN32
		HANDLE clipboardDataHandle;
		LPSTR clipboardData;
		std::string clipboardText = "";

		if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(nullptr)) {
			clipboardDataHandle = GetClipboardData(CF_TEXT);
			if (clipboardDataHandle) {
				clipboardData = static_cast<LPSTR>(GlobalLock(clipboardDataHandle));
				clipboardText = clipboardData;
				CloseClipboard();

				GlobalUnlock(clipboardDataHandle);
			}
			CloseClipboard();
		}
#elif __unix__
		// TODO: Implement.
#endif
		return clipboardText;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIUtil::SetClipboardText(const std::string &text) {
#ifdef _WIN32
		if (OpenClipboard(nullptr)) {
			// Allocate global memory for the text
			HGLOBAL hMemory = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
			if (hMemory == nullptr) {
				CloseClipboard();
				return false;
			}
			EmptyClipboard();

			// Copy the text into memory
			char *clipboardText = static_cast<char *>(GlobalLock(hMemory));
			memcpy(clipboardText, text.c_str(), text.size());
			clipboardText[text.size()] = '\0';
			GlobalUnlock(hMemory);

			SetClipboardData(CF_TEXT, hMemory);
			CloseClipboard();
			return true;
		}
#elif __unix__
		// TODO: Implement.
#endif
		return false;
	}
}