# Compatibility audit — 1.1.1

Target: Skyrim Steam 1.7.104 / SKSE 2.3.1 / AE Address Library format v5.

Both DLLs are compiled against CommonLibSSE-NG 10.0.1. Metadata declares Address Library v5 and updated structures, and the load function rejects any other game runtime before registering Papyrus functions.

The standalone 1.1.1 and Shout Progression addon 0.6.1 retain their original force curve, configuration, native function signature, and replacement scripts. They do not hook `NativeFunctionBase::Call`. The scripts perform one native `GetScaledPushForce` call followed by one vanilla `PushActorAway` call.

The local 1.7.104 executable and Address Library resolve `Actor::HasShout` (AE ID 38783) to executable `.text` RVA `0x6D7750`. Access to PlayerCharacter and Actor state uses the current CommonLib runtime accessors rather than historical hardcoded offsets.

Four PEX files were rebuilt with the local game's Papyrus compiler. Both variants compiled without warnings or errors, and their generated assembly preserves the correct native class and one push call.

Both DLLs completed Release builds with MSVC 19.44 and the matching CommonLib source build. PE exports, plugin versions, Address Library / structure flags, minimum SKSE version, and runtime metadata were checked. Imports contain only Windows and Microsoft C++ runtime libraries. Direct negative load tests confirm that null interfaces, Skyrim 1.6.1170, and editor interfaces return false before SKSE initialization.

Nexus feedback reviewed on 2026-10-02:

- https://www.nexusmods.com/skyrimspecialedition/mods/184210?tab=posts — compatibility-update request; ordinary stagger with many dragon souls; corpses not being pushed on lethal hits.
- https://www.nexusmods.com/skyrimspecialedition/mods/184210?tab=bugs — no reports listed.

The runtime rebuild addresses the version request. Script-conflict guidance was added for the stagger report. No log or reproducing save was attached to that report, so an actual Falskaar knockback fix is not claimed. Engine knockback immunity and corpse physics are unchanged.

Binary compilation and static checks do not establish in-game behavior. The release still needs a full in-game shout check on a living, non-immune target.
