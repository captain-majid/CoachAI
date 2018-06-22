#include "AnyRace_CoachAI.h"
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>

#include "nlohmann\json.hpp"
#include "algorithm"

using json = nlohmann::json;
using namespace BWAPI;
using namespace Filter;
using namespace std;

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
bool DOWN;
bool UP;

bool F6;
int F6_Pressed;
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

std::string formated(UnitType ut)
{
	std::string unitName = ut.getName();
	if (strstr(unitName.c_str(), "Protoss_"))
		unitName.replace(unitName.find("Protoss_"), 8, "");
	if (strstr(unitName.c_str(), "Terran_"))
		unitName.replace(unitName.find("Terran_"), 7, "");
	if (strstr(unitName.c_str(), "Zerg_"))
		unitName.replace(unitName.find("Zerg_"), 5, "");
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
	//Broodwar->setLocalSpeed(42);
	mapName = Broodwar->mapName();
	if (strstr(mapName.c_str(), "| iCCup | "))
		mapName.replace(mapName.find("| iCCup | "), 10, "");


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
	gameType = Broodwar->getGameType().getName();

	FrameCount = Broodwar->getFrameCount();
	avgFPS = Broodwar->getAverageFPS();
	FPS = Broodwar->getFPS();
	if (FPS < 1)
		return;

	gameTime = getTime(FrameCount / 23.81); //Broodwar->elapsedTime() * 0.6728971962616822

	std::ifstream i("bwapi-data\\AnyRace_CoachAI.json");
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
	if (Broodwar->isReplay())
		return Replay();

	F5 = Broodwar->getKeyState(Key::K_F5);
	DOWN = Broodwar->getKeyState(Key::K_DOWN);
	UP = Broodwar->getKeyState(Key::K_UP);

	F6 = Broodwar->getKeyState(Key::K_F6);

	static int lastCheckedFrame4 = 0;
	if (F5 && lastCheckedFrame4 + FPS / 2 < FrameCount)
	{
		lastCheckedFrame4 = FrameCount;
		F5_Pressed++;
	}

	static int lastCheckedFrame5 = 0;
	if (F6 && lastCheckedFrame5 + FPS / 6 < FrameCount)
	{
		lastCheckedFrame5 = FrameCount;
		F6_Pressed++;
		if (F6_Pressed == 4)
			F6_Pressed = 0;
	}
	bool F7 = Broodwar->getKeyState(Key::K_F7);
	bool F8 = Broodwar->getKeyState(Key::K_F8);
	bool F9 = Broodwar->getKeyState(Key::K_F9);

	if (Broodwar->elapsedTime() > 1 && idleWorkersEvery())
		Broodwar->sendText("You've an idle worker !");

	for (auto &u : Broodwar->getSelectedUnits())
	{
		std::string unitName = u->getType().getName().c_str();
		if (u->canTrain() && unitName != "Protoss_Reaver" && unitName != "Protoss_Carrier")
		{
			if (u->getTrainingQueue().size() > maxProductionBuildingQueue)
			{
				u->cancelTrain();
				Broodwar->sendText("Not allowed to train more than %d units.", maxProductionBuildingQueue);
			}
		}
	}

	//std::list<std::string> upgrades;
	//std::list<std::string> techno;
	std::string ugName;
	for (auto &ug : UpgradeTypes::allUpgradeTypes())
	{
		if (Broodwar->self()->getUpgradeLevel(ug))
		{
			for (auto &u : ug.whatUses())
			{
				if (ug.maxRepeats() > 1)
				{
					ugName = ug.getName() + " +" + std::to_string(Broodwar->self()->getUpgradeLevel(ug));
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
		std::string techName = tech.getName();
		for (auto &u : tech.whatUses())
		{
			if (Broodwar->self()->hasResearched(tech) && !strstr(u.getName().c_str(), "Hero") && tech.whatResearches() != UnitTypes::None)
			{
				if (techName == "Burrowing" || techName == "Tank_Siege_Mode")
				{
				}
				else
				{
					techName = formated(u) + "s " + techName;
				}

				auto iter = macroLogMap.find(techName);
				if (iter != macroLogMap.end())
				{
				}
				else
				{
					macroLogMap[techName] = gameTime;
				}
			}
		}
	}
	//upgrades.unique();
	//upgrades.sort();
	//techno.unique();
	//techno.sort();
	//std::string upgradesFinal;
	//for (auto entry : upgrades)
	//	upgradesFinal += entry + "\n\r";
	//std::string technoFinal;
	//for (auto entry : techno)
	//	technoFinal += entry + "\n\r";

	idleProdBuildOrLarva = 0;
	idleFightUnits = 0;
	std::list<std::string> listOfUnits;
	std::list<std::string> listOfBuildings;
	std::list<std::string> listOfInCompleteUnits;
	std::list<std::string> listOfInCompleteBuildings;

	std::list<std::string> listOfIdleFightUnits;
	std::list<std::string> listOfIdleBuildings;

	for (auto &u : Broodwar->self()->getUnits())
	{
		if (!u->getType().isBuilding())
		{
			if (u->isCompleted())
				listOfUnits.push_back(formated(u->getType()));
			else listOfInCompleteUnits.push_back(formated(u->getType()));
		}

		if (u->getType().isBuilding())
		{
			if (u->isCompleted())
				listOfBuildings.push_back(formated(u->getType()));
			else listOfInCompleteBuildings.push_back(formated(u->getType()));
		}

		if (Broodwar->self()->getRace() == Races::Zerg && u->getType().isResourceDepot())
			idleProdBuildOrLarva += u->getLarva().size();

		if (u->isIdle() && u->getType().isBuilding() && u->canTrain() && !u->getType().isResourceDepot())
		{
			idleProdBuildOrLarva++;
			listOfIdleBuildings.push_back(formated(u->getType()));
		}
		if (u->isIdle() && u->canAttackMove() && !u->getType().isBuilding() && !u->getType().isWorker())
		{
			idleFightUnits++;
			listOfIdleFightUnits.push_back(formated(u->getType()));
		}

		//if (u->canAttackMove())//draw circle to show tank splash radius
		//{
		//	Broodwar->drawCircleMap(u->getPosition(), 25, Color(128, 0, 0), false);//nothing
		//	Broodwar->drawCircleMap(u->getPosition(), 40, Color(64, 0, 0), false);
		//}
		//newUID = u->getID();
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

		if (Broodwar->self()->getRace() != Races::Zerg && u->getType().isResourceDepot() && u->isIdle() && FrameCount < workersProductionStoppedDuring)
			workersProductionStopped++;
		if (Broodwar->self()->getRace() != Races::Zerg && u->getType().isResourceDepot() && !u->isIdle() && FrameCount < workersProductionStoppedDuring)
		{
			workersProductionStopped += notIdleResourceDepot - inProgressWorkers;
			if (notIdleResourceDepot > 1)
			{
				notIdleResourceDepot = 0;
				inProgressWorkers = 0;
			}
		}
	}
	listOfUnits.sort();
	listOfBuildings.sort();
	listOfInCompleteUnits.sort();
	listOfInCompleteBuildings.sort();
	listOfIdleFightUnits.sort();
	listOfIdleBuildings.sort();

	std::map<std::string, int> UnitsMap;
	for (std::string item : listOfUnits)
		UnitsMap[item]++;
	std::map<std::string, int> BuildingsMap;
	for (std::string item : listOfBuildings)
		BuildingsMap[item]++;
	std::string BuildingsFinal;
	for (auto entry : BuildingsMap)
		BuildingsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";
	std::string UnitsFinal;
	for (auto entry : UnitsMap)
		UnitsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";

	std::map<std::string, int> inCompleteUnitsMap;
	for (std::string item : listOfInCompleteUnits)
		inCompleteUnitsMap[item]++;
	std::map<std::string, int> inCompleteBuildingsMap;
	for (std::string item : listOfInCompleteBuildings)
		inCompleteBuildingsMap[item]++;
	std::string inCompleteBuildingsFinal;
	for (auto entry : inCompleteBuildingsMap)
		inCompleteBuildingsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";
	std::string inCompleteUnitsFinal;
	for (auto entry : inCompleteUnitsMap)
		inCompleteUnitsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";

	std::map<std::string, int> idleFightUnitsMap;
	for (std::string item : listOfIdleFightUnits)
		idleFightUnitsMap[item]++;

	std::string idleFightUnitsFinal;
	for (auto entry : idleFightUnitsMap)
		idleFightUnitsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";

	std::map<std::string, int> idleBuildingsMap;
	for (std::string item : listOfIdleBuildings)
		idleBuildingsMap[item]++;

	std::string idleBuildingsFinal;
	for (auto entry : idleBuildingsMap)
		idleBuildingsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";

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
	Broodwar->drawTextScreen(5, 45, "%cIdle workers: %c%d, %cTime: %c%s", Text::Grey, Text::BrightRed, idleWorkers, Text::Grey, Text::BrightRed, getTime(idleWorkersFor / FPS).c_str());

	string m = Broodwar->self()->getRace().getName().substr(0, 1) + "v" + Broodwar->enemy()->getRace().getName().substr(0, 1);
	const char * match = m.c_str();

	Broodwar->drawTextScreen(215, 25, "%c%s %c%s", Text::Purple, mapName.c_str(), Text::Brown, m.c_str());
	Broodwar->drawTextScreen(180, 5, "%c:: CoachAI v2.9.3 ::", Text::Tan);
	//2.9.3:
	//- Ability to include/exclude non-Worker units in the MacroLog (AnyRace_CoachAI.json -> logUnitsProduction)
	//- (Both) Showing the unit & building infos in the replay.
	//- (RepOnly) Showing replay name, map name, replay duration.
	//- (RepOnly) Showing if playerX allied with playerY, and if playerY did the same.

	Broodwar->drawTextScreen(310, 15, "%cWorkers production stopped for: %c%s", Text::Grey, Text::BrightRed, getTime(workersProductionStopped / FPS).c_str());
	Broodwar->drawTextScreen(520, 150, "%cFPS: %c%d, %cFrame: %c%d", 14, 4, FPS, 14, 4, FrameCount);
	//Broodwar->drawTextScreen(520, 15, "%ceS: %c%d, %ceT: %c%s", 14, 4, Broodwar->elapsedTime(), 14, 4, gameTime.c_str());

	if (Broodwar->self()->getRace() == Races::Zerg)
		Broodwar->drawTextScreen(5, 65, "%cIdle production: %c%d Larvae\n\r\n\r%cIdle fighting units: \n\r%c%s", 5, 25, idleProdBuildOrLarva, 5, 25, idleFightUnitsFinal.c_str());
	else Broodwar->drawTextScreen(5, 65, "%cIdle production: \n\r%c%s\n\r%cIdle fighting units: \n\r%c%s", 5, 25, idleBuildingsFinal.c_str(), 5, 25, idleFightUnitsFinal.c_str());

	int Minerals = Broodwar->self()->spentMinerals() - Broodwar->self()->refundedMinerals();
	int Gas = Broodwar->self()->spentGas() - Broodwar->self()->refundedGas();

	Broodwar->drawTextScreen(5, 15, "%cMinerals spent: %d + %cGas spent: %d = %cMacro: %d", 31, Minerals, 7, Gas, 17, Minerals + Gas);
	Broodwar->drawTextScreen(250, 320, "%cSupply blocked in: %c%d", 3, Text::BrightRed, supplyAvailable / 2);
	UnitType supplyName = Broodwar->self()->getRace().getSupplyProvider();
	Broodwar->drawTextScreen(250, 330, "%cSupply maxed with: %c%d %s", 3, Text::BrightRed, (200 - Broodwar->self()->supplyTotal() / 2) / 8, formated(supplyName).c_str());


	if (F5_Pressed % 2 == 0)
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
				pos = std::next(iter, macroLogMap.size() - 22);
			else pos = std::next(iter, macroLogMap.size() - (macroLogMap.size() % 22));
			populatePage();
			indexNow = std::distance(iter, pos);
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
			pos = std::next(iter, indexNow);
			populatePage();
			indexNow = std::distance(iter, pos);
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
				pos = std::next(iter, indexNow);
				populatePage();
			}
		}

		Broodwar->drawTextScreen(150, 25, "%cMacroLog: \n\r%s", Text::Blue, str1.c_str());
		Broodwar->drawTextScreen(185, 25, "\n\r%c%s", Text::Default, str2.c_str());
	}
	if (F6_Pressed == 0)
	{
		Broodwar->drawTextScreen(385, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(520, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteBuildingsFinal.c_str());
	}
	else if (F6_Pressed == 1)
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo1Name"].get<std::string>().c_str(), 4, j["Preset Plan"]["Bo1Text"].get<std::string>().c_str(),
			28, 31, j["Preset Plan"]["Bo1Tips"].get<std::string>().c_str());
	else if (F6_Pressed == 2)
	{
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo2Name"].get<std::string>().c_str(), 4, j["Preset Plan"]["Bo2Text"].get<std::string>().c_str(),
			28, 31, j["Preset Plan"]["Bo2Tips"].get<std::string>().c_str());
	}
	else if (F6_Pressed == 3)
	{
		Broodwar->drawTextScreen(400, 25, "%c%s%c%s \n\r\n\r%cTips:\n\r====\n\r%c%s", 7, j["Preset Plan"]["Bo3Name"].get<std::string>().c_str(), 4, j["Preset Plan"]["Bo3Text"].get<std::string>().c_str(),
			28, 31, j["Preset Plan"]["Bo3Tips"].get<std::string>().c_str());
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
						Broodwar << Broodwar->getLastError() << std::endl; ////Note: When no Minerals patches visible, you'll get "Unit_Does_Not_Exist" here.
					}
				} // closure: has no powerup
			} // closure: if idle
		}
		else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
		{
			static int lastChecked = 0;
			static int lastChecked2 = 0;
			//supplyProviderType
			UnitType supplyUnit = u->getType().getRace().getSupplyProvider();
			//============
			//Build Worker: Order the depot to construct more workers, But only when it is idle.
			//============
			if (Broodwar->self()->minerals() >= 50 && lastChecked2 < FrameCount && !u->isTraining() && workers < maxWorkers && autoTrainWorkers)
			{
				lastChecked2 = FrameCount + FPS * 0;
				u->train(u->getType().getRace().getWorker());

				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame, so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				if (lastErr.getName() != "None")
					Broodwar->registerEvent([pos, lastErr](Game*) { Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
						nullptr,    // condition
						Broodwar->getLatencyFrames());  // frames to run
			} // closure: failed to train idle unit

			std::string supplyName = supplyUnit.getName().c_str();
			//============
			//Build supply:
			//============
			buildSupply(u, supplyUnit, lastChecked);
		}
	} // closure: unit iterator
}

void AnyRace_CoachAI::populatePage()
{
	int posIndex = std::distance(iter, pos);
	str1 = ""; str2 = "";
	for (pos; std::distance(iter, pos) < posIndex + 22 && pos != multiMap.end(); pos++)
	{
		str1 += (*pos).first + "\n\r";
		str2 += (*pos).second + "\n\r";
	}
}

void AnyRace_CoachAI::Replay()
{
	int repFrames = Broodwar->getReplayFrameCount();
	int scnds = Broodwar->getReplayFrameCount() / 23.81;

	Broodwar->drawTextScreen(180, 333, "%c%s / %s / %c%s", 29, Broodwar->mapFileName().c_str(), mapName.c_str(), 7, getTime(scnds).c_str());
	Broodwar->drawTextScreen(5, 95, "\n\r%cMinerals spent + %cGas spent = %cMacro", 31, 7, 17);

	int y1 = 120;
	std::list<Player> playersList;
	for (auto p : players)
	{
		if (!p->isNeutral() && !p->isObserver() && p->spentMinerals() != 0)
			playersList.push_back(p);
	}

	std::list<std::string> forceList;
	for (auto p : playersList)
	{
		std::string forceName = p->getForce()->getName().c_str();
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
					string pTempName = pTemp->getName().substr(0, 6);
					allies.append(pTempName + " ");
					color2 = pTemp->getTextColor();
					if (color2 == 2)
						color2 = 17;
				}
			}

			if (!isThereAllies)
				allies = "";

			Broodwar->drawTextScreen(5, y1, "%c(%s) %s %c%s: %c%d + %c%d = %c%d",
				color, p->getRace().getName().substr(0, 1).c_str(), p->getName().substr(0, 6).c_str(), color2, allies.c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas);
			y1 += 10;
		}
		else
		{	//team
			if (!(std::find(forceList.begin(), forceList.end(), forceName) != forceList.end()))
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
						Broodwar->drawTextScreen(5, y1, "%c(%s) %s: %c%d + %c%d = %c%d", color, f_p->getRace().getName().substr(0, 1).c_str(), f_p->getName().c_str(), 31, Minerals, 7, Gas, 17, Minerals + Gas);
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
	for (auto &u : Broodwar->getAllUnits())
	{
		Color color;
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
				color = u->getPlayer()->getColor();

				Broodwar->drawLineMap(building, RP, Text::BrightRed);
				Broodwar->drawCircleMap(RP.x, RP.y, 5, Text::BrightRed, true);
			}
		}
		else
		{
			Position unitPos = u->getPosition();
			Position orderPos = u->getOrderTargetPosition();
			std::string orderName = u->getOrder().getName();

			if (unitPos.isValid() && unitPos.y != 0 && orderPos.isValid() && orderPos.y != 0)
			{
				color = u->getPlayer()->getColor();

				Broodwar->drawLineMap(unitPos, orderPos, color);
				Broodwar->drawTextMap(unitPos, orderName.c_str());
				Broodwar->drawCircleMap(orderPos.x, orderPos.y, 5, color, true);
			}
		}
	}

	for (auto &u : Broodwar->getSelectedUnits())
	{
		std::list<std::string> upgrades;
		std::list<std::string> techno;
		std::list<std::string> listOfUnits;
		std::list<std::string> listOfBuildings;
		std::list<std::string> listOfInCompleteUnits;
		std::list<std::string> listOfInCompleteBuildings;

		for (auto &up : u->getPlayer()->getUnits())
		{
			if (!up->getType().isBuilding())
			{
				if (up->isCompleted())
					listOfUnits.push_back(formated(up->getType()));
				else listOfInCompleteUnits.push_back(formated(up->getType()));
			}
			if (up->getType().isBuilding())
			{
				if (up->isCompleted())
					listOfBuildings.push_back(formated(up->getType()));
				else listOfInCompleteBuildings.push_back(formated(up->getType()));
			}
		}
		listOfUnits.sort();
		listOfBuildings.sort();
		listOfInCompleteUnits.sort();
		listOfInCompleteBuildings.sort();
		std::map<std::string, int> UnitsMap;
		for (std::string item : listOfUnits)
			UnitsMap[item]++;
		std::map<std::string, int> BuildingsMap;
		for (std::string item : listOfBuildings)
			BuildingsMap[item]++;
		std::string BuildingsFinal;
		for (auto entry : BuildingsMap)
			BuildingsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";
		std::string UnitsFinal;
		for (auto entry : UnitsMap)
			UnitsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";

		std::map<std::string, int> inCompleteUnitsMap;
		for (std::string item : listOfInCompleteUnits)
			inCompleteUnitsMap[item]++;
		std::map<std::string, int> inCompleteBuildingsMap;
		for (std::string item : listOfInCompleteBuildings)
			inCompleteBuildingsMap[item]++;
		std::string inCompleteBuildingsFinal;
		for (auto entry : inCompleteBuildingsMap)
			inCompleteBuildingsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";
		std::string inCompleteUnitsFinal;
		for (auto entry : inCompleteUnitsMap)
			inCompleteUnitsFinal += std::to_string(entry.second) + " " + entry.first + "\n\r";
		Broodwar->drawTextScreen(385, 25, "%cUnits:\n\r%c%s%cUnits in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, UnitsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteUnitsFinal.c_str());
		Broodwar->drawTextScreen(520, 25, "%cBuildings:\n\r%c%s%cBuildings in progress:\n\r%c%s", Text::DarkGreen, Text::GreyGreen, BuildingsFinal.c_str(), Text::DarkGreen, Text::GreyGreen, inCompleteBuildingsFinal.c_str());

		for (auto &ug : UpgradeTypes::allUpgradeTypes())
		{
			if (u->getPlayer()->getUpgradeLevel(ug))
			{
				for (auto &ut : ug.whatUses())
				{
					if (ug.maxRepeats() > 1)
						upgrades.push_back(ug.getName() + " +" + std::to_string(u->getPlayer()->getUpgradeLevel(ug)));
					else upgrades.push_back(formated(ut) + "s " + ug.getName());
				}
			}
		}
		for (auto &tech : TechTypes::allTechTypes())
		{
			if (u->getPlayer()->hasResearched(tech) && tech.whatResearches() != UnitTypes::None)
				for (auto &ut : tech.whatUses())
					techno.push_back(tech.getName());
		}
		upgrades.unique();
		upgrades.sort();
		techno.unique();
		techno.sort();
		std::string upgradesFinal;
		for (auto entry : upgrades)
			upgradesFinal += entry + "\n\r";
		std::string technoFinal;
		for (auto entry : techno)
			technoFinal += entry + "\n\r";
		Broodwar->drawTextScreen(220, 25, "%cTech: \n\r%c%s%cUpgrades: \n\r%c%s", Text::Blue, Text::Teal, technoFinal.c_str(), Text::Blue, Text::Teal, upgradesFinal.c_str());
	}
	return;
}

void AnyRace_CoachAI::buildSupply(const BWAPI::Unit & u, BWAPI::UnitType &supplyUnit, int &lastChecked)
{
	//supplyBuilder
	Unit Probe_SCV_Hatchery = u->getClosestUnit(GetType == supplyUnit.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);//

	if (Probe_SCV_Hatchery != nullptr) //If a unit was found (Probe_SCV_Hatchery)
	{
		int i = Probe_SCV_Hatchery->getID();

		int inProgressSupplies = Broodwar->self()->incompleteUnitCount(supplyUnit);

		// previously:lastErr == Errors::Insufficient_Supply ---> was below
		if (Broodwar->self()->minerals() >= 100 && lastChecked < FrameCount && supplyAvailable / 2 <= autoBuildSuppliesBeforeBlocked && Broodwar->self()->incompleteUnitCount(supplyUnit) == 0)
		{
			lastChecked = FrameCount + FPS * 10;
			if (supplyUnit.isBuilding())
			{
				TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyUnit, Probe_SCV_Hatchery->getTilePosition());
				if (targetBuildLocation)
				{
					// Register an event that draws the target build location
					Broodwar->registerEvent([targetBuildLocation, supplyUnit](Game*) { Broodwar->drawBoxMap(Position(targetBuildLocation), Position(targetBuildLocation + supplyUnit.tileSize()), Colors::Blue); },
						nullptr, supplyUnit.buildTime());  // frames to run

					//========================
					//Build Pylon/Supply Depot:
					//========================
					Probe_SCV_Hatchery->build(supplyUnit, targetBuildLocation);
				}
			}
			//==============
			//Build Overload:
			//==============
			else Probe_SCV_Hatchery->train(supplyUnit);// Train the supply provider (Overlord) if the provider is not a structure
		} // closure: supplyBuilder is valid
	} // closure: insufficient supply
}

void AnyRace_CoachAI::onSendText(std::string text)
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
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
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
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
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
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void AnyRace_CoachAI::onUnitMorph(BWAPI::Unit unit)
{
	UnitType ut = unit->getType();
	string un = formated(ut);

	if (unit->getPlayer() == Broodwar->self() && un != "Egg")
	{
		if (!logWorkersAndSupplyProduction && (ut.isWorker() || ut == Broodwar->self()->getRace().getSupplyProvider()))
			return;
		else if (!logUnitsProduction && (!ut.isWorker() && ut != Broodwar->self()->getRace().getSupplyProvider() && !ut.isBuilding()))
			return;
		else
			macroLogMap[un + " #" + std::to_string(unit->getID())] = gameTime;
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
			macroLogMap[un + " #" + std::to_string(unit->getID())] = gameTime;
	}
}

void AnyRace_CoachAI::onUnitComplete(BWAPI::Unit unit)
{
}
