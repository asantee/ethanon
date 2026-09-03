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

Dependencies (Windows)
----------------------
- Some git tool for cloning codes
- [Visual Studio 2022][6] with the "Desktop development with C++" workload (MSVC v143 toolset,
  Windows 10/11 SDK). The VS2019 folder name of the project files is historical; the projects
  target `v143`.
- [VSCODE][6] (opcional, mas recomendado)
- [Ethanon Engine Libraries][1]
  - The sourcetree for Ethanon must be at same directory as the sourcetree for projectx (MagicRampage), or an environment variable ETHANON should be set to ethanon's sourcetree root.
  - Ethanon must be compiled for target configuration in use (Release, Debug or RelWithDebInfo), using VS2022

- [Angelscript library][1] (the one inside of Ethanon Engine; built by the solution)
- VCPkg for installing libraries. **The Win32 project configurations do not list these include
  paths themselves: they rely on vcpkg's user-wide MSBuild integration (`vcpkg integrate install`)
  to add `<vcpkg>/installed/x86-windows[-static]/include` automatically.**
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

#### Vcpkg command 
 vcpkg install boost-smart-ptr boost-any boost-asio boost-beast boost-chrono boost-convert boost-date-time boost-regex glew sdl2 libzip libwebp msgpack openssl --triplet x86-windows

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

### Windows (what the Magic Rampage Steam build needs)

Nothing has to be unpacked in `toolkit/Source/src/gs2d/vendors` for Windows: the old
`vendors.zip` / `Cg.framework.zip` no longer exist, `SDL2.framework.zip` is for macOS and
`fmod/extract-fmod-*.zip` only holds the Android/iOS FMOD binaries. Boost, SDL2, GLEW, libzip,
libwebp and OpenSSL come from vcpkg (see above); FMOD comes from its default install path
`C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core`.
FMOD headers: on Windows the gs2d project takes them from that SDK install (`api\core\inc`), and the
game copies `fmod.dll` from the same install, so header and library always match as long as the
engine is rebuilt after an FMOD upgrade. The copy under `src/gs2d/src/Audio/fmod/inc` (2.03.x) is
only for Android/iOS, whose projects add it to their include path; it must not be included by
relative path from the sources.

1. Install vcpkg, run `vcpkg integrate install` and install the packages listed above for the
   triplet you are going to link against (`x86-windows` for a DLL-runtime game build,
   `x86-windows-static` for a static-runtime game build).
2. Build the **Engine** target of `toolkit\Source\projects\vs2019\Ethanon\Ethanon.sln` for
   `RelWithDebInfo|x86` (solution platform `x86` = project platform `Win32`). It pulls in
   angelscript, Box2D, hashlib and gs2d through the solution dependencies:

   ```
   MSBuild.exe toolkit\Source\projects\vs2019\Ethanon\Ethanon.sln -t:Engine -p:Configuration=RelWithDebInfo -p:Platform=x86 -m
   ```

   Outputs: `projects\vs2019\Ethanon\RelWithDebInfo\Win32\{Engine,gs2d,Box2D,hashlib}.lib` and
   `src\angelscript\lib\angelscript.lib`.
3. The `Machine` project (the stand-alone engine runner) is not needed for the game and its
   post-build step references a `sdl2_x64-windows` vcpkg package that is normally not installed;
   leave it out of the build.

Notes:
- Only the `Win32` configurations are maintained. The `x64` configurations still point at a
  non-existent `src/gs2d/vendors/boost` folder.
- The Release/RelWithDebInfo projects compile with `/GL` (whole program optimisation), so
  consumers must link with `/LTCG` and the libraries must be rebuilt after every Visual Studio
  toolset update.
- The projects use the DLL runtime (`/MD`) by default. Magic Rampage's master build project
  injects a `.props` file (MSBuild `-p:ForceImportBeforeCppTargets=...`) to switch them to `/MT`
  and to define `_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR`; nothing in this repo needs to change
  for that.
- `toolkit\Source\Ethanon Engine.sln` at the toolkit root is the ancient msvc9 solution
  (editor, audiere, etc.) and is not used.

### macOS / iOS
Unpack `toolkit/Source/src/gs2d/vendors/SDL2.framework.zip` and the FMOD archive in
`toolkit/Source/src/gs2d/vendors/fmod/`, then use the Xcode projects under
`toolkit/Source/projects/xcode`.
