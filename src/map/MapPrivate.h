#pragma once

#include "KnightContracts.h"
#include <cstddef>
#include <vector>

namespace knight {

struct TileRef {
    int row = 0;
    int column = 0;
    Rect worldBounds{};
};

// 地图索引只属于地图模块；不保存玩家或世界的可变数据。
struct MapHandle {
    LevelId levelId;
    float tileSize = 0.0f;
    TileLayer collisionLayer;
    std::vector<TriggerDefinition> triggers;
    // 图块下标为 row * width + column；值为触发区下标列表。
    std::vector<std::vector<std::size_t>> triggerIndicesByCell;
};

} // namespace knight
