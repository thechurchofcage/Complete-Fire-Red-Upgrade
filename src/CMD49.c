#include "defines.h"
#include "helper_functions.h"

//Make sure there's no choice lock glitch
//Add check to see if AI move prediction was successful. If not, then if the same move is predicted, don't predict that same move again.
//Remove the lines at the bottom?
//Put Soul-Heart somewhere

/*Fix references to:
BattleScript_ActionSwitch
BattleScript_PursuitDmgOnSwitchOut
BattleScript_SpikesOnAttackerFainted
BattleScript_SpikesOnTargetFainted
BattleScript_IgnoresWhileAsleep
BattleScript_MoveUsedLoafingAround
BattleScript_IgnoresAndFallsAsleep
*/

#define BattleScript_DefrostedViaFireMove (u8*) 0x81D9098
#define BattleScript_FlushMessageBox (u8*) 0x81D96A8
#define BattleScript_Recoil (u8*) 0x81D9243

extern move_t MovesCanUnfreezeTarget[];
extern move_t DanceMoveTable[];
extern move_t TwoToFiveStrikesMoves[];
extern move_t TwoStrikesMoves[];
extern move_t Percent25RecoilMoves[];
extern move_t Percent33RecoilMoves[];
extern move_t Percent50RecoilMoves[];
extern move_t Percent66RecoilMoves[];
extern move_t Percent75RecoilMoves[];
extern move_t Percent100RecoilMoves[];

extern u8 BattleScript_PoisonTouch[];
extern u8 BattleScript_RageIsBuilding[];
extern u8 BattleScript_BeakBlastBurn[];
extern u8 BattleScript_Magician[];
extern u8 BattleScript_Moxie[];
extern u8 BattleScript_MindBlownDamage[];
extern u8 BattleScript_LifeOrbDamage[];
extern u8 BattleScript_Pickpocket[];
extern u8 BattleScript_DancerActivated[];
extern u8 BattleScript_MultiHitPrintStrings[];
extern u8 BattleScript_FaintAttackerForExplosion[];
extern u8 BattleScript_ExplosionAnim[];
extern u8 BattleScript_PluckEat[];
extern u8 BattleScript_EjectButton[];
extern u8 BattleScript_RedCard[];
extern u8 BattleScript_EmergencyExit[];
extern u8 BattleScript_PrintCustomString[];

extern u8 FreedFromSkyDropString[];

extern u32 SpeedCalc(u8 bank);
extern bool8 SetMoveEffect2(void);

extern u8* gBattleScriptsForMoveEffects[];

enum
{
	ATK49_SET_UP,
	ATK49_ATTACKER_ABILITIES,
    ATK49_RAGE,
    ATK49_SYNCHRONIZE_TARGET,
    ATK49_MOVE_END_ABILITIES,
	ATK49_BEAK_BLAST_BURN,
    ATK49_SYNCHRONIZE_ATTACKER,
    ATK49_CHOICE_MOVE,
	ATK49_UNDO_SKY_DROP,
    ATK49_ATTACKER_INVISIBLE,
	ATK49_TARGET_INVISIBLE,
    ATK49_ATTACKER_VISIBLE,
    ATK49_TARGET_VISIBLE,
	ATK49_ITEM_EFFECTS_END_TURN_TARGET,
	ATK49_ITEM_EFFECTS_END_TURN_ATTACKER,
	ATK49_PLUCK,
	ATK49_ITEM_EFFECTS_CONTACT_ATTACKER,
	ATK49_ITEM_EFFECTS_CONTACT_TARGET,
	ATK49_STATUS_IMMUNITY_ABILITIES,
    ATK49_UPDATE_LAST_MOVES,
    ATK49_MIRROR_MOVE,
	ATK49_MULTI_HIT_MOVES,
	ATK49_DEFROST,
	ATK49_SECOND_MOVE_EFFECT,
	ATK49_MAGICIAN_MOXIE,
    ATK49_NEXT_TARGET,
	ATK49_MOVE_RECOIL,
	ATK49_EJECT_BUTTON,
	ATK49_RED_CARD,
	ATK49_SWITCH_OUT_ABILITIES,
	ATK49_LIFE_ORB_RECOIL,
	ATK49_RESTORE_ABILITIES,
	ATK49_PICKPOCKET,
	ATK49_SUBSTITUTE,
	ATK49_END_ZMOVES,
	ATK49_DANCER,
    ATK49_COUNT //28 or 0x1C
};

enum
{
	Force_Switch_Regular,
	Force_Switch_Dragon_Tail,
	Force_Switch_Red_Card
};

#define ARG_IN_FUTURE_ATTACK 3

void atk49_moveend(void);
bank_t GetNextMultiTarget(void);
void SortBanksBySpeed(u8 banks[], bool8 slowToFast);

void atk49_moveend(void) //All the effects that happen after a move is used
{
    int i;
    u32 effect = FALSE;
    u8 moveType = 0;
    u8 holdEffectAtk = 0;
    u16* choicedMoveAtk = NULL;
    u8 arg1, arg2;
    u16 originallyUsedMove;
	
	u8 bankAtk = gBankAttacker;
	u8 bankDef = gBankTarget;

    if (gChosenMove == -1)
        originallyUsedMove = 0;
    else
        originallyUsedMove = gChosenMove;

    arg1 = gBattlescriptCurrInstr[1];
    arg2 = gBattlescriptCurrInstr[2];

    holdEffectAtk = ITEM_EFFECT(bankAtk);

    choicedMoveAtk = &gBattleStruct->choicedMove[bankAtk];
    moveType = gBattleStruct->dynamicMoveType;
	
	if (gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)
	{ 	//Cancel the Z-Moves
		gNewBS->ZMoveData->active = FALSE;
		gNewBS->ZMoveData->toBeUsed[gBankAttacker] = 0;
	}

    do
    {
        switch (gBattleScripting->atk49_state)
        {
		case ATK49_SET_UP: //For Emergency Exit to use later
			if (gNewBS->MultiHitOn)
				gNewBS->DamageTaken[gBankTarget] += gHpDealt; //Total up damage taken
			else
				gNewBS->DamageTaken[gBankTarget] = gHpDealt;
			
			gNewBS->ResultFlags[gBankTarget] = gMoveResultFlags;
			gBattleScripting->atk49_state++;
            break;
		
		case ATK49_ATTACKER_ABILITIES:			
			if (arg1 != ARG_IN_FUTURE_ATTACK
			&& TOOK_DAMAGE(bankDef)
			&& MOVE_HAD_EFFECT
			&& gBattleMons[bankDef].hp
			&& !MoveBlockedBySubstitute(gCurrentMove, bankAtk, bankDef))
			{
				switch (ABILITY(bankAtk)) {
					case ABILITY_STENCH: //Check for Stench is taken care of in King's Rock check
						if (umodsi(Random(), 100) < 10
						&& gCurrentTurnActionNumber < GetBattlerTurnOrderNum(bankDef)) //Attacker moved before target
						{
							gBattleMons[bankDef].status2 |= STATUS2_FLINCHED;
						}
						break;
					
					case ABILITY_POISONTOUCH:
						if (CheckContact(gCurrentMove, bankAtk)
						&& ABILITY(bankDef) != ABILITY_SHIELDDUST
						&& CanBePoisoned(bankDef, bankAtk)
						&& umodsi(Random(), 100) < 30)
						{
							BattleScriptPushCursor();
							gBattlescriptCurrInstr = BattleScript_PoisonTouch;
							effect = TRUE;
						}
				}
			}
			gBattleScripting->atk49_state++;
            break;
		
        case ATK49_RAGE: // rage check
            if (gBattleMons[bankDef].status2 & STATUS2_RAGE
            && gBattleMons[bankDef].hp 
			&& bankAtk != bankDef
            && SIDE(bankAtk) != SIDE(bankDef)
            && MOVE_HAD_EFFECT 
			&& TOOK_DAMAGE(bankDef)
            && SPLIT(gCurrentMove) != SPLIT_STATUS 
			&& STAT_CAN_RISE(bankDef, STAT_ATK))
            {
                gBattleMons[bankDef].statStages[STAT_ATK-1]++;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_RageIsBuilding;
                effect = TRUE;
            }
            gBattleScripting->atk49_state++;
            break;
			
        case ATK49_SYNCHRONIZE_TARGET: // target synchronize
			if (gCurrentMove != MOVE_PSYCHOSHIFT || !MOVE_HAD_EFFECT) //The lazy way of taking care of Psycho Shift Status Transfer->Synchronize->Heal Status
			{
				if (AbilityBattleEffects(ABILITYEFFECT_SYNCHRONIZE, bankDef, 0, 0, 0))
					effect = TRUE;
			}
            gBattleScripting->atk49_state++;
            break;
			
        case ATK49_MOVE_END_ABILITIES: // Such as abilities activating on contact(Poison Spore, Rough Skin, etc.).
            if (AbilityBattleEffects(ABILITYEFFECT_CONTACT, bankDef, 0, 0, 0))
                effect = TRUE;
            gBattleScripting->atk49_state++;
            break;
		
		case ATK49_BEAK_BLAST_BURN:
			if (CheckContact(gCurrentMove, bankAtk)
			&& MOVE_HAD_EFFECT
			&& TOOK_DAMAGE(bankDef)
			&& gNewBS->BeakBlastByte & gBitTable[bankDef]
			&& CanBeBurned(bankAtk))
			{
				BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_BeakBlastBurn;
                effect = TRUE;
			}
			else
				gBattleScripting->atk49_state++;
            break;
			
        case ATK49_SYNCHRONIZE_ATTACKER: // attacker synchronize
            if (AbilityBattleEffects(ABILITYEFFECT_ATK_SYNCHRONIZE, bankAtk, 0, 0, 0))
                effect = TRUE;
            gBattleScripting->atk49_state++;
            break;
			
        case ATK49_CHOICE_MOVE: // update choice band move
			if (arg1 != ARG_IN_FUTURE_ATTACK)
			{
				if (gHitMarker & HITMARKER_OBEYS)
				{
					if (holdEffectAtk == ITEM_EFFECT_CHOICE_BAND
					&&  gChosenMove != MOVE_STRUGGLE
					&& (*choicedMoveAtk == 0 || *choicedMoveAtk == 0xFFFF))
					{
						if (gChosenMove == MOVE_BATONPASS && !(gMoveResultFlags & MOVE_RESULT_FAILED))
						{
							gBattleScripting->atk49_state++;
							break;
						}
						
						*choicedMoveAtk = gChosenMove;
					}
					else //This should remove the choice lock glitch
						*choicedMoveAtk = 0;
				}
				
				for (i = 0; i < MAX_MON_MOVES; ++i)
				{
					if (gBattleMons[bankAtk].moves[i] == *choicedMoveAtk)
						break;
				}
				if (i == MAX_MON_MOVES)
					*choicedMoveAtk = 0;
			}
            gBattleScripting->atk49_state++;
            break;
		
		case ATK49_UNDO_SKY_DROP:  //The attacker can no longer attack while in the air due to paralysis etc.
			if (gCurrentMove == MOVE_SKYDROP
			&& gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE
			&& gStatuses3[gBankAttacker] & STATUS3_SKY_DROP_ATTACKER)
			{
				gStatuses3[gBankAttacker] &= ~STATUS3_SKY_DROP_ATTACKER;
				gStatuses3[gBankTarget] &= ~STATUS3_SKY_DROP_TARGET;
				BattleStringLoader = FreedFromSkyDropString;
				BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_PrintCustomString;
			}
            gBattleScripting->atk49_state++;
            break;

        case ATK49_ATTACKER_INVISIBLE: // make attacker sprite invisible
            if (gStatuses3[bankAtk] & (STATUS3_SEMI_INVULNERABLE)
            &&  gHitMarker & HITMARKER_NO_ANIMATIONS)
            {
                gActiveBattler = bankAtk;
                EmitSpriteInvisibility(0, TRUE);
                MarkBufferBankForExecution(gActiveBattler);
                gBattleScripting->atk49_state++;
                return;
            }
            gBattleScripting->atk49_state++;
            break;
			
        case ATK49_TARGET_INVISIBLE: // make target sprite invisible
            if (gStatuses3[bankDef] & (STATUS3_SEMI_INVULNERABLE)
            &&  gHitMarker & HITMARKER_NO_ANIMATIONS)
            {
                gActiveBattler = bankDef;
                EmitSpriteInvisibility(0, TRUE);
                MarkBufferBankForExecution(gActiveBattler);
                gBattleScripting->atk49_state++;
                return;
            }
            gBattleScripting->atk49_state++;
            break;
		
        case ATK49_ATTACKER_VISIBLE: // make attacker sprite visible
            if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT
            || !(gStatuses3[bankAtk] & (STATUS3_SEMI_INVULNERABLE))
            || WasUnableToUseMove(bankAtk))
            {
                gActiveBattler = bankAtk;
                EmitSpriteInvisibility(0, FALSE);
                MarkBufferBankForExecution(gActiveBattler);
                gStatuses3[bankAtk] &= ~(STATUS3_SEMI_INVULNERABLE);
                gSpecialStatuses[bankAtk].restoredBankSprite = 1;
                gBattleScripting->atk49_state++;
                return;
            }
            gBattleScripting->atk49_state++;
            break;
		
        case ATK49_TARGET_VISIBLE: // make target sprite visible
            if (!gSpecialStatuses[bankDef].restoredBankSprite 
			&& bankDef < gBattlersCount
            && !(gStatuses3[bankDef] & STATUS3_SEMI_INVULNERABLE))
            {
                gActiveBattler = bankDef;
                EmitSpriteInvisibility(0, FALSE);
                MarkBufferBankForExecution(gActiveBattler);
                gStatuses3[bankDef] &= ~(STATUS3_SEMI_INVULNERABLE);
                gBattleScripting->atk49_state++;
                return;
            }
            gBattleScripting->atk49_state++;
            break;

        case ATK49_ITEM_EFFECTS_CONTACT_ATTACKER:
			if (arg1 != ARG_IN_FUTURE_ATTACK)
			{
				if (ItemBattleEffects(ItemEffects_ContactAttacker, bankAtk, TRUE, FALSE)) //Attacker comes first because it can be switched
					effect = TRUE;														  //out with Red Card
			}
			gBattleScripting->atk49_state++;
			break;
			
        case ATK49_ITEM_EFFECTS_CONTACT_TARGET:
            if (ItemBattleEffects(ItemEffects_ContactTarget, bankDef, TRUE, FALSE))
				effect = TRUE;
			gBattleScripting->atk49_state++;
			break;

		case ATK49_PLUCK:
			if (gBattleMoves[gCurrentMove].effect == EFFECT_EAT_BERRY
			&& ITEM_POCKET(bankDef) == POCKET_BERRY_POUCH
			&&  ABILITY(bankDef) != ABILITY_STICKYHOLD)
			{
				gLastUsedItem = gBattleMons[bankDef].item;
				gBattleMons[bankDef].item = 0;
					
				gActiveBattler = bankDef;
                EmitSetMonData(0, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[gActiveBattler].item);
                MarkBufferBankForExecution(gActiveBattler);

                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_PluckEat;
			}
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_ITEM_EFFECTS_END_TURN_TARGET:
			if (ItemBattleEffects(ItemEffects_EndTurn, bankDef, TRUE, FALSE))
                effect = TRUE;
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_ITEM_EFFECTS_END_TURN_ATTACKER:
			if (ItemBattleEffects(ItemEffects_EndTurn, bankAtk, TRUE, FALSE))
                effect = TRUE;
			gBattleScripting->atk49_state++;
			break;
		
        case ATK49_STATUS_IMMUNITY_ABILITIES: // status immunities
            if (AbilityBattleEffects(ABILITYEFFECT_IMMUNITY, 0, 0, 0, 0))
                effect = TRUE; // it loops through all battlers, so we increment after its done with all battlers
            else
                gBattleScripting->atk49_state++;
            break;
		
        case ATK49_UPDATE_LAST_MOVES:
			if (gNewBS->DancerInProgress
			||  arg1 == ARG_IN_FUTURE_ATTACK) 
			{
				gBattleScripting->atk49_state++;
				break;
			}
			
            if (gHitMarker & HITMARKER_SWAP_ATTACKER_TARGET)
            {
                bankAtk = gBankTarget;
                bankDef = gBankAttacker;
                gHitMarker &= ~(HITMARKER_SWAP_ATTACKER_TARGET);
            }
			
            if (gHitMarker & HITMARKER_ATTACKSTRING_PRINTED)
            {
                gLastPrintedMoves[bankAtk] = gChosenMove;
				gNewBS->lastTargeted[gBankAttacker] = gBankTarget;
				gNewBS->usedMoveIndices[gBankAttacker] |= gBitTable[gCurrMovePos];
            }
			
            if (!(gAbsentBattlerFlags & gBitTable[bankAtk])
            && !(gBattleStruct->field_91 & gBitTable[bankAtk])
            && gBattleMoves[originallyUsedMove].effect != EFFECT_BATON_PASS)
            {
                if (gHitMarker & HITMARKER_OBEYS)
                {
                    gLastUsedMoves[bankAtk] = gChosenMove;
                    gLastResultingMoves[bankAtk] = gCurrentMove;
                }
                else
                {
                    gLastUsedMoves[bankAtk] = 0xFFFF;
                    gLastResultingMoves[bankAtk] = 0xFFFF;
                }

                if (!(gHitMarker & HITMARKER_FAINTED(bankDef)))
                    gLastHitBy[bankDef] = bankAtk;

                if (gHitMarker & HITMARKER_OBEYS && MOVE_HAD_EFFECT)
                {
                    if (gChosenMove == 0xFFFF)
                    {
                        gLastLandedMoves[bankDef] = gChosenMove;
                    }
                    else
                    {
						if (gNewBS->ZMoveData->active)
							gLastLandedMoves[bankDef] = gChosenMove;
						
						else
							gLastLandedMoves[bankDef] = gCurrentMove;

						gLastHitByType[bankDef] = moveType;
                    }
                }
                else
                {
                    gLastLandedMoves[bankDef] = 0xFFFF;
                }
            }
            gBattleScripting->atk49_state++;
            break;
		
        case ATK49_MIRROR_MOVE:
            if (arg1 != ARG_IN_FUTURE_ATTACK
			&& !(gAbsentBattlerFlags & gBitTable[bankAtk]) 
			&& !(gBattleStruct->field_91 & gBitTable[bankAtk])
            && gBattleMoves[originallyUsedMove].flags & FLAG_MIRROR_MOVE_AFFECTED 
			&& gHitMarker & HITMARKER_OBEYS
            && bankAtk != bankDef 
			&& !(gHitMarker & HITMARKER_FAINTED(bankDef))
            && MOVE_HAD_EFFECT)
            {
                gBattleStruct->lastTakenMove[bankDef] = gChosenMove;
				gBattleStruct->lastTakenMoveFrom[bankDef][bankAtk] = gChosenMove;
            }
			
			if (gNewBS->ZMoveData->active)
            {
                gBattleStruct->lastTakenMove[bankDef] = 0;
				gBattleStruct->lastTakenMoveFrom[bankDef][bankAtk] = 0;
            }
			
            gBattleScripting->atk49_state++;
			SeedHelper[0] = 0; //Reset Seed Helper for Soul Heart
            break;
		
		case ATK49_MULTI_HIT_MOVES:
			if (arg1 != ARG_IN_FUTURE_ATTACK
			&& !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
			&& !(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)
			&& gMultiHitCounter)
			{
				++gBattleScripting->multihitString[4];
				if (--gMultiHitCounter == 0)
				{
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_MultiHitPrintStrings;
					effect = 1;
				}
				else
				{
					if (gBattleMons[gBankAttacker].hp
					&& gBattleMons[gBankTarget].hp
					&& (gChosenMove == MOVE_SLEEPTALK || !(gBattleMons[gBankAttacker].status1 & STATUS1_SLEEP))
					&& !(gBattleMons[gBankAttacker].status1 & STATUS1_FREEZE))
					{
						if (gNewBS->ParentalBondOn)
							--gNewBS->ParentalBondOn;
						
						gHitMarker |= (HITMARKER_NO_PPDEDUCT | HITMARKER_NO_ATTACKSTRING);
						gBattleScripting->animTargetsHit = 0;
						gBattleScripting->atk49_state = 0;
						gNewBS->MultiHitOn = TRUE; //Used to help second accuracy calcs
						MoveValuesCleanUp();
						BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
						gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
						return;
					}
					else
					{
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_MultiHitPrintStrings;
						effect = 1;
					}
				}
			}
			
            gBattleScripting->atk49_state++;
			gMultiHitCounter = 0;
			gNewBS->ParentalBondOn = 0;
			gNewBS->MultiHitOn = FALSE;
            break;

        case ATK49_DEFROST: // defrosting check
            if (gBattleMons[bankDef].status1 & STATUS1_FREEZE
            &&  gBattleMons[bankDef].hp 
			&&  bankAtk != bankDef
            &&  MOVE_HAD_EFFECT
            &&  TOOK_DAMAGE(bankDef)
			&& (moveType == TYPE_FIRE || CheckTableForMove(gCurrentMove, MovesCanUnfreezeTarget)))
            {
                gBattleMons[bankDef].status1 &= ~(STATUS1_FREEZE);
                gActiveBattler = bankDef;
                EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[bankDef].status1);
                MarkBufferBankForExecution(gActiveBattler);
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_DefrostedViaFireMove;
                effect = TRUE;
            }
            gBattleScripting->atk49_state++;
            break;
		
		case ATK49_SECOND_MOVE_EFFECT:
			gBattleCommunication[MOVE_EFFECT_BYTE] = gNewBS->backupMoveEffect;
			gNewBS->backupMoveEffect = 0;
			effect = SetMoveEffect2();
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_MAGICIAN_MOXIE:
			switch (ABILITY(bankAtk)) {
				case ABILITY_MAGICIAN:
					if (ITEM(bankAtk) == 0
					&& ITEM(bankDef)
					&& gBattleMons[bankAtk].hp
					&& !MoveBlockedBySubstitute(gCurrentMove, bankAtk, bankDef)
					&& TOOK_DAMAGE(bankDef)
					&& MOVE_HAD_EFFECT
					&& CanTransferItem(gBattleMons[bankDef].species, ITEM(bankDef), GetBankPartyData(bankDef))
					&& CanTransferItem(gBattleMons[bankAtk].species, ITEM(bankDef), GetBankPartyData(bankAtk))
					&& (ABILITY(bankDef) != ABILITY_STICKYHOLD || gBattleMons[bankDef].hp == 0))
					{
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Magician;
						effect = 1;
					}
					break;
					
				case ABILITY_MOXIE:
					if ((arg1 != ARG_IN_FUTURE_ATTACK || gWishFutureKnock->futureSightPartyIndex[bankDef] == gBattlerPartyIndexes[bankAtk])
					&& gBattleMons[bankDef].hp == 0
					&& gBattleMons[bankAtk].hp
					&& TOOK_DAMAGE(bankDef)
					&& MOVE_HAD_EFFECT
					&& STAT_CAN_RISE(gBankAttacker, STAT_STAGE_ATK)
					&& PartyAlive(bankDef))
					{
						PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_STAGE_ATK);
						
						gEffectBank = gBankAttacker;
						gBattleScripting->bank = gBankAttacker;
						gBattleScripting->statChanger = INCREASE_1 | STAT_STAGE_ATK;
						gBattleScripting->animArg1 = 0xE + STAT_STAGE_ATK;
						gBattleScripting->animArg2 = 0;
						gLastUsedAbility = ABILITY_MOXIE;
						RecordAbilityBattle(gBankAttacker, gLastUsedAbility);
						
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Moxie;
						effect = 1;
					}
					break;
					
				case ABILITY_BEASTBOOST: ;
					if ((arg1 != ARG_IN_FUTURE_ATTACK || gWishFutureKnock->futureSightPartyIndex[bankDef] == gBattlerPartyIndexes[bankAtk])
					&& gBattleMons[bankDef].hp == 0
					&& gBattleMons[bankAtk].hp
					&& TOOK_DAMAGE(bankDef)
					&& MOVE_HAD_EFFECT
					&& PartyAlive(bankDef))
					{
						u16 temp;
						u16 max;
						u16 stats[STAT_STAGE_SPDEF]; //Create new array to avoid modifying original stats
						
						stats[STAT_STAGE_ATK-1] = gBattleMons[bankAtk].attack;
						stats[STAT_STAGE_DEF-1] = gBattleMons[bankAtk].defense;
						stats[STAT_STAGE_SPATK-2] = gBattleMons[bankAtk].spAttack;
						stats[STAT_STAGE_SPDEF-2] = gBattleMons[bankAtk].spDefense;
						stats[STAT_STAGE_SPEED+1] = gBattleMons[bankAtk].speed;
						
						if (gNewBS->WonderRoomTimer) {
							temp = stats[STAT_STAGE_DEF-1];
							stats[STAT_STAGE_DEF-1] = stats[STAT_STAGE_SPDEF-2]; //-2 b/c shifted left due to speed
							stats[STAT_STAGE_SPDEF-2] = temp;
						}
						
						max = 0;
						for (int i = 1; i < STAT_STAGE_SPDEF; ++i) {
							if (stats[i] > stats[max])
								max = i;
						}
						
						//Get the proper stat stage value
						switch(max) {
							case 0: //Attack
							case 1: //Defense
								max += 1;
								break;
							case 2: //Special Attack
							case 3: //Special Defense
								max += 2;
								break;
							case 4:
								max = STAT_STAGE_SPEED;
						}
						
						if (STAT_CAN_RISE(gBankAttacker, max))
						{
							PREPARE_STAT_BUFFER(gBattleTextBuff1, max);
							
							gEffectBank = gBankAttacker;
							gBattleScripting->bank = gBankAttacker;
							gBattleScripting->statChanger = INCREASE_1 | max;
							gBattleScripting->animArg1 = 0xE + max;
							gBattleScripting->animArg2 = 0;
							gLastUsedAbility = ABILITY_BEASTBOOST;
							RecordAbilityBattle(gBankAttacker, gLastUsedAbility);
							
							BattleScriptPushCursor();
							gBattlescriptCurrInstr = BattleScript_Moxie;
							effect = 1;
						}
					}
			}
            gBattleScripting->atk49_state++;
            break;
		
        case ATK49_NEXT_TARGET: // For moves hitting two opposing Pokemon or whole field.
			gHitMarker |= HITMARKER_NO_ATTACKSTRING;
			if (gNewBS->MoveBounceInProgress)
				++gNewBS->MoveBounceTargetCount;
			else
				++gNewBS->OriginalAttackerTargetCount;

			if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
			{
				if (!(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)
				&& !gProtectStructs[bankAtk].chargingTurn)
				{
					if (gBattleMoves[gCurrentMove].target == MOVE_TARGET_BOTH)
					{
						if ((gNewBS->OriginalAttackerTargetCount < 2 && !gNewBS->MoveBounceInProgress)
						|| (gNewBS->MoveBounceTargetCount < 2 && gNewBS->MoveBounceInProgress))
						{ //Get Next Target
							u8 battlerId = PARTNER(bankDef);
							if (gBattleMons[battlerId].hp && battlerId != gBankAttacker)
							{
								gBankTarget = battlerId;
								gBattleScripting->atk49_state = 0;
								MoveValuesCleanUp();
								BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
								gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
								return;
							}
							else if (gNewBS->MoveBounceInProgress)
								goto RESTORE_BOUNCE_ATTACKER;
						}
						else if (gNewBS->MoveBounceInProgress)
						{
						RESTORE_BOUNCE_ATTACKER:
							++gNewBS->OriginalAttackerTargetCount;
							gNewBS->MoveBounceInProgress = 2; //Bounce just finished
							gNewBS->MoveBounceTargetCount = 0;
							u8 battlerId = PARTNER(bankAtk);
							if (gBattleMons[battlerId].hp && gNewBS->OriginalAttackerTargetCount < 2)
							{
								gBankAttacker = gBanksByTurnOrder[gCurrentTurnActionNumber]; //Restore original attacker
								gBankTarget = battlerId; //Attack Bouncer's partner
								gBattleScripting->animTargetsHit = 0;
								gBattleScripting->atk49_state = 0;
								MoveValuesCleanUp();
								BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
								gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
								return;
							}
						}
					}
					else if (gBattleMoves[gCurrentMove].target == MOVE_TARGET_ALL)
					{
						if (gNewBS->OriginalAttackerTargetCount < 3)
						{ //Get Next Target
							u8 battlerId = GetNextMultiTarget();
							if (battlerId != 0xFF)
							{
								while (GetNextMultiTarget() != 0xFF)
								{
									gBankTarget = battlerId;
									if (gBattleMons[battlerId].hp && battlerId != gBankAttacker)
									{
										gBattleScripting->atk49_state = 0;
										MoveValuesCleanUp();
										BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
										gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
										return;
									}
									++gNewBS->OriginalAttackerTargetCount;
								}
							}
						}
					}
				}
			}
			
			if (gNewBS->MoveBounceInProgress)
				gNewBS->MoveBounceInProgress = 2; //Bounce just finished
			
			gNewBS->MoveBounceTargetCount = 0;			
			gNewBS->OriginalAttackerTargetCount = 0;
            gBattleScripting->atk49_state++;
            break;
		
		case ATK49_MOVE_RECOIL:
			if (arg1 != ARG_IN_FUTURE_ATTACK
			&& gBattleMons[bankAtk].hp
			&&  !(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE))
			{
				if (gCurrentMove == MOVE_MINDBLOWN
				&& ABILITY(bankAtk) != ABILITY_MAGICGUARD)
				{
					gBattleMoveDamage = MathMax(1, gBattleMons[bankAtk].maxHP / 2);
					
					if (IS_ODD(gBattleMons[bankAtk].maxHP)
					&& gBattleMons[bankAtk].maxHP >= 3)
						++gBattleMoveDamage; //Round up Damage
						
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_MindBlownDamage;
					
					effect = 1;
				}
				else if (gCurrentMove == MOVE_STRUGGLE)
				{
					gBattleMoveDamage = MathMax(1, gBattleMons[bankAtk].maxHP / 4);
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_Recoil;
					effect = 1;
				}
				else if (gBattleMoves[gCurrentMove].effect == EFFECT_EXPLOSION
				&& !ABILITY_PRESENT(ABILITY_DAMP))
				{
					BattleScriptPushCursor();
					
					if (gNewBS->AttackerDidDamageAtLeastOnce)
						gBattlescriptCurrInstr = BattleScript_FaintAttackerForExplosion;
					else
						gBattlescriptCurrInstr = BattleScript_ExplosionAnim; //The attacker still needs to explode
				
					effect = 1;
				}
				else if (ABILITY(bankAtk) != ABILITY_MAGICGUARD
				&& ABILITY(bankAtk) != ABILITY_ROCKHEAD
				&& gNewBS->AttackerDidDamageAtLeastOnce)
				{
					if (CheckTableForMove(gCurrentMove, Percent25RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, udivsi(gHpDealt, 4));
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
					else if (CheckTableForMove(gCurrentMove, Percent33RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, udivsi(gHpDealt, 3));
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
					else if (CheckTableForMove(gCurrentMove, Percent50RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, udivsi(gHpDealt, 2));
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
					else if (CheckTableForMove(gCurrentMove, Percent66RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, udivsi(gHpDealt * 2, 3));
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
					else if (CheckTableForMove(gCurrentMove, Percent75RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, udivsi(gHpDealt * 3, 4));
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
					else if (CheckTableForMove(gCurrentMove, Percent100RecoilMoves))
					{
						gBattleMoveDamage = MathMax(1, gHpDealt);
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_Recoil;
						effect = 1;
					}
				}
			}
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_EJECT_BUTTON:
			if (gBattleMoves[gCurrentMove].effect != EFFECT_ROAR)
			{
				u8 banks[4] = {0, 1, 2, 3};
				SortBanksBySpeed(banks, FALSE);
				
				for (i = 0; i < gBattlersCount; ++i)
				{
					if (banks[i] != gBankAttacker
					&&  gBattleMons[banks[i]].hp
					&&  !SheerForceCheck()
					&&  ITEM_EFFECT(banks[i]) == ITEM_EFFECT_EJECT_BUTTON
					&&  !(gNewBS->ResultFlags[banks[i]] & MOVE_RESULT_NO_EFFECT)
					&&  gNewBS->DamageTaken[banks[i]]
					&&  !MoveBlockedBySubstitute(gCurrentMove, gBankAttacker, gBankTarget)
					&&  ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) || SIDE(i) == B_SIDE_PLAYER)) //Wilds can't activate
					{
						if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
							gBattlescriptCurrInstr = BattleScript_MoveEnd; //Cancel switchout for U-Turn & Volt Switch
						
						gNewBS->NoSymbiosisByte = TRUE;
						gActiveBattler = gBattleScripting->bank = banks[i];
						gLastUsedItem = ITEM(banks[i]);
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_EjectButton;
						effect = 1;
						break; //Only the fastest Eject Button activates so end loop
					}
				}
			}
			gBattleScripting->atk49_state++;
			break;
			
		case ATK49_RED_CARD:
			if (gBattleMoves[gCurrentMove].effect != EFFECT_ROAR)
			{
				u8 banks[4] = {0, 1, 2, 3};
				SortBanksBySpeed(banks, FALSE);
				
				for (i = 0; i < gBattlersCount; ++i)
				{
					if (banks[i] != gBankAttacker
					&&  gBattleMons[banks[i]].hp
					&&  !SheerForceCheck()
					&&  ITEM_EFFECT(banks[i]) == ITEM_EFFECT_RED_CARD
					&&  !(gNewBS->ResultFlags[banks[i]] & MOVE_RESULT_NO_EFFECT)
					&&  gNewBS->DamageTaken[banks[i]]
					&&  !MoveBlockedBySubstitute(gCurrentMove, gBankAttacker, gBankTarget)
					&&  ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) || SIDE(gBankAttacker) == B_SIDE_PLAYER)) //Wild attackers can't activate
					{
						gNewBS->NoSymbiosisByte = TRUE;
						ForceSwitchHelper = Force_Switch_Red_Card;
						gBattlescriptCurrInstr = BattleScript_Atk49; //Cancel's U-Turn and Volt Switch
						gActiveBattler = gBattleScripting->bank = banks[i];
						gLastUsedItem = ITEM(banks[i]);
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_RedCard;
						effect = 1;
						break; //Only the fastest Red Card activates so end loop
					}
				}
			}
			*SeedHelper = 0; //For Later
			gBattleScripting->atk49_state++;
			break;

		case ATK49_SWITCH_OUT_ABILITIES:
			if (gBattleMoves[gCurrentMove].effect != EFFECT_ROAR)
			{
				for (; *SeedHelper < gBattlersCount; ++*SeedHelper)
				{
					if (*SeedHelper != gBankAttacker
					&&  !SheerForceCheck()
					&& (ABILITY(*SeedHelper) == ABILITY_WIMPOUT || ABILITY(*SeedHelper) == ABILITY_PRESSURE)
					&&  !(gNewBS->ResultFlags[*SeedHelper] & MOVE_RESULT_NO_EFFECT)
					&&  !MoveBlockedBySubstitute(gCurrentMove, gBankAttacker, gBankTarget)
					&&  !(gStatuses3[*SeedHelper] & (STATUS3_SKY_DROP_ANY))
					&&  gBattleMons[*SeedHelper].hp
					&&  gBattleMons[*SeedHelper].hp < gBattleMons[*SeedHelper].maxHP / 2
					&&  gBattleMons[*SeedHelper].hp + gNewBS->DamageTaken[*SeedHelper] > gBattleMons[*SeedHelper].maxHP / 2) //Fell this turn
					{
						if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
							gBattlescriptCurrInstr = BattleScript_Atk49; //Cancel switchout for U-Turn & Volt Switch
						
						gActiveBattler = gBattleScripting->bank = *SeedHelper;
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_EmergencyExit;
						effect = 1;
						break;
					}
				}
			}
            gBattleScripting->atk49_state++;
            break;
	
		case ATK49_LIFE_ORB_RECOIL:
			if (arg1 != ARG_IN_FUTURE_ATTACK
			&& ITEM_EFFECT(bankAtk) == ITEM_EFFECT_LIFE_ORB
			&& gNewBS->AttackerDidDamageAtLeastOnce
			&& ABILITY(bankAtk) != ABILITY_MAGICGUARD
			&& gBattleMons[bankAtk].hp
			&& !SheerForceCheck())
			{
				gBattleMoveDamage = MathMax(1, udivsi(gBattleMons[bankAtk].maxHP, 10));
				BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_LifeOrbDamage;
				effect = 1;
			}
			gBattleScripting->atk49_state++;
			break;

		case ATK49_RESTORE_ABILITIES:
			for (i = 0; i < gBattlersCount; ++i)
			{
				if (gNewBS->DisabledMoldBreakerAbilities[i])
				{
					gBattleMons[i].ability = gNewBS->DisabledMoldBreakerAbilities[i];
					gNewBS->DisabledMoldBreakerAbilities[i] = 0;
				}
			}
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_PICKPOCKET: //Move to before choose new target?
			if (ABILITY(bankDef) == ABILITY_PICKPOCKET
			&& CheckContact(gCurrentMove, bankAtk)
			&& gBattleMons[bankDef].hp
			&& gMultiHitCounter <= 1
			&& ITEM(bankAtk)
			&& ITEM(bankDef) == 0
			&& (ABILITY(bankAtk) != ABILITY_STICKYHOLD || gBattleMons[bankAtk].hp == 0)
			&& !MoveBlockedBySubstitute(gCurrentMove, bankAtk, bankDef)
			&& TOOK_DAMAGE(bankDef)
			&& MOVE_HAD_EFFECT)
			{
				BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_Pickpocket;
				effect = 1;
			}
			gBattleScripting->atk49_state++;
			break;

        case ATK49_SUBSTITUTE: // update substitute
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gDisableStructs[i].substituteHP == 0)
                    gBattleMons[i].status2 &= ~(STATUS2_SUBSTITUTE);
            }
            gBattleScripting->atk49_state++;
            break;
		
		case ATK49_END_ZMOVES:
			if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
			&&  gBattleMoves[gCurrentMove].target & (MOVE_TARGET_BOTH | MOVE_TARGET_ALL))
			{
				int i;
				
				for (i = 0; i < 4; ++i)
				{
					if (!(gNewBS->ResultFlags[i] & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED))) //If at least one of the targets was affected
						break;
				}
				if (i == 4) //No target's were affected
					gNewBS->StompingTantrumTimers[gBankAttacker]++;
			}
			else if (gMoveResultFlags & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED))
				gNewBS->StompingTantrumTimers[gBankAttacker]++;
			
			for (int i = 0; i < 4; ++i)
			{
				gNewBS->DamageTaken[i] = 0;
				gNewBS->ResultFlags[i] = 0;
			}
			
			gNewBS->ZMoveData->active = FALSE;
			gNewBS->ZMoveData->effectApplied = FALSE;
			gNewBS->AttackerDidDamageAtLeastOnce = FALSE;		
			gNewBS->secondaryEffectApplied = FALSE;
			gNewBS->InstructInProgress = FALSE;
			gNewBS->bypassSubstitute = FALSE;
			gBattleScripting->atk49_state++;
			break;
		
		case ATK49_DANCER:	
			if (gNewBS->DancerBankCount == gBattlersCount)
			{
				gNewBS->DancerInProgress = FALSE;
				gNewBS->DancerByte = 0;
				gNewBS->DancerTurnOrder[0] = 0; //Set the whole array to 0
				gNewBS->DancerTurnOrder[1] = 0;
				gNewBS->DancerTurnOrder[2] = 0;
				gNewBS->DancerTurnOrder[3] = 0;
				gBattleScripting->atk49_state++;
				break;
			}
		
			if (!gNewBS->DancerInProgress
			&& ABILITY_PRESENT(ABILITY_DANCER)
			&& gHitMarker & HITMARKER_ATTACKSTRING_PRINTED
			&& MOVE_HAD_EFFECT
			&& !gNewBS->MoveBounceInProgress
			&& CheckTableForMove(gCurrentMove, DanceMoveTable))
			{
				gNewBS->CurrentTurnAttacker = bankAtk;
				gNewBS->DancerBankCount = 0;
				
				for (i = 0; i < gBattlersCount; ++i)
					gNewBS->DancerTurnOrder[i] = i;
					
				SortBanksBySpeed(gNewBS->DancerTurnOrder, TRUE);
			}
			else if (!gNewBS->DancerInProgress)
			{
				gBattleScripting->atk49_state++;
				break;
			}
				
			u8 bank = gNewBS->DancerTurnOrder[gNewBS->DancerBankCount];
			
			if (ABILITY(bank) == ABILITY_DANCER
			&& !(gAbsentBattlerFlags & gBitTable[bank])
			&& bank != gNewBS->CurrentTurnAttacker)
			{
				gBankAttacker = bank;
				gBankTarget = GetMoveTarget(gCurrentMove, FALSE);
				gBattleScripting->atk49_state = 0;
				gRandomMove = gCurrentMove;
				gBattleScripting->bank = gBankAttacker;
                gBattlescriptCurrInstr = BattleScript_DancerActivated;
				gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING | HITMARKER_ATTACKSTRING_PRINTED);
				gNewBS->AttackerDidDamageAtLeastOnce = FALSE;
				gNewBS->DancerInProgress = TRUE;
				effect = TRUE;
			}
			
			++gNewBS->DancerBankCount;
			break;
			
        case ATK49_COUNT:
            break;
        }
		
		//Should I remove these lines?
        if (arg1 == 1 && effect == FALSE)
            gBattleScripting->atk49_state = ATK49_COUNT;
        if (arg1 == 2 && arg2 == gBattleScripting->atk49_state)
            gBattleScripting->atk49_state = ATK49_COUNT;

    } while (gBattleScripting->atk49_state != ATK49_COUNT && effect == FALSE);
	
    if (gBattleScripting->atk49_state == ATK49_COUNT && effect == FALSE)
        gBattlescriptCurrInstr += 3;
}

const bank_t gTargetsByBank[4][4] = 
{
	{B_POSITION_OPPONENT_LEFT, B_POSITION_OPPONENT_RIGHT, B_POSITION_PLAYER_RIGHT, 0xFF},	//Bank 0 - Player Left 
	{B_POSITION_PLAYER_LEFT, B_POSITION_PLAYER_RIGHT, B_POSITION_OPPONENT_RIGHT, 0xFF}, 	//Bank 1 - Opponent Left
	{B_POSITION_OPPONENT_LEFT, B_POSITION_OPPONENT_RIGHT, B_POSITION_PLAYER_LEFT, 0xFF}, 	//Bank 2 - Player Right
	{B_POSITION_PLAYER_LEFT, B_POSITION_PLAYER_RIGHT, B_POSITION_OPPONENT_LEFT, 0xFF}  		//Bank 3 - Opponent Right
};

bank_t GetNextMultiTarget(void) 
{
	return gTargetsByBank[gBankAttacker][gNewBS->OriginalAttackerTargetCount];
}

void SortBanksBySpeed(u8 banks[], bool8 slowToFast)
{
	int i, j, key, keyBank;
	u16 speeds[4] = {0};
	for (i = 0; i < gBattlersCount; ++i)
		speeds[i] = SpeedCalc(banks[i]);

	for (i = 1; i < gBattlersCount; ++i) 
	{ 
		keyBank = banks[i];
		key = speeds[i]; 
		j = i - 1; 

		if (slowToFast)
		{
			while (j >= 0 && speeds[j] > key) 
			{ 
				banks[j + 1] = banks[j];
				speeds[j + 1] = speeds[j];
				j = j - 1; 
			}
		}
		else
		{
			while (j >= 0 && speeds[j] < key) 
			{ 
				banks[j + 1] = banks[j];
				speeds[j + 1] = speeds[j];
				j = j - 1; 
			}
		}
		
		banks[j + 1] = keyBank; 
		speeds[j + 1] = key;
	} 
}