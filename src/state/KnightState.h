/**
 * @file KnightState.h
 * @brief 权威世界的创建、读取和原子提交接口。
 *
 * 仅供业务层调用；其他计算模块只能接收世界快照。
 * 目前只有 createWorld、destroyWorld、getPlayer 有演示实现；其余函数为目标接口声明。
 * Result 的 value 仅在状态为 Ok 时有效；公开接口的异常须在本 DLL 内处理。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTSTATE_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTSTATE_BUILD)
#    define KNIGHTSTATE_API __declspec(dllexport)
#  else
#    define KNIGHTSTATE_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTSTATE_API
#endif

namespace knight {

/// @brief 状态模块拥有的权威世界；外部只能持有不透明指针并通过公开接口读写。
struct World;

/**
 * @brief 分配候选世界句柄，正式关卡仍需 initializeWorld 初始化。
 * @details 当前演示实现会预置编号 1、生命 5 的玩家，不能视为正式关卡初始化。
 * @return 成功返回 World*，初始分配失败返回 nullptr；由 destroyWorld 释放。
 */
KNIGHTSTATE_API World* createWorld() noexcept;

/**
 * @brief 释放状态模块创建的世界及其内部值对象。
 * @param world 本模块创建的世界，可为 nullptr；释放后原指针失效。
 */
KNIGHTSTATE_API void destroyWorld(World* world) noexcept;

/**
 * @brief 读取玩家单体状态副本；完整帧计算应使用 readWorld。
 * @details 当前演示实现固定查找编号 1 的玩家。
 * @param world 当前世界；nullptr 会失败。
 * @param [out] output 调用方提供的可写快照地址，不得为 nullptr。
 * @return 成功写入 *output 并返回 true；失败返回 false，不应使用输出结果。
 */
KNIGHTSTATE_API bool getPlayer(World* world, EntitySnapshot* output) noexcept;

/**
 * @brief 按静态关卡与可选存档建立完整世界。
 * @details 先在私有草稿中构造，全部成功后整体替换。
 * @param world 待初始化的候选世界句柄。
 * @param level 已验证的完整关卡和数值原型。
 * @param restore 可空的已验证存档；nullptr 表示不从存档恢复。
 * @return 成功建立实体与进度，失败保留候选世界原有内容。
 */
KNIGHTSTATE_API Status initializeWorld(
    World* world, const LevelDefinition& level, const SaveData* restore) noexcept;

/**
 * @brief 复制已提交的完整世界，供本帧计算或生成存档。
 * @param world 当前有效世界句柄。
 * @return 成功返回独立世界副本，修改副本不会回写权威状态。
 */
KNIGHTSTATE_API Result<WorldSnapshot> readWorld(World* world) noexcept;

/**
 * @brief 校验并原子提交业务层合并的整帧变更。
 * @details 每个逻辑帧仅提交一次；校验实体、载荷类型及来源帧，失败不发布成功事件。
 * @param world 当前权威世界。
 * @param changes 本帧全部候选；sourceFrame 必须等于当前世界帧号加一。
 * @return 成功返回新世界与提交事件；失败整批不生效，保留旧世界。
 */
KNIGHTSTATE_API Result<CommitResult> applyChanges(
    World* world, const ChangeBatch& changes) noexcept;

/**
 * @brief 从检查点进度重建玩家、敌人和陷阱等运行状态。
 * @param world 要替换内容的当前世界句柄。
 * @param level 检查点所属的完整关卡定义。
 * @param progress 最近检查点与永久能力、关键物品等进度。
 * @return 成功替换世界内容，失败保留原世界。
 */
KNIGHTSTATE_API Status resetAtCheckpoint(
    World* world, const LevelDefinition& level, const SaveData& progress) noexcept;

} // namespace knight
