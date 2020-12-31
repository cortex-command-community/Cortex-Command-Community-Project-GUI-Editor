#include "EditorUtil.h"

#include <Windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <direct.h>

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int EditorUtil::DisplayDialogBox(const std::string &message, const HWND &windowHandle) {
		PlaySound("SystemExclamation", nullptr, SND_ASYNC);
		int result = MessageBox(windowHandle, message.c_str(), "Cortex Command GUI Editor", MB_YESNOCANCEL);

		if (result == IDNO) { return -1; }
		if (result == IDYES) { return 1; }
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorUtil::DisplayLoadFileDialogBox(std::string &filename, const HWND &windowHandle) {
		OPENFILENAMEA dialogBox;
		std::string filenameToLoad(MAX_PATH, '\0'); // Make sure the string is initialized with the correct size and filled with null characters.

		std::string currentDir(MAX_PATH, '\0');
		currentDir = std::filesystem::current_path().string();

		ZeroMemory(&dialogBox, sizeof(OPENFILENAME)); // Initialize OPENFILENAME
		dialogBox.lStructSize = sizeof(OPENFILENAME);
		dialogBox.hInstance = nullptr;
		dialogBox.hwndOwner = windowHandle;
		dialogBox.lpstrFile = filenameToLoad.data();
		dialogBox.nMaxFile = filenameToLoad.size();
		dialogBox.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		dialogBox.nFilterIndex = 1;
		dialogBox.lpstrFileTitle = nullptr;
		dialogBox.nMaxFileTitle = 0;
		dialogBox.lpstrTitle = "Open";
		dialogBox.lpstrInitialDir = currentDir.data();
		dialogBox.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		dialogBox.lpstrDefExt = "ini";

		if (GetOpenFileName(&dialogBox)) {
			filename = std::string(filenameToLoad.data());
			std::filesystem::current_path(currentDir);
			return true;
		}
		std::filesystem::current_path(currentDir);
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorUtil::DisplaySaveFileDialogBox(std::string &filename, const HWND &windowHandle) {
		OPENFILENAMEA dialogBox;
		std::string filenameToSave(MAX_PATH, '\0'); // Make sure the string is initialized with the correct size and filled with null characters.

		std::string currentDir(MAX_PATH, '\0');
		currentDir = std::filesystem::current_path().string();

		ZeroMemory(&dialogBox, sizeof(OPENFILENAME)); // Initialize OPENFILENAME
		dialogBox.lStructSize = sizeof(OPENFILENAME);
		dialogBox.hInstance = nullptr;
		dialogBox.hwndOwner = windowHandle;
		dialogBox.lpstrFile = filenameToSave.data();
		dialogBox.nMaxFile = filenameToSave.size();
		dialogBox.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		dialogBox.nFilterIndex = 1;
		dialogBox.lpstrFileTitle = nullptr;
		dialogBox.nMaxFileTitle = 0;
		dialogBox.lpstrTitle = "Save As";
		dialogBox.lpstrInitialDir = currentDir.data();
		dialogBox.Flags = OFN_PATHMUSTEXIST;
		dialogBox.lpstrDefExt = "ini";

		if (GetSaveFileName(&dialogBox)) {
			// Check if the file exists
			FILE *file = fopen(filenameToSave.c_str(), "rt");
			if (file) {
				fclose(file);
				if (MessageBox(windowHandle, "File Exists\nOverwrite it?", "Confirmation", MB_YESNO) == IDNO) {
					std::filesystem::current_path(currentDir);
					return false;
				}
			}
			filename = filenameToSave;
			std::filesystem::current_path(currentDir);
			return true;
		}
		// Restore the current working directory
		std::filesystem::current_path(currentDir);
		return false;
	}
}