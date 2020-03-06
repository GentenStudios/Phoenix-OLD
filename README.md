[![Build Status](https://dev.azure.com/GentenStudios/ProjectPhoenix/_apis/build/status/GentenStudios.Phoenix?branchName=develop)](https://dev.azure.com/GentenStudios/ProjectPhoenix/_build/latest?definitionId=1&branchName=develop)
# GENTEN STUDIOS: PROJECT PHOENIX
## Introduction
Project Phoenix is an open world sandbox style voxel game with a twist. The game itself does not provide any content but gets that content entirely from modules written in Lua. An easy to use Lua API provides the capability to define all of the games content in addition to some functional features. This allows content to be quickly created by someone with little to no programming experience while still retaining the power of C++. 

## Community
[Here's a link to our public discord server](https://discord.gg/XRttqAm), where we collaborate and discuss the development of the Phoenix.
[Here's a link to our community guidelines](./CODE_OF_CONDUCT.md)

## Dependencies
- CMake (Version >= 3.0)
- A C++17 compatible compiler. The following have been tested
  - Visual Studio 2017 & 2019 (MSVC >= 19.14)
  - Clang (>= 5.0.0)
  - GCC (>= 4.8.4)
- OpenGL (Version >= 3.3)

## Build Instructions
Clone with the `--recursive` flag to also clone all submodules
If you already cloned you can run `git submodule update --init` to do the same thing on an existing clone

Once cloned, navigate to the projects root directory and execute the following commands in a terminal.

  1. `mkdir Build`
  2. `cd Build`
  3. `cmake ..`
  4. `cmake --build . --target Phoenix`

Now follow the platform specific instructions detailed below.

### Visual Studio
  - Open the generated solution file in the `Build/` folder in Visual Studio
  - Set the Startup Project to `Phoenix`.
  - At this point you should be able to run, since the project should have already been
    built in step 2. above. You can always build the traditional way with Visual Studio.
  - And voila, all done. Now you should be able to run the project!

### Linux, Mac OS X, MSYS
 
  - Navigate to the `Build/Phoenix` folder and run `./Phoenix` to run the executable.

## Contributing
We encourage everyone to contribute, this is an open source project that will ultimately be powered by its community. If you are interested in contributing check out [our contributing guidelines](./CONTRIBUTING.md) for information on how we work and our coding standards. Before you get started, reach out on #programming on discord to collaborate so you aren't duplicating any work. Finally check out [Our issues on GitHub](https://github.com/GentenStudios/Phoenix/issues) for a list of what we currently have planned/ need help with.
