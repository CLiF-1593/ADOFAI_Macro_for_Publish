#include "HandCam.h"
#include <iostream>
#include <chrono>
#include <algorithm>
using namespace chrono;

FingerData GetFingerData(Input input, bool press, double pitch) {
	FingerData fd;
	input.finger -= ThumbNumber - 1;
	if (input.finger < 0) input.finger = 0;
	fd.finger = (FingerType)input.finger;
	fd.hand_num = input.hand;
	fd.time = 5000000000; // 5 sec
	if (press) fd.time += input.push_time * 1e6 / pitch;
	else fd.time += input.release_time * 1e6 / pitch;
	fd.hand_begin = input.begin;
	fd.press = press;
	return fd;
}

double* GetAngleData(Hand* hand, FingerType finger) {
	switch (finger) {
	case THUMB:
		return &(hand->SetAngle()->thumb_base);
	case INDEX:
		return &(hand->SetAngle()->index_begin);
	case MIDDLE:
		return &(hand->SetAngle()->middle_begin);
	case RING:
		return &(hand->SetAngle()->ring_begin);
	case PINKY:
		return &(hand->SetAngle()->pinky_begin);
	}
	return nullptr;
}

void HandCam::Init(InputManager input, double pitch) {
	// Set Finger List (Press / Release Time List)
	auto input_list = input.GetInputData();
	vector<FingerData> finger_list;
	vector<FingerData> press_list;
	vector<FingerData> release_list;
	for (int i = 0; i < input_list.size(); i++) {

		input_list[i].hand = MaxHandNumber / 2 + (input_list[i].hand + 1) / 2 * (input_list[i].hand % 2 ? -1 : 1);
		input_list[i].hand = MaxHandNumber - input_list[i].hand - 1;

		auto press = GetFingerData(input_list[i], true, pitch);
		auto release = GetFingerData(input_list[i], false, pitch);
		finger_list.push_back(press);
		finger_list.push_back(release);
		press_list.push_back(press);
		release_list.push_back(release);
	}
	sort(finger_list.begin(), finger_list.end());
	this->input_list = press_list;

	// Set Hand and Finger List
	int level = -(MaxHandNumber / 2);
	for (int i = 0; i < MaxHandNumber; i++) {
		AdofaiHand hnd;
		if (i >= (MaxHandNumber + 1) / 2) hnd.hand = new Hand(Hand::LeftHand);
		else hnd.hand = new Hand(Hand::RightHand);

		hnd.finger[0].finger = &hnd.hand->Thumb();
		hnd.finger[1].finger = &hnd.hand->Index();
		hnd.finger[2].finger = &hnd.hand->Middle();
		hnd.finger[3].finger = &hnd.hand->Ring();
		hnd.finger[4].finger = &hnd.hand->Pinky();

		// Hand Angle Setting
		{
			double end_degree = 20;
			double middle_degree = 45;
			double base_degree = 20;

			hnd.hand->SetAngle()->index_end = end_degree;
			hnd.hand->SetAngle()->middle_end = end_degree;
			hnd.hand->SetAngle()->ring_end = end_degree;
			hnd.hand->SetAngle()->pinky_end = end_degree;

			hnd.hand->SetAngle()->index_middle = middle_degree;
			hnd.hand->SetAngle()->middle_middle = middle_degree;
			hnd.hand->SetAngle()->ring_middle = middle_degree;
			hnd.hand->SetAngle()->pinky_middle = middle_degree;

			hnd.hand->SetAngle()->index_begin = base_degree;
			hnd.hand->SetAngle()->middle_begin = base_degree;
			hnd.hand->SetAngle()->ring_begin = base_degree;
			hnd.hand->SetAngle()->pinky_begin = base_degree;

			hnd.hand->SetAngle()->thumb_base = base_degree;
			hnd.hand->SetAngle()->thumb_middle = base_degree;
			hnd.hand->SetAngle()->thumb_end = base_degree;

			hnd.hand->degree_factor = 15 - 5 * abs(level);
			if (level < 0) hnd.hand->degree_factor *= -1;
			hnd.hand->SetAngle()->basement_leftright = -8 * level;
			hnd.hand->SetAngle()->basement_updown = 30;

			hnd.hand->zoom_factor = HandSize;
		}

		this->hand[i] = hnd;

		level++;
		if (level == 0 && MaxHandNumber % 2 == 0) level++;
	}

	// Set Finger Animation
	unsigned long long int finger_std_moving_time = 50 * 1000000; // 50ms
	for (int i = 0; i < finger_list.size(); i++) {
		FingerData fd = finger_list[i];
		auto ani_list = &(this->hand[fd.hand_num].finger[fd.finger].animation);
		auto target = GetAngleData(this->hand[fd.hand_num].hand, fd.finger);
		double initial_degree = *target;
		double prepareing_degree = -15.0;
		double pressing_degree = 15.0;
		int prepareing_ratio = 2;
		if (fd.press) {
			// Full Prepareing and Pressing
			if (ani_list->empty() || ani_list->back().Get().end_time < fd.time - finger_std_moving_time * (prepareing_ratio + 1)) {
				ani_list->push_back(
					Animation(
						target, initial_degree, initial_degree + prepareing_degree,
						fd.time - finger_std_moving_time * (prepareing_ratio + 1),
						fd.time - finger_std_moving_time - 1,
						Animation::EASE_OUT
					)
				);
				ani_list->push_back(
					Animation(
						target, initial_degree + prepareing_degree, initial_degree + pressing_degree, // Press
						fd.time - finger_std_moving_time,
						fd.time,
						Animation::EASE_IN
					)
				);
			}
			// Partial Prepareing and Full Pressing
			else if (ani_list->back().Get().end_time < fd.time - finger_std_moving_time) {
				ani_list->push_back(
					Animation(
						target, initial_degree, initial_degree + prepareing_degree, // Prepareing to Press
						ani_list->back().Get().end_time + 1,
						fd.time - finger_std_moving_time - 1,
						Animation::EASE_OUT
					)
				);
				ani_list->push_back(
					Animation(
						target, initial_degree + prepareing_degree, initial_degree + pressing_degree, // Press
						fd.time - finger_std_moving_time,
						fd.time,
						Animation::EASE_IN
					)
				);
			}
			// Partital Pressing
			else {
				unsigned long long int begin_time = ani_list->back().Get().begin_time;
				unsigned long long int end_time = fd.time;
				unsigned long long int time = (end_time + begin_time) / 2;
				ani_list->back().Set()->end_time = time;
				ani_list->back().Set()->end_value = initial_degree + prepareing_degree;
				ani_list->push_back(
					Animation(
						target, initial_degree + prepareing_degree, initial_degree + pressing_degree, // Press
						time + 1,
						fd.time,
						Animation::EASE_IN
					)
				);
			}
		}
		else {
			ani_list->push_back(
				Animation(
					target, initial_degree + pressing_degree, initial_degree, // Press
					fd.time,
					fd.time + finger_std_moving_time,
					Animation::EASE_OUT
				)
			);
		}
	}

	// Set Hand Animation
	unsigned long long int hand_std_moving_time = 80 * 1000000; // 80ms
	for (int i = 0; i < press_list.size(); i++) {
		FingerData fd = press_list[i];
		if (fd.hand_begin) {
			int prepareing_ratio = 2;
			FingerData prev_press{}, prev_release{};

			auto EndPress = [&]() -> FingerData {
				if (prev_press.hand_num != -1) return prev_press;
				prev_press = press_list[i];
				for (int j = i + 1; j < press_list.size(); j++) {
					if (press_list[j].hand_begin) {
						return prev_press;
					}
					prev_press = press_list[j];
				}
				return FingerData();
			};
			auto EndRelease = [&]() -> FingerData {
				if (prev_release.hand_num != -1) return prev_release;
				prev_release = release_list[i];
				for (int j = i + 1; j < release_list.size(); j++) {
					if (release_list[j].hand_begin) {
						return prev_release;
					}
					prev_release = release_list[j];
				}
				return FingerData();
			};

			// Zoom
			{
				auto ani_list = &(this->hand[fd.hand_num].zoom_animation);
				auto target_zoom = &this->hand[fd.hand_num].hand->zoom_factor;
				double initial_zoom = *target_zoom;
				double prepareing_zoom = 0.05 * initial_zoom;
				double pressing_zoom = -0.1 * initial_zoom;
				// Down Time Setting
				if (ani_list->empty() || ani_list->back().Get().end_time < fd.time - finger_std_moving_time * (prepareing_ratio + 1)) {
					ani_list->push_back(
						Animation(
							target_zoom, initial_zoom, initial_zoom + prepareing_zoom,
							fd.time - finger_std_moving_time * (prepareing_ratio + 1),
							fd.time - finger_std_moving_time - 1,
							Animation::EASE_OUT
						)
					);
					ani_list->push_back(
						Animation(
							target_zoom, initial_zoom + prepareing_zoom, initial_zoom + pressing_zoom, // Press
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partial Prepareing and Full Pressing
				else if (ani_list->back().Get().end_time < fd.time - finger_std_moving_time) {
					ani_list->push_back(
						Animation(
							target_zoom, initial_zoom, initial_zoom + prepareing_zoom, // Prepareing to Press
							ani_list->back().Get().end_time + 1,
							fd.time - finger_std_moving_time - 1,
							Animation::EASE_OUT
						)
					);
					ani_list->push_back(
						Animation(
							target_zoom, initial_zoom + prepareing_zoom, initial_zoom + pressing_zoom, // Press
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partital Pressing
				else {
					unsigned long long int begin_time = ani_list->back().Get().begin_time;
					unsigned long long int end_time = fd.time;
					unsigned long long int time = (end_time + begin_time) / 2;
					ani_list->back().Set()->end_time = time;
					ani_list->back().Set()->end_value = initial_zoom + prepareing_zoom;
					ani_list->push_back(
						Animation(
							target_zoom, initial_zoom + prepareing_zoom, initial_zoom + pressing_zoom, // Press
							time + 1,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Up Time Setting
				ani_list->push_back(
					Animation(
						target_zoom, initial_zoom + pressing_zoom, initial_zoom, // Zoom
						fd.time + 1,
						EndRelease().time,
						Animation::EASE_OUT
					)
				);
			}

			// Updown
			{
				auto ani_list = &(this->hand[fd.hand_num].updown_animation);
				auto target_updown = &this->hand[fd.hand_num].hand->SetAngle()->basement_updown;
				double initial_updown = *target_updown;
				double prepareing_updown = 10.0;
				double pressing_updown = -10.0;
				// Down Time Setting
				if (ani_list->empty() || ani_list->back().Get().end_time < fd.time - finger_std_moving_time * (prepareing_ratio + 1)) {
					ani_list->push_back(
						Animation(
							target_updown, initial_updown, initial_updown + prepareing_updown,
							fd.time - finger_std_moving_time * (prepareing_ratio + 1),
							fd.time - finger_std_moving_time - 1,
							Animation::EASE_OUT
						)
					);
					ani_list->push_back(
						Animation(
							target_updown, initial_updown + prepareing_updown, initial_updown + pressing_updown, // Press
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partial Prepareing and Full Pressing
				else if (ani_list->back().Get().end_time < fd.time - finger_std_moving_time) {
					ani_list->push_back(
						Animation(
							target_updown, initial_updown, initial_updown + prepareing_updown, // Prepareing to Press
							ani_list->back().Get().end_time + 1,
							fd.time - finger_std_moving_time - 1,
							Animation::EASE_OUT
						)
					);
					ani_list->push_back(
						Animation(
							target_updown, initial_updown + prepareing_updown, initial_updown + pressing_updown, // Press
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partital Pressing
				else {
					unsigned long long int begin_time = ani_list->back().Get().begin_time;
					unsigned long long int end_time = fd.time;
					unsigned long long int time = (end_time + begin_time) / 2;
					ani_list->back().Set()->end_time = time;
					ani_list->back().Set()->end_value = initial_updown + prepareing_updown;
					ani_list->push_back(
						Animation(
							target_updown, initial_updown + prepareing_updown, initial_updown + pressing_updown, // Press
							time + 1,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Up Time Setting
				ani_list->push_back(
					Animation(
						target_updown, initial_updown + pressing_updown, initial_updown, // updown
						fd.time + 1,
						EndRelease().time,
						Animation::EASE_OUT
					)
				);
			}

			// Leftright
			{
				auto ani_list = &(this->hand[fd.hand_num].leftright_animation);
				auto x_ani = &(this->hand[fd.hand_num].x_offset_animation);
				auto target_leftright = &this->hand[fd.hand_num].hand->SetAngle()->basement_leftright;
				auto x_target = &this->hand[fd.hand_num].x_offset;
				double initial_leftright = *target_leftright;
				FingerData next_fd = fd;
				for (int j = i + 1; j < press_list.size() && !press_list[j].hand_begin; j++) next_fd = press_list[j];
				auto GetDegree = [](FingerData fd, FingerData fd_sub) -> double {
					double i = fd.finger - INDEX;
					double j = fd_sub.finger - INDEX;
					if (i == -1) return -15;
					return (pow(2, i) - 1) * 3 - (pow(2, j) - 1);
				};
				double prepareing_leftright = GetDegree(fd, next_fd);
				double pressing_leftright = GetDegree(next_fd, fd);
				double x_prepareing = (1 - cos(prepareing_leftright * M_PI / 180)) * this->hand[fd.hand_num].hand->GetLength().basement * 4.0 * HandSize * (prepareing_leftright > 0 ? 1.0 : -1.0);
				double x_pressing = (1 - cos(pressing_leftright * M_PI / 180)) * this->hand[fd.hand_num].hand->GetLength().basement * 4.0 * HandSize * (pressing_leftright > 0 ? 1.0 : -1.0);
				if (this->hand[fd.hand_num].hand->GetType() == Hand::LeftHand) {
					prepareing_leftright *= -1;
					pressing_leftright *= -1;
				}

				// Down Time Setting
				if (ani_list->empty() || ani_list->back().Get().end_time < fd.time - finger_std_moving_time * (prepareing_ratio + 1)) {
					ani_list->push_back(
						Animation(
							target_leftright, initial_leftright, initial_leftright + prepareing_leftright, // leftright
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
					x_ani->push_back(
						Animation(
							x_target, 0, x_prepareing, // x_offset
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partial Prepareing and Full Pressing
				else if (ani_list->back().Get().end_time < fd.time - finger_std_moving_time) {
					ani_list->push_back(
						Animation(
							target_leftright, initial_leftright, initial_leftright + prepareing_leftright, // leftright
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
					x_ani->push_back(
						Animation(
							x_target, 0, x_prepareing, // x_offset
							fd.time - finger_std_moving_time,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Partital Pressing
				else {
					unsigned long long int begin_time = ani_list->back().Get().begin_time;
					unsigned long long int end_time = fd.time;
					unsigned long long int time = (end_time + begin_time) / 2;
					ani_list->back().Set()->end_time = time;
					ani_list->push_back(
						Animation(
							target_leftright, initial_leftright, initial_leftright + prepareing_leftright, // Press
							time + 1,
							fd.time,
							Animation::EASE_IN
						)
					);
					x_ani->back().Set()->end_time = time;
					x_ani->push_back(
						Animation(
							x_target, 0, x_prepareing, // Press
							time + 1,
							fd.time,
							Animation::EASE_IN
						)
					);
				}
				// Uptime Setting
				ani_list->push_back(
					Animation(
						target_leftright, initial_leftright + prepareing_leftright, initial_leftright + pressing_leftright, // leftright
						fd.time + 1,
						EndPress().time,
						Animation::EASE_OUT
					)
				);
				ani_list->push_back(
					Animation(
						target_leftright, initial_leftright + pressing_leftright, initial_leftright, // leftright
						EndPress().time + 1,
						EndRelease().time,
						Animation::EASE_INOUT
					)
				);
				x_ani->push_back(
					Animation(
						x_target, x_prepareing, x_pressing, // x_offset
						fd.time + 1,
						EndPress().time,
						Animation::EASE_OUT
					)
				);
				x_ani->push_back(
					Animation(
						x_target, x_pressing, 0, // x_offset
						EndPress().time + 1,
						EndRelease().time,
						Animation::EASE_INOUT
					)
				);
			}
		}
	}
}

void HandCam::Show() {
	vector<int> pos_list;
	for (int i = 0; i < MaxHandNumber; i++) {
		int pos = WINDOW_WIDTH / (MaxHandNumber * 2 + 1) * (MaxHandNumber * 2 - 1 - i * 2);
		if (i < MaxHandNumber / 2) {
			pos += WINDOW_WIDTH / (MaxHandNumber * 2 + 1);
		}
		pos_list.push_back(pos);
	}

	auto begin_time = high_resolution_clock::now();

	int input_list_index = 0;


	unsigned long long int prev_time = 0;

	bool running = true;

	while (running) {

		long long int cl = ((high_resolution_clock::now() - begin_time).count());
		// Animation
		for (int i = 0; i < MaxHandNumber; i++) {
			for (int j = 0; j < 5; j++) {
				auto ani = &(this->hand[i].finger[j].animation);
				for (int k = 0; k < ani->size(); k++) {
					if (!(*ani)[k].Update(cl)) break;
					if ((*ani)[k].IsEnd()) {
						ani->erase(ani->begin() + k);
						k--;
					}
				}
			}
			auto ani = &(this->hand[i].zoom_animation);
			for (int k = 0; k < ani->size(); k++) {
				if (!(*ani)[k].Update(cl)) break;
				if ((*ani)[k].IsEnd()) {
					ani->erase(ani->begin() + k);
					k--;
				}
			}
			ani = &(this->hand[i].leftright_animation);
			for (int k = 0; k < ani->size(); k++) {
				if (!(*ani)[k].Update(cl)) break;
				if ((*ani)[k].IsEnd()) {
					ani->erase(ani->begin() + k);
					k--;
				}
			}
			ani = &(this->hand[i].updown_animation);
			for (int k = 0; k < ani->size(); k++) {
				if (!(*ani)[k].Update(cl)) break;
				if ((*ani)[k].IsEnd()) {
					ani->erase(ani->begin() + k);
					k--;
				}
			}
			ani = &(this->hand[i].x_offset_animation);
			for (int k = 0; k < ani->size(); k++) {
				if (!(*ani)[k].Update(cl)) break;
				if ((*ani)[k].IsEnd()) {
					ani->erase(ani->begin() + k);
					k--;
				}
			}
		}

		// Effect
		if (input_list_index < this->input_list.size() && cl >= this->input_list[input_list_index].time) {
			int index = this->input_list[input_list_index].hand_num * 5 + this->input_list[input_list_index].finger;
			Point point;
			switch (this->input_list[input_list_index].finger) {
			case THUMB: point = this->hand[this->input_list[input_list_index].hand_num].hand->Thumb().middle.Point(); break;
			case INDEX: point = this->hand[this->input_list[input_list_index].hand_num].hand->Index().end.Point(); break;
			case MIDDLE: point = this->hand[this->input_list[input_list_index].hand_num].hand->Middle().end.Point(); break;
			case RING: point = this->hand[this->input_list[input_list_index].hand_num].hand->Ring().end.Point(); break;
			case PINKY: point = this->hand[this->input_list[input_list_index].hand_num].hand->Pinky().end.Point(); break;
			}
			EffectData ed;
			this->effect_list.push_back(ed);
			this->effect_list.back().x = point.x;
			this->effect_list.back().y = point.y;
			this->effect_list.back().progress = 0;
			this->effect_list.back().hand_num = this->input_list[input_list_index].hand_num;
			input_list_index++;
		}
		for (int k = 0; k < this->effect_list.size(); k++) {
			this->effect_list[k].progress += (cl - prev_time) / 500000.0 / 1000.0;
			if (this->effect_list[k].progress >= 1) {
				this->effect_list.erase(this->effect_list.begin() + k);
				k--;
			}
		}
		prev_time = cl;

		for (int i = 0; i < this->effect_list.size(); i++) {
			int hnd = this->effect_list[i].hand_num;
			Graphic::DrawDot({
				this->effect_list[i].x + pos_list[hnd] + (int)(this->hand[hnd].x_offset * (this->hand[hnd].hand->GetType() == Hand::LeftHand ? -1 : 1)),
				this->effect_list[i].y + 200
				},
				{ 0, 0, 100 },
				100.0 * (1.0 - this->effect_list[i].progress),
				this->effect_list[i].progress * 300 * HandSize
			);
		}
		for (int i = 0; i < MaxHandNumber; i++) {
			auto leftright = 0;
			auto updown = 0;
			this->hand[i].hand->SetAngle()->basement_leftright -= leftright;
			this->hand[i].hand->SetAngle()->basement_updown -= updown;
			this->hand[i].hand->Simulate();
			this->hand[i].hand->SetAngle()->basement_leftright += leftright;
			this->hand[i].hand->SetAngle()->basement_updown += updown;
		}
		for (int i = 0; i < pos_list.size(); i++) {
			this->hand[i].hand->Draw(pos_list[i] + this->hand[i].x_offset * (this->hand[i].hand->GetType() == Hand::LeftHand ? -1 : 1), 200);
		}
		Graphic::Render();
	}
	Graphic::Quit();
	return;
}

Animation::Animation(double* target, double start, double end, unsigned long long int begin_time, unsigned long long int end_time, Transition transition) {
	this->target = target;
	this->data.start_value = start;
	this->data.end_value = end;
	this->data.begin_time = begin_time;
	this->data.end_time = end_time;
	this->data.transition = transition;
	this->finish = false;
}

Animation::Animation() {
}

bool Animation::Update(unsigned long long int time) {
	if (time < this->data.begin_time) return false;
	if (time > this->data.end_time) {
		*(this->target) = this->data.end_value;
		this->finish = true;
		return true;
	}
	int progress = 100 * (time - this->data.begin_time) / (this->data.end_time - this->data.begin_time);
	switch (this->data.transition) {
	case EASE_IN:
		*(this->target) = EaseIn(progress, this->data.start_value, this->data.end_value);
		break;
	case EASE_OUT:
		*(this->target) = EaseOut(progress, this->data.start_value, this->data.end_value);
		break;
	case EASE_INOUT:
		*(this->target) = EaseInOut(progress, this->data.start_value, this->data.end_value);
		break;
	case LINEAR:
		*(this->target) = Linear(progress, this->data.start_value, this->data.end_value);
		break;
	}
	return true;
}

bool Animation::IsEnd() {
	return this->finish;
}

Animation::Data Animation::Get() {
	return this->data;
}

Animation::Data* Animation::Set() {
	return &(this->data);
}

double Animation::EaseIn(int progress, double start, double end) {
	double p = (double)progress / 100;
	return (end - start) * p * p + start;
}

double Animation::EaseOut(int progress, double start, double end) {
	double p = (double)progress / 100;
	return (1 - (1 - p) * (1 - p)) * (end - start) + start;
}

double Animation::EaseInOut(int progress, double start, double end) {
	double p = (double)progress / 100;
	if (p < 0.5) return (end - start) * 2 * p * p + start;
	else return (1 - pow(-2 * p + 2, 2) / 2) * (end - start) + start;
}

double Animation::Linear(int progress, double start, double end) {
	double p = (double)progress / 100;
	return (end - start) * p + start;
}
