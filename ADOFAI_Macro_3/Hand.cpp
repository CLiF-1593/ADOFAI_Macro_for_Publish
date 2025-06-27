#include "Hand.h"
#include <Eigen/Dense>
#include <iostream>
#include "Graphic.h"
#include "Setting.h"
using namespace std;

#define proj(x) cos(x)
#define pi 3.14159265358979323846
#define rad(x) (x * pi / 180.0)

double GetDistance(Vector v1, Vector v2) {
	return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
}

void Finger::Move(double progress) {
	if (progress == -1) this->progress += this->progress_sign;
	else this->progress = progress;
	if (this->progress > this->progress_max) {
		this->progress = this->progress_max;
		this->progress_sign *= -1;
	}
	if (this->progress < this->progress_min) {
		this->progress = this->progress_min;
		this->progress_sign = 1;
		this->is_clicked = false;
	}
}

void Finger::Click() {
	this->progress_sign = 2;
	this->is_clicked = true;
}

void Finger::SetHandVector(Vector hand) {
	this->hand = hand;
}

void Finger::SetFingerLength(double base, double begin, double middle, double end, double zoom) {
	this->base_length = base * zoom;
	this->begin_length = begin * zoom;
	this->middle_length = middle * zoom;
	this->end_length = end * zoom;
}

void Finger::CalculateFingerAngle(double begin, double middle, double end) {
	this->basement = Vector(this->base_length, 0, 0);

	double angle = rad(begin);
	this->begin = this->basement + Vector(this->begin_length * cos(angle), 0, this->begin_length * sin(angle));

	angle += rad(middle);
	this->middle = this->begin + Vector(this->middle_length * cos(angle), 0, this->middle_length * sin(angle));

	angle += rad(end);
	this->end = this->middle + Vector(this->end_length * cos(angle), 0, this->end_length * sin(angle));
}

void Finger::CalculateFingerAngleByProgress(bool is_thumb) {
	double angle = (double)this->progress * this->click_angle / (double)this->progress_max;
	if (is_thumb) angle *= 1.5;
	if (this->is_clicked) this->Move();
	this->CalculateFingerAngle(angle, angle, angle);
}

void Finger::CalculateThumbBasementAngle(double angle) {
	angle = rad(angle);
	double roll = rad(50);
	double pitch = angle;
	double yaw = 0;

	this->basement.Rotate(roll, pitch, yaw);
	this->begin.Rotate(roll, pitch, yaw);
	this->middle.Rotate(roll, pitch, yaw);
	this->end.Rotate(roll, pitch, yaw);
}

void Finger::CalculateThumbBasementAngleByProgress() {
	double angle = (double)this->progress * this->click_angle_for_thumb / (double)this->progress_max;
	this->CalculateThumbBasementAngle(angle);
}

void Finger::SetFingerDirection(double angle) {
	angle = rad(angle);
	double roll = 0;
	double pitch = 0;
	double yaw = angle;

	this->basement.Rotate(roll, pitch, yaw);
	this->begin.Rotate(roll, pitch, yaw);
	this->middle.Rotate(roll, pitch, yaw);
	this->end.Rotate(roll, pitch, yaw);
}

void Finger::RotateCameraView(double roll, double pitch, double yaw) {
	this->basement = this->basement + this->hand;
	this->begin = this->begin + this->hand;
	this->middle = this->middle + this->hand;
	this->end = this->end + this->hand;

	roll = rad(roll);
	pitch = rad(pitch);
	yaw = rad(yaw);

	this->hand.Rotate(roll, pitch, yaw);
	this->basement.Rotate(roll, pitch, yaw);
	this->begin.Rotate(roll, pitch, yaw);
	this->middle.Rotate(roll, pitch, yaw);
	this->end.Rotate(roll, pitch, yaw);
}

void Finger::LeftRightInversion() {
	this->hand = Vector(-this->hand.x, this->hand.y, this->hand.z);
	this->basement = Vector(-this->basement.x, this->basement.y, this->basement.z);
	this->begin = Vector(-this->begin.x, this->begin.y, this->begin.z);
	this->middle = Vector(-this->middle.x, this->middle.y, this->middle.z);
	this->end = Vector(-this->end.x, this->end.y, this->end.z);
}

void Finger::UpDownInversion() {
	this->hand = Vector(this->hand.x, -this->hand.y, this->hand.z);
	this->basement = Vector(this->basement.x, -this->basement.y, this->basement.z);
	this->begin = Vector(this->begin.x, -this->begin.y, this->begin.z);
	this->middle = Vector(this->middle.x, -this->middle.y, this->middle.z);
	this->end = Vector(this->end.x, -this->end.y, this->end.z);
}

void Finger::RawDraw(int x, int y, bool is_thumb) {
	Vector cur, prev;
	cur = Vector(x, y, 0);
	DrawPoint(cur.Point());
	prev = cur;

	cur = this->hand + Vector(x, y, 0);
	DrawLine(prev.Point(), cur.Point());
	DrawPoint(cur.Point());
	prev = cur;

	cur = this->basement + Vector(x, y, 0);
	DrawLine(prev.Point(), cur.Point());
	DrawPoint(cur.Point());
	prev = cur;

	cur = this->begin + Vector(x, y, 0);
	DrawLine(prev.Point(), cur.Point());
	DrawPoint(cur.Point());
	prev = cur;

	cur = this->middle + Vector(x, y, 0);
	DrawLine(prev.Point(), cur.Point());
	DrawPoint(cur.Point());
	prev = cur;

	if (!is_thumb) {
		cur = this->end + Vector(x, y, 0);
		DrawLine(prev.Point(), cur.Point());
		DrawPoint(cur.Point());
	}
}

void Finger::Draw_OnlySkeleton(int x, int y, int w, bool is_thumb) {
	int width = w;

	Vector cur, prev;
	cur = Vector(x, y, 0);
	prev = cur;

	cur = this->hand + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), SkeletonColor, width);
	prev = cur;

	cur = this->basement + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), SkeletonColor, width);
	prev = cur;

	cur = this->begin + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), SkeletonColor, width);
	prev = cur;

	cur = this->middle + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), SkeletonColor, width);
	prev = cur;

	if (!is_thumb) {
		cur = this->end + Vector(x, y, 0);
		Graphic::DrawLine(cur.Point(), prev.Point(), SkeletonColor, width);
	}
}

void Finger::Draw_SimpleMethod(int x, int y, int w, bool is_thumb) {
	int width = w;

	Vector cur, prev;
	cur = Vector(x, y, 0);
	prev = cur;

	cur = this->hand + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), HandColor, width);
	prev = cur;

	cur = this->basement + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), HandColor, width);
	prev = cur;

	cur = this->begin + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), HandColor, width);
	prev = cur;

	cur = this->middle + Vector(x, y, 0);
	Graphic::DrawLine(cur.Point(), prev.Point(), HandColor, width);
	prev = cur;

	if (!is_thumb) {
		cur = this->end + Vector(x, y, 0);
		Graphic::DrawLine(cur.Point(), prev.Point(), HandColor, width);
	}
}

void Finger::AdvancedDraw_Depth(int x, int y, int w, bool is_thumb) {
	int width = w;
	auto color = HandColor;

	Vector cur, prev;
	cur = Vector(x, y, 0);
	prev = cur;

	cur = this->hand + Vector(x, y, 0);
	Graphic::DrawLine(cur, prev, color, width);
	prev = cur;

	cur = this->basement + Vector(x, y, 0);
	Graphic::DrawLine(cur, prev, color, width);
	prev = cur;

	cur = this->begin + Vector(x, y, 0);
	Graphic::DrawLine(cur, prev, color, width);
	prev = cur;

	cur = this->middle + Vector(x, y, 0);
	Graphic::DrawLine(cur, prev, color, width);
	prev = cur;

	if (!is_thumb) {
		cur = this->end + Vector(x, y, 0);
		Graphic::DrawLine(cur, prev, color, width);
	}
}

void Finger::AdvancedDraw_Shading(int x, int y, int w, bool is_thumb) {

}

void Finger::Draw(int x, int y, int w, bool is_thumb) {
	this->CurrentDrawFunction(x, y, w, is_thumb);
}

Hand::Hand(Type type) {
	this->type = type;
}

Hand::~Hand() {
}

Hand::Length Hand::GetLength() {
	return this->length;
}

Hand::Angle* Hand::SetAngle() {
	return &this->angle;
}

Hand::Angle Hand::GetAngle() {
	return this->angle;
}

void Hand::RawDraw(int x, int y) {
	this->thumb.RawDraw(x, y, true);
	this->index.RawDraw(x, y);
	this->middle.RawDraw(x, y);
	this->ring.RawDraw(x, y);
	this->pinky.RawDraw(x, y);

	DrawLine(this->thumb.basement.Point(x, y), this->index.basement.Point(x, y));
	DrawLine(this->index.basement.Point(x, y), this->middle.basement.Point(x, y));
	DrawLine(this->middle.basement.Point(x, y), this->ring.basement.Point(x, y));
	DrawLine(this->ring.basement.Point(x, y), this->pinky.basement.Point(x, y));
}

void Hand::Draw_OnlySkeleton(int x, int y) {
	int width = SkeletonWidth;
	
	Graphic::DrawLine(this->thumb.basement.Point(x, y), this->index.basement.Point(x, y), SkeletonColor, width);
	Graphic::DrawLine(this->index.basement.Point(x, y), this->middle.basement.Point(x, y), SkeletonColor, width);
	Graphic::DrawLine(this->middle.basement.Point(x, y), this->ring.basement.Point(x, y), SkeletonColor, width);
	Graphic::DrawLine(this->ring.basement.Point(x, y), this->pinky.basement.Point(x, y), SkeletonColor, width);
	
	this->thumb.Draw(x, y, width, true);
	this->index.Draw(x, y, width);
	this->middle.Draw(x, y, width);
	this->ring.Draw(x, y, width);
	this->pinky.Draw(x, y, width);
}

void Hand::Draw_SimpleMethod(int x, int y) {
	int width = HandWidth;
	
	Graphic::DrawLine(this->thumb.basement.Point(x, y), this->index.basement.Point(x, y), HandColor, width);
	Graphic::DrawLine(this->index.basement.Point(x, y), this->middle.basement.Point(x, y), HandColor, width);
	Graphic::DrawLine(this->middle.basement.Point(x, y), this->ring.basement.Point(x, y), HandColor, width);
	Graphic::DrawLine(this->ring.basement.Point(x, y), this->pinky.basement.Point(x, y), HandColor, width);
	
	this->thumb.Draw(x, y, width + 10, true);
	this->index.Draw(x, y, width);
	this->middle.Draw(x, y, width);
	this->ring.Draw(x, y, width);
	this->pinky.Draw(x, y, width - 10);
}

void Hand::AdvancedDraw_Depth(int x, int y) {
	int width = HandWidth * this->zoom_factor;
	auto color = HandColor;

	auto move = [&](Vector v) -> Vector {
		v.x += x;
		v.y += y;
		return v;
	};
	
	Graphic::DrawLine(move(this->thumb.basement), move(this->index.basement), color, width);
	Graphic::DrawLine(move(this->index.basement), move(this->middle.basement), color, width);
	Graphic::DrawLine(move(this->middle.basement), move(this->ring.basement), color, width);
	Graphic::DrawLine(move(this->ring.basement), move(this->pinky.basement), color, width);
	
	this->thumb.Draw(x, y, width + 10, true);
	this->index.Draw(x, y, width);
	this->middle.Draw(x, y, width);
	this->ring.Draw(x, y, width);
	this->pinky.Draw(x, y, width - 10);
}

void Hand::AdvancedDraw_Shading(int x, int y) {
	
}

void Hand::Draw(int x, int y) {
	this->CurrentDrawFunction(x, y);
}

void Hand::Simulate() {

	Vector base_vector = Vector(0, 0, 0);
	this->thumb.SetHandVector(base_vector - Vector(this->length.basement / 2 * this->zoom_factor, 0, 0));
	this->index.SetHandVector(base_vector - Vector(this->length.basement / 4 * this->zoom_factor, 0, 0));
	this->middle.SetHandVector(base_vector);
	this->ring.SetHandVector(base_vector + Vector(this->length.basement / 4 * this->zoom_factor, 0, 0));
	this->pinky.SetHandVector(base_vector + Vector(this->length.basement / 2 * this->zoom_factor, 0, 0));

	this->thumb.SetFingerLength(this->length.thumb_base, this->length.thumb_middle, this->length.thumb_end, 0, this->zoom_factor);
	this->index.SetFingerLength(this->length.index_base, this->length.index_begin, this->length.index_middle, this->length.index_end, this->zoom_factor);
	this->middle.SetFingerLength(this->length.middle_base, this->length.middle_begin, this->length.middle_middle, this->length.middle_end, this->zoom_factor);
	this->ring.SetFingerLength(this->length.ring_base, this->length.ring_begin, this->length.ring_middle, this->length.ring_end, this->zoom_factor);
	this->pinky.SetFingerLength(this->length.pinky_base, this->length.pinky_begin, this->length.pinky_middle, this->length.pinky_end, this->zoom_factor);

	
	#if Test
	this->thumb.CalculateFingerAngleByProgress();
	this->index.CalculateFingerAngleByProgress();
	this->middle.CalculateFingerAngleByProgress();
	this->ring.CalculateFingerAngleByProgress();
	this->pinky.CalculateFingerAngleByProgress();

	this->thumb.CalculateThumbBasementAngleByProgress();

	#else
	this->thumb.CalculateFingerAngle(this->angle.thumb_middle, this->angle.thumb_end, 0);
	this->index.CalculateFingerAngle(this->angle.index_begin, this->angle.index_middle, this->angle.index_end);
	this->middle.CalculateFingerAngle(this->angle.middle_begin, this->angle.middle_middle, this->angle.middle_end);
	this->ring.CalculateFingerAngle(this->angle.ring_begin, this->angle.ring_middle, this->angle.ring_end);
	this->pinky.CalculateFingerAngle(this->angle.pinky_begin, this->angle.pinky_middle, this->angle.pinky_end);
	
	this->thumb.CalculateThumbBasementAngle(this->angle.thumb_base);
	
	#endif

	this->thumb.SetFingerDirection((125));
	this->index.SetFingerDirection((100));
	this->middle.SetFingerDirection((90));
	this->ring.SetFingerDirection((80));
	this->pinky.SetFingerDirection((70));

	if (this->type == LeftHand) {
		this->thumb.LeftRightInversion();
		this->index.LeftRightInversion();
		this->middle.LeftRightInversion();
		this->ring.LeftRightInversion();
		this->pinky.LeftRightInversion();
	}

	this->thumb.UpDownInversion();
	this->index.UpDownInversion();
	this->middle.UpDownInversion();
	this->ring.UpDownInversion();
	this->pinky.UpDownInversion();

	this->thumb.RotateCameraView(this->angle.basement_updown, this->angle.basement_leftright, this->degree_factor);
	this->index.RotateCameraView(this->angle.basement_updown, this->angle.basement_leftright, this->degree_factor);
	this->middle.RotateCameraView(this->angle.basement_updown, this->angle.basement_leftright, this->degree_factor);
	this->ring.RotateCameraView(this->angle.basement_updown, this->angle.basement_leftright, this->degree_factor);
	this->pinky.RotateCameraView(this->angle.basement_updown, this->angle.basement_leftright, this->degree_factor);
}

Finger &Hand::Thumb() {
	return this->thumb;
}

Finger& Hand::Index() {
	return this->index;
}

Finger& Hand::Middle() {
	return this->middle;
}

Finger& Hand::Ring() {
	return this->ring;
}

Finger& Hand::Pinky() {
	return this->pinky;
}

Hand::Type Hand::GetType() {
	return this->type;
}

const Vector Vector::operator+(const Vector& v) const {
	Vector vec;
	vec.x = this->x + v.x;
	vec.y = this->y + v.y;
	vec.z = this->z + v.z;
	return vec;
}

const Vector Vector::operator-(const Vector& v) const {
	Vector vec;
	vec.x = this->x - v.x;
	vec.y = this->y - v.y;
	vec.z = this->z - v.z;
	return vec;
}

void Vector::Rotate(double roll, double pitch, double yaw) {
	Eigen::Matrix3d rotation_matrix;
	rotation_matrix =
		Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

	Eigen::Vector3d original_vector(this->x, this->y, this->z);
	Eigen::Vector3d rotated_vector = original_vector.transpose() * rotation_matrix;
	this->x = rotated_vector.x();
	this->y = rotated_vector.y();
	this->z = rotated_vector.z();
}

Point Vector::Point(int x, int y) {
	return { (int)this->x + x, (int)this->y + y };
}