#pragma once

#include "KnightContracts.h"

namespace knight {

// 解析期间使用的未验证数据，不允许作为公开接口返回。
struct LevelDraft {
    LevelDefinition partial;
    std::vector<ResourceId> unresolvedRefs;
    std::string sourcePathUtf8;
    int errorLine = 0;
    int errorColumn = 0;
};

} // namespace knight
