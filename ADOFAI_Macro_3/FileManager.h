#pragma once
#include <string>
#include <queue>
using namespace std;

struct Effect {
	int tile_num;
	double value;
};

class FileManager {
public:
	bool Read(string adofai_location);
	queue<double> GetAngleData();
	double GetBPM(); 
	queue<Effect> GetBPMList(); //get bpm change data
	queue<Effect> GetTwirlList(); //get twirl data
	queue<Effect> GetHoldList(); //get hold data
	queue<Effect> GetPlanetList(); //get planet data
	queue<Effect> GetPauseList(); //get pause data
	queue<Effect> GetFreeRoamList(); //get free roam data
	queue<Effect> GetAutoPlayTileList(); //get free roam data
	double GetOffset(); //not use
	void Log();
};

