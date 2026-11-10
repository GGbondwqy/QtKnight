#pragma once

#include "servicelib/GameSession.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QKeyEvent;
QT_END_NAMESPACE

namespace QtKnight
{

/// 处理游戏键盘事件和失焦清理，将动作状态交给 GameSession。
class GamePlayWidget : public QWidget
{
public:
    explicit GamePlayWidget(GameSession& session, QWidget* parent = nullptr);

    void setKeyBindings(const QHash<GameSession::Action, QList<int>>& bindings);
    void releaseInput(); ///< 清物理按键和会话动作，重绑及失焦时调用。

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    GameSession& session_;
    QHash<GameSession::Action, QList<int>> keyBindings_;
    QSet<int> pressedKeys_; ///< 同动作多键同时按下时，释放其中一键不误释放动作。

    void updateActions();
};

} // namespace QtKnight
