#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTCONTENT_BUILD)
#    define KNIGHTCONTENT_API __declspec(dllexport)
#  else
#    define KNIGHTCONTENT_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTCONTENT_API
#endif

namespace knight {

KNIGHTCONTENT_API Result<LevelDefinition> loadLevel(
    const ContentRoot& root, const LevelId& id) noexcept;

} // namespace knight
