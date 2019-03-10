#if 0
#include "wallet.h"

#include <cassert>
#include <iostream>

static const int UNITS_IN_B = 100000000;

using std::move;

int main() {
    {
        Wallet w1(1), w2(2);
        Wallet suma1(0);
        Wallet suma2 = Wallet(2) + w2; // OK, w w2 jest 0 B po operacji

        Wallet suma4 = Wallet(2) + Wallet(1);  // OK, suma4 ma dwa wpisy
        // w historii i 3 B
        std::cout << suma2.opSize() << " " << suma2.getUnits() << "\n"
                  << suma4.opSize() << " " << suma4.getUnits() << "\n";
    }


    {
        Wallet w1, w2;
        bool b;

        w1 += Wallet(3);
        w1 *= 3;
        b = 2 < w2;
        Wallet suma2 = 2 + w2;
    }

    {
        Wallet w1 = Wallet(1) * 2;
        assert(w1.getUnits() == 2 * UNITS_IN_B);

        Wallet w2 = 2 * Wallet(1);
        assert(w1.getUnits() == 2 * UNITS_IN_B);
    }

    {
        Wallet w1;
        w1 += Wallet(1) += Wallet(2);
    }

    {
        assert(Empty() == 0);
        assert(Empty() < Wallet(1));
        assert(Empty().getUnits() == 0);
        assert(Empty().opSize() == 1);
        assert(Empty()[0].getUnits() == 0);

        Wallet w1;
        assert(w1 == Empty());
        assert(w1.opSize() == 1);

        Wallet w2(1), w3(1);
        assert(w2.opSize() == 1);
        assert(w2.getUnits() == UNITS_IN_B);

        assert(Wallet::fromBinary("100") == Wallet(4));
        assert(Wallet(" 1.2000 ") == Wallet("1,2"));

        assert(Wallet(2) + w2 == Wallet(3));
        assert(Wallet(1) + Wallet(2) == Wallet(3));
        assert(1 + 2 == Wallet(3));

        Wallet w4;
        assert(w4.opSize() == 1);

        w4 += 2;
        assert(w4.opSize() == 2);
        assert(w4[0] <= w4[1]);
        assert(w4[1].getUnits() == w4.getUnits());

        w4 *= 3;
        assert(w4.opSize() == 3);
        assert(w4[2].getUnits() == 6 * UNITS_IN_B);

        assert(Wallet(move(w4)).opSize() == 4);

        Wallet w5{2};
        Wallet w6 = Wallet(10) - w5;
        assert(w5 == Wallet(4));
        assert(w6 == Wallet(8));

        Wallet w7{ 7};
        w6 -= w7;
        w6 += 7;
    }
}

#else
#include "wallet.h"

#include <cassert>
#include <iostream>

static const int UNITS_IN_B = 100000000;

using std::move;

int main() {

    Wallet topGuard = Wallet(0);

    Wallet q1, q2;
    q1 = Wallet(1); // OK
    //q1 = q2; // błąd kompilacji

    Wallet r1(1), r2(2);
    //r1 + r2; // błąd kompilacji
    //Wallet suma0 = 1 += Wallet(1); // błąd kompilacji
    //Wallet suma1 = r1 + Wallet(1); // błąd kompilacji
    Wallet suma2 = Wallet(2) + r2; // OK
    assert(r2.getUnits() == 0);
    assert(r2.opSize() == 2);
    assert(suma2.getUnits() == 4 * UNITS_IN_B);
    //Wallet suma3 = suma1 + suma2;  // błąd kompilacji
    Wallet suma4 = Wallet(1) + Wallet(2);  // OK
    assert(suma4.opSize() == 2);
    assert(suma4.getUnits() == 3 * UNITS_IN_B);

    Wallet s1, s2;
    bool b;
    float f;
    //Wallet s3(true); // błąd kompilacji
    //Wallet s4('a');  // błąd kompilacji
    //Wallet s5(f); // błąd kompilacji
    //s1 += "10"; // błąd kompilacji
    //s1 = s2 + "10"; // błąd kompilacji
    //b = "10" < s2; // błąd kompilacji

    s1 += Wallet(3);
    s1 *= 3;
    b = 2 < s2;
    Wallet suma5 = 2 + s2;
    Wallet suma6 = Wallet(1) + 2;
    //Wallet suma7 = 2.5 + s2; // błąd kompilacji
    //Wallet suma8 = Wallet(1) - Wallet(2); // exception

#if 0
    Wallet t1(10000000), t2(10000000);
    //Wallet t3(2000000); // exception
    t1.~Wallet();
    t2.~Wallet();
#endif

    Wallet u1(1), u2(2);
    u1 += 1;
    u2 += Wallet(1);
    u1 += 0;
    u2 += 1;
    Wallet u3(std::move(u1), std::move(u2));
    assert(u3.opSize() == 7);
    assert(u3.getUnits() == 6 * UNITS_IN_B);

    Wallet x1(1), x2(2);
    assert(x1 < x2);
    assert(x1 <= x2);
    assert(x1 <= x1);
    assert(x1 != x2);
    assert(x2 >= x1);
    assert(x2 >= x2);
    assert(x2 > x1);

    assert(Wallet(1) < x2);
    assert(Wallet(1) <= x2);
    assert(Wallet(1) <= Wallet(1));
    assert(Wallet(1) != x2);
    assert(x2 >= Wallet(1));
    assert(x2 >= x2);
    assert(x2 > Wallet(1));

    assert(Wallet(1) < Wallet(2));
    assert(Wallet(1) <= Wallet(2));
    assert(Wallet(1) <= Wallet(1));
    assert(Wallet(1) != Wallet(2));
    assert(Wallet(2) >= Wallet(1));
    assert(Wallet(2) >= Wallet(2));
    assert(Wallet(2) > Wallet(1));

    Wallet lolz = 2;
    Wallet omg = 3;
    std::cout << lolz << "\n";
    std::cout << lolz[0] << "\n";

    //Wallet emp = Empty(); // błąd kompilacji
    assert(Empty() == 0);
    assert(Empty() < Wallet(1));
    assert(Empty().getUnits() == 0);
    assert(Empty().opSize() == 1);
    assert(Empty()[0].getUnits() == 0);
    //Empty() += Wallet(1); // błąd kompilacji

    Wallet w1;
    assert(w1 == Empty());
    assert(w1.opSize() == 1);

    Wallet w2(1), w3(1);
    assert(w2.opSize() == 1);
    assert(w2.getUnits() == UNITS_IN_B);
    assert(w2 == 1);
    assert(1 == w2);
    assert(w2 == w3);

    assert(Wallet::fromBinary("100") == Wallet(4));
    std::string str(" 1.2000 ");
    assert(Wallet(" 1.2000 ") == Wallet("1,2"));
    assert(Wallet(str) == Wallet("1,2"));
    // Wallet("1.a"); // exception
    // Wallet("53.5."); // exception
#if 0
    assert(Wallet("000.555") == Wallet(".555"));
#endif

    assert(Wallet(2) + w2 == Wallet(3));
    assert(Wallet(1) + Wallet(2) == Wallet(3));
    assert(1 + 2 == Wallet(3));

    Wallet w4;
    assert(w4.opSize() == 1);

    w4 += 2;
    assert(w4.opSize() == 2);
    assert(w4[0] <= w4[1]);
    assert(w4[1].getUnits() == w4.getUnits());

    w4 *= 3;
    assert(w4.opSize() == 3);
    assert(w4[2].getUnits() == 6 * UNITS_IN_B);

    assert(Wallet(move(w4)).opSize() == 4);

    Wallet w5{2};
    //Wallet w6{true}; // błąd kompilacji
    Wallet w7 = Wallet(10) - w5;
    assert(w5 == Wallet(4));
    assert(w7 == Wallet(8));

#if 1
    Wallet foo{ 5};
    Wallet bar {3 };
    foo = std::move(bar);
#endif

    Wallet guard{ 0 };
}
#endif
