#pragma once
#include "InputManager.h"
#include "Hand.h"
#include "Setting.h"
#include <vector>
using namespace std;

enum FingerType { THUMB = 0, INDEX, MIDDLE, RING, PINKY };

struct FingerData{
	FingerType finger;
	int hand_num = -1;
	unsigned long long int time;
	bool press;
	bool hand_begin;

	bool operator <(FingerData& fd) {
		return this->time < fd.time;
	}
};

class Animation {
public:
	enum Transition { EASE_IN, EASE_OUT, EASE_INOUT, LINEAR };
	struct Data {
		Transition transition;
		unsigned long long int begin_time, end_time;
		double start_value, end_value;
	};

	Animation();
	Animation(double* target, double start, double end, unsigned long long int begin_time, unsigned long long int end_time, Transition transition);
	bool Update(unsigned long long int time);
	bool IsEnd();

	Data Get();
	Data* Set();

private:
	Data data;
	bool finish;
	double* target;

	double EaseIn(int progress, double start, double end);
	double EaseOut(int progress, double start, double end);
	double EaseInOut(int progress, double start, double end);
	double Linear(int progress, double start, double end);
};

struct AdofaiFinger {
	Finger* finger;
	vector<Animation> animation;
};

struct AdofaiHand {
	Hand* hand;
	AdofaiFinger finger[5];
	vector<Animation> zoom_animation;
	vector<Animation> updown_animation;
	vector<Animation> leftright_animation;
	vector<Animation> x_offset_animation;
	double x_offset = 0;
};

struct EffectData {
	int x;
	int y;
	double progress;
	int hand_num = 0;
};

class HandCam {
private:
	AdofaiHand hand[MaxHandNumber];
	vector<FingerData> input_list;
	vector<EffectData> effect_list;

public:
	void Init(InputManager input, double pitch);
	void Show();
};

