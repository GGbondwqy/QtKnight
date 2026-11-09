#pragma once

#include "servicelib/GameSession.h"

#include <QtCore/QElapsedTimer>
#include <QtCore/QString>
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
class QCloseEvent;
class QEvent;
class QLabel;
class QListWidget;
class QStackedWidget;
class QTimer;
QT_END_NAMESPACE

namespace QtKnight
{

class AudioPlayer;
class GamePlayWidget;

/// 页面、计时与用户确认的入口；玩法只调用 GameSession，窗口行为复用 QMainWindow。
class MainWindow : public QMainWindow
{
public:
    MainWindow(const QString& contentDirectory, const QString& userDataDirectory,
               QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override; ///< 窗口失焦时清输入并暂停。

private:
    GameSession session_;
    QElapsedTimer elapsedTimer_;
    QTimer* frameTimer_ = nullptr;
    QStackedWidget* pages_ = nullptr;
    GamePlayWidget* gameView_ = nullptr;
    AudioPlayer* audio_ = nullptr;
    QLabel* messageLabel_ = nullptr;
    QListWidget* saveSlots_ = nullptr;
    QListWidget* levelList_ = nullptr;

    /// 建页面并在按钮连接处调用会话；选关开始先确认，失败提示原因后留在原页。
    void buildPages();
    void refreshPage();
    void onFrame(); ///< 推进会话后刷新画面并消费反馈，不在 paintEvent 推进逻辑。

    bool confirmLeave(bool& saveProgress); ///< 返回 false 表示用户取消离开。
};

} // namespace QtKnight
