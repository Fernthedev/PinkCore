#pragma once
#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include <string>

namespace DifficultyNameUtils
{
	void SetDifficultyNameCacheFromDifficulty(GlobalNamespace::BeatmapDifficulty difficulty, std::u16string_view name);
	std::u16string GetDifficultyNameFromDoc(rapidjson::Document& d, GlobalNamespace::BeatmapDifficulty difficulty);
	std::u16string GetDifficultyNameFromCache(GlobalNamespace::BeatmapDifficulty difficulty);
	void SetDifficultyNameCacheFromArray(::ArrayW<GlobalNamespace::IDifficultyBeatmap*>& difficultyArray);

}