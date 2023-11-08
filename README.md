# Ethanon engine

- [About the engine] [1]
- [Sample code] [2]

  [1]: http://doc.ethanonengine.com/manual/6
  [2]: https://github.com/asantee/ethanon-samples

### License

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Dependencies
------------
- Some git tool for cloning codes
- [MSVC 10 BuildTools][5] ou [VS2022][6]
- [VSCODE][6] (opcional, mas recomendado)
- [Ethanon Engine Libraries][1]
  - The sourcetree for Ethanon must be at same directory as the sourcetree for projectx (MagicRampage), or an environment variable ETHANON should be set to ethanon's sourcetree root.
  - Ethanon must be compiled for target configuration in use (Release, Debug or RelWithDebInfo), using VS2022

- [Angelscript library][1] (the one inside of Ethanon Engine)
- VCPkg for installing libraries:
  - boost-smart-ptr
  - boost-any
  - boost-asio
  - boost-beast
  - boost-chrono
  - boost-convert
  - boost-date-time
  - boost-regex
  - glew
  - sdl2
  - libzip
  - libwebp
  - msgpack
  - openssl
- [Fmod][3]
  
It is recommended to use [VCPkg][4] to download most of these libraries. (except Ethanon with Angelscript, Steam SDK and Fmod)

Instructions to install VCPkg and required libraries
----------------------------------------------------
#### [Instructions according official docs](https://learn.microsoft.com/en/vcpkg/get_started/get-started-msbuild?pivots=shell-cmd)

### 1. Clone the repository

```
git clone https://github.com/microsoft/vcpkg.git
```

### 2. Run the bootstrap script

Navigate to the vcpkg directory and execute the bootstrap script:
```
cd vcpkg && bootstrap-vcpkg.bat
```
The bootstrap script performs prerequisite checks and downloads the vcpkg executable.

### 3. Integrate with Visual Studio MSBuild

The next step is to set the user-wide instance of vcpkg so that MSBuild will be able to find it:

```
.\vcpkg.exe integrate install
```

This outputs:

```
All MSBuild C++ projects can now #include any installed libraries. Linking will be handled automatically. Installing new libraries will make them instantly available.
```

### 4. Set environment variables

Open the `Developer command prompt in Visual Studio` and run the commands
```
set VCPKG_ROOT="C:\path\to\vcpkg"
set PATH=%VCPKG_ROOT%;%PATH%
```
Also edit on System > Environment Variables

Add the variables above in order to make them permanent.

### 5. Install the libraries
On `Developer command prompt in Visual Studio` run the command
```
vcpkg install boost-smart-ptr boost-any boost-asio boost-beast boost-chrono boost-convert boost-date-time boost-regex glew sdl2 libzip libwebp msgpack openssl --triplet=x86-windows
```

Instructions to install FMod
----------------------------
1. Create a fmod.com account
2. Go to [Download](https://fmod.com/download) section
3. Download latest FMod Engine for windows (not UWP)
4. Install it on default directory (c:\Program Files (x86)\FMOD SoundSystem)

## Build Tools
### Windows

- MSVC 19

## Currently used IDEs

### Windows
- Visual Studio 2022

### Mac OS X

- Xcode

## How to build

1. Unpack the following files located at **toolkit/Source/src/gs2d/vendors**:
  - vendors.zip
  - Cg.framework.zip
  - SDL2.framework.zip

- Use Visual Studio 2022 (19.0)
- Unpack the following files located at **toolkit/Source/src/gs2d/vendors**:
  - vendors.zip
  
2. The project file for Visual Studio is located at
   `ethanon\toolkit\Source\projects\vs2019`
