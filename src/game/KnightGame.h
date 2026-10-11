/**
 * @file KnightGame.h
 * @brief 界面调用游戏协调器的公开接口。
 *
 * 界面只持有 GameHandle，不接触底层世界、地图或数据库对象。
 * 当前函数仅有声明；注释说明目标接口约定。Result 的 value 仅在状态为 Ok 时有效。
 * 公开接口为 noexcept，实现时须在本 DLL 内处理异常并转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTGAME_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTGAME_BUILD)
#    define KNIGHTGAME_API __declspec(dllexport)
#  else
#    define KNIGHTGAME_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTGAME_API
#endif

namespace knight {

/// @brief 业务会话句柄；公开头文件仅声明类型，具体资源与会话成员只在业务层可见。
struct GameHandle;

/**
 * @brief 建立业务会话并打开存档库，初始显示主菜单。
 * @details 返回句柄由 destroyGame 释放，界面不得自行 delete。
 * @param config 内容目录、数据库路径、首关编号及契约版本。
 * @return 成功返回业务句柄，失败返回状态；此时不创建关卡世界。
 */
KNIGHTGAME_API Result<GameHandle*> createGame(const GameConfig& config) noexcept;

/**
 * @brief 释放业务会话并安排底层模块销毁其各自句柄。
 * @details 世界、地图、存档连接分别通过所属模块销毁接口释放。
 * @param game 本 DLL 创建的会话句柄，可为 nullptr；释放后失效。
 */
KNIGHTGAME_API void destroyGame(GameHandle* game) noexcept;

/**
 * @brief 验证、编号并记录界面命令及其输入状态。
 * @details 持续按住与新按下边沿独立记录；菜单命令在推进玩法前处理。
 * @param game 当前有效业务会话。
 * @param command 动作、输入阶段、方向及可选槽位；编号和目标帧由业务层分配。
 * @return 成功表示命令已接受并入队，不表示玩法动作已经生效。
 */
KNIGHTGAME_API Status submitCommand(
    GameHandle* game, const PlayerCommand& command) noexcept;

/**
 * @brief 按真实经过时间处理命令并推进固定逻辑帧。
 * @details 目标步长为 1/60 秒；每次最多计入 250 ms、补 5 帧；暂停不累计世界时间。
 * @param game 当前有效业务会话。
 * @param elapsedMicroseconds 单调时钟测得的自上次唤醒以来的微秒数，不是绝对时间戳。
 * @return 返回本次处理状态，结果保存在会话的世界、画面和事件中。
 */
KNIGHTGAME_API Status advanceElapsed(
    GameHandle* game, std::uint64_t elapsedMicroseconds) noexcept;

/**
 * @brief 复制最近的完整画面结果供界面显示。
 * @param game 当前有效业务会话。
 * @return 成功返回画面副本；读取不消费一次性事件。
 */
KNIGHTGAME_API Result<RenderSnapshot> readRenderSnapshot(GameHandle* game) noexcept;

/**
 * @brief 取走尚未消费的一次性业务事件。
 * @details 相同事件不会在下一次成功读取时重复返回。
 * @param game 当前有效业务会话。
 * @return 成功返回事件并清空队列；失败保留队列；无事件时返回空批次。
 */
KNIGHTGAME_API Result<EventBatch> takeEvents(GameHandle* game) noexcept;

} // namespace knight
