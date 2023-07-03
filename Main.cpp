#include "EditorApp.h"
#include "EditorUtil.h"
#include "ContentFile.h"

#include "allegro.h"
#include "SDL.h"

#include "GUI.h"
#include "GUIInputWrapper.h"
#include "AllegroScreen.h"
#include "AllegroBitmap.h"

#include "MainMenuGUI.h"
#include "ScenarioGUI.h"
#include "TitleScreen.h"

#include "MenuMan.h"
#include "ConsoleMan.h"
#include "SettingsMan.h"
#include "PresetMan.h"
#include "UInputMan.h"
#include "PerformanceMan.h"
#include "MetaMan.h"
#include "WindowMan.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

using namespace RTEGUI;

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Initializes all the essential managers.
	/// </summary>
	void InitializeManagers() {
		g_SettingsMan.Initialize();

		g_WindowMan.SetIgnoreMultiDisplays(true);

		//g_LuaMan.Initialize();
		//g_NetworkServer.Initialize();
		//g_NetworkClient.Initialize();
		g_TimerMan.Initialize();
		g_WindowMan.Initialize();
		g_FrameMan.Initialize();
		//g_PostProcessMan.Initialize();
		//g_PerformanceMan.Initialize();

		if (g_AudioMan.Initialize()) { g_GUISound.Initialize(); }

		g_UInputMan.Initialize();
		g_ConsoleMan.Initialize();
		//g_SceneMan.Initialize();
		//g_MovableMan.Initialize();
		//g_MetaMan.Initialize();
		//g_MenuMan.Initialize();

		g_WindowMan.SetVSyncEnabled(true);

		// Overwrite Settings.ini after all the managers are created to fully populate the file. Up until this moment Settings.ini is populated only with minimal required properties to run.
		// If Settings.ini already exists and is fully populated, this will deal with overwriting it to apply any overrides performed by the managers at boot (e.g resolution validation).
		//if (g_SettingsMan.SettingsNeedOverwrite()) { g_SettingsMan.UpdateSettingsFile(); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Destroys all the managers and frees all loaded data before termination.
	/// </summary>
	void DestroyManagers() {
		//g_NetworkClient.Destroy();
		//g_NetworkServer.Destroy();
		//g_MetaMan.Destroy();
		//g_PerformanceMan.Destroy();
		//g_MovableMan.Destroy();
		//g_SceneMan.Destroy();
		//g_ActivityMan.Destroy();
		g_GUISound.Destroy();
		g_AudioMan.Destroy();
		g_PresetMan.Destroy();
		g_UInputMan.Destroy();
		//g_PostProcessMan.Destroy();
		g_FrameMan.Destroy();
		g_TimerMan.Destroy();
		//g_LuaMan.Destroy();
		ContentFile::FreeAllLoaded();
		g_ConsoleMan.Destroy();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Polls the SDL event queue and passes events to be handled by the relevant managers.
	/// </summary>
	void PollSDLEvents() {
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent)) {
			switch (sdlEvent.type) {
				case SDL_QUIT:
					System::SetQuit(true);
					return;
				case SDL_WINDOWEVENT:
					if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
						System::SetQuit(true);
						return;
					}
					g_WindowMan.QueueWindowEvent(sdlEvent);
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN:
				case SDL_TEXTINPUT:
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEWHEEL:
				case SDL_CONTROLLERAXISMOTION:
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
				case SDL_JOYAXISMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
					g_UInputMan.QueueInputEvent(sdlEvent);
					break;
				default:
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Entry point for the GUI editor app.
/// </summary>
int main(int argc, char **argv) {
	install_allegro(SYSTEM_NONE, &errno, std::atexit);
	loadpng_init();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);

#if SDL_MINOR_VERSION > 22
	SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");
#endif

	SDL_ShowCursor(SDL_ENABLE);
	SDL_SetHint("SDL_ALLOW_TOPMOST", "0");

	System::Initialize();
	SeedRNG();

	InitializeManagers();

	g_GUIEditor.Initialize();

	std::chrono::steady_clock::time_point frameTimeStart;
	while (true) {
		PollSDLEvents();

		g_UInputMan.Update();
		g_WindowMan.Update();

		if (!g_GUIEditor.UpdateEditor()) {
			break;
		}
		frameTimeStart = std::chrono::high_resolution_clock::now();

		g_GUIEditor.DrawEditor();
		g_WindowMan.UploadFrame();

		EditorApp::s_FrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - frameTimeStart).count();
	}

	g_GUIEditor.DestroyBackBuffers();

	DestroyManagers();

	allegro_exit();
	SDL_Quit();

	std::exit(EXIT_SUCCESS);
}