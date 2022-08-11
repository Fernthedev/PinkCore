#pragma once
#include <string>

typedef struct config_t {
	bool enableExtraSongDetails = true;
	bool forceNoteColours = true;
	bool enableCustomSongColours = true;
	bool enableCustomDiffNames = true;
	bool enableCustomCharacteristics = true;
	bool enableBurnMarks = true;
	bool openToCustomLevels = true;
} config_t;

extern config_t config;

bool LoadConfig();
void SaveConfig();
