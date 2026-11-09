#pragma once

#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QString>

namespace QtKnight
{

/// 一关的配置与生成描述；加载器填充并校验，会话据此创建运行期对象。
class GameMap
{
public:
    // 空间为像素，速度为像素/秒，加速度为像素/秒²；计时为 60Hz 逻辑帧。
    struct MotionRules
    {
        qreal speed = 0;
        qreal jumpSpeed = 0; ///< 起跳速度大小，实际向上速度取负值。
        qreal gravity = 0;
        qreal maxFallSpeed = 0;
    };

    struct AttackRules
    {
        int damage = 0;
        int energyCost = 0;
        int windupFrames = 0;
        int activeFrames = 0;
        int recoveryFrames = 0;
        int cooldownFrames = 0;
        qreal reach = 0;
    };

    struct EnemyRules
    {
        qreal detectDistance = 0;
        qreal disengageDistance = 0; ///< 与索敌距离分开，避免反复切换状态。
        qreal patrolSpeed = 0;
        qreal chaseSpeed = 0;
        qreal attackRange = 0;
        int attackDamage = 0;
        int attackCooldownFrames = 0;
    };

    enum class SpawnKind
    {
        Enemy,
        Trap,
        Checkpoint,
        Pickup,
        Door,
        Exit
    };

    /// 只描述初始配置，按 kind 读取对应字段，不承担运行期行为。
    struct Spawn
    {
        int id = 0;
        SpawnKind kind = SpawnKind::Enemy;
        QRectF bounds;
        QString subtype; ///< 类别内细分标记，例如拾取物奖励种类。
        int health = 0; ///< 敌人使用。
        int damage = 0;
        int periodFrames = 0; ///< 陷阱使用；0 表示持续危险区。
        int activeFrames = 0;
        int offsetFrames = 0;
        int damageIntervalFrames = 0; ///< 同一陷阱再次伤害的最小间隔。
        int knockback = 0;
        int requiredKeyId = 0; ///< 门/出口使用，0 表示无钥匙条件。
        int requiredEnemyId = 0; ///< 门/出口使用，0 表示无指定敌人条件。
        int requiredGoalId = 0; ///< 门/出口使用，0 表示无指定目标条件。
        int targetLevelId = 0; ///< 出口使用，0 表示没有下一关。
        QPointF respawnPosition; ///< 检查点复活位置。
        int keyItemId = 0; ///< 钥匙奖励使用。
        int amount = 0; ///< 生命/能量奖励数量。
        quint32 abilityMask = 0; ///< 能力奖励使用。
        bool requiresInteraction = false;
    };

    int levelId = 0;
    QString name;
    QString subtitle;
    int nextLevelId = 0; ///< 0 表示没有下一关。
    QRectF bounds; ///< 关卡范围及相机边界。
    QPointF spawnPoint; ///< 默认出生左上角坐标。
    QList<QRectF> solidBounds; ///< 静态平台和墙体，不包括门的运行期状态。
    QList<Spawn> spawns;
    MotionRules motion;
    AttackRules attack;
    EnemyRules enemy;
};

} // namespace QtKnight
