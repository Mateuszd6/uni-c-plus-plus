#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

#include "helper.h"

#include <memory>
#include <vector>

// Common class for all imperial starships.
struct ImperialStarship : public virtual BaseFighter {
  protected:
    ImperialStarship() = default;
};

// Singular imperial starship, with getShield() and getAttackpower() implemented
struct SingleImperialStarship : public SingleStarship,
                                public SingleFighter,
                                public ImperialStarship {
  protected:
    SingleImperialStarship(ShieldPoints shield, AttackPower power);
};

struct DeathStar : public SingleImperialStarship {
  public:
    DeathStar(ShieldPoints shield, AttackPower power);
};

struct ImperialDestroyer : public SingleImperialStarship {
  public:
    ImperialDestroyer(ShieldPoints shield, AttackPower power);
};

struct TIEFighter : public SingleImperialStarship {
  public:
    TIEFighter(ShieldPoints shield, AttackPower power);
};

// Squadron implements methods to affect all contained ships at the same time
struct Squadron : public ImperialStarship {
  private:
    std::vector<std::shared_ptr<ImperialStarship>> ships;

  public:
    Squadron(
        const std::vector<std::shared_ptr<ImperialStarship>> &ships_vector);
    Squadron(const std::initializer_list<std::shared_ptr<ImperialStarship>>
                 &ships_initlist);

    // Returns the sum of all ships' attack power
    AttackPower getAttackPower() const override;
    // Returns the sum of all ships' shield
    ShieldPoints getShield() const override;
    // Returns the number of ships, including nested squadrons
    size_t getQuantity() const override;
    // Damages all ships with dmg damage
    void takeDamage(AttackPower dmg) override;
};

std::shared_ptr<ImperialStarship>
createSquadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships);

std::shared_ptr<ImperialStarship> createSquadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships);

std::shared_ptr<DeathStar> createDeathStar(ShieldPoints shield,
                                           AttackPower power);

std::shared_ptr<ImperialDestroyer> createImperialDestroyer(ShieldPoints shield,
                                                           AttackPower power);

std::shared_ptr<TIEFighter> createTIEFighter(ShieldPoints shield,
                                             AttackPower power);

#endif // IMPERIALFLEET_H
