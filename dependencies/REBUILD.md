# Corresponding source and rebuilding

These are the already-expanded dependency sources corresponding to the published
1.1.1 DLLs. The exact plugin source is at this repository root. Source hashes
from the published archive are recorded in `../SOURCE_PROVENANCE.json`. No proprietary Skyrim or
Papyrus compiler files are redistributed.

Included dependency sources:

- CommonLibSSE-NG 10.0.1, tag `v10.0.1`, commit
  `de1ca9826919d04649e21e93358d7313459e0a68` (exact Git archive).
- MinHook 1.3.4, tag `v1.3.4` (exact Git archive). CommonLib uses its hde64
  instruction decoder when patch safety is enabled.
- fmt 12.1.0, spdlog 1.16.0, rapidcsv 8.90, DirectXTK 2025-10-27, and
  DirectXMath 2025-04-03, from the corresponding vcpkg clean source trees.
- vcpkg port definitions, helper ports and the `x64-windows-static-md` triplet
  at baseline `ee12231b20c95013c6638d845d04c91559a1d1ff`.

The published dependency archive omitted the OpenVR submodule. This repository
adds the headers, Windows x64 import library, and license at its exact CommonLib
gitlink commit `60eb187801956ad277f1cae6680e3a410ee0873b`. See
`../docs/openvr-dependency.json`. VR remains disabled for the plugin build. Xbyak is disabled. The included upstream CMake files and vcpkg
ports contain the library build options and any patches used by those ports.
`SHA256SUMS.txt` identifies every source archive. The vcpkg baseline's MIT
license is provided separately as `vcpkg-LICENSE.txt`.

Build environment: Windows x64, Visual Studio 2022 Build Tools, MSVC
19.44.35227, Windows SDK 10.0.26100.0, C++23, Release, dynamic MSVC CRT (`/MD`).
Build the listed dependency versions using the included vcpkg ports and
`x64-windows-static-md` triplet. Supply their installed package prefixes as a
semicolon-separated CMake prefix path. CommonLib and each plugin must use the
same compiler and CRT configuration.

The following PowerShell example assumes those dependency packages are already
built. Replace the source, install and dependency paths with your local paths.

```powershell
$commonLibSource = "C:/source/CommonLibSSE-NG"
$minHookSource = "C:/source/MinHook"
$commonLibPrefix = "C:/build/CommonLib-installed"
$dependencyPrefixes = "C:/vcpkg/packages/fmt_x64-windows-static-md;C:/vcpkg/packages/spdlog_x64-windows-static-md;C:/vcpkg/packages/rapidcsv_x64-windows-static-md;C:/vcpkg/packages/directxtk_x64-windows-static-md;C:/vcpkg/packages/directxmath_x64-windows-static-md"
cmake -S $commonLibSource -B C:/build/CommonLib -G "Visual Studio 17 2022" -A x64 "-DCMAKE_PREFIX_PATH=$dependencyPrefixes" "-DCMAKE_INSTALL_PREFIX=$commonLibPrefix" -DENABLE_SKYRIM_SE=ON -DENABLE_SKYRIM_AE=ON -DENABLE_SKYRIM_VR=OFF -DSKSE_SUPPORT_XBYAK=OFF -DSKSE_SUPPORT_PATCH_SAFETY=ON -DCOMMONLIB_ENABLE_IPO=OFF -DBUILD_TESTS=OFF "-DFETCHCONTENT_SOURCE_DIR_HDE64=$minHookSource"
cmake --build C:/build/CommonLib --config Release
cmake --install C:/build/CommonLib --config Release
cmake "-DCOMMONLIB_SOURCE=$commonLibSource" "-DCOMMONLIB_PREFIX=$commonLibPrefix" -P ./finalize-commonlib.cmake
cmake -S .. -B C:/build/Plugin -G "Visual Studio 17 2022" -A x64 "-DCMAKE_PREFIX_PATH=$commonLibPrefix;$dependencyPrefixes"
cmake --build C:/build/Plugin --config Release
```

The included finalization script supplies the missing package version file
and copies the upstream CommonLib CMake helper after installation, as done for
these release builds. Build the DUF addon separately by changing the final
configure source to `../addons/ShoutProgressionPushSubmod`.

For DUF PEX files, use the installed Skyrim Papyrus compiler, the included
`../Papyrus/TESV_Papyrus_Flags.flg`, each variant's `package/Source/Scripts`,
and the game's standard/SKSE Papyrus definitions as import paths. Compile with
`-all -optimize`, selecting separate output directories for the standalone and
addon scripts. This avoids combining their two `VoicePushEffectScript` files.

For the repository-local build entry point, see `../docs/BUILDING.md` and
`../scripts/BuildDependencies.ps1`. Its defaults point at these source directories
and use the same static libraries, dynamic CRT, CommonLib runtime set, patch
safety setting, and disabled IPO as the published build.
