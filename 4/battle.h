#ifndef BATTLE_H
#define BATTLE_H

#include <iostream>
#include <algorithm>
#include <array>
#include <functional>

#include "imperialfleet.h"
#include "rebelfleet.h"

template <typename T, T t0, T t1, typename... S>
struct SpaceBattle {
    static_assert(t0 <= t1);

  private:
    std::tuple<S...> ships;
    T cnt;

    // Helper function, counts number of square roots that are between 0
    // and t1. This is done at compile time.
    static constexpr size_t countElements(size_t t1_) {
        size_t idx = 0;
        for (T i = 0; size_t(i * i) <= t1_; ++i)
            ++idx;

        return idx;
    }

    // Helper function, makes an array of square roots that are between 0
    // and t1. This is done at compile time.
    constexpr std::array<T, countElements(t1)> makeArrayOfSquareRoots() {
        auto retval = std::array<T, countElements(t1)>{};
        size_t idx = 0;
        for (T i = 0; i * i <= t1; ++i)
            retval[idx++] = i * i;

        return retval;
    }

    // This hack allows us to check if particular ship is an imperial ship, or
    // not.
    template <typename U>
    constexpr static bool isImperialShip(ImperialStarship<U> &) {
        return true;
    }
    template <typename Other>
    constexpr static bool isImperialShip(Other &) {
        return false;
    }

    // Super cool for_eachs stuff. Given a lambda we can iterate over
    // tuple.
    template <std::size_t I = 0, typename Functor, typename... Args>
    inline typename std::enable_if<I == sizeof...(Args), void>::type
    forEachInTuple(std::tuple<Args...> &, Functor) {
    }

    template <std::size_t I = 0, typename Functor, typename... Args>
        inline typename std::enable_if <
        I<sizeof...(Args), void>::type forEachInTuple(std::tuple<Args...> &t,
                                                      Functor f) {
        f(std::get<I>(t));
        forEachInTuple<I + 1, Functor, Args...>(t, f);
    }

  public:

    // Function counts how many ships from imperial fleet are undestroyed.
    size_t countImperialFleet() {
        size_t result = 0;
        auto countShipHelper = [&result](auto ship) {
            if (isImperialShip(ship) && ship.getShield() > 0)
                result++;
        };

        forEachInTuple(ships, countShipHelper);
        return result;
    }

    // Function counts how many ships from rebel fleet are undestroyed.
    size_t countRebelFleet() {
        size_t result = 0;
        auto countShipHelper = [&result](auto ship) {
            if (!isImperialShip(ship) && ship.getShield() > 0)
                result++;
        };

        forEachInTuple(ships, countShipHelper);
        return result;
    }

    // Helper function, calls function attack for every undestroyed ship from
    // imperial fleet.
    void performAttack() {
        auto attackFun = [this](auto &ship) {
            auto attackIfPossible = [&ship](auto &otherShip) {
                attack(ship, otherShip);
            };

            if (isImperialShip(ship))
                forEachInTuple(ships, attackIfPossible);
        };

        forEachInTuple(ships, attackFun);
    }

    // Function prints DRAW when every ship was destroyed, prints IMPERIUM WON
    // when every ship from rebel fleet was destroyed, prints REBELION WON
    // when every ship from imperial fleet was destroyed, otherwise performs
    // attack (if current time is a square root of an integer).
    void tick(T timeStep) {
        if (countRebelFleet() == 0 && countImperialFleet() == 0)
            std::cout << "DRAW\n";
        else if (countRebelFleet() == 0)
            std::cout << "IMPERIUM WON\n";
        else if (countImperialFleet() == 0)
            std::cout << "REBELLION WON\n";
        else {
            // Lets pray that this is done at compile time.
            auto attackTimes{makeArrayOfSquareRoots()};
            // We check whether attack can be performed.
            if (std::binary_search(attackTimes.begin(), attackTimes.end(),
                                   cnt)) {
                performAttack();
            }
        }

        // Updating time by adding timeStep to currentTime.
        cnt = (cnt + timeStep) % (t1 + 1);
    }

    // Constructor for SpaceBattle. All given ships are stored in a tuple.
    // Current time (cnt) is set at t0.
    template <class... Sh>
    SpaceBattle(Sh... battleShips)
        : ships(std::make_tuple(battleShips...)), cnt(t0) {
        static_assert(sizeof...(battleShips) == sizeof...(S));
    }
};

#endif // BATTLE_H
