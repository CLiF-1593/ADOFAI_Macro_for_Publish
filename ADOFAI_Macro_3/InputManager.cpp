#include "InputManager.h"
#include "Setting.h"
#include <iostream>
#include <iomanip>
#include <random>

extern int midspin_delta[sizeof(hand_number_list) / sizeof(HandPair)];


void InputManager::ChangeHand(int index, int hand) {
	int change = hand - this->input_list[index].hand;
	for (int i = index; i < this->input_list.size(); i++) {
		this->input_list[i].hand += change;
	}
}

void InputManager::SetPositive(int& hand) {
	while (hand < 0) {
		hand += cur_hand_number;
	}
}

void InputManager::ArrangeHand() {
	for (int i = 0; i < this->input_list.size(); i++) {
		SetPositive(this->input_list[i].hand);
		this->input_list[i].hand %= cur_hand_number;
	}
}

void InputManager::UpdateHandNumber(int tile) {
	for (int i = 0; i < sizeof(hand_number_list) / sizeof(HandPair); i++) {
		if (hand_number_list[i].tile - midspin_delta[i] >= tile || hand_number_list[i].tile == -1) {
			cur_hand_number = hand_number_list[i].num;
			cur_key_offset = hand_number_list[i].key_offset;
			break;
		}
	}
}

void InputManager::Init() {
	this->input_list.clear();
}

double SetHandBPM(double bpm, double abs_bpm, double prev_bpm) {
	if (!bpm)
		return OneHandBPM;
	while (bpm > OneHandBPM) {
		bpm /= 2.0;
	}
	while (bpm < OneHandBPM) {
		bpm *= 2.0;
	}
	if (bpm - OneHandBPM > OneHandBPM - bpm / 2.0) {
		bpm /= 2.0;
	}

	if (!abs_bpm)
		return OneHandBPM;
	while (abs_bpm > OneHandBPM) {
		abs_bpm /= 2.0;
	}
	while (abs_bpm < OneHandBPM) {
		abs_bpm *= 2.0;
	}
	if (abs_bpm - OneHandBPM > OneHandBPM - abs_bpm / 2.0) {
		abs_bpm /= 2.0;
	}

	if (prev_bpm == 0) {
		bpm = abs_bpm;
	}

	prev_bpm -= 20;
	if (abs(abs_bpm - prev_bpm) < abs(bpm - prev_bpm)) {
		bpm = prev_bpm;
	}

	bpm += 20;
	return bpm;
}

bool InBinary(double num1, double num2) {
	if (num1 > num2) {
		auto t = num1;
		num1 = num2;
		num2 = num1;
	}

	while (num1 < num2 * 4.0) {
		if (abs(num1 - num2) < 0.001) {
			return true;
		}
		num1 *= 2.0;
	}
	return false;
}

void InputManager::SetInput(FileManager &file, BeatManager &beat) {
	auto beat_list = beat.GetBeatList();
	
	Input input;
	input.hand = 0;
	input.finger = 0;
	input.push_time = 0;
	input.release_time = -1;
	input.pitch = 1;

	double cur_time = 0;

	//Push Time Allocation
	for (int i = 0; i < beat_list.size(); i++) {
		cur_time += beat_list[i].beat * BPM_to_Time(beat_list[i].bpm);
		if (beat_list[i].autoplay) {
			continue;
		}
		if (beat_list[i].hold == HOLD_OUT) {
			continue;
		}
		input.push_time = cur_time;
		input.pitch = (double)OneHandBPM / beat_list[i].bpm;
		this->input_list.push_back(input);
	}

	double hand_duration = 0;
	int finger = 0;
	double hand_speed_coefficient = 1.0;
	double hand_bpm = 0;

	//Finger Allocation
	bool reset_hand_speed_coefficient = false;
	double prev_duration = 0;
	for (int i = 0; i < input_list.size() - 1; i++) {
		hand_bpm = SetHandBPM(beat_list[i + 1].bpm / beat_list[i + 1].beat, beat_list[i + 1].bpm, hand_bpm);
		double hand_speed = HandSpeed(hand_bpm) * hand_speed_coefficient;
		double duration = this->input_list[i + 1].push_time - this->input_list[i].push_time;
		if (prev_duration == 0) prev_duration = duration;
		hand_duration += duration;
		if (hand_duration < hand_speed) {
			this->input_list[i + 1].finger = ++finger;
			if (finger >= FingerNumber || (IncreaseHandSpeed && (abs(duration - prev_duration) > 25))) {
				for (int j = i + 1; this->input_list[j].finger; j--) {
					this->input_list[j].finger = 0;
				}
				i -= finger;
				hand_speed_coefficient /= 2;
				finger = 0;
				hand_duration = 0;
				prev_duration = 0;
				reset_hand_speed_coefficient = false;
				continue;
			}
		}
		else if (reset_hand_speed_coefficient) {
			hand_speed_coefficient = 1.0;
			reset_hand_speed_coefficient = false;
			hand_duration = 0;
			prev_duration = 0;
			finger = 0;
			continue;
		}
		else {
			reset_hand_speed_coefficient = true;
			hand_duration = 0;
			prev_duration = 0;
			finger = 0;
			continue;
		}
		prev_duration = duration;
	}

	auto special_min = [](double x, double y) -> double {
		y *= x > 0 ? 1 : -1;
		if (abs(x) > abs(y)) return y;
		return x;
	};

	random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> dist_err(0, StandardDeviation(ErrorRange));
	normal_distribution<double> dist_prs(0, StandardDeviation(PressRange));

	for (int i = 0; i < this->input_list.size(); i++) {
		this->input_list[i].push_time += (ErrorRange ? special_min(round(dist_err(gen)), ErrorRange) : 0);
	}

	//Release Time Allocation
	this->UpdateHandNumber(0);
	double last_time = this->input_list.back().push_time + MaxPressSpeed;
	for (int i = this->input_list.size() - 1; i >= 0; i--) {
		this->UpdateHandNumber(i);
		double interval = last_time - this->input_list[i].push_time;
		interval *= this->cur_key_offset;
		if (interval > MaxPressSpeed) {
			interval = MaxPressSpeed * pow(this->cur_key_offset, (double)this->input_list[i].finger / 2.0);
		}
		this->input_list[i].release_time = this->input_list[i].push_time + interval + (PressRange ? special_min(round(dist_prs(gen)), PressRange) : 0);
		if (!this->input_list[i].finger) {
			last_time = this->input_list[i].release_time + this->input_list[i].push_time;
			last_time /= 2;
		}
	}

	//Release Time Modification (Hold)
	bool hold_trigger = false;
	int index = -1;
	cur_time = 0;
	Input prev_input;
	prev_input.hand = -1;
	prev_input.finger = -1;
	for (int i = 0; i < beat_list.size(); i++) {
		cur_time += beat_list[i].beat * BPM_to_Time(beat_list[i].bpm);
		if (beat_list[i].autoplay) {
			continue;
		}
		if (hold_trigger) {
			hold_trigger = false;
			this->input_list[index].release_time = cur_time;
			if (this->input_list[index].finger == prev_input.finger && this->input_list[index].hand == prev_input.hand) {
				//this->input_list[index].finger++;
			}
			prev_input = this->input_list[index];
		}
		if (beat_list[i].hold == HOLD_OUT) {
			continue;
		}
		if (beat_list[i].hold == HOLD_IN) {
			hold_trigger = true;
		}
		index++;
	}

	//Hand Allocation
	#if SentenceInput
	for (int i = 0; i < input_list.size(); i++) {
		this->input_list[i].finger %= FingerNumber;
		for (int j = 0; j < HandNumber; j++) {
			if (key_table[j][0] == Sentence[i]) {
				this->input_list[i].hand = j;
				break;
			}
		}
	}
	#else
	this->UpdateHandNumber(0);
	int hand = cur_hand_number - 1;
	for (int i = 0; i < input_list.size(); i++) {
		this->input_list[i].finger %= FingerNumber;
		this->UpdateHandNumber(i);
		if (!this->input_list[i].finger) {
			#if Random
			bool random = true;
			double push_time = this->input_list[i].push_time;
			while (random) {
				random = false;
				hand = rand() % cur_hand_number;
				for (int j = i - 1; j >= 0; j--) {
					if (this->input_list[j].hand == hand) {
						if (this->input_list[j].release_time + MaxPressSpeed * 0.5 >= push_time) {
							random = true;
						}
						break;
					}
				}
			}
			#endif
			hand++;
			hand %= cur_hand_number;
		}
		
		this->input_list[i].hand = hand;
	}
	#endif

	//Fit Beat
	#if FitBeat
	index = 0;
	double cur_beat = BeginBeat;
	double cur_bpm = StandardBPM;
	bool changeable = true;
	for (int i = 1; i < beat_list.size(); i++) {
		bool chk = true;
		if (beat_list[i].autoplay || beat_list[i].hold == HOLD_OUT) {
			chk = false;
		}

		if (chk && index != -1 && !this->input_list[index].finger) {
			int beat = (int)cur_beat;
			if (abs((double)beat - cur_beat) < 0.001) {
				if (changeable || ((index + 1 < this->input_list.size() && this->input_list[index + 1].finger) && !(beat % HandNumber))) {
					//cout << beat_list[i].tile_num << " " << (beat % HandNumber) << " " << index << endl;
					int delta = (beat % HandNumber) - this->input_list[index].hand + HandNumber;
					for (int j = index; j < this->input_list.size(); j++) {
						this->input_list[j].hand += delta;
						this->input_list[j].hand %= HandNumber;
					}
				}
				else {
					changeable = true;
				}
			}
			else {
				changeable = false;
			}
		}

		if (InBinary(cur_bpm, beat_list[i].bpm) || InBinary(cur_bpm, beat_list[i].bpm / beat_list[i].beat)) {
			cur_beat += cur_bpm / (beat_list[i].bpm / beat_list[i].beat) * 0.5 * (double)HandNumber;
		}
		else {
			cur_beat += beat_list[i].beat * 0.5 * (double)HandNumber;
			double prev = cur_bpm;
			cur_bpm = beat_list[i].bpm;
			while (abs(cur_bpm - prev) > abs(cur_bpm * 2 - prev)) cur_bpm *= 2;
			while (abs(cur_bpm - prev) > abs(cur_bpm * 0.5 - prev)) cur_bpm *= 0.5;
		}
		index++;
	}
	#endif

	this->sound_list = this->input_list;

	auto not_reversed_input_list = this->input_list;

	//Out To In -> Reverse Finger
	vector<bool> trans(sizeof(style)/sizeof(InputStyle));
	auto CheckReverse = [&](int begin) {
		InputStyle input_style = InToOut;
		for (int i = 0; i < sizeof(style) / sizeof(InputStyle); i++) {
			if (style[i].num > begin || style[i].num == -1) {
				input_style = style[i].style[this->input_list[begin].hand];
				break;
			}
		}
		if (input_style == OutToIn) return true;
		if (input_style == Trans) {
			trans[this->input_list[begin].hand] = !(bool)trans[this->input_list[begin].hand];
			return (bool)trans[this->input_list[begin].hand];
		}
		return false;
	};
	int begin = 0;
	input_list[0].begin = true;
	for (int i = 0; i < input_list.size(); i++) {
		if (this->input_list[i].finger == 0) {
			if (CheckReverse(begin)) {
				for (int j = begin; j < i; j++) {
					input_list[j].finger = input_list[i - 1].finger - input_list[j].finger;
					
				}
			}
			input_list[i].begin = true;
			begin = i;
		}
	}
	if (CheckReverse(begin)) {
		for (int j = begin; j < input_list.size(); j++) {
			input_list[j].finger = input_list[input_list.size() - 1].finger - input_list[j].finger;
		}
	}

	// Thumb
	if (ThumbNumber) {
		int plus = ThumbNumber;
		bool set_plus = false;
		for (int i = not_reversed_input_list.size() - 1; i >= 0; i--) {
			int finger = not_reversed_input_list[i].finger;
			if (!set_plus && finger >= FingerNumber - ThumbNumber) {
				plus = FingerNumber - finger - 1;
				set_plus = true;
			}
			this->input_list[i].finger += plus;
			if (finger == 0) {
				plus = ThumbNumber;
				set_plus = false;
			}
		}
	}

	#if !Clear
	this->input_list.pop_back();
	#endif
}

void SetStdBPM(double& std_bpm) {
	while (std_bpm < (OneHandBPM) / 2.0) {
		std_bpm *= 2.0;
	}

	while (std_bpm > (OneHandBPM)) {
		std_bpm /= 2.0;
	}

	std_bpm += 20;
}

struct CMP {
	bool operator()(KeyData a, KeyData b) {
		return a.time > b.time;
	}
};

queue<KeyData> InputManager::GetPressList(double offset, double pitch) {
	priority_queue<KeyData, vector<KeyData>, CMP> q;
	for (int i = 0; i < this->input_list.size(); i++) {
		KeyData kd;
		kd.time = (this->input_list[i].push_time - offset * pow(this->input_list[i].pitch, 0.1)) * 1e6 / pitch;
		kd.key_code = key_table[this->input_list[i].hand][this->input_list[i].finger];
		if (MouseMode) {
			if (kd.key_code == 'L') {
				kd.key_code = /*MOUSEEVENTF_LEFTUP | */MOUSEEVENTF_LEFTDOWN;
			}
			else {
				kd.key_code = /*MOUSEEVENTF_RIGHTUP | */MOUSEEVENTF_RIGHTDOWN;
			}
		}
		q.push(kd);
	}
	queue<KeyData> ret;
	while (!q.empty()) {
		ret.push(q.top());
		q.pop();
	}
	return ret;
}


queue<KeyData> InputManager::GetReleaseList(double offset, double pitch) {
	priority_queue<KeyData, vector<KeyData>, CMP> q;
	for (int i = 0; i < this->input_list.size(); i++) {
		KeyData kd;
		kd.time = (this->input_list[i].release_time - offset * pow(this->input_list[i].pitch, 0.1)) * 1e6 / pitch;
		kd.key_code = key_table[this->input_list[i].hand][this->input_list[i].finger];
		if (MouseMode) {
			if (kd.key_code == 'L') {
				kd.key_code = MOUSEEVENTF_LEFTUP;
			}
			else {
				kd.key_code = MOUSEEVENTF_RIGHTUP;
			}
		}
		q.push(kd);
	}
	queue<KeyData> ret;
	while (!q.empty()) {
		ret.push(q.top());
		q.pop();
	}
	return ret;
}

queue<KeyData> InputManager::GetSoundEffectList(double offset, double pitch) {
	priority_queue<KeyData, vector<KeyData>, CMP> q;
	int time = 0;
	for (int i = 0; i < this->sound_list.size(); i++) {
		KeyData kd;
		if (this->sound_list[i].finger) {
			kd.time = (this->sound_list[i].push_time - offset - time * this->sound_list[i].finger / ((double)FingerNumber * 1.1)) * 1e6 / pitch;
		}
		else {
			time = (this->sound_list[i].release_time - this->sound_list[i].push_time);
			kd.time = (this->sound_list[i].push_time - offset) * 1e6 / pitch;
		}
		kd.key_code = this->sound_list[i].finger + FingerNumber;
		for (int j = i; j < this->sound_list.size() && this->sound_list[j].finger; j++) {
			kd.key_code--;
		}
		q.push(kd);
	}
	queue<KeyData> ret;
	while (!q.empty()) {
		ret.push(q.top());
		q.pop();
	}
	return ret;
}

vector<KeyData> InputManager::GetKeyTable() {
	vector<KeyData> data;
	KeyData kd;
	kd.time = 0;
	for (int i = 0; i < MaxHandNumber; i++) {
		for (int j = 0; j < FingerNumber; j++) {
			kd.key_code = key_table[i][j];
			data.push_back(kd);
		}
	}
	return data;
}

vector<Input> InputManager::GetInputData() {
	return this->input_list;
}

void InputManager::Log() {
	cout << "[Log] InputManager" << endl;
	cout << "beat_list (type=vector<Input>)" << endl;
	cout << "data = ";
	cout << "["<<endl;
	for (int i = 0; i < this->input_list.size(); i++) {
		cout << "\t(" << input_list[i].push_time << ", " << input_list[i].release_time << ", " << input_list[i].hand << ", " << input_list[i].finger << ")";
		if (i != this->input_list.size() - 1) {
			cout << ", ";
		}
		cout << endl;
	}
	cout << "]";
	cout << endl;
}