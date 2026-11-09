#pragma once

#include "servicelib/GameSession.h"
#include "visuallib/SceneRenderer.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QKeyEvent;
class QPaintEvent;
QT_END_NAMESPACE

namespace QtKnight
{

/// UI 游戏视图，处理按键与焦点，将动作状态交给 GameSession，委托 SceneRenderer 绘制。
class GamePlayWidget : public QWidget
{
public:
    GamePlayWidget(GameSession& session, QWidget* parent = nullptr);

    void setKeyBindings(const QHash<GameSession::Action, QList<int>>& bindings);
    void releaseInput(); ///< 清物理按键和会话动作，重绑及失焦时调用。

protected:
    void paintEvent(QPaintEvent* event) override; ///< 借用会话的只读世界，委托渲染器分层绘制。
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    GameSession& session_;
    SceneRenderer renderer_;
    QHash<GameSession::Action, QList<int>> keyBindings_;
    QSet<int> pressedKeys_; ///< 同动作多键同时按下时，释放其中一键不误释放动作。

    void updateActions();
};

} // namespace QtKnight
