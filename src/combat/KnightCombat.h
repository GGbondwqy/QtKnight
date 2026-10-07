#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTCOMBAT_BUILD)
#    define KNIGHTCOMBAT_API __declspec(dllexport)
#  else
#    define KNIGHTCOMBAT_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTCOMBAT_API
#endif

namespace knight {

KNIGHTCOMBAT_API Result<CombatResult> evaluateAttacks(
    const CombatInput& input) noexcept;
KNIGHTCOMBAT_API Result<CombatResult> evaluateTraps(
    const CombatInput& input, const TriggerBatch& triggers) noexcept;
KNIGHTCOMBAT_API Result<CombatResult> resolveDamage(
    const WorldSnapshot& before, const CombatResult& candidates) noexcept;

} // namespace knight
