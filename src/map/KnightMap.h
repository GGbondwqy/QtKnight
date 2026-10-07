#pragma once

#include "KnightContracts.h"

#if defined(_WIN32)
#  if defined(KNIGHTMAP_BUILD)
#    define KNIGHTMAP_API __declspec(dllexport)
#  else
#    define KNIGHTMAP_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTMAP_API
#endif

namespace knight {

struct MapHandle;

KNIGHTMAP_API Result<MapHandle*> createMap(const LevelDefinition& level) noexcept;
KNIGHTMAP_API void destroyMap(MapHandle* map) noexcept;
KNIGHTMAP_API Result<CollisionBatch> resolveMovement(
    MapHandle* map, const WorldSnapshot& before,
    const MotionBatch& proposed) noexcept;
KNIGHTMAP_API Result<TriggerBatch> queryTriggers(
    MapHandle* map, const WorldSnapshot& before,
    const CollisionBatch& resolved) noexcept;

} // namespace knight
