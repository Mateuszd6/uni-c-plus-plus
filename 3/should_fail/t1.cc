#include "../wallet.h"

int main()
{
    Wallet w1, w2;
    w1 = Wallet(1); // OK
    w1 = w2; // błąd kompilacji
}
