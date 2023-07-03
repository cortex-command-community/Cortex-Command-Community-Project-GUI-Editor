# Cortex Command GUI Editor
A tool for creating and editing Cortex Command GUI layouts.

***

# Windows Build Instructions

1. Clone this repository and the **Cortex Command Community Project [Source](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source)** and **[Data](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Data)** repositories in neighboring folders.  
**Do Not** change the folder names unless you want to make trouble for yourself.

2. Follow the [CCCP Source Repository Windows Build Instructions](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source#windows-build-instructions) on copying libraries into the **Data Repository**.

3. Open `RTEGUI.sln`, choose the desired configuration and run the project.
Build supports Visual Studio 2022 and 2019 (>=16.10) versions. Earlier versions are not supported due to lack of C++20 standard library features and conformance.

4. The compiled executable will be located in the **Data Repository**.

***

## Notes
- Loading an `.ini` file containing data that isn't valid for the editor will crash.
- Use `Add File` button to merge contents of a file into the current document.
- Workspace zoom is disabled for resolutions under 1080p. Maximize the window when enabling.

## Keyboard Controls
- `Ctrl + O` - Load file.
- `Ctrl + Shift + O` - Add file.
- `Ctrl + S` - Save file.
- `Ctrl + Shift + S` - Save as new file.
- `Ctrl + C/V`- Copy/paste.
- `Esc` - Deselect current selection.
- `Delete` - Delete current selection.
- `Arrow Keys` - Nudge the current selection by `Grid/Nudge Size` pixels.
- `Shift + Arrow Keys` - Nudge the current selection by 1 pixel.
- `Ctrl + 1-4` - Change workspace skin to preview layout under different skin.

***

## Changelog
https://github.com/cortex-command-community/Cortex-Command-Community-GUI-Editor/wiki/Changelog

***

**Please report any bugs, there should be plenty.**