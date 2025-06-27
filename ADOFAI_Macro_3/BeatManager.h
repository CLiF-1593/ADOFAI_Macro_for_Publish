#pragma once
#include <vector>
#include "FileManager.h"
using namespace std;

enum Hold { NONE = 0, HOLD_IN, HOLD_OUT };

struct Beat {
	double beat;
	double bpm;
	Hold hold;
	bool autoplay;
	int tile_num;
};

class BeatManager {
private:
	vector<Beat> beat_list;
public:
	void Init(FileManager &file);
	vector<Beat> GetBeatList();
	void Log();
};
