#pragma once

#include "CharacterObject.h"

#include <QtCore/QJsonObject>
#include <QtCore/QSet>
#include <QtCore/QString>

namespace QtKnight
{

inline constexpr int kMaxHealth = 5;
inline constexpr int kMaxEnergy = 30;

/// 一次挥击的阶段，只有 Active 阶段参与命中判定。
enum class AttackPhase
{
    Idle,
    Windup,
    Active,
    Recovery
};

/// 继承公共角色状态，保存玩家专属的能量、无敌、挥击与永久进度。
class PlayerObject : public CharacterObject
{
public:
    PlayerObject(); ///< 初始碰撞体 24×46、满血，生命上限为 kMaxHealth。

    int jumpsLeft = 1;

    int energy = kMaxEnergy;
    int maxEnergy = kMaxEnergy;

    int invincibleFrames = 0; ///< 会话按固定逻辑步递减至 0，0 表示可受伤。

    // 永久进度：集合保存对象或关卡编号，不保存实时坐标和战斗状态。
    quint32 abilities = 0; ///< 已解锁能力的位集合。
    QSet<int> keyItems;
    int currentCheckpointId = 0; ///< 0 表示使用本关默认出生点。

    QSet<int> unlockedLevels;
    QSet<int> completedLevels;
    QSet<int> consumedPickupIds;
    QSet<int> openedDoorIds;
    QSet<int> completedGoalIds;
    bool gameCompleted = false;

    // 攻击计时为逻辑帧；本次命中集合防止同一挥击重复伤害。
    AttackPhase attackPhase = AttackPhase::Idle;
    int attackPhaseFrame = 0; ///< 当前阶段内的帧偏移，切换阶段时从 0 计数。
    QSet<int> attackHitTargets;

    int takeDamage(int damage) override; ///< 先判断无敌，再复用基类扣血；返回实际扣血量。

    void heal(int amount); ///< 恢复生命，不超过 maxHealth。

    bool spendEnergy(int amount); ///< 不足时返回 false，保持 energy 不变。

    /// 推进已发起的挥击；三个参数分别为起手、有效、收招的逻辑帧数。
    void advanceAttack(int windupFrames, int activeFrames, int recoveryFrames);

    void resetAttack(); ///< 回到 Idle 并清空阶段帧和命中集合，冷却独立处理。

    QJsonObject toSaveJson() const; ///< 只导出可恢复进度，不执行文件读写。

    /// 读取玩家进度层 JSON；失败返回 false 并写 errorMessage，整个对象保持原值。
    bool loadFromSaveJson(const QJsonObject& saveJson, QString& errorMessage);
};

} // namespace QtKnight
