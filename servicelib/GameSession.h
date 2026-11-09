#pragma once

#include "datamanagelib/EnemyObject.h"
#include "datamanagelib/GameMap.h"
#include "datamanagelib/InteractionObject.h"
#include "datamanagelib/DoorObject.h"
#include "datamanagelib/PlayerObject.h"
#include "datamanagelib/SaveSlot.h"
#include "datamanagelib/TrapObject.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QString>

namespace QtKnight
{

/// 游戏业务入口，持有运行期对象并协调规则、关卡、存档与页面状态。
class GameSession
{
public:
    enum class State
    {
        MainMenu,
        LevelSelection,
        Loading,
        Playing,
        Paused,
        Dead,
        LevelComplete
    };

    enum class Action
    {
        MoveLeft,
        MoveRight,
        Jump,
        Attack,
        Interact,
        Pause
    };

    enum class Feedback
    {
        Jump,
        Attack,
        Hit,
        Pickup,
        Checkpoint,
        Door,
        Death,
        LevelComplete,
        SaveSuccess,
        SaveFailure
    };

    struct World
    {
        GameMap map;
        PlayerObject player;
        QList<EnemyObject> enemies;
        QList<TrapObject> traps;
        QList<InteractionObject> interactions;
        QList<DoorObject> doors; ///< 包含门与出口。
        quint64 frameNumber = 0;
        QString message; ///< 本次玩法结果的提示，不决定物理或战斗规则。
    };

    struct LevelChoice
    {
        int levelId = 0;
        QString name;
        QString subtitle;
        bool unlocked = false;
        bool completed = false;
    };

    static constexpr qreal kFixedStepSeconds = 1.0 / 60.0;
    static constexpr int kMaxCatchUpSteps = 5;

    GameSession(const QString& levelDirectory, const QString& saveDirectory);

    /// 扫描三个槽，坏槽标记 corrupted；目录级读取失败不修改输出 saveSlots。
    bool readSaveSlots(QList<SaveSlot>& saveSlots, QString& errorMessage) const;

    bool newGame(int slotNumber, QString& errorMessage); ///< 仅创建空槽，成功进入选关页。
    bool continueGame(int slotNumber, QString& errorMessage);
    bool openLevelSelection(int slotNumber, QString& errorMessage);

    /// 用户已确认继续位置将改变；复核解锁，准备和原子保存均成功后才切换世界。
    bool startSelectedLevel(int levelId, QString& errorMessage);
    bool retryCheckpoint(QString& errorMessage); ///< 使用内存中的最近检查点，包含尚未保存的进度。
    bool nextLevel(QString& errorMessage); ///< 失败保留本关完成状态，可再次尝试。

    /// 仅完成页有未保存的通关进度时重试写入。成功清除未保存标记，仍停留完成页。
    /// 写入失败保留旧档与内存进度。
    bool retryCompletionSave(QString& errorMessage);

    /// 保存失败保持会话；saveProgress 为 false 时由窗口先完成放弃确认。
    bool returnToMenu(bool saveProgress, QString& errorMessage);

    void setAction(Action action, bool pressed); ///< 记录真实状态变化，跳跃/交互取按下沿。
    void releaseAll(); ///< 失焦、暂停和切关时释放全部动作。
    void setPaused(bool paused);
    void advance(qreal elapsedSeconds); ///< 仅 Playing 推进，每次最多补 5 个固定逻辑步。

    const World& world() const; ///< 只读借用，不跨下一次切关保留对象引用。
    State state() const;
    const QList<LevelChoice>& levelChoices() const;
    bool hasUnsavedProgress() const;
    QList<Feedback> takeFeedback(); ///< 取走已确认的反馈并清空，绘图时不重复播放。

private:
    struct InputState
    {
        QSet<Action> held;
        QSet<Action> pressed;
    };

    QString levelDirectory_;
    QString saveDirectory_;
    State state_ = State::MainMenu;
    World world_;
    int slotNumber_ = 0;
    bool dirty_ = false;
    qreal accumulator_ = 0;
    InputState input_;
    QList<LevelChoice> levelChoices_;
    QList<Feedback> feedback_;
    QHash<int, quint64> trapLastDamageFrames_; ///< 无记录表示首次；仅实际扣血后更新伤害帧号。

    /// 只准备临时世界，失败不改当前会话；成功后仍需按入口完成保存与启用。
    bool prepareLevel(int levelId, const PlayerObject& progress, bool restoreCheckpoint,
                      World& prepared, QString& errorMessage) const;
    void activateLevel(World&& prepared);
    bool readSaveSlot(int slotNumber, SaveSlot& slot, QString& errorMessage) const;

    /// 原子写入当前或待启用世界的可恢复进度，失败保留旧档。
    bool writeProgress(const World& world, int slotNumber, QString& errorMessage) const;

    /// 推进计时、AI 与移动，再结算战斗；死亡则结束本步，否则结算交互。
    void step();
    void resolveCombat();
    void resolveInteractions(); ///< step 确认存活后调用，统一提交拾取、检查点与通路结果。
};

} // namespace QtKnight
