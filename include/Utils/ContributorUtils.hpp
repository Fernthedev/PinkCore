#pragma once
#include "Contributor.hpp"
#include <vector>
#include "LevelDetailAPI.hpp"
namespace PinkCore::UI {
	class ContributorHandler;
}

namespace ContributorUtils
{
	/// @brief fetches the internal list of contributors and stores it internally
	void FetchListOfContributors(PinkCore::API::LevelDetails& levelDetail);

	/// @brief clears the contributor list
	void EmptyContributors();

	/// @brief Gets whether the passed contributor is a contributor for the current song
	/// @param contributor the contributor to check against
	/// @return true for is current, false for not
	bool GetIsCurrentContributor(PinkCore::Contributor& contributor);

	/*
	/// @brief handles the updating of the contributorHandler
	/// @param contributorHandler the handler to update
	/// @param firstUpdate whether or not this is the first time updating (right after creation)
	void UpdateContributorHandler(PinkCore::UI::ContributorHandler* contributorHandler, bool firstUpdate);
	*/
	/// @brief Whether or not there are contributors, true for yes, false for no
	bool DidAnyoneWorkOnThis();
}