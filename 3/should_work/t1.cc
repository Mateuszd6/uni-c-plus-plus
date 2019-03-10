#include "../wallet.h"

int main()
{
    {
        Wallet w1(1), w2(2);
        Wallet suma1(0);
        Wallet suma2 = Wallet(2) + w2; // OK, w w2 jest 0 B po operacji

        Wallet suma4 = Wallet(1) + Wallet(2);  // OK, suma4 ma dwa wpisy
        // w historii i 3 B
    }


    {
        Wallet w1, w2;
        bool b;
        float f;

        w1 += Wallet(3);
        w1 *= 3;
        b = 2 < w2;
        Wallet suma2 = 2 + w2;
    }
}
