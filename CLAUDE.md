# Ethanon engine — orientation for agents

Branch `v0-9-11-mr-supersimple` is the engine Magic Rampage ships on. `README.md` has the build steps per platform; the
master build project (`Magic-Rampage-Builds/CLAUDE.md`, sibling folder of this repo) has the machine facts, release
procedures and measured findings (§ numbers below refer to it). The game's AngelScript side is documented in
`projectx/tools/code-toolkit/skills/mr-angelscript` (its `references/ethanon-api.md` lists the engine API as scripts use it).

## Source map (`toolkit/Source/src`)

| Folder | What it is |
|---|---|
| `gs2d/src` | Platform layer: `Audio.h` (+ `Audio/`), video, input, `Platform/` (file I/O hubs, `FileLogger`, `SharedData`, native command forwarding; per-platform code in `Platform/{windows,macosx,android,ios,apple}`) |
| `engine` | Ethanon core (`ETH*`): `ETHEngine.cpp` builds the script module and drives registration; `Entity`, `Scene`, `Physics`, `Resource`, `Drawing`, `Script` |
| `engine/Script` | Everything bound to AngelScript (below) |
| `angelscript`, `addons` | AngelScript 2.34.0 and its add-ons (`addons/scriptbuilder.cpp` handles `#include` and section names) |
| `box2d`, `cjson`, `vendors` | Third-party code; `vendors/openssl-*` and `gs2d/vendors/BoostSDK` are git-ignored and unpacked per machine (README) |

## How the script interface is bound

`ETHEngine.cpp` (around the `RegisterGlobalFunctions` call) registers, in order: enum types, global properties
(`ETHGlobal::*` in `Script/ETHScriptObjRegister.cpp`), object types (declared in `ETHScriptObjRegister.cpp`, their methods in
`ETHScriptObjRegister.generic.cpp`), then the global functions (`ETHScriptWrapper::RegisterGlobalFunctions` in
`ETHScriptWrapper.generic.cpp`; math globals in `ETHScriptWrapper.Math.generic.cpp`; msgpack and websocket in their own
`*Register.cpp`). Every binding uses `asCALL_GENERIC` through the `asDECLARE_FUNCTION_WRAPPER` / `asDECLARE_METHOD_WRAPPER(PR)`
macros, and each function is registered in exactly one place: there is no native-calling-convention copy to keep in sync.
The split by file is not strict, so grep both `*.generic.cpp` files for a name: `RegisterGlobalFunctions` also registers some
`ETHEntity` methods (`SetPosition*`, `AddToPosition*`, `GetCurrentBucket`, `PlayParticleSystem`) and `ETHInput` methods
(`GetCursorPos`, `SetCursorPos`, …), and `ETHScriptObjRegister.generic.cpp` also registers globals (`GetStringFromFile`,
`SaveStringToFile`, the `matrix4x4` functions, `getAngle`). A single-argument constructor of a registered value type acts as an
implicit conversion: `vector3` has `vector3(float)`, so a `uint` passed where a `vector3` is expected compiles
(`entity.SetColor(0xFFFFFFFF)`), which matters when choosing overload parameter types. This AngelScript accepts `auto` and has
no `foreach` (checked with `machine -norun`, 2026-10-02).

**Adding a global function:**
1. Declare it `static` in `ETHScriptWrapper.h`, implement it in the matching `ETHScriptWrapper.<Area>.cpp` (`Audio`, `Drawing`,
   `Math`, `Scene`, `SharedData`, `System`).
2. In `ETHScriptWrapper.generic.cpp`: one `asDECLARE_FUNCTION_WRAPPER(__Name, ETHScriptWrapper::Name)` line, one
   `RegisterGlobalFunction("decl", asFUNCTION(__Name), asCALL_GENERIC)` line. Parameter names are allowed in the declaration;
   use them where the unit matters (`const float milliseconds`). Script time is in milliseconds throughout (`GetTime()`).
3. Add the name to the VS Code keyword list (`toolkit/Ethanon Toolkit/tools/vscode-extension/*/ethanon-angelscript.json`) and to
   `ethanon-api.md` in projectx.

**Overloads** keep the script name and use a differently named C++ wrapper: `DrawSprite` → `__DrawSprite` / `__DrawSpriteEx`,
`PlaySample` → `__PlaySample` / `__FadeInSample`.

**Never change an existing registered declaration**, not even by adding a default parameter: compiled byte code
(`game_32.bin` / `game_64.bin`) refers to functions by declaration, and AngelScript rejects a bin whose references do not
match the engine. Add an overload instead. A script that calls a new function needs an engine built with it on every
platform (Windows engine libs + Steam exe, Android, iOS), and the shipped bins must be compiled by such an engine (§13).

**Script ↔ host:** `ForwardCommand(string)` reaches the host app's command listeners (`projectx/mr-steam/Steam`,
`projectx/xcode-project/CommandListeners`, the Android activity); `SetSharedData` / `GetSharedData` share key/value strings
with them through `gs2d::Application::SharedData`, which is not synchronised: touch it on the main thread only.

## Audio

`gs2d/src/Audio.h` defines `Audio` and `AudioSample`. The only implementation that ships, on every platform, is FMOD Core:
`Audio/fmod/FMAudioContext.*` and `FMAudioSample.*` (Windows compiles against the installed 2.02.23 SDK, Android/iOS/macOS
against the 2.03.13 headers in `Audio/fmod/inc`; §3). `Audio/AudioDummy.*` is compiled by Magic Rampage's Xcode project;
`Audio/Android/AndroidAudio.*` is legacy and no longer compiles, but it still implements the interface. **A new
`AudioSample` method is a pure virtual in `Audio.h` plus an implementation in all three classes.** Music, soundtrack and
ambient types are streams (`FMOD_CREATESTREAM`), sound effects are loaded into memory; both play on an FMOD channel, so
channel-level features (volume, fade points, delays) need one code path. The script wrappers find a sample by file name
through `m_provider->GetAudioResourceManager()->GetPointer(...)`; "File not found" means it was never loaded.
Fades (`FadeOutSample`, `PlaySample(name, fadeInMs)`), the `setFadePointRamp` trap and the known quirks of this layer: §15.

## Adding a new source file

Edits to existing files need no project changes. A **new** file must be listed in every build Magic Rampage uses, because
none of them globs:

| Build | Project file(s) |
|---|---|
| Steam (Windows) engine libs | `toolkit/Source/projects/vs2019/Ethanon/Engine/Engine.vcxproj` (engine), `toolkit/Source/src/gs2d/projects/vs2019/gs2d/gs2d.vcxproj` (gs2d) |
| Android | `projectx/android-studio-project/mr-base/CMakeLists.txt` (compiles the engine sources directly) |
| iOS / macOS app | `projectx/xcode-project/Magic-Rampage.xcodeproj` |
| macOS runner `machine.app` | `toolkit/Source/projects/xcode/OSXEngine/engine.xcodeproj`, `toolkit/Source/src/gs2d/projects/xcode/gs2d/gs2d.xcodeproj` |

Not used by Magic Rampage: the ndk-build `Android.mk` files under `src/gs2d/projects/Android`, `projects/xcode/iOS/Application`
and the editor projects.

## Checking a change quickly on the Mac

- **Build and install `machine.app`**, which is what André runs the game with (Sublime Text build system): command and install
  steps in §16. The log has about 2 000 warnings from old code; grep it for the files you changed.
- **Interface check:** `/Applications/machine.app/Contents/MacOS/machine "dir=<folder>/" -nowait -norun` compiles the
  `main.angelscript` in that folder and exits 0 ("Compilation successful") or 1. For a new binding, use a scratch folder whose
  `main.angelscript` calls it in every intended form, plus one call that must fail to compile, then run it on `projectx/game`.
  Each run overwrites `~/Library/Application Support/machine/game_64.bin`, André's cache: copy it aside first, put it back after.
- **Runtime check without a window:** link a small program against the frameworks of the Xcode build in DerivedData (path in
  §16), e.g. `gs2d.framework` for `CreateAudio` → `LoadSampleFromFile` → `Play`/`FadeIn`/…. Put the binary in `bin/` next to
  a `Frameworks` symlink to `<app>/Contents/Frameworks` so `@executable_path/../Frameworks` resolves; compile with
  `-DMACOSX -DBOOST_NO_CXX98_FUNCTION_BASE` and `-I` for `gs2d/src`, `gs2d/vendors/BoostSDK`, `gs2d/vendors/tsl`,
  `gs2d/src/Audio/fmod/inc`. For audio, call `setOutput(FMOD_OUTPUTTYPE_NOSOUND)` on the FMOD system after init so nothing
  plays, and meter the master channel group. Never load `gs2d/vendors/fmod/lib/libfmod.dylib` itself: it is quarantined and
  macOS shows André a Gatekeeper dialog (§8).
- **Single-file syntax check:** `clang++ -std=c++17 -fsyntax-only` with the include paths of
  `projectx/android-studio-project/mr-base/CMakeLists.txt`. In zsh, pass `-I` flags through an array: an unquoted
  `$VAR` is not split into words.
- Windows (engine libs, Steam exe) and Android builds run on André's Windows PC with the master project's scripts (§5, §12).
