# Building 1.1.1

Use Windows x64, Visual Studio 2022 with the C++ workload, CMake, and the Windows SDK. The published build used MSVC 19.44, C++23, Release, and the dynamic MSVC CRT (`/MD`). The native and Papyrus source files in this repository match the published 1.1.1 standalone DLL and 0.6.1 addon.

From the repository root:

```powershell
.\scripts\BuildDependencies.ps1 -CheckOnly
.\scripts\BuildDependencies.ps1
.\scripts\build.ps1
.\scripts\build.ps1 -Addon
```

The dependency script builds the included fmt, spdlog, rapidcsv, DirectXMath, DirectXTK, and CommonLibSSE-NG 10.0.1 sources into `build/commonlib-installed`. It uses the included MinHook hde64 source and fixed OpenVR headers/import library, enables SE and AE, and disables VR, Xbyak, IPO, and tests. Source directories and the install prefix can be overridden explicitly. The plugin CMake files and build script discover this repository-local install prefix by default; pass `-CMakePrefixPath` to use an equivalent existing installation.

Compile each variant's `package/Source/Scripts/*.psc` using your installed Skyrim Papyrus compiler, the game's standard/SKSE script definitions, and `Papyrus/TESV_Papyrus_Flags.flg`. Use `-all -optimize`. Put the standalone PEX files in `build/vs2022-release/Data/Scripts` and the addon PEX files in `build/addon/Data/Scripts`. Compile them separately because both variants replace `VoicePushEffectScript`.

Create the player FOMOD after committing the corresponding source:

```powershell
.\scripts\package.ps1
```

The script copies only each variant's DLL, INI, and two PEX files, one root FOMOD configuration, and license notices. It creates fresh staging, rejects source/debug files and nested archives, and includes the full Git commit URL for corresponding source. Input Data paths, the output directory, and the source commit can be supplied explicitly. It never includes repository or dependency trees in the player ZIP.

`SOURCE_PROVENANCE.json` records the original archive SHA-256 and source file hashes. `dependencies/REBUILD.md` preserves the detailed published build configuration. Gameplay has not been newly tested by the source synchronization or package layout changes.
