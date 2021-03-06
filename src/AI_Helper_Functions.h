#pragma once
#include "defines.h"

extern bool8 CanKillAFoe(u8 bank);
extern bool8 CanKnockOut(u8 bankAtk, u8 bankDef);
extern bool8 MoveKnocksOut(u16 move, u8 bankAtk, u8 bankDef);
extern bool8 IsStrongestMove(u16 currentMove, u8 bankAtk, u8 bankDef);
extern bool8 MoveWillHit(u16 move, u8 bankAtk, u8 bankDef);
extern bool8 MoveWouldHitFirst(u16 move, u16 bankAtk, u16 bankDef);
extern bool8 DamagingMoveInMoveset(u8 bank);
extern bool8 PhysicalMoveInMoveset(u8 bank);
extern bool8 SpecialMoveInMoveset(u8 bank);
extern bool8 MagicCoatableMovesInMoveset(u8 bank);
extern bool8 GetHealthPercentage(u8 bank);
extern bool8 TeamFullyHealedMinusBank(u8 bank);
extern bool8 HasProtectionMoveInMoveset(u8 bank, bool8 AllKinds);
extern move_t ShouldAIUseZMove(u8 bank, u8 moveIndex, u16 move);