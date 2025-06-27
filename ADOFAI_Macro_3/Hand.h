#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

/*
* 0: RawDraw
* 1: Draw_OnlySkeleton
* 2: Draw_SimpleMethod
* 3: AdvancedDraw_Depth
* 4: AdvancedDraw_Shading
*/
#define CurrentDrawFunction AdvancedDraw_Depth
#define HandColor GraphicColor::HSV(0, 0, 50)
#define SkeletonColor GraphicColor::HSV(0, 0, 100)
#define HandWidth 50
#define SkeletonWidth 104

class Point {
public:
	int x, y;
};

class Vector {
public:
	double x, y, z;

	Vector() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	Vector(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	const Vector operator+ (const Vector& v) const;
	const Vector operator- (const Vector& v) const;

	void Rotate(double roll, double pitch, double yaw);
	Point Point(int x = 0, int y = 0);

	bool operator < (const Vector &a) const {
		return this->z < a.z;
	}
};

class Hand;

class Finger {
public:

	Vector hand;
	Vector basement, begin, middle, end;

	void Move(double progress = -1);
	void Click();

private:
	bool is_clicked = false;

	const int progress_max = 200;
	const int progress_min = 0;
	const double click_angle = 30.0;
	const double click_angle_for_thumb = 10.0;
	int progress = this->progress_min;
	int progress_sign = 1;

	double base_length, begin_length, middle_length, end_length;

	//Init
	void SetHandVector(Vector hand);
	void SetFingerLength(double base, double begin, double middle, double end, double zoom);

	//Calculate
	void CalculateFingerAngle(double begin, double middle, double end);
	void CalculateFingerAngleByProgress(bool is_thumb = false);
	void CalculateThumbBasementAngle(double angle);
	void CalculateThumbBasementAngleByProgress();
	void SetFingerDirection(double angle);
	void RotateCameraView(double roll, double pitch, double yaw);
	void LeftRightInversion();
	void UpDownInversion();

	//Draw
	void RawDraw(int x, int y, bool is_thumb = false);
	void Draw_OnlySkeleton(int x, int y, int w, bool is_thumb = false);
	void Draw_SimpleMethod(int x, int y, int w, bool is_thumb = false);
	void AdvancedDraw_Depth(int x, int y, int w, bool is_thumb = false);
	void AdvancedDraw_Shading(int x, int y, int w, bool is_thumb = false);
	
	void Draw(int x, int y, int w, bool is_thumb = false);

	friend class Hand;
};

class Hand {
public:
	enum Type { LeftHand = -1, RightHand = 1 };

private:
	class Length {
	public:
		const double basement		= 100;
									  
		const double thumb_base		= 110;
		const double thumb_middle	= 70;
		const double thumb_end		= 45;
									  
		const double index_base		= 160;
		const double index_begin	= 85;
		const double index_middle	= 60;
		const double index_end		= 40;
									  
		const double middle_base	= 160;
		const double middle_begin	= 90;
		const double middle_middle	= 60;
		const double middle_end		= 40;
									  
		const double ring_base		= 150;
		const double ring_begin		= 85;
		const double ring_middle	= 60;
		const double ring_end		= 30;
									  
		const double pinky_base		= 130;
		const double pinky_begin	= 70;
		const double pinky_middle	= 40;
		const double pinky_end		= 30;
	};

	class Angle {
	public:
		double basement_updown = 0;
		double basement_leftright = 0;

		double thumb_base	= 0;
		double thumb_middle	= 0;
		double thumb_end		= 0;
		
		double index_begin	= 0;
		double index_middle	= 0;
		double index_end		= 0;
		
		double middle_begin	= 0;
		double middle_middle = 0;
		double middle_end	= 0;
		
		double ring_begin	= 0;
		double ring_middle	= 0;
		double ring_end		= 0;
		
		double pinky_begin	= 0;
		double pinky_middle	= 0;
		double pinky_end		= 0;
	};

	const Length length;
	Angle angle;
	Type type;

	Vector base_vector;
	Finger thumb, index, middle, ring, pinky;

public:
	Hand(Type type);
	~Hand();

	Length GetLength();

	Angle* SetAngle();
	Angle GetAngle();

	void RawDraw(int x, int y);
	void Draw_OnlySkeleton(int x, int y);
	void Draw_SimpleMethod(int x, int y);
	void AdvancedDraw_Depth(int x, int y);
	void AdvancedDraw_Shading(int x, int y);

	void Draw(int x, int y);

	void Simulate();

	double zoom_factor = 1.0;
	double degree_factor = 0;

	Finger& Thumb();
	Finger& Index();
	Finger& Middle();
	Finger& Ring();
	Finger& Pinky();

	Type GetType();
};

