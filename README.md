# Dynamic Unrelenting Force

Dynamic Unrelenting Force is an SKSE/CommonLibSSE-NG plugin for Skyrim Special Edition and Anniversary Edition. It makes the knockback force of the third word of Unrelenting Force scale with the player's dragon soul progress.

The mod changes the `PushForce` used by vanilla `VoicePushEffectScript` before making the original single `Caster.PushActorAway(Target, force)` call. It does not scale magic effect magnitude directly; that is a separate part of shout behavior.

## Default Scaling

The default curve is anchored to vanilla and common 700 Percent-style tuning:

```text
0 progress          -> PushForce 15
iMaxDragonSouls cap -> PushForce 100
```

By default, progress is based on current unspent dragon souls plus known shout words. This is a practical approximation of spent dragon souls.

## Repository Layout

```text
src/                                      Standalone SKSE plugin source
package/SKSE/Plugins/                    Standalone INI
package/Source/Scripts/                  Standalone Papyrus source
addons/ShoutProgressionPushSubmod/        Optional Shout Progression addon source
installer/fomod/                         FOMOD metadata used by release packages
```

Release packages include compiled DLL and PEX files. The source repository intentionally excludes generated binaries.

## Compatibility

Dynamic Unrelenting Force is compatible with Shout Progression because it targets a different part of the shout pipeline:

- Shout Progression scales shout magnitude, range, cooldown, and related magic effect values.
- Dynamic Unrelenting Force scales the Papyrus `PushActorAway` force used by Unrelenting Force.

Use either the standalone version or the Shout Progression addon version. Do not install both at the same time because both replace `Scripts/VoicePushEffectScript.pex`.

## Build Notes

Requirements:

- Visual Studio 2022 with C++ toolchain
- CMake 3.24 or newer
- SKSE-compatible Skyrim SE/AE runtime
- CommonLibSSE-NG available through CMake, or checked out at `external/CommonLibSSE-NG-lite`
- Skyrim Papyrus compiler for rebuilding `.pex` files from the included `.psc` sources

Example CMake build:

```powershell
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
```

The CMake build produces the DLL and copies the INI and Papyrus source files into a `Data` layout under the build directory. Compile the Papyrus scripts separately when preparing a release package.

## License

MIT License. See `LICENSE`.
