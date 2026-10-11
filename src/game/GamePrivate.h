/**
 * @file GamePrivate.h
 * @brief 协调器私有的会话、输入和资源管理结构。
 *
 * 仅供 KnightGame.dll 内部使用；界面和底层 DLL 不得包含此文件。
 * 协调器安排模块调用，权威世界的写入仍由 KnightState.dll 执行。
 */

#pragma once

#include "KnightContracts.h"
#include <cstdint>
#include <deque>
#include <optional>

namespace knight {

/// @brief 状态模块拥有的权威世界；外部只能持有不透明指针并通过公开接口读写。
struct World;

/// @brief 地图模块拥有的碰撞和触发区索引句柄，不拥有可变实体状态。
struct MapHandle;

/// @brief 存档模块拥有的数据库句柄，其他模块不得读取连接或自行 delete。
struct SaveHandle;

/// @brief 协调器的会话状态，控制可接受命令及是否推进世界。
enum class SessionState {
    MainMenu, ///< 主菜单，等待开始、继续或退出。
    Loading, ///< 准备或恢复候选关卡。
    Playing, ///< 游玩中，允许固定逻辑帧推进。
    Paused, ///< 已暂停，不累计世界时间。
    Dead, ///< 玩家死亡，等待复活或返回。
    LevelComplete, ///< 本关已完成，等待下一关或返回。
    Exiting ///< 退出中，释放模块资源。
};

/// @brief 持续按住与本帧新按下分开记录，避免移动时吞掉或重复触发攻击。
struct InputState {
    bool leftHeld = false, rightHeld = false; ///< 左右键持续按住状态。
    bool jumpHeld = false, attackHeld = false, interactHeld = false; ///< 跳跃、攻击和交互的持续按住状态，用于识别新按下与释放。
    bool jumpPressedThisFrame = false, attackPressedThisFrame = false; ///< 跳跃和攻击的新按下边沿，消费后清除。
    bool interactPressedThisFrame = false; ///< 本逻辑帧是否新按下交互，消费后清除。
    CommandId nextCommandId = 1; ///< 下一个待分配命令编号，从 1 开始。
};

/// @brief 业务会话句柄；公开头文件仅声明类型，具体资源与会话成员只在业务层可见。
struct GameHandle {
    GameConfig config; ///< 创建会话时的配置副本。
    SessionState session = SessionState::MainMenu; ///< 当前菜单、加载、游玩或退出状态。
    InputState input; ///< 按住状态、新按下边沿和命令编号。
    std::deque<PlayerCommand> pendingCommands; ///< 已验证并编号、等待业务处理的命令。
    std::uint64_t accumulatedMicroseconds = 0; ///< 尚未转为固定逻辑帧的累计微秒数，暂停不累计世界时间。
    std::optional<LevelDefinition> currentLevel; ///< 已启用的静态关卡；主菜单时可为空。
    World* world = nullptr; ///< 权威世界句柄，必须经 destroyWorld 释放。
    MapHandle* map = nullptr; ///< 地图索引句柄，必须经 destroyMap 释放。
    SaveHandle* save = nullptr; ///< 存档连接句柄，必须经 closeSaveStore 释放。
    SlotId currentSlot = 0; ///< 当前槽位编号，0 表示未选槽。
    RenderSnapshot rendered; ///< 最近一次完整画面结果，供界面复制读取。
    EventBatch queuedEvents; ///< 已确认且尚未被 takeEvents 消费的事件。
};

} // namespace knight
