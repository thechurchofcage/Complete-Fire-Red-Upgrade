#pragma once

#include "defines.h"

#define MAKE_POKEMON(structure)																												\
{																																			\
	for (j = 0; gSpeciesNames[structure[i].species][j] != EOS; ++j)																			\
		nameHash += gSpeciesNames[structure[i].species][j];																					\
																																			\
	personalityValue += nameHash << 8;																										\
																																			\
	u8 lvl = structure[i].lvl;																												\
	if (FlagGet(SCALE_TRAINER_LEVELS_FLAG)																									\
	|| (gBattleTypeFlags & BATTLE_TYPE_TRAINER_TOWER))																						\
		lvl = GetHighestMonLevel(gPlayerParty);																								\
																																			\
	CreateMon(&party[i], structure[i].species, lvl, STANDARD_IV, TRUE, personalityValue, OT_ID_PRESET, otid);								\
	party[i].metLevel = structure[i].lvl;																									\
}																																

#define SET_MOVES(structure)													\
{																	\
	for (j = 0; j < MAX_MON_MOVES; j++) {							\
		party[i].moves[j] = structure[i].moves[j];					\
		party[i].pp[j] = gBattleMoves[structure[i].moves[j]].pp;	\
	}																\
}

#define SET_IVS_SINGLE_VALUE(val)					\
{													\
    party[i].hpIV = val;							\
	party[i].attackIV = val;						\
	party[i].defenseIV = val;						\
	party[i].speedIV = val;							\
	party[i].spAttackIV = val;						\
	party[i].spDefenseIV = val;						\
}

#define SET_IVS(structure)							\
{													\
    party[i].hpIV = structure.hpIv;					\
	party[i].attackIV = structure.atkIv;			\
	party[i].defenseIV = structure.defIv;			\
	party[i].speedIV = structure.spdIv;				\
	party[i].spAttackIV = structure.spAtkIv;		\
	party[i].spDefenseIV = structure.spDefIv;		\
}

#define SET_EVS(structure)							\
{													\
    party[i].hpEv = structure.hpEv;					\
	party[i].atkEv = structure.atkEv;				\
	party[i].defEv = structure.defEv;				\
	party[i].spdEv = structure.spdEv;				\
	party[i].spAtkEv = structure.spAtkEv;			\
	party[i].spDefEv = structure.spDefEv;			\
}

#define LOAD_TIER_CHECKING_ABILITY										\
{																		\
	if (spread->ability == 0 && gBaseStats[species].hiddenAbility != 0)	\
		ability = gBaseStats[species].hiddenAbility;					\
	else if (spread->ability == 2 && gBaseStats[species].ability2 != 0)	\
		ability = gBaseStats[species].ability2;							\
	else																\
		ability = gBaseStats[species].ability1;							\
}

#define TRAINERS_WITH_EVS_TABLE_SIZE 2

struct TrainersWithEvs {
    u8 nature;
	u8 _1;
	u8 _2;
	u8 _3;
    u8 ivs;
    u8 hpEv;
	u8 atkEv;
	u8 defEv;
	u8 spdEv;
	u8 spAtkEv;
	u8 spDefEv;
	u8 ball; //0xFF = Random Ball
	u8 ability; //0 = Hidden, 1 = Ability_1, 2 = Ability_2
	u8 _4;
    u16 _5;         // 0x0000
};

extern struct TrainersWithEvs TrainersWithEvsTable[];

