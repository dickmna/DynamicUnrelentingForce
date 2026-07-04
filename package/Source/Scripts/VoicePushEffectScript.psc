ScriptName VoicePushEffectScript Extends ActiveMagicEffect

Int Property PushForce Auto

Event OnEffectStart(Actor Target, Actor Caster)
	If Target
		Float force = PushForce as Float
		If Caster
			force = DynamicUnrelentingForce.GetScaledPushForce(force, Caster)
			Caster.PushActorAway(Target, force)
		EndIf
	EndIf
EndEvent
