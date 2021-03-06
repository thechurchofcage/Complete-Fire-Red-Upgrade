#include "defines.h"
#include "helper_functions.h"

//Change this to account for new base Exp
//Test battle continuing after Wild Poke kills itself with Explosion

#define SE_EXP 0x1B
#define MUS_WILD_POKE_VICTORY 0x137
#define BattleScript_LevelUp (u8*) 0x81D89F5

enum
{
GetExp_Start,
GetExp_CheckCurrentMonDeserving,
GetExp_Calculation,
GetExp_Distribute,
GetExp_SetStats,
GetExp_LevelUp,
GetExp_PrepareLoop,
GetExp_End,
};

enum
{
GiveExpBattlePariticpants,
GiveExpViaExpShare,
};

extern u8 String_TeamExpGain[];

extern void PrepareStringBattle(u16 stringId, u8 bank);

bool8 CouldHaveEvolved(pokemon_t* mon);
u32 ExpCalculator(u32 a, u32 t, u32 b, u32 e, u32 L, u32 Lp, u32 p, u32 f, u32 v, u32 s);
bool8 WasWholeTeamSentIn(u8 bank, u8 sentIn);
void EmitExpBarUpdate(u8 a, u8 b, u32 c);
void EmitExpTransferBack(u8 bufferId, u8 b, u8 *c);
void PlayerHandleExpBarUpdate(void);
void Task_GiveExpToMon(u8 taskId);
void Task_PrepareToGiveExpWithExpBar(u8 taskId);
void sub_80300F4(u8 taskId);
u32 GetExpToLevel(u8 toLevel, u8 growthRate);

void atk23_getexp(void) {
    u16 item = gPlayerParty[gBattleStruct->expGetterId].item;
	u8 holdEffect = gItems[SanitizeItemId(item)].holdEffect;
    u32 i; // also used as stringId
    u32 sentIn;
    u32 viaExpShare = 0, viaSentIn = 0;
	u32 calculatedExp = 0;
	
    u16* expGiveType = &gBattleStruct->expValue;

    gBankFainted = GetBattleBank(gBattlescriptCurrInstr[1]);
    sentIn = gSentPokesToOpponent[(gBankFainted & 2) >> 1]; //sentIn = 0 if Bank 1 killed, sentIn = 1 if Bank 3 killed

    switch (gBattleScripting->expStateTracker)
    {
    case GetExp_Start: // check if should receive exp at all
		
        if (GetBattlerSide(gBankFainted) != B_SIDE_OPPONENT 
		|| gBattleStruct->givenExpMons & gBitTable[gBattlerPartyIndexes[gBankFainted]]
		|| (gBattleTypeFlags &
             (BATTLE_TYPE_LINK
              | BATTLE_TYPE_SAFARI
              | BATTLE_TYPE_EREADER_TRAINER
			  | BATTLE_TYPE_FRONTIER
			  | BATTLE_TYPE_TRAINER_TOWER)))
        {
            gBattleScripting->expStateTracker = GetExp_End; // goto last case
        }
        else
        {
            gBattleScripting->expStateTracker++;
            gBattleStruct->givenExpMons |= gBitTable[gBattlerPartyIndexes[gBankFainted]]; //The party indices in the opponent's party that have fainted and been given exp for
			gBattleStruct->expGetterId = 0;
			gBattleStruct->sentInPokes = sentIn;
			gNewBS->SentInBackup = sentIn;
			gBattleMoveDamage = 0;
			*expGiveType = GiveExpBattlePariticpants; //Start with battle participants
        }
        break;
	
    case GetExp_CheckCurrentMonDeserving: //Check if Current mon deserves EXP
		if (gPlayerParty[gBattleStruct->expGetterId].level >= MAX_LEVEL
		||  gPlayerParty[gBattleStruct->expGetterId].hp == 0
		||  GetMonData(&gPlayerParty[gBattleStruct->expGetterId], MON_DATA_IS_EGG, 0))
        {
            gBattleStruct->sentInPokes >>= 1; //One less pokemon to distribute exp to
            gBattleScripting->expStateTracker = GetExp_PrepareLoop;
            gBattleMoveDamage = 0; // used for exp
			break;
        }
		
		#ifdef OLD_EXP_SHARE
		else if (holdEffect != ITEM_EFFECT_EXP_SHARE && !(gBattleStruct->sentInPokes & 1))
		{
			gBattleStruct->sentInPokes >>= 1;
			gBattleScripting->expStateTracker = GetExp_PrepareLoop;
			gBattleMoveDamage = 0; // used for exp
			break;
		}
		#else //New Exp Share
		else if (*expGiveType == GiveExpBattlePariticpants && !(gBattleStruct->sentInPokes & 1)) //Exp Share runs on a different loop
		{
			gBattleStruct->sentInPokes >>= 1;
			gBattleScripting->expStateTracker = GetExp_PrepareLoop;
			gBattleMoveDamage = 0; // used for exp
			break;
		}
		else if (*expGiveType == GiveExpViaExpShare && gBattleStruct->sentInPokes & 1)
		{
			gBattleStruct->sentInPokes >>= 1;
			gBattleScripting->expStateTracker = GetExp_PrepareLoop;
			gBattleMoveDamage = 0; // used for exp
			break;
		}
		#endif
		
		gBattleScripting->expStateTracker++;
	__attribute__ ((fallthrough));
		
    case GetExp_Calculation:	; // calculate experience points to redistribute
		u32 trainerBonus, tradeBonus, baseExp, eggBoost, defLevel, pokeLevel, passPower, affection, evolutionBoost, divisor;
		
        for (viaSentIn = 0, i = 0; i < 6; i++)
        {
			if (gPlayerParty[i].species == PKMN_NONE || gPlayerParty[i].hp == 0)
				continue;
			
			if (gBitTable[i] & sentIn)
				viaSentIn++;
			#ifndef OLD_EXP_SHARE //In new gens, you either get bonus for being in, or exp share
			else if (FlagGet(EXP_SHARE_FLAG))
				viaExpShare++;
			#endif
			
			#ifdef OLD_EXP_SHARE
			if (gItems[SanitizeItemId(gPlayerParty[i].item)].holdEffect == ITEM_EFFECT_EXP_SHARE)
					viaExpShare++;
			#endif
        }

		//Trainer Boost - a
		#ifdef TRAINER_EXP_BOOST
			trainerBonus = 10;
			if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
				trainerBonus = 15;
		#else
			trainerBonus = 10;
		#endif
			
		//Trade Boost - t
		tradeBonus = 10;
        if (IsTradedMon(&gPlayerParty[gBattleStruct->expGetterId]))
			tradeBonus = 15;
			
		//Base Experience - b
		baseExp = gBaseStats[gBattleMons[gBankFainted].species].expYield; 
			
		//Lucky Egg Boost - e
		eggBoost = 10;
		if (holdEffect == ITEM_EFFECT_LUCKY_EGG)
			eggBoost = 15;
			
		//Level of Fainted Mon
		defLevel = gBattleMons[gBankFainted].level;
			
        //Level of Victorious Mon
		pokeLevel = gPlayerParty[gBattleStruct->expGetterId].level;
			
		//Pass Power Bonus - Not implemented
		passPower = 1;
			
		//Affection Boost - Not implemented
		affection = 1;
			
		//Evolution Boost
		evolutionBoost = 10;
		if (CouldHaveEvolvedViaLevelUp(&gPlayerParty[gBattleStruct->expGetterId]))
			evolutionBoost = 12;
			
		//Exp Share/Num Battlers Divisor
		#ifdef OLD_EXP_SHARE
			#ifdef OLD_EXP_SPLIT
				if (viaExpShare) { // at least one mon is getting exp via exp share
					if (holdEffect == ITEM_EFFECT_EXP_SHARE)
						calculatedExp += ExpCalculator(trainerBonus, tradeBonus, baseExp, eggBoost, defLevel, pokeLevel, passPower, affection, evolutionBoost, 2 * viaExpShare);
					if (gNewBS->SentInBackup & (1 << gBattleStruct->expGetterId))
						calculatedExp += ExpCalculator(trainerBonus, tradeBonus, baseExp, eggBoost, defLevel, pokeLevel, passPower, affection, evolutionBoost, 2 * viaSentIn);
					goto SKIP_EXP_CALC;
				}
				else //No Pokemon holds Exp Share
					divisor = viaSentIn;
			#else
				divisor = 1;
			#endif
		#else //New Exp. Share
			#ifdef OLD_EXP_SPLIT
				if (*expGiveType == GiveExpBattlePariticpants) //In battle recieves full EXP / num battlers
					divisor = viaSentIn;
				else //Otherwise Exp / 2
					divisor = 2 * viaSentIn;
			#else
				if (*expGiveType == GiveExpBattlePariticpants) //In battle recieves full EXP
					divisor = 1;
				else //Otherwise Exp / 2
					divisor = 2;
			#endif
		#endif
		
		calculatedExp = ExpCalculator(trainerBonus, tradeBonus, baseExp, eggBoost, defLevel, pokeLevel, passPower, affection, evolutionBoost, divisor);
		goto SKIP_EXP_CALC; //Only here so the compiler stops giving potential unused label errors (because it can be depending on your configuration)
		
	SKIP_EXP_CALC:	
		calculatedExp = MathMax(1, calculatedExp);
		gBattleMoveDamage = calculatedExp;
		
		gBattleScripting->expStateTracker++;
	__attribute__ ((fallthrough));
		
    case GetExp_Distribute: // set exp value to the poke in expgetter_id and print message
	
        // music change in wild battle after fainting a poke
        if (!(gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_POKE_DUDE)) && gBattleMons[0].hp && !gBattleStruct->wildVictorySong)
        {
            BattleStopLowHpSound();
            PlayBGM(MUS_WILD_POKE_VICTORY); //Wild PKMN Victory
            gBattleStruct->wildVictorySong++;
			gAbsentBattlerFlags |= gBitTable[gBankFainted];
        }

        if (gPlayerParty[gBattleStruct->expGetterId].hp)
        {
		
			//Check Boosted String necessary
            if (IsTradedMon(&gPlayerParty[gBattleStruct->expGetterId])
			||  CouldHaveEvolvedViaLevelUp(&gPlayerParty[gBattleStruct->expGetterId])
			||  holdEffect == ITEM_EFFECT_LUCKY_EGG
	      /*|| [gBattleStruct->expGetterId]->affection >= 2*/) //There's no affection boost in Vanilla FR. If you want it, you'll have to implement it yourself.
            {
                // check if the pokemon doesn't belong to the player
                if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && gBattleStruct->expGetterId >= 3)
                    i = STRINGID_EMPTYSTRING4;
                else
                    i = STRINGID_ABOOSTED;
            }
            else
                i = STRINGID_EMPTYSTRING4;

            // get exp getter bank
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            {
                if (gBattlerPartyIndexes[B_POSITION_PLAYER_RIGHT] == gBattleStruct->expGetterId && !(gAbsentBattlerFlags & gBitTable[B_POSITION_PLAYER_RIGHT]))
                    gBattleStruct->expGetterBank = B_POSITION_PLAYER_RIGHT;
                else
                {
                    if (!(gAbsentBattlerFlags & gBitTable[B_POSITION_PLAYER_LEFT]))
                        gBattleStruct->expGetterBank = B_POSITION_PLAYER_LEFT;
                    else
                        gBattleStruct->expGetterBank = B_POSITION_PLAYER_RIGHT;
                }
            }
            else
                gBattleStruct->expGetterBank = B_POSITION_PLAYER_LEFT;

            PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gBattleStruct->expGetterBank, gBattleStruct->expGetterId)

			// buffer 'gained' or 'gained a boosted'
			PREPARE_STRING_BUFFER(gBattleTextBuff2, i)

            PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff3, 10, calculatedExp)
			
			#ifndef OLD_EXP_SHARE
				if (*expGiveType == GiveExpBattlePariticpants) //Don't print the gained exp string unless it participated in battle
					PrepareStringBattle(STRINGID_PKMNGAINEDEXP, gBattleStruct->expGetterBank);
			#else
				PrepareStringBattle(STRINGID_PKMNGAINEDEXP, gBattleStruct->expGetterBank);
			#endif
            MonGainEVs(&gPlayerParty[gBattleStruct->expGetterId], gBattleMons[gBankFainted].species);
        }
		else 
		{
			gBattleStruct->sentInPokes >>= 1;
			gBattleScripting->expStateTracker = GetExp_PrepareLoop;
			gBattleMoveDamage = 0; // used for exp
			break;
		}
        gBattleStruct->sentInPokes >>= 1;
        gBattleScripting->expStateTracker++;
	__attribute__ ((fallthrough));
	
    case GetExp_SetStats: // Set stats and give exp
        if (gBattleExecBuffer) break;
		
        gBattleBufferB[gBattleStruct->expGetterBank][0] = 0;
        if (gPlayerParty[gBattleStruct->expGetterId].hp && gPlayerParty[gBattleStruct->expGetterId].level < MAX_LEVEL)
        {
            gBattleResources->statsBeforeLvlUp->hp = gPlayerParty[gBattleStruct->expGetterId].maxHP;
            gBattleResources->statsBeforeLvlUp->atk = gPlayerParty[gBattleStruct->expGetterId].attack;
            gBattleResources->statsBeforeLvlUp->def = gPlayerParty[gBattleStruct->expGetterId].defense;
            gBattleResources->statsBeforeLvlUp->spd = gPlayerParty[gBattleStruct->expGetterId].speed;
            gBattleResources->statsBeforeLvlUp->spAtk = gPlayerParty[gBattleStruct->expGetterId].spAttack;
            gBattleResources->statsBeforeLvlUp->spDef = gPlayerParty[gBattleStruct->expGetterId].spDefense;
                
            gActiveBattler = gBattleStruct->expGetterBank;
            EmitExpBarUpdate(0, gBattleStruct->expGetterId, gBattleMoveDamage);
            MarkBufferBankForExecution(gActiveBattler);
        }
		else 
		{
			gBattleScripting->expStateTracker = GetExp_PrepareLoop;
			gBattleMoveDamage = 0; // used for exp
			break;
		}
        gBattleScripting->expStateTracker++;
		break;
	
    case GetExp_LevelUp: // lvl up if necessary
        if (gBattleExecBuffer) break;
        
        gActiveBattler = gBattleStruct->expGetterBank;
        if (gBattleBufferB[gActiveBattler][0] == CONTROLLER_CHOSENMONRETURNVALUE
        &&  gBattleBufferB[gActiveBattler][1] == RET_VALUE_LEVELED_UP)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && gBattlerPartyIndexes[gActiveBattler] == gBattleStruct->expGetterId)
                HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);

            PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gActiveBattler, gBattleStruct->expGetterId)

            PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff2, 3, gPlayerParty[gBattleStruct->expGetterId].level)

            BattleScriptPushCursor();
            gLeveledUpInBattle |= gBitTable[gBattleStruct->expGetterId];
            gBattlescriptCurrInstr = BattleScript_LevelUp;       
			gBattleMoveDamage = (gBattleBufferB[gActiveBattler][2] | (gBattleBufferB[gActiveBattler][3] << 8) | (gBattleBufferB[gActiveBattler][4] << 0x10) | (gBattleBufferB[gActiveBattler][5] << 0x18));
 			AdjustFriendship(&gPlayerParty[gBattleStruct->expGetterId], FRIENDSHIP_EVENT_GROW_LEVEL);

            // update battle mon structure after level up
            if (gBattlerPartyIndexes[B_POSITION_PLAYER_LEFT] == gBattleStruct->expGetterId && gBattleMons[B_POSITION_PLAYER_LEFT].hp)
            {
                gBattleMons[B_POSITION_PLAYER_LEFT].level = gPlayerParty[gBattleStruct->expGetterId].level;
                gBattleMons[B_POSITION_PLAYER_LEFT].hp = gPlayerParty[gBattleStruct->expGetterId].hp;
                gBattleMons[B_POSITION_PLAYER_LEFT].maxHP = gPlayerParty[gBattleStruct->expGetterId].maxHP;
                gBattleMons[B_POSITION_PLAYER_LEFT].attack = gPlayerParty[gBattleStruct->expGetterId].attack;
                gBattleMons[B_POSITION_PLAYER_LEFT].defense = gPlayerParty[gBattleStruct->expGetterId].defense;
                gBattleMons[B_POSITION_PLAYER_LEFT].speed = gPlayerParty[gBattleStruct->expGetterId].speed;
                gBattleMons[B_POSITION_PLAYER_LEFT].spAttack = gPlayerParty[gBattleStruct->expGetterId].spAttack;
                gBattleMons[B_POSITION_PLAYER_LEFT].spDefense = gPlayerParty[gBattleStruct->expGetterId].spDefense;
            }
            else if (gBattlerPartyIndexes[B_POSITION_PLAYER_RIGHT] == gBattleStruct->expGetterId && gBattleMons[B_POSITION_PLAYER_RIGHT].hp && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            {
                gBattleMons[B_POSITION_PLAYER_RIGHT].level = gPlayerParty[gBattleStruct->expGetterId].level;
                gBattleMons[B_POSITION_PLAYER_RIGHT].hp = gPlayerParty[gBattleStruct->expGetterId].hp;
                gBattleMons[B_POSITION_PLAYER_RIGHT].maxHP = gPlayerParty[gBattleStruct->expGetterId].maxHP;
                gBattleMons[B_POSITION_PLAYER_RIGHT].attack = gPlayerParty[gBattleStruct->expGetterId].attack;
                gBattleMons[B_POSITION_PLAYER_RIGHT].defense = gPlayerParty[gBattleStruct->expGetterId].defense;
                gBattleMons[B_POSITION_PLAYER_RIGHT].speed = gPlayerParty[gBattleStruct->expGetterId].speed;
                gBattleMons[B_POSITION_PLAYER_RIGHT].spAttack = gPlayerParty[gBattleStruct->expGetterId].spAttack;
				gBattleMons[B_POSITION_PLAYER_RIGHT].spDefense = gPlayerParty[gBattleStruct->expGetterId].spDefense;
            }
        }
        else
            gBattleMoveDamage = 0;

        gBattleScripting->expStateTracker++;
		break;

    case GetExp_PrepareLoop: // looper increment
        if (gBattleMoveDamage) // there is exp to give, goto case 3 that gives exp
            gBattleScripting->expStateTracker = GetExp_SetStats;
        else
        {
            gBattleStruct->expGetterId++;
			if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) {
				if (gBattleStruct->expGetterId < 3)
					gBattleScripting->expStateTracker = GetExp_CheckCurrentMonDeserving; // loop again
				else
					gBattleScripting->expStateTracker = GetExp_End; // we're done
			}
			else {
				if (gBattleStruct->expGetterId < 6)
					gBattleScripting->expStateTracker = GetExp_CheckCurrentMonDeserving; // loop again
				else
					gBattleScripting->expStateTracker = GetExp_End; // we're done
			}
        }
        break;

    case GetExp_End: // increment instruction
        if (gBattleExecBuffer) break;
		
		#ifndef OLD_EXP_SHARE
			if (FlagGet(EXP_SHARE_FLAG) && *expGiveType == GiveExpBattlePariticpants && !WasWholeTeamSentIn(B_POSITION_PLAYER_LEFT, gNewBS->SentInBackup)) {
				*expGiveType = GiveExpViaExpShare;
				gBattleStruct->expGetterId = 0;
				gBattleMoveDamage = 0;
				gBattleStruct->sentInPokes = gNewBS->SentInBackup;
				gBattleScripting->expStateTracker = GetExp_CheckCurrentMonDeserving; // Time for Exp Share loop
				BattleStringLoader = String_TeamExpGain;
				PrepareStringBattle(0x184, 0);
			}
			else 
			{
				gBattleMons[gBankFainted].item = 0;
				gBattleMons[gBankFainted].ability = 0;
				gBattlescriptCurrInstr += 2;
			}
		#else
			gBattleMons[gBankFainted].item = 0;
			gBattleMons[gBankFainted].ability = 0;
			gBattlescriptCurrInstr += 2;
		#endif
        break;
    }
}

u32 ExpCalculator(u32 a, u32 t, u32 b, u32 e, u32 L, u32 Lp, u32 p, u32 f, u32 v, u32 s) {
	u32 calculatedExp;
	
	#ifdef FLAT_EXP_FORMULA
		++Lp; //So the variable doesn't remain unused
		calculatedExp = udivsi(a * t * b * e * L * p * f * v, 10 * 10 * 10 * 10); //Did the calcs, shouldn't overflow (unless Base Exp > 1060)
		calculatedExp = udivsi(calculatedExp, 7 * s);
	
	#else //Scaled Formula Gens 5, 7
		u32 calculatedExpTop, calculatedExpBottom;
		
		calculatedExp = udivsi(a * b * L, 10 * 5 * s);
		
		calculatedExpTop = (2 * L + 10) * (2 * L + 10); //* Sqrt(2 * L + 10); //(2 * L + 10)**2.5
		calculatedExpBottom = (L + Lp + 10) * (L + Lp + 10) * Sqrt(L + Lp + 10); //(L + Lp + 10)**2.5
		
		calculatedExp *= calculatedExpTop;
		calculatedExp = udivsi(calculatedExp, calculatedExpBottom);
		calculatedExp *= Sqrt(2 * L + 10); //Best way to prevent overflow; now Base Exp < 3245 works
		
		calculatedExp += 1;
			
		calculatedExp = udivsi(calculatedExp * t * e * v, 10 * 10 * 10) * p * f;
	#endif
	return MathMin(1640000, calculatedExp);
}

bool8 WasWholeTeamSentIn(u8 bank, u8 sentIn) {
	u8 start, end;
	int i;
	
	pokemon_t* party = LoadPartyRange(bank, &start, &end);
	
	for (i = start; i < end; ++i) {
		if (party[i].species == 0)
			return TRUE;
		
		if (party[i].hp == 0 || GetMonData(&party[i], MON_DATA_IS_EGG, 0) || party[i].level >= MAX_LEVEL)
			continue;
	
		if (!(sentIn & (1 << i)))
			return FALSE;
	}
	
	return TRUE;
}


void EmitExpBarUpdate(u8 a, u8 b, u32 c) //Changed the u16 to a u32 to allow for more exp gain
{
    gBattleBuffersTransferData[0] = 25;
    gBattleBuffersTransferData[1] = b;
    gBattleBuffersTransferData[2] = c;
    gBattleBuffersTransferData[3] = (c & 0xFF00)     >> 0x08;
    gBattleBuffersTransferData[4] = (c & 0xFF0000)   >> 0x10;
    gBattleBuffersTransferData[5] = (c & 0xFF000000) >> 0x18;
    PrepareBufferDataTransfer(a, gBattleBuffersTransferData, 6);
}

void EmitExpTransferBack(u8 bufferId, u8 b, u8 *c)
{
    s32 i;

    gBattleBuffersTransferData[0] = CONTROLLER_CHOSENMONRETURNVALUE;
    gBattleBuffersTransferData[1] = b;
    for (i = 0; i < 4; i++)
        gBattleBuffersTransferData[2 + i] = c[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 6);
}

void PlayerHandleExpBarUpdate(void)
{
	
    u8 bankPartyIndex = gBattleBufferA[gActiveBattler][1];

    if (gPlayerParty[bankPartyIndex].level >= MAX_LEVEL)
    {
        PlayerBufferExecCompleted();
    }
    else
    {
        s32 gainedExp;
        u8 taskId;

        load_gfxc_health_bar(1);
        gainedExp = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 0x8) | (gBattleBufferA[gActiveBattler][4] << 0x10) | (gBattleBufferA[gActiveBattler][5] << 0x18);
		SeedHelper[0] = gainedExp;
		SeedHelper[1] = gainedExp >> 8;
		SeedHelper[2] = gainedExp >> 0x10;
		SeedHelper[3] = gainedExp >> 0x18;
        taskId = CreateTask(Task_GiveExpToMon, 10);
        gTasks[taskId].data[0] = bankPartyIndex;
        gTasks[taskId].data[1] = gainedExp;
        gTasks[taskId].data[2] = gActiveBattler;
        gBattleBankFunc[gActiveBattler] = (u32) nullsub_8;
    }
}

void Task_GiveExpToMon(u8 taskId)
{
    u32 pkmnIndex = (u8)gTasks[taskId].data[0];
    u8 bank = gTasks[taskId].data[2];
    s32 gainedExp = SeedHelper[0] | (SeedHelper[1] << 0x8) | (SeedHelper[2] << 0x10) | (SeedHelper[3] << 0x18);
	
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE || pkmnIndex != gBattlerPartyIndexes[bank])
    {
        pokemon_t* pkmn = &gPlayerParty[pkmnIndex];
        u16 species = pkmn->species;
        u8 level = pkmn->level;
        u32 currExp = pkmn->experience;
		u32 nextLvlExp = GetExpToLevel(level + 1, gBaseStats[species].growthRate);
		
        if (currExp + gainedExp >= nextLvlExp)
        {
            u8 savedActiveBank;

            pkmn->experience = nextLvlExp;
            CalculateMonStats(pkmn);
            gainedExp -= (nextLvlExp - currExp);
            savedActiveBank = gActiveBattler;
            gActiveBattler = bank;
            EmitExpTransferBack(1, RET_VALUE_LEVELED_UP, (u8*) (&gainedExp)); //Used to be EmitCmd33, but Cmd34 allows for more data transfer
            gActiveBattler = savedActiveBank;

            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
             && ((u8) pkmnIndex == gBattlerPartyIndexes[bank] || (u8) pkmnIndex == gBattlerPartyIndexes[bank ^ BIT_FLANK]))
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            else
                gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
        }
        else
        {
            currExp += gainedExp;
            pkmn->experience = currExp;
            gBattleBankFunc[bank] = (u32) CompleteOnInactiveTextPrinter;
            DestroyTask(taskId);
        }
    }
    else
        gTasks[taskId].func = Task_PrepareToGiveExpWithExpBar;
}

void Task_PrepareToGiveExpWithExpBar(u8 taskId)
{
    u8 pkmnIndex = gTasks[taskId].data[0];
    s32 gainedExp = SeedHelper[0] | (SeedHelper[1] << 0x8) | (SeedHelper[2] << 0x10) | (SeedHelper[3] << 0x18);
    u8 bank = gTasks[taskId].data[2];
    pokemon_t* pkmn = &gPlayerParty[pkmnIndex];
    u8 level = pkmn->level;
    u16 species = pkmn->species;
    u32 exp = pkmn->experience;
    u32 currLvlExp = GetExpToLevel(level, gBaseStats[species].growthRate);
    u32 expToNextLvl;
	
    exp -= currLvlExp;
    expToNextLvl = GetExpToLevel(level + 1, gBaseStats[species].growthRate) - currLvlExp;
    SetBattleBarStruct(bank, gHealthboxIDs[bank], expToNextLvl, exp, -gainedExp); //sub_8043D84 in Ruby
    PlaySE(SE_EXP);
    gTasks[taskId].func = sub_80300F4; //sub_802DB6C
}

void sub_80300F4(u8 taskId)
{
    if (gTasks[taskId].data[10] < 13)
        gTasks[taskId].data[10]++;

    else
    {
        u8 monId = gTasks[taskId].data[0];
        s32 gainedExp = SeedHelper[0] | (SeedHelper[1] << 0x8) | (SeedHelper[2] << 0x10) | (SeedHelper[3] << 0x18);
        u8 battlerId = gTasks[taskId].data[2];
        s32 newExpPoints;
		
        newExpPoints = MoveBattleBar(battlerId, gHealthboxIDs[battlerId], EXP_BAR, 0);
        SetHealthboxSpriteVisible(gHealthboxIDs[battlerId]);
        if (newExpPoints == -1)
        {
            u8 level;
            u32 currExp;
            u16 species;
            u32 expOnNextLvl;

            m4aSongNumStop(SE_EXP);
            level = gPlayerParty[monId].level;
            currExp = gPlayerParty[monId].experience;
            species = gPlayerParty[monId].species;
            expOnNextLvl = GetExpToLevel(level + 1, gBaseStats[species].growthRate);
			
            if (currExp + gainedExp >= expOnNextLvl)
            {
                u8 savedActiveBattler;

                gPlayerParty[monId].experience = expOnNextLvl;
                CalculateMonStats(&gPlayerParty[monId]);
                gainedExp -= (expOnNextLvl - currExp);
                savedActiveBattler = gActiveBattler;
                gActiveBattler = battlerId;
                EmitExpTransferBack(1, RET_VALUE_LEVELED_UP, (u8*) &gainedExp);
                gActiveBattler = savedActiveBattler;
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            }
            else
            {
                currExp += gainedExp;
                gPlayerParty[monId].experience = currExp;
                gBattleBankFunc[battlerId] = (u32) CompleteOnInactiveTextPrinter;
                DestroyTask(taskId);
            }
        }
    }
}

u32 GetExpToLevel(u8 toLevel, u8 growthRate) {
	u32 required;
	
	if (toLevel <= 100)
		return gExperienceTables[growthRate][toLevel];

	switch (growthRate) {
		case GROWTH_MEDIUM_FAST:
			required = toLevel * toLevel * toLevel;
			break;
		
		case GROWTH_ERRATIC:
		//The original function was 160 - level, which can cause issues if the level if > 160
		//So the function was changed to be the maximum level + 60, similar to the original
			required = udivsi(((MAX_LEVEL + 60) - toLevel) * toLevel * toLevel, 100) * toLevel;
			break;	
		
		case GROWTH_FLUCTUATING:
			required = udivsi((toLevel / 2) + 32, 50) * toLevel * toLevel * toLevel;
			break;	
		
		case GROWTH_MEDIUM_SLOW:
			required = udivsi(6 * toLevel * toLevel * toLevel, 5) + 100 * toLevel - 15 * toLevel * toLevel - 140;
			break;
		
		case GROWTH_FAST:
			required = udivsi(4 * toLevel * toLevel * toLevel, 5);
			break;
		
		case GROWTH_SLOW:
			required = udivsi(5 * toLevel * toLevel * toLevel, 4);
			break;
		
		default:
			required = 1;
	}
	
	return required;
}
