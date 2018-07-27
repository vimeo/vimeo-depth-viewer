<h1 align="center">Vimeo Depth Viewer</h1>


<p align="center">
<img src="https://github.com/vimeo/depth-viewer/blob/master/docs/cover.png" alt="A GIF of a volumetric WebVR demo" height="400" />
</p>

<h4 align="center">An OpenGL application for viewing depth and color video streams from Intel RealSense cameras (D415/D435). The intention behind this application is to allow easy live streaming of color-depth information using <a href="https://vimeo.com/live">Vimeo Live</a>.</h4>

<p align="center">
  <img src="https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square" alt="PRs Welcome">
</p>
 
1. [Quick-start](#quick-start)
1. [Features](#features)
1. [Build from source](#build-from-source)
1. [Dependencies](#dependencies)
1. [License](#license)

# Quick-start
To quickly get started, download a pre-built release of the depth-viewer [from here](https://github.com/vimeo/depth-viewer/releases) or [build it from source](#build-from-source). Once you have a binary launch it by double clicking it or calling `./VimeoDepthViewer` from the folder and you should be good to go.

# Features
- [x] An easy to use interface based on [nanogui](https://github.com/wjakob/nanogui).
- [x] Depth clipping for easy background removal.
- [x] Seprate stream view and monitor window for capturing and live streaming a full window using [OBS](http://obsproject.com)
- [x] Depth filtering (decimation, spatial and temporal filters)
- [x] Fully `git submodules` and `CMake` for easy set up

# Build from source
All 3rd party libraries used are included as `git submodules` to ease the setup and development process, it should be easy to clone using the `--recursive` flag and build everything from source.
1. Make sure you have `CMake` installed, for macOS, install [Homebrew](https://brew.sh) and run
```sh
brew install cmake
```
2. Clone the repository including all the submodules:
```
git clone https://github.com/vimeo/depth-viewer.git --recursive
```
3. Go inside the folder and create a build folder by running `cd vimeo-depth-viewer && mkdir build`
4. Run `CMake` by calling `cmake ../` from the build folder
5. Run `make` to build an executable
6. Run the executable by running `./VimeoDepthViewer`

> Tested on macOS v10.13.4 & CMake v3.11.4

# Dependencies
1. [GLFW](https://github.com/glfw/glfw)
1. [nanogui](https://github.com/wjakob/nanogui)
1. [Eigen](https://github.com/libigl/eigen)
1. [librealsense2](https://github.com/IntelRealSense/librealsense)

# License
License file could be [found here](https://github.com/vimeo/vimeo-depth-viewer)

# Questions, help, and support
For questions and support, [ask on StackOverflow](https://stackoverflow.com/questions/ask/?tags=vimeo). If you found a bug, please file a [GitHub issue](https://github.com/vimeo/vimeo-depth-viewer/issues).

Make pull requests, file bug reports, and make feature requests via a [GitHub issue](https://github.com/vimeo/vimeo-depth-viewer/issues).

# Let's collaborate
Working on a cool video project? [Let's talk!](mailto:labs@vimeo.com)
