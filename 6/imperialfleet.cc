#include "imperialfleet.h"

#include <algorithm>
#include <memory>
#include <numeric>

SingleImperialStarship::SingleImperialStarship(ShieldPoints shield,
                                               AttackPower power)
    : SingleStarship{shield}, SingleFighter{power} {
}

DeathStar::DeathStar(ShieldPoints shield, AttackPower power)
    : SingleImperialStarship{shield, power} {
}

ImperialDestroyer::ImperialDestroyer(ShieldPoints shield, AttackPower power)
    : SingleImperialStarship{shield, power} {
}

TIEFighter::TIEFighter(ShieldPoints shield, AttackPower power)
    : SingleImperialStarship{shield, power} {
}

Squadron::Squadron(
    const std::vector<std::shared_ptr<ImperialStarship>> &ships_vector)
    : ships{ships_vector} {
}

Squadron::Squadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>>
        &ships_initlist)
    : ships{ships_initlist} {
}

AttackPower Squadron::getAttackPower() const {
    return std::accumulate(
        ships.begin(), ships.end(), ShieldPoints{0},
        [](auto const &x, auto const &y) { return x + y->getAttackPower(); });
}

ShieldPoints Squadron::getShield() const {
    return std::accumulate(
        ships.begin(), ships.end(), ShieldPoints{0},
        [](auto const &x, auto const &y) { return x + y->getShield(); });
}

size_t Squadron::getQuantity() const {
    return std::accumulate(ships.begin(), ships.end(), size_t{0},
                           [](auto const &x, auto const &y) {
                               auto quantity = y->getQuantity();
                               return x + quantity;
                           });
}

void Squadron::takeDamage(AttackPower dmg) {
    for (auto &ship : ships)
        ship->takeDamage(dmg);

    ships.erase(std::remove_if(ships.begin(), ships.end(), helper::isShipDead),
                ships.end());
}

std::shared_ptr<DeathStar> createDeathStar(ShieldPoints shield,
                                           AttackPower power) {
    return std::make_shared<DeathStar>(shield, power);
}

std::shared_ptr<ImperialDestroyer> createImperialDestroyer(ShieldPoints shield,
                                                           AttackPower power) {
    return std::make_shared<ImperialDestroyer>(shield, power);
}

std::shared_ptr<TIEFighter> createTIEFighter(ShieldPoints shield,
                                             AttackPower power) {
    return std::make_shared<TIEFighter>(shield, power);
}

std::shared_ptr<ImperialStarship> createSquadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships) {
    return std::make_shared<Squadron>(ships);
}

std::shared_ptr<ImperialStarship>
createSquadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships) {
    return std::make_shared<Squadron>(ships);
}
