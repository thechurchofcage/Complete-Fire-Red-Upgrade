#include "defines.h"
#include "helper_functions.h"
#include "multi.h"

#define BattleScript_LinkBattleWonOrLost (u8*) 0x81D88CC
#define BattleScript_PayDayMoneyAndPickUpItems (u8*) 0x81D8803
#define BattleScript_LocalBattleLost (u8*) 0x81D8806
#define BattleScript_GotAwaySafely (u8*) 0x81D8916
#define BattleScript_RanAwayUsingMonAbility (u8*) 0x81D890F
#define BattleScript_SmokeBallEscape (u8*) 0x81D8901

void HandleEndTurn_BattleWon(void);
void HandleEndTurn_BattleLost(void);
void HandleEndTurn_RanFromBattle(void);
void EndOfBattleThings(void);
void RestoreNonConsumableItems(void);
void RecalcAllStats(void);
void BringBackTheDead(void);
void EndPartnerBattlePartyRestore(void);
void EndSkyBattlePartyRestore(void);
void EndBattleFlagClear(void);
bool8 IsConsumable(u16 item);

extern void FormsRevert(pokemon_t* party);
extern void MegaRevert(void);
extern void UpdateBurmy(void);

extern u8 BattleScript_Victory[];
extern u8 BattleScript_PrintPlayerForfeited[];
extern u8 BattleScript_PrintPlayerForfeitedLinkBattle[];
extern u8 BattleScript_LostMultiBattleTower[];
extern u8 BattleScript_LostBattleTower[];

extern u8 ConsumableItemEffectTable[];

void HandleEndTurn_BattleWon(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        //gSpecialVar_Result = gBattleOutcome;
        gBattleTextBuff1[0] = gBattleOutcome;
        gBankAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
          && gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_TRAINER_TOWER))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_Victory;

        PlayBGM(BGM_VICTORY_TRAINER_BATTLE);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_Victory;
		
		u16 id = gTrainerBattleOpponent_A;
		u8 specialMus = FALSE;
		u8 loop = FALSE;
	
	VICTORY_MUSIC_SELECTION:
        switch (gTrainers[id].trainerClass) {
		#ifndef UNBOUND //Change this part
			case TRAINER_CLASS_LEADER:
			case TRAINER_CLASS_ELITE_4:
			case TRAINER_CLASS_CHAMPION:
				PlayBGM(BGM_VICTORY_SPECIAL);
				specialMus = TRUE;
				break;
			default:
				PlayBGM(BGM_VICTORY_TRAINER_BATTLE);
				break;
		
		#else //For Pokemon Unbound
			case TRAINER_CLASS_CHAMPION:
				PlayBGM(BGM_VICTORY_CHAMPION);
				specialMus = TRUE;
				break;
			case TRAINER_CLASS_ELITE_4:
				PlayBGM(BGM_VICYORY_ELITE_4);
				specialMus = TRUE;
				break;
			case TRAINER_CLASS_LEADER:
				PlayBGM(BGM_VICYORY_GYM);
				specialMus = TRUE;
				break;
			case TRAINER_CLASS_SHADOW_ADMIN: //0x30
			case TRAINER_CLASS_BOSS: //0x53
			case TRAINER_CLASS_SHADOW: //0x55
			#ifndef DEBUG_UNBOUND_MUSIC
				PlayBGM(BGM_VICYORY_PLASMA);
				specialMus = TRUE;
				break;
			#endif
			case TRAINER_CLASS_LOR_LEADER: //0x2
			case TRAINER_CLASS_LOR_ADMIN: //0x2E
			case TRAINER_CLASS_LOR: //0x2F
				PlayBGM(BGM_VICYORY_GALACTIC);
				specialMus = TRUE;
				break;
			default:
				PlayBGM(BGM_VICTORY_TRAINER_BATTLE);
				break;
		#endif
        }
		
		if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !specialMus && !loop) {
			id = VarGet(SECOND_OPPONENT_VAR);
			loop = TRUE;
			goto VICTORY_MUSIC_SELECTION;
		}
    }
    else
        gBattlescriptCurrInstr = BattleScript_PayDayMoneyAndPickUpItems;

    gBattleMainFunc = (u32) HandleEndTurn_FinishBattle;
}

void HandleEndTurn_BattleLost(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        {
            if (gBattleOutcome & B_OUTCOME_LINK_BATTLE_RAN)
            {
                gBattlescriptCurrInstr = BattleScript_PrintPlayerForfeitedLinkBattle;
                gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
            }
            else
            {
                gBattlescriptCurrInstr = BattleScript_LostMultiBattleTower;
                gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
            }
        }
        else
        {
            gBattleTextBuff1[0] = gBattleOutcome;
            gBankAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
            gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
        }
    }
	else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
	{
		if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
			gBattlescriptCurrInstr = BattleScript_LostMultiBattleTower;
		else
			gBattlescriptCurrInstr = BattleScript_LostBattleTower;
			
        gBattleOutcome &= ~(B_OUTCOME_RAN);
	}
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && sTrainerBattleMode == TRAINER_BATTLE_OAK_TUTORIAL)
	{
		if (sTrainerBattleOakTutorialHelper & 1)
			gBattleCommunication[MULTISTRING_CHOOSER] = 1;
		else
			gBattleCommunication[MULTISTRING_CHOOSER] = 2;
		
		gBankAttacker = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
		gBattlescriptCurrInstr = BattleScript_LocalBattleLost;
	}	
    else 
	{
		gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        gBattlescriptCurrInstr = BattleScript_LocalBattleLost;
    }

    gBattleMainFunc = (u32) HandleEndTurn_FinishBattle;
}

void HandleEndTurn_RanFromBattle(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER && gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        gBattlescriptCurrInstr = BattleScript_PrintPlayerForfeited;
        gBattleOutcome = B_OUTCOME_FORFEITED;
    }
    else
    {
        switch (gProtectStructs[gBankAttacker].fleeFlag) {
			case 1:
				gBattlescriptCurrInstr = BattleScript_SmokeBallEscape; //0x81D8901
				break;
			case 2:
				gBattlescriptCurrInstr = BattleScript_RanAwayUsingMonAbility; //0x81D890F
				break;
			default:
				gBattlescriptCurrInstr = BattleScript_GotAwaySafely; //0x81D8916
				break;
        }
    }

    gBattleMainFunc = (u32) HandleEndTurn_FinishBattle;
}

u8 IsRunningFromBattleImpossible(void)
{
    u8 itemEffect;
    u8 side;
    int i;

    itemEffect = ITEM_EFFECT(gActiveBattler);
    gStringBank = gActiveBattler;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER))
        return 0;
		
	else if (FlagGet(NO_RUNNING_FLAG) || FlagGet(NO_CATCHING_AND_RUNNING_FLAG))
		return 1;
		
    else if (itemEffect == ITEM_EFFECT_CAN_ALWAYS_RUN)
        return 0;
    else if (gBattleMons[gActiveBattler].ability == ABILITY_RUNAWAY)
        return 0;
	else if (IsOfType(gActiveBattler, TYPE_GHOST))
		return 0;

    side = SIDE(gActiveBattler);

    for (i = 0; i < gBattlersCount; i++)
    {
        if (ABILITY(gActiveBattler) != ABILITY_SHADOWTAG //Shadow Tag's not affected by Shadow Tag
		&& side != SIDE(i)
        && ABILITY(i) == ABILITY_SHADOWTAG)
        {
            gBattleScripting->bank = i;
            gLastUsedAbility = ABILITY(i);
            gBattleCommunication[MULTISTRING_CHOOSER] = 2;
            return 2;
        }
        if (side != SIDE(i)
        && ABILITY(i) == ABILITY_ARENATRAP
		&& !CheckGrounding(gActiveBattler))
        {
            gBattleScripting->bank = i;
            gLastUsedAbility = ABILITY(i);
            gBattleCommunication[MULTISTRING_CHOOSER] = 2;
            return 2;
        }
		
		if (i != gActiveBattler
		&& ABILITY(i) == ABILITY_MAGNETPULL
		&& IsOfType(gActiveBattler, TYPE_STEEL))
		{
			gBattleScripting->bank = i;
			gLastUsedAbility = ABILITY(i);
			gBattleCommunication[MULTISTRING_CHOOSER] = 2;
			return 2;
		}
    }
	
    if ((gBattleMons[gActiveBattler].status2 & (STATUS2_ESCAPE_PREVENTION | STATUS2_WRAPPED))
    || (gStatuses3[gActiveBattler] & (STATUS3_ROOTED | STATUS3_SKY_DROP_TARGET)))
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        return 1;
    }
	
	if (gNewBS->FairyLockTimer)
	{
		gBattleCommunication[MULTISTRING_CHOOSER] = 0;
		return 1;
	}
		
    if (gBattleTypeFlags & BATTLE_TYPE_OAK_TUTORIAL)
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
        return 1;
    }
	
    return 0;
}

bool8 TryRunFromBattle(u8 bank)
{
    bool8 effect = FALSE;
    u8 itemEffect;
    u8 speedVar;

    itemEffect = ITEM_EFFECT(bank);
    gStringBank = bank;

	if (IsOfType(bank, TYPE_GHOST))
	{
		++effect;
	}
    else if (itemEffect == ITEM_EFFECT_CAN_ALWAYS_RUN)
    {
        gLastUsedItem = ITEM(bank);
        gProtectStructs[bank].fleeFlag = 1;
        ++effect;
    }
    else if (ABILITY(bank) == ABILITY_RUNAWAY)
    {
        gLastUsedAbility = ABILITY_RUNAWAY;
        gProtectStructs[bank].fleeFlag = 2;
        ++effect;
    }
	#ifdef GHOST_BATTLES
	else if ((gBattleTypeFlags & (BATTLE_TYPE_SCRIPTED_WILD_1 | BATTLE_TYPE_GHOST)) == BATTLE_TYPE_GHOST) 
	{
		if (SIDE(bank) == B_SIDE_PLAYER)
			++effect;
	}
	#endif
    else if (gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_TOWER) && gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        ++effect;
    }
    else
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
		SINGLE_FLEE_CALC:
            if (gBattleMons[bank].speed < gBattleMons[BATTLE_OPPOSITE(bank)].speed)
            {
                speedVar = udivsi((gBattleMons[bank].speed * 128), (gBattleMons[BATTLE_OPPOSITE(bank)].speed)) + (gBattleStruct->runTries * 30);
                if (speedVar > (Random() & 0xFF))
                    ++effect;
            }
            else // same speed or faster
            {
                ++effect;
            }
        }
		else //Wild Double
		{
			if (gBattleMons[BATTLE_OPPOSITE(bank)].hp)
				goto SINGLE_FLEE_CALC;
			
            if (gBattleMons[bank].speed < gBattleMons[PARTNER(BATTLE_OPPOSITE(bank))].speed)
            {
                speedVar = udivsi((gBattleMons[bank].speed * 128), (gBattleMons[PARTNER(BATTLE_OPPOSITE(bank))].speed)) + (gBattleStruct->runTries * 30);
                if (speedVar > (Random() & 0xFF))
                    ++effect;
            }
            else // same speed or faster
            {
                ++effect;
            }		
		}

        gBattleStruct->runTries++;
    }

    if (effect)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        gBattleOutcome = B_OUTCOME_RAN;
    }

    return effect;
}

void EndOfBattleThings(void) {
	RestoreNonConsumableItems();
	FormsRevert(gPlayerParty);
	MegaRevert();
	UpdateBurmy();
	EndPartnerBattlePartyRestore();
	EndSkyBattlePartyRestore();
	RecalcAllStats();
	BringBackTheDead();
	EndBattleFlagClear();
	TerrainType = 0; //Reset now b/c normal reset is after BG is loaded
}

void RestoreNonConsumableItems(void) {
	u16* items = ExtensionState.itemBackup;
	
	if (ExtensionState.itemBackup != NULL) {
		if (gBattleTypeFlags & BATTLE_TYPE_TRAINER) {
			for (int i = 0; i < PARTY_SIZE; ++i) 
			{
				if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER
				||  items[i] == 0
				||  !IsConsumable(items[i]))
					gPlayerParty[i].item = items[i];
				
				else if (gPlayerParty[i].item != items[i] //The player consumed their item, and then picked up another one
				&& IsConsumable(items[i]))
					gPlayerParty[i].item = 0; //They don't get to keep the new item
			}
		}
		Free(items);
	}
}

void RecalcAllStats(void) {
	for (int i = 0; i < PARTY_SIZE; ++i) 
		CalculateMonStats(&gPlayerParty[i]);
}

void BringBackTheDead(void) { //Used after Multi Battles that you lost, but your partner won
	if (ViableMonCount(gPlayerParty) == 0) {
		for (int i = 0; i < PARTY_SIZE; ++i) {
			if (gPlayerParty[i].species != 0 && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG, 0)) {
				gPlayerParty[i].hp = 1;
				break;
			}
		}
	}
}

void EndPartnerBattlePartyRestore(void) {
	int i;
	u8 counter = 0;
	pokemon_t* backup = ExtensionState.partyBackup;
	
	if (ExtensionState.partyBackup != NULL) {
		if (gSelectedOrderFromParty[0] == 0) { //Special 0x2F was not used
			for (i = 0; i < PARTY_SIZE; ++i) {
				if (gPlayerParty[i].species == 0)
					Memcpy(&gPlayerParty[i], &backup[counter++], sizeof(struct Pokemon));
			}
		}
		
		else { //Special 0x2F was used
			pokemon_t* foughtMons = Calloc(sizeof(struct Pokemon) * 3);
			if (foughtMons != NULL) {
				Memcpy(foughtMons, gPlayerParty, sizeof(struct Pokemon) * 3);
				Memset(gPlayerParty, 0x0, sizeof(struct Pokemon) * 6);
				for (i = 0; i < 3; ++i) {
					if (gSelectedOrderFromParty[i] != 0)
						Memcpy(&gPlayerParty[gSelectedOrderFromParty[i] - 1], &foughtMons[i], sizeof(struct Pokemon));
				}
			}
			for (i = 0; i < PARTY_SIZE; ++i) {
				if (gPlayerParty[i].species == 0)
					Memcpy(&gPlayerParty[i], &backup[counter++], sizeof(struct Pokemon));
			}
		}	
		Free(backup);
	}
}


//TO DO, restore party order like above
void EndSkyBattlePartyRestore(void) {
	int i;
	u8 counter = 0;
	pokemon_t* backup = ExtensionState.skyBattlePartyBackup;
	
	if (ExtensionState.skyBattlePartyBackup != NULL) {
		for (i = 0; i < PARTY_SIZE; ++i) {
			if (gPlayerParty[i].species == 0)
				Memcpy(&gPlayerParty[i], &backup[counter++], sizeof(struct Pokemon));
		}
		Free(backup);
	}
}

void EndBattleFlagClear(void) {
	FlagClear(NO_CATCHING_FLAG);
	FlagClear(NO_RUNNING_FLAG);
	FlagClear(NO_CATCHING_AND_RUNNING_FLAG);
	FlagClear(WILD_CUSTOM_MOVES_FLAG);
	FlagClear(TAG_BATTLE_FLAG);
	FlagClear(TWO_OPPONENT_FLAG);
	FlagClear(SMART_WILD_FLAG);
	VarSet(TERRAIN_VAR, 0x0);
	VarSet(BATTLE_TOWER_TRAINER_NAME, 0xFFFF);
	Free(gNewBS->MegaData);
	Free(gNewBS->UltraData);
	Free(gNewBS->ZMoveData);
	Free(gNewBS);
	Memset(&ExtensionState, 0x0, sizeof(struct BattleExtensionState));
}

bool8 IsConsumable(u16 item) {
	u8 effect = gItems[SanitizeItemId(item)].holdEffect;
	
	for (u32 i = 0; ConsumableItemEffectTable[i] != 0xFF; ++i) {
		if (effect == ConsumableItemEffectTable[i])
			return TRUE;
	}
	
	return FALSE;
}