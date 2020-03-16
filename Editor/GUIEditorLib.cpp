#include "GUIEditorLib.h"

#include <Windows.h>
#include <commdlg.h>
#include <direct.h>

namespace RTE {

	HINSTANCE g_hInstance = 0;
	HWND g_hWnd = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorLib::QuitMessageBox(std::string strMessage, std::string strTitle) {
		int nRetCode = MessageBoxA(g_hWnd, strMessage.c_str(), strTitle.c_str(), MB_YESNOCANCEL);

		if (nRetCode == IDNO) { return -1; }
		if (nRetCode == IDYES) { return 1; }
		return 0; // Cancel
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorLib::DisplayLoadGUIFile(std::string *strFilename) {
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
		ofn.hInstance = g_hInstance;
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrTitle = "Open";
		ofn.lpstrInitialDir = szCurrentDir;
		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = "ini";

		if (GetOpenFileNameA(&ofn)) {
			*strFilename = std::string(szFile);
			_chdir(szCurrentDir);
			return true;
		}
		_chdir(szCurrentDir);
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorLib::DisplaySaveGUIFile(std::string *strFilename) {
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
		ofn.hInstance = g_hInstance;
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "GUI Files (*.ini)\0*.ini\0All Files\0*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrTitle = "Save As";
		ofn.lpstrInitialDir = szCurrentDir;
		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = "ini";

		if (GetSaveFileNameA(&ofn)) {
			// Check if the file exists
			FILE *fp = fopen(szFile, "rt");
			if (fp) {
				fclose(fp);
				if (MessageBoxA(g_hWnd, "File Exists\nOverwrite it?", "Confirmation", MB_YESNO) == IDNO) {
					_chdir(szCurrentDir);
					return false;
				}
			}
			*strFilename = std::string(szFile);
			_chdir(szCurrentDir);
			return true;
		}
		// Restore the current working directory
		_chdir(szCurrentDir);
		return false;
	}
}