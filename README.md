[![Build status](https://ci.appveyor.com/api/projects/status/ks7mwo62678xjq3m?svg=true)](https://ci.appveyor.com/project/GentenStudios/phoenix)
# GENTEN STUDIOS: PROJECT PHOENIX
## Introduction
Project Phoenix is a open world sandbox style voxel game with a twist. The game itself does not provide any content but gets that content entirely from modules written in Lua. An easy to use Lua API provides the capability to define all of the games content in addition to some functional features. This allows content to be quickly created by someone with little to no programming experience while still retaining the power of C++. 

## Community
[Here's a link to our public discord server](https://discord.gg/XRttqAm), where we collaborate and discuss the development of the engine.

## Dependencies
- CMake (Version >= 3.0)
- A C++17 compatible compiler. The following have been tested
  - Visual Studio 2017 & 2019 (MSVC >= 19.14)
  - Clang (>= 5.0.0)
  - GCC (>= 4.8.4)
- OpenGL (Version >= 3.3)

## Build Instructions
Once cloned, navigate to the projects root directory and execute the following commands in a terminal.

  1. `cmake -S. -BBuild`
  2. `cmake --build Build`

Now follow the platform specific instructions detailed below.

### Visual Studio
  - Open the generated solution file in the `Build/` folder in Visual Studio
  - Set the Startup Project to `Sandbox`.
  - At this point you should be able to run, since the project should have already been
    built in step 2. above. You can always build the traditional way with Visual Studio.
  - And voila, all done. Now you should be able to run the project!

### Linux, Mac OS X, MSYS
 
  - Navigate to the `Build/Sandbox` folder and run `./Sandbox` to run the executable.
