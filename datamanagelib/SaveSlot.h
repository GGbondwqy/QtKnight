#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>
#include <QtCore/QString>

namespace QtKnight
{

inline constexpr int kSaveSlotCount = 3;
inline constexpr int kSaveVersion = 1;

/// 一个槽位的元数据与玩家进度；只负责 JSON 转换，文件读写由会话处理。
class SaveSlot
{
public:
    int slotNumber = 0; ///< 有效槽号为 1～kSaveSlotCount，0 表示未绑定。
    int levelId = 0;
    QDateTime savedAt;
    bool corrupted = false; ///< 槽位列表的损坏提示标记。
    QJsonObject playerData; ///< PlayerObject 导出的进度层，不包含实时位置、速度或攻击状态。

    bool isOccupied() const; ///< 有存档或坏文件即占用；已建档但未选关不等于空槽。

    QJsonObject toSaveJson() const; ///< 组合元数据、格式版本与玩家进度，不写文件。

    /// 读取完整存档 JSON；失败返回 false 并写 errorMessage，整个对象保持原值。
    bool loadFromSaveJson(const QJsonObject& saveJson, QString& errorMessage);
};

} // namespace QtKnight
