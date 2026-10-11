/**
 * @file KnightMap.h
 * @brief 地图碰撞裁决与触发区查询的公开接口。
 *
 * 当前函数仅有声明，注释说明目标约定；只返回结果，不直接修改世界。
 * 运动提案和地图结果均经业务层传递，不直接调用其他底层 DLL。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTMAP_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
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

/// @brief 地图模块拥有的碰撞和触发区索引句柄，不拥有可变实体状态。
struct MapHandle;

/**
 * @brief 从已验证关卡建立碰撞和触发区索引。
 * @details 句柄由 destroyMap 释放，不保留调用方输入引用。
 * @param level 完整关卡，含图块尺寸、碰撞图层与触发区。
 * @return 成功返回地图句柄；失败返回状态。
 */
KNIGHTMAP_API Result<MapHandle*> createMap(const LevelDefinition& level) noexcept;

/**
 * @brief 释放地图模块建立的索引。
 * @param map 本模块创建的句柄，可为 nullptr；释放后失效。
 */
KNIGHTMAP_API void destroyMap(MapHandle* map) noexcept;

/**
 * @brief 根据旧碰撞盒和运动提案裁决地图阻挡。
 * @param map 对应当前关卡的有效地图索引。
 * @param before 本帧旧世界快照，提供实体身份和碰撞盒。
 * @param proposed 经协调器传入的同帧运动提案。
 * @return 成功返回允许位置、碰撞法线、落地及撞顶标志，不修改 World。
 */
KNIGHTMAP_API Result<CollisionBatch> resolveMovement(
    MapHandle* map, const WorldSnapshot& before,
    const MotionBatch& proposed) noexcept;

/**
 * @brief 查询实体在碰撞修正后位置命中的触发区。
 * @details 不直接伤害、拾取或切关，后续业务规则由协调器安排。
 * @param map 对应当前关卡的有效触发区索引。
 * @param before 本帧旧快照，提供实体身份和碰撞盒。
 * @param resolved 同帧地图裁决后的允许位置。
 * @return 成功返回危险区、物品、检查点、门和出口命中事实。
 */
KNIGHTMAP_API Result<TriggerBatch> queryTriggers(
    MapHandle* map, const WorldSnapshot& before,
    const CollisionBatch& resolved) noexcept;

} // namespace knight
