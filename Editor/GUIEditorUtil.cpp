#include "GUIEditorUtil.h"

#include <Windows.h>
#include <commdlg.h>
#include <direct.h>

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorUtil::QuitMessageBox(const std::string &message, const std::string &title) {
		int nRetCode = MessageBox(nullptr, message.c_str(), title.c_str(), MB_YESNOCANCEL);

		if (nRetCode == IDNO) { return -1; }
		if (nRetCode == IDYES) { return 1; }
		return 0; // Cancel
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorUtil::DisplayLoadGUIFile(std::string *filename) {
		OPENFILENAMEA ofn; // common dialog box structure
		char szFile[260]; // File name

		// Save the current working directory
		char szCurrentDir[_MAX_PATH];
		_getcwd(szCurrentDir, _MAX_PATH);

		// Clear the filename (otherwise it won't work)
		memset(szFile, 0, sizeof(szFile));

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hInstance = nullptr;
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrTitle = "Open";
		ofn.lpstrInitialDir = szCurrentDir;
		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = "ini";

		if (GetOpenFileName(&ofn)) {
			*filename = std::string(szFile);
			_chdir(szCurrentDir);
			return true;
		}
		_chdir(szCurrentDir);
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorUtil::DisplaySaveGUIFile(std::string *filename) {
		OPENFILENAMEA ofn; // common dialog box structure
		char szFile[260]; // File name

		// Save the current working directory
		char szCurrentDir[_MAX_PATH];
		_getcwd(szCurrentDir, _MAX_PATH);

		// Clear the filename (otherwise it won't work)
		memset(szFile, 0, sizeof(szFile));

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hInstance = nullptr;
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrTitle = "Save As";
		ofn.lpstrInitialDir = szCurrentDir;
		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = "ini";

		if (GetSaveFileName(&ofn)) {
			// Check if the file exists
			FILE *fp = fopen(szFile, "rt");
			if (fp) {
				fclose(fp);
				if (MessageBox(nullptr, "File Exists\nOverwrite it?", "Confirmation", MB_YESNO) == IDNO) {
					_chdir(szCurrentDir);
					return false;
				}
			}
			*filename = std::string(szFile);
			_chdir(szCurrentDir);
			return true;
		}
		// Restore the current working directory
		_chdir(szCurrentDir);
		return false;
	}
}