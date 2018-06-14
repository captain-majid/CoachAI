#pragma once
#include <BWAPI.h>


// Remember not to use "Broodwar" in any global class constructor!

class AnyRace_CoachAI : public BWAPI::AIModule
{
public:
  // Virtual functions for callbacks, leave these as they are.
  virtual void onStart() override;
  virtual void onEnd(bool isWinner) override;
  virtual void onFrame() override;
  void populatePage();
  void Replay();
  void buildSupply(const BWAPI::Unit & u, BWAPI::UnitType &supplyUnit, int &lastChecked);
  virtual void onSendText(std::string text) override;
  virtual void onReceiveText(BWAPI::Player player, std::string text)override ;
  virtual void onPlayerLeft(BWAPI::Player player) override;
  virtual void onNukeDetect(BWAPI::Position target) override;
  virtual void onUnitDiscover(BWAPI::Unit unit) override;
  virtual void onUnitEvade(BWAPI::Unit unit) override;
  virtual void onUnitShow(BWAPI::Unit unit) override;
  virtual void onUnitHide(BWAPI::Unit unit) override;
  virtual void onUnitCreate(BWAPI::Unit unit) override;
  virtual void onUnitDestroy(BWAPI::Unit unit) override;
  virtual void onUnitMorph(BWAPI::Unit unit) override;
  virtual void onUnitRenegade(BWAPI::Unit unit) override;
  virtual void onSaveGame(std::string gameName) override;
  virtual void onUnitComplete(BWAPI::Unit unit) override;
  // Everything below this line is safe to modify.
};
