#ifndef HELPER_H
#define HELPER_H

#include <cstddef>
#include <memory>

// TODO(M): I really think they should be ints now.
using ShieldPoints = uint64_t;
using Speed = uint64_t;
using Time = uint64_t;
using AttackPower = uint64_t;

// Strategy abstraction. Used to determine if ships can attach in current time.
struct BaseStrategy {
  public:
    virtual ~BaseStrategy() = default;

    // Returns true if ships can attack in current time.
    virtual bool canAttack(Time t) const = 0;
};

// The strategy specified in the problem statement.
struct StrategyDivisible2Or3ButNot5 : public BaseStrategy {
    // Returns true if time is divisable by 2 and 3 but not by 5.
    bool canAttack(Time t) const override;
};

// Clock abstraction. Used to advance time in the space battle.
struct BaseClock {
  public:
    virtual ~BaseClock() = default;

    // Advance time by timeStep. This _should_ be equivalent to calling this
    // function timeStep times with value 1.
    virtual Time stepTime(Time currentTime, Time maxTime,
                          Time timeStep) const = 0;
};

// Clock class specified in the problem statement.
struct ModuloClock : public BaseClock {
  public:
    // Increments time and wraps after exceeding maxTime.
    Time stepTime(Time currentTime, Time maxTime, Time timeStep) const override;
};

// Starship abstraction. Provides shield related fucntions (getShield,
// takeDamage) and getQuantity used in the space battle to count alive ships.
struct BaseStarship {
  protected:
    BaseStarship() = default;

  public:
    virtual ~BaseStarship() = default;

    // Returns the current value for the shield.
    virtual ShieldPoints getShield() const = 0;

    // Removes shieldPoints by damage. Shield points are always >= 0.
    virtual void takeDamage(AttackPower damage) = 0;

    // Returns the quantity, which is how many single starships contains this
    // object.
    virtual size_t getQuantity() const = 0;
};

// The implementation of BaseStarship interface for trivial ships with qunatity
// equal to 1 and simply calculated shield points.
struct SingleStarship : public virtual BaseStarship {
  private:
    ShieldPoints shieldPoints;

  protected:
    SingleStarship(ShieldPoints shield);

  public:
    ShieldPoints getShield() const override;
    void takeDamage(AttackPower damage) override;
    size_t getQuantity() const override;
};

// Abstraction of the attacking ship.
struct BaseFighter : public virtual BaseStarship {
  protected:
    BaseFighter() = default;

  public:
    // Returns the current attack power of the ship.
    virtual AttackPower getAttackPower() const = 0;
};

// Implementation of BaseFighter abstraction for trivial ships with constant
// attack power.
struct SingleFighter : public virtual BaseFighter {
  private:
    AttackPower const attackPower;

  protected:
    SingleFighter(AttackPower attack);

  public:
    AttackPower getAttackPower() const override;
};

// Forward declaration for helper functions
struct RebelStarship;

namespace helper {
// Asserts correct speed bounds for fast ships
void assertShipIsFast(const RebelStarship *ship);

// Asserts correct speed bounds for slow ships
void assertShipIsSlow(const RebelStarship *ship);

// This is a predicate to pass to a function, so it can't be a method
// Returns true if ship or all ships in squadron have no shields left
bool isShipDead(const std::shared_ptr<BaseStarship> &ship);
}; // namespace helper

#endif // HELPER_H
