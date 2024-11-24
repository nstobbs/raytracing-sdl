# Raytracing-SDL

Raytracing-SDL is a implementation of Peter Shirley's Ray Tracing in One Weekend with rendering directly to a window GUI via SDL2.

Peter Shirley's Ray Tracing in One Weekend - Book Series -> https://raytracing.github.io/

![](assets/20241124_211423_raytracingSDL_demo.gif)

*GIF is 9x faster then the actual render time.*

## Build on Windows

### Requirements

```
Git
CMake
Conan
````

Git Clone this repository.

```bash
$git clone git@github.com:nstobbs/raytracing-sdl.git
```

Step into the repository directory.

```bash
$cd ./raytracing-sdl
```

Run Conan install.

```bash
$conan install . --build=missing --settings=build_type=Debug
```

Step into the build directory.

```bash
$cd ./build
```

Configure CMake.

```bash
$cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
```

Run CMake Build.

```bash
$cmake --build . --config Debug
```

Run Raytracing-SDL!

```bash
$.\Debug\raytracingSDL.exe
```
