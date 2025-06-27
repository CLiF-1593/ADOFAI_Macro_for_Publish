#pragma once
#include <vector>
#include "BeatManager.h"

using namespace std;

struct Input {
	double push_time, release_time;
	int finger;
	int hand;
	double pitch;
	bool begin = false;
};

struct KeyData {
	double time;
	int key_code;
};

class InputManager {
private:
	vector<Input> input_list;
	vector<Input> sound_list;

	void ChangeHand(int index, int hand);
	void ArrangeHand();
	void SetPositive(int& hand);

	int cur_hand_number = 0;
	double cur_key_offset = 0;
	void UpdateHandNumber(int tile);

public:
	void Init();
	void SetInput(FileManager &file, BeatManager &beat);
	queue<KeyData> GetPressList(double offset, double pitch);
	queue<KeyData> GetReleaseList(double offset, double pitch);
	queue<KeyData> GetSoundEffectList(double offset, double pitch);
	vector<KeyData> GetKeyTable();
	vector<Input> GetInputData();
	void Log();
};

