#include "AnyRace_CoachAI.h"
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include "nlohmann\json.hpp"
#include "algorithm"
#include <windows.h>

using json = nlohmann::json;
using namespace BWAPI;
using namespace Filter;
using namespace std;

int workersProductionStoppedAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int idleWorkersAll[8];
int idleWorkersForAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int idleWorkersMaxAll[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
string idleWorkersMaxTimeAll[8] = { "", "", "", "", "", "", "", "" };
int mineralsAbove[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int groundRange, airRange;
int ReplaySeconds;
static int lastChecked;
map<string, string> hkAll;
map<string, list<string>> hkAllRaw;
list<Key> keysPressed;
int minerals_250;
int minerals_500;
int minerals_750;
int minerals_1000;

Position currScreen;
Position prevScreen;
double screenCounter = 0;
list<double> screenJumps;

Unitset selectedUnits;
Unitset prevSelectedUnits;
double selectedUnitsCounter = 0;
list<double> selectedUnitsJumps;

bool okUDAI;
Unit Probe_SCV_Larva;
bool OvIsMorphing;
bool droneIsMorphing;
string unitName;

list <string> macroLogP, macroLogP0, macroLogP1, macroLogP2, macroLogP3, macroLogP4, macroLogP5, macroLogP6, macroLogP7;
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

bool F5;
int F5_Pressed;
bool F6;
int F6_Pressed;
bool F7;
int F7_Pressed = -1;
bool F8;
int F8_Pressed = -1;
bool shift;
bool F9;
int F9_Pressed = -1;
bool F11;
int F11_Pressed;
bool DOWN;
bool UP;

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
string repDate;
string repGameTitle;
string repType;
bool autoMine;
bool autoTrainWorkers;
bool logWorkersAndSupplyProduction;
bool logUnitsProduction;
int replayLogUnitsFor;
int replayLogSupplyFor;

int autoBuildSuppliesBeforeBlocked;
int maxWorkers;
int maxProductionBuildingQueue;
int idleWorkerWarningEvery;
int sameScreenOrSelectionWarningEvery;
int totalTimeOnScreenOrSelectionAbove;
int idleProductionBuildingWarningEvery;
int idleProdBuildOrLarva;
int idleProdFrameCounter;
int idleFightingUnitWarningEvery;
int idleFightUnits;
int idleFightFrameCounter;
int workersProductionStoppedSeconds;

int mineralsWorker;
int gasWorker;
int workers;

string idleTime;
string workerCut;

int	idleWorkers;
int	idleWorkersFor;
int inProgressWorkers;
int notIdleResourceDepot;
int workersProductionStopped;
int workersProductionStoppedPrev;
vector<Position> idleWorkerPos;

// To quickly test things:
//#include "stdafx.h"
//#include <iostream> 
//
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

	if (strstr(unitName.c_str(), "Special_Map_Revealer"))
		unitName.replace(unitName.find("Special_Map_Revealer"), 8, "");

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
	//Broodwar->setLocalSpeed(42);
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
		string repPath = Broodwar->mapPathName();
		string cmd = "bwapi-data\\screp.exe \"" + repPath + "\" > bwapi-data\\repInfo.json";
		system(cmd.c_str());
		ifstream u("bwapi-data\\repInfo.json");
		j2 = json::parse(u);
		repDate = j2["Header"]["StartTime"].get<string>().erase(16, 40);
		repDate = repDate.replace(repDate.find("T"), 1, " ");
		repGameTitle = j2["Header"]["Title"].get<string>();
		repType = j2["Header"]["Type"]["Name"].get<string>();

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

	if (Broodwar->enemy()) // First make sure there is an enemy
		if (!Broodwar->isReplay())
		{
			char color = Broodwar->enemy()->getTextColor();
			if (color == 2)
				color = 17;

			if (!Broodwar->isMultiplayer())
				Broodwar->sendText("%cI have top intel about you %c%s, %cI know u don't understand.", Text::Tan, color, Broodwar->enemy()->getName().c_str(), Text::Tan);
		}
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
	FPS = Broodwar->getFPS();
	if (!Broodwar->isPaused())
		FrameCount = Broodwar->getFrameCount();
	else
		FrameCount++;

	Broodwar->drawTextScreen(180, 5, "%c:: CoachAI v3.0 ::", Text::Tan);

	if (FPS < 1) //gamePaused
		FPS = 24;

	// FrameCount / (1000/42), the in-game (but not with replay) time is accurate with the previous even if speed has changed,
	// this why sscait doesn't bother themselves finding solution for when a saved game loaded and FrameCount starts from 0. 
	gameTime = getTime(Broodwar->elapsedTime() * 0.6718);

	static int lastCheckedFrame11 = 0;
	if (lastCheckedFrame11 < FrameCount)
	{
		lastCheckedFrame11 = FrameCount + FPS;
		ifstream i("bwapi-data\\AnyRace_CoachAI.json");
		j = json::parse(i);
		autoTrainWorkers = j["Control Panel"]["autoTrainWorkers"].get<bool>();
		autoMine = j["Control Panel"]["autoMine"].get<bool>();
		autoBuildSuppliesBeforeBlocked = j["Control Panel"]["autoBuildSuppliesBeforeBlocked"].get<int>();
		maxWorkers = j["Control Panel"]["maxWorkers"].get<int>();
		maxProductionBuildingQueue = j["Control Panel"]["maxProductionBuildingQueue"].get<int>();
		idleWorkerWarningEvery = j["Control Panel"]["idleWorkerWarningEvery"].get<int>() * FPS;
		sameScreenOrSelectionWarningEvery = j["Control Panel"]["sameScreenOrSelectionWarningEvery"].get<int>() * FPS;
		totalTimeOnScreenOrSelectionAbove = j["Control Panel"]["totalTimeOnScreenOrSelectionAbove"].get<int>();
		idleProductionBuildingWarningEvery = j["Control Panel"]["idleProductionBuildingWarningEvery"].get<int>() * FPS;
		idleFightingUnitWarningEvery = j["Control Panel"]["idleFightingUnitWarningEvery"].get<int>() * FPS;
		workersProductionStoppedSeconds = j["Control Panel"]["workersProductionStoppedDuring"].get<int>();

		logWorkersAndSupplyProduction = j["Control Panel"]["logWorkersAndSupplyProduction"].get<bool>();
		logUnitsProduction = j["Control Panel"]["logUnitsProduction"].get<bool>();
		replayLogUnitsFor = j["Control Panel"]["replayLogUnitsFor"].get<int>();
		replayLogSupplyFor = j["Control Panel"]["replayLogSupplyFor"].get<int>();
	}
	if (Broodwar->isReplay())
		return Replay();

	currScreen = Broodwar->getScreenPosition();
	selectedUnits = Broodwar->getSelectedUnits();
	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame14 = 0;
		if (prevScreen == currScreen)
		{
			screenCounter++;
			if (lastCheckedFrame14 < FrameCount && FrameCount > sameScreenOrSelectionWarningEvery)
			{
				lastCheckedFrame14 = FrameCount + FPS * 1; // warning frequency every 1s
				PlaySound(L".\\bwapi-data\\2- multitask.wav", NULL, SND_ASYNC);
				Broodwar << Text::BrightRed << "Switch screens, divide tasks !" << endl;
			}
		}
		else // screens not the same, reset counter.
		{
			static int lastCheckedFrame1 = 0;
			if (lastCheckedFrame1 < FrameCount)
			{
				lastCheckedFrame14 = FrameCount + sameScreenOrSelectionWarningEvery; // warning initialized after ?s
				lastCheckedFrame1 = FrameCount + FPS / 12; // capture screen difference every .5s
				screenJumps.push_back(screenCounter / FPS);
				screenCounter = 0;
			}
		}

		static int lastCheckedFrame2 = 0;
		if (prevSelectedUnits == selectedUnits)
		{
			selectedUnitsCounter++;
			if (lastCheckedFrame2 < FrameCount && FrameCount > sameScreenOrSelectionWarningEvery)
			{
				lastCheckedFrame2 = FrameCount + FPS * 1; // warning frequency every 1s
				PlaySound(L".\\bwapi-data\\2- multitask.wav", NULL, SND_ASYNC);
				Broodwar << Text::Orange << "Switch units, divide tasks !" << endl;
			}
		}
		else // screens not the same, reset counter.
		{
			static int lastCheckedFrame3 = 0;
			if (lastCheckedFrame3 < FrameCount)
			{
				lastCheckedFrame2 = FrameCount + sameScreenOrSelectionWarningEvery; // warning initialized after ?s
				lastCheckedFrame3 = FrameCount + FPS / 12; // capture screen difference every .5s
				selectedUnitsJumps.push_back(selectedUnitsCounter / FPS);
				selectedUnitsCounter = 0;
			}
		}
	}

	static int lastCheckedFrame15 = 0;
	if (workersProductionStopped != workersProductionStoppedPrev)
	{
		if (lastCheckedFrame15 < FrameCount + 1)
		{
			lastCheckedFrame15 = FrameCount + FPS * 1; // warning frequency every 1s
			PlaySound(L".\\bwapi-data\\3- workerCut.wav", NULL, SND_ASYNC);
			Broodwar << Text::White << "Worker production stopped !" << endl;
		}
	}

	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame9 = 0;
		if (Broodwar->elapsedTime() > 1 && idleWorkersEvery() && lastCheckedFrame9 < FrameCount)
		{
			lastCheckedFrame9 = FrameCount + FPS * 2;
			PlaySound(L".\\bwapi-data\\1- idleWorker.wav", NULL, SND_ASYNC);
			Broodwar << Text::Grey << "Idle worker !" << endl;
		}
	}

	hotKeyHandler();

	DOWN = Broodwar->getKeyState(Key::K_DOWN);
	UP = Broodwar->getKeyState(Key::K_UP);

	F5 = Broodwar->getKeyState(Key::K_F5);
	F6 = Broodwar->getKeyState(Key::K_F6);
	F7 = Broodwar->getKeyState(Key::K_F7);
	F8 = Broodwar->getKeyState(Key::K_F8);
	F9 = Broodwar->getKeyState(Key::K_F9);
	F11 = Broodwar->getKeyState(Key::K_F11);
	shift = Broodwar->getKeyState(Key::K_SHIFT);

	static int lastCheckedFrame4 = 0;
	if (F5 && lastCheckedFrame4 < FrameCount)
	{
		F6_Pressed = 0;
		F7_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame4 = FrameCount + FPS / 6;
		F5_Pressed++;
		if (F5_Pressed == 2)
			F5_Pressed = 0;
	}

	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 < FrameCount)
	{
		F7_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame5 = FrameCount + FPS / 6;
		F6_Pressed++;
		if (F6_Pressed == 2)
			F6_Pressed = 0;
	}

	static int lastCheckedFrame6 = 0;
	if (F7 && lastCheckedFrame6 < FrameCount)
	{
		F6_Pressed = -1;
		F8_Pressed = -1;
		lastCheckedFrame6 = FrameCount + FPS / 6;
		F7_Pressed++;
		if (F7_Pressed == 3)
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
		if (F8_Pressed == Broodwar->enemies().size())
			F8_Pressed = 0;
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
		Broodwar->setScreenPosition(idleWorkerPos[F9_Pressed].x - 310, idleWorkerPos[F9_Pressed].y - 210);

	idleWorkerPos.clear();

	for (auto &u : selectedUnits)
	{
		string unitName = u->getType().getName().c_str();
		if (u->canTrain() && unitName != "Protoss_Reaver" && unitName != "Protoss_Carrier")
		{
			if (u->getTrainingQueue().size() > maxProductionBuildingQueue)
			{	
				u->cancelTrain();
				Broodwar << "Not allowed to queue units more than " << maxProductionBuildingQueue << endl;
			}
		}
		char color = Broodwar->self()->getTextColor();
		if (color == 2)
			color = 17;

		int dead = u->getPlayer()->deadUnitCount(u->getType());
		Broodwar->drawTextMap(u->getPosition().x, u->getBottom() + 35,
			"%d %s lost", dead, formated(u->getType()).c_str());
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
	idleProdBuildOrLarva = 0;
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
	//================================================== enemies
	vector<Player> enemies;
	if (Broodwar->enemy()) // First make sure there is an enemy
	{
		for (auto &enemy : Broodwar->enemies())
			enemies.push_back(enemy);
		static int lastCheckedFrame13 = 0;
		if (Broodwar->isMultiplayer() && lastCheckedFrame13 + 240 < FrameCount && !okUDAI)
		{
			lastCheckedFrame13 = FrameCount;
			Broodwar->sendText("I have top intel about u \"%s\", accept playing by typing UDAI.", Broodwar->enemy()->getName().c_str());
		}
	}
	if (enemies.size() > 0 && F8_Pressed > -1)
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
		for (auto &u : enemies[F8_Pressed]->getUnits())
		{
			UnitType ut = u->getType();
			char color = enemies[F8_Pressed]->getTextColor();
			if (color == 2)
				color = 17;
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
							Broodwar->drawLineMap(unitPos, orderPos, 8);
							Broodwar->drawTextMap(unitPos.x, unitPos.y + 10, orderName.c_str());
							Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, 8, true);
						}
						else
						{
							if (orderName == "HoldPosition")
								Broodwar->drawTextMap(unitPos, orderName.c_str());
						}
						if (u->getTransport() == nullptr)
							Broodwar->drawTextMap(unitPos, "%c%s", color, formated(ut).c_str());
					}
				}
				else
				{
					if (ut.getName() == "Zerg_Egg")
						ut = u->getBuildType();
					if (shift)
						listOfInCompleteUnits_Enemy.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
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
				Broodwar->drawTextMap(unitPos, "%c%s", color, formated(ut).c_str());

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
						listOfInCompleteBuildings_Enemy.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
					else if (!shift)
						listOfInCompleteBuildings_Enemy.push_back(formated(ut));
				}
			}
		}
		string ugName_e;
		list<string> upgrades;

		for (auto &ug : UpgradeTypes::allUpgradeTypes())
		{
			if (enemies[F8_Pressed]->getUpgradeLevel(ug))
			{
				for (auto &u : ug.whatUses())
				{
					if (ug.maxRepeats() > 1)
						ugName_e = ug.getName() + " +" + to_string(enemies[F8_Pressed]->getUpgradeLevel(ug));
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
				if (enemies[F8_Pressed]->hasResearched(tech) && !strstr(u.getName().c_str(), "Hero") && tech.whatResearches() != UnitTypes::None)
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

		if (shift)
		{
			for (string item : listOfInCompleteUnits_Enemy)
				InCompleteUnitsFinal_Enemy += item + "\n\r";
			for (string item : listOfInCompleteBuildings_Enemy)
				InCompleteBuildingsFinal_Enemy += item + "\n\r";

			Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::BrightRed, Text::Tan, UnitsFinal_Enemy.c_str(), 28, 27, InCompleteUnitsFinal_Enemy.c_str());
			Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::BrightRed, 27, BuildingsFinal_Enemy.c_str(), 28, 27, InCompleteBuildingsFinal_Enemy.c_str(), 8, 27, InProgressUpgradeTech_Enemy.c_str());
		}
		else if (!shift)
		{
			for (string item : listOfInCompleteUnits_Enemy)
				InCompleteUnitsMap_Enemy[item]++;
			for (string item : listOfInCompleteBuildings_Enemy)
				InCompleteBuildingsMap_Enemy[item]++;
			for (auto entry : InCompleteBuildingsMap_Enemy)
				InCompleteBuildingsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";
			for (auto entry : InCompleteUnitsMap_Enemy)
				InCompleteUnitsFinal_Enemy += to_string(entry.second) + " " + entry.first + "\n\r";
			Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::BrightRed, Text::Tan, UnitsFinal_Enemy.c_str(), 8, 27, InCompleteUnitsFinal_Enemy.c_str());
			Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::BrightRed, 27, BuildingsFinal_Enemy.c_str(), 8, 27, InCompleteBuildingsFinal_Enemy.c_str(), 8, 27, InProgressUpgradeTech_Enemy.c_str());
		}
		Player e = enemies[F8_Pressed];
		char color = e->getTextColor();
		if (color == 2)
			color = 17;

		if (Broodwar->isPaused())
			Broodwar->drawTextScreen(190, 35, "%c%s: %c%d, %c%d, %c%d/%d", color, e->getName().c_str(), Text::Turquoise, e->minerals(), Text::GreyGreen, e->gas(),
				Text::Yellow, e->supplyUsed() / 2, e->supplyTotal() / 2);
		else
			Broodwar->drawTextScreen(190, 35, "%c%s: %c%d, %c%d, %c%d/%d", color, e->getName().c_str(), Text::Turquoise, e->minerals(), Text::Green, e->gas(),
				Text::Teal, e->supplyUsed() / 2, e->supplyTotal() / 2);

		string upgradesFinal;
		for (auto entry : upgrades)
			upgradesFinal += entry + "\n\r";
		string technoFinal;
		for (auto entry : techs)
			technoFinal += entry + "\n\r";

		Broodwar->drawTextScreen(190, 45, "%cTech: %c%s%cUpgrade: %c%s", Text::BrightRed, Text::Tan, technoFinal.c_str(), 8, 27, upgradesFinal.c_str());
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
					listOfInCompleteUnits.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
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
					listOfInCompleteBuildings.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
				else listOfInCompleteBuildings.push_back(formated(ut));
			}
		}

		if (Broodwar->self()->getRace() == Races::Zerg && ut.isResourceDepot())
			idleProdBuildOrLarva += u->getLarva().size();

		if (u->isIdle() && ut.isBuilding() && u->canTrain() && !ut.isResourceDepot())
		{
			idleProdBuildOrLarva++;
			listOfIdleBuildings.push_back(formated(ut));
		}
		if (u->isIdle() && u->canAttackMove() && !ut.isBuilding() && !ut.isWorker())
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

		Unitset baseMineralsWorkers;
		int px;
		if (u->getType().isResourceDepot())
		{
			Position p = u->getPosition();
			baseMineralsWorkers = Broodwar->getUnitsInRadius(p, 300, IsWorker && IsGatheringMinerals && IsOwned);
			if (baseMineralsWorkers.size() < 10)
				px = p.x - 2;
			else px = p.x - 5;
			Broodwar->drawCircleMap(p, 10, Colors::Black, true);
			Broodwar->drawTextMap(px, p.y - 5, "%d", baseMineralsWorkers.size());
		}

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
	for (string item : listOfUnits)
		UnitsMap[item]++;
	for (string item : listOfBuildings)
		BuildingsMap[item]++;
	for (auto entry : BuildingsMap)
		BuildingsFinal += to_string(entry.second) + " " + entry.first + "\n\r";
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

	if (idleProdBuildOrLarva > 0)
		idleProdFrameCounter++;
	else idleProdFrameCounter = 0;

	// lastCheckedFrame used to display the warning once (in 1 frame), don't display it again unless the required amount of frames (idleProductionBuildingWarningEvery) passed.
	if (!Broodwar->isPaused())
	{
		static int lastCheckedFrame = 0;
		if (idleProdFrameCounter > idleProductionBuildingWarningEvery && lastCheckedFrame + idleProductionBuildingWarningEvery < FrameCount)
		{
			lastCheckedFrame = FrameCount;
			Broodwar << Text::Grey << "Idle production building for " << idleProductionBuildingWarningEvery / FPS << "s !" << endl;
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
			Broodwar << Text::Grey << "Idle fighting unit for " << idleFightingUnitWarningEvery / FPS << "s !" << endl;
		}
	}
	supplyAvailable = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();

	Broodwar->drawTextScreen(5, 25, "%cMinerals workers: %c%d", Text::Turquoise, Text::BrightRed, mineralsWorker);
	Broodwar->drawTextScreen(5, 35, "%cGas workers: %c%d", Text::Green, Text::BrightRed, gasWorker);
	Broodwar->drawTextScreen(5, 45, "%cIdle workers: %c%d, %s", Text::Grey, Text::BrightRed, idleWorkers, getTime(idleWorkersFor / FPS).c_str());

	string match = Broodwar->self()->getRace().getName().substr(0, 1) + "v";
	if (enemies.size() > 0)
		for (int i = 0; i < enemies.size(); i++)
			match += enemies[i]->getRace().getName().substr(0, 1);

	Broodwar->drawTextScreen(245, 25, "%c%s %c%s", Text::Purple, match.c_str(), Text::White, mapName.c_str());
	Broodwar->drawTextScreen(310, 15, "%cWorkers production stopped for: %c%s", Text::Grey, Text::BrightRed, getTime(workersProductionStopped / FPS).c_str());
	Broodwar->drawTextScreen(520, 15, "%cFPS: %c%d, %cTime: %c%s", 14, 4, FPS, 14, 4, gameTime.c_str());

	if (!Broodwar->isPaused())
	{
		if (Broodwar->self()->minerals() > 250)
			minerals_250++;
		if (Broodwar->self()->minerals() > 500)
			minerals_500++;
		if (Broodwar->self()->minerals() > 750)
			minerals_750++;
		if (Broodwar->self()->minerals() > 1000)
			minerals_1000++;
	}

	Broodwar->drawTextScreen(5, 63, "%cMinerals above:\n\r250: %c%s, %c500: %c%s\n\r%c750: %c%s, %c1000: %c%s",
		31, 25, getTime(minerals_250 / FPS).c_str(), 31, 25, getTime(minerals_500 / FPS).c_str(),
		31, 25, getTime(minerals_750 / FPS).c_str(), 31, 25, getTime(minerals_1000 / FPS).c_str());

	int Minerals = Broodwar->self()->spentMinerals() - Broodwar->self()->refundedMinerals();
	int Gas = Broodwar->self()->spentGas() - Broodwar->self()->refundedGas();

	Broodwar->drawTextScreen(5, 15, "%cMinerals spent: %d + %cGas spent: %d = %cMacro: %d", 31, Minerals, 7, Gas, 17, Minerals + Gas);
	UnitType supplyName = Broodwar->self()->getRace().getSupplyProvider();
	Broodwar->drawTextScreen(190, 330, "%cSupply blocked in: %c%d, %cmaxed with: %c%d %s", 3, Text::BrightRed, supplyAvailable / 2,
		3, Text::BrightRed, (200 - Broodwar->self()->supplyTotal() / 2) / 8, formated(supplyName).c_str());

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
		Broodwar->drawTextScreen(190, 25, "%cMacroLog: \n\r%s", Text::Blue, str1.c_str());
		Broodwar->drawTextScreen(225, 25, "\n\r%c%s", Text::Default, str2.c_str());

		if (Broodwar->self()->getRace() == Races::Zerg)
			Broodwar->drawTextScreen(5, 107, "%cIdle fighting units: \n\r%c%s", 5, 25, idleFightUnitsFinal.c_str());
		else Broodwar->drawTextScreen(5, 107, "%cIdle production: \n\r%c%s%cIdle fighting units: \n\r%c%s", 5, 25, idleBuildingsFinal.c_str(), 5, 25, idleFightUnitsFinal.c_str());
	}
	else if (F5_Pressed == 0)
	{
		Broodwar->drawTextScreen(143, 25, "%cHotkeys: (%c%d%c)", 8, 17, keysPressed.size(), 8);
		int y = 35;
		for (auto entry : hkAll)
		{
			Broodwar->drawTextScreen(143, y, "%c%s: %c%s", 8, entry.first.c_str(), 27, entry.second.c_str());
			y += 10;
		}
		Broodwar->drawTextScreen(5, 106, "%cMultitasking:\n\r%c==========\n\r%cScreen counter: %c%.0fs", 14, 4, 14, 4, screenCounter / FPS);
		Broodwar->drawTextScreen(5, 143, "%cScreen jumps: %c%d", Text::Blue, Text::White, screenJumps.size());
		double totalStay;
		double totalStayAbove;
		for (auto j : screenJumps)
		{
			totalStay += j;
			if (j > totalTimeOnScreenOrSelectionAbove)
			{
				totalStayAbove += j - totalTimeOnScreenOrSelectionAbove;
			}
		}
		Broodwar->drawTextScreen(5, 153, "%cAvg stay: %c%.1fs", Text::Blue, Text::White, totalStay / screenJumps.size());
		Broodwar->drawTextScreen(5, 163, "%cTotal stay > %c%ds: %c%s", Text::Blue, 7, totalTimeOnScreenOrSelectionAbove, Text::White, getTime(totalStayAbove).c_str());
		//=========================
		Broodwar->drawTextScreen(5, 183, "%cSelection counter: %c%.0fs", 14, 4, selectedUnitsCounter / FPS);
		Broodwar->drawTextScreen(5, 193, "%cSelection changed: %c%d", Text::Blue, Text::White, selectedUnitsJumps.size());
		double totalStayUnits;
		double totalStayAboveUnits;
		for (auto j : selectedUnitsJumps)
		{
			totalStayUnits += j;
			if (j > totalTimeOnScreenOrSelectionAbove)
			{
				totalStayAboveUnits += j - totalTimeOnScreenOrSelectionAbove;
			}
		}
		Broodwar->drawTextScreen(5, 203, "%cAvg selection: %c%.1fs", Text::Blue, Text::White, totalStayUnits / selectedUnitsJumps.size());
		Broodwar->drawTextScreen(5, 213, "%cTotal selection > %c%ds: %c%s", Text::Blue, 7, totalTimeOnScreenOrSelectionAbove, Text::White, getTime(totalStayAboveUnits).c_str());
	}

	if (F6_Pressed == 1)
	{
		for (string item : listOfInCompleteUnits)
			inCompleteUnitsFinal += item + "\n\r";
		for (string item : listOfInCompleteBuildings)
			inCompleteBuildingsFinal += item + "\n\r";

		Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 28, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 28, Text::GreyGreen, inCompleteBuildingsFinal.c_str(), 24, 29, inProgressUpgradeTech.c_str());
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
		Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 24, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 24, Text::GreyGreen, inCompleteBuildingsFinal.c_str(), 24, 29, inProgressUpgradeTech.c_str());
	}

	if (F7_Pressed == 0)
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo1Name"].get<string>().c_str(), 4, j["Preset Plan"]["Bo1Text"].get<string>().c_str(),
			28, 31, j["Preset Plan"]["Bo1Tips"].get<string>().c_str());
	else if (F7_Pressed == 1)
	{
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo2Name"].get<string>().c_str(), 4, j["Preset Plan"]["Bo2Text"].get<string>().c_str(),
			28, 31, j["Preset Plan"]["Bo2Tips"].get<string>().c_str());
	}
	else if (F7_Pressed == 2)
	{
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo3Name"].get<string>().c_str(), 4, j["Preset Plan"]["Bo3Text"].get<string>().c_str(),
			28, 31, j["Preset Plan"]["Bo3Tips"].get<string>().c_str());
	}


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
	prevScreen = currScreen;
	prevSelectedUnits = selectedUnits;
}
int Dragoons;
int tankOuterSplashRadius;

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
			Broodwar->drawTextScreen(125, y, "%c%d", 17, entry.second);
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
	F5 = Broodwar->getKeyState(Key::K_F5);
	F6 = Broodwar->getKeyState(Key::K_F6);
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
	static int lastCheckedFrame7 = 0;
	if (F11 && lastCheckedFrame7 < FrameCount)
	{
		lastCheckedFrame7 = FrameCount + FPS / 6;
		F11_Pressed++;
		if (F11_Pressed == 2)
			F11_Pressed = 0;
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

	Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s\n\r  %c%s / %c%s / %s", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7,
		getTime(ReplaySeconds).c_str(), Text::Teal, repDate.c_str(), 29, repGameTitle.c_str(), repType.c_str());


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

		if (p->minerals() > 250)
			mineralsAbove[pid]++;
		if (p->minerals() > 500)
			mineralsAbove[pid + 8]++;
		if (p->minerals() > 750)
			mineralsAbove[pid + 16]++;
		if (p->minerals() > 1000)
			mineralsAbove[pid + 24]++;

		char color = p->getTextColor();
		if (color == 2)
			color = 17;

		idleWorkers = 0;
		idleWorkersFor = 0;
		inProgressWorkers = 0;
		notIdleResourceDepot = 0;
		workersProductionStopped = 0;

		for (auto u : p->getUnits())
		{
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

		if (idleWorkers > idleWorkersMaxAll[pid])
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
				if (p != pTemp && p->isAlly(pTemp))
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
					if (color == 2)
						color = 17;
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
						Broodwar->drawLineMap(unitPos, orderPos, color);
						Broodwar->drawTextMap(unitPos, orderName.c_str());
						Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, color, true);
					}
					else
					{
						if (orderName == "HoldPosition")
							Broodwar->drawTextMap(unitPos, orderName.c_str());
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
		Player pl = unit->getPlayer();
		char color = pl->getTextColor();
		if (color == 2)
			color = 17;

		pid = pl->getID();
		Broodwar->drawTextScreen(270, 15, "%cMinerals above: 250: %c%s, %c500: %c%s, %c750: %c%s, %c1000: %c%s", color,
			25, getTime(mineralsAbove[pid] / FPS).c_str(), color, 25, getTime(mineralsAbove[pid + 8] / FPS).c_str(), color,
			25, getTime(mineralsAbove[pid + 16] / FPS).c_str(), color, 25, getTime(mineralsAbove[pid + 24] / FPS).c_str());

		list<string> upgrades;
		list<string> techno;
		string ugName;
		list<UpgradeType> selectedPlayerUpgrades;
		for (auto ug : UpgradeTypes::allUpgradeTypes())
		{
			if (pl->getUpgradeLevel(ug))
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
				ugName = upg.getName() + " +" + to_string(pl->getUpgradeLevel(upg));
			else ugName = formated(UnitT) + "s " + upg.getName();

			//if ugName not exists
			auto iter = std::find(upgrades.begin(), upgrades.end(), ugName);
			if (iter != upgrades.end())
				;
			else upgrades.push_back(ugName);
		}
		for (auto tech : TechTypes::allTechTypes())
		{
			string techName = tech.getName();
			if (pl->hasResearched(tech) && tech.whatResearches() != UnitTypes::None)
			{
				auto iter = std::find(techno.begin(), techno.end(), techName);
				if (iter != techno.end())
					;
				else techno.push_back(techName);
			}
		}
		string upgradesFinal;
		for (auto entry : upgrades)
			upgradesFinal += entry + "\n\r";
		string technoFinal;
		for (auto entry : techno)
			technoFinal += entry + "\n\r";

		for (auto u : pl->getUnits())
		{
			//baseMineralsWorkers:
			Unitset baseMineralsWorkers;
			int px;
			if (u->getType().isResourceDepot())
			{
				Position p = u->getPosition();

				baseMineralsWorkers = Broodwar->getUnitsInRadius(p, 300, IsWorker && IsGatheringMinerals); // && isOwned ?
				for (auto worker : baseMineralsWorkers)
				{
					if (worker->getPlayer()->getID() != pl->getID())
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
						listOfInCompleteUnits.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
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
						listOfInCompleteBuildings.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
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
		int dead = pl->deadUnitCount(unit->getType());
		Broodwar->drawTextMap(unit->getPosition().x, unit->getBottom() + 35,
			"%c%d %s lost", color, dead, formated(unit->getType()).c_str());

		//InCompleteUnits & upgrades mode (final process):
		if (F6_Pressed == 1)
		{
			for (string item : listOfInCompleteUnits)
				inCompleteUnitsFinal += item + "\n\r";
			for (string item : listOfInCompleteBuildings)
				inCompleteBuildingsFinal += item + "\n\r";

			Broodwar->drawTextScreen(440, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 28, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 28, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
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

			Broodwar->drawTextScreen(440, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), 24, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), 24, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
		}
		Broodwar->drawTextScreen(270, 25, "%cTech:\n\r%c%s%cUpgrade:\n\r%c%s", Text::Blue, Text::Teal, technoFinal.c_str(), Text::Blue, Text::Teal, upgradesFinal.c_str());

		//replay BO log:
		if (F5_Pressed == 3)
		{
			switch (pl->getID())
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
			string BO, BO2 = "";
			int colSize = 0;
			for (auto item : macroLogP)
			{
				colSize++;
				if (colSize < 15)
					BO += item + "\n\r";
				else BO2 += item + "\n\r";
				if (colSize > 37)
				{
					BO2 += "^^^^^^";
					break;
				}
			}
			Broodwar->drawTextScreen(5, 95, "Build order:\n\r%c%s", 7, BO.c_str());
			Broodwar->drawTextScreen(140, 15, "%c%s", 7, BO2.c_str());
		}
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
	// Send the text to the game if it is not being processed.
	else Broodwar->sendText("%s", text.c_str());

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void AnyRace_CoachAI::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	//Broodwar << player->getName() << " said \"" << text << "\"" << endl;
	if (strstr(text.c_str(), "UDAI"))
		okUDAI = true;
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

		if (p->getRace().getName() == "Zerg" && unit->getType().getName() != "Zerg_Egg" && !ut.isBuilding())
			return;

		bool UnitLog = true;
		if (ut.isWorker() && (Broodwar->elapsedTime() * 0.6718 > 25 || Broodwar->elapsedTime() * 0.6718 < 10))
			return;
		if (ut == p->getRace().getSupplyProvider() && p->supplyUsed() / 2 > 10)
			return;

		if (Broodwar->elapsedTime() * 0.6718 > replayLogUnitsFor) //log units until 1st 00:00, then only buildings
			UnitLog = ut.isBuilding();

		if (p->spentMinerals() != 0 && UnitLog)
		{
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
		Player p = unit->getPlayer();

		bool UnitLog = true;
		if (ut.isWorker() && (Broodwar->elapsedTime() * 0.6718 > 25 || Broodwar->elapsedTime() * 0.6718 < 10))
			return;
		if (ut == p->getRace().getSupplyProvider() && p->supplyUsed() / 2 > 10)
			return;

		if (Broodwar->elapsedTime() * 0.6718 > replayLogUnitsFor) //log units until 1st 00:00, then only buildings
			UnitLog = ut.isBuilding();

		if (p->spentMinerals() != 0 && UnitLog && un != "Larva" && un != "Zergling" && un != "Scourge")
		{
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
