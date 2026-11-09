#pragma once

#include "MapObject.h"

namespace QtKnight
{

class PlayerObject;

/// 有开启条件的通路；门改变通行状态，出口还携带目标关卡。
class DoorObject : public MapObject
{
public:
    enum class Kind
    {
        Door,
        Exit
    };

    Kind kind = Kind::Door;
    int requiredKeyId = 0; ///< 0 表示不需要钥匙。
    int requiredEnemyId = 0; ///< 0 表示没有指定敌人条件。
    int requiredGoalId = 0; ///< 0 表示没有指定目标条件。
    int targetLevelId = 0; ///< 出口使用；0 表示没有下一关，由会话完成游戏流程。
    bool open = false;

    bool isBlocking() const override; ///< 只有未开启的门阻挡移动，出口另行检查切关条件。

    /// 只查询钥匙、目标及指定敌人条件，不消耗物品、不改变 open、不执行切关。
    /// requiredEnemyAlive 由会话查询；未设置 requiredEnemyId 时忽略该参数。
    bool canOpen(const PlayerObject& player, bool requiredEnemyAlive) const;
};

} // namespace QtKnight
