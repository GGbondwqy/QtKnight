/**
 * @file KnightSave.h
 * @brief 存档数据库与进度槽位的公开接口。
 *
 * 当前函数仅有声明，注释说明目标约定；数据库连接和 SQL 不向业务层暴露。
 * 写入数据来自已提交世界，失败须事务回滚并保留旧存档。
 * Result 的 value 仅在状态为 Ok 时有效；异常须在本 DLL 内转换为错误状态。
 */

#pragma once

#include "KnightContracts.h"

// 编译本 DLL 时定义 KNIGHTSAVE_BUILD 以导出；调用方导入，非 Windows 平台宏为空。
#if defined(_WIN32)
#  if defined(KNIGHTSAVE_BUILD)
#    define KNIGHTSAVE_API __declspec(dllexport)
#  else
#    define KNIGHTSAVE_API __declspec(dllimport)
#  endif
#else
#  define KNIGHTSAVE_API
#endif

namespace knight {

/// @brief 存档模块拥有的数据库句柄，其他模块不得读取连接或自行 delete。
struct SaveHandle;

/**
 * @brief 创建或打开当前用户的存档数据库。
 * @details 按约定创建或迁移受支持的表结构；句柄由 closeSaveStore 释放。
 * @param path 数据库文件的 UTF-8 路径。
 * @return 成功返回私有连接句柄；失败返回状态。
 */
KNIGHTSAVE_API Result<SaveHandle*> openSaveStore(
    const DatabasePath& path) noexcept;

/**
 * @brief 关闭存档连接并释放私有句柄。
 * @param store 本模块创建的存档句柄，可为 nullptr；释放后失效。
 */
KNIGHTSAVE_API void closeSaveStore(SaveHandle* store) noexcept;

/**
 * @brief 以事务创建新存档槽位及其初始进度。
 * @param store 有效存档连接句柄。
 * @param seed 候选首关已准备好后生成的初始进度。
 * @return 成功返回新槽位编号；失败事务回滚。
 */
KNIGHTSAVE_API Result<SlotId> createSlot(
    SaveHandle* store, const SlotSeed& seed) noexcept;

/**
 * @brief 读取并校验指定槽位的完整持久进度。
 * @param store 有效存档连接句柄。
 * @param id 用户选择的有效槽位编号。
 * @return 成功返回独立 SaveData 副本；不存在、格式或版本错误由状态报告。
 */
KNIGHTSAVE_API Result<SaveData> readSlot(
    SaveHandle* store, SlotId id) noexcept;

/**
 * @brief 以单笔事务写入完整持久进度。
 * @details 失败不撤销已生效的内存进度，由协调器提示用户重试。
 * @param store 有效存档连接句柄。
 * @param data 由已提交世界生成的进度副本，含目标槽位和版本。
 * @return 成功表示整笔提交；失败回滚并保留旧存档。
 */
KNIGHTSAVE_API Status writeSlot(
    SaveHandle* store, const SaveData& data) noexcept;

/**
 * @brief 读取主菜单所需的全部槽位摘要。
 * @param store 有效存档连接句柄。
 * @return 成功返回摘要列表；没有存档时返回空列表。
 */
KNIGHTSAVE_API Result<SlotSummaryBatch> listSlots(SaveHandle* store) noexcept;

} // namespace knight
