#include "helper.h"
#include "rebelfleet.h"

#include <cassert>

bool StrategyDivisible2Or3ButNot5::canAttack(Time t) const {
    return (t % 2 == 0 || t % 3 == 0) && (t % 5 != 0);
}

// Increments time and wraps after exceeding maxTime.
Time ModuloClock::stepTime(Time currentTime, Time maxTime,
                           Time timeStep) const {
    return (currentTime + timeStep) % (maxTime + 1);
}

SingleStarship::SingleStarship(ShieldPoints shield) : shieldPoints{shield} {
}

ShieldPoints SingleStarship::getShield() const {
    return shieldPoints;
}

void SingleStarship::takeDamage(AttackPower damage) {
    if (damage > shieldPoints)
        damage = shieldPoints;
    shieldPoints -= damage;
}

size_t SingleStarship::getQuantity() const {
    return shieldPoints > 0;
}

SingleFighter::SingleFighter(AttackPower attack) : attackPower{attack} {
}

AttackPower SingleFighter::getAttackPower() const {
    return attackPower;
}

void helper::assertShipIsFast(const RebelStarship *ship) {
    assert(299796 <= ship->getSpeed() && ship->getSpeed() <= 2997960);
}

void helper::assertShipIsSlow(const RebelStarship *ship) {
    assert(99999 <= ship->getSpeed() && ship->getSpeed() <= 299795);
}

bool helper::isShipDead(const std::shared_ptr<BaseStarship> &ship) {
    return ship->getQuantity() == 0;
}
