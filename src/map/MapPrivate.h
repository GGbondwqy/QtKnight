/**
 * @file MapPrivate.h
 * @brief 地图模块私有的图块引用与空间索引。
 *
 * 仅供 KnightMap.dll 内部使用，不保存玩家或实体的可变运行状态。
 */

#pragma once

#include "KnightContracts.h"
#include <cstddef>
#include <vector>

namespace knight {

/// @brief 地图查询得到的图块引用及其世界矩形，仅供地图模块使用。
struct TileRef {
    int row = 0; ///< 图块所在行。
    int column = 0; ///< 图块所在列。
    Rect worldBounds{}; ///< 图块的世界碰撞矩形。
};

/// @brief 地图模块拥有的碰撞和触发区索引句柄，不拥有可变实体状态。
struct MapHandle {
    LevelId levelId; ///< 此索引对应的关卡编号。
    float tileSize = 0.0f; ///< 图块世界尺寸。
    TileLayer collisionLayer; ///< 构建索引时复制的碰撞图层。
    std::vector<TriggerDefinition> triggers; ///< 本关静态触发区副本。

    std::vector<std::vector<std::size_t>> triggerIndicesByCell; ///< 图块单元到触发区下标列表的索引；单元下标为 row * width + column。
};

} // namespace knight
