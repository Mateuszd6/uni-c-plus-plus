#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

// Class representing imperial starship. Template argument U is the type used
// to store shield and attackPower of the ship.
template <typename U>
struct ImperialStarship {
    using valueType = U;

    U shield;
    U attackPower;

    U getShield() {
        return shield;
    }
    U getAttackPower() {
        return attackPower;
    };
    
    // Reduces shield by damage, but when damage is greater than shield,
    // sets it to 0.
    void takeDamage(U damage) {
        shield -= std::min(damage, shield);
    }

    ImperialStarship(U shield, U attackPower) {
        this->shield = shield;
        this->attackPower = attackPower;
    }
};

template <typename U>
using DeathStar = ImperialStarship<U>;

template <typename U>
using ImperialDestroyer = ImperialStarship<U>;

template <typename U>
using TIEFighter = ImperialStarship<U>;

// Function attack for attacking an Explorer.
template <typename IU, typename RU>
void attack(ImperialStarship<IU> &imperialShip, Explorer<RU> &rebelShip) {
    if (imperialShip.getShield() && rebelShip.getShield()) {
        rebelShip.takeDamage(imperialShip.getAttackPower());
    }
}

// Function attack for attacking other rebel ships.
template <typename IU, typename U, size_t minSpeed, size_t maxSpeed>
void attack(ImperialStarship<IU> &imperialShip,
            RebelStarship<U, minSpeed, maxSpeed, true> &rebelShip) {
    if (imperialShip.getShield() && rebelShip.getShield()) {
        rebelShip.takeDamage(imperialShip.getAttackPower());
        imperialShip.takeDamage(rebelShip.getAttackPower());
    }
}

// We will never be here, but compiler must have this function.
template <typename ImpShip, typename RebShip>
void attack(ImpShip &, RebShip &) {
}

#endif // IMPERIALFLEET_H
