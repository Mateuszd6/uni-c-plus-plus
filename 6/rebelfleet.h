#ifndef REBELFLEET_H
#define REBELFLEET_H

#include "helper.h"
#include "imperialfleet.h"

// Common class for all rebel starships. Rebel starship in addition to base
// starship/base attacker properties contain speed info (no idea why).
struct RebelStarship : public SingleStarship {
  private:
    Speed const speed;

  protected:
    RebelStarship(ShieldPoints shield, Speed speed);

  public:
    // Returns the speed of the rebel ship.
    Speed getSpeed() const;

    // Rebel starships behave differently when they take damage for imperials.
    virtual void receiveAttack(ImperialStarship *imperialStarship);
};

// Rebel defenders have special behaviour on taking damage, as after receiving
// damage they attack imperial attacker.
struct RebelDefender : public RebelStarship, public SingleFighter {
  protected:
    RebelDefender(ShieldPoints shield, Speed speed, AttackPower power);

  public:
    void receiveAttack(ImperialStarship *imperialStarship) override;
};

struct Explorer : public RebelStarship {
  public:
    Explorer(ShieldPoints shield, Speed speed);
};

struct StarCruiser : public RebelDefender {
  public:
    StarCruiser(ShieldPoints shield, Speed speed, AttackPower power);
};

struct XWing : public RebelDefender {
  public:
    XWing(ShieldPoints shield, Speed speed, AttackPower power);
};

std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed);

std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed,
                                               AttackPower power);

std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed,
                                   AttackPower power);

#endif // REBELFLEET_H
