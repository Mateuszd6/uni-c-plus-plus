#include <cassert>
#include <iostream>
#include <sstream>

#include "battle.h"
#include "imperialfleet.h"
#include "rebelfleet.h"

// Asserts that the given EXPRESSION prints TXT to the STREAM.
#define EXPECT_PRINTING(EXPRESSION, STREAM, TXT)                               \
    do {                                                                       \
        (STREAM) << std::flush;                                                \
        std::streambuf *stream_psbuf_;                                         \
        std::streambuf *stream_backup_;                                        \
        std::stringstream result_strstr_;                                      \
        stream_backup_ = (STREAM).rdbuf();                                     \
        stream_psbuf_ = result_strstr_.rdbuf();                                \
        (STREAM).rdbuf(stream_psbuf_);                                         \
        EXPRESSION;                                                            \
        (STREAM) << std::flush;                                                \
        (STREAM).rdbuf(stream_backup_);                                        \
        bool missmatch_ = false;                                               \
        char const *p_ = &(TXT[0]);                                            \
        while (stream_psbuf_->sgetc() != EOF) {                                \
            if (!*p_) {                                                        \
                missmatch_ = true;                                             \
                break;                                                         \
            }                                                                  \
            char ch = static_cast<char>(stream_psbuf_->sbumpc());              \
            if (ch != *p_++) {                                                 \
                missmatch_ = true;                                             \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        if (*p_)                                                               \
            missmatch_ = true;                                                 \
        if (missmatch_)                                                        \
            assert(false and ("Expected print does not match!"));              \
    } while (0)

static void test1() {
    // Corner case test: destroyer should not attack the explorer.
    auto xwing = createXWing(1000, 300000, 100);
    auto explorer = createExplorer(1, 400000);
    auto destroyer = createImperialDestroyer(1, 20);

    auto battle = SpaceBattle::Builder()
                      .ship(xwing)
                      .ship(destroyer)
                      .ship(explorer)
                      .startTime(2)
                      .maxTime(1000)
                      .build();

    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(3);

    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 0);
}

static void test2() // github:fifek
{
    auto attack = [](std::shared_ptr<ImperialStarship> imperial,
                     std::shared_ptr<RebelStarship> rebel) {
        rebel->receiveAttack(imperial.get());
    };

    {
        auto xwing = createXWing(100, 300000, 50);
        auto explorer = createExplorer(150, 400000);
        auto cruiser = createStarCruiser(1234, 100000, 11);
        auto deathStar = createDeathStar(10000, 75);
        auto fighter = createTIEFighter(50, 9);
        auto destroyer = createImperialDestroyer(150, 20);
        auto squadron = createSquadron({deathStar, fighter});

        auto battle = SpaceBattle::Builder()
                          .startTime(2)
                          .maxTime(23)
                          .ship(xwing)
                          .ship(squadron)
                          .ship(explorer)
                          .build();

        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 2);

        battle.tick(3);
        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 1);

        battle.tick(1);
        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 1);

        battle.tick(4);
        assert(battle.countRebelFleet() == 0);
        assert(battle.countImperialFleet() == 1);

        battle.tick(1);
    }

    {
        auto xwing = createXWing(100, 300000, 50);
        auto explorer = createExplorer(150, 400000);
        auto cruiser = createStarCruiser(1234, 100000, 11);
        auto deathStar = createDeathStar(10000, 75);
        auto fighter = createTIEFighter(50, 9);
        auto destroyer = createImperialDestroyer(150, 20);
        auto battle = SpaceBattle::Builder()
                          .startTime(1)
                          .maxTime(23)
                          .ship(deathStar)
                          .ship(explorer)
                          .ship(fighter)
                          .ship(xwing)
                          .build();

        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 2);

        battle.tick(2);
        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 2);

        battle.tick(1);
        assert(battle.countRebelFleet() == 2);
        assert(battle.countImperialFleet() == 1);

        battle.tick(4);
        assert(battle.countRebelFleet() == 0);
        assert(battle.countImperialFleet() == 1);

        battle.tick(1);
    }

    {
        auto xwing = createXWing(100, 300000, 50);
        auto explorer = createExplorer(150, 400000);

        auto battle = SpaceBattle::Builder()
                          .startTime(1)
                          .maxTime(23)
                          .ship(explorer)
                          .ship(xwing)
                          .build();

        assert(battle.countImperialFleet() == 0);
        assert(battle.countRebelFleet() == 2);

        battle.tick(1);

        assert(battle.countImperialFleet() == 0);
        assert(battle.countRebelFleet() == 2);

        battle.tick(2);

        assert(battle.countImperialFleet() == 0);
        assert(battle.countRebelFleet() == 2);

        battle.tick(1000);

        assert(battle.countImperialFleet() == 0);
        assert(battle.countRebelFleet() == 2);
    }

    {
        auto deathStar = createDeathStar(10000, 75);
        auto fighter = createTIEFighter(50, 9);

        auto battle = SpaceBattle::Builder()
                          .startTime(1)
                          .maxTime(23)
                          .ship(deathStar)
                          .ship(fighter)
                          .build();

        assert(battle.countImperialFleet() == 2);
        assert(battle.countRebelFleet() == 0);

        battle.tick(1);

        assert(battle.countImperialFleet() == 2);
        assert(battle.countRebelFleet() == 0);

        battle.tick(2);

        assert(battle.countImperialFleet() == 2);
        assert(battle.countRebelFleet() == 0);

        battle.tick(1000);

        assert(battle.countImperialFleet() == 2);
        assert(battle.countRebelFleet() == 0);
    }

    {
        auto explorer = createExplorer(100, 400000);
        auto deathStar = createDeathStar(10000, 10);
        auto fighter = createTIEFighter(50, 10);
        auto destroyer = createImperialDestroyer(150, 10);
        auto squadron = createSquadron({deathStar, fighter});
        auto squadron2 =
            createSquadron(std::vector<std::shared_ptr<ImperialStarship>>{
                destroyer, squadron});

        auto battle = SpaceBattle::Builder()
                          .startTime(0)
                          .maxTime(20)
                          .ship(squadron2)
                          .ship(explorer)
                          .build();

        battle.tick(2);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 1);

        battle.tick(2);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 1);

        battle.tick(6);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 1);

        battle.tick(2);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 1);

        battle.tick(2);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 1);

        battle.tick(2);
        assert(battle.countImperialFleet() == 3);
        assert(battle.countRebelFleet() == 0);
    }

    {
        auto xwing = createXWing(42, 300000, 2000);
        auto ds = createDeathStar(42, 2000);
        attack(ds, xwing);

        assert(ds->getShield() == 0);
        assert(xwing->getShield() == 0);
    }

    {
        auto xw = createXWing(100, 299796, 1);
        auto sc = createStarCruiser(101, 99999, 2);
        auto ex = createExplorer(102, 299796);
        auto ds = createDeathStar(103, 3);
        auto id = createImperialDestroyer(104, 4);
        auto tf = createTIEFighter(105, 5);

        attack(ds, xw);
        attack(ds, sc);
        attack(ds, ex);
        attack(id, xw);
        attack(id, sc);
        attack(id, ex);
        attack(tf, xw);
        attack(tf, sc);
        attack(tf, ex);

        assert(xw->getShield() == 88);
        assert(sc->getShield() == 89);
        assert(ex->getShield() == 90);
        assert(ds->getShield() == 100);
        assert(id->getShield() == 101);
        assert(tf->getShield() == 102);
    }

    {
        auto exp = createExplorer(42, 299796);
        auto ds = createDeathStar(42, 2000);

        attack(ds, exp);

        assert(ds->getShield() == 42);
        assert(exp->getShield() == 0);
    }

    {
        auto xwing = createXWing(300, 300000, 100);
        auto destroyer = createImperialDestroyer(300, 100);

        auto battle = SpaceBattle::Builder()
                          .startTime(0)
                          .maxTime(100)
                          .ship(xwing)
                          .ship(destroyer)
                          .build();

        assert(battle.countImperialFleet() == 1);
        assert(battle.countRebelFleet() == 1);
        battle.tick(2);
        battle.tick(4);
        battle.tick(2);
        assert(battle.countImperialFleet() == 1);
        assert(battle.countRebelFleet() == 1);
        battle.tick(1);

        {
            for (int i = 0; i < 5; ++i) {
                assert(battle.countImperialFleet() == 0);
                assert(battle.countRebelFleet() == 0);
                battle.tick(1);
            }
        }
    }
}

static void test3() // 4um
{
    auto r = createXWing(10, 300000, 10);
    auto i1 = createTIEFighter(10, 10);
    auto i2 = createTIEFighter(10, 10);
    auto s = createSquadron({i1, i2});

    auto battle = SpaceBattle::Builder()
                      .ship(r)
                      .ship(s)
                      .startTime(2)
                      .maxTime(1337)
                      .build();

    battle.tick(3); // time == 2, zachodzi atak

    EXPECT_PRINTING((battle.tick(2)), std::cout, "DRAW\n");
}

// martin_testy
static void test4() {
    auto xwing = createXWing(500, 300000, 50);
    auto fighter1 = createTIEFighter(50, 9);
    auto fighter2 = createTIEFighter(50, 9);
    auto fighter3 = createTIEFighter(50, 9);
    auto fighter4 = createTIEFighter(50, 9);
    auto fighter5 = createTIEFighter(50, 9);
    auto squadron =
        createSquadron({fighter1, fighter2, fighter3, fighter4, fighter5});
    auto fighter6 = createTIEFighter(50, 9);
    auto squadron_squared = createSquadron({squadron, fighter6});

    auto battle = SpaceBattle::Builder()
                      .ship(squadron_squared)
                      .startTime(24)
                      .maxTime(30)
                      .ship(xwing)
                      .build();

    assert(battle.countImperialFleet() == 6);
    assert(battle.countRebelFleet() == 1);

    battle.tick(1);

    assert(battle.countImperialFleet() == 0);
    assert(battle.countRebelFleet() == 1);
    assert(xwing->getShield() == 500 - 9 * 6);
}

static void test5() {
    auto xwing = createXWing(200, 300000, 50);
    auto fighter1 = createTIEFighter(50, 9);
    auto fighter2 = createTIEFighter(50, 9);
    auto fighter3 = createTIEFighter(50, 9);
    auto fighter4 = createTIEFighter(50, 9);
    auto fighter5 = createTIEFighter(51, 9);
    auto squadron =
        createSquadron({fighter1, fighter2, fighter3, fighter4, fighter5});
    auto fighter6 = createTIEFighter(50, 9);
    auto squadron_squared = createSquadron({squadron, fighter6});

    auto battle = SpaceBattle::Builder()
                      .ship(squadron_squared)
                      .startTime(24)
                      .maxTime(30)
                      .ship(xwing)
                      .build();

    assert(battle.countImperialFleet() == 6);
    assert(battle.countRebelFleet() == 1);

    battle.tick(1);

    assert(battle.countImperialFleet() == 1);
    assert(battle.countRebelFleet() == 1);
    assert(xwing->getShield() == 200 - 9 * 6);

    battle.tick(2);

    assert(battle.countImperialFleet() == 1);
    assert(battle.countRebelFleet() == 1);
    assert(xwing->getShield() == 200 - 9 * 6);

    battle.tick(7);

    assert(battle.countImperialFleet() == 0);
    assert(battle.countRebelFleet() == 1);
    assert(xwing->getShield() == 200 - 9 * 6 - 9);
}

static void test6() {
    auto xwing = createXWing(200, 300000, 20);
    auto explorer = createExplorer(50, 330111);
    auto star_cruiser = createStarCruiser(250, 120979, 25);
    auto fighter = createTIEFighter(50, 10);
    auto fighter2 = createTIEFighter(55, 10);
    auto destroyer = createImperialDestroyer(60, 15);
    auto destroyer2 = createImperialDestroyer(65, 15);
    auto death_star = createDeathStar(100, 30);
    auto squadron = createSquadron({fighter2, destroyer2});

    auto battle = SpaceBattle::Builder()
                      .ship(xwing)
                      .ship(explorer)
                      .ship(star_cruiser)
                      .ship(fighter)
                      .ship(destroyer)
                      .ship(death_star)
                      .ship(squadron)
                      .startTime(2)
                      .maxTime(4)
                      .build();

    assert(battle.countImperialFleet() == 5);
    assert(battle.countRebelFleet() == 3);

    battle.tick(1);

    assert(battle.countImperialFleet() == 5);
    assert(battle.countRebelFleet() == 2);
    assert(xwing->getShield() == 120);
    assert(explorer->getShield() == 0);
    assert(star_cruiser->getShield() == 170);
    assert(fighter->getShield() == 5);
    assert(fighter2->getShield() == 10);
    assert(destroyer->getShield() == 15);
    assert(destroyer2->getShield() == 20);
    assert(death_star->getShield() == 55);
    assert(squadron->getShield() == 30);

    battle.tick(2);

    assert(battle.countImperialFleet() == 1);
    assert(battle.countRebelFleet() == 2);
    assert(xwing->getShield() == 40);
    assert(explorer->getShield() == 0);
    assert(star_cruiser->getShield() == 140);
    assert(fighter->getShield() == 0);
    assert(fighter2->getShield() == 0);
    assert(destroyer->getShield() == 0);
    assert(destroyer2->getShield() == 0);
    assert(death_star->getShield() == 10);
    assert(squadron->getShield() == 0);

    battle.tick(2);

    assert(battle.countImperialFleet() == 1);
    assert(battle.countRebelFleet() == 2);
    assert(xwing->getShield() == 40);
    assert(explorer->getShield() == 0);
    assert(star_cruiser->getShield() == 140);
    assert(fighter->getShield() == 0);
    assert(fighter2->getShield() == 0);
    assert(destroyer->getShield() == 0);
    assert(destroyer2->getShield() == 0);
    assert(death_star->getShield() == 10);
    assert(squadron->getShield() == 0);

    battle.tick(1);

    assert(battle.countImperialFleet() == 0);
    assert(battle.countRebelFleet() == 2);
    assert(xwing->getShield() == 10);
    assert(explorer->getShield() == 0);
    assert(star_cruiser->getShield() == 140);
    assert(fighter->getShield() == 0);
    assert(fighter2->getShield() == 0);
    assert(destroyer->getShield() == 0);
    assert(destroyer2->getShield() == 0);
    assert(death_star->getShield() == 0);
    assert(squadron->getShield() == 0);
}

static void test7() {
    /// dane
    auto xwing = createXWing(100, 300000, 50);
    auto explorer = createExplorer(150, 400000);
    auto cruiser = createStarCruiser(1234, 100000, 11);
    auto deathStar = createDeathStar(10000, 75);
    auto fighter = createTIEFighter(50, 9);
    auto destroyer = createImperialDestroyer(150, 20);
    auto squadron = createSquadron({deathStar, fighter});

    /// Test geterów bitwy - OK
    auto battle = SpaceBattle::Builder()
                      .ship(squadron)
                      .ship(destroyer)
                      .startTime(2)
                      .maxTime(23)
                      .ship(xwing)
                      .ship(explorer)
                      .ship(cruiser)
                      .build();
    assert(battle.countImperialFleet() == 3);
    assert(battle.countRebelFleet() == 3);
    //// Nie ma już dostępnego SpaceTime
    //    SpaceBattle::SpaceTime spaceTime(2, 23);
    //    assert(spaceTime.isNowAttackTime());
    //    spaceTime.moveTime(1); // t0 = 3
    //    assert(spaceTime.isNowAttackTime());
    //    spaceTime.moveTime(2); // t0 = 5
    //    assert(!spaceTime.isNowAttackTime());
    //    spaceTime.moveTime(18); // t0 = 0
    //    assert(!spaceTime.isNowAttackTime());
    //    spaceTime.moveTime(2); // t0 = 2
    //    assert(!spaceTime.isNowAttackTime());

    auto xwing1 = createXWing(100, 300000, 50);
    auto explorer1 = createExplorer(1000, 400000);
    auto cruiser1 = createStarCruiser(1234, 100000, 11);
    auto deathStar1 = createDeathStar(10000, 75);
    auto fighter1 = createTIEFighter(50, 9);
    auto destroyer1 = createImperialDestroyer(150, 20);
    auto squadron1 =
        createSquadron({deathStar1, fighter1}); /// SP = 10050 ; AP = 84

    /// Test receiveAttack - OK
    explorer1->receiveAttack(destroyer1.get());
    assert(explorer1->getShield() == 1000 - 20);
    assert(destroyer1->getShield() == 150);
    explorer1->receiveAttack(squadron1.get());
    assert(explorer1->getShield() == 980 - 84);
    assert(squadron1->getShield() == 10050);
    xwing1->receiveAttack(destroyer1.get());
    assert(xwing1->getShield() == 80);
    assert(destroyer1->getShield() == 100);
    cruiser1->receiveAttack(squadron1.get());
    assert(cruiser1->getShield() == 1234 - 84);
    assert(squadron1->getShield() == 10050 - 2 * 11);

    ////Test konstruktora z vectora - OK
    std::vector<std::shared_ptr<ImperialStarship>> vec;
    vec.push_back(deathStar);
    vec.push_back(fighter);
    auto squadron3 = createSquadron(vec);
    volatile auto squadron4 = createSquadron(std::move(vec));

    ////Test asercji w konstruktorze - OK
    //    auto sc1 = createStarCruiser(1, 1, 1);
    //    auto sc2 = createStarCruiser(1, 299796, 1);
    //    auto xw1 = createXWing(100, 100, 50);
    //    auto xw2 = createXWing(100, 100000000, 50);
    //    auto ex1 = createExplorer(100, 100);
    //    auto ex2 = createExplorer(100, 100000000);

    ////Test zbyt krotkiego buildera - OK, czy nie powinnismy sprawdzac czy sa
    ///jakiekolwiek statki? (Patrz: linia 79 w treści)
    //    auto battle1 = SpaceBattle::Builder()
    //            .startTime(2)
    //            .build();
    //    auto battle2 = SpaceBattle::Builder()
    //            .maxTime(2)
    //            .build();
    //    auto battle3 = SpaceBattle::Builder()
    //            .build();

    ////Test na zgodnosc geterów - OK
    assert(xwing->getShield() == 100);
    assert(xwing->getSpeed() == 300000);
    assert(xwing->getAttackPower() == 50);
    assert(xwing->getShield() > 0);
    assert(xwing->getQuantity() == 1);
    xwing->takeDamage(50);
    assert(xwing->getShield() == 50);

    assert(explorer->getShield() == 150);
    assert(explorer->getSpeed() == 400000);
    assert(explorer->getShield() > 0);
    assert(explorer->getQuantity() == 1);
    explorer->takeDamage(149);
    assert(explorer->getShield() == 1);

    assert(cruiser->getShield() == 1234);
    assert(cruiser->getSpeed() == 100000);
    assert(cruiser->getAttackPower() == 11);
    assert(cruiser->getShield() > 0);
    assert(cruiser->getQuantity() == 1);
    cruiser->takeDamage(234);
    assert(cruiser->getShield() == 1000);

    assert(deathStar->getShield() == 10000);
    assert(deathStar->getAttackPower() == 75);
    assert(deathStar->getShield() > 0);
    assert(deathStar->getQuantity() == 1);
    deathStar->takeDamage(1);
    assert(deathStar->getShield() == 9999);

    assert(fighter->getShield() == 50);
    assert(fighter->getAttackPower() == 9);
    assert(fighter->getShield() > 0);
    assert(fighter->getQuantity() == 1);
    fighter->takeDamage(25);
    assert(fighter->getShield() == 25);

    assert(destroyer->getShield() == 150);
    assert(destroyer->getAttackPower() == 20);
    assert(destroyer->getShield() > 0);
    assert(destroyer->getQuantity() == 1);
    destroyer->takeDamage(151);
    assert(destroyer->getShield() == 0);
    assert(!(destroyer->getShield() > 0));
    assert(destroyer->getQuantity() == 0);

    auto deathStar2 = createDeathStar(10000, 75);
    auto fighter2 = createTIEFighter(50, 9);
    auto squadron2 = createSquadron({deathStar2, fighter2});

    assert(squadron2->getShield() ==
           deathStar2->getShield() + fighter2->getShield());
    assert(squadron2->getShield() == 10050);
    assert(squadron2->getAttackPower() ==
           deathStar2->getAttackPower() + fighter2->getAttackPower());
    assert(squadron2->getAttackPower() == 84);
    assert(squadron2->getShield() > 0);
    assert(squadron2->getQuantity() == 2);
    squadron2->takeDamage(100); // fighter umrze
    assert(squadron2->getShield() ==
           deathStar2->getShield() + fighter2->getShield());
    assert(squadron2->getShield() == 9900);
    assert(squadron2->getShield() > 0);
    assert(squadron2->getQuantity() == 1);
    assert(squadron1->getAttackPower() ==
           deathStar1->getAttackPower() + fighter1->getAttackPower());
    assert(squadron2->getAttackPower() == deathStar2->getAttackPower());
    squadron2->takeDamage(10000); // deathstar umrze
    assert(squadron2->getShield() ==
           deathStar2->getShield() + fighter2->getShield());
    assert(squadron2->getShield() == 0);
    assert(squadron1->getAttackPower() ==
           deathStar1->getAttackPower() + fighter1->getAttackPower());
    assert(squadron2->getAttackPower() == 0);
    assert(!(squadron2->getShield() > 0));
    assert(squadron2->getQuantity() == 0);
}

static void test8() {
    auto xwing = createXWing(100, 300000, 50);
    auto explorer = createExplorer(150, 400000);
    auto cruiser = createStarCruiser(1234, 100000, 11);
    auto deathStar = createDeathStar(10000, 75);
    auto fighter = createTIEFighter(50, 9);
    auto destroyer = createImperialDestroyer(150, 20);
    auto squadron = createSquadron({deathStar, fighter});

    auto battle = SpaceBattle::Builder()
                      .ship(squadron)
                      .startTime(2)
                      .maxTime(23)
                      .ship(xwing)
                      .ship(explorer)
                      .build();

    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 2);

    battle.tick(3);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(1);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(4);
    assert(battle.countRebelFleet() == 0);
    assert(battle.countImperialFleet() == 1);

    EXPECT_PRINTING((battle.tick(1)), std::cout, "IMPERIUM WON\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto exWing = createXWing(1, 299800, 1);
    auto tieFighter = createTIEFighter(10, 1);
    auto aVeryBoringBattle = SpaceBattle::Builder()
                                 .ship(exWing)
                                 .ship(tieFighter)
                                 .startTime(0)
                                 .maxTime(10)
                                 .build();

    assert(aVeryBoringBattle.countRebelFleet() == 1);
    assert(aVeryBoringBattle.countImperialFleet() == 1);

    // time: 0 - no battle
    aVeryBoringBattle.tick(10);

    assert(aVeryBoringBattle.countRebelFleet() == 1);
    assert(aVeryBoringBattle.countImperialFleet() == 1);

    // time: 10 - no battle
    aVeryBoringBattle.tick(3);

    assert(aVeryBoringBattle.countRebelFleet() == 1);
    assert(aVeryBoringBattle.countImperialFleet() == 1);

    // time: 2 - battle
    aVeryBoringBattle.tick(1);

    assert(aVeryBoringBattle.countRebelFleet() == 0);
    assert(aVeryBoringBattle.countImperialFleet() == 1);

    EXPECT_PRINTING((aVeryBoringBattle.tick(1)), std::cout, "IMPERIUM WON\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto xwin = createXWing(100, 300000, 50);
    auto explore = createExplorer(150, 400000);
    auto deathSta = createDeathStar(10000, 75);
    auto fighte = createTIEFighter(50, 9);
    auto destroye = createImperialDestroyer(150, 20);
    auto squadro = createSquadron({deathSta, fighte});
    auto destroyeS = createSquadron({destroye});
    auto squadroSquadro = createSquadron({squadro});
    auto sqadroSquadroSquadro = createSquadron({squadroSquadro});

    auto battl = SpaceBattle::Builder()
                     .ship(sqadroSquadroSquadro)
                     .startTime(2)
                     .maxTime(23)
                     .ship(xwin)
                     .ship(explore)
                     .build();

    assert(battl.countRebelFleet() == 2);
    assert(battl.countImperialFleet() == 2);

    battl.tick(3);
    assert(battl.countRebelFleet() == 2);
    assert(battl.countImperialFleet() == 1);

    battl.tick(1);
    assert(battl.countRebelFleet() == 2);
    assert(battl.countImperialFleet() == 1);

    battl.tick(4);
    assert(battl.countRebelFleet() == 0);
    assert(battl.countImperialFleet() == 1);

    EXPECT_PRINTING((battl.tick(1)), std::cout, "IMPERIUM WON\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class OddStrategy : public BaseStrategy {
      public:
        OddStrategy() {
        }
        OddStrategy(OddStrategy const bitand) {
        }

        bool canAttack(Time t) const override {
            return t % 2 == 1;
        }
    };

    class WeirdClock : public BaseClock {
      public:
        WeirdClock() {
        }

        Time stepTime(Time currentTime, Time maxTime,
                      Time timeStep) const override {
            if (static_cast<int>(timeStep) < 0)
                timeStep =
                    static_cast<Time>(abs(static_cast<int>(timeStep)) + 1);

            auto retval = currentTime + timeStep;
            retval %= maxTime + 1;
            return retval;
        }
    };

    auto oddStrategy = std::make_shared<OddStrategy>();
    auto reverseClock = std::make_shared<WeirdClock>();
    auto yLeg = createXWing(1, 299800, 1);
    auto bowtieBrawler = createTIEFighter(1, 1);
    auto oddBattle = SpaceBattle::Builder()
                         .ship(yLeg)
                         .ship(bowtieBrawler)
                         .strategy(oddStrategy)
                         .clock(reverseClock)
                         .startTime(0)
                         .maxTime(10)
                         .build();

    assert(oddBattle.countImperialFleet() == 1);
    assert(oddBattle.countRebelFleet() == 1);

    // time: 0 - no battle
    oddBattle.tick(2);

    assert(oddBattle.countImperialFleet() == 1);
    assert(oddBattle.countRebelFleet() == 1);

    // time: 2 - no battle
    oddBattle.tick(static_cast<Time>(-2));

    assert(oddBattle.countImperialFleet() == 1);
    assert(oddBattle.countRebelFleet() == 1);

    // time: 5 - battle
    oddBattle.tick(2);

    assert(oddBattle.countImperialFleet() == 0);
    assert(oddBattle.countRebelFleet() == 0);

    EXPECT_PRINTING((oddBattle.tick(0)), std::cout, "DRAW\n");
}

static void test9() {
    auto xwing = createXWing(100, 300000, 50);
    auto explorer = createExplorer(150, 400000);
    auto cruiser = createStarCruiser(1234, 100000, 11);
    auto deathStar = createDeathStar(10000, 75);
    auto fighter = createTIEFighter(50, 9);
    auto destroyer = createImperialDestroyer(150, 20);

#define ENABLE_SQUADRON
#ifdef ENABLE_SQUADRON
    auto squadron = createSquadron({deathStar, fighter});
#endif

    auto battle = SpaceBattle::Builder()
#ifdef ENABLE_SQUADRON
                      .ship(squadron)
#else
                      .ship(deathStar)
                      .ship(fighter)
#endif
                      .startTime(2)
                      .maxTime(23)
                      .ship(xwing)
                      .ship(explorer)
                      .build();

    // NOTE(M): More asserts, but i kept the original test there just in case.
#if 1
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 2);

    assert(explorer->getShield() == 150);
    assert(explorer->getSpeed() == 400000);
    //    assert(explorer.getAttackPower() == 0); // does not compile

    assert(cruiser->getShield() == 1234);
    assert(cruiser->getSpeed() == 100000);
    assert(cruiser.get()->getAttackPower() == 11);

    assert(xwing->getShield() == 100);
    assert(xwing->getSpeed() == 300000);
    assert(xwing->getAttackPower() == 50);

    assert(deathStar->getShield() == 10000);
    assert(deathStar->getAttackPower() == 75);

    assert(destroyer->getShield() == 150);
    assert(destroyer->getAttackPower() == 20);

    assert(fighter->getShield() == 50);
    assert(fighter->getAttackPower() == 9);

#ifdef ENABLE_SQUADRON
    assert(squadron->getShield() == 10050);
    assert(squadron->getAttackPower() == 84);
#endif

    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 2);

    battle.tick(3);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(1);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(4);
    assert(battle.countRebelFleet() == 0);
    assert(battle.countImperialFleet() == 1);
#else
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 2);

    battle.tick(3);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(1);
    assert(battle.countRebelFleet() == 2);
    assert(battle.countImperialFleet() == 1);

    battle.tick(4);
    assert(battle.countRebelFleet() == 0);
    assert(battle.countImperialFleet() == 1);
#endif

    EXPECT_PRINTING((battle.tick(1)), std::cout, "IMPERIUM WON\n");
    EXPECT_PRINTING((battle.tick(1)), std::cout, "IMPERIUM WON\n");
    EXPECT_PRINTING((battle.tick(5)), std::cout, "IMPERIUM WON\n");
    EXPECT_PRINTING((battle.tick(2)), std::cout, "IMPERIUM WON\n");
    EXPECT_PRINTING((battle.tick(3)), std::cout, "IMPERIUM WON\n");

#if 1
    // This can be check if memory is managed correctly, and if proper dctors
    // are called.
    {
        auto anyShip = createXWing(100, 300000, 20);
        auto anyShip1 = createXWing(101, 300045, 30);
        auto anyShip2 = createXWing(104, 400000, 40);
    }

    {
        auto battle_ =
            SpaceBattle::Builder().ship(createXWing(104, 400000, 40)).build();
    }
#endif
}

int main() {
    // Commented tests mean bugs:
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
}
