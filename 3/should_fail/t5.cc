#include "../wallet.h"

int main()
{
    Wallet w1(1), w2(2);
    Wallet suma1(3);
    Wallet suma2 = Wallet(2) - w2; // OK, w w2 jest 0 B po operacji
                                   // i jeden nowy wpis w historii,
                                   // a w suma2 jest w2.getUnits() - 2 B.
                                   // Historia operacji powstałego obiektu
                                   // zależy od implementacji.
    Wallet suma3 = suma1 - suma2;  // błąd kompilacji
}
