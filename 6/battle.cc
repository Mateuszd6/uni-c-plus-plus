#include "battle.h"
#include "helper.h"

#include <iostream>
#include <numeric>

SpaceBattle::SpaceBattle(
    std::vector<std::shared_ptr<RebelStarship>> &&rebels_,
    std::vector<std::shared_ptr<ImperialStarship>> &&imperials_,
    std::shared_ptr<BaseStrategy> &&attackStrategy_,
    std::shared_ptr<BaseClock> &&clock_, Time currentTime_, Time maxTime_)
    : rebels{rebels_}, imperials{imperials_}, attackStrategy{attackStrategy_},
      clock{clock_}, currentTime{currentTime_}, maxTime{maxTime_} {
}

size_t SpaceBattle::countImperialFleet() const {
    return std::accumulate(
        imperials.begin(), imperials.end(), size_t{0},
        [](auto const &x, auto const &y) { return x + y->getQuantity(); });
}

// We technically don't have to do it like this, every rebel has quantity 1 in
// the problem statement, but lets be future-proof.
size_t SpaceBattle::countRebelFleet() const {
    return std::accumulate(
        rebels.begin(), rebels.end(), size_t{0},
        [](auto const &x, auto const &y) { return x + y->getQuantity(); });
}

void SpaceBattle::tick(Time timeStep) {
    if (countImperialFleet() == 0) {
        if (countRebelFleet() == 0)
            std::cout << "DRAW\n";
        else
            std::cout << "REBELLION WON\n";
    }
    else if (countRebelFleet() == 0) {
        std::cout << "IMPERIUM WON\n";
    }
    else if (attackStrategy->canAttack(currentTime)) {
        for (auto &imperial : imperials)
            for (auto &rebel : rebels)
                if (!helper::isShipDead(rebel) && !helper::isShipDead(imperial))
                    rebel->receiveAttack(imperial.get());

        rebels.erase(
            std::remove_if(rebels.begin(), rebels.end(), helper::isShipDead),
            rebels.end());
        imperials.erase(std::remove_if(imperials.begin(), imperials.end(),
                                       helper::isShipDead),
                        imperials.end());
    }

    currentTime = clock->stepTime(currentTime, maxTime, timeStep);
}

SpaceBattle::Builder &
SpaceBattle::Builder::ship(std::shared_ptr<RebelStarship> const &ship) {
    build_rebels.push_back(ship);
    return *this;
}

SpaceBattle::Builder &
SpaceBattle::Builder::ship(std::shared_ptr<ImperialStarship> const &ship) {
    build_imperials.push_back(ship);
    return *this;
}

SpaceBattle::Builder &SpaceBattle::Builder::startTime(Time t0_) {
    build_startTime = t0_;
    return *this;
}

SpaceBattle::Builder &SpaceBattle::Builder::maxTime(Time t1_) {
    build_maxTime = t1_;
    return *this;
}

SpaceBattle::Builder &
SpaceBattle::Builder::strategy(std::shared_ptr<BaseStrategy> strategy) {
    build_attackStrategy = strategy;
    return *this;
}

SpaceBattle::Builder &
SpaceBattle::Builder::clock(std::shared_ptr<BaseClock> clock) {
    build_clock = clock;
    return *this;
}

SpaceBattle SpaceBattle::Builder::build() {
    // We allow users to change clock and strategy, but if they don't we fall
    // back to some defaults:
    if (build_clock == nullptr)
        build_clock = std::make_shared<ModuloClock>();
    if (build_attackStrategy == nullptr)
        build_attackStrategy = std::make_shared<StrategyDivisible2Or3ButNot5>();

    return SpaceBattle(std::move(build_rebels), std::move(build_imperials),
                       std::move(build_attackStrategy), std::move(build_clock),
                       build_startTime, build_maxTime);
}
