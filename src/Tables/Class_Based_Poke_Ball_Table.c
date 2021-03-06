#include "../defines.h"
#include "../catching.h"

#ifndef UNBOUND
u8 ClassPokeBalls[NUM_TRAINER_CLASSES] = {
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNTRAINER	       	0x00
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNTRAINER	       	0x01
    BALL_TYPE_DIVE_BALL,      //	TRAINER_AQUALEADER	       	0x02
    BALL_TYPE_DIVE_BALL,      //	TRAINER_TEAMAQUA	       	0x03
    BALL_TYPE_POKE_BALL,      //	TRAINER_AROMALADY	       	0x04
    BALL_TYPE_POKE_BALL,      //	TRAINER_RUINMANIAC	       	0x05
    BALL_TYPE_POKE_BALL,      //	TRAINER_INTERVIEWER	       	0x06
    BALL_TYPE_POKE_BALL,      //	TRAINER_TUBER	       		0x07
    BALL_TYPE_POKE_BALL,      //	TRAINER_TUBER	       		0x08
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_COOLTRAINER	       	0x09
    BALL_TYPE_POKE_BALL,      //	TRAINER_HEXMANIAC	       	0x0A
    BALL_TYPE_POKE_BALL,      //	TRAINER_LADY	       		0x0B
    BALL_TYPE_POKE_BALL,      //	TRAINER_BEAUTY	       		0x0C
    BALL_TYPE_LUXURY_BALL,    //	TRAINER_RICHBOY	       		0x0D
    BALL_TYPE_PREMIER_BALL,   //	TRAINER_POKEMANIAC	       	0x0E
    BALL_TYPE_NET_BALL,       //	TRAINER_SWIMMER	       		0x0F
    BALL_TYPE_POKE_BALL,      //	TRAINER_BLACKBELT	       	0x10
    BALL_TYPE_POKE_BALL,      //	TRAINER_GUITARIST	       	0x11
    BALL_TYPE_POKE_BALL,      //	TRAINER_KINDLER	       		0x12
    BALL_TYPE_POKE_BALL,      //	TRAINER_CAMPER	       		0x13
    BALL_TYPE_NET_BALL,       //	TRAINER_BUGMANIAC	       	0x14
    BALL_TYPE_POKE_BALL,      //	TRAINER_PSYCHIC	       		0x15
    BALL_TYPE_POKE_BALL,      //	TRAINER_GENTLEMAN	       	0x16
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_ELITEFOUR	       	0x17
    BALL_TYPE_POKE_BALL,      //	TRAINER_LEADER	       		0x18
    BALL_TYPE_POKE_BALL,      //	TRAINER_SCHOOLKID	       	0x19
    BALL_TYPE_POKE_BALL,      //	TRAINER_SRANDJR	       		0x1A
    BALL_TYPE_LUXURY_BALL,    //	TRAINER_POKEFAN	       		0x1B
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_EXPERT	       		0x1C
    BALL_TYPE_POKE_BALL,      //	TRAINER_YOUNGSTER	       	0x1D
    BALL_TYPE_POKE_BALL,      //	TRAINER_CHAMPION	       	0x1E
    BALL_TYPE_DIVE_BALL,      //	TRAINER_FISHERMAN	       	0x1F
    BALL_TYPE_POKE_BALL,      //	TRAINER_TRIATHLETE	       	0x20
    BALL_TYPE_POKE_BALL,      //	TRAINER_DRAGONTAMER	       	0x21
    BALL_TYPE_POKE_BALL,      //	TRAINER_BIRDKEEPER	       	0x22
    BALL_TYPE_POKE_BALL,      //	TRAINER_NINJABOY	       	0x23
    BALL_TYPE_POKE_BALL,      //	TRAINER_BATTLEGIRL	       	0x24
    BALL_TYPE_POKE_BALL,      //	TRAINER_PARASOLLADY	       	0x25
    BALL_TYPE_NET_BALL,       //	TRAINER_SWIMMERF	       	0x26
    BALL_TYPE_POKE_BALL,      //	TRAINER_PICNICKER	       	0x27
    BALL_TYPE_REPEAT_BALL,    //	TRAINER_TWINS	       		0x28
    BALL_TYPE_POKE_BALL,      //	TRAINER_SAILER	       		0x29
    BALL_TYPE_POKE_BALL,      //	TRAINER_BOARDER	       		0x2A
    BALL_TYPE_PREMIER_BALL,   //	TRAINER_COLLECTOR	       	0x2B
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNTRAINER	       	0x2C
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNBREEDER	       	0x2D
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNRANGER	       	0x2E
    BALL_TYPE_SAFARI_BALL,    //	TRAINER_MAGMALEADER	       	0x2F
    BALL_TYPE_SAFARI_BALL,    //	TRAINER_TEAMMAGMA	       	0x30
    BALL_TYPE_POKE_BALL,      //	TRAINER_LASS	       		0x31
    BALL_TYPE_NET_BALL,       //	TRAINER_BUGCATCHER	       	0x32
    BALL_TYPE_POKE_BALL,      //	TRAINER_HIKER	       		0x33
    BALL_TYPE_POKE_BALL,      //	TRAINER_YOUNGCOUPLE	       	0x34
    BALL_TYPE_POKE_BALL,      //	TRAINER_OLDCOUPLE	       	0x35
    BALL_TYPE_POKE_BALL,      //	TRAINER_SISANDBRO	       	0x36
    BALL_TYPE_DIVE_BALL,      //	TRAINER_AQUAADMIN	       	0x37
    BALL_TYPE_SAFARI_BALL,    //	TRAINER_MAGMAADMIN	       	0x38
    BALL_TYPE_POKE_BALL,      //	TRAINER_YOUNGSTER	       	0x39
    BALL_TYPE_NET_BALL,       //	TRAINER_BUGCATCHER	       	0x3A
    BALL_TYPE_POKE_BALL,      //	TRAINER_LASS	       		0x3B
    BALL_TYPE_POKE_BALL,      //	TRAINER_SAILER	       		0x3C
    BALL_TYPE_POKE_BALL,      //	TRAINER_CAMPER	       		0x3D
    BALL_TYPE_POKE_BALL,      //	TRAINER_PICNICKER	       	0x3E
    BALL_TYPE_PREMIER_BALL,   //	TRAINER_POKEMANIAC	       	0x3F
    BALL_TYPE_POKE_BALL,      //	TRAINER_SUPERNERD	       	0x40
    BALL_TYPE_POKE_BALL,      //	TRAINER_HIKER	       		0x41
    BALL_TYPE_POKE_BALL,      //	TRAINER_BIKER	       		0x42
    BALL_TYPE_POKE_BALL,      //	TRAINER_BURGLAR	       		0x43
    BALL_TYPE_POKE_BALL,      //	TRAINER_ENGINEER	       	0x44
    BALL_TYPE_DIVE_BALL,      //	TRAINER_FISHERMAN	       	0x45
    BALL_TYPE_NET_BALL,       //	TRAINER_SWIMMERM	       	0x46
    BALL_TYPE_POKE_BALL,      //	TRAINER_CUEBALL	       		0x47
    BALL_TYPE_POKE_BALL,      //	TRAINER_GAMER	       		0x48
    BALL_TYPE_POKE_BALL,      //	TRAINER_BEAUTY	       		0x49
    BALL_TYPE_NET_BALL,       //	TRAINER_SWIMMERF	       	0x4A
    BALL_TYPE_POKE_BALL,      //	TRAINER_PSYCHIC	       		0x4B
    BALL_TYPE_POKE_BALL,      //	TRAINER_ROCKER	       		0x4C
    BALL_TYPE_POKE_BALL,      //	TRAINER_JUGGLER	       		0x4D
    BALL_TYPE_POKE_BALL,      //	TRAINER_TAMER	       		0x4E
    BALL_TYPE_POKE_BALL,      //	TRAINER_BIRDKEEPER	       	0x4F
    BALL_TYPE_POKE_BALL,      //	TRAINER_BLACKBELT	       	0x50
    BALL_TYPE_POKE_BALL,      //	TRAINER_RIVAL	       		0x51
    BALL_TYPE_POKE_BALL,      //	TRAINER_SCIENTIST	       	0x52
    BALL_TYPE_POKE_BALL,      //	TRAINER_BOSS	      	 	0x53
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_LEADER	       		0x54
    BALL_TYPE_NEST_BALL,      //	TRAINER_TEAMROCKET	       	0x55
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_COOLTRAINER	       	0x56
    BALL_TYPE_ULTRA_BALL,     //	TRAINER_ELITEFOUR	       	0x57
    BALL_TYPE_POKE_BALL,      //	TRAINER_GENTLEMAN	       	0x58
    BALL_TYPE_POKE_BALL,      //	TRAINER_RIVAL	       		0x59
    BALL_TYPE_POKE_BALL,      //	TRAINER_CHAMPION	       	0x5A
    BALL_TYPE_POKE_BALL,      //	TRAINER_CHANNELER	       	0x5B
    BALL_TYPE_REPEAT_BALL,    //	TRAINER_TWINS	       		0x5C
    BALL_TYPE_POKE_BALL,      //	TRAINER_COOLCOUPLE	       	0x5D
    BALL_TYPE_POKE_BALL,      //	TRAINER_YOUNGCOUPLE	       	0x5E
    BALL_TYPE_POKE_BALL,      //	TRAINER_CRUSHKIN	       	0x5F
    BALL_TYPE_POKE_BALL,      //	TRAINER_SISANDBRO	       	0x60
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNPROF	       	0x61
    BALL_TYPE_POKE_BALL,      //	TRAINER_PLAYER	       		0x62
    BALL_TYPE_POKE_BALL,      //	TRAINER_CRUSHGIRL	       	0x63
    BALL_TYPE_POKE_BALL,      //	TRAINER_TUBER	       		0x64
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNBREEDER	       	0x65
    BALL_TYPE_POKE_BALL,      //	TRAINER_PKMNRANGER	       	0x66
    BALL_TYPE_POKE_BALL,      //	TRAINER_AROMALADY	       	0x67
    BALL_TYPE_POKE_BALL,      //	TRAINER_RUINMANIAC	       	0x68
    BALL_TYPE_POKE_BALL,      //	TRAINER_LADY	       		0x69
    BALL_TYPE_POKE_BALL,      //	TRAINER_PAINTER	       		0x6A
};

#else
//This table is for Pokemon Unbound. Modifying it will do nothing for you.
u8 ClassPokeBalls[NUM_TRAINER_CLASSES] = {
    BALL_TYPE_POKE_BALL,      	//	BLANK	                   0x00
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNTRAINER	       0x01
    BALL_TYPE_ULTRA_BALL,      	//	TRAINER_LORLEADER	       0x02
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TEAMAQUA	       0x03
    BALL_TYPE_POKE_BALL,      	//	TRAINER_AROMALADY	       0x04
    BALL_TYPE_POKE_BALL,      	//	TRAINER_RUINMANIAC	       0x05
    BALL_TYPE_POKE_BALL,      	//	TRAINER_INTERVIEWER	       0x06
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TUBER	           0x07
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TUBER	           0x08
    BALL_TYPE_POKE_BALL,     	//	TRAINER_ACETRAINER	       0x09
    BALL_TYPE_HEAL_BALL,      	//	TRAINER_NURSE	           0x0A
    BALL_TYPE_POKE_BALL,      	//	TRAINER_LADY	           0x0B
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BEAUTY	           0x0C
    BALL_TYPE_LUXURY_BALL,    	//	TRAINER_RICHBOY	           0x0D
    BALL_TYPE_POKE_BALL,      	//	TRAINER_POKEMANIAC	       0x0E
    BALL_TYPE_POKE_BALL,       	//	TRAINER_CAMPINGDUO         0x0F
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SKIIER	           0x10
    BALL_TYPE_POKE_BALL,      	//	TRAINER_GUITARIST	       0x11
    BALL_TYPE_POKE_BALL,      	//	TRAINER_KINDLER	           0x12
    BALL_TYPE_POKE_BALL,      	//	TRAINER_CAMPER	           0x13
    BALL_TYPE_NET_BALL,       	//	TRAINER_BUGMANIAC	       0x14
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PSYCHIC	           0x15
    BALL_TYPE_POKE_BALL,      	//	TRAINER_GENTLEMAN	       0x16
    BALL_TYPE_POKE_BALL,     	//	TRAINER_ELITEFOUR	       0x17
    BALL_TYPE_LUXURY_BALL,     	//	TRAINER_SUCCESSOR	       0x18
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SCHOOLKID	       0x19
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SRANDJR	           0x1A
    BALL_TYPE_LUXURY_BALL,    	//	TRAINER_POKEFAN	           0x1B
    BALL_TYPE_ULTRA_BALL,     	//	TRAINER_EXPERT	           0x1C
    BALL_TYPE_POKE_BALL,      	//	TRAINER_YOUNGSTER	       0x1D
    BALL_TYPE_POKE_BALL,      	//	TRAINER_CHAMPION	       0x1E
    BALL_TYPE_POKE_BALL,      	//	TRAINER_FISHERMAN	       0x1F
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TRIATHLETE	       0x20
    BALL_TYPE_GREAT_BALL,      	//	TRAINER_DRAGONTAMER	       0x21
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BIRDKEEPER	       0x22
    BALL_TYPE_POKE_BALL,      	//	TRAINER_NINJABOY	       0x23
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BATTLEGIRL	       0x24
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PARASOLLADY	       0x25
    BALL_TYPE_POKE_BALL,       	//	TRAINER_SWIMMERF	       0x26
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PICNICKER	       0x27
    BALL_TYPE_POKE_BALL,    	//	TRAINER_TWINS	           0x28
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SAILER	           0x29
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BOARDER	           0x2A
    BALL_TYPE_PREMIER_BALL,   	//	TRAINER_COLLECTOR	       0x2B
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNTRAINER	       0x2C
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNBREEDER	       0x2D
    BALL_TYPE_DUSK_BALL,      	//	TRAINER_LORADMIN	       0x2E
    BALL_TYPE_PREMIER_BALL,    	//	TRAINER_LIGHTOFRUIN	       0x2F
    BALL_TYPE_DUSK_BALL,    	//	TRAINER_SHADOWADMIN	       0x30
    BALL_TYPE_POKE_BALL,      	//	TRAINER_LASS	           0x31
    BALL_TYPE_POKE_BALL,       	//	TRAINER_BUGCATCHER	       0x32
    BALL_TYPE_POKE_BALL,      	//	TRAINER_HIKER	           0x33
    BALL_TYPE_POKE_BALL,      	//	TRAINER_YOUNGCOUPLE	       0x34
    BALL_TYPE_ULTRA_BALL,      	//	TRAINER_EXPERTDUO	       0x35
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SISANDBRO	       0x36
    BALL_TYPE_POKE_BALL,      	//	TRAINER_AQUAADMIN	       0x37
    BALL_TYPE_POKE_BALL,    	//	TRAINER_MAGMAADMIN	       0x38
    BALL_TYPE_POKE_BALL,      	//	TRAINER_YOUNGSTER	       0x39
    BALL_TYPE_NET_BALL,       	//	TRAINER_BUGCATCHER	       0x3A
    BALL_TYPE_POKE_BALL,      	//	TRAINER_LASS	           0x3B
    BALL_TYPE_DIVE_BALL,      	//	TRAINER_SAILER	           0x3C
    BALL_TYPE_POKE_BALL,      	//	TRAINER_CAMPER	           0x3D
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PICNICKER	       0x3E
    BALL_TYPE_PREMIER_BALL,   	//	TRAINER_POKEMANIAC	       0x3F
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SUPERNERD	       0x40
    BALL_TYPE_POKE_BALL,      	//	TRAINER_HIKER	           0x41
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BIKER	           0x42
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BURGLAR	           0x43
    BALL_TYPE_POKE_BALL,      	//	TRAINER_ENGINEER	       0x44
    BALL_TYPE_DIVE_BALL,      	//	TRAINER_FISHERMAN	       0x45
    BALL_TYPE_NET_BALL,       	//	TRAINER_SWIMMERM	       0x46
    BALL_TYPE_POKE_BALL,      	//	TRAINER_ROUGHNECK	       0x47
    BALL_TYPE_GREAT_BALL,      	//	TRAINER_GAMBLER	           0x48
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BEAUTY	           0x49
    BALL_TYPE_NET_BALL,       	//	TRAINER_SWIMMERF	       0x4A
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PSYCHIC	           0x4B
    BALL_TYPE_POKE_BALL,      	//	TRAINER_ROCKER	           0x4C
    BALL_TYPE_POKE_BALL,      	//	TRAINER_JUGGLER	           0x4D
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TAMER	           0x4E
    BALL_TYPE_NEST_BALL,      	//	TRAINER_BIRDKEEPER	       0x4F
    BALL_TYPE_POKE_BALL,      	//	TRAINER_BLACKBELT	       0x50
    BALL_TYPE_POKE_BALL,      	//	TRAINER_RIVAL	           0x51
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SCIENTIST	       0x52
    BALL_TYPE_ULTRA_BALL,      	//	TRAINER_BOSS	           0x53
    BALL_TYPE_ULTRA_BALL,     	//	TRAINER_LEADER	           0x54
    BALL_TYPE_DUSK_BALL,      	//	TRAINER_SHADOW	           0x55
    BALL_TYPE_ULTRA_BALL,     	//	TRAINER_ACETRAINER	       0x56
    BALL_TYPE_ULTRA_BALL,     	//	TRAINER_ELITEFOUR	       0x57
    BALL_TYPE_LUXURY_BALL,     	//	TRAINER_GENTLEMAN	       0x58
    BALL_TYPE_POKE_BALL,      	//	TRAINER_RIVAL	           0x59
    BALL_TYPE_ULTRA_BALL,      	//	TRAINER_CHAMPION	       0x5A
    BALL_TYPE_POKE_BALL,      	//	TRAINER_MEDIUM	           0x5B
    BALL_TYPE_REPEAT_BALL,    	//	TRAINER_TWINS	           0x5C
    BALL_TYPE_ULTRA_BALL,      	//	TRAINER_ACEDUO	           0x5D
    BALL_TYPE_POKE_BALL,      	//	TRAINER_YOUNGCOUPLE	       0x5E
    BALL_TYPE_POKE_BALL,      	//	TRAINER_CRUSHKIN	       0x5F
    BALL_TYPE_POKE_BALL,      	//	TRAINER_SISANDBRO	       0x60
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNPROF	       0x61
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PLAYER	           0x62
    BALL_TYPE_POKE_BALL,      	//	TRAINER_CRUSHGIRL	       0x63
    BALL_TYPE_POKE_BALL,      	//	TRAINER_TUBER	           0x64
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNBREEDER	       0x65
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PKMNRANGER	       0x66
    BALL_TYPE_POKE_BALL,      	//	TRAINER_AROMALADY	       0x67
    BALL_TYPE_POKE_BALL,      	//	TRAINER_RUINMANIAC	       0x68
    BALL_TYPE_LUXURY_BALL,     	//	TRAINER_LADY	           0x69
    BALL_TYPE_POKE_BALL,      	//	TRAINER_PAINTER	           0x6A
};
#endif
