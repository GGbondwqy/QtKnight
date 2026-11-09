#pragma once

#include "datamanagelib/EnemyObject.h"

#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

namespace QtKnight
{

/// 无状态规则计算；输入只含几何、数值和枚举，不持有运行期对象。
class GameRules
{
public:
    struct MovementResult
    {
        QRectF bounds;
        QPointF velocity;
        bool onGround = false;
    };

    struct EnemyInput
    {
        EnemyState state = EnemyState::Patrol;
        QRectF bounds;
        QRectF targetBounds;
        QPointF homePosition;
        int moveDirection = 0;
        bool alive = true;
        bool targetAlive = true;
        bool attackReady = false;
        bool wallAhead = false;
        qreal detectDistance = 0;
        qreal disengageDistance = 0;
        qreal attackRange = 0;
    };

    struct EnemyDecision
    {
        EnemyState state = EnemyState::Patrol;
        int moveDirection = 0;
        bool attackRequested = false;
    };

    GameRules() = delete;

    // 距离为像素，速度为像素/秒，加速度为像素/秒²，seconds 为本次逻辑步时长。
    static QPointF applyGravity(const QPointF& velocity, qreal gravity,
                               qreal maxFallSpeed, qreal seconds);

    /// 对平台、墙和当前关闭门统一做碰撞修正，返回实际矩形、速度及落地状态。
    static MovementResult moveAndCollide(const QRectF& bounds, const QPointF& velocity,
                                        qreal seconds, const QList<QRectF>& solidBounds);

    static QRectF attackBounds(const QRectF& bounds, bool facingLeft, qreal reach);
    static EnemyDecision decideEnemy(const EnemyInput& input);

};

} // namespace QtKnight
