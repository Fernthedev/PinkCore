#include "Hooks.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapData_-get_beatmapObjectsData-d__31.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor.hpp"

#include <map>

using namespace GlobalNamespace;

extern Logger& getLogger();

std::optional<int> addBeatmapObjectDataLineIndex;
MAKE_HOOK_MATCH(BeatmapData_AddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, void,
				BeatmapData *self, BeatmapObjectData *item) {
	addBeatmapObjectDataLineIndex = item->lineIndex;
	// Preprocess the lineIndex to be 0-3 (the real method is hard-coded to 4
	// lines), recording the info needed to reverse it
	if (addBeatmapObjectDataLineIndex > 3) {
		item->lineIndex = 3;
	} else if (addBeatmapObjectDataLineIndex < 0) {
		item->lineIndex = 0;
	}

	BeatmapData_AddBeatmapObjectData(self, item);
}

MAKE_HOOK_MATCH(BeatmapLineData_AddBeatmapObjectData, &BeatmapLineData::AddBeatmapObjectData, void,
				BeatmapLineData *self, BeatmapObjectData *item) {
	if (addBeatmapObjectDataLineIndex) {
		item->lineIndex = addBeatmapObjectDataLineIndex.value();
	}
	addBeatmapObjectDataLineIndex = std::nullopt;
	BeatmapLineData_AddBeatmapObjectData(self, item);
}

MAKE_HOOK_MATCH(NoteProcessorClampPatch, &BeatmapObjectsInTimeRowProcessor::ProcessAllNotesInTimeRow, void,
				BeatmapObjectsInTimeRowProcessor *self, List<NoteData *> *notesInTimeRow) {
	if (!notesInTimeRow->get_Count())
	{
		NoteProcessorClampPatch(self, notesInTimeRow);
		return;
	}
	// save all the original information
	std::vector<int> extendedLanes(notesInTimeRow->size);
	int idx = 0;
	for (auto& lane : extendedLanes)
	{
		auto *item = notesInTimeRow->items->values[idx];
		lane = item->lineIndex;
		item->lineIndex = std::max(std::min(item->lineIndex, 3), 0);
		idx++;
	}

	// NotesInTimeRowProcessor_ProcessAllNotesInTimeRow(self, notesInTimeRow);
	// Instead, we have a reimplementation of the hooked method to deal with precision
	// noteLineLayers:
	int columnsLength = self->notesInColumns->Length();
	for (il2cpp_array_size_t i = 0; i < self->notesInColumns->Length(); i++) {
		self->notesInColumns->values[i]->Clear();
	}
	
	for (int j = 0; j < notesInTimeRow->get_Count(); j++) {
		auto* noteData = notesInTimeRow->items->values[j];
		auto* list = self->notesInColumns->values[noteData->lineIndex];

		bool flag = false;
		for (int k = 0; k < list->get_Count(); k++) {
			if (list->items->values[k]->noteLineLayer.value > noteData->noteLineLayer.value) {
				list->Insert(k, noteData);
				flag = true;
				break;
			}
		}
		if (!flag) {
			list->Add(noteData);
		}
	}
	for (il2cpp_array_size_t l = 0; l < self->notesInColumns->Length(); l++) {
		auto *list2 = self->notesInColumns->values[l];
		for (int m = 0; m < list2->get_Count(); m++) {
			auto *note = list2->items->values[m];
			if (note->noteLineLayer.value >= 0 && note->noteLineLayer.value <= 2) {
				note->SetBeforeJumpNoteLineLayer(m);
			}
		}
	}
	idx = 0;
	for (auto& lane : extendedLanes)
	{
		notesInTimeRow->items->values[idx]->lineIndex = lane;
		idx++;
	}

	notesInTimeRow->Clear();
}


MAKE_HOOK_MATCH(BeatmapObjectsDataClampPatch, &BeatmapData::$get_beatmapObjectsData$d__31::MoveNext,
				bool, BeatmapData::$get_beatmapObjectsData$d__31 *self) {
	int num = self->$$1__state;
	BeatmapData *beatmapData = self->$$4__this;
	if (num != 0) {
		if (num != 1) {
			return false;
		}
		self->$$1__state = -1;
		// Increment index in idxs with clamped lineIndex
		int lineIndex = self->$minBeatmapObjectData$5__4->lineIndex;
		int clampedLineIndex = std::clamp(lineIndex, 0, 3);
		self->$idxs$5__3->values[clampedLineIndex]++;
		self->$minBeatmapObjectData$5__4 = nullptr;
	} else {
		self->$$1__state = -1;
		auto *arr =
			reinterpret_cast<Array<BeatmapLineData *> *>(beatmapData->get_beatmapLinesData());
		self->$beatmapLinesData$5__2 = arr;
		self->$idxs$5__3 = Array<int>::NewLength(self->$beatmapLinesData$5__2->Length());
	}
	self->$minBeatmapObjectData$5__4 = nullptr;
	float num2 = std::numeric_limits<float>::max();
	for (int i = 0; i < self->$beatmapLinesData$5__2->Length(); i++) {
		int idx = self->$idxs$5__3->values[i];
		BeatmapLineData *lineData = self->$beatmapLinesData$5__2->values[i];
		if (idx < lineData->beatmapObjectsData->get_Count()) {
			BeatmapObjectData *beatmapObjectData = lineData->beatmapObjectsData->get_Item(idx);
			float time = beatmapObjectData->time;
			if (time < num2) {
				num2 = time;
				self->$minBeatmapObjectData$5__4 = beatmapObjectData;
			}
		}
	}
	if (self->$minBeatmapObjectData$5__4 == nullptr) {
		return false;
	}
	self->$$2__current = self->$minBeatmapObjectData$5__4;
	self->$$1__state = 1;
	return true;
}

void InstallClampHooks(Logger &logger) {
	SIMPLE_INSTALL_HOOK(BeatmapObjectsDataClampPatch);
	SIMPLE_INSTALL_HOOK_ORIG(NoteProcessorClampPatch);
	SIMPLE_INSTALL_HOOK(BeatmapData_AddBeatmapObjectData);
	SIMPLE_INSTALL_HOOK(BeatmapLineData_AddBeatmapObjectData);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallClampHooks)