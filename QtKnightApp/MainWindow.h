#pragma once

#include "servicelib/GameSession.h"
#include "visuallib/SceneRenderer.h"

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

/// 组织页面和用户确认，调度会话推进、画布重绘与音效。
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
    SceneRenderer renderer_;
    QElapsedTimer elapsedTimer_;
    QTimer* frameTimer_ = nullptr;
    QStackedWidget* pages_ = nullptr;
    GamePlayWidget* gameInput_ = nullptr;
    QWidget* gameCanvas_ = nullptr; ///< 画布在自身的 paintEvent() 中调用 SceneRenderer。
    AudioPlayer* audio_ = nullptr;
    QLabel* messageLabel_ = nullptr;
    QListWidget* saveSlots_ = nullptr;
    QListWidget* levelList_ = nullptr;

    /// 建页面并在按钮连接处调用会话；选关开始先确认，失败提示原因后留在原页。
    void buildPages();
    void refreshPage();
    void onFrame(); ///< 推进会话，提交重绘请求，再转交音效反馈。

    bool confirmLeave(bool& saveProgress); ///< 返回 false 表示用户取消离开。
};

} // namespace QtKnight
