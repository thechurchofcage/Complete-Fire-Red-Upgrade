#include "defines.h"
#include "helper_functions.h"
#include "battle_terrain.h"

extern const struct BattleBackground gAttackTerrainTable[];
extern const struct TerrainTableStruct TerrainTable[];

//This File's Functions
u8 BattleSetup_GetTerrainId(void);
u8 LoadBattleBG_TerrainID(void);
void LoadBattleBG_Background(u8 terrainId);
void LoadBattleBG_EntryOverlay(u8 terrainId);
bool8 MetatileBehavior_IsIce(u8 metatileBehavior);

u8 BattleSetup_GetTerrainId(void)
{
    u16 tileBehavior;
    s16 x, y;
	u8 terrain = BATTLE_TERRAIN_PLAIN;

    PlayerGetDestCoords(&x, &y);
    tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

	if (VarGet(BATTLE_BG_VAR))
		terrain = VarGet(BATTLE_BG_VAR);
	
	else
	{
		//Maybe add check for fishing byte?
		if (MetatileBehavior_IsTallGrass(tileBehavior))
			terrain = BATTLE_TERRAIN_GRASS;
		else if (tileBehavior == MB_VERY_TALL_GRASS) //The old useless function for this check just returns 0
			terrain = BATTLE_TERRAIN_LONG_GRASS;
		else if (MetatileBehavior_IsSandOrDeepSand(tileBehavior))
			terrain = BATTLE_TERRAIN_SAND;
		else if (MetatileBehavior_IsMountain(tileBehavior))
			terrain = BATTLE_TERRAIN_MOUNTAIN;
		
		#ifdef BRIDGE_FIX
			else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
			{
				if (MetatileBehavior_IsDeepOrOceanWater(tileBehavior))
					terrain = BATTLE_TERRAIN_WATER;
				else if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior))
					terrain = BATTLE_TERRAIN_POND;
			}
		#else
			else if (MetatileBehavior_IsDeepOrOceanWater(tileBehavior))
				terrain = BATTLE_TERRAIN_WATER;
			else if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior))
				terrain = BATTLE_TERRAIN_POND;
		#endif

		switch (gMapHeader.mapType) {
			case MAP_TYPE_UNDERGROUND:
				if (MetatileBehavior_IsIndoorEncounter(tileBehavior))
				{
					terrain = BATTLE_TERRAIN_INSIDE; 
					break;
				}
				
				#ifdef UNBOUND
					terrain = BATTLE_TERRAIN_CAVE;
				#else
					if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior))
						terrain = BATTLE_TERRAIN_POND;
					else
						terrain = BATTLE_TERRAIN_CAVE;
				#endif
				break;
				
			case MAP_TYPE_INDOOR:
			case MAP_TYPE_SECRET_BASE:
				terrain = BATTLE_TERRAIN_INSIDE;
				break;
				
			case MAP_TYPE_UNDERWATER:
				terrain = BATTLE_TERRAIN_UNDERWATER;
				break;
				
			case MAP_TYPE_6:
				if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior))
					terrain = BATTLE_TERRAIN_WATER;
				else
					terrain = BATTLE_TERRAIN_PLAIN;
		}
	}
		
	#ifdef UNBOUND //Loads special BG's if you're surfing
		if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
		{
			switch (terrain) {
				case BATTLE_TERRAIN_CAVE:
					terrain = BATTLE_TERRAIN_WATER_IN_CAVE;
					break;
				case BATTLE_TERRAIN_VOLCANO:
					terrain = BATTLE_TERRAIN_LAVA_IN_VOLCANO;
					break;
				case BATTLE_TERRAIN_FOREST:
					terrain = BATTLE_TERRAIN_WATER_IN_FOREST;
					break;
				case BATTLE_TERRAIN_DARK_CAVE:
					terrain = BATTLE_TERRAIN_DARK_CAVE_WATER;
					break;
				case BATTLE_TERRAIN_SNOW_CAVE:
					terrain = BATTLE_TERRAIN_WATER_IN_SNOW_CAVE;
					break;
			}
		}
		
		if (terrain == BATTLE_TERRAIN_SNOW_CAVE
		&&  MetatileBehavior_IsIce(tileBehavior))
			terrain = BATTLE_TERRAIN_ICE_IN_CAVE;	
	#endif
	
    return terrain;
}

u8 LoadBattleBG_TerrainID(void) {
	u8 terrain = gBattleTerrain;

	if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_TRAINER_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_LINK))
	{
		terrain =  10;
	}
	else if (gBattleTypeFlags & BATTLE_TYPE_POKE_DUDE)
	{
		gBattleTerrain = 0;
		terrain = 0;
	}
	else if (VarGet(BATTLE_BG_VAR))
		;
	else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
	{
		u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;
		u8 trainerClassB = gTrainers[SECOND_OPPONENT].trainerClass;
		#ifdef OVERWRITE_BG_FOR_LEADER_CHAMPION
			if (trainerClass == TRAINER_CLASS_LEADER || trainerClassB == TRAINER_CLASS_LEADER)
			{
				terrain = 12;
			}
			else if (trainerClass == TRAINER_CLASS_CHAMPION || trainerClassB == TRAINER_CLASS_CHAMPION)
			{
				terrain = 19;
			}
			else if (GetCurrentMapBattleScene() != 0)
			{
				terrain = LoadBattleBG_SpecialTerrainID(GetCurrentMapBattleScene());
			}
		#else
			trainerClass += trainerClassB; //Useless statement so it compiles without warning
			if (GetCurrentMapBattleScene() != 0)
			{
				terrain = LoadBattleBG_SpecialTerrainID(GetCurrentMapBattleScene());
			}
		#endif
	}
	
	return terrain;
}

void LoadBattleBG_Background(u8 terrainId) {
	struct BattleBackground* table = gBattleTerrainTable;

	if (TerrainType) //A terrain like Electric Terrain is active
	{
		LZDecompressVram(gAttackTerrainTable[TerrainType - 1].tileset, (void*) 0x6008000);
		LZDecompressVram(gAttackTerrainTable[TerrainType - 1].tilemap, (void*) 0x600d000);
		LoadCompressedPalette(gAttackTerrainTable[TerrainType - 1].palette, 0x20, 0x60);
		return;
	}
	
	#ifdef UNBOUND //Load different BGs depending on time of day
		if (Clock->hour >= TIME_NIGHT_START || Clock->hour < TIME_MORNING_START)
			table = gBattleTerrainTableNight;
		else if (Clock->hour >= TIME_AFTERNOON_START)
			table = gBattleTerrainTableAfternoon;
	#endif
	
	LZDecompressVram(table[terrainId].tileset, (void*) 0x6008000);
	LZDecompressVram(table[terrainId].tilemap, (void*) 0x600d000);
	LoadCompressedPalette(table[terrainId].palette, 0x20, 0x60);
}

void LoadBattleBG_EntryOverlay(u8 terrainId)  {
	struct BattleBackground* table = gBattleTerrainTable;

	#ifdef UNBOUND //Load different BGs depending on time of day
		if (Clock->hour >= TIME_NIGHT_START || Clock->hour < TIME_MORNING_START)
			table = gBattleTerrainTableNight;
		else if (Clock->hour >= TIME_AFTERNOON_START)
			table = gBattleTerrainTableAfternoon;
	#endif
	
    LZDecompressVram(table[terrainId].entryTileset, (void*) 0x6004000);
    LZDecompressVram(table[terrainId].entryTilemap, (void*) 0x600E000);
}

bool8 MetatileBehavior_IsIce(u8 metatileBehavior)
{
    if (metatileBehavior == MB_ICE
	||  metatileBehavior == MB_THIN_ICE
	||  metatileBehavior == MB_CRACKED_ICE)
        return TRUE;
    else
        return FALSE;
}