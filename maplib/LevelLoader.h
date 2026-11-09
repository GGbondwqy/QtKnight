#pragma once

#include "datamanagelib/GameMap.h"

#include <QtCore/QList>
#include <QtCore/QString>

namespace QtKnight
{

/// 读取并校验关卡配置，只生成 GameMap，不创建角色或地图对象实例。
class LevelLoader
{
public:
    struct LevelInfo
    {
        int levelId = 0;
        QString name;
        QString subtitle;
        QString filePath;
    };

    LevelLoader() = delete;

    /// 在文件边界完成 JSON 解析与校验；失败写 errorMessage，输出 map 保持原值。
    static bool loadFromFile(const QString& path, GameMap& map, QString& errorMessage);

    /// 复用加载校验读取选关摘要，额外检查关卡编号冲突；失败不修改 levels。
    static bool listLevels(const QString& directory, QList<LevelInfo>& levels,
                           QString& errorMessage);

};

} // namespace QtKnight
