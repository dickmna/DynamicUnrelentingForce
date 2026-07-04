# Installer Notes

`installer/fomod` contains the FOMOD metadata used by the release package.

The final installer package is assembled from compiled outputs:

- Standalone option: `DynamicUnrelentingForce.dll`, `DynamicUnrelentingForce.ini`, `DynamicUnrelentingForce.pex`, and the replacement `VoicePushEffectScript.pex`.
- Shout Progression addon option: `ShoutProgressionPushSubmod.dll`, `ShoutProgressionPushSubmod.ini`, `ShoutProgressionPushSubmod.pex`, and the replacement `VoicePushEffectScript.pex`.

The source repository does not include generated DLL, PDB, ZIP, or PEX artifacts.
