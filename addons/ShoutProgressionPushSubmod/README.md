# Shout Progression Addon

This optional variant keeps the older `ShoutProgressionPushSubmod` plugin name and can import Shout Progression scaling settings before applying its own overrides.

It uses the same safe mechanism as the standalone version: the replacement `VoicePushEffectScript` asks the SKSE DLL for a scaled `PushForce`, then performs one vanilla `PushActorAway` call.

Use this addon only if you specifically want the Shout Progression-named files and configuration path. Most users should use the standalone plugin.
