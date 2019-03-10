#include "../wallet.h"

int main()
{
    Wallet w1(1), w2(2);
    Wallet suma1 = w1 - Wallet(1); // błąd kompilacji
}
