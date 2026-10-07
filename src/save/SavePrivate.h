#pragma once

#include "KnightContracts.h"
#include <QtSql/QSqlDatabase>

namespace knight {

// 数据库连接只存在于存档模块，公开接口只暴露 SaveHandle*。
struct SaveHandle {
    DatabasePath path;
    std::string connectionName;
    QSqlDatabase connection;
    int schemaVersion = 0;
};

} // namespace knight
