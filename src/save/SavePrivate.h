/**
 * @file SavePrivate.h
 * @brief 存档模块独占的数据库连接结构。
 *
 * 仅供 KnightSave.dll 内部使用；公开头文件只前向声明 SaveHandle。
 * Qt Sql 依赖留在本模块，连接由 openSaveStore/closeSaveStore 管理。
 */

#pragma once

#include "KnightContracts.h"
#include <QtSql/QSqlDatabase>

namespace knight {

/// @brief 存档模块拥有的数据库句柄，其他模块不得读取连接或自行 delete。
struct SaveHandle {
    DatabasePath path; ///< 数据库文件路径副本。
    std::string connectionName; ///< 本模块管理的 Qt 数据库连接名。
    QSqlDatabase connection; ///< 私有 Qt SQL 连接，不跨 DLL 暴露。
    int schemaVersion = 0; ///< 已打开数据库的表结构版本，不等同于 SaveData 格式版本。
};

} // namespace knight
