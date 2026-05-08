# Super Pixel Smash project

Super pixel smash is a video game developed in C/C++. The code available in this directory corresponds to the basecode of the end-of-year project at ESIEA in 2025. 

## License

All code produced for this project is licensed under the [MIT license](/LICENSE.md). It uses the [SDL3](https://github.com/libsdl-org/SDL), [SDL3_image](https://github.com/libsdl-org/SDL_image), [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer), [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf), [Box2D](https://github.com/erincatto/box2d), [ImGui](https://github.com/ocornut/imgui) and [entt](https://github.com/skypjack/entt) libraries.

Game asset licenses are available in the assets folder.


## Building

### Building for Visual Studio (Windows)

This project was developed for Windows under Visual Studio.
To compile this project, make sure you have correctly installed the following tools:
- [Visual Studio](https://visualstudio.microsoft.com/) with "Desktop Development in C++" option,
- [CMake](https://cmake.org/download/).

Rename the ``build_vs.txt`` file to ``build_vs.bat`` and double-click on this file to run the script.
The Visual Studio solution is created in the ``_build_vs`` folder.

If you wish to compile the project manually, you can run the following commands:
```
mkdir _build_vs
cd _build_vs
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Building for linux

> TODO: Find a solution for the SDL3 library.

Use the following commands to install project dependencies:
```
sudo apt install cmake
```
To create the project with CMake, run the following commands:
```
mkdir _build_linux _build_linux/debug
cd _build_linux/debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug
```
You can then compile the project from the `_build_linux/debug` folder with either of the following two commands:
```
cmake --build .
cmake --build . --clean-first
```
Again from the `_build_linux/debug` folder, you can run the binary with the following line
```
../../_bin/Debug/application
```
To run valgrind, use
```
valgrind --leak-check=full --show-leak-kinds=all ../../_bin/Debug/application
```

## License

All code produced for this project is licensed under the [MIT license](/LICENSE.md).
