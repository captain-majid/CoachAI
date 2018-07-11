#include "AnyRace_CoachAI.h"
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include "nlohmann\json.hpp"
#include "algorithm"

using json = nlohmann::json;
using namespace BWAPI;
using namespace Filter;
using namespace std;

int ReplaySeconds;
static int lastChecked;
map<string, string> hkAll;
map<string, list<string>> hkAllRaw;
list<Key> keysPressed;
int minerals_250;
int minerals_500;
int minerals_750;
int minerals_1000;

Unit Probe_SCV_Larva;
bool OvIsMorphing;
bool droneIsMorphing;
string unitName;

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

int seconds;
int minutes;
int hours;
string gameTime;

bool F5;
int F5_Pressed;
bool F6;
int F6_Pressed;
bool F7;
int F7_Pressed = -1;
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
int workersProductionStopped;
int notIdleResourceDepot;

json j;
bool autoMine;
bool autoTrainWorkers;
bool logWorkersAndSupplyProduction;
bool logUnitsProduction;
int autoBuildSuppliesBeforeBlocked;
int maxWorkers;
int maxProductionBuildingQueue;
int idleWorkerWarningEvery;
int idleProductionBuildingWarningEvery;
int idleProdBuildOrLarva;
int idleProdFrameCounter;
int idleFightingUnitWarningEvery;
int idleFightUnits;
int idleFightFrameCounter;
int workersProductionStoppedDuring;

int mineralsWorker;
int gasWorker;
int workers;
int inProgressWorkers;
int	idleWorkers;
int	idleWorkersFor;

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
	//if (strstr(unitName.c_str(), "Robotics_Support_Bay"))
	//	unitName.replace(unitName.find("Robotics_Support_Bay"), 20, "Robotics_S_Bay");

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
	idleWorkers = 0;
	inProgressWorkers = 0;
	notIdleResourceDepot = 0;

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
				idleWorkers++;
				idleWorkersFor++;
			}
		}
		else if (u->getType().isWorker() && !u->isCompleted())
		{
			inProgressWorkers++;
		}
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
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	if (Broodwar->isReplay())
	{
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
	FrameCount = Broodwar->getFrameCount();
	FPS = Broodwar->getFPS();
	if (FPS < 1)
		return;
	if (Broodwar->isReplay())
		return Replay();

	gameTime = getTime(Broodwar->elapsedTime() * 0.6718); //FrameCount / 23.81

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
		idleProductionBuildingWarningEvery = j["Control Panel"]["idleProductionBuildingWarningEvery"].get<int>() * FPS;
		idleFightingUnitWarningEvery = j["Control Panel"]["idleFightingUnitWarningEvery"].get<int>() * FPS;
		workersProductionStoppedDuring = j["Control Panel"]["workersProductionStoppedDuring"].get<int>() * FPS;

		logWorkersAndSupplyProduction = j["Control Panel"]["logWorkersAndSupplyProduction"].get<bool>();
		logUnitsProduction = j["Control Panel"]["logUnitsProduction"].get<bool>();
	}
	hotKeyHandler();

	Unitset baseMineralsWorkers;
	int px;
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot())
		{
			Position p = u->getPosition();
			baseMineralsWorkers = Broodwar->getUnitsInRadius(p, 300, IsWorker && IsGatheringMinerals);
			Broodwar->drawCircleMap(p, 10, Colors::Black, true);
			if (baseMineralsWorkers.size() < 10)
				px = p.x - 2;
			else px = p.x - 5;
			Broodwar->drawTextMap(px, p.y - 5, "%d", baseMineralsWorkers.size());
		}
	}

	DOWN = Broodwar->getKeyState(Key::K_DOWN);
	UP = Broodwar->getKeyState(Key::K_UP);

	F5 = Broodwar->getKeyState(Key::K_F5);
	F6 = Broodwar->getKeyState(Key::K_F6);
	F7 = Broodwar->getKeyState(Key::K_F7);

	static int lastCheckedFrame4 = 0;
	if (F5 && lastCheckedFrame4 < FrameCount)
	{
		lastCheckedFrame4 = FrameCount + FPS / 2;
		F5_Pressed++;
		if (F5_Pressed == 2)
			F5_Pressed = 0;
	}

	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 < FrameCount)
	{
		F7_Pressed = -1;
		lastCheckedFrame5 = FrameCount + FPS / 6;
		F6_Pressed++;
		if (F6_Pressed == 2)
			F6_Pressed = 0;
	}

	static int lastCheckedFrame6 = 0;
	if (F7 && lastCheckedFrame6 < FrameCount)
	{
		F6_Pressed = -1;
		lastCheckedFrame6 = FrameCount + FPS / 6;
		F7_Pressed++;
		if (F7_Pressed == 3)
			F7_Pressed = 0;
	}

	if (Broodwar->elapsedTime() > 1 && idleWorkersEvery())
		Broodwar->sendText("You've an idle worker !");

	for (auto &u : Broodwar->getSelectedUnits())
	{
		string unitName = u->getType().getName().c_str();
		if (u->canTrain() && unitName != "Protoss_Reaver" && unitName != "Protoss_Carrier")
		{
			if (u->getTrainingQueue().size() > maxProductionBuildingQueue)
			{
				u->cancelTrain();
				Broodwar->sendText("Not allowed to train more than %d units.", maxProductionBuildingQueue);
			}
		}
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
				if (techName == "Burrowing" || techName == "Tank_Siege_Mode")
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

	//call init()  or reassign here
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
	string inProgressUpgradeTech;

	for (auto &u : Broodwar->self()->getUnits())
	{
		UnitType ut = u->getType();
		if (!ut.isBuilding())
		{
			if (u->isCompleted())
				listOfUnits.push_back(formated(ut));
			else
			{
				if (ut.getName() == "Zerg_Egg")
					ut = u->getBuildType();
				if (F6_Pressed == 0)
					listOfInCompleteUnits.push_back(formated(ut) + " " + to_string(u->getRemainingBuildTime() / FPS));
				else listOfInCompleteUnits.push_back(formated(ut));
			}
		}
		else
		{
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
				if (F6_Pressed == 0)
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

		if (Broodwar->self()->getRace() != Races::Zerg && ut.isResourceDepot() && u->isIdle() && FrameCount < workersProductionStoppedDuring)
			workersProductionStopped++;
		if (Broodwar->self()->getRace() != Races::Zerg && ut.isResourceDepot() && !u->isIdle() && FrameCount < workersProductionStoppedDuring)
		{
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
	static int lastCheckedFrame = 0;
	if (idleProdFrameCounter > idleProductionBuildingWarningEvery && lastCheckedFrame + idleProductionBuildingWarningEvery < FrameCount)
	{
		lastCheckedFrame = FrameCount;
		Broodwar->sendText("You've an idle production building for %ds !, planning to fight with workers ?", idleProductionBuildingWarningEvery / FPS);
	}

	if (idleFightUnits > 0)
		idleFightFrameCounter++;
	else idleFightFrameCounter = 0;
	static int lastChecked4 = 0;
	if (idleFightFrameCounter > idleFightingUnitWarningEvery && lastChecked4 + idleFightingUnitWarningEvery < FrameCount)
	{
		lastChecked4 = FrameCount;
		Broodwar->sendText("You've an idle fighting unit for %ds !, trained them for something ?", idleFightingUnitWarningEvery / FPS);
	}

	supplyAvailable = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();

	Broodwar->drawTextScreen(5, 25, "%cMinerals workers: %c%d", Text::Turquoise, Text::BrightRed, mineralsWorker);
	Broodwar->drawTextScreen(5, 35, "%cGas workers: %c%d", Text::Green, Text::BrightRed, gasWorker);
	Broodwar->drawTextScreen(5, 45, "%cIdle workers: %c%d, %s", Text::Grey, Text::BrightRed, idleWorkers, getTime(idleWorkersFor / FPS).c_str());

	string m = Broodwar->self()->getRace().getName().substr(0, 1) + "v" + Broodwar->enemy()->getRace().getName().substr(0, 1);
	const char * match = m.c_str();


	Broodwar->drawTextScreen(245, 25, "%c%s %c%s", Text::Purple, mapName.c_str(), Text::Brown, m.c_str());
	Broodwar->drawTextScreen(180, 5, "%c:: CoachAI v2.9.4.1 ::", Text::Tan);

	Broodwar->drawTextScreen(310, 15, "%cWorkers production stopped for: %c%s", Text::Grey, Text::BrightRed, getTime(workersProductionStopped / FPS).c_str());
	Broodwar->drawTextScreen(520, 15, "%cFPS: %c%d, %cTime: %c%s", 14, 4, FPS, 14, 4, gameTime.c_str());

	if (Broodwar->self()->getRace() == Races::Zerg)
		Broodwar->drawTextScreen(5, 65, "%cIdle production: \n\r%c%d Larvae\n\r%cIdle fighting units: \n\r%c%s", 5, 25, idleProdBuildOrLarva, 5, 25, idleFightUnitsFinal.c_str());
	else Broodwar->drawTextScreen(5, 65, "%cIdle production: \n\r%c%s\n\r%cIdle fighting units: \n\r%c%s", 5, 25, idleBuildingsFinal.c_str(), 5, 25, idleFightUnitsFinal.c_str());

	int Minerals = Broodwar->self()->spentMinerals() - Broodwar->self()->refundedMinerals();
	int Gas = Broodwar->self()->spentGas() - Broodwar->self()->refundedGas();

	Broodwar->drawTextScreen(5, 15, "%cMinerals spent: %d + %cGas spent: %d = %cMacro: %d", 31, Minerals, 7, Gas, 17, Minerals + Gas);
	Broodwar->drawTextScreen(250, 320, "%cSupply blocked in: %c%d", 3, Text::BrightRed, supplyAvailable / 2);
	UnitType supplyName = Broodwar->self()->getRace().getSupplyProvider();
	Broodwar->drawTextScreen(250, 330, "%cSupply maxed with: %c%d %s", 3, Text::BrightRed, (200 - Broodwar->self()->supplyTotal() / 2) / 8, formated(supplyName).c_str());

	if (F5_Pressed == 0)
	{
		if (macroLogMap.size() != multiMap.size())
		{
			if (FPS != 24)
			{
				seconds = FrameCount / FPS; // was avgFPS, also you can use Broodwar->elapsedTime() * 0.672? to get the time since the game started, but buggy also.
				Broodwar->sendText("%cYou might've inaccurate MacroLog time, with speeds other than Fastest: 24 FPS.", Text::BrightRed);
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
	}
	else if (F5_Pressed == 1)
	{
		Broodwar->drawTextScreen(138, 25, "%cHotkeys: (%c%d%c)", 8, 17, keysPressed.size(), 8);
		int y = 35;
		for (auto entry : hkAll)
		{
			Broodwar->drawTextScreen(138, y, "%c%s: %c%s", 8, entry.first.c_str(), 27, entry.second.c_str());
			y += 10;
		}
	}

	if (Broodwar->self()->minerals() > 250)
		minerals_250++;
	if (Broodwar->self()->minerals() > 500)
		minerals_500++;
	if (Broodwar->self()->minerals() > 750)
		minerals_750++;
	if (Broodwar->self()->minerals() > 1000)
		minerals_1000++;

	Broodwar->drawTextScreen(100, 175, "%cMinerals:\n\r> 250: %c%s\n\r%c> 500: %c%s\n\r%c> 750: %c%s\n\r%c> 1000: %c%s",
		31, 8, getTime(minerals_250 / FPS).c_str(), 31, 8, getTime(minerals_500 / FPS).c_str(),
		31, 8, getTime(minerals_750 / FPS).c_str(), 31, 8, getTime(minerals_1000 / FPS).c_str());

	if (F6_Pressed == 0)
	{
		for (string item : listOfInCompleteUnits)
			inCompleteUnitsFinal += item + "\n\r";
		for (string item : listOfInCompleteBuildings)
			inCompleteBuildingsFinal += item + "\n\r";

		Broodwar->drawTextScreen(400, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(500, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s%cTech/Upgrade in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteBuildingsFinal.c_str(), 24, 29, inProgressUpgradeTech.c_str());
	}
	else if (F6_Pressed == 1)
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
	else
	{
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

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
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
}

void AnyRace_CoachAI::hotKeyHandler()
{
	bool shift = Broodwar->getKeyState(Key::K_SHIFT);

	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_1))
		hkAll["1"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "1");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_2))
		hkAll["2"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "2");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_3))
		hkAll["3"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "3");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_4))
		hkAll["4"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "4");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_5))
		hkAll["5"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "5");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_6))
		hkAll["6"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "6");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_7))
		hkAll["7"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "7");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_8))
		hkAll["8"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "8");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_9))
		hkAll["9"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "9");
	if ((Broodwar->getKeyState(Key::K_CONTROL) || shift) && Broodwar->getKeyState(Key::K_0))
		hkAll["0"] = getHotKeyGroup(Broodwar->getSelectedUnits(), shift, "0");
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

	if (F5_Pressed == 1)
	{
		int y = 35;
		for (auto entry : keysPressedMap)
		{
			Broodwar->drawTextScreen(120, y, "%c%d", 17, entry.second);
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

	if (shift)//A trivial display bug here where you can combine unit(s) with a building
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
	F6 = Broodwar->getKeyState(Key::K_F6);

	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 < FrameCount)
	{
		lastCheckedFrame5 = FrameCount + FPS / 6;
		F6_Pressed++;
		if (F6_Pressed == 2)
			F6_Pressed = 0;
	}

	Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7, getTime(ReplaySeconds).c_str());
	Broodwar->drawTextScreen(5, 95, "\n\r%cMinerals spent + %cGas spent = %cMacro", 31, 7, 17);

	int y1 = 120;
	list<Player> playersList;
	for (auto p : players)
	{
		if (!p->isNeutral() && !p->isObserver() && p->spentMinerals() != 0)
			playersList.push_back(p);
	}

	list<string> forceList;
	for (auto p : playersList)
	{
		string forceName = p->getForce()->getName().c_str();
		if (forceName == "")// no team players
		{
			int Minerals = p->spentMinerals() - p->refundedMinerals();
			int Gas = p->spentGas() - p->refundedGas();
			char color2 = 15;
			char color = p->getTextColor();
			if (color == 2)
				color = 17;

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

			Broodwar->drawTextScreen(5, y1, "%c(%s) %s %c%s: %c%d+%c%d=%c%d %c%d/%d",
				color, p->getRace().getName().substr(0, 1).c_str(), p->getName().substr(0, 3).c_str(), color2, allies.c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas, 15, p->supplyUsed() / 2, p->supplyTotal() / 2);
			y1 += 10;
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
					if (f_p->spentMinerals() != 0)
					{
						Broodwar->drawTextScreen(5, y1, "%c(%s) %s: %c%d+%c%d=%c%d %c%d/%d", color, f_p->getRace().getName().substr(0, 1).c_str(), f_p->getName().c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas, 15, f_p->supplyUsed() / 2, f_p->supplyTotal() / 2);
						y1 += 10;
					}
				}
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
	for (auto &u : Broodwar->getAllUnits())
	{
		Color color = u->getPlayer()->getColor();
		if (u->getType().isBuilding())
		{
			if (!replayCenteredOnNexus && u->getType().getRace() == Races::Protoss && u->getType().isResourceDepot())
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
			unitPos = u->getPosition();
			orderPos = u->getOrderTargetPosition();
			orderName = u->getOrder().getName();

			if (unitPos.isValid() && unitPos.y != 0 && orderPos.isValid() && orderPos.y != 0)
			{
				Broodwar->drawLineMap(unitPos, orderPos, color);
				Broodwar->drawTextMap(unitPos, orderName.c_str());
				Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, color, true);
			}
		}
	}

	//call init()  or reassign here
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
			UnitType ut = u->getType();
			if (!ut.isBuilding())
			{
				if (u->isCompleted())
					listOfUnits.push_back(formated(ut));
				else
				{
					if (ut.getName() == "Zerg_Egg")
						ut = u->getBuildType();

					if (F6_Pressed == 0)
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
					if (F6_Pressed == 0)
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

		if (F6_Pressed == 0)
		{
			for (string item : listOfInCompleteUnits)
				inCompleteUnitsFinal += item + "\n\r";
			for (string item : listOfInCompleteBuildings)
				inCompleteBuildingsFinal += item + "\n\r";

			Broodwar->drawTextScreen(425, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
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

			Broodwar->drawTextScreen(425, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), Text::GreyBlue, Text::GreyGreen, inCompleteUnitsFinal.c_str());
			Broodwar->drawTextScreen(530, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), Text::GreyBlue, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
		}
		Broodwar->drawTextScreen(255, 15, "%c%s =========================================", color, pl->getName().c_str());
		Broodwar->drawTextScreen(255, 25, "%cTech: \n\r%c%s%cUpgrades: \n\r%c%s", Text::Blue, Text::Teal, technoFinal.c_str(), Text::Blue, Text::Teal, upgradesFinal.c_str());
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
		Broodwar->sendText("%s", "\"show me the money\"");
	}
	else if (text == "c2")
	{
		Broodwar->sendText("%s", "black sheep wall");
		Broodwar->sendText("%s", "\"black sheep wall\"");
	}
	else if (text == "c3")
	{
		Broodwar->sendText("%s", "operation cwal");
		Broodwar->sendText("%s", "\"operation cwal\"");
	}
	else if (text == "c4")
	{
		Broodwar->sendText("%s", "power overwhelming");
		Broodwar->sendText("%s", "\"power overwhelming\"");
	}
	else if (text == "c5")
	{
		Broodwar->sendText("%s", "staying alive");
		Broodwar->sendText("%s", "\"staying alive\"");
	}
	// Send the text to the game if it is not being processed.
	else Broodwar->sendText("%s", text.c_str());

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void AnyRace_CoachAI::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << endl;
}

void AnyRace_CoachAI::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void AnyRace_CoachAI::onNukeDetect(BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << endl;
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
	if (unit->getType().getName() == "Zerg_Overlord")
		OvIsMorphing = false;
	if (unit->getType().getName() == "Zerg_Drone")
		droneIsMorphing = false;

	UnitType ut;
	if (unit->getType().getName() == "Zerg_Egg")
		ut = unit->getBuildType();
	else ut = unit->getType();

	string un = formated(ut);
	if (unit->getPlayer() == Broodwar->self() && un != "None")
	{
		if (!logWorkersAndSupplyProduction && (ut.isWorker() || ut == Broodwar->self()->getRace().getSupplyProvider()))
			return;
		else if (!logUnitsProduction && (!ut.isWorker() && ut != Broodwar->self()->getRace().getSupplyProvider() && !ut.isBuilding()))
			return;
		else
			macroLogMap[un + " #" + to_string(unit->getID())] = gameTime;
	}

	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		//if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		//{
		//	int seconds = FrameCount / FPS;
		//	int minutes = seconds / 60;
		//	seconds %= 60;
		//	Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		//}
	}
}

void AnyRace_CoachAI::onUnitCreate(BWAPI::Unit unit)
{
	UnitType ut = unit->getType();
	string un = formated(ut);

	if (FrameCount > 0 && unit->getPlayer() == Broodwar->self() && un != "Larva") //!unit->getPlayer()->isNeutral())
	{
		if (!logWorkersAndSupplyProduction && (ut.isWorker() || ut == Broodwar->self()->getRace().getSupplyProvider()))
			return;
		else if (!logUnitsProduction && (!ut.isWorker() && ut != Broodwar->self()->getRace().getSupplyProvider() && !ut.isBuilding()))
			return;
		else
			macroLogMap[un + " #" + to_string(unit->getID())] = gameTime;
	}
}

void AnyRace_CoachAI::onUnitComplete(BWAPI::Unit unit)
{
}
