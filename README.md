_**This BWAPI-AI/UDAI (User defined AI) works as an in-game helper and also as an online replay analyzer.
It can help the players at-all-levels to improve, also to test and sense and remember things that weren't easy/possible before.**_
# Coach AI has arrived !
[![](https://i.ibb.co/VgLmsJC/Carrier-SCR-Art2-resize.jpg)](https://www.youtube.com/watch?v=GhJ0zlsOEyo)

> _**Note**_:
* CoachAI works only with StarCraft v1.16.1 using ChaosLauncher.
* When playing in ICCUP, the game is logged and you'll get the win/lose points, this won't happen if one of the players is Computer.

#  Features in-game & in-replay:
- The RealTime measure of Macro (train/build/upgrade) is here, Terran repairs not accounted, many players still measure it just by the used supplies at specific time or by inaccurate replay analysis tools, in-replay you can know @any second who is the best Macro player (same as the score screen you see at the end of game/replay).
- Want to know how many unit/building has been lost in-game/in-replay ?, just select one of the same type.
- The # of units/buildings and also the ones in-progress, and the remaining build time for each one in progress, also the remaining time for each Tech/Upgrade in progress.
- Buildings that has Rally Point will draw a line to the RP.
- Displaying the number of the assigned mineral workers on each Nexus/CC/Hatchery.
- WorkersCut measure, counts the time that Nexus/CC has been idle, if you have 2 bases the counter will be doubled, 3 -> tripled, etc.., it works for P & T, not Z.
- Idle Workers measure displays the number of idle workers, and the total lost time (as with Workers Cut, this increase when there is more Idle Workers).
- Minerals Above measure that counts the time of being above 750/1000 minerals.
#  Features in-game:
- The # of Minerals/Gas workers from all bases.
- The # of idle production buildings (for Zerg, only the number of Larvae displayed).
- The # of idle of idle fighting units (only non-worker units that canAttackMove: HT, Siege-Mode Tank, Lurker cannot, Medics can !).
- The # of all killed enemies during the game.
- The # of all current enemies, their supplies, Minerals/Gas, upgrades, in-progress things and you can even see the enemies while they move in the dark/fog. (this works in multiplayer, beware !)
- MacroLog: Logs all build/train/upgrade actions and its time during the game.
- Hotkeys tracker/counter: Tracks how much you press a specific hotkey.
- The total time of WorkersCut and the number of lost workers due to that.
- WorkersCut log (only 2+ seconds cut is calculated): to see when exactly in the game you've stopped making workers and for how long (I personally record my screen to see what I was doing, because replay is not always an option).
- MineralsAbove log: logs the time when you're above certain amount of minerals, and for how long that was, to see when exactly you forgot to Macro.
- Detecting the game speed, by just looking at the FPS value: 24=Fastest, 21=Faster, 18=Fast, 15=Normal, etc..
- Customizable notification sound and period when WorkersCut happens (5s), idle worker exist (5s) or you've missed multitasking (20s).
- Multitasking++:
- Screen Multitasking:
A counter for each screen you step into, with total jumps during the game, the average stay @1 screen, the total stay on each screen for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you stay @1 screen for above 20s (customizable live, by editing sameScreenOrSelectionWarningEvery).
- Selection Multitasking:
A counter for each selection you select, with total switches during the game, the average focus @1 selection, the total focus on each selection for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you focus @1 selection for above 20s (customizable live, by editing sameScreenOrSelectionWarningEvery).
- TimedBO: most of us might forget what was planned before the game, so a written BO and tips on the game screen that dynamically prepares you for "What next?" JIT, will accelerate the familiarity with a specific build remarkably.
#  Features in-replay:
- Showing the # of units/buildings & the types of tech/upgrade for each player (just select any player's unit to display the infos).
- Showing the replay duration beforehand, and also the replay name, date, game title, game type and the map used.
- Showing each unit order and drawing a line to the order target if there is a target.
- Recording each player BO. (only the 6th worker is logged for each player, to see his split skill. only the 1st created supply unit is logged, you don't want every Overlord created to be recorded !, for more info see replayLogUnitsFor/replayLogSupplyFor in the json file)
- Easily distinguishing which players against which on team replays, (even SC:R doesn't has this !, maybe time to write some code lazy Blizzard ?)
- During team replays showing if playerX allied with playerY, and if playerY also did the same.
- Automatic highlighting of idle workers.
- Recording the max number of idle workers of each player, and when that happened.
- The screen will automatically move to where a Nuclear launch, Comsat scan or Storm is positioned.
- An intuitive/easier way to toggle vision of the selected player (and his allies) by pressing F7 (SC:R requires that you blind ALL other players individually to get the vision of 1 player which is not smart, here we just need to select a player/or 1 of his allies to see the required vision).
- Displaying live score of produced/killed/lost units/buildings, to get the current state of players, this is about unit/building control (more close to micro, same as the score screen you see at the end of game/replay).

# AnyRace_CoachAI.json:
This config file allows the player to modify/disable/enable many values:
- _**autoTrainWorkers (default=false)**_: trains 1 worker @time from each base until "maxWorkers" number reached (for Zerg only 1 Hatchery is used for producing).
- **_maxWorkers (default=50)_**: only works if autoTrainWorkers = true
- _**autoMine (default=false)**_: makes workers gather Minerals/Gas automatically like in SC2, so there is no idle worker.
- **_autoBuildSuppliesBeforeBlocked (default=-200)_**: if you set it to 6, the AI will try to build supply units when its 24/30 (for Zerg only 1 Hatchery is used for building)
- **_maxProductionBuildingQueue (default=2)_**: prevent the player from queuing more workers/units from the same production building.
- **_workerCutWarningEvery (default=5)_**: in seconds
- **_idleWorkerWarningEvery (default=5)_**: ^
- **_idleProductionBuildingWarningEvery (default=10)_**: ^
- **_idleFightingUnitWarningEvery (default=20)_**: ^
- **_totalTimeOnScreenOrSelectionAbove (default=5)_**: see Multitasking++
- **_sameScreenOrSelectionWarningEvery (default=20)_**: see Multitasking++
- **_logWorkersAndSupplyProduction (default=true)_**: include Probes/Pylons, etc.. in the MacroLog.
- **_logUnitsProduction (default=true)_**: include Dragoons/Zealots, etc.. in the MacroLog.
- **_workersCutCalculationPeriod (default=540)_**: affects both game/replay, calculates WorkersCut for the specified 1st minutes in the game/replay.
- **_replayLogUnitsFor (default=420)_**: in-replay BO recorder, for how long to log units production. (until 7:00)
- **_replayLogSupplyFor (default=40)_**: in-replay BO recorder, for how long to log the supply indicator. (like 12,Nexus --> until 40/?)
- **_dontDrift (default=-1)_**: initiates a custom CountDownTimer, just like seen in some missions, but you can use it in any map to practice/polish specific task/timing, when the timer ends the game will too, you can set it in seconds, it works whether its a game start or a saved game, when it reaches 00:15, visual/sound warning will start (this helped me remembering to save the game-state @lets say 6:00), in order to master the game-phases/tasks partially 1st, then as a whole.
- **_workerCutLimit (default=3600)_**: you can set this to 120 (as a goal/target), this will end the game immediately if you intermittently cut workers for 2 minutes in the default 1st 9 minutes.
- **_workerCutLimitForOnce (default=-1)_**: same as above but a 1 time continuous cut with this value (lets say 20s) will end the game. 
- **_spend_more_minerals_WarningFor (default=900)_**: minerals sound warning "Spend more minerals", if minerals 500-750 its every 3s, if > 750 its every 1s, until the default 15 minutes, that doesn't happen in the [multitask/macro maps](https://liquipedia.net/starcraft/Micro_Training_Maps#Multitask/Macro_training), because it has it already.
- **_mineralsAboveLog (default=750)_**: logs the time when you're above certain amount of minerals, and for how long that was, to see when exactly you forgot to Macro.
- **_stickyScreen (default=1)_**: which screen to display by default ?, 1 = TimedBO, 2 = WorkersCut log & MineralsAbove log, 3 = Idle Production & Fighting units & Multitasking.
- **_Bo1Text_**: an editable static build order that appear on-game screen, you can use "\n" to move to a new line.
- **_TimedBO1_**: an editable dynamic build order that appear on-game screen, it highlights what should be done next JIT.

# Message shortcuts:
The player can input some text the UDAI can understand:

**_c1_**="show me the money", **_c2_**="black sheep wall", **_c3_**="operation cwal", **_c4_**="power overwhelming", **_c5_**="staying alive", **_c6_**="there is no cow level".

**_/speed #_**=The delay per frame in milliseconds (Fastest: 42ms/frame, Slowest: 167ms/frame). /speed 0 will run EXTREMELY fast, to reset speed, just type: **_/speed_**

**_UDAI_**=only in multiplayer (ICCUP/LAN) to acknowledge superiority from the non-CoachAI player, and that the CoachAI-player is not cheating by having intel about him.

**_?_** (Only in-replay, obviously!)=Who wins?

# Hotkeys:
**_F5_**=Toggle between the display of the MacroLog (**_Up,Down_**=Scroll through the MacroLog when more than one page exists) & the Hotkeys tracker. When on the Hotkeys tracker screen, you can briefly see other screens by pressing _**Control or Shift**_. (in-replay **_F5_** switches between different screens)

**_F6 (also in-replay)_**=Toggle the display mode of in-progress units/buildings.

**_F7_**=Switches between the preset game plans.(in-replay **_F7_** toggles the vision of the selected player)

**_F8_**=Switches between the current enemies infos, and also drawing the selected enemy army even while moving in the dark/fog, 
pressing Shift toggles the in-progress mode.

**_F9_**=Finds the idle workers (like in SC2).

**_F11 (also in-replay)_**=Displays buildings gaps values and ground units sizes, for quick and easy reference to better walling.

**_F12 (also in-replay)_**=Hide/show the CoachAI's drawing/text.

**_Control+F1,F2,F3_**=Switches between the TimedBOs.

# To re-build this project:
* Setup Visual Studio 2017 choosing the following components:
![requirements](https://github.com/captain-majid/CoachAI/blob/master/c%2B%2B%20BWAPI%20requirements.png)
* Extract [lib.7z](https://drive.google.com/open?id=1UyIE_go2FRpS1OaRAl0JE_jcbV8ZfVqf) contents into the same folder that contains AnyRace_CoachAI.sln, so that a "lib" folder with these contents gets created.

# Installation:
- Extract the .7z file contents from this [topic](http://www.teamliquid.net/forum/brood-war/531310-human-like-broodwar-ais-revamped) to SC v1.16.1 folder.
- Extract CoachAI.zip contents to StarCraft\bwapi-data folder.
- Watch 1m from this [video](https://www.youtube.com/watch?v=vmKHsCGFMa0), but change the AI from Protoss_McRave.dll to AnyRace_CoachAI.dll.

[Download](http://drives.news/CoachAI)
