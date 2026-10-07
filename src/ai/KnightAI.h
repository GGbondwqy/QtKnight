#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTAI_BUILD)
#    define KNIGHTAI_API __declspec(dllexport)
#  else
#    define KNIGHTAI_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTAI_API
#endif

namespace knight {

KNIGHTAI_API Result<AIResult> decideEnemies(
    const WorldSnapshot& before, const LevelDefinition& level,
    FrameId frame) noexcept;

} // namespace knight
