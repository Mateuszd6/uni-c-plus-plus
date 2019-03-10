#ifndef WALLET_H
#define WALLET_H

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

struct WalletOperation
{
  private:
    // size_t is an incorrect type for members units due to possible 32-bit size
    unsigned long long int units;
    std::chrono::system_clock::time_point tp;

  public:
    WalletOperation(unsigned long long int units);

    unsigned long long int getUnits() const;

    friend bool operator>(const WalletOperation &lhs,
                          const WalletOperation &rhs);
    friend bool operator==(const WalletOperation &lhs,
                           const WalletOperation &rhs);
    friend bool operator<=(const WalletOperation &lhs,
                           const WalletOperation &rhs);
    friend bool operator<(const WalletOperation &lhs,
                          const WalletOperation &rhs);
    friend bool operator>=(const WalletOperation &lhs,
                           const WalletOperation &rhs);
    friend bool operator!=(const WalletOperation &lhs,
                           const WalletOperation &rhs);
    friend std::ostream &operator<<(std::ostream &stream,
                                    const WalletOperation &operation);
};

struct Wallet
{
  private:
    unsigned long long int units;
    std::vector<WalletOperation> operations;

    static unsigned long long int allUnits;
    static void
    addToAllUnits(long long int units); // intentionally not unsigned

  public:
    Wallet();
    Wallet(int n);
    explicit Wallet(const char *str);
    explicit Wallet(std::string str);
    template <typename T> Wallet(T t) = delete;

    ~Wallet();

    Wallet(const Wallet &other) = delete;
    Wallet &operator=(const Wallet &other) = delete;

    Wallet(Wallet &&other);
    Wallet &operator=(Wallet &&other);

    Wallet(Wallet &&w1, Wallet &&w2);
    static Wallet fromBinary(const std::string &binary_str);

    friend Wallet operator+(Wallet &&lhs, Wallet &rhs);
    friend Wallet operator+(Wallet &&lhs, Wallet &&rhs);

    friend Wallet operator-(Wallet &&lhs, Wallet &rhs);
    friend Wallet operator-(Wallet &&lhs, Wallet &&rhs);

    Wallet &operator+=(Wallet &&rhs);
    Wallet &operator+=(Wallet &rhs);

    Wallet &operator-=(Wallet &&rhs);
    Wallet &operator-=(Wallet &rhs);

    friend Wallet operator*(int n, const Wallet &self);
    Wallet operator*(int n) const;
    Wallet &operator*=(int n);

    // If we define them as members, not friends, we cannot do something like:
    // (1 + 2 == Wallet(3)).
    friend bool operator==(const Wallet &lhs, const Wallet &rhs);
    friend bool operator<(const Wallet &lhs, const Wallet &rhs);
    friend bool operator!=(const Wallet &lhs, const Wallet &rhs);
    friend bool operator<=(const Wallet &lhs, const Wallet &rhs);
    friend bool operator>(const Wallet &lhs, const Wallet &rhs);
    friend bool operator>=(const Wallet &lhs, const Wallet &rhs);
    friend std::ostream &operator<<(std::ostream &os, const Wallet &rhs);

    unsigned long long int getUnits() const;
    size_t opSize() const;

    const WalletOperation &operator[](size_t x) const;
};

const Wallet &Empty();

#endif // WALLET_H
