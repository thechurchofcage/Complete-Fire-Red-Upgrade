.text
.thumb
.align 2

.include "..\\defines"

.global BattleScript_OverworldWeatherStarts
.global BattleScript_NewWeatherAbilityActivates
.global BattleScript_IntimidateActivatesEnd3
.global BattleScript_TraceActivates
.global BattleScript_TerrainFromAbility
.global BattleScript_ImposterActivates
.global BattleScript_AttackerAbilityStatRaiseEnd3
.global BattleScript_SwitchInAbilityMsg
.global BattleScript_AbilityCuredStatusEnd3
.global BattleScript_StartedSchoolingEnd3
.global BattleScript_StoppedSchoolingEnd3
.global BattleScript_ShieldsDownToCoreEnd3
.global BattleScript_ShieldsDownToMeteorEnd3
.global BattleScript_EmergencyExit
.global BattleScript_TransformedEnd3

.global BattleScript_RainDishActivates
.global BattleScript_DrySkinDamage
.global BattleScript_SolarPowerDamage
.global BattleScript_Healer
.global BattleScript_MoodySingleStat
.global BattleScript_MoodyRegular
.global BattleScript_BadDreams
.global BattleScript_Harvest
.global BattleScript_Pickup
.global BattleScript_SoundproofProtected
.global BattleScript_FlashFireBoost_PPLoss
.global BattleScript_FlashFireBoost
.global BattleScript_MoveHPDrain_PPLoss
.global BattleScript_MoveHPDrain
.global BattleScript_MonMadeMoveUseless_PPLoss
.global BattleScript_MonMadeMoveUseless
.global BattleScript_MoveStatDrain_PPLoss
.global BattleScript_MoveStatDrain
.global BattleScript_TargetAbilityStatRaise
.global BattleScript_RoughSkinActivates
.global BattleScript_CuteCharmActivates
.global BattleScript_WeakArmorActivates
.global BattleScript_CursedBodyActivates
.global BattleScript_MummyActivates
.global BattleScript_GooeyActivates
.global BattleScript_IllusionBroken
.global BattleScript_AngerPointActivates
.global BattleScript_SynchronizeActivates_StatusesAttacker
.global BattleScript_SynchronizeActivates_StatusesTarget

.global BattleScript_Protean
.global BattleScript_MimikyuTransform
.global MimikyuDisguisedTookDamageString
.global BattleScript_EnduredSturdy
.global StringEnduredHitWithSturdy
.global BattleScript_Receiver
.global BattleScript_Symbiosis
.global BattleScript_DefiantCompetitive

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_OverworldWeatherStarts:
	printfromtable gWeatherContinuesStringIds
	waitmessage DELAY_1SECOND
	playanimation2 BANK_ATTACKER ANIM_ARG_1 0x0
	end3
	
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_NewWeatherAbilityActivates:
	pause DELAY_HALFSECOND
	printstring 0x184
	waitstateatk
	playanimation2 BANK_ATTACKER ANIM_ARG_1 0x0
	call 0x81D92DC @;BattleScript_WeatherFormChanges
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_IntimidateActivatesEnd3:
	call BattleScript_PauseIntimidateActivates
	end3

BattleScript_PauseIntimidateActivates:
	pause DELAY_HALFSECOND
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
	setbyte TARGET_BANK 0x0
	setstatchanger STAT_ATK | DECREASE_1

BS_IntimidateActivatesLoop:
	trygetintimidatetarget BattleScript_IntimidateActivatesReturn
	jumpifbehindsubstitute BANK_TARGET IntimidateActivatesLoopIncrement
	statbuffchange STAT_TARGET | STAT_NOT_PROTECT_AFFECTED | STAT_BS_PTR IntimidateActivatesLoopIncrement
	jumpifbyte EQUALS MULTISTRING_CHOOSER 0x2 BattleScript_IntimidatePrevented
	setgraphicalstatchangevalues
	playanimation BANK_TARGET ANIM_STAT_BUFF ANIM_ARG_1
	printfromtable 0x83FE588
	waitmessage DELAY_1SECOND
	jumpifhelditemeffect BANK_TARGET ITEM_EFFECT_ADRENALINE_ORB BattleScript_AdrenalineOrb
	goto IntimidateActivatesLoopIncrement
	
BattleScript_AdrenalineOrb:
	setstatchanger STAT_SPD | INCREASE_1
	statbuffchange STAT_TARGET | STAT_NOT_PROTECT_AFFECTED | STAT_BS_PTR, IntimidateActivatesLoopIncrement
	jumpifbyte EQUALS MULTISTRING_CHOOSER 0x2 IntimidateActivatesLoopIncrement
	playanimation BANK_TARGET ANIM_ITEM_USE 0x0
	setgraphicalstatchangevalues
	playanimation BANK_TARGET ANIM_STAT_BUFF ANIM_ARG_1
	setword BATTLE_STRING_LOADER AdrenalineOrbString
	printstring 0x184
	waitmessage DELAY_1SECOND
	removeitem BANK_TARGET
	goto IntimidateActivatesLoopIncrement
	
BattleScript_IntimidatePrevented:
	pause DELAY_HALFSECOND
	printfromtable 0x83FE588
	waitmessage DELAY_1SECOND

IntimidateActivatesLoopIncrement:
	addbyte TARGET_BANK 0x1
	goto BS_IntimidateActivatesLoop

BattleScript_IntimidateActivatesReturn:
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_TraceActivates:
	pause DELAY_HALFSECOND
	printstring 0xD0 @;STRINGID_PKMNTRACED
	waitmessage DELAY_1SECOND
	callasm TryActivateNewSwitchInAbility
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_TerrainFromAbility:
	pause DELAY_HALFSECOND
	playanimation2 BANK_ATTACKER ANIM_ARG_1 0x0
	printstring 0x184
	waitmessage DELAY_1SECOND
	call SEED_HELPER
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_ImposterActivates:
	pause DELAY_HALFSECOND
	playanimation BANK_ATTACKER ANIM_TRANSFORM 0x0
	printfromtable 0x83FE5B4
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_AttackerAbilityStatRaiseEnd3:
	call BattleScript_AttackerAbilityStatRaise
	end3	
	
BattleScript_AttackerAbilityStatRaise:
	setgraphicalstatchangevalues
	playanimation BANK_ATTACKER ANIM_STAT_BUFF ANIM_ARG_1
	setword BATTLE_STRING_LOADER DownloadString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_SwitchInAbilityMsg:
	pause DELAY_HALFSECOND
	printstring 0x184
	waitmessage DELAY_1SECOND
	end3	

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_AbilityCuredStatusEnd3:
	printstring 0x40 @;STRINGID_PKMNSXCUREDITSYPROBLEM
	waitmessage DELAY_1SECOND
	refreshhpbar BANK_SCRIPTING
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_StartedSchoolingEnd3:
	call BattleScript_StartedSchoolingRet
	end3

BattleScript_StoppedSchoolingEnd3:
	call BattleScript_StartedSchoolingRet
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_ShieldsDownToCoreEnd3:
	call BattleScript_ShieldsDownToCoreRet
	end3
	
BattleScript_ShieldsDownToMeteorEnd3:
	call BattleScript_ShieldsDownToMeteorRet
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_TransformedEnd3:
	playanimation BANK_ATTACKER ANIM_TRANSFORM 0x0
	setword BATTLE_STRING_LOADER TransformedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	end3

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_EmergencyExit:
	jumpifbattletype BATTLE_TRAINER EmergencyExitSwitchBS

EmergencyExitFleeBS:
	getifcantrunfrombattle BANK_SCRIPTING
	jumpifbyte EQUALS BATTLE_COMMUNICATION 0x1 EmergencyExitReturn
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	copyarray USER_BANK BATTLE_SCRIPTING_BANK 0x1
	printstring 0xA0 @;STRINGID_PKMNFLEDFROMBATTLE
	waitmessage DELAY_1SECOND
	setbyte BATTLE_OUTCOME 0x5 @;Teleported
	end

EmergencyExitSwitchBS:
	jumpifcannotswitch BANK_SCRIPTING EmergencyExitReturn
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	playanimation BANK_SCRIPTING ANIM_CALL_BACK_POKEMON 0x0
	callasm MakeScriptingBankInvisible
	openpartyscreen BANK_SCRIPTING EmergencyExitReturn
	switchoutabilities BANK_SCRIPTING
	waitstateatk
	switchhandleorder BANK_SCRIPTING 0x2
	returntoball BANK_SCRIPTING
	switch1 BANK_SCRIPTING
	switch2 BANK_SCRIPTING
	hpthresholds BANK_SCRIPTING
	printstring 0x3
	switch3 BANK_SCRIPTING 0x1
	waitstateatk
	switchineffects BANK_SCRIPTING
EmergencyExitReturn:
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_RainDishActivates:
	playanimation BANK_ATTACKER ANIM_HEALING_SPARKLES 0x0
	orword HIT_MARKER HITMARKER_IGNORE_SUBSTITUTE
	graphicalhpupdate BANK_ATTACKER
	datahpupdate BANK_ATTACKER
	printstring 0x133
	waitmessage DELAY_1SECOND
	end2

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_DrySkinDamage:
BattleScript_SolarPowerDamage:
	orword HIT_MARKER HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_NON_ATTACK_DMG
	graphicalhpupdate BANK_ATTACKER
	datahpupdate BANK_ATTACKER
	setword BATTLE_STRING_LOADER HurtByAbilityString
	printstring 0x184
	waitmessage DELAY_1SECOND
	faintpokemon BANK_ATTACKER 0x0 0x0
	end2
	
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Healer:
	setword BATTLE_STRING_LOADER, HealerString
	printstring 0x184
	waitmessage DELAY_1SECOND
	refreshhpbar BANK_TARGET
HealerBSEnd:
	end2
	
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_MoodySingleStat:
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
MoodySkipString:	
	statbuffchange STAT_ATTACKER MoodyStatRaiseEnd
	setgraphicalstatchangevalues
	playanimation BANK_ATTACKER 0x1 0x2023FD4
	printfromtable 0x83FE57C
	waitmessage DELAY_1SECOND
MoodyStatRaiseEnd:
	end2

BattleScript_MoodyRegular:
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
	statbuffchange STAT_ATTACKER MoodyRegularP2
	setgraphicalstatchangevalues
	playanimation BANK_ATTACKER 0x1 0x2023FD4
	printfromtable 0x83FE57C
	waitmessage DELAY_1SECOND
	
MoodyRegularP2:
	callasm LoadMoodyStatToLower
	setbyte SEED_HELPER 0x0
	goto MoodySkipString

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_BadDreams:
	setbyte SEED_HELPER 0x0
	jumpifbyte ANDS USER_BANK 0x1 BadDreams_OpponentSidePlayer1

BadDreams_PlayerSideOpponent1:
	setbyte TARGET_BANK 0x1
	jumpifstatus BANK_TARGET STATUS_SLEEP BadDreams_callhurt
	jumpifability BANK_TARGET ABILITY_COMATOSE BadDreams_callhurt
	addbyte SEED_HELPER 0x1
BadDreams_PlayerSideOpponent2:
	jumpifbyte NOTEQUALS NUM_POKEMON 0x4 BadDreams_end @not double battle
	setbyte TARGET_BANK 0x3
	jumpifstatus BANK_TARGET STATUS_SLEEP BadDreams_callhurt
	jumpifability BANK_TARGET ABILITY_COMATOSE BadDreams_callhurt
	goto BadDreams_end

BadDreams_OpponentSidePlayer1:
	setbyte SEED_HELPER 0x2
	setbyte TARGET_BANK 0x0
	jumpifstatus BANK_TARGET STATUS_SLEEP BadDreams_callhurt
	jumpifability BANK_TARGET ABILITY_COMATOSE BadDreams_callhurt
	addbyte SEED_HELPER 0x1
	BadDreams_OpponentSidePlayer2:
	jumpifbyte NOTEQUALS NUM_POKEMON 0x4 BadDreams_end @not double battle
	setbyte TARGET_BANK 0x2
	jumpifstatus BANK_TARGET STATUS_SLEEP BadDreams_callhurt
	jumpifability BANK_TARGET ABILITY_COMATOSE BadDreams_callhurt
	goto BadDreams_end

BadDreams_callhurt:
	call BadDreams_hurt
	jumpifbyte EQUALS SEED_HELPER 0x1 BadDreams_PlayerSideOpponent2
	jumpifbyte EQUALS SEED_HELPER 0x2 BadDreams_end
	jumpifbyte EQUALS SEED_HELPER 0x3 BadDreams_OpponentSidePlayer2
	jumpifbyte EQUALS SEED_HELPER 0x4 BadDreams_end
	goto BadDreams_end

BadDreams_hurt:
	jumpifability BANK_TARGET ABILITY_MAGICGUARD BadDreams_return
	callasm BadDreamsHurtASM+1
	graphicalhpupdate BANK_TARGET
	datahpupdate BANK_TARGET
	setword BATTLE_STRING_LOADER BadDreamsHurtString
	printstring 0x184
	waitmessage DELAY_1SECOND
	faintpokemon BANK_TARGET 0x0 0x0
	BadDreams_return:
	addbyte SEED_HELPER 0x1
	return

BadDreams_end:
	setbyte SEED_HELPER 0x0
	end2

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Harvest:
	recycleitem HarvestBSEnd
	setword BATTLE_STRING_LOADER HarvestString
	printstring 0x184
	waitmessage DELAY_1SECOND
	callasm HarvestActivateBerry

HarvestBSEnd:
	setbyte FORM_COUNTER 0x0
	end2

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Pickup:
	copyarray USER_BANK 0x2023BC4 0x1
	setword BATTLE_STRING_LOADER PickupString
	printstring 0x184
	waitmessage DELAY_1SECOND
	end2

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_SoundproofProtected:
	attackstring
	ppreduce
	pause DELAY_HALFSECOND
	printstring 0x132 @;STRINGID_PKMNSXBLOCKSY
	waitmessage DELAY_1SECOND
	goto BS_MOVE_END

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_FlashFireBoost_PPLoss:
	ppreduce
BattleScript_FlashFireBoost:
	attackstring
	pause DELAY_HALFSECOND
	printfromtable 0x83FE650 @;gFlashFireStringIds
	waitmessage DELAY_1SECOND
	goto BS_MOVE_END

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_MoveHPDrain_PPLoss:
	ppreduce
BattleScript_MoveHPDrain:
	attackstring
	pause DELAY_HALFSECOND
	playanimation BANK_TARGET ANIM_HEALING_SPARKLES 0x0
	orword HIT_MARKER HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BANK_TARGET
	datahpupdate BANK_TARGET
	printstring 0xC5 @;STRINGID_PKMNRESTOREDHPUSING
	waitmessage DELAY_1SECOND
	orbyte OUTCOME OUTCOME_NOT_AFFECTED
	goto BS_MOVE_END

BattleScript_MonMadeMoveUseless_PPLoss:
	ppreduce
BattleScript_MonMadeMoveUseless:
	attackstring
	pause DELAY_HALFSECOND
	printstring 0x147 @;STRINGID_PKMNSXMADEYUSELESS
	waitmessage DELAY_1SECOND
	orbyte OUTCOME OUTCOME_NOT_AFFECTED
	goto BS_MOVE_END

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_MoveStatDrain_PPLoss:
	ppreduce
BattleScript_MoveStatDrain:
	attackstring
	pause DELAY_HALFSECOND

BattleScript_TargetAbilityStatRaise:
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
	statbuffchange BANK_TARGET BattleScript_MoveStatDrainReturn
	jumpifbyte EQUALS MULTISTRING_CHOOSER 0x2 BattleScript_MoveStatDrainReturn
	setgraphicalstatchangevalues
	playanimation BANK_TARGET ANIM_STAT_BUFF ANIM_ARG_1
	
BattleScript_MoveStatDrainReturn:
	goto BS_MOVE_END
	

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_RoughSkinActivates:
	orword HIT_MARKER, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_NON_ATTACK_DMG
	healthbarupdate BANK_ATTACKER
	datahpupdate BANK_ATTACKER
	printstring 0xCF @;STRINGID_PKMNHURTSWITH
	waitmessage DELAY_1SECOND
	faintpokemon BANK_ATTACKER 0x0 0x0
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global DestinyKnotString
BattleScript_CuteCharmActivates:
	status2animation BANK_ATTACKER STATUS2_INLOVE
	printstring 0x136
	waitmessage DELAY_1SECOND
	jumpifsecondarystatus BANK_TARGET STATUS2_INLOVE CuteCharmActivatesReturn
	jumpifability BANK_TARGET ABILITY_OBLIVIOUS CuteCharmActivatesReturn
	jumpifabilitypresenttargetfield ABILITY_AROMAVEIL CuteCharmActivatesReturn
	jumpifhelditemeffect BANK_ATTACKER ITEM_EFFECT_DESTINY_KNOT CuteCharmDestinyKnot

CuteCharmActivatesReturn:
	return

CuteCharmDestinyKnot:
	tryinfatuatebank BANK_TARGET CuteCharmActivatesReturn
	playanimation BANK_ATTACKER ANIM_ITEM_USE 0x0
	status2animation BANK_TARGET STATUS2_INLOVE
	copyarray BATTLE_SCRIPTING_BANK TARGET_BANK 0x1
	setword BATTLE_STRING_LOADER DestinyKnotString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_WeakArmorActivates:
	jumpifstat BANK_TARGET GREATERTHAN STAT_DEF STAT_MIN WeakArmorModStats
	jumpifstat BANK_TARGET EQUALS STAT_SPDEF STAT_MAX WeakArmorReturn

WeakArmorModStats:
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	setmoveeffect MOVE_EFFECT_DEF_MINUS_1 | MOVE_EFFECT_CERTAIN
	seteffecttarget
	setmoveeffect MOVE_EFFECT_SPD_PLUS_2 | MOVE_EFFECT_CERTAIN
	seteffecttarget

WeakArmorReturn:
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_CursedBodyActivates:
	disablelastusedattack BANK_ATTACKER CursedBodyReturn
	setword BATTLE_STRING_LOADER CursedBodyString
	printstring 0x184
	waitmessage DELAY_1SECOND
	
CursedBodyReturn:
	return
	
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_MummyActivates:
	setword BATTLE_STRING_LOADER MummyString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
BattleScript_GooeyActivates:
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	setbyte STAT_ANIM_PLAYED 0x0
	playstatchangeanimation BANK_ATTACKER STAT_ANIM_SPD STAT_ANIM_DOWN | STAT_ANIM_CAN_FAIL
	setstatchanger STAT_SPD | DECREASE_1
	statbuffchange STAT_ATTACKER | STAT_BS_PTR GooeyReturn
	jumpifbyte EQUALS MULTISTRING_CHOOSER 0x2 GooeyReturn
	printfromtable 0x83FE588
	waitmessage DELAY_1SECOND

GooeyReturn:
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_IllusionBroken:
	reloadhealthbar BANK_TARGET
	playanimation BANK_TARGET ANIM_TRANSFORM 0x0
	setword BATTLE_STRING_LOADER IllusionWoreOffString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_AngerPointActivates:
	setstatchanger STAT_ATK | INCREASE_1
	setgraphicalstatchangevalues
	playanimation BANK_TARGET ANIM_STAT_BUFF ANIM_ARG_1
	setword BATTLE_STRING_LOADER AngerPointString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_SynchronizeActivates_StatusesAttacker:
	waitstateatk
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	copyarray BATTLE_SCRIPTING_BANK BANK_ATTACKER 0x1
	jumpifstatus BANK_ATTACKER STATUS_ANY SynchronizeNoEffect
	seteffecttarget
	jumpifstatus BANK_ATTACKER STATUS_ANY SynchronizeReturn

SynchronizeNoEffect:
	setword BATTLE_STRING_LOADER SynchronizeNoEffectString
	printstring 0x184
	waitmessage DELAY_1SECOND	

SynchronizeReturn:
	return
	
BattleScript_SynchronizeActivates_StatusesTarget:
	waitstateatk
	setword BATTLE_STRING_LOADER AbilityActivatedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	copyarray BATTLE_SCRIPTING_BANK BANK_TARGET 0x1
	jumpifstatus BANK_TARGET STATUS_ANY SynchronizeNoEffect
	seteffecttarget
	jumpifstatus BANK_TARGET STATUS_ANY SynchronizeReturn
	goto SynchronizeNoEffect
	

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Protean:
	pause DELAY_HALFSECOND
	setword BATTLE_STRING_LOADER ProteanString
	printstring 0x184
	waitmessage DELAY_HALFSECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_MimikyuTransform:
	playanimation BANK_TARGET ANIM_TRANSFORM 0x0
	setword BATTLE_STRING_LOADER DisguiseBustedString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_EnduredSturdy:
	setword BATTLE_STRING_LOADER StringEnduredHitWithSturdy
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Receiver:
	setword BATTLE_STRING_LOADER ReceiverString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_Symbiosis:
	setword BATTLE_STRING_LOADER SymbiosisString
	printstring 0x184
	waitmessage DELAY_1SECOND
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

BattleScript_DefiantCompetitive:
	statbuffchange 0xA DefiantReturn
	jumpifbyte EQUALS MULTISTRING_CHOOSER 0x2 DefiantReturn
	setgraphicalstatchangevalues
	playanimation 0xA 0x1 0x2023FD4
	setword BATTLE_STRING_LOADER DefiantString
	printstring 0x184
	waitmessage DELAY_1SECOND

DefiantReturn:
	return

@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

AdrenalineOrbString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xBB, 0xD8, 0xE6, 0xD9, 0xE2, 0xD5, 0xE0, 0xDD, 0xE2, 0xD9, 0x00, 0xC9, 0xE6, 0xD6, 0xFE, 0xE6, 0xD5, 0xDD, 0xE7, 0xD9, 0xD8, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xCD, 0xE4, 0xD9, 0xD9, 0xD8, 0xAB, 0xFF
DownloadString: .byte 0xFD, 0x13, 0xB4, 0xE7, 0x00, 0xFD, 0x1A, 0xFE, 0xE6, 0xD5, 0xDD, 0xE7, 0xD9, 0xD8, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xFD, 0x00, 0xAB, 0xFF

HurtByAbilityString: .byte 0xFD, 0x10, 0x00, 0xEB, 0xD5, 0xE7, 0x00, 0xDC, 0xE9, 0xE6, 0xE8, 0xFE, 0xD6, 0xED, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xFD, 0x19, 0xAB, 0xFF
HealerString: .byte 0xFD, 0x0F, 0xB4, 0xE7, 0x00, 0xFD, 0x18, 0xFE, 0xD7, 0xE9, 0xE6, 0xD9, 0xD8, 0x00, 0xFD, 0x10, 0xB4, 0xE7, 0xFA, 0xFD, 0x00, 0x00, 0xE4, 0xE6, 0xE3, 0xD6, 0xE0, 0xD9, 0xE1, 0xAB, 0xFF
BadDreamsHurtString: .byte 0xFD, 0x10, 0x00, 0xDD, 0xE7, 0x00, 0xE8, 0xE3, 0xE6, 0xE1, 0xD9, 0xE2, 0xE8, 0xD9, 0xD8, 0xFE, 0xD6, 0xED, 0x00, 0xFD, 0x0F, 0xB4, 0xE7, 0x00, 0xFD, 0x18, 0xAB, 0xFF
HarvestString: .byte 0xFD, 0x0F, 0x00, 0xDC, 0xD5, 0xE6, 0xEA, 0xD9, 0xE7, 0xE8, 0xD9, 0xD8, 0xFE, 0xE3, 0xE2, 0xD9, 0x00, 0xFD, 0x16, 0x00, 0xEB, 0xDD, 0xE8, 0xDC, 0x00, 0xFD, 0x18, 0xAB, 0xFF
PickupString: .byte 0xFD, 0x0F, 0x00, 0xDA, 0xE3, 0xE9, 0xE2, 0xD8, 0x00, 0xE3, 0xE2, 0xD9, 0xFE, 0xFD, 0x16, 0x00, 0xEB, 0xDD, 0xE8, 0xDC, 0x00, 0xFD, 0x18, 0xAB, 0xFF
DestinyKnotString: .byte 0xFD, 0x13, 0x00, 0xDA, 0xD9, 0xE0, 0xE0, 0x00, 0xDD, 0xE2, 0x00, 0xE0, 0xE3, 0xEA, 0xD9, 0xFE, 0xDA, 0xE6, 0xE3, 0xE1, 0x00, 0xE8, 0xDC, 0xD9, 0x00, 0xFD, 0x16, 0xAB, 0xFF
CursedBodyString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xFD, 0x19, 0xFE, 0xD8, 0xDD, 0xE7, 0xD5, 0xD6, 0xE0, 0xD9, 0xD8, 0x00, 0xFD, 0x0F, 0xB4, 0xE7, 0x00, 0xFD, 0x14, 0xAB, 0xFF
MummyString: .byte 0xFD, 0x0F, 0xB4, 0xE7, 0x00, 0xD5, 0xD6, 0xDD, 0xE0, 0xDD, 0xE8, 0xED, 0xFE, 0xD6, 0xD9, 0xD7, 0xD5, 0xE1, 0xD9, 0x00, 0xFD, 0x19, 0xAB, 0xFF
AngerPointString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xFD, 0x19, 0xFE, 0xE1, 0xD5, 0xEC, 0xD9, 0xD8, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xFD, 0x0, 0xAB, 0xFF
IllusionWoreOffString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xDD, 0xE0, 0xE0, 0xE9, 0xE7, 0xDD, 0xE3, 0xE2, 0xFE, 0xEB, 0xE3, 0xE6, 0xD9, 0x00, 0xE3, 0xDA, 0xDA, 0xAB, 0xFF
SynchronizeNoEffectString: .byte 0xBC, 0xE9, 0xE8, 0x00, 0xDD, 0xE8, 0x00, 0xDC, 0xD5, 0xD8, 0x00, 0xE2, 0xE3, 0x00, 0xD9, 0xDA, 0xDA, 0xD9, 0xD7, 0xE8, 0x00, 0xE3, 0xE2, 0xFE, 0xFD, 0x13, 0xB0, 0xFF

ProteanString: .byte 0xFD, 0x0F, 0xB4, 0xE7, 0x0, 0xFD, 0x18, 0x0, 0xD7, 0xDC, 0xD5, 0xE2, 0xDB, 0xD9, 0xD8, 0xFE, 0xDD, 0xE8, 0x0, 0xDD, 0xE2, 0xE8, 0xE3, 0x0, 0xE8, 0xDC, 0xD9, 0x0, 0xFD, 0x0, 0x0, 0xE8, 0xED, 0xE4, 0xD9, 0xAB, 0xFF
MimikyuDisguisedTookDamageString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xD8, 0xDD, 0xE7, 0xDB, 0xE9, 0xDD, 0xE7, 0xD9, 0xFE, 0xE7, 0xD9, 0xE6, 0xEA, 0xD9, 0xD8, 0x00, 0xD5, 0xE7, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xD8, 0xD9, 0xD7, 0xE3, 0xED, 0xAB, /*0xFB,*/ 0xFF
DisguiseBustedString: .byte 0xFD, 0x10, 0xB4, 0xE7, 0x00, 0xD8, 0xDD, 0xE7, 0xDB, 0xE9, 0xDD, 0xE7, 0xD9, 0xFE, 0xEB, 0xD5, 0xE7, 0x00, 0xD6, 0xE9, 0xE7, 0xE8, 0xD9, 0xD8, 0xAB, /*0xFB,*/ 0xFF
StringEnduredHitWithSturdy: .byte 0xFD, 0x10, 0x00, 0xD9, 0xE2, 0xD8, 0xE9, 0xE6, 0xD9, 0xD8, 0xFE, 0xE8, 0xDC, 0xD9, 0x00, 0xDC, 0xDD, 0xE8, 0x00, 0xEB, 0xDD, 0xE8, 0xDC, 0x00, 0xFD, 0x19, 0xAB, 0xFF
ReceiverString: .byte 0xFD, 0x13, 0x00, 0xE8, 0xE3, 0xE3, 0xDF, 0x00, 0xE3, 0xEA, 0xD9, 0xE6, 0x00, 0xFE, 0xFD, 0x11, 0xB4, 0xE7, 0x00, 0xFD, 0x1A, 0xAB, 0xFF
SymbiosisString: .byte 0xFD, 0x13, 0x00, 0xE7, 0xDC, 0xD5, 0xE6, 0xD9, 0xD8, 0x00, 0xDD, 0xE8, 0xE7, 0xFE, 0xFD, 0x16, 0x00, 0xEB, 0xDD, 0xE8, 0xDC, 0x00, 0xFD, 0x11, 0xFA, 0xE9, 0xE7, 0xDD, 0xE2, 0xDB, 0x00, 0xFD, 0x1A, 0xAB, 0xFF
DefiantString: .byte 0xFD, 0x13, 0xB4, 0xE7, 0x00, 0xFD, 0x1A, 0xFE, 0xE7, 0xDC, 0xD5, 0xE6, 0xE4, 0xE0, 0xED, 0x00, 0xE6, 0xD5, 0xDD, 0xE7, 0xD9, 0xD8, 0x00, 0xDD, 0xE8, 0xE7, 0x00, 0xFD, 0x00, 0xAB, 0xFF

.align 2
LoadMoodyStatToLower:
	ldr r0, =SEED_HELPER
	ldrb r0, [r0]
	ldr r1, =0x2023FD4
	mov r2, #0x15
	add r2, r0
	strb r2, [r1]
	
	ldr r1, =STAT_CHANGE_BYTE
	mov r2, #0x90
	orr r2, r0
	strb r2, [r1]
	bx lr


BadDreamsHurtASM:
	push {lr}
	ldr r0, =USER_BANK
	ldrb r0, [r0, #0x1]
	bl GetCurrentHealth
	ldrh r0, [r1, #0x4]
	lsr r0, #0x2
	cmp r0, #0x0
	bne SetBDDamage
	mov r0, #0x1

SetBDDamage:
	ldr r1, =DAMAGE_LOC
	str r0, [r1]
	pop {pc}


HarvestActivateBerry:
	push {lr}
	ldr r5, =BS_SCRIPT_LOC
	ldr r1, [r5]
	add r1, #0x5
	str r1, [r5]
	mov r0, #0x1
	ldr r1, =USER_BANK
	ldrb r1, [r1]
	mov r2, #0x0
	bl BerryEffects
	ldr r1, [r5]
	sub r1, #0x5
	str r1, [r5]
	pop {pc}

BerryEffects:
	ldr r3, =0x0801BC68
	bx r3
