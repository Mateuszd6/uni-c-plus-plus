#ifndef REBELFLEET_H
#define REBELFLEET_H

#include <algorithm>
#include <cassert>

// Class representing rebel starship. Template argument U is the type used
// to store shield and speed of the ship and if ship is XWing or StarCruiser
// then also stores its attackPower.
template <typename U, size_t minSpeed, size_t maxSpeed, bool attack>
struct RebelStarship {};

// Specialization of the template. RebelStarship has attack power.
template <typename U, size_t minSpeed, size_t maxSpeed>
struct RebelStarship<U, minSpeed, maxSpeed, true> {
    using valueType = U;

    U shield;
    U speed;
    U attackPower;

    U getShield() {
        return shield;
    }
    U getSpeed() {
        return speed;
    }
    U getAttackPower() {
        return attackPower;
    }
    
    // Reduces shield by damage, but when damage is greater than shield,
    // sets it to 0.
    void takeDamage(U damage) {
        shield -= std::min(damage, shield);
    }

    RebelStarship(U shield, U speed, U attackPower) {
        // First we check if the speed is > 0. Then cast it to size_t to
        // supress warrnings.
        assert(speed > 0);
        assert(minSpeed <= (size_t)speed && (size_t)speed <= maxSpeed);
        this->shield = shield;
        this->speed = speed;
        this->attackPower = attackPower;
    }
};

// Specialization of the template. RebelStarship doesn't have attack power.
template <typename U, size_t minSpeed, size_t maxSpeed>
struct RebelStarship<U, minSpeed, maxSpeed, false> {
    using valueType = U;

    U shield;
    U speed;

    U getShield() {
        return shield;
    }
    U getSpeed() {
        return speed;
    }
    
    // Reduces shield by damage, but when damage is greater than shield,
    // sets it to 0.
    void takeDamage(U damage) {
        shield -= std::min(damage, shield);
    }

    RebelStarship(U shield, U speed) {
        // First we check if the speed is > 0. Then cast it to size_t to
        // supress warrnings.
        assert(speed > 0);
        assert(minSpeed <= (size_t)speed && (size_t)speed <= maxSpeed);
        this->shield = shield;
        this->speed = speed;
    }
};

template <typename U>
using Explorer = RebelStarship<U, 299796, 2997960, false>;

template <typename U>
using XWing = RebelStarship<U, 299796, 2997960, true>;

template <typename U>
using StarCruiser = RebelStarship<U, 99999, 299795, true>;

#endif // REBELFLEET_H
