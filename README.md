>_**This BWAPI-AI/UDAI (User defined AI) works as an in-game helper and also as an online replay analyzer.
It can help the players at-all-levels to improve, also to test and sense and remember things that weren't easy/possible before.**_
# Coach AI has arrived !
![Coach AI has arrived !](https://i.ibb.co/VgLmsJC/Carrier-SCR-Art2-resize.jpg)
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
- Minerals Above measure that counts the time of being above 250/500/750/1000 minerals.
#  Features in-game:
- The # of Minerals/Gas workers from all bases.
- The # of idle production buildings (for Zerg, only the number of Larvae displayed).
- The # of idle of idle fighting units (only non-worker units that canAttackMove: HT, Siege-Mode Tank, Lurker cannot, Medics can !, also note that Hold position units aren't idle).
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
Screen Multitasking:
A counter for each screen you step into, with total jumps during the game, the average stay @1 screen, the total stay on each screen for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you stay @1 screen for above 20s (customizable live, by editing sameScreenOrSelectionWarningEvery).
Selection Multitasking:
A counter for each selection you select, with total switches during the game, the average focus @1 selection, the total focus on each selection for above 5s (customizable live, by editing totalTimeOnScreenOrSelectionAbove) and finally, a customizable notification sound when you focus @1 selection for above 20s (customizable live, by editing sameScreenOrSelectionWarningEvery).
- TimedBO: most of us might forget what was planned before the game, so a written BO and tips on the game screen that dynamically prepares you for "What next?" JIT, will accelerate the familiarity with a specific build remarkably.

