#pragma once

#include "EntityObject.h"

namespace QtKnight
{

/// 继承实体空间属性，统一角色的移动、生命与冷却状态。
class CharacterObject : public EntityObject
{
public:
    // 角色专用移动状态；速度为像素/秒，右、下为正。
    bool facingLeft = false;
    QPointF velocity;
    bool onGround = false;

    int health = 0;
    int maxHealth = 0;
    int attackCooldownFrames = 0; ///< 会话按固定逻辑步递减至 0，暂停不推进。

    virtual int takeDamage(int damage); ///< 基础扣血，返回实际扣除量，生命不低于 0。

protected:
    /// 派生类提供碰撞尺寸与生命上限，按满血初始化。
    CharacterObject(const QSizeF& size, int maxHealth);
};

} // namespace QtKnight
