#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTMOTION_BUILD)
#    define KNIGHTMOTION_API __declspec(dllexport)
#  else
#    define KNIGHTMOTION_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTMOTION_API
#endif

namespace knight {

KNIGHTMOTION_API Result<MotionBatch> proposeMotion(
    const WorldSnapshot& before, const LevelDefinition& level,
    const IntentBatch& intents, float fixedSeconds) noexcept;
KNIGHTMOTION_API Result<ChangeBatch> finishMotion(
    const MotionBatch& proposed, const CollisionBatch& resolved) noexcept;

} // namespace knight
