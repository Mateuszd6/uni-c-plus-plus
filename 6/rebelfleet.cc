#include "rebelfleet.h"

RebelStarship::RebelStarship(ShieldPoints shield_, Speed speed_)
    : SingleStarship{shield_}, speed{speed_} {
}

Speed RebelStarship::getSpeed() const {
    return speed;
}

void RebelStarship::receiveAttack(ImperialStarship *imperialStarship) {
    takeDamage(imperialStarship->getAttackPower());
}

RebelDefender::RebelDefender(ShieldPoints shieldPoints_, Speed speed_,
                             AttackPower attackPower_)
    : RebelStarship{shieldPoints_, speed_}, SingleFighter{attackPower_} {
}

void RebelDefender::receiveAttack(ImperialStarship *imperialStarship) {
    RebelStarship::receiveAttack(imperialStarship);
    imperialStarship->takeDamage(getAttackPower());
}

Explorer::Explorer(ShieldPoints shieldPoints_, Speed speed_)
    : RebelStarship{shieldPoints_, speed_} {
    helper::assertShipIsFast(this);
}

StarCruiser::StarCruiser(ShieldPoints shieldPoints_, Speed speed_,
                         AttackPower attackPower_)
    : RebelDefender{shieldPoints_, speed_, attackPower_} {
    helper::assertShipIsSlow(this);
}

XWing::XWing(ShieldPoints shieldPoints_, Speed speed_, AttackPower attackPower_)
    : RebelDefender{shieldPoints_, speed_, attackPower_} {
    helper::assertShipIsFast(this);
}

std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed) {
    return std::make_shared<Explorer>(shield, speed);
}

std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed,
                                               AttackPower power) {
    return std::make_shared<StarCruiser>(shield, speed, power);
}

std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed,
                                   AttackPower power) {
    return std::make_shared<XWing>(shield, speed, power);
}
