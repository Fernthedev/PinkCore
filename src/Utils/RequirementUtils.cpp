#include "Utils/RequirementUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "logging.hpp"
//#include "CustomTypes/RequirementHandler.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/Button.hpp"

using StandardLevelDetailView = GlobalNamespace::StandardLevelDetailView;

#include <algorithm>

static std::string toLower(std::string in)
{
	std::string output = "";
	for (auto& c : in)
	{
		output += tolower(c);
	}

	return output;
}

static std::string removeSpaces(std::string input)
{
	std::string output = "";
	for (auto c : input)
	{
		if (c == ' ') continue;
		output += c;
	}
	return output;
}

namespace RequirementUtils
{

	std::unordered_set<std::string> installedRequirements = {};
	std::unordered_set<std::string> forcedSuggestions = {};
    PinkCore::API::RequirementSet currentRequirements = {};
    PinkCore::API::RequirementSet currentSuggestions = {};

	std::unordered_set<std::string> disablingModIds = {};
	
	FoundRequirementsEvent onFoundRequirementsEvent;
	FoundSuggestionsEvent onFoundSuggestionsEvent;
	
	//void HandleRequirementDetails(StandardLevelDetailView* detailView)
	void HandleRequirementDetails()
	{
		if (installedRequirements.size() == 0) FindInstalledRequirements();
		currentRequirements.clear();
		currentSuggestions.clear();
		
		bool hasNoodle = false;

		// if custom
		if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && SongUtils::SongInfo::get_currentInfoDatValid())
		{
			auto& doc = SongUtils::GetCurrentInfoDat();
			//INFO("handling requirements for %s", doc["_songName"].GetString());
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			// get the custom data, if it exists
			if (SongUtils::CustomData::GetCurrentCustomData(doc, customData))
			{
				INFO("There was custom data!");
				// there was custom data
				auto requirementsArray = customData.FindMember(u"_requirements");
				if (requirementsArray != customData.MemberEnd())
				{
					INFO("Extracting Requirements");
					SongUtils::CustomData::ExtractRequirements(requirementsArray->value, currentRequirements);
				}

				auto suggestionsArray = customData.FindMember(u"_suggestions");
				if (suggestionsArray != customData.MemberEnd())
				{
					INFO("Extracting Suggestions");
					SongUtils::CustomData::ExtractRequirements(suggestionsArray->value, currentSuggestions);
				}

				for (auto req : currentRequirements)
				{
					INFO("ReqName: %s", req.c_str());

					if (req.find("Noodle Extensions") != std::string::npos)
					{
						hasNoodle = true;
						break;
					}
				}
			}
			else
			{
				// there was no custom data
				INFO("There was no custom data!");
			}
		}

		SongUtils::SongInfo::set_currentlySelectedIsNoodle(hasNoodle);


	}

	bool AllowPlayerToStart()
	{
		if (disablingModIds.size() > 0) return false;
		if (!SongUtils::SongInfo::get_currentlySelectedIsCustom()) return true;
		// for every required requirement
		for (auto req : currentRequirements)
		{
			// if any is not installed, return false
			if (!GetRequirementInstalled(req) && !GetIsForcedSuggestion(req)) return false;
		}
		// all requirements were installed, we can start!
		return true;
	}

	bool IsAnythingNeeded()
	{
		return currentRequirements.size() || currentSuggestions.size();
	}

	bool IsAnythingMissing()
	{
		// if the player is not allowed to start
		// a requirement must be missing
		if (!AllowPlayerToStart()) return true;

		// for every suggested suggestion
		for (auto sug : currentSuggestions)
		{
			// if any is not installed, return false
			if (!GetRequirementInstalled(sug)) return true;
		}

		// all requirements/suggestions were installed, nothing is missing!
		return false;
	}

	bool GetRequirementInstalled(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto req : installedRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				INFO("Match installed %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetIsForcedSuggestion(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : forcedSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				INFO("Match requirement %s, %s", sug.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasRequirement(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto req : currentRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				INFO("Match requirement %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasSuggestion(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : currentSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				INFO("Match Suggestion %s, %s", sug.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	void FindInstalledRequirements()
	{
		return;
		for (auto mod : Modloader::getMods())
		{
			// if mod is loaded, put it in the list of installed requirements
			if (mod.second.get_loaded())
			{
				installedRequirements.emplace(mod.second.info.id);
				INFO("Found loaded id: %s", mod.second.info.id.c_str());
			}
		}
	}

	const PinkCore::API::RequirementSet& GetCurrentRequirements()
	{
		return currentRequirements;
	}

	const PinkCore::API::RequirementSet& GetCurrentSuggestions()
	{
		return currentSuggestions;
	}

	/*
	void UpdateRequirementHandler(PinkCore::UI::RequirementHandler* handler, bool firstUpdate)
	{
		INFO("Handler ptr: %p", handler);
		if (!handler) return;
		for (auto req : currentRequirements)
		{
			handler->AddID(req);
		}

		for (auto sug : currentSuggestions)
		{
			handler->AddID(sug);
		}

		if (!firstUpdate) handler->CheckAllRequirements();
	}
	*/
	void UpdatePlayButton()
	{
		auto levelViews = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailView*>();
		int length = levelViews.Length();
		if (length > 0)
		{
			bool interactable = AllowPlayerToStart();
                        bool isCustom = SongUtils::SongInfo::get_currentlySelectedIsCustom();
                        bool isWip = SongUtils::SongInfo::get_currentlySelectedIsWIP();
			INFO("interactable: %d, custom: %d, wip: %d", interactable, isCustom, isWip);
                        if (isCustom && isWip)
			{
				levelViews[length - 1]->get_practiceButton()->set_interactable(interactable);
				levelViews[length - 1]->get_actionButton()->set_interactable(false);
			}
			else
			{
				levelViews[length - 1]->get_practiceButton()->set_interactable(interactable);
				levelViews[length - 1]->get_actionButton()->set_interactable(interactable);
			}
		}
	}
	namespace ExternalAPI
	{
		bool RegisterInstalled(std::string identifier)
		{
            return installedRequirements.emplace(identifier).second;
		}

		bool RemoveInstalled(std::string identifier)
		{
            auto it = installedRequirements.find(identifier);

            bool existed = it != installedRequirements.end();

            if (existed) {
                installedRequirements.erase(it);
            }

            return existed;
		}

		bool RegisterAsSuggestion(std::string identifier)
		{
            return forcedSuggestions.emplace(identifier).second;
		}
		
		bool RemoveSuggestion(std::string identifier)
		{
            auto it = forcedSuggestions.find(identifier);

            bool existed = it != forcedSuggestions.end();

            if (existed) {
                forcedSuggestions.erase(it);
            }

            return existed;
		}

		

		void RegisterDisablingModId(std::string id)
		{
            bool existed = disablingModIds.emplace(id).second;

			if (existed)
			{
				INFO("Mod %s is trying to disable the play button again!", id.c_str());
				return;
			}

            INFO("Mod %s is disabling the play button!", id.c_str());
			UpdatePlayButton();
		}

		void RemoveDisablingModId(std::string id)
		{
            auto it = disablingModIds.find(id);

            bool existed = it != disablingModIds.end();

            if (!existed) return;

            INFO("Mod %s is no longer disabling the play button", id.c_str());
            disablingModIds.erase(it);
            UpdatePlayButton();
        }
	}

	FoundRequirementsEvent& onFoundRequirements() {
		return onFoundRequirementsEvent;
	}

	FoundSuggestionsEvent& onFoundSuggestions() {
		return onFoundSuggestionsEvent;
	}
}
