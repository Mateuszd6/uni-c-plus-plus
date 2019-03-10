#ifndef BATTLE_H
#define BATTLE_H

#include "helper.h"
#include "imperialfleet.h"
#include "rebelfleet.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

struct SpaceBattle {
  private:
    std::vector<std::shared_ptr<RebelStarship>> rebels;
    std::vector<std::shared_ptr<ImperialStarship>> imperials;
    std::shared_ptr<const BaseStrategy> attackStrategy;
    std::shared_ptr<const BaseClock> clock;
    Time currentTime;
    const Time maxTime;

    // Large ctor called internally by the builder class, which moves-in all
    // constructed vectors and parameters.
    SpaceBattle(std::vector<std::shared_ptr<RebelStarship>> &&rebels_,
                std::vector<std::shared_ptr<ImperialStarship>> &&imperials_,
                std::shared_ptr<BaseStrategy> &&attackStrategy_,
                std::shared_ptr<BaseClock> &&clock_, Time currentTime_,
                Time maxTime_);

  public:
    struct Builder;

    // Returns the number of imperal ships that are still alive.
    size_t countImperialFleet() const;

    // Returns the number of rebel ships that are still alive.
    size_t countRebelFleet() const;

    // Checks if it is possible to attack in current time (using Strategy class)
    // and if it is possible perfroms the attack simulation. Then the time is
    // changed (using timeStep value and Clock class).
    void tick(Time timeStep);
};

// Builder class used to create a battle simulation.
struct SpaceBattle::Builder {
  private:
    std::shared_ptr<BaseStrategy> build_attackStrategy;
    std::shared_ptr<BaseClock> build_clock;
    std::vector<std::shared_ptr<RebelStarship>> build_rebels;
    std::vector<std::shared_ptr<ImperialStarship>> build_imperials;
    Time build_startTime;
    Time build_maxTime;

  public:
    // Adds a rebel ship to the battle simulation.
    Builder &ship(std::shared_ptr<RebelStarship> const &rebelShip);

    // Adds an imperial ship to the battle simulation.
    Builder &ship(std::shared_ptr<ImperialStarship> const &imperialShip);

    // Sets the start time of the battle.
    Builder &startTime(Time t0);

    // Sets the max time of the battle. The default clock is not alowed to
    // advance time more than this value.
    Builder &maxTime(Time t1);

    // Set the strategy for the attack rules in the battle. Refer to the
    // Strategy class for more details.
    Builder &strategy(std::shared_ptr<BaseStrategy> strategy);

    // Set the clock for the battle. Refer to the clock class for more details.
    Builder &clock(std::shared_ptr<BaseClock> clock);

    // Create a space battle object based on the data previously set in the
    // builder. The state of the builder after this function is undefined.
    SpaceBattle build();
};

#endif // BATTLE_H
