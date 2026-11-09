#pragma once

#include "servicelib/GameSession.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtMultimedia/QSoundEffect>

namespace QtKnight
{

/// 消费会话已确认的音效反馈，复用 QObject 管理声音对象生命周期。
class AudioPlayer : public QObject
{
public:
    enum class MenuSound
    {
        Confirm,
        Cancel
    };

    explicit AudioPlayer(QObject* parent = nullptr);

    /// 资源文件边界；失败写 errorMessage，保留此前已准备的音效。
    bool loadFromDirectory(const QString& directory, QString& errorMessage);
    void setVolumes(qreal masterVolume, qreal effectsVolume);
    void play(GameSession::Feedback feedback);
    void playMenuSound(MenuSound sound);

private:
    qreal masterVolume_ = 1;
    qreal effectsVolume_ = 1;
    QHash<GameSession::Feedback, QSoundEffect*> sounds_;
    QSoundEffect* menuConfirm_ = nullptr;
    QSoundEffect* menuCancel_ = nullptr;
};

} // namespace QtKnight
