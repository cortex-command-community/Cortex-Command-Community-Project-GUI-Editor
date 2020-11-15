#include "GUIUtil.h"
#ifdef WIN32
#include <Windows.h>
#endif

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetRect(GUIRect *pRect, int left, int top, int right, int bottom) {
		pRect->left = left;
		pRect->top = top;
		pRect->right = right;
		pRect->bottom = bottom;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	char *GUIUtil::TrimString(char *String) {
		char *ptr = String;

		// Find the first non-space character
		while (*ptr) {
			if (*ptr != ' ') { break; }
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

	char* GUIUtil::SafeOverlappingStrCpy(char* dst, char* src) {
		memmove(dst, src, strlen(src) + 1);
		return dst;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIUtil::GetClipboardText(std::string *Text) { return 0; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIUtil::SetClipboardText(std::string Text) { return 0; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
	bool WinUtil::GetClipboardText(std::string *Text) {
		HANDLE CBDataHandle; // handle to the clipboard data
		LPSTR CBDataPtr;    // pointer to data to send

		// Check the pointer
		assert(Text);

		// Does the clipboard contain text?
		if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(0)) {
			CBDataHandle = GetClipboardData(CF_TEXT);

			if (CBDataHandle) {
				CBDataPtr = (LPSTR)GlobalLock(CBDataHandle);
				int TextSize = strlen(CBDataPtr);

				// Insert the text
				Text->erase();
				Text->insert(0, CBDataPtr);
				CloseClipboard();

				GlobalUnlock(CBDataHandle);

				return true;
			}
			CloseClipboard();

		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool WinUtil::SetClipboardText(std::string Text) {
		// Open the clipboard
		if (OpenClipboard(0)) {
			// Allocate global memory for the text
			HGLOBAL hMemory = GlobalAlloc(GMEM_MOVEABLE, Text.size() + 1);
			if (hMemory == 0) {
				CloseClipboard();
				return false;
			}
			// Empty the clipboard
			EmptyClipboard();

			// Copy the text into memory
			char *CText = (char *)GlobalLock(hMemory);
			memcpy(CText, Text.c_str(), Text.size());
			CText[Text.size()] = '\0';
			GlobalUnlock(hMemory);

			// Set the data
			SetClipboardData(CF_TEXT, hMemory);
			CloseClipboard();
			return true;
		}
		return false;
	}
#endif
}