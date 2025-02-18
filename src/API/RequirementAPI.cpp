#include <string>
#include "Utils/RequirementUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "RequirementAPI.hpp"

namespace PinkCore::RequirementAPI
{
	std::vector<std::string> disablingIDs;

	bool RegisterInstalled(std::string identifier)
	{
		return RequirementUtils::ExternalAPI::RegisterInstalled(identifier);
	}

	bool RemoveInstalled(std::string identifier)
	{
		return RequirementUtils::ExternalAPI::RemoveInstalled(identifier);
	}

	bool RegisterAsSuggestion(std::string identifier)
	{
		return RequirementUtils::ExternalAPI::RegisterAsSuggestion(identifier);
	}

	bool RemoveSuggestion(std::string identifier)
	{
		return RequirementUtils::ExternalAPI::RemoveSuggestion(identifier);
	}

	bool GetCurrentlySelectedCustomData(rapidjson::Value& value)
	{
		return SongUtils::CustomData::GetCurrentCustomData(SongUtils::GetCurrentInfoDat(), value);
	}

	void DisablePlayButton(const ModInfo& info)
	{
		RequirementUtils::ExternalAPI::RegisterDisablingModId(info.id);
	}

	void EnablePlayButton(const ModInfo& info)
	{
		RequirementUtils::ExternalAPI::RemoveDisablingModId(info.id);
	}
}
