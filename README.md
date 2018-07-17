# Depth Viewer
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)  
An OpenGL application for viewing depth and color video streams from Intel RealSense cameras (D415/D435). The intention behind this application is to allow easy live streaming of color-depth information using [Vimeo Live](https://vimeo.com/live).
![Interface Cover Image](https://github.com/vimeo/depth-viewer/blob/master/docs/cover.png)  
1. [Quick-start](#quick-start)
1. [Features](#features)
1. [Development](#development)
1. Dependencies
1. License

### Quick-start

### Features

### Development
All 3rd party libraries used are included as `git submodules` to ease the setup and development process, it should be easy to clone using the `--recursive` flag and build everything from source.
1. Make sure you have `CMake` installed
2. Clone the repository including all the submodules:
```
git clone https://github.com/vimeo/depth-viewer.git --recursive
```
3. `cd` into the folder and create a `build` folder (e.g `cd depth-viewer && mkdir build`)
4. Run `CMake` by calling `cmake ../` from the build folder
5. Run `make` to build an executable

> Tested on macOS v10.13.4 & CMake v3.11.4

### Dependencies
1. [GLFW](https://github.com/glfw/glfw)
1. [nanogui](https://github.com/wjakob/nanogui)
1. [Eigen](https://github.com/libigl/eigen)
1. [librealsense2](https://github.com/IntelRealSense/librealsense)

### License
Some license

### Questions, help, and support
For questions and support, [ask on StackOverflow](https://stackoverflow.com/questions/ask/?tags=vimeo). If you found a bug, please file a [GitHub issue](https://github.com/vimeo/depth-viewer/issues).

Make pull requests, file bug reports, and make feature requests via a [GitHub issue](https://github.com/vimeo/depth-viewer/issues).

### Let's collaborate
Working on a cool video project? [Let's talk!](mailto:labs@vimeo.com)
