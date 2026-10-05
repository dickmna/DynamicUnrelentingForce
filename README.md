# Dynamic Unrelenting Force 1.1.1

**Skyrim Steam 1.7.104 / SKSE64 2.3.1 only. Skyrim 1.6.1170 users should keep 1.1.0; this compatibility update is not needed on 1170.**

Player downloads contain runtime files and license notices. Corresponding source, dependencies, and build instructions are available in this GitHub repository.

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

This release targets Skyrim Steam 1.7.104, SKSE 2.3.1, and the matching Anniversary Edition Address Library. Other runtime versions are rejected before registering the plugin. DLLs are rebuilt with CommonLibSSE-NG 10.0.1; gameplay force scaling and the Papyrus API are unchanged.

Dynamic Unrelenting Force is compatible with Shout Progression because it targets a different part of the shout pipeline:

- Shout Progression scales shout magnitude, range, cooldown, and related magic effect values.
- Dynamic Unrelenting Force scales the Papyrus `PushActorAway` force used by Unrelenting Force.

Use either the standalone version or the Shout Progression addon version. Do not install both at the same time because both replace `Scripts/VoicePushEffectScript.pex`.

Only the third word of Unrelenting Force uses the vanilla push script. If enemies only stagger, verify that this mod wins conflicts for `Scripts/VoicePushEffectScript.pex`, and check `DynamicUnrelentingForce.log` (or `ShoutProgressionPushSubmod.log` for the addon) for both native-function registration and `VoicePushEffectScript scaled PushForce` entries. A loaded DLL alone does not prove that the replacement script is running.

Engine knockback immunity and enemies dying before the push is applied can still prevent visible knockback. This release preserves the single vanilla `PushActorAway` call and does not add corpse physics impulses.

## Build

The release was compiled with Visual Studio 2022 MSVC 19.44, C++23, Release, and the dynamic MSVC CRT. Use the included CommonLibSSE-NG 10.0.1 and dependency sources under `dependencies/`.

See [building](docs/BUILDING.md) for dependency, plugin, and packaging commands. [SOURCE_PROVENANCE.json](SOURCE_PROVENANCE.json) records the original published source files and hashes.

## License

Original plugin source: MIT; see `LICENSE`. Linked CommonLibSSE-NG uses GPL-3.0-or-later with its published exceptions; see [third-party notices](THIRD_PARTY.md).
