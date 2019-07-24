#include "AnyRace_CoachAI.h"
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include "nlohmann\json.hpp"
#include "algorithm"
#include <windows.h>
#pragma comment(lib, "winmm.lib")

using json = nlohmann::json;
using namespace BWAPI;
using namespace Filter;
using namespace std;

vector<string> acknowledgedHumanEnemies;

int now;
string cmd;
int workersProductionStoppedAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int idleWorkersAll[8];
int idleWorkersForAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int idleWorkersMaxAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
string idleWorkersMaxTimeAll[8] = { "", "", "", "", "", "", "", "" };
int mineralsAbove[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool isLeave, isGG;
Player plSelectedTemp;
int workerCutLimit;
int workerCutLimitForOnce;
int spend_more_minerals_WarningFor;
bool gameover;
static int lastChecked16 = 0;
map<Position, int> baseMineralsWorkersMap;
string lastShift;
int currentStepIndex;
bool stepDone;
vector<string> stepsArray;

string upgradeTechReplayUnitIDs;
int idleGateTime, idleRoboTime, idleStarTime;

int groundRange, airRange;
int ReplaySeconds;
static int lastChecked;
map<string, string> hkAll;
map<string, list<string>> hkAllRaw;
list<Key> keysPressed;
int minerals_750;
int minerals_1000;
int mineralsAboveLimit;
int mineralsAboveLimitPrev;
int mineralsAboveLimit_LastTime;
int mineralsAboveLimit_LastTimeCounter;
string mineralsAboveLimitLog;

Position currScreen;
Position prevScreen;
double screenCounter = 0;
list<double> screenJumps;

Unitset selectedUnits;
Unitset prevSelectedUnits;
double selectedUnitsCounter = 0;
list<double> selectedUnitsJumps;

Unit Probe_SCV_Larva;
bool OvIsMorphing;
bool droneIsMorphing;
string unitName;
list<string> eve;
int workersCutLogSize;
string repPath;

vector <string> macroLogP, macroLogP0, macroLogP1, macroLogP2, macroLogP3, macroLogP4, macroLogP5, macroLogP6, macroLogP7;
map<string, string> macroLogMap;
multimap<string, string> multiMap;
string str1;
string str2;
multimap<string, string>::iterator iter;
multimap<string, string>::iterator pos;
int indexNow;
bool replayCenteredOnNexus;
string mapName;
string gameType;
bool isThereAllies;

int minutes;
int hours;
string gameTime;

string tips;
bool button1;
bool button2;
bool ctrlF1 = true;
bool ctrlF2;
bool ctrlF3;
bool F1;
bool F2;
bool F3;
bool F5;
int F5_Pressed;
bool F6;
int F6_Pressed = 1;
bool F7;
int F7_Pressed = -1;
bool F8;
int F8_Pressed = -1;
bool shift;
bool ctrl;
bool F9;
int F9_Pressed = -1;
bool F11;
int F11_Pressed;
bool F12;
int F12_Pressed;
bool DOWN;
bool UP;
bool home;
bool end_;
bool pageUp;
bool pageDown;
//bool lMouse = Broodwar->getKeyState(Key::K_LBUTTON);
//bool rMouse = Broodwar->getKeyState(Key::K_RBUTTON);

static int speed = 42;

Playerset players;

int supplyAvailable;
int oldUID;
int newUID;
int FrameCount;

int FPS;
double avgFPS;
int LatencyFrame;
int initialFrameCount;
bool restartCounting_ForIdleWorkerWarning = true;

json j;
json j2;
json j3;
bool screpOK = true;
bool jsonOK = true;
string repDate;
string repGameTitle;
string repType;
bool autoMine;
bool autoTrainWorkers;
bool logWorkersAndSupplyProduction;
bool logUnitsProduction;
int replayLogUnitsFor;
int replayLogSupplyFor;
int stickyScreen;
int dontDrift;
int mineralsAboveValue;

int autoBuildSuppliesBeforeBlocked;
int maxWorkers;
int maxProductionBuildingQueue;
int idleWorkerWarningEvery;
int sameSelectionWarningEvery;
int sameScreenWarningEvery;

int workerCutWarningEvery;
int totalTimeOnScreenOrSelectionAbove;
int idleProductionBuildingWarningEvery;
int idleProdBuild;
int idleProdFrameCounter;
int idleFightingUnitWarningEvery;
int idleFightUnits;
int idleFightFrameCounter;

int mineralsWorker;
int gasWorker;
int workers;

string idleTime;
string workerCut;

int	idleWorkers;
int	idleWorkersFor;
int inProgressWorkers;
int notIdleResourceDepot;
int workersProductionStoppedSeconds;
int workersProductionStopped;
int workersProductionStoppedPrev;
int workersProductionStopped_LastTime;
int workersProductionStopped_LastTimeCounter;
string workersCutLog;
string workersCutLog2;

vector<Position> idleWorkerPos;

// To quickly test things:
//#include "stdafx.h"
//#include <iostream> 
//#include <list>

//using namespace std;
//
//int main()
//{
//	int a = 5, b = 7;
//	cout << "a + b = " << a + b << endl;
//	return 0;
//}
//or
//list<string> strList = { "A", "B", "C", "D" };
//string x = *next(strList.begin(), 1);//item "B"
//
//cout << x.c_str() << endl;//endl = "\n\r"


string getTime(int s)
{
	string time = "";
	hours = s / 3600;
	s = s % 3600;
	minutes = s / 60;
	s = s % 60;
	string seconds_str = to_string(s);
	string minutes_str = to_string(minutes);

	if (s < 10)
		seconds_str = '0' + to_string(s);
	if (minutes < 10)
		minutes_str = '0' + to_string(minutes);
	if (hours > 0)
		time = to_string(hours) + ":" + minutes_str + ":" + seconds_str;
	else
		time = minutes_str + ":" + seconds_str;
	return time;
}

std::string formated(string str)
{
	if (strstr(str.c_str(), "Protoss_"))
		str.replace(str.find("Protoss_"), 8, "");
	if (strstr(str.c_str(), "Terran_"))
		str.replace(str.find("Terran_"), 7, "");
	if (strstr(str.c_str(), "Zerg_"))
		str.replace(str.find("Zerg_"), 5, "");

	return str;
}

std::string formated(UnitType ut)
{
	unitName = ut.getName();
	if (strstr(unitName.c_str(), "Protoss_"))
		unitName.replace(unitName.find("Protoss_"), 8, "");
	if (strstr(unitName.c_str(), "Terran_"))
		unitName.replace(unitName.find("Terran_"), 7, "");
	if (strstr(unitName.c_str(), "Zerg_"))
		unitName.replace(unitName.find("Zerg_"), 5, "");

	if (strstr(unitName.c_str(), "Siege_Tank_Siege_Mode"))
		unitName.replace(unitName.find("Siege_Tank_Siege_Mode"), 21, "Tank_S");
	if (strstr(unitName.c_str(), "Siege_Tank_Tank_Mode"))
		unitName.replace(unitName.find("Siege_Tank_Tank_Mode"), 20, "Tank_T");
	if (strstr(unitName.c_str(), "Vulture_"))
		unitName.replace(unitName.find("Vulture_"), 8, "");

	if (strstr(unitName.c_str(), "High_Templar"))
		unitName.replace(unitName.find("High_Templar"), 12, "HT");
	if (strstr(unitName.c_str(), "Dark_Templar"))
		unitName.replace(unitName.find("Dark_Templar"), 12, "DT");
	if (strstr(unitName.c_str(), "Dark_Archon"))
		unitName.replace(unitName.find("Dark_Archon"), 11, "DA");
	if (strstr(unitName.c_str(), "Robotics_Support_Bay"))
		unitName.replace(unitName.find("Robotics_Support_Bay"), 20, "Robotics_S_Bay");
	if (strstr(unitName.c_str(), "Infested_Command_Center"))
		unitName.replace(unitName.find("Infested_Command_Center"), 23, "Infested_CC");

	if (strstr(unitName.c_str(), "Hero_"))
		unitName.replace(unitName.find("Hero_"), 5, "");

	if (strstr(unitName.c_str(), "Special_"))
		unitName.replace(unitName.find("Special_"), 8, "");

	if (strstr(unitName.c_str(), "Critter_"))
		unitName.replace(unitName.find("Critter_"), 8, "Crit_");

	if (strstr(unitName.c_str(), "Resource_"))
		unitName.replace(unitName.find("Resource_"), 9, "");

	return unitName;
}
//std::string formatedReplay(UnitType ut)
//{
//	std::string unitName = ut.getName();
//
//	if (ut.getRace() == Races::Protoss)
//		unitName.erase(0, 8);
//	if (ut.getRace() == Races::Terran)
//		unitName.erase(0, 7);
//	if (ut == Races::Zerg)
//		unitName.erase(0, 5);
//	return unitName;
//}

bool idleWorkersEvery()
{
	workers = 0;
	mineralsWorker = 0;
	gasWorker = 0;
	inProgressWorkers = 0;
	notIdleResourceDepot = 0;
	idleWorkers = 0;

	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot() && !u->isIdle() && u->isCompleted())
			notIdleResourceDepot++;

		if (u->getType().isWorker() && u->isCompleted())
		{
			workers++;
			if (u->isGatheringMinerals())
				mineralsWorker++;
			if (u->isGatheringGas())
				gasWorker++;
			if (u->isIdle())
			{
				if (!Broodwar->isPaused())
				{
					idleWorkers++;
					idleWorkersFor++;
					idleWorkerPos.push_back(u->getPosition());
				}
			}
		}
		else if (u->getType().isWorker() && !u->isCompleted())
			inProgressWorkers++;
	}

	if (idleWorkers > 0)
	{
		if (restartCounting_ForIdleWorkerWarning)
		{
			initialFrameCount = FrameCount;
			restartCounting_ForIdleWorkerWarning = false; //don't restart counting unless idle workers be 0 again.
		}

		if (FrameCount - initialFrameCount > idleWorkerWarningEvery)
			return true;
		else return false;
	}
	else
	{
		restartCounting_ForIdleWorkerWarning = true;
		return false;
	}
}

void AnyRace_CoachAI::onStart()
{
	gameType = Broodwar->getGameType().getName();
	mapName = Broodwar->mapName();

	if (strstr(mapName.c_str(), "| iCCup | "))
		mapName.replace(mapName.find("| iCCup | "), 10, "");

	hkAll["1"];
	hkAll["2"];
	hkAll["3"];
	hkAll["4"];
	hkAll["5"];
	hkAll["6"];
	hkAll["7"];
	hkAll["8"];
	hkAll["9"];
	hkAll["0"];
	hkAll["F2"] = "Set";
	hkAll["F3"];
	hkAll["F4"];

	// Print the map name. BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	//Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	if (Broodwar->isReplay())
	{
		F7_Pressed = 0;
		repPath = Broodwar->mapPathName();
		string cmd = "bwapi-data\\screp.exe \"" + repPath + "\" > bwapi-data\\repInfo.json";
		system(cmd.c_str());
		ifstream u("bwapi-data\\repInfo.json");
		try
		{
			j2 = json::parse(u);

			repDate = j2["Header"]["StartTime"].get<string>().erase(16, 40);
			repDate = repDate.replace(repDate.find("T"), 1, " ");
			repGameTitle = j2["Header"]["Title"].get<string>();
			repType = j2["Header"]["Type"]["Name"].get<string>();
		}
		catch (exception e)
		{
			screpOK = false;
		}
		ReplaySeconds = Broodwar->getReplayFrameCount() / 23.809523809523809523809523809524;

		// Iterate all the players in the game using a std:: iterator
		players = Broodwar->getPlayers();
		//for (auto p : players)
		//{
		//	if (!p->isObserver()) // Only print the player if they are not an observer
		//		Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		//}
	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		//if (Broodwar->enemy()) // First make sure there is an enemy
		//	Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}

	ifstream i("bwapi-data\\AnyRace_CoachAI.json");
	try
	{
		j = json::parse(i);

		workerCutLimit = j["Control Panel"]["workerCutLimit"].get<int>();
		workerCutLimitForOnce = j["Control Panel"]["workerCutLimitForOnce"].get<int>();
		dontDrift = j["Control Panel"]["dontDrift"].get<int>();
		spend_more_minerals_WarningFor = j["Control Panel"]["spend_more_minerals_WarningFor"].get<int>();
		mineralsAboveValue = j["Control Panel"]["mineralsAboveLog"].get<int>();
	}
	catch (exception e)
	{
		jsonOK = false;
	}

	if (jsonOK && dontDrift > 0)
		dontDrift = dontDrift * 24;
}

void AnyRace_CoachAI::onEnd(bool isWinner)
{
	if (isWinner)// Called when the game ends
	{
		// Log your win here!
	}
}

void AnyRace_CoachAI::onFrame()	// Called every game frame.
{
	vector<Player> AllPlayers;

	FPS = Broodwar->getFPS();
	if (!Broodwar->isPaused())
		FrameCount = Broodwar->getFrameCount();
	else
		FrameCount++;

	//Broodwar->hasPath();
	Broodwar->setTextSize(Text::Size::Huge);
	Broodwar->drawTextScreen(134, 0, "%cCoachAI ", Text::Turquoise);
	Broodwar->setTextSize();
	Broodwar->drawTextScreen(213, 4, "%c4.2", Text::Turquoise);

	if (FPS < 1) //gamePaused
		FPS = 24;


	// FrameCount / (1000/42),  1000/42 = Broodwar->getAverageFPS(), when a saved game loaded FrameCount starts from 0.
	// this why sscait/SAIDA doesn't bother themselves finding solution for an accurate time when a saved game loaded.

	//gameTime = getTime(FrameCount / Broodwar->getAverageFPS());//only usefull @ game start
	now = Broodwar->elapsedTime() * 0.6718;
	gameTime = getTime(now);

	try
	{
		static int lastCheckedFrame11 = 0;
		if (lastCheckedFrame11 < FrameCount)
		{
			jsonOK = true;
			lastCheckedFrame11 = FrameCount + FPS;
			ifstream i("bwapi-data\\AnyRace_CoachAI.json");
			j = json::parse(i);
			autoTrainWorkers = j["Control Panel"]["autoTrainWorkers"].get<bool>();
			autoMine = j["Control Panel"]["autoMine"].get<bool>();
			autoBuildSuppliesBeforeBlocked = j["Control Panel"]["autoBuildSuppliesBeforeBlocked"].get<int>();
			maxWorkers = j["Control Panel"]["maxWorkers"].get<int>();
			maxProductionBuildingQueue = j["Control Panel"]["maxProductionBuildingQueue"].get<int>();
			idleWorkerWarningEvery = j["Control Panel"]["idleWorkerWarningEvery"].get<int>() * FPS;
			sameScreenWarningEvery = j["Control Panel"]["sameScreenWarningEvery"].get<int>() * FPS;
			sameSelectionWarningEvery = j["Control Panel"]["sameSelectionWarningEvery"].get<int>() * FPS;
			totalTimeOnScreenOrSelectionAbove = j["Control Panel"]["totalTimeOnScreenOrSelectionAbove"].get<int>();
			workerCutWarningEvery = j["Control Panel"]["workerCutWarningEvery"].get<int>() * FPS;
			idleProductionBuildingWarningEvery = j["Control Panel"]["idleProductionBuildingWarningEvery"].get<int>() * FPS;
			idleFightingUnitWarningEvery = j["Control Panel"]["idleFightingUnitWarningEvery"].get<int>() * FPS;
			workersProductionStoppedSeconds = j["Control Panel"]["workersCutCalculationPeriod"].get<int>();

			logWorkersAndSupplyProduction = j["Control Panel"]["logWorkersAndSupplyProduction"].get<bool>();
			logUnitsProduction = j["Control Panel"]["logUnitsProduction"].get<bool>();
			replayLogUnitsFor = j["Control Panel"]["replayLogUnitsFor"].get<int>();
			replayLogSupplyFor = j["Control Panel"]["replayLogSupplyFor"].get<int>();
			stickyScreen = j["Control Panel"]["stickyScreen"].get<int>();
		}
	}
	catch (exception e)
	{
		jsonOK = false;
	}
	if (!jsonOK)
	{
		Broodwar->drawTextScreen(150, 15, "%c:: Error parsing AnyRace_CoachAI.json, %cI'm off !%c ::", Text::BrightRed, Text::Tan, 8);
		return;
	}

	if (F12_Pressed == 0)
	{
		Broodwar->setGUI(true);
	}
	if (F12_Pressed == 1)
	{
		Broodwar->setGUI(false);
	}


	if (Broodwar->isReplay())
	{
		F12_method();
		return Replay();
	}

	if (gameover && FrameCount > lastChecked16)
	{
		Broodwar->sendText("%s", "game over man");
		gameover = false;
	}

	static int lastCheckedFrame_minerals1 = 0;
	static int lastCheckedFrame_minerals2 = 0;
	if ((!strstr(mapName.c_str(), "Multitask") && !strstr(mapName.c_str(), "Macro")) && !Broodwar->isPaused() && Broodwar->elapsedTime() *  0.6718 < spend_more_minerals_WarningFor && lastCheckedFrame_minerals1 < FrameCount && Broodwar->self()->minerals() > 500 && Broodwar->self()->minerals() < 750)
	{
		lastCheckedFrame_minerals1 = FrameCount + FPS * 3;
		PlaySound(L".\\bwapi-data\\5- spend_more_minerals.wav", NULL, SND_ASYNC);
	}
	if ((!strstr(mapName.c_str(), "Multitask") && !strstr(mapName.c_str(), "Macro")) && !Broodwar->isPaused() && Broodwar->elapsedTime() *  0.6718 < spend_more_minerals_WarningFor && lastCheckedFrame_minerals2 < FrameCount && Broodwar->self()->minerals() > 750)
	{
		lastCheckedFrame_minerals2 = FrameCount + FPS * 1;
		PlaySound(L".\\bwapi-data\\5- spend_more_minerals.wav", NULL, SND_ASYNC);
	}

	static int lastCheckedFrame_dontDrift = 0;
	if (dontDrift != -1)
	{
		if (dontDrift < 1 && !gameover)
		{
			Broodwar << Text::BrightRed << Text::Align_Center << "Time out !" << endl;
			gameover = true;
			lastChecked16 = FrameCount + 96;
		}
		else
		{
			if (!Broodwar->isPaused())
				dontDrift--;

			if (Broodwar->countdownTimer() > 0)
				;//do nothing
			else
			{
				if (dontDrift / FPS < 15 && FPS < 25 && !Broodwar->isPaused())// initial notification delay, 2nd bool is only for game start.
				{
					if (lastCheckedFrame_dontDrift < FrameCount)
					{
						lastCheckedFrame_dontDrift = FrameCount + FPS * 1;
						PlaySound(L".\\bwapi-data\\4- beep.wav", NULL, SND_ASYNC);
						Broodwar->setTextSize(Text::Size::Large);
						Broodwar->drawTextScreen(240, 0, "%c%s", 8, getTime(dontDrift / FPS).c_str());
						Broodwar->setTextSize();
					}
				}
				else
				{
					Broodwar->setTextSize(Text::Size::Large);
					Broodwar->drawTextScreen(240, 0, "%c%s", Text::Default, getTime(dontDrift / FPS).c_str());
					Broodwar->setTextSize();
				}
			}
		}
	}

	currScreen = Broodwar->getScreenPosition();
	selectedUnits = Broodwar->getSelectedUnits();
	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame14 = 0;
		if (prevScreen == currScreen)
		{
			screenCounter++;
			if (lastCheckedFrame14 < FrameCount && FrameCount > sameScreenWarningEvery)// initial notification delay, 2nd bool is only for game start.
			{
				lastCheckedFrame14 = FrameCount + FPS * 1; // repeat warning every 1s, if same screen
				PlaySound(L".\\bwapi-data\\2- multitask.wav", NULL, SND_ASYNC);
				Broodwar << Text::BrightRed << Text::Align_Right << "Switch screens, " << Text::Grey << "you spent much time on 1 screen !" << endl;
			}
		}
		else // screens not the same
		{
			static int lastCheckedFrame1 = 0;
			if (lastCheckedFrame1 < FrameCount)
			{
				lastCheckedFrame1 = FrameCount + FPS / 12; // check difference every .5s
				lastCheckedFrame14 = FrameCount + sameScreenWarningEvery; // reset counter, and re-initialized notification delay
				screenJumps.push_back(screenCounter / FPS);
				screenCounter = 0;
			}
		}

		static int lastCheckedFrame2 = 0;
		if (prevSelectedUnits == selectedUnits)
		{
			selectedUnitsCounter++;
			if (lastCheckedFrame2 < FrameCount && FrameCount > sameSelectionWarningEvery)
			{
				lastCheckedFrame2 = FrameCount + FPS * 1;
				PlaySound(L".\\bwapi-data\\2- multitask.wav", NULL, SND_ASYNC);
				Broodwar << Text::Orange << Text::Align_Right << "Switch units, " << Text::Grey << "you focused too much on 1 selection !" << endl;
			}
		}
		else
		{
			static int lastCheckedFrame3 = 0;
			if (lastCheckedFrame3 < FrameCount)
			{
				lastCheckedFrame2 = FrameCount + sameSelectionWarningEvery;
				lastCheckedFrame3 = FrameCount + FPS / 12;
				selectedUnitsJumps.push_back(selectedUnitsCounter / FPS);
				selectedUnitsCounter = 0;
			}
		}
	}

	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame9 = 0;
		if (Broodwar->elapsedTime() > 1 && idleWorkersEvery() && lastCheckedFrame9 < FrameCount)
		{
			lastCheckedFrame9 = FrameCount + FPS * 2;
			PlaySound(L".\\bwapi-data\\1- idleWorker.wav", NULL, SND_ASYNC);
			Broodwar << Text::Grey << Text::Align_Right << "Move the idle worker !" << endl;
		}
	}
	hotKeyHandler();

	DOWN = Broodwar->getKeyState(Key::K_DOWN);
	UP = Broodwar->getKeyState(Key::K_UP);

	F1 = Broodwar->getKeyState(Key::K_F1);
	F2 = Broodwar->getKeyState(Key::K_F2);
	F3 = Broodwar->getKeyState(Key::K_F3);
	F5 = Broodwar->getKeyState(Key::K_F5);
	F6 = Broodwar->getKeyState(Key::K_F6);
	F7 = Broodwar->getKeyState(Key::K_F7);
	F8 = Broodwar->getKeyState(Key::K_F8);
	F9 = Broodwar->getKeyState(Key::K_F9);
	F11 = Broodwar->getKeyState(Key::K_F11);

	shift = Broodwar->getKeyState(Key::K_SHIFT);
	ctrl = Broodwar->getKeyState(Key::K_CONTROL);
	pageUp = Broodwar->getKeyState(Key::K_PRIOR);
	pageDown = Broodwar->getKeyState(Key::K_NEXT);

	home = Broodwar->getKeyState(Key::K_HOME);
	end_ = Broodwar->getKeyState(Key::K_END);
	button1 = Broodwar->getKeyState(Key::K_OEM_COMMA);
	button2 = Broodwar->getKeyState(Key::K_OEM_PERIOD);
	F12_method();

	//speed = 39/26fps --- 36/28fps --- 33/31fps --- 30/34fps --- 27/37fps --- 24/42fps --- 21/48fps
	static int lastCheckedFrame95 = 0;
	if (!Broodwar->isPaused() && pageUp && lastCheckedFrame95 < FrameCount)
	{
		lastCheckedFrame95 = FrameCount + FPS / 6;
		speed -= 3;
		Broodwar->setLocalSpeed(speed);
	}
	static int lastCheckedFrame96 = 0;
	if (!Broodwar->isPaused() && pageDown && lastCheckedFrame96 < FrameCount)
	{
		lastCheckedFrame96 = FrameCount + FPS / 6;
		speed += 3;
		Broodwar->setLocalSpeed(speed);
	}
	static int lastCheckedFrame97 = 0;
	if (!Broodwar->isPaused() && home && lastCheckedFrame97 < FrameCount)
	{
		lastCheckedFrame97 = FrameCount + FPS / 6;
		speed = 42;
		Broodwar->setLocalSpeed(speed);
	}
	static int lastCheckedFrame98 = 0;
	if (!Broodwar->isPaused() && end_ && lastCheckedFrame98 < FrameCount)
	{
		lastCheckedFrame98 = FrameCount + FPS / 6;
		speed = 0;
		Broodwar->setLocalSpeed(speed);
	}

	static int lastCheckedFrame4 = 0;
	if (F5 && lastCheckedFrame4 < FrameCount)
	{
		F6_Pressed = 1;
		F7_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame4 = FrameCount + FPS / 6;
		F5_Pressed++;
		if (F5_Pressed >= 2)
			F5_Pressed = 0;
	}

	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 < FrameCount)
	{
		F7_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame5 = FrameCount + FPS / 6;
		F6_Pressed++;
		if (F6_Pressed >= 2)
			F6_Pressed = 0;
	}

	static int lastCheckedFrame6 = 0;
	if (F7 && lastCheckedFrame6 < FrameCount)
	{
		F6_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame6 = FrameCount + FPS / 6;
		F7_Pressed++;
		if (F7_Pressed >= 3)
			F7_Pressed = 0;
	}

	static int lastCheckedFrame7 = 0;
	if (F8 && lastCheckedFrame7 < FrameCount)
	{
		F5_Pressed = -1;
		F6_Pressed = -1;
		F7_Pressed = -1;

		lastCheckedFrame7 = FrameCount + FPS / 6;
		F8_Pressed++;
	}

	static int lastCheckedFrame10 = 0;
	if (F11 && lastCheckedFrame10 < FrameCount)
	{
		lastCheckedFrame10 = FrameCount + FPS / 6;
		F11_Pressed++;
		if (F11_Pressed == 2)
			F11_Pressed = 0;
	}

	static int lastCheckedFrame8 = 0;
	if (F9 && lastCheckedFrame8 < FrameCount)
	{
		lastCheckedFrame8 = FrameCount + FPS / 6;
		F9_Pressed++;

		if (F9_Pressed >= idleWorkerPos.size())
			F9_Pressed = 0;
	}
	if (F9 && idleWorkerPos.size() > 0)
	{
		Broodwar->setScreenPosition(idleWorkerPos[F9_Pressed].x - 310, idleWorkerPos[F9_Pressed].y - 210);
		Broodwar->drawCircleMap(idleWorkerPos[F9_Pressed].x, idleWorkerPos[F9_Pressed].y, 10, Color(255, 0, 0), true);
	}
	idleWorkerPos.clear();

	for (auto &u : selectedUnits)
	{
		string unitName = u->getType().getName().c_str();
		if (u->getPlayer() == Broodwar->self() && u->isTraining() && unitName != "Protoss_Reaver" && unitName != "Protoss_Carrier")
		{
			if (u->getType().isResourceDepot())
			{
				if (Broodwar->elapsedTime() *  0.6718 > workersProductionStoppedSeconds)
				{
					u->cancelTrain();
					Broodwar << Text::Align_Right << "Not allowed to make workers after " << getTime(workersProductionStoppedSeconds) << endl;
				}
			}
			else //prodBuild
			{
				if (u->getTrainingQueue().size() > maxProductionBuildingQueue)
				{
					u->cancelTrain();
					Broodwar << Text::Align_Right << "Not allowed to queue units more than " << maxProductionBuildingQueue << endl;
				}
			}
		}
		int dead = u->getPlayer()->deadUnitCount(u->getType());
		if (dead > 0)
			Broodwar->drawTextMap(u->getPosition().x, u->getBottom() + 35, "%d %s lost", dead, formated(u->getType()).c_str());
		break;
	}

	string ugName;
	for (auto &ug : UpgradeTypes::allUpgradeTypes())
	{
		if (Broodwar->self()->getUpgradeLevel(ug))
		{
			for (auto &u : ug.whatUses())
			{
				if (ug.maxRepeats() > 1)
				{
					ugName = ug.getName() + " +" + to_string(Broodwar->self()->getUpgradeLevel(ug));
					//upgrades.push_back(ugName);
				}
				else
				{
					ugName = formated(u) + "s " + ug.getName();
					//upgrades.push_back(ugName);
				}

				auto iter = macroLogMap.find(ugName);
				if (iter != macroLogMap.end())
				{
					// key exists, do something with iter->second (the value)
					//std::string key = iter->first;
					//std::string value = iter->second;
				}
				else
					macroLogMap[ugName] = gameTime;
			}
		}
	}

	for (auto &tech : TechTypes::allTechTypes())
	{
		string techName = tech.getName();
		for (auto &u : tech.whatUses())
		{
			if (Broodwar->self()->hasResearched(tech) && !strstr(u.getName().c_str(), "Hero") && tech.whatResearches() != UnitTypes::None)
			{
				if (techName == "Burrowing" || techName == "Tank_Siege_Mode" || techName == "Stim_Packs")
				{
					//if techName = one of those, just get the techName because its related to different units.
				}
				else
				{
					techName = formated(u) + "s " + techName;
				}

				auto iter = macroLogMap.find(techName);
				if (iter != macroLogMap.end())
				{
					//if techName found don't add it again
				}
				else
				{
					macroLogMap[techName] = gameTime;
				}
			}
		}
	}
	idleProdBuild = 0;
	idleFightUnits = 0;

	list<string> listOfIdleFightUnits;
	list<string> listOfIdleBuildings;

	list<string> listOfUnits;
	list<string> listOfBuildings;
	list<string> listOfInCompleteUnits;
	list<string> listOfInCompleteBuildings;
	map<string, int> UnitsMap;
	map<string, int> BuildingsMap;
	map<string, int> inCompleteUnitsMap;
	map<string, int> inCompleteBuildingsMap;

	string BuildingsFinal;
	string UnitsFinal;
	string inCompleteBuildingsFinal;
	string inCompleteUnitsFinal;
	string inProgressUpgradeTech;
	//================================================== Was enemies, now it all players on map, for observing as much as possible in-game
	static int lastCheckedFrame13 = 0;
	if (now < 120 && Broodwar->isMultiplayer() && Broodwar->enemy() && !Broodwar->isPaused() && lastCheckedFrame13 + 240 < FrameCount) // check if there is an enemy
	{
		lastCheckedFrame13 = FrameCount;
		for (auto &enemy : Broodwar->enemies())
		{
			if (enemy->getType() == PlayerTypes::Player && !(find(acknowledgedHumanEnemies.begin(), acknowledgedHumanEnemies.end(), enemy->getName()) != acknowledgedHumanEnemies.end()))
			{
				Broodwar->sendText("I've top intel about you \"%s\", accept playing by typing \"icCoachAI\".", enemy->getName().c_str());
			}
		}
	}

	if (F8_Pressed > -1)
	{
		AllPlayers.push_back(Broodwar->self());
		for (auto &p : Broodwar->getPlayers())
		{
			if (p != Broodwar->self())
				AllPlayers.push_back(p);
		}
		if (F8_Pressed >= AllPlayers.size())
			F8_Pressed = 0;
	}

	if (AllPlayers.size() > 0)
	{
		list<string> listOfUnits_Enemy;
		list<string> listOfBuildings_Enemy;
		list<string> listOfInCompleteUnits_Enemy;
		list<string> listOfInCompleteBuildings_Enemy;

		map<string, int> UnitsMap_Enemy;
		map<string, int> BuildingsMap_Enemy;
		map<string, int> InCompleteUnitsMap_Enemy;
		map<string, int> InCompleteBuildingsMap_Enemy;

		string UnitsFinal_Enemy;
		string BuildingsFinal_Enemy;
		string InCompleteUnitsFinal_Enemy;
		string InCompleteBuildingsFinal_Enemy;
		string InProgressUpgradeTech_Enemy;

		Position unitPos;
		Position orderPos;
		string orderName;
		char color;
		Player P_F8 = AllPlayers[F8_Pressed];

		for (auto &u : AllPlayers[F8_Pressed]->getUnits())
		{
			UnitType ut = u->getType();
			color = AllPlayers[F8_Pressed]->getTextColor();

			unitPos = u->getPosition();
			orderPos = u->getOrderTargetPosition();
			orderName = u->getOrder().getName();

			if (!ut.isBuilding())
			{
				if (u->isCompleted())
				{
					listOfUnits_Enemy.push_back(formated(ut));

					if (unitPos.isValid() && unitPos.y != 0)
					{
						if (orderPos.isValid() && orderPos.y != 0)
						{
							if (orderName != "MiningMinerals" && orderName != "HarvestGas")
							{
								if (P_F8 == Broodwar->self() && (orderName == "MoveToMinerals" || orderName == "MoveToGas"))
									;
								else
								{
									Broodwar->drawTextMap(unitPos.x, unitPos.y + 10, "%s", orderName.c_str());
									Broodwar->drawLineMap(unitPos, orderPos, 8);
									Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, 8, true);
								}
							}
						}
						else
						{
							if (orderName == "HoldPosition")
								Broodwar->drawTextMap(unitPos.x, unitPos.y + 10, "Hold");
						}

						if (u->getTransport() == nullptr)
						{
							if (u->getPlayer() != Broodwar->self() && !(find(Broodwar->allies().begin(), Broodwar->allies().end(), u->getPlayer()) != Broodwar->allies().end()))
							{
								if (ut.maxEnergy())
									Broodwar->drawTextMap(unitPos, "%c%s: %de", color, formated(ut).c_str(), u->getEnergy());
								else
									Broodwar->drawTextMap(unitPos, "%c%s", color, formated(ut).c_str());
							}
							else
							{
								if (ut.maxEnergy())
									Broodwar->drawTextMap(unitPos, "%c%de", color, u->getEnergy());
							}
						}
					}
				}
				else
				{
					if (ut.getName() == "Zerg_Egg")
						ut = u->getBuildType();
					if (shift)
						listOfInCompleteUnits_Enemy.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
					else if (!shift)
						listOfInCompleteUnits_Enemy.push_back(formated(ut));
				}
			}
			else //building
			{
				Position building = u->getPosition();
				Position RP = u->getRallyPosition();
				if (RP.isValid() && RP.y != 0)
				{
					Broodwar->drawLineMap(building, RP, Text::BrightRed);
					Broodwar->drawCircleMap(RP.x, RP.y, 5, Text::BrightRed, true);
				}
				if (u->getPlayer() != Broodwar->self() && !(find(Broodwar->allies().begin(), Broodwar->allies().end(), u->getPlayer()) != Broodwar->allies().end()))
				{
					if (ut.maxEnergy())
						Broodwar->drawTextMap(unitPos, "%c%s: %de", color, formated(ut).c_str(), u->getEnergy());
					else
						Broodwar->drawTextMap(unitPos, "%c%s", color, formated(ut).c_str());
				}
				else
				{
					if (ut.maxEnergy())
						Broodwar->drawTextMap(unitPos, "%c%de", color, u->getEnergy());
				}

				if (u->isCompleted())
				{
					listOfBuildings_Enemy.push_back(formated(ut));

					if (u->isResearching())
						InProgressUpgradeTech_Enemy += formated(u->getTech().getName()) + " " + to_string(u->getRemainingResearchTime() / FPS) + "\n\r";

					if (u->isUpgrading())
						InProgressUpgradeTech_Enemy += formated(u->getUpgrade().getName()) + " " + to_string(u->getRemainingUpgradeTime() / FPS) + "\n\r";
				}
				else
				{
					if (shift)
						listOfInCompleteBuildings_Enemy.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
					else if (!shift)
						listOfInCompleteBuildings_Enemy.push_back(formated(ut));
				}
			}
		}
		string ugName_e;
		list<string> upgrades;

		for (auto &ug : UpgradeTypes::allUpgradeTypes())
		{
			if (AllPlayers[F8_Pressed]->getUpgradeLevel(ug))
			{
				for (auto &u : ug.whatUses())
				{
					if (ug.maxRepeats() > 1)
						ugName_e = ug.getName() + " +" + to_string(AllPlayers[F8_Pressed]->getUpgradeLevel(ug));
					else
						ugName_e = formated(u) + "s " + ug.getName();

					if (!(find(upgrades.begin(), upgrades.end(), ugName_e) != upgrades.end()))
						upgrades.push_back(ugName_e);
				}
			}
		}

		list<string> techs;
		for (auto &tech : TechTypes::allTechTypes())
		{
			string techName = tech.getName();
			for (auto &u : tech.whatUses())
			{
				if (AllPlayers[F8_Pressed]->hasResearched(tech) && !strstr(u.getName().c_str(), "Hero") && tech.whatResearches() != UnitTypes::None)
				{
					if (techName == "Burrowing" || techName == "Tank_Siege_Mode" || techName == "Stim_Packs")
					{
						//if techName = one of those, just get the techName because its related to different units.
					}
					else
					{
						techName = formated(u) + "s " + techName;
					}

					if (!(find(techs.begin(), techs.end(), techName) != techs.end()))
						techs.push_back(techName);
				}
			}
		}

		for (string item : listOfUnits_Enemy)
			UnitsMap_Enemy[item]++;
		for (auto entry : UnitsMap_Enemy)
			UnitsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";
		for (string item : listOfBuildings_Enemy)
			BuildingsMap_Enemy[item]++;
		for (auto entry : BuildingsMap_Enemy)
			BuildingsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";

		if (ctrl)// && P_F8 != Broodwar->self()
		{
			if (shift)
			{
				for (string item : listOfInCompleteUnits_Enemy)
					InCompleteUnitsFinal_Enemy += item + "\n\r";
				for (string item : listOfInCompleteBuildings_Enemy)
					InCompleteBuildingsFinal_Enemy += item + "\n\r";
				Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", color, Text::Default, UnitsFinal_Enemy.c_str(), 28, Text::Default, InCompleteUnitsFinal_Enemy.c_str());
				Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", color, Text::Default, BuildingsFinal_Enemy.c_str(), 28, Text::Default, InCompleteBuildingsFinal_Enemy.c_str(), color, Text::Default, InProgressUpgradeTech_Enemy.c_str());
			}
			else
			{
				for (string item : listOfInCompleteUnits_Enemy)
					InCompleteUnitsMap_Enemy[item]++;
				for (string item : listOfInCompleteBuildings_Enemy)
					InCompleteBuildingsMap_Enemy[item]++;
				for (auto entry : InCompleteBuildingsMap_Enemy)
					InCompleteBuildingsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";
				for (auto entry : InCompleteUnitsMap_Enemy)
					InCompleteUnitsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";
				Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", color, Text::Default, UnitsFinal_Enemy.c_str(), color, Text::Default, InCompleteUnitsFinal_Enemy.c_str());
				Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", color, Text::Default, BuildingsFinal_Enemy.c_str(), color, Text::Default, InCompleteBuildingsFinal_Enemy.c_str(), color, Text::Default, InProgressUpgradeTech_Enemy.c_str());
			}
			string upgradesFinal;
			for (auto entry : upgrades)
				upgradesFinal += entry + "\n\r";
			string technoFinal;
			for (auto entry : techs)
				technoFinal += entry + "\n\r";

			Broodwar->drawTextScreen(190, 45, "%cTech: %c%s%cUpgrade: %c%s", color, Text::Default, technoFinal.c_str(), color, Text::Default, upgradesFinal.c_str());
		}

		if (P_F8 != Broodwar->self())
			Broodwar->drawTextScreen(190, 35, "%c%s: %c%d, %c%d, %c%d/%d", color, P_F8->getName().c_str(), Text::Turquoise, P_F8->minerals(), Text::Green, P_F8->gas(),
				Text::Teal, P_F8->supplyUsed() / 2, P_F8->supplyTotal() / 2);
	}

	int left, right, up, down;
	for (auto &u : Broodwar->self()->getUnits())
	{
		UnitType ut = u->getType();

		if (!ut.isBuilding())
		{
			if (u->isCompleted())
			{
				//draw ground unit sizes that is not inside Transport:
				if (F11_Pressed == 1)
					if (!ut.isFlyer() && u->getTransport() == nullptr)
						Broodwar->drawTextMap(u->getPosition(), "%dx%d", ut.width(), ut.height());

				listOfUnits.push_back(formated(ut));
			}
			else
			{
				if (ut.getName() == "Zerg_Egg")
					ut = u->getBuildType();
				if (F6_Pressed == 1)
					listOfInCompleteUnits.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
				else listOfInCompleteUnits.push_back(formated(ut));
			}
		}
		else
		{
			//draw buildings gaps values:
			if (F11_Pressed == 1)
			{
				left = ut.tileWidth() * 16 - ut.dimensionLeft();
				right = ut.tileWidth() * 16 - ut.dimensionRight() - 1;
				up = ut.tileHeight() * 16 - ut.dimensionUp();
				down = ut.tileHeight() * 16 - ut.dimensionDown() - 1;

				//if (left/right/up/down) > 9 then circle.x + 2
				Broodwar->drawCircleMap(u->getPosition().x - 1, u->getPosition().y + 20, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x - 1, u->getPosition().y - 20, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x + 19, u->getPosition().y, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x - 21, u->getPosition().y, 7, 20, true);

				Broodwar->drawTextMap(u->getPosition().x - 5, u->getPosition().y + 15, "%d", down);
				Broodwar->drawTextMap(u->getPosition().x - 5, u->getPosition().y - 25, "%d", up);
				Broodwar->drawTextMap(u->getPosition().x + 15, u->getPosition().y - 5, "%d", right);
				Broodwar->drawTextMap(u->getPosition().x - 25, u->getPosition().y - 5, "%d", left);
			}

			if (u->isCompleted())
			{
				listOfBuildings.push_back(formated(ut));

				if (u->isResearching())
					inProgressUpgradeTech += formated(u->getTech().getName()) + " " + to_string(u->getRemainingResearchTime() / FPS) + "\n\r";

				if (u->isUpgrading())
					inProgressUpgradeTech += formated(u->getUpgrade().getName()) + " " + to_string(u->getRemainingUpgradeTime() / FPS) + "\n\r";
			}
			else
			{
				if (F6_Pressed == 1)
					listOfInCompleteBuildings.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
				else listOfInCompleteBuildings.push_back(formated(ut));
			}
		}

		if (u->isIdle() && ut.isBuilding() && u->canTrain() && !ut.isResourceDepot())
		{
			if (!Broodwar->isPaused())
			{
				if (ut == UnitTypes::Protoss_Gateway)
					idleGateTime++;
				if (ut == UnitTypes::Protoss_Robotics_Facility)
					idleRoboTime++;
				if (ut == UnitTypes::Protoss_Stargate)
					idleStarTime++;
			}
			idleProdBuild++;
			listOfIdleBuildings.push_back(formated(ut));
		}
		if (u->canAttackMove() && !ut.isWorker() && !ut.isBuilding() && (u->isIdle() || u->isHoldingPosition()))
		{
			idleFightUnits++;
			listOfIdleFightUnits.push_back(formated(ut));
		}

		if (u->canSetRallyPoint())
		{
			Position building = u->getPosition();

			Position RP = u->getRallyPosition();
			if (RP.y != 0)
			{
				Color color = u->getPlayer()->getColor();
				Broodwar->drawLineMap(building, RP, color);
				Broodwar->drawCircleMap(RP.x, RP.y, 5, color, true);

				//if (newUID != oldUID) //selection has changed
				//{
				//	Broodwar->pingMinimap(RP);
				//	Broodwar->sendText("RP building selected.");
				//}
			}
		}
		//oldUID = newUID;
		if (!Broodwar->isPaused() && Broodwar->self()->getRace() != Races::Zerg && ut.isResourceDepot() && u->isIdle() && Broodwar->elapsedTime() *  0.6718 < workersProductionStoppedSeconds)
			workersProductionStopped++;
		if (Broodwar->self()->getRace() != Races::Zerg && ut.isResourceDepot() && !u->isIdle() && Broodwar->elapsedTime() *  0.6718 < workersProductionStoppedSeconds)
		{
			if (!Broodwar->isPaused())
				workersProductionStopped += notIdleResourceDepot - inProgressWorkers;
			if (notIdleResourceDepot > 1)
			{
				notIdleResourceDepot = 0;
				inProgressWorkers = 0;
			}
		}
	}

	static int lastCheckedFrame92 = 0;
	if (lastCheckedFrame92 < FrameCount)
	{
		lastCheckedFrame92 = FrameCount + FPS * 1;
		for (auto un : Broodwar->self()->getUnits())
		{
			if (un->getType().isResourceDepot())
			{
				Position p = un->getPosition();
				baseMineralsWorkersMap[p] = Broodwar->getUnitsInRadius(p, 300, IsWorker && IsGatheringMinerals && IsOwned).size();
			}
		}
	}
	int px;
	for (auto kv : baseMineralsWorkersMap)
	{
		Broodwar->drawCircleMap(kv.first, 10, Colors::Black, true);
		if (kv.second < 10)
			px = kv.first.x - 2;
		else px = kv.first.x - 5;

		Broodwar->drawTextMap(px, kv.first.y - 5, "%d", kv.second);
	}

	static int lastCheckedFrame90 = 0;
	if (workersProductionStopped != workersProductionStoppedPrev)
	{
		workersProductionStopped_LastTimeCounter++;
		workersProductionStopped_LastTime = workersProductionStopped_LastTimeCounter;

		if (lastCheckedFrame90 < FrameCount && FrameCount > workerCutWarningEvery)
		{
			lastCheckedFrame90 = FrameCount + FPS * 2;
			PlaySound(L".\\bwapi-data\\3- workerCut.wav", NULL, SND_ASYNC);

			Broodwar << Text::White << Text::Align_Right << "Don't stop making workers !" << endl;
		}
	}
	else
	{
		if (workersProductionStopped_LastTimeCounter != 0 && workersProductionStopped_LastTime / FPS > 1 && !Broodwar->isPaused())
		{
			workersCutLogSize++;
			if (workersCutLogSize < 18)
				workersCutLog += getTime((Broodwar->elapsedTime() * 0.6718) - workersProductionStopped_LastTime / FPS) + " " + to_string(workersProductionStopped_LastTime / FPS) + "s\n\r";
			else workersCutLog2 += getTime((Broodwar->elapsedTime() * 0.6718) - workersProductionStopped_LastTime / FPS) + " " + to_string(workersProductionStopped_LastTime / FPS) + "s\n\r";

		}
		workersProductionStopped_LastTimeCounter = 0;

		static int lastCheckedFrame91 = 0;
		if (lastCheckedFrame91 < FrameCount)
			lastCheckedFrame90 = FrameCount + workerCutWarningEvery; // re-initialized notification delay
	}

	for (string item : listOfUnits)
		UnitsMap[item]++;
	for (string item : listOfBuildings)
		BuildingsMap[item]++;
	for (auto entry : BuildingsMap)
	{
		if (entry.first == "Gateway")
			BuildingsFinal += to_string(entry.second) + " " + entry.first + " " + getTime(idleGateTime / FPS) + "\n\r";
		else if (entry.first == "Robotics_Facility")
			BuildingsFinal += to_string(entry.second) + " " + entry.first + " " + getTime(idleRoboTime / FPS) + "\n\r";
		else if (entry.first == "Stargate")
			BuildingsFinal += to_string(entry.second) + " " + entry.first + " " + getTime(idleStarTime / FPS) + "\n\r";
		else
			BuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
	}
	for (auto entry : UnitsMap)
		UnitsFinal += to_string(entry.second) + " " + entry.first + "\n\r";

	map<string, int> idleFightUnitsMap;
	for (string item : listOfIdleFightUnits)
		idleFightUnitsMap[item]++;

	string idleFightUnitsFinal;
	for (auto entry : idleFightUnitsMap)
		idleFightUnitsFinal += to_string(entry.second) + " " + entry.first + "\n\r";

	map<string, int> idleBuildingsMap;
	for (string item : listOfIdleBuildings)
		idleBuildingsMap[item]++;

	string idleBuildingsFinal;
	for (auto entry : idleBuildingsMap)
		idleBuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";

	if (idleProdBuild > 0)
		idleProdFrameCounter++;
	else idleProdFrameCounter = 0;

	// lastCheckedFrame used to display the warning once (in 1 frame), don't display it again unless the required amount of frames (idleProductionBuildingWarningEvery) passed.
	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame = 0;
		if (idleProdFrameCounter > idleProductionBuildingWarningEvery && lastCheckedFrame + idleProductionBuildingWarningEvery < FrameCount)
		{
			lastCheckedFrame = FrameCount;
			Broodwar << Text::Grey << Text::Align_Right << "Idle production building for " << idleProductionBuildingWarningEvery / FPS << "s !" << endl;
		}
	}

	if (idleFightUnits > 0)
		idleFightFrameCounter++;
	else idleFightFrameCounter = 0;

	if (!Broodwar->isPaused())
	{
		static int lastChecked4 = 0;
		if (idleFightFrameCounter > idleFightingUnitWarningEvery && lastChecked4 + idleFightingUnitWarningEvery < FrameCount)
		{
			lastChecked4 = FrameCount;
			Broodwar << Text::Grey << Text::Align_Right << "Idle fighting unit for " << idleFightingUnitWarningEvery / FPS << "s !" << endl;
		}
	}
	supplyAvailable = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();

	Broodwar->drawTextScreen(5, 25, "%cMinerals workers: %c%d", Text::Turquoise, Text::BrightRed, mineralsWorker);
	Broodwar->drawTextScreen(5, 35, "%cGas workers: %c%d", Text::Green, Text::BrightRed, gasWorker);
	Broodwar->drawTextScreen(5, 45, "%cIdle workers: %c%d, %s", Text::Grey, Text::BrightRed, idleWorkers, getTime(idleWorkersFor / FPS).c_str());

	string match = Broodwar->self()->getRace().getName().substr(0, 1) + "v";
	if (Broodwar->enemies().size() > 0)
	{
		for (auto e : Broodwar->enemies())
			match += e->getRace().getName().substr(0, 1);
	}

	if (!Broodwar->isPaused() && !gameover && workersProductionStopped / 24 > workerCutLimit)
	{
		Broodwar << Text::Blue << Text::Align_Center << "You cut workers intermittently for " << workerCutLimit << "s" << endl;
		gameover = true;
		lastChecked16 = FrameCount + 96;
	}

	if (workerCutLimitForOnce != -1 && !Broodwar->isPaused() && !gameover && workersProductionStopped_LastTimeCounter / 24 > workerCutLimitForOnce)
	{
		Broodwar << Text::Orange << Text::Align_Center << "You cut workers continuously for " << workerCutLimitForOnce << "s" << endl;
		gameover = true;
		lastChecked16 = FrameCount + 96;
	}

	if (shift)
		Broodwar->drawTextScreen(225, 25, "%c%s %c%s", Text::Purple, match.c_str(), Text::White, gameType.c_str());
	else
		Broodwar->drawTextScreen(225, 25, "%c%s %c%s", Text::Purple, match.c_str(), Text::White, mapName.c_str());

	Broodwar->drawTextScreen(310, 15, "%cWorkers prod., stopped for: %c%s,%c%.1fw", Text::Grey, Text::BrightRed, getTime(workersProductionStopped / FPS).c_str(),
		Text::Turquoise, (workersProductionStopped / FPS) / 12.5);
	static int lastCheckedFrame45 = 0;
	if (FPS != 24 && !Broodwar->isPaused())
	{
		Broodwar->drawTextScreen(530, 15, "%cFPS:%c%d, %cTime:%c%s", 8, 4, FPS, 8, 4, gameTime.c_str());

		if (lastCheckedFrame45 < FrameCount - FPS * 2 && j["Control Panel"]["autoGameSpeed"].get<bool>())
		{
			Broodwar << Text::Blue << Text::Align_Right << "autoGameSpeed=true in the config file, game speed now Fastest." << endl;
			Broodwar->setLocalSpeed(42);
		}
	}
	else
	{
		Broodwar->drawTextScreen(530, 15, "%cFPS:%c%d, %cTime:%c%s", 14, 4, FPS, 14, 4, gameTime.c_str());
		lastCheckedFrame45 = FrameCount;
	}

	if (!Broodwar->isPaused())
	{
		if (Broodwar->self()->minerals() > 750)
			minerals_750++;
		if (Broodwar->self()->minerals() > 1000)
			minerals_1000++;

		if (Broodwar->self()->minerals() > mineralsAboveValue)
			mineralsAboveLimit++;
	}
	static int lastCheckedFrame94 = 0;
	if (mineralsAboveLimit != mineralsAboveLimitPrev)// minerals increase going
	{
		mineralsAboveLimit_LastTimeCounter++;
		mineralsAboveLimit_LastTime = mineralsAboveLimit_LastTimeCounter;
	}
	else//increase stopped, lets log
	{
		if (mineralsAboveLimit_LastTimeCounter != 0 && mineralsAboveLimit_LastTime / FPS > 1 && !Broodwar->isPaused())
			mineralsAboveLimitLog += getTime((Broodwar->elapsedTime() * 0.6718) - mineralsAboveLimit_LastTime / FPS) + " " + to_string(mineralsAboveLimit_LastTime / FPS) + "s\n\r";

		mineralsAboveLimit_LastTimeCounter = 0;
	}
	int Minerals = Broodwar->self()->spentMinerals() - Broodwar->self()->refundedMinerals();
	int Gas = Broodwar->self()->spentGas() - Broodwar->self()->refundedGas();

	Broodwar->drawTextScreen(5, 15, "%cMinerals spent: %d + %cGas spent: %d = %cMacro: %d", 31, Minerals, 7, Gas, 17, Minerals + Gas);
	UnitType supplyName = Broodwar->self()->getRace().getSupplyProvider();
	//	Broodwar->drawTextScreen(190, 330, "%cSupply blocked in: %c%d, %cmaxed with: %c%d %s", 3, Text::BrightRed, supplyAvailable / 2,
	//		3, Text::BrightRed, (200 - Broodwar->self()->supplyTotal() / 2) / 8, formated(supplyName).c_str());

	if (F5_Pressed == 1)
	{
		if (macroLogMap.size() != multiMap.size())
		{
			if (FPS != 24)
			{
				Broodwar << "You might've inaccurate MacroLog time, with speeds other than Fastest: 24 FPS." << endl;
			}

			multiMap.clear();
			for (auto const &kv : macroLogMap)
				multiMap.insert(make_pair(kv.second, kv.first));  // Flip the pairs.
			iter = multiMap.begin();
			//================
			//Default behavior always display the last page when new item added
			//================
			if (macroLogMap.size() % 22 == 0)
				pos = next(iter, macroLogMap.size() - 22);
			else pos = next(iter, macroLogMap.size() - (macroLogMap.size() % 22));
			populatePage();
			indexNow = distance(iter, pos);
		}
		static int lastChecked12 = 0;
		//============
		//Down pressed page down
		//============
		if (DOWN && macroLogMap.size() > 22 && lastChecked12 + FPS / 4 < FrameCount)
		{
			lastChecked12 = FrameCount;
			indexNow = indexNow - (indexNow % 22);
			if (indexNow + 22 < macroLogMap.size())
				indexNow += 22;
			pos = next(iter, indexNow);
			populatePage();
			indexNow = distance(iter, pos);
		}
		//==========
		//Up pressed page up
		//==========
		if (UP && macroLogMap.size() > 22 && macroLogMap.size() > 22 && lastChecked12 + FPS / 4 < FrameCount)
		{
			lastChecked12 = FrameCount;
			if (indexNow != 0)
			{
				indexNow = indexNow - ((indexNow % 22) + 22);
				pos = next(iter, indexNow);
				populatePage();
			}
		}
		Broodwar->drawTextScreen(170, 25, "%cMacroLog: \n\r%s", Text::Blue, str1.c_str());
		Broodwar->drawTextScreen(205, 25, "\n\r%c%s", Text::Default, str2.c_str());

		if (Broodwar->self()->getRace() == Races::Zerg)
			Broodwar->drawTextScreen(5, 55, "%cIdle fighting units: \n\r%c%s", 5, 25, idleFightUnitsFinal.c_str());
		else Broodwar->drawTextScreen(5, 55, "%cIdle production: \n\r%c%s%cIdle fighting units: \n\r%c%s", 5, 25, idleBuildingsFinal.c_str(), 5, 25, idleFightUnitsFinal.c_str());
	}
	else if (F5_Pressed == 0)
	{
		DrawHotkeysAndEnemyKilled();
		if (ctrl && F3)
		{
			ctrlF3 = true;
			ctrlF2 = false;
			ctrlF1 = false;
		}
		else if (ctrl && F2)
		{
			ctrlF2 = true;
			ctrlF3 = false;
			ctrlF1 = false;
		}
		else if (ctrl && F1)
		{
			ctrlF1 = true;
			ctrlF2 = false;
			ctrlF3 = false;
		}
		if (!shift && !ctrl)
		{
			if (stickyScreen == 3)
				Multitasking(idleFightUnitsFinal, idleBuildingsFinal);
			else if (stickyScreen == 2)
				drawWorkersCutLog();
			else
				TimedBo();
		}
		if (ctrl)
		{
			if (stickyScreen != 1)
				TimedBo();
			else
				Multitasking(idleFightUnitsFinal, idleBuildingsFinal);
		}
		if (shift)
		{
			lastShift = gameTime;
			if (stickyScreen != 2)
				drawWorkersCutLog();
			else
				Multitasking(idleFightUnitsFinal, idleBuildingsFinal);
		}
		Broodwar->drawTextScreen(50, 8, "%cshift:%s", Text::Brown, lastShift.c_str());

		for (int i = 0; i < stepsArray.size(); i++)
		{
			static int lastCheckedFrame40 = 0;
			if (!Broodwar->isPaused() && lastCheckedFrame40 < FrameCount && parseTime(stepsArray[i].substr(0, 5)) == now + 2)
			{
				lastCheckedFrame40 = FrameCount + FPS * 4;
				PlaySound(L".\\bwapi-data\\4- beep.wav", NULL, SND_ASYNC);
				cmd = "bwapi-data\\balcon\\balcon.exe -t \"" + stepsArray[i] + "\"";
				WinExec(cmd.c_str(), SW_HIDE);
			}

			if (parseTime(stepsArray[i].substr(0, 5)) < now + 3)//currentStep
				currentStepIndex = i;
			else
				break;
		}
	}

	if (F6_Pressed == 1)
	{
		for (string item : listOfInCompleteUnits)
			inCompleteUnitsFinal += item + "\n\r";
		for (string item : listOfInCompleteBuildings)
			inCompleteBuildingsFinal += item + "\n\r";

		Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 24, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 24, Text::GreyGreen, inCompleteBuildingsFinal.c_str(), 24, 29, inProgressUpgradeTech.c_str());
	}
	else if (F6_Pressed == 0)
	{
		for (string item : listOfInCompleteUnits)
			inCompleteUnitsMap[item]++;
		for (string item : listOfInCompleteBuildings)
			inCompleteBuildingsMap[item]++;
		for (auto entry : inCompleteBuildingsMap)
			inCompleteBuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
		for (auto entry : inCompleteUnitsMap)
			inCompleteUnitsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
		Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 28, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 28, Text::GreyGreen, inCompleteBuildingsFinal.c_str(), 24, 29, inProgressUpgradeTech.c_str());
	}

	string t1 = j["Preset Plan"]["Tips1"].get<string>().c_str();
	string t2 = j["Preset Plan"]["Tips2"].get<string>().c_str();
	string t3 = j["Preset Plan"]["Tips3"].get<string>().c_str();

	if (F7_Pressed == 0)
		Broodwar->drawTextScreen(400, 25, "%c= %s =:\n%c%s", 28, t1.substr(0, t1.find(":")).c_str(), 31, t1.substr(t1.find(":") + 1, t1.size()).c_str());
	else if (F7_Pressed == 1)
		Broodwar->drawTextScreen(400, 25, "%c= %s =:\n%c%s", 28, t2.substr(0, t2.find(":")).c_str(), 31, t2.substr(t2.find(":") + 1, t2.size()).c_str());
	else if (F7_Pressed == 2)
		Broodwar->drawTextScreen(400, 25, "%c= %s =:\n%c%s", 28, t3.substr(0, t3.find(":")).c_str(), 31, t3.substr(t3.find(":") + 1, t3.size()).c_str());

	//custom();

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (FrameCount % Broodwar->getLatencyFrames() != 0)// LatencyFrames() seems to be 2 all the time.
		return;


	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		//if (u->getType().isRefinery())
		//GasReady = true;

		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;

		// Finally make the unit do some stuff!

		if (u->getType().isWorker())
		{
			if (u->isIdle() && autoMine)
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (u->isCarryingGas() || u->isCarryingMinerals())
					u->returnCargo();
				else if (!u->getPowerUp())  // The worker cannot harvest anything if it-
				{                             // is carrying a powerup such as a flag
											  // Harvest from the nearest mineral patch or gas refinery
					if (gasWorker < 3)
						u->gather(u->getClosestUnit(IsRefinery));

					if (!u->gather(u->getClosestUnit(IsMineralField)))
					{
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << endl; ////Note: When no Minerals patches visible, you'll get "Unit_Does_Not_Exist" here.
					}
				} // closure: has no powerup
			} // closure: if idle
		}
		else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
		{
			//============
			//Build Worker: Order the depot to construct more workers, But only when it is idle.
			//============
			if (autoTrainWorkers && Broodwar->self()->minerals() >= 50 && !u->isTraining()
				&& workers < maxWorkers && !droneIsMorphing)
			{
				UnitType worker = u->getType().getRace().getWorker();
				if (u->getType().getRace().getName() == "Zerg")
				{
					Unit larva = u->getClosestUnit(GetType == worker.whatBuilds().first);
					if (larva != nullptr) //If a unit was found (Probe_SCV_Larva)
					{
						larva->morph(worker);
						droneIsMorphing = larva->getBuildType().getName() == "Zerg_Drone";
					}
				}
				else u->train(worker);

				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame, so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				if (lastErr.getName() != "None")
					Broodwar->registerEvent([pos, lastErr](Game*) { Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
						nullptr,    // condition
						Broodwar->getLatencyFrames());  // frames to run
			} // closure: failed to train idle unit
		}
		//supplyProviderType
		UnitType supplyUnit = u->getType().getRace().getSupplyProvider();
		string supplyName = supplyUnit.getName().c_str();
		//============
		//Build supply:
		//============
		buildSupply(u, supplyUnit);
	} // closure: unit iterator
	workersProductionStoppedPrev = workersProductionStopped;
	mineralsAboveLimitPrev = mineralsAboveLimit;
	prevScreen = currScreen;
	prevSelectedUnits = selectedUnits;
}
void AnyRace_CoachAI::F12_method()
{
	F12 = Broodwar->getKeyState(Key::K_F12);
	static int lastCheckedFrame93 = 0;
	if (F12 && lastCheckedFrame93 < FrameCount)
	{
		lastCheckedFrame93 = FrameCount + FPS / 6;
		F12_Pressed++;
		if (shift)
			Broodwar->pauseGame();
		if (ctrl)
			Broodwar->resumeGame();

		if (F12_Pressed > 1)
			F12_Pressed = 0;
	}
}
void AnyRace_CoachAI::Multitasking(std::string &idleFightUnitsFinal, std::string &idleBuildingsFinal)
{
	if (Broodwar->self()->getRace() == Races::Zerg)
		Broodwar->drawTextScreen(5, 55, "%cIdle fighting units: \n\r%c%s", 5, 25, idleFightUnitsFinal.c_str());
	else Broodwar->drawTextScreen(5, 55, "%cIdle production: \n\r%c%s%cIdle fighting units: \n\r%c%s", 5, 25, idleBuildingsFinal.c_str(), 5, 25, idleFightUnitsFinal.c_str());

	Broodwar->drawTextScreen(5, 175, "%cScreen Multitasking:", Text::White);

	Broodwar->drawTextScreen(5, 185, "%cScreen counter: %c%.0fs", Text::Purple, 4, screenCounter / FPS);
	Broodwar->drawTextScreen(5, 195, "%cScreen jumps: %c%d", 16, Text::White, screenJumps.size());
	double totalStay = 0;
	double totalStayAbove = 0;

	for (auto j : screenJumps)
	{
		totalStay += j;
		if (j > totalTimeOnScreenOrSelectionAbove)
		{
			totalStayAbove += j - totalTimeOnScreenOrSelectionAbove;
		}
	}
	Broodwar->drawTextScreen(5, 205, "%cAvg stay: %c%.1fs", 16, Text::White, totalStay / screenJumps.size());
	Broodwar->drawTextScreen(5, 215, "%cAll stay > %c%ds: %c%s", 16, 7, totalTimeOnScreenOrSelectionAbove, Text::White, getTime(totalStayAbove).c_str());
	//=========================
	Broodwar->drawTextScreen(5, 235, "%cSelection Multitasking:", Text::White);
	Broodwar->drawTextScreen(5, 245, "%cSelection counter: %c%.0fs", 16, 4, selectedUnitsCounter / FPS);
	Broodwar->drawTextScreen(5, 255, "%cSelection changed: %c%d", 16, Text::White, selectedUnitsJumps.size());
	double totalStayUnits = 0;
	double totalStayAboveUnits = 0;

	for (auto j : selectedUnitsJumps)
	{
		totalStayUnits += j;
		if (j > totalTimeOnScreenOrSelectionAbove)
		{
			totalStayAboveUnits += j - totalTimeOnScreenOrSelectionAbove;
		}
	}
	Broodwar->drawTextScreen(5, 265, "%cAvg selection: %c%.1fs", 16, Text::White, totalStayUnits / selectedUnitsJumps.size());
	Broodwar->drawTextScreen(5, 275, "%cAll select > %c%ds: %c%s", 16, 7, totalTimeOnScreenOrSelectionAbove, Text::White, getTime(totalStayAboveUnits).c_str());
}
void AnyRace_CoachAI::drawWorkersCutLog()
{
	Broodwar->drawTextScreen(5, 55, "%cWorkcut%c%d%c\n\r%s", Text::White, Text::BrightRed, workersProductionStopped_LastTime / FPS, Text::White, workersCutLog.c_str());
	Broodwar->drawTextScreen(60, 55, "%cMine>%d%c%d\n%c%s%c>750 %c%s\n%c>1000 %c%s", Text::Turquoise, mineralsAboveValue, Text::BrightRed,
		mineralsAboveLimit_LastTime / FPS, Text::White, mineralsAboveLimitLog.c_str(), Text::Turquoise, Text::LightYellow, getTime(minerals_750 / FPS).c_str(),
		Text::Turquoise, Text::LightYellow, getTime(minerals_1000 / FPS).c_str());
}
void AnyRace_CoachAI::TimedBo()
{
	Broodwar->drawBoxScreen(5, 57, 124, 67, 0, true);
	Broodwar->drawBoxScreen(5, 57, 124, 301, 0, false);

	if (ctrlF1)
	{
		Broodwar->drawTextScreen(5, 55, "%c%s\n", j["Preset Plan"]["TimedBo Color1"].get<int>(), j["Preset Plan"]["TimedBo1 Title"].get<string>().c_str());
		stepsArray = j["Preset Plan"]["TimedBo1"].get<vector<string>>();
		tips = j["Preset Plan"]["TimedBo1 Tips"].get<string>();
	}
	if (ctrlF2)
	{
		Broodwar->drawTextScreen(5, 55, "%c%s\n", j["Preset Plan"]["TimedBo Color1"].get<int>(), j["Preset Plan"]["TimedBo2 Title"].get<string>().c_str());
		stepsArray = j["Preset Plan"]["TimedBo2"].get<vector<string>>();
		tips = j["Preset Plan"]["TimedBo2 Tips"].get<string>();
	}
	if (ctrlF3)
	{
		Broodwar->drawTextScreen(5, 55, "%c%s\n", j["Preset Plan"]["TimedBo Color1"].get<int>(), j["Preset Plan"]["TimedBo3 Title"].get<string>().c_str());
		stepsArray = j["Preset Plan"]["TimedBo3"].get<vector<string>>();
		tips = j["Preset Plan"]["TimedBo3 Tips"].get<string>();
	}

	//for (int i = 0; i < stepsArray.size(); i++)
	//{
	//	if (parseTime(stepsArray[i].substr(0, 5)) < now)
	//		qIndex = i;
	//}
	//stepsArray.erase(stepsArray.begin() + qIndex);


	//string delimiter = "--->";//split string
	//size_t pos = 0;
	//while ((pos = bo.find(delimiter)) != string::npos)
	//{
	//	step = bo.substr(0, pos);
	//	steps.push_back(step);
	//	bo.erase(0, pos + delimiter.length());
	//}
	//steps.push_back(bo);

	int yy = 65;
	size_t pos;
	string main, extra;
	stepDone = false;

	for (int i = 0; i < stepsArray.size(); i++)
	{
		if (i > 8)
			Broodwar->drawTextScreen(0, yy, "%c%d", j["Preset Plan"]["TimedBo Color1"].get<int>(), i - 9);
		else
			Broodwar->drawTextScreen(0, yy, "%c%d", j["Preset Plan"]["TimedBo Color1"].get<int>(), i + 1);

		if (strstr(stepsArray[i].c_str(), ";"))
		{
			pos = stepsArray[i].find(";");
			main = stepsArray[i].substr(0, pos + 1);
			extra = stepsArray[i].substr(pos + 1, stepsArray[i].size());
		}
		else
		{
			main = stepsArray[i];
			extra = "";
		}

		if (i == currentStepIndex)//currentStep
		{
			Broodwar->drawTextScreen(7, yy, "%c%s\n", j["Preset Plan"]["TimedBo Color1"].get<int>(), main.c_str());
			if (strstr(stepsArray[i].c_str(), ";"))
			{
				Broodwar->drawBoxScreen(175, 329, 485, 344, 0, true);
				Broodwar->drawTextScreen(180, 330, "%c%s", j["Preset Plan"]["TimedBo Color1"].get<int>(), extra.c_str());
			}
		}
		else
			Broodwar->drawTextScreen(7, yy, "%c%s\n", j["Preset Plan"]["TimedBo Color2"].get<int>(), main.c_str());
		yy += 10;
	}

	if (strstr(tips.c_str(), ";"))
		Broodwar->drawTextScreen(7, yy, "%c%s\n", Text::LightYellow, tips.substr(0, tips.find(";") + 1).c_str());
	else
		Broodwar->drawTextScreen(7, yy, "%c%s\n", Text::LightYellow, tips.c_str());

	static int lastCheckedFrame41 = 0;
	if ((button1 || button2 || Broodwar->getKeyState(Key::K_SPACE)) && lastCheckedFrame41 < FrameCount)
	{
		lastCheckedFrame41 = FrameCount + FPS / 4;
		if (button1)
		{
			for (int i = 49; i < 58 && i - 49 < stepsArray.size(); i++)
			{
				if (Broodwar->getKeyState((Key)i))
				{
					cmd = "bwapi-data\\balcon\\balcon.exe -t \"" + stepsArray[i - 49] + "\"";
					WinExec(cmd.c_str(), SW_HIDE);
					break;
				}
			}
			if (Broodwar->getKeyState(Key::K_0) && stepsArray.size() > 9)
			{
				cmd = "bwapi-data\\balcon\\balcon.exe -t \"" + stepsArray[9] + "\"";
				WinExec(cmd.c_str(), SW_HIDE);
			}
		}
		else if (button2)
		{
			for (int i = 49; i < 58 && i - 39 < stepsArray.size(); i++)
			{
				if (Broodwar->getKeyState((Key)i))
				{
					cmd = "bwapi-data\\balcon\\balcon.exe -t \"" + stepsArray[i - 39] + "\"";
					WinExec(cmd.c_str(), SW_HIDE);
					break;
				}
			}
		}
		else if (!Broodwar->isPaused() && Broodwar->getKeyState(Key::K_SPACE))
		{
			for (int i = 0; i < stepsArray.size(); i++)
			{
				if (parseTime(stepsArray[i].substr(0, 5)) > now)
				{
					cmd = "bwapi-data\\balcon\\balcon.exe -t \"" + stepsArray[i] + "\"";
					WinExec(cmd.c_str(), SW_HIDE);
					break;
				}
			}
		}
	}
}
void AnyRace_CoachAI::DrawHotkeysAndEnemyKilled()
{
	Broodwar->drawTextScreen(143, 25, "%cHotkeys: (%c%d%c)", 8, 17, keysPressed.size(), 8);
	int y = 35;
	for (auto entry : hkAll)
	{
		Broodwar->drawTextScreen(143, y, "%c%s: %c%s", 8, entry.first.c_str(), 27, entry.second.c_str());
		y += 10;
	}
	//=========================================================
	int kCount;
	string K0, K1, K2 = "";
	int colSize = 0;
	for (auto u : UnitTypes::allUnitTypes())
	{
		kCount = Broodwar->self()->killedUnitCount(u);
		if (kCount > 0)
		{
			colSize++;
			if (colSize < 11)
				K0 += to_string(kCount) + " " + formated(u) + "\n\r";
			if (colSize < 22 && colSize > 10)
			{
				K1 += to_string(kCount) + " " + formated(u) + "\n\r";
			}
			if (colSize > 22)
			{
				K2 += to_string(kCount) + " " + formated(u) + "\n\r";
			}
		}
	}

	Broodwar->drawTextScreen(125, 165, "%cEnemies killed:\n\r%c%s", Text::BrightRed, Text::Tan, K0.c_str());
	Broodwar->drawTextScreen(215, 165, "%c%s", 27, K1.c_str());
	Broodwar->drawTextScreen(315, 165, "%c%s", 27, K2.c_str());
}
int Dragoons;
int tankOuterSplashRadius;
int mm, ss;
int allSeconds;

int AnyRace_CoachAI::parseTime(string str)
{
	if (str == "")
		return 0;
	try
	{
		mm = std::stoi(str.substr(0, 2), nullptr, 10);
		ss = std::stoi(str.substr(3, 2), nullptr, 10);
	}
	catch (exception e)
	{
		return 0;
	}

	allSeconds = mm * 60 + ss;
	return allSeconds;
}

void AnyRace_CoachAI::custom()
{
	if (Broodwar->isReplay())
		return;
	if (Broodwar->elapsedTime() * 0.6718 > 540 && Broodwar->elapsedTime() * 0.6718 < 550)
		Broodwar->sendText("@9min reassign #0,#9,#8, build 4th base (10:00)", Colors::Red);

	Dragoons = Broodwar->self()->allUnitCount(UnitTypes::Protoss_Dragoon);

	if (Dragoons > 7 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Facility) < 2)
	{
		Broodwar->sendText("on 8 Dragoons -> 2nd Robo, Stargate @7:00 (~3 scouts + ~3 corsairs_DW) RP all buildings ahead.", Colors::Red);
	}

	//tankOuterSplashRadius = WeaponTypes::Arclite_Shock_Cannon.outerSplashRadius();
	//for (auto &u : Broodwar->self()->getUnits())
	//{
	//	UnitType ut = u->getType();
	//	if (ut.canAttack() && !ut.isFlyer())// draw tank outer splash radius on ground units
	//	{
	//		Broodwar->drawTextMap(u->getPosition(), "%cTank Splash = %d", 8, tankOuterSplashRadius);
	//		Broodwar->drawCircleMap(u->getPosition(), tankOuterSplashRadius, 8, false);
	//	}

	//	if (ut.canAttack() && u->isCompleted() && !ut.isWorker())// && ut.groundWeapon() && u->canAttack()
	//	{
	//		int extra = 0;
	//		if (u->getTransport() != nullptr && u->getTransport()->getType() == UnitTypes::Terran_Bunker)
	//			extra = 32;

	//		groundRange = extra + Broodwar->self()->weaponMaxRange(u->getType().groundWeapon());
	//		if (groundRange)
	//		{
	//			Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y - 10, "%cGround range = %d", 8, groundRange / 32);
	//			Broodwar->drawCircleMap(u->getPosition(), groundRange, 8, false);
	//		}

	//		airRange = extra + Broodwar->self()->weaponMaxRange(ut.airWeapon());
	//		if (airRange)
	//		{
	//			Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y - 20, "%cAir range = %d", 31, airRange / 32);
	//			Broodwar->drawCircleMap(u->getPosition(), airRange, 31, false);
	//		}
	//	}
	//}
	//for (auto &u : Broodwar->enemy()->getUnits())
	//{
	//	if (u->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)// && ut.groundWeapon() && u->canAttack()
	//	{
	//		groundRange = Broodwar->enemy()->weaponMaxRange(u->getType().groundWeapon());
	//		Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y - 10, "%cGround range = %d", 8, groundRange / 32);
	//		Broodwar->drawCircleMap(u->getPosition(), groundRange, 8, false);
	//	}
	//}
}
void AnyRace_CoachAI::hotKeyHandler()
{
	bool shift = Broodwar->getKeyState(Key::K_SHIFT);

	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_1))
		hkAll["1"] = getHotKeyGroup(selectedUnits, shift, "1");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_2))
		hkAll["2"] = getHotKeyGroup(selectedUnits, shift, "2");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_3))
		hkAll["3"] = getHotKeyGroup(selectedUnits, shift, "3");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_4))
		hkAll["4"] = getHotKeyGroup(selectedUnits, shift, "4");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_5))
		hkAll["5"] = getHotKeyGroup(selectedUnits, shift, "5");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_6))
		hkAll["6"] = getHotKeyGroup(selectedUnits, shift, "6");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_7))
		hkAll["7"] = getHotKeyGroup(selectedUnits, shift, "7");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_8))
		hkAll["8"] = getHotKeyGroup(selectedUnits, shift, "8");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_9))
		hkAll["9"] = getHotKeyGroup(selectedUnits, shift, "9");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_0))
		hkAll["0"] = getHotKeyGroup(selectedUnits, shift, "0");
	if (shift && Broodwar->getKeyState(Key::K_F3))
		hkAll["F3"] = "Set";
	if (shift && Broodwar->getKeyState(Key::K_F4))
		hkAll["F4"] = "Set";

	list<Key> keysList = { Key::K_1, Key::K_2, Key::K_3, Key::K_4, Key::K_5, Key::K_6, Key::K_7, Key::K_8, Key::K_9, Key::K_0,
		Key::K_F2, Key::K_F3, Key::K_F4 };

	static int lastCheckedFrame12 = 0;
	if (lastCheckedFrame12 < FrameCount)
	{
		lastCheckedFrame12 = FrameCount + 2;

		for (Key &k : keysList)
		{
			if (Broodwar->getKeyState(k) && !Broodwar->getKeyState(Key::K_CONTROL) && !Broodwar->getKeyState(Key::K_SHIFT)
				&& hkAll[convertAsciiToString(k)].size() > 0)
				keysPressed.push_back(k);
		}
	}
	map<string, int> keysPressedMap;
	keysPressedMap["1"];
	keysPressedMap["2"];
	keysPressedMap["3"];
	keysPressedMap["4"];
	keysPressedMap["5"];
	keysPressedMap["6"];
	keysPressedMap["7"];
	keysPressedMap["8"];
	keysPressedMap["9"];
	keysPressedMap["0"];
	keysPressedMap["F2"];
	keysPressedMap["F3"];
	keysPressedMap["F4"];
	for (auto k : keysPressed)
		keysPressedMap[convertAsciiToString(k)]++;

	if (F5_Pressed == 0)
	{
		int y = 35;
		for (auto entry : keysPressedMap)
		{
			Broodwar->drawTextScreen(125, y, "%c%d", Text::Orange, entry.second);
			y += 10;
		}
	}
}

Unitset usTemp;
string hkeyTemp;
std::string AnyRace_CoachAI::getHotKeyGroup(Unitset us, bool shift, string hkey)
{
	if (usTemp == us && hkeyTemp == hkey)
		return hkAll[hkey];

	list<string> hkSelfUnitsList;
	string hkSelfString = "";
	map<string, int> hkSelfUnitsMap;

	for (Unit u : us)
	{
		if (u->getType().isBuilding() && shift && hkAll[hkey].size() != 0)//don't add building to a used hotkey
			return hkAll[hkey];
		hkSelfUnitsList.push_back(formated(u->getType()));
	}

	if (shift)//A trivial display bug here where you can add unit(s) to a building
	{
		list<string> old;
		for (auto entry : hkAllRaw[hkey])
			old.push_back(entry);
		int allowedSize = 12 - old.size();

		if (old.size() + hkSelfUnitsList.size() > 12)
			hkSelfUnitsList.resize(allowedSize);
		old.sort();
		hkSelfUnitsList.sort();

		hkSelfUnitsList.merge(old);
	}
	hkAllRaw[hkey] = hkSelfUnitsList;

	for (string item : hkSelfUnitsList)
		hkSelfUnitsMap[item]++;
	for (auto entry : hkSelfUnitsMap)
		hkSelfString += to_string(entry.second) + " " + entry.first + ", ";

	usTemp = us;
	hkeyTemp = hkey;
	return hkSelfString;
}

string AnyRace_CoachAI::convertAsciiToString(BWAPI::Key & k)
{
	string kStr;
	int kk = k;
	if (kk > 47 && kk < 58)
	{
		kk -= 48;
		kStr = to_string(kk);
	}
	else if (k == 113)
		kStr = "F2";
	else if (k == 114)
		kStr = "F3";
	else if (k == 115)
		kStr = "F4";
	return kStr;
}

void AnyRace_CoachAI::populatePage()
{
	int posIndex = distance(iter, pos);
	str1 = ""; str2 = "";
	for (pos; distance(iter, pos) < posIndex + 22 && pos != multiMap.end(); pos++)
	{
		str1 += (*pos).first + "\n\r";
		str2 += (*pos).second + "\n\r";
	}
}

void AnyRace_CoachAI::Replay()
{
	ctrl = Broodwar->getKeyState(Key::K_CONTROL);
	F5 = Broodwar->getKeyState(Key::K_F5);
	F6 = Broodwar->getKeyState(Key::K_F6);
	F7 = Broodwar->getKeyState(Key::K_F7);
	F11 = Broodwar->getKeyState(Key::K_F11);

	static int lastCheckedFrame4 = 0;
	if (F5 && lastCheckedFrame4 < FrameCount)
	{
		lastCheckedFrame4 = FrameCount + FPS / 6;
		F5_Pressed++;
		if (F5_Pressed == 4)
			F5_Pressed = 0;
	}
	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 < FrameCount)
	{
		lastCheckedFrame5 = FrameCount + FPS / 6;
		F6_Pressed++;
		if (F6_Pressed == 2)
			F6_Pressed = 0;
	}

	static int lastCheckedFrame15 = 0;
	if (F7 && lastCheckedFrame15 < FrameCount)
	{
		lastCheckedFrame15 = FrameCount + FPS / 6;
		F7_Pressed++;
		if (F7_Pressed == 2)
			F7_Pressed = 0;
	}

	static int lastCheckedFrame7 = 0;
	if (F11 && lastCheckedFrame7 < FrameCount)
	{
		lastCheckedFrame7 = FrameCount + FPS / 6;
		F11_Pressed++;
		if (F11_Pressed == 2)
			F11_Pressed = 0;
	}

	//list<Event> events = Broodwar->getEvents();//a bug when this code below getLatencyFrames(), maybe cause skipping the key frame.
	//for (auto e : events)
	//{
		//if (e.getType() == EventType::UnitComplete && e.getUnit()->getType().getName() == "Spell_Scanner_Sweep")
		//{
		//if (e.getType() != EventType::MatchFrame)
		//	eve.push_back(e.getType() + ", " + e.getText());
		//}
	//}

	Bulletset bulls = Broodwar->getBullets();
	for (auto b : bulls)
	{
		if (b->getType() == BulletTypes::Psionic_Storm && j["Control Panel"]["replayAutoMoveToScanOrStorm"].get<bool>())
		{
			Broodwar->setScreenPosition(b->getPosition());
			int x = b->getPosition().x - 280;
			int y = b->getPosition().y - 180;
			Broodwar->setScreenPosition(x, y);
		}
	}

	//string path = Broodwar->mapPathName();
	//std::wstring repPath(path.begin(), path.end());

	//SYSTEMTIME stUTC, stLocal;
	//WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
	//GetFileAttributesExW(repPath.c_str(), GetFileExInfoStandard, &attr);
	//FILETIME ftLastModifiedDate = attr.ftLastWriteTime;

	//FileTimeToSystemTime(&ftLastModifiedDate, &stUTC);
	//SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	//Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s\n\r \t\t\t\t%02d/%02d/%d %02d:%02d", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7,
	//	getTime(ReplaySeconds).c_str(), stLocal.wDay, stLocal.wMonth, stLocal.wYear, stLocal.wHour, stLocal.wMinute);

	if (screpOK)
	{
		Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s\n\r  %c%s / %c%s / %s", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7,
			getTime(ReplaySeconds).c_str(), Text::Teal, repDate.c_str(), 29, repGameTitle.c_str(), repType.c_str());
	}
	else
		Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7,
			getTime(ReplaySeconds).c_str());

	list<Player> playersList;
	for (auto p : players)
	{
		if (!p->isNeutral() && !p->isObserver() && p->spentMinerals() != 0)
			playersList.push_back(p);
	}

	list<string> forceList;
	int y = 205;
	int y1 = 105;
	int pid;
	for (auto p : playersList)
	{
		string race = p->getRace().getName().substr(0, 1);
		string name = p->getName();
		pid = p->getID();

		if (p->minerals() > 750)
			mineralsAbove[pid + 16]++;
		if (p->minerals() > 1000)
			mineralsAbove[pid + 24]++;

		char color = p->getTextColor();

		idleWorkers = 0;
		idleWorkersFor = 0;
		inProgressWorkers = 0;
		notIdleResourceDepot = 0;
		workersProductionStopped = 0;
		string upTech, freq, pattern, nonPattern, delimit;

		for (auto u : p->getUnits())
		{
			if (u->getType().isBuilding() && u->isCompleted() && (u->isUpgrading() || u->isResearching()))
			{
				if (u->isUpgrading())
				{
					if (u->getUpgrade().maxRepeats() > 1)
						upTech = formated(u->getUpgrade().getName()) + "+" + to_string(p->getUpgradeLevel(u->getUpgrade()) + 1);
					else
						upTech = formated(u->getUpgrade().getName());
				}
				if (u->isResearching())
					upTech = u->getTech().getName();

				pattern = "#" + to_string(u->getID()) + upTech;
				nonPattern = "#" + to_string(u->getID()) + "None";

				//doesn't conatain that pattern
				// goon range happend before, if this building has been idle for whatever time after the pattern has been logged, do it.
				if (!strstr(upgradeTechReplayUnitIDs.c_str(), pattern.c_str()))
				{

					upgradeTechReplayUnitIDs += pattern;
					if (p->supplyUsed() / 2 < replayLogSupplyFor)
						freq = gameTime + " " + to_string(p->supplyUsed() / 2) + "," + upTech + ".isTechOrUpgrade";
					else
						freq = gameTime + " " + upTech + ".isTechOrUpgrade";
					switch (pid)
					{
					case 0: macroLogP0.push_back(freq); break;
					case 1: macroLogP1.push_back(freq); break;
					case 2: macroLogP2.push_back(freq); break;
					case 3: macroLogP3.push_back(freq); break;
					case 4: macroLogP4.push_back(freq); break;
					case 5: macroLogP5.push_back(freq); break;
					case 6: macroLogP6.push_back(freq); break;
					case 7: macroLogP7.push_back(freq); break;
					default:
						break;
					}
				}
				else
				{
					if (strstr(upgradeTechReplayUnitIDs.c_str(), nonPattern.c_str()))
					{
						upgradeTechReplayUnitIDs.erase(upgradeTechReplayUnitIDs.find(nonPattern), nonPattern.length());
						if (upgradeTechReplayUnitIDs.find(pattern) != string::npos && !strstr(pattern.c_str(), "None"))
							upgradeTechReplayUnitIDs.erase(upgradeTechReplayUnitIDs.find(pattern), pattern.length());
					}
				}
			}
			//UnitType ut = u->getType();
			//if (ut.canAttack() && u->isCompleted() && !ut.isWorker())// && ut.groundWeapon() && u->canAttack()
			//{
			//	int extra = 0;
			//	if (u->getTransport() != nullptr && u->getTransport()->getType() == UnitTypes::Terran_Bunker)
			//		extra = 32;

			//	groundRange = extra + p->weaponMaxRange(u->getType().groundWeapon());
			//	if (groundRange)
			//	{
			//		Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y - 10, "%cGround range = %d", 8, groundRange / 32);
			//		Broodwar->drawCircleMap(u->getPosition(), groundRange, 8, false);
			//	}

			//	airRange = extra + p->weaponMaxRange(ut.airWeapon());
			//	if (airRange)
			//	{
			//		Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y - 20, "%cAir range = %d", 31, airRange / 32);
			//		Broodwar->drawCircleMap(u->getPosition(), airRange, 31, false);
			//	}
			//}

			if (u->getType().isResourceDepot() && !u->isIdle() && u->isCompleted()) //working ResourceDepot
				notIdleResourceDepot++;
			if (u->getType().isWorker() && u->isCompleted() && u->isIdle()) //idle worker
			{
				Broodwar->drawCircleMap(u->getPosition().x, u->getPosition().y, 8, Text::BrightRed, true);
				idleWorkers++;
				idleWorkersFor++;
			}
			if (u->getType().isWorker() && !u->isCompleted()) //in progress worker
				inProgressWorkers++;
		}
		for (auto u : p->getUnits())//part2
		{
			if (p->getRace() != Races::Zerg && u->getType().isResourceDepot() && u->isIdle() && Broodwar->elapsedTime() *  0.6718 < workersProductionStoppedSeconds)
				workersProductionStopped++;

			if (p->getRace() != Races::Zerg && u->getType().isResourceDepot() && !u->isIdle() && Broodwar->elapsedTime() *  0.6718 < workersProductionStoppedSeconds)
			{
				workersProductionStopped += notIdleResourceDepot - inProgressWorkers;
				if (notIdleResourceDepot > 1)
				{
					notIdleResourceDepot = 0;
					inProgressWorkers = 0;
				}
			}
		}
		if (idleWorkers >= idleWorkersMaxAll[pid] && Broodwar->elapsedTime() * 0.6718 > 2)
		{
			idleWorkersMaxAll[pid] = idleWorkers;
			idleWorkersMaxTimeAll[pid] = gameTime;
		}

		idleTime = getTime((idleWorkersForAll[pid] += idleWorkersFor) / FPS);

		if (Broodwar->elapsedTime() * 0.6718 < workersProductionStoppedSeconds)
		{
			workerCut = getTime((workersProductionStoppedAll[pid] += workersProductionStopped) / FPS);
			//Broodwar->setLocalSpeed(42);
		}
		else workerCut = getTime(workersProductionStoppedAll[pid] / FPS);

		if (F5_Pressed < 2)
		{
			if (Broodwar->elapsedTime() * 0.6718 > workersProductionStoppedSeconds)
				Broodwar->drawTextScreen(5, 195, "==>IdleWorkers/Time, MaxIdle/When, %c%s passed !", 8, getTime(workersProductionStoppedSeconds).c_str());
			else Broodwar->drawTextScreen(5, 195, "==>IdleWorkers/Time, MaxIdle/When, WorkersCut");

			Broodwar->drawTextScreen(5, y, "%c(%s) %s: %d/%s,   %d/%s,   %s", color, race.c_str(), name.c_str(),
				idleWorkersAll[pid] = idleWorkers, idleTime.c_str(), idleWorkersMaxAll[pid],
				idleWorkersMaxTimeAll[pid].c_str(),
				workerCut.c_str());
			y += 10;
		}
		string forceName = p->getForce()->getName();
		string pName = p->getName();

		if (F5_Pressed == 0)
			Broodwar->drawTextScreen(5, 95, "%c==>Minerals spent + %cGas spent = %cMacro", 31, 7, 17);
		if (F5_Pressed == 1)
			Broodwar->drawTextScreen(5, 95, "Produced/killed/lost %cUnits+%cBuildings=%cState/Score", 31, 7, 17);

		if (forceName == "" || forceName == "Random Teams")// no team players
		{
			int Minerals = p->spentMinerals() - p->refundedMinerals();
			int Gas = p->spentGas() - p->refundedGas();
			char color2 = 15;

			Player pTemp;
			string allies = "allied ";
			for (int i = 0; i < playersList.size(); i++)
			{
				pTemp = Broodwar->getPlayer(i);
				if (p != pTemp && p->isAlly(pTemp) && !pTemp->leftGame())
				{
					isThereAllies = true;
					string pTempName = pTemp->getName().substr(0, 3);
					allies.append(pTempName + " ");
					color2 = pTemp->getTextColor();
					if (color2 == 2)
						color2 = 17;
				}
			}

			if (!isThereAllies)
				allies = "";

			if (F5_Pressed == 0)
			{
				Broodwar->drawTextScreen(5, y1, "%c(%s) %s %c%s: %c%d+%c%d=%c%d %c%d/%d",
					color, p->getRace().getName().substr(0, 1).c_str(), p->getName().substr(0, 3).c_str(), color2, allies.c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas, 15, p->supplyUsed() / 2, p->supplyTotal() / 2);
				y1 += 10;
			}
			if (F5_Pressed == 1)
			{
				int uScore = p->getUnitScore();
				int bScore = p->getBuildingScore();
				Broodwar->drawTextScreen(5, y1, "%c(%s) %s : %c%d+%c%d=%c%d",
					color, p->getRace().getName().substr(0, 1).c_str(), p->getName().substr(0, 3).c_str(),
					31, uScore, 7, bScore, 17, bScore + uScore);
				y1 += 10;
			}
		}
		else
		{	//team
			if (!(find(forceList.begin(), forceList.end(), forceName) != forceList.end()))
			{	// if this force not looped-through.
				for (auto f_p : p->getForce()->getPlayers())
				{
					int Minerals = f_p->spentMinerals() - f_p->refundedMinerals();
					int Gas = f_p->spentGas() - f_p->refundedGas();
					char color = f_p->getTextColor();

					if (F5_Pressed == 0)
					{
						Broodwar->drawTextScreen(5, y1, "%c(%s) %s: %c%d+%c%d=%c%d %c%d/%d", color, f_p->getRace().getName().substr(0, 1).c_str(), f_p->getName().c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas, 15, f_p->supplyUsed() / 2, f_p->supplyTotal() / 2);
						y1 += 10;
					}
					if (F5_Pressed == 1)
					{
						int uScore = f_p->getUnitScore();
						int bScore = f_p->getBuildingScore();
						Broodwar->drawTextScreen(5, y1, "%c(%s) %s : %c%d+%c%d=%c%d",
							color, f_p->getRace().getName().substr(0, 1).c_str(), f_p->getName().substr(0, 3).c_str(),
							31, uScore, 7, bScore, 17, bScore + uScore);
						y1 += 10;
					}
				}

				if (F5_Pressed < 2)
					Broodwar->drawTextScreen(0, y1, "\n\r");

				y1 += 10;
				forceList.push_back(forceName);
				forceList.unique();
			}
		}
	}

	Position unitPos;
	Position orderPos;
	string orderName;
	UnitType ut;
	int left, right, up, down;
	for (auto &u : Broodwar->getAllUnits())
	{
		ut = u->getType();
		Color color = u->getPlayer()->getColor();
		if (ut.isBuilding())
		{
			if (F11_Pressed > 0)
			{
				left = ut.tileWidth() * 16 - ut.dimensionLeft();
				right = ut.tileWidth() * 16 - ut.dimensionRight() - 1;
				up = ut.tileHeight() * 16 - ut.dimensionUp();
				down = ut.tileHeight() * 16 - ut.dimensionDown() - 1;

				Broodwar->drawCircleMap(u->getPosition().x - 1, u->getPosition().y + 20, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x - 1, u->getPosition().y - 20, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x + 19, u->getPosition().y, 7, 20, true);
				Broodwar->drawCircleMap(u->getPosition().x - 21, u->getPosition().y, 7, 20, true);

				Broodwar->drawTextMap(u->getPosition().x - 5, u->getPosition().y + 15, "%d", down);
				Broodwar->drawTextMap(u->getPosition().x - 5, u->getPosition().y - 25, "%d", up);
				Broodwar->drawTextMap(u->getPosition().x + 15, u->getPosition().y - 5, "%d", right);
				Broodwar->drawTextMap(u->getPosition().x - 25, u->getPosition().y - 5, "%d", left);
			}

			if (!replayCenteredOnNexus && ut.getRace() == Races::Protoss && ut.isResourceDepot())
			{
				int x = u->getPosition().x - 280;
				int y = u->getPosition().y - 180;
				Broodwar->setScreenPosition(x, y);
				replayCenteredOnNexus = true;
				int dummy = 0;
			}

			Position building = u->getPosition();
			Position RP = u->getRallyPosition();
			if (RP.isValid() && RP.y != 0)
			{
				Broodwar->drawLineMap(building, RP, Text::BrightRed);
				Broodwar->drawCircleMap(RP.x, RP.y, 5, Text::BrightRed, true);
			}
		}
		else
		{
			if (F11_Pressed > 0)
			{
				if (!ut.isFlyer() && u->getTransport() == nullptr && (u->isCompleted() || ut.getName() == "Zerg_Egg"))
					Broodwar->drawTextMap(u->getPosition(), "%dx%d", ut.width(), ut.height());
			}
			else
			{
				unitPos = u->getPosition();
				orderPos = u->getOrderTargetPosition();
				orderName = u->getOrder().getName();

				if (unitPos.isValid() && unitPos.y != 0)
				{
					if (orderPos.isValid() && orderPos.y != 0)
					{
						if (orderName != "MiningMinerals" && orderName != "HarvestGas")
						{
							Broodwar->drawLineMap(unitPos, orderPos, 8);
							Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, 8, true);
							Broodwar->drawTextMap(unitPos.x, unitPos.y + 10, "%s", orderName.c_str());
						}
					}
					else
					{
						if (orderName == "HoldPosition")
							Broodwar->drawTextMap(unitPos.x, unitPos.y + 10, "Hold");
					}
				}
			}
		}
	}

	list<string> listOfUnits;
	list<string> listOfBuildings;
	list<string> listOfInCompleteUnits;
	list<string> listOfInCompleteBuildings;
	map<string, int> UnitsMap;
	map<string, int> BuildingsMap;
	string BuildingsFinal;
	string UnitsFinal;
	map<string, int> inCompleteUnitsMap;
	map<string, int> inCompleteBuildingsMap;
	string inCompleteBuildingsFinal;
	string inCompleteUnitsFinal;

	for (auto unit : Broodwar->getSelectedUnits())
	{
		Player plSelected = unit->getPlayer();
		char color = plSelected->getTextColor();

		pid = plSelected->getID();

		if (F7_Pressed == 1 && plSelected == plSelectedTemp)
		{
			for (auto p : players)
			{
				if (p->isObserver() || (plSelected != Broodwar->neutral() && p != plSelected && !p->isAlly(plSelected)) || (p->spentMinerals() == 50 && p->supplyUsed() / 2 < 2))
					Broodwar->setVision(p, false);
			}
		}
		else
		{
			for (auto p : playersList)
				Broodwar->setVision(p, true);
		}

		Broodwar->drawTextScreen(590, 15, "%cFPS:%c%d", 14, 4, FPS);
		Broodwar->drawTextScreen(270, 15, "%c%s: Minerals above: 750: %c%s, %c1000: %c%s", color, plSelected->getName().c_str(),
			25, getTime(mineralsAbove[pid + 16] / FPS).c_str(), color, 25, getTime(mineralsAbove[pid + 24] / FPS).c_str());

		list<string> upgrades;
		list<string> techno;
		string ugName;
		list<UpgradeType> selectedPlayerUpgrades;
		for (auto ug : UpgradeTypes::allUpgradeTypes())
		{
			if (plSelected->getUpgradeLevel(ug))
				selectedPlayerUpgrades.push_back(ug);
		}

		UnitType UnitT;
		list<string> strList;
		for (auto upg : selectedPlayerUpgrades)
		{
			for (auto ut : upg.whatUses())
			{
				strList.push_back(ut.getName());
				UnitT = ut;
			}

			if (upg.maxRepeats() > 1)
				ugName = upg.getName() + " +" + to_string(plSelected->getUpgradeLevel(upg));
			else ugName = formated(UnitT) + "s " + upg.getName();

			//if ugName not exists
			auto iter = std::find(upgrades.begin(), upgrades.end(), ugName);
			if (iter != upgrades.end())
				;
			else
				upgrades.push_back(ugName);
		}
		for (auto tech : TechTypes::allTechTypes())
		{
			string techName = tech.getName();
			if (plSelected->hasResearched(tech) && tech.whatResearches() != UnitTypes::None)
			{
				auto iter = std::find(techno.begin(), techno.end(), techName);
				if (iter != techno.end())
					;
				else
					techno.push_back(techName);
			}
		}
		string upgradesFinal;
		for (auto entry : upgrades)
			upgradesFinal += entry + "\n\r";
		string technoFinal;
		for (auto entry : techno)
			technoFinal += entry + "\n\r";

		for (auto u : plSelected->getUnits())
		{
			Unitset baseMineralsWorkers;
			int px;
			if (u->getType().isResourceDepot())
			{
				Position p = u->getPosition();

				baseMineralsWorkers = Broodwar->getUnitsInRadius(p, 300, IsWorker && IsGatheringMinerals); // && isOwned ?
				for (auto worker : baseMineralsWorkers)
				{
					if (worker->getPlayer()->getID() != plSelected->getID())
						baseMineralsWorkers.erase(worker);
				}

				if (baseMineralsWorkers.size() < 10)
					px = p.x - 2;
				else px = p.x - 5;
				Broodwar->drawCircleMap(p, 10, Colors::Black, true);
				Broodwar->drawTextMap(px, p.y - 5, "%d", baseMineralsWorkers.size());
			}
			//unit & building & upgrade infos:
			UnitType ut = u->getType();
			if (!ut.isBuilding())
			{
				if (u->isCompleted())
					listOfUnits.push_back(formated(ut));
				else
				{
					if (ut.getName() == "Zerg_Egg")
						ut = u->getBuildType();

					if (F6_Pressed == 1)
						listOfInCompleteUnits.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
					else listOfInCompleteUnits.push_back(formated(ut));
				}
			}
			else
			{
				if (u->isCompleted())
				{
					listOfBuildings.push_back(formated(ut));

					if (u->isUpgrading())
						upgradesFinal += u->getUpgrade().getName() + " " + to_string(u->getRemainingUpgradeTime() / FPS) + "\n\r";
					if (u->isResearching())
						technoFinal += u->getTech().getName() + " " + to_string(u->getRemainingResearchTime() / FPS) + "\n\r";
				}
				else
				{
					if (F6_Pressed == 1)
						listOfInCompleteBuildings.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS + 1));
					else listOfInCompleteBuildings.push_back(formated(ut));
				}
			}
		}
		for (string item : listOfUnits)
			UnitsMap[item]++;
		for (string item : listOfBuildings)
			BuildingsMap[item]++;
		for (auto entry : BuildingsMap)
			BuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
		for (auto entry : UnitsMap)
			UnitsFinal += to_string(entry.second) + " " + entry.first + "\n\r";


		//pl->allUnitCount(); //interesting, no need for p.getUnits()
		int dead = plSelected->deadUnitCount(unit->getType());
		if (dead > 0)
			Broodwar->drawTextMap(unit->getPosition().x, unit->getBottom() + 35, "%c%d %s lost", color, dead, formated(unit->getType()).c_str());

		//InCompleteUnits & upgrades mode (final process):
		if (F6_Pressed == 1)
		{
			for (string item : listOfInCompleteUnits)
				inCompleteUnitsFinal += item + "\n\r";
			for (string item : listOfInCompleteBuildings)
				inCompleteBuildingsFinal += item + "\n\r";

			Broodwar->drawTextScreen(440, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 24, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 24, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
		}
		else
		{
			for (string item : listOfInCompleteUnits)
				inCompleteUnitsMap[item]++;
			for (string item : listOfInCompleteBuildings)
				inCompleteBuildingsMap[item]++;
			for (auto entry : inCompleteBuildingsMap)
				inCompleteBuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
			for (auto entry : inCompleteUnitsMap)
				inCompleteUnitsFinal += to_string(entry.second) + " " + entry.first + "\n\r";

			Broodwar->drawTextScreen(440, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 28, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 28, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
		}
		if (F5_Pressed != 2)
			Broodwar->drawTextScreen(270, 25, "%cTech:\n\r%c%s%cUpgrade:\n\r%c%s", Text::Blue, Text::Teal, technoFinal.c_str(), Text::Blue, Text::Teal, upgradesFinal.c_str());

		//replay BO log:
		if (F5_Pressed == 3)
		{
			switch (plSelected->getID())
			{
			case 0: macroLogP = macroLogP0; break;
			case 1: macroLogP = macroLogP1; break;
			case 2: macroLogP = macroLogP2; break;
			case 3: macroLogP = macroLogP3; break;
			case 4: macroLogP = macroLogP4; break;
			case 5: macroLogP = macroLogP5; break;
			case 6: macroLogP = macroLogP6; break;
			case 7: macroLogP = macroLogP7; break;
			default:
				break;
			}
			int y1 = 105, y2 = 15;
			string step, type;

			if (ctrl)
			{
				if (macroLogP.size() > 48)
				{
					for (int i = 48; i < macroLogP.size(); i++)
					{
						int color1 = j["Preset Plan"]["ReplayBo Color1"].get<int>();
						int color2 = j["Preset Plan"]["ReplayBo Color2"].get<int>();
						if (i < 66)
						{
							if (strstr(macroLogP[i].c_str(), "."))
							{
								step = macroLogP[i].substr(0, macroLogP[i].find("."));
								type = macroLogP[i].substr(macroLogP[i].find(".") + 1, macroLogP[i].size());

								if (type == "isResourceDepot")
									color1 = 31;
								if (type == "isBuilding")
									color1 = color2;
								if (type == "isTechOrUpgrade")
									color1 = 3;

								Broodwar->drawTextScreen(5, y1, "%c%s\n", color1, step.c_str());
							}
							else
								Broodwar->drawTextScreen(5, y1, "%c%s\n", color1, macroLogP[i].c_str());
							y1 += 10;
						}
						else // > 66
						{
							if (strstr(macroLogP[i].c_str(), "."))
							{
								step = macroLogP[i].substr(0, macroLogP[i].find("."));
								type = macroLogP[i].substr(macroLogP[i].find(".") + 1, macroLogP[i].size());

								if (type == "isResourceDepot")
									color1 = 31;
								if (type == "isBuilding")
									color1 = color2;
								if (type == "isTechOrUpgrade")
									color1 = 3;

								Broodwar->drawTextScreen(140, y2, "%c%s\n", color1, step.c_str());
							}
							else
								Broodwar->drawTextScreen(140, y2, "%c%s\n", color1, macroLogP[i].c_str());
							y2 += 10;
						}
					}
				}
			}
			else // < 48
			{
				for (int i = 0; i < macroLogP.size(); i++)
				{
					int color1 = j["Preset Plan"]["ReplayBo Color1"].get<int>();
					int color2 = j["Preset Plan"]["ReplayBo Color2"].get<int>();

					if (i < 18)
					{
						if (strstr(macroLogP[i].c_str(), "."))
						{
							step = macroLogP[i].substr(0, macroLogP[i].find("."));
							type = macroLogP[i].substr(macroLogP[i].find(".") + 1, macroLogP[i].size());

							if (type == "isResourceDepot")
								color1 = 31;
							if (type == "isBuilding")
								color1 = color2;
							if (type == "isTechOrUpgrade")
								color1 = 3;

							Broodwar->drawTextScreen(5, y1, "%c%s\n", color1, step.c_str());
						}
						else
							Broodwar->drawTextScreen(5, y1, "%c%s\n", color1, macroLogP[i].c_str());
						y1 += 10;
					}
					if (i >= 18 && i < 48)
					{
						if (strstr(macroLogP[i].c_str(), "."))
						{
							step = macroLogP[i].substr(0, macroLogP[i].find("."));
							type = macroLogP[i].substr(macroLogP[i].find(".") + 1, macroLogP[i].size());

							if (type == "isResourceDepot")
								color1 = 31;
							if (type == "isBuilding")
								color1 = color2;
							if (type == "isTechOrUpgrade")
								color1 = 3;

							Broodwar->drawTextScreen(140, y2, "%c%s\n", color1, step.c_str());
						}
						else
							Broodwar->drawTextScreen(140, y2, "%c%s\n", color1, macroLogP[i].c_str());
						y2 += 10;

						if (i == 47)
						{
							Broodwar->drawTextScreen(140, y2, "--> Press CTRL to continue...");
							break;
						}
					}
				}
			}
			Broodwar->drawTextScreen(5, 95, "Build order:");
		}
		plSelectedTemp = plSelected;
		break;
	}
}
void AnyRace_CoachAI::buildSupply(const BWAPI::Unit & u, BWAPI::UnitType &supplyUnit)
{
	// previously:lastErr == Errors::Insufficient_Supply ---> was below
	if (supplyAvailable / 2 <= autoBuildSuppliesBeforeBlocked && lastChecked < FrameCount && Broodwar->self()->minerals() >= 100
		&& Broodwar->self()->incompleteUnitCount(supplyUnit) == 0)
	{
		lastChecked = FrameCount + FPS * 10;// 600 = #frames for Overload to finish (600 * 0.042 = 25s) or FPS * 25

		Probe_SCV_Larva = u->getClosestUnit(GetType == supplyUnit.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);

		if (Probe_SCV_Larva != nullptr) //If a unit was found (Probe_SCV_Larva)
		{
			int i = Probe_SCV_Larva->getID();
			int inProgressSupplies = Broodwar->self()->incompleteUnitCount(supplyUnit);

			if (supplyUnit.isBuilding())
			{
				TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyUnit, Probe_SCV_Larva->getTilePosition());
				if (targetBuildLocation)
				{
					// Register an event that draws the target build location
					Broodwar->registerEvent([targetBuildLocation, supplyUnit](Game*) { Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyUnit.tileSize()), Colors::Blue); },
						nullptr, supplyUnit.buildTime());  // frames to run

					//========================
					//Build Pylon/Supply Depot:
					//========================
					Probe_SCV_Larva->build(supplyUnit, targetBuildLocation);
				}
			}
			//==============
			//Build Overload:
			//==============
			else
			{
				if (u->getType().getRace().getName() == "Zerg" && !OvIsMorphing)
				{
					Probe_SCV_Larva->morph(supplyUnit);// Train the supply provider (Overlord) if the provider is not a structure
					OvIsMorphing = Probe_SCV_Larva->getBuildType().getName() == "Zerg_Overlord";
				}
			}
		}
	}
}

void AnyRace_CoachAI::onSendText(string text)
{
	if (text == "c1")
	{
		Broodwar->sendText("%s", "show me the money");
		Broodwar << "show me the money" << endl;
	}
	else if (text == "c2")
	{
		Broodwar->sendText("%s", "black sheep wall");
		Broodwar << "black sheep wall" << endl;
	}
	else if (text == "c3")
	{
		Broodwar->sendText("%s", "operation cwal");
		Broodwar << "operation cwal" << endl;
	}
	else if (text == "c4")
	{
		Broodwar->sendText("%s", "power overwhelming");
		Broodwar << "power overwhelming" << endl;
	}
	else if (text == "c5")
	{
		Broodwar->sendText("%s", "staying alive");
		Broodwar << "staying alive" << endl;
	}
	else if (text == "c6")
	{
		Broodwar->sendText("%s", "there is no cow level");
		Broodwar << "there is no cow level" << endl;
	}
	else if (text == "?")
	{
		if (screpOK)
		{
			string cmd2 = "bwapi-data\\screp.exe -cmds \"" + repPath + "\" > bwapi-data\\repInfo2.json";
			system(cmd2.c_str());
			ifstream u2("bwapi-data\\repInfo2.json");
			j3 = json::parse(u2);
			auto framesArray = j3["Commands"]["Cmds"];
			auto playersArray = j3["Header"]["Players"];

			string name = "";
			string origin = "Unknown";
			string timeHappened = "";
			for (auto it = framesArray.begin(); it != framesArray.end(); it++)
			{
				auto check = it.value()["Type"]["Name"];
				if (check == "Leave Game")
				{
					isLeave = true;
					int pid = it.value()["PlayerID"];
					timeHappened = getTime(it.value()["Frame"].get<int>() / 23.809523809523809523809523809524);

					for (auto it2 = playersArray.begin(); it2 != playersArray.end(); it2++)
					{
						if (it2.value()["ID"] == pid)
							name = it2.value()["Name"].get<string>();
					}
					Broodwar << Text::Align_Right << name << " leaves @" << timeHappened << endl;
				}

				auto check2 = it.value()["Message"];
				if (!check2.is_null())
				{
					int pid = it.value()["PlayerID"];
					for (auto it2 = playersArray.begin(); it2 != playersArray.end(); it2++)
					{
						if (it2.value()["ID"] == pid)
							origin = it2.value()["Name"].get<string>();
					}
				}


				if (check2 == "gg" || check2 == "GG")
				{
					isGG = true;
					int SlotID = it.value()["TargetPlayerID"];
					timeHappened = getTime(it.value()["Frame"].get<int>() / 23.809523809523809523809523809524);

					for (auto it2 = playersArray.begin(); it2 != playersArray.end(); it2++)
					{
						if (it2.value()["SlotID"] == SlotID)
							name = it2.value()["Name"].get<string>();
					}
					Broodwar << Text::Align_Right << name << " types gg @" << timeHappened << endl;
				}
			}

			if (!isLeave && !isGG)
				Broodwar << Text::Align_Right << "I don't know !" << endl;

			Broodwar << Text::Align_Right << "Replay origin: " << origin << endl;
		}
	}
	// Send the text to the game if it is not being processed.
	else Broodwar->sendText("%s", text.c_str());

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void AnyRace_CoachAI::onReceiveText(BWAPI::Player player, std::string text)
{
	//Parse the received text
	//Broodwar << player->getName() << " said \"" << text << "\"" << endl;
	if ((strstr(text.c_str(), "icCoachAI") || strstr(text.c_str(), "Enabled Flag") || strstr(text.c_str(), "is now live using")) && !strstr(text.c_str(), "<Nothing>"))
	{
		acknowledgedHumanEnemies.push_back(player->getName());
	}
}

void AnyRace_CoachAI::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	//Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void AnyRace_CoachAI::onNukeDetect(BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << endl;
		Broodwar->setScreenPosition(target);

		Broodwar->registerEvent([target](Game*) { Broodwar->drawCircleMap(target, 5, Colors::Red, false);
		Broodwar->drawCircleMap(target, 10, Colors::Red, false);
		Broodwar->drawCircleMap(target, 15, Colors::Red, false); },   // action
			nullptr,    // condition
			240);  // frames to run
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void AnyRace_CoachAI::onUnitDiscover(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onUnitEvade(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onUnitShow(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onUnitHide(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onUnitDestroy(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onUnitRenegade(BWAPI::Unit unit)
{
}

void AnyRace_CoachAI::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << endl;
}


void AnyRace_CoachAI::onUnitMorph(BWAPI::Unit unit)
{
	UnitType ut;
	if (unit->getType().getName() == "Zerg_Egg")
		ut = unit->getBuildType();
	else ut = unit->getType();
	string un = formated(ut);

	if (Broodwar->isReplay())
	{
		Player p = unit->getPlayer();

		if ((p->getRace().getName() == "Zerg" && unit->getType().getName() != "Zerg_Egg" && !ut.isBuilding()) || un == "Tank_S")
			return;

		bool UnitLog = true;
		if (ut.isWorker() && (Broodwar->elapsedTime() * 0.6718 > 25 || Broodwar->elapsedTime() * 0.6718 < 10))
			return;
		if (ut == p->getRace().getSupplyProvider() && p->supplyUsed() / 2 > replayLogSupplyFor)
			return;

		if (Broodwar->elapsedTime() * 0.6718 > replayLogUnitsFor) //log units until 1st 00:00, then only buildings
			UnitLog = ut.isBuilding();

		if (p->spentMinerals() != 0 && UnitLog)
		{
			if (ut.isResourceDepot() && un != "Lair" && un != "Hive")
				un += ".isResourceDepot";
			else
			{
				if (ut.isBuilding() && ut != p->getRace().getSupplyProvider())
					un += ".isBuilding";
			}

			string freq = gameTime + " " + un;
			if (p->supplyUsed() / 2 < replayLogSupplyFor)
				freq = gameTime + " " + to_string(p->supplyUsed() / 2) + "," + un;
			switch (p->getID())
			{
			case 0: macroLogP0.push_back(freq); break;
			case 1: macroLogP1.push_back(freq); break;
			case 2: macroLogP2.push_back(freq); break;
			case 3: macroLogP3.push_back(freq); break;
			case 4: macroLogP4.push_back(freq); break;
			case 5: macroLogP5.push_back(freq); break;
			case 6: macroLogP6.push_back(freq); break;
			case 7: macroLogP7.push_back(freq); break;
			default:
				break;
			}
		}
	}
	else
	{
		if (unit->getPlayer() == Broodwar->self()) //check own unit
		{
			if (unit->getType().getName() == "Zerg_Overlord")
				OvIsMorphing = false;
			if (unit->getType().getName() == "Zerg_Drone")
				droneIsMorphing = false;

			if (!logWorkersAndSupplyProduction && (ut.isWorker() || ut == Broodwar->self()->getRace().getSupplyProvider()))
				return;
			else if (!logUnitsProduction && (!ut.isWorker() && ut != Broodwar->self()->getRace().getSupplyProvider() && !ut.isBuilding()))
				return;
			else
				macroLogMap[un + " #" + to_string(unit->getID())] = gameTime;
		}
	}
}

void AnyRace_CoachAI::onUnitCreate(BWAPI::Unit unit)//Note: Protoss_Assimilator morphs from Geyser !, so its not here.
{
	UnitType ut = unit->getType();
	string un = formated(ut);

	if (Broodwar->isReplay())
	{
		if (ut.getName() == "Spell_Scanner_Sweep" && j["Control Panel"]["replayAutoMoveToScanOrStorm"].get<bool>())
		{
			int x = unit->getPosition().x - 280;
			int y = unit->getPosition().y - 180;
			Broodwar->setScreenPosition(x, y);
			Broodwar << "Comsat scan here !" << endl;
		}
		Player p = unit->getPlayer();

		bool UnitLog = true;
		if (ut.isWorker() && (Broodwar->elapsedTime() * 0.6718 > 25 || Broodwar->elapsedTime() * 0.6718 < 10))
			return;
		if (ut == p->getRace().getSupplyProvider() && p->supplyUsed() / 2 > replayLogSupplyFor)
			return;

		if (Broodwar->elapsedTime() * 0.6718 > replayLogUnitsFor) //log units until 1st 00:00, then only buildings
			UnitLog = ut.isBuilding();

		if (p->spentMinerals() != 0 && UnitLog && un != "Larva"  && un != "Scarab" && un != "Zergling" && un != "Scourge")
		{
			if (ut.isResourceDepot())
				un += ".isResourceDepot";
			else
			{
				if (ut.isBuilding() && ut != p->getRace().getSupplyProvider())
					un += ".isBuilding";
			}
			string freq = gameTime + " " + un;
			if (p->supplyUsed() / 2 < replayLogSupplyFor)
				freq = gameTime + " " + to_string(p->supplyUsed() / 2) + "," + un;
			switch (p->getID())
			{
			case 0: macroLogP0.push_back(freq); break;
			case 1: macroLogP1.push_back(freq); break;
			case 2: macroLogP2.push_back(freq); break;
			case 3: macroLogP3.push_back(freq); break;
			case 4: macroLogP4.push_back(freq); break;
			case 5: macroLogP5.push_back(freq); break;
			case 6: macroLogP6.push_back(freq); break;
			case 7: macroLogP7.push_back(freq); break;
			default:
				break;
			}
		}
	}
	else // (FrameCount > 0 && unit->getPlayer() == Broodwar->self() && un != "Larva") //!unit->getPlayer()->isNeutral())
	{
		if (un != "Larva" && un != "Zergling" && un != "Scourge" && FrameCount > 0 && unit->getPlayer() == Broodwar->self())
		{
			if (!logWorkersAndSupplyProduction && (ut.isWorker() || ut == Broodwar->self()->getRace().getSupplyProvider()))
				return;
			else if (!logUnitsProduction && (!ut.isWorker() && ut != Broodwar->self()->getRace().getSupplyProvider() && !ut.isBuilding()))
				return;
			else
				macroLogMap[un + " #" + to_string(unit->getID())] = gameTime;
		}
	}
}

void AnyRace_CoachAI::onUnitComplete(BWAPI::Unit unit)
{
}
