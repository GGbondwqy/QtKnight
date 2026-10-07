#pragma once

#include "KnightContracts.h"

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

struct SaveHandle;

KNIGHTSAVE_API Result<SaveHandle*> openSaveStore(
    const DatabasePath& path) noexcept;
KNIGHTSAVE_API void closeSaveStore(SaveHandle* store) noexcept;
KNIGHTSAVE_API Result<SlotId> createSlot(
    SaveHandle* store, const SlotSeed& seed) noexcept;
KNIGHTSAVE_API Result<SaveData> readSlot(
    SaveHandle* store, SlotId id) noexcept;
KNIGHTSAVE_API Status writeSlot(
    SaveHandle* store, const SaveData& data) noexcept;
KNIGHTSAVE_API Result<SlotSummaryBatch> listSlots(SaveHandle* store) noexcept;

} // namespace knight
