#pragma once

#include "CharacterObject.h"

namespace QtKnight
{

enum class EnemyState
{
    Patrol,
    Alert,
    Chase,
    Attack,
    Return,
    Dead
};

/// 继承公共角色状态，只补充敌人的 AI 阶段、巡逻锚点与移动意图。
class EnemyObject : public CharacterObject
{
public:
    EnemyObject(); ///< 初始碰撞体 42×38、生命与上限均为 2。

    EnemyState state = EnemyState::Patrol;
    QPointF homePosition; ///< 巡逻与脱战返回的世界坐标锚点。
    int moveDirection = 0; ///< -1 向左，0 停止，1 向右。
};

} // namespace QtKnight
