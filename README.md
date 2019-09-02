_**This BWAPI-AI/UDAI (User defined AI) works as an in-game helper and also as an online replay analyzer.
It can help the players at-all-levels to improve, also to test and sense and remember things that weren't easy/possible before.**_
# Coach AI has arrived !
[![](https://i.ibb.co/VgLmsJC/Carrier-SCR-Art2-resize.jpg)](https://www.youtube.com/watch?v=GhJ0zlsOEyo)

> _**Note**_:
* CoachAI works only with StarCraft v1.16.1 using ChaosLauncher.
* When playing in ICCUP, the game is logged and you'll get the win/lose points, this won't happen if one of the players is Computer.
* In multiplayer matches (ICCUP/Lan), CoachAI will auto send message every 10s (for the 1st 2m) to each human enemy until they all acknowledge/accept that CoachAI-player has multiple advantages (cheats), so nobody can use it as a deceiving/hack tool. Also the non-CoachAI-player can send a messsage “noCoachAI” during the 1st 2m, to force the CoachAI-player to leave. (in team matches, messages sent automatically to allies, press Shift+Enter to send to all, or the CoachAI-player won't get it)
* The CoachAI can run on Windows XP starting from BWAPI v4.4.

#  Features in-game & in-replay:
- A RealTime measure for _**Macro = spent minerals + spent gas**_ (Terran repairs not accounted), many players still measure it just by the used supplies at specific time or by inaccurate replay analysis tools, in-replay you can know @any second who is the best Macro player (same as the score screen you see at the end of game/replay).
- Want to know how many unit/building has been lost in-game/in-replay ?, just select one of the same type.
- The # of units/buildings and also the ones in-progress, and the remaining build time for each one in progress, also the remaining time for each Tech/Upgrade in progress.
- Buildings that has Rally Point will draw a line to the RP.
- Displaying the number of the assigned mineral workers on each Nexus/CC/Hatchery.
- WorkersCut measure, counts the time that Nexus/CC has been idle, if you have 2 bases the counter will be doubled, 3 -> tripled, etc.., it works for P & T, not Z.
- Idle Workers measure displays the number of idle workers, and the total lost time (as with Workers Cut, this increase when there is more Idle Workers).
- Minerals Above measure that counts the time of being above 750/1000 minerals.
- Detecting the game speed, by just looking at the FPS value: 24=Fastest, 21=Faster, 18=Fast, 15=Normal, etc..
#  Features in-game:
- The # of Minerals/Gas workers from all bases.
- The # of idle production buildings (for Zerg, only the number of Larvae displayed), also the buildings IdleTime for Protoss.
- The # of idle of idle fighting units (only non-worker units that canAttackMove: HT, Siege-Mode Tank, Lurker cannot, Medics can !).
- The # of all killed enemies during the game.
- The # of all current enemies, their supplies, Minerals/Gas, upgrades, in-progress things and you can even see the enemies while they move in the dark/fog.
- MacroLog: Logs all build/train/upgrade actions and its time during the game (Tech/Upgrade logged only when its finished, unlike the reply Bo recorder).
- **_Hotkeys tracker/counter_**: Tracks how much you press a specific hotkey.
- The total time of WorkersCut and the number of lost workers due to that.
- **_WorkersCut log_** (only 2+ seconds cut is calculated): to see when exactly in the game you've stopped making workers and for how long (I personally record my screen to see what I was doing, because replay is not always an option).
- **_MineralsAbove log_**: logs the time when you're above certain amount of minerals, and for how long that was, to see when exactly you forgot to Macro.
- Customizable notification sound and period when WorkersCut happens (5s), idle worker exist (5s) or you've missed multitasking (30s/20s).
- **_Screen-Multitasking_**:
A counter for each screen you step into, with total jumps during the game, the average stay @1 screen, the total stay on each screen for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you stay @1 screen for above 30s (customizable live, by editing sameScreenWarningEvery).
- **_Selection-Multitasking_**:
A counter for each selection you select, with total switches during the game, the average focus @1 selection, the total focus on each selection for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you focus @1 selection for above 20s (customizable live, by editing sameSelectionWarningEvery).
- **_The TimedBo_**: all of us forget 1 or more things that was planned before the game, so a written Bo and tips on the game screen that dynamically highlights "What now/next?" while alerting with sound and pronouncing the current step in English JIT and allowing for more step-details to be shown/pronounced out of the succinct Bo form, will accelerate the familiarity with a specific build remarkably, 
#  Features in-replay:
- Showing the # of units/buildings & the types of tech/upgrade for each player (just select any player's unit to display the infos).
- Showing the replay duration beforehand, and also the replay name, date, game title, game type and the map used.
- Showing each unit order and drawing a line to the order target if there is a target.
- Recording each player Bo. Only the 6th worker is logged for each player to see his split skill, for more info see replayLogUnitsFor/replayLogSupplyFor in the json file.
- Easily distinguishing which players against which on team replays, (even SC:R doesn't has this !, maybe time to write some code lazy Blizzard ?)
- During team replays showing JIT if playerX allied with playerY, and if playerY also did the same.
- Automatic highlighting of idle workers.
- Recording the max number of idle workers of each player, and when that happened.
- The screen will automatically move to where a Nuclear launch is positioned (Comsat scan or Storm autoMove is disabled by default).
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
- **_totalTimeOnScreenOrSelectionAbove (default=5)_**: see Multitasking above
- **_sameScreenWarningEvery (default=30)_**: see Multitasking above
- **_sameSelectionWarningEvery (default=20)_**: see Multitasking above
- **_logWorkersAndSupplyProduction (default=false)_**: include Probes/Pylons, etc.. in the MacroLog.
- **_logUnitsProduction (default=true)_**: include Dragoons/Zealots, etc.. in the MacroLog.
- **_workersCutCalculationPeriod (default=540)_**: affects both game/replay, calculates WorkersCut for the specified 1st minutes in the game/replay.
- **_replayLogUnitsFor (default=420)_**: in-replay Bo recorder, for how long to log units production. (until 7:00)
- **_replayLogSupplyFor (default=40)_**: in-replay Bo recorder, for how long to log the supply units & the supply indicator. (like 12,Nexus --> until 40/?)
- **_dontDrift (default=-1)_**: initiates a custom CountDownTimer, just like seen in some missions, but you can use it in any map to practice/polish specific task/timing, when the timer ends the game will too, you can set it in seconds, it works whether its a game start or a saved game, when it reaches 00:15, visual/sound warning will start (this helped me remembering to save the game-state @lets say 6:00), in order to master the game-phases/tasks partially 1st, then as a whole.
- **_workerCutLimit (default=3600)_**: you can set this to 120 (as a goal/target), this will end the game immediately if you intermittently cut workers for 2 minutes in the default 1st 9 minutes.
- **_workerCutLimitForOnce (default=-1)_**: same as above but a 1 time continuous cut with this value (lets say 20s) will end the game. 
- **_spend_more_minerals_WarningFor (default=900)_**: minerals sound warning "Spend more minerals", if minerals 500-750 its every 3s, if > 750 its every 1s, until the default 15 minutes, that doesn't happen in the [multitask/macro maps](https://liquipedia.net/starcraft/Micro_Training_Maps#Multitask/Macro_training), because it has it already.
- **_mineralsAboveLog (default=750)_**: logs the time when you're above certain amount of minerals, and for how long that was, to see when exactly you forgot to Macro.
- **_stickyScreen (default=1)_**: which screen to display by default ?, 1 = TimedBo, 2 = WorkersCut log & MineralsAbove log, 3 = Idle Production & Fighting units & Multitasking.
- **_autoGameSpeed (default=true)_**: some values could go wrong if game speed isn't Fastest=24 FPS, like mine > 750 time, you can set it to false to allow game speed changes.
- **_replayAutoMoveToScanOrStorm (default=false)_**: set it to true if you don't want to miss any action !
- **_TimedBo1_**: an editable dynamic build order that appear on-game screen, it highlights/pronounces what should be done now JIT. Use ';' to move any extra step-details out of the succinct Bo form, to highlight an important TimedBo step (that you keep forgetting), add “!” as the 1st character, like: “04:00 !gascut end”.
- **_TimedBo1 Tips_**: a block of text that appear below a specific TimedBo, you can use "\n" to move to a new line, you can also use ';' here.
- **_Tips1_**: an editable custom tips that appear on-game screen when pressing **_F7_**, you can use "\n" to move to a new line.

# Message shortcuts:
The player can input some text the UDAI can understand:

**_c1_**="show me the money", **_c2_**="black sheep wall", **_c3_**="operation cwal", **_c4_**="power overwhelming", **_c5_**="staying alive", **_c6_**="there is no cow level".

**_?_** (Only in-replay, obviously!)=Who wins?

# Hotkeys:
**_F5_**=Toggle between the display of the MacroLog (**_Up,Down_**=Scroll through the MacroLog when more than one page exists) & the Hotkeys tracker. When on the Hotkeys tracker screen, you can briefly see other screens by pressing _**Ctrl or Shift**_, pressing _**Shift**_ will also show the game type. (in-replay **_F5_** switches between different screens).

**_F6 (also in-replay)_**=Toggle the display mode of in-progress units/buildings.

**_F7_**=Switches between the preset custom tips.(in-replay **_F7_** toggles the vision of the selected player)

**_F8_**=Switches between all players infos (press Ctrl for more), this is useful while observing games and will draw the selected player's army/orders/destinations even while moving in the dark/fog, pressing Ctrl+Shift toggles the in-progress mode.

**_F9_**=Finds the idle workers (like in SC2).

**_F11 (also in-replay)_**=Displays buildings gaps values and ground units sizes, for quick and easy reference to better walling.

**_F12 (also in-replay)_**=Hide/show the CoachAI's drawing/text.

**_Ctrl+F1,F2,F3_**=Switches between the TimedBos.

**_PgUp_**=increase game speed, **_PgDown_**=decrease game speed, **_End_**=/speed 0 will run EXTREMELY fast, **_Home_**=reset speed to Fastest (24 FPS) **_Note_**: speed will revert to 24 FPS if **_"autoGameSpeed: true"_**.

**_Space_**=while busy you can press Space to pronounce the next step in the TimedBo.

**_Comma|Period + #_**=pronounce a specific step in the TimedBo up to 19 (ie: for step3 press ", + 3", step13= ". + 3").

**_Shift|Ctrl + F12_**= Pause|resume the game, when paused you can only move the screen during a multiplayer match.
With some trick this allows pausing a multiplayer game for more than 3 times, which gives the player enough time to comprehend a complex situation and analyse the game-state quickly.

# To re-build this project:
* Setup Visual Studio 2017 choosing the following components:
![requirements](https://github.com/captain-majid/CoachAI/blob/master/c%2B%2B%20BWAPI%20requirements.png)

# Known issues:
- The hotkey tracker is buggy when adding units using shift, also when a unit is lost it still shows as an existing one.
- Cancelled unit/building/upgrade/tech appears in ReplayBo/MacroLog, for a cancelled upgrade/tech you'll additionally get a 'None' item below it.

# Screenshots:
in-game:
![CoachAI](https://im3.ezgif.com/tmp/ezgif-3-00772993d6ab.gif)

in-replay:
![Replay](https://i.ibb.co/zf6Gkgd/replay.png)
![ReplayBo](https://i.ibb.co/zNbZg67/replay-bo.png)

# Installation:
- Extract the .7z file contents from this [topic](http://www.teamliquid.net/forum/brood-war/531310-human-like-broodwar-ais-revamped) to SC v1.16.1 folder.
- Extract CoachAI.zip contents to StarCraft\bwapi-data folder.
- Watch 1m from this [video](https://www.youtube.com/watch?v=vmKHsCGFMa0), but change the AI from Protoss_McRave.dll to AnyRace_CoachAI_usesBWAPI_v4.4.dll.

[Download](https://cutt.ly/CoachAI)
