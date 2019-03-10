#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "wallet.h"

constexpr unsigned long long int MAX_COINS = 21 * 1000 * 1000;
constexpr int FRANCTION_DIGITS_IN_COIN = 8;
constexpr unsigned long long int UNITS_IN_COIN = 100 * 1000 * 1000;
constexpr unsigned long long int MAX_UNITS = MAX_COINS * UNITS_IN_COIN;

unsigned long long int Wallet::allUnits = 0;

namespace
{

/*
 * Returns 10 to the power of x
 */
unsigned long long power10(int x)
{
    int result{1};
    for (; x > 0; x--)
        result *= 10;

    return result;
}

/*
 * Converts string to units. In the case of
 * an invalid argument throws exception.
 */
unsigned long long int strToUnits(const std::string &str)
{
    size_t idx;
    long long int temp{std::stoll(str, &idx)};

    if (temp < 0) {
        throw std::invalid_argument{str};
    }

    unsigned long long int coins{static_cast<unsigned long long int>(temp)};
    unsigned long long int units{0};

    if ((str[idx] == ',' || str[idx] == '.') && isdigit(str[idx + 1])) {

        size_t idx_substr;
        units = std::stoll(str.substr(idx + 1), &idx_substr);
        // value of units is guaranteed to be nonnegative due to isdigit.

        if (units >= UNITS_IN_COIN) {
            throw std::invalid_argument{str};
        }

        units *= power10(FRANCTION_DIGITS_IN_COIN - idx_substr);

        idx += 1 + idx_substr;
    }

    if (!std::all_of(str.begin() + idx, str.end(), isspace)) {
        throw std::invalid_argument{str};
    }
    if (coins > MAX_COINS || units > UNITS_IN_COIN) {
        throw std::invalid_argument{str};
    }

    units += coins * UNITS_IN_COIN;

    return units;
}

} // namespace

WalletOperation::WalletOperation(unsigned long long int units)
    : units(units), tp(std::chrono::system_clock::now())
{
}

unsigned long long int WalletOperation::getUnits() const { return units; }

std::ostream &operator<<(std::ostream &stream, const WalletOperation &operation)
{
    std::time_t time{std::chrono::system_clock::to_time_t(operation.tp)};
    std::tm timetm = *std::localtime(&time);

    stream << "Wallet balance is " << operation.units / UNITS_IN_COIN << ','
           << operation.units % UNITS_IN_COIN
           << " B after operation made at day "
           << std::put_time(&timetm, "%Y-%m-%d");

    return stream;
}

bool operator>(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp > rhs.tp;
}

bool operator<=(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp <= rhs.tp;
}

bool operator<(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp < rhs.tp;
}

bool operator>=(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp >= rhs.tp;
}

bool operator==(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp == rhs.tp;
}

bool operator!=(const WalletOperation &lhs, const WalletOperation &rhs)
{
    return lhs.tp != rhs.tp;
}

/*
 * Adds units to the global counter of units in all wallets. Before adding,
 * the function checks if the result of that addition exceeds MAX_UNITS. If so,
 * it throws exception. The value of units may be negative, then instead of
 * adding, it substracts that many units from the global counter. It does not
 * check if the result of that substraction falls below 0.
 */
void Wallet::addToAllUnits(long long int units)
{
    if (allUnits + units > MAX_UNITS) {
        throw std::range_error{"Units in all wallets cannot exceed 2,1e15."};
    }
    else {
        allUnits += units;
    }
}

Wallet::Wallet(int n)
{
    unsigned long long int units{static_cast<unsigned long long int>(n) *
                                 UNITS_IN_COIN};

    addToAllUnits(units);

    this->operations.emplace_back(units);
    this->units = units;
}

Wallet::Wallet() : Wallet(0) {}

Wallet::Wallet(Wallet &&rhs)
{
    this->units = rhs.units;
    this->operations = std::move(rhs.operations);
    this->operations.emplace_back(rhs.units);

    rhs.units = 0;
}

Wallet &Wallet::operator=(Wallet &&rhs)
{
    if (this != &rhs) {
        addToAllUnits(-this->units);

        // NOTE(M): Must suceed, because no additional coins are created.

        this->units = rhs.units;
        this->operations = std::move(rhs.operations);
        this->operations.emplace_back(rhs.units);

        rhs.units = 0;
    }

    return *this;
}

Wallet::Wallet(Wallet &&w1, Wallet &&w2)
{
    this->units = w1.units + w2.units;
    this->operations = std::move(w1.operations);
    this->operations.reserve(w1.operations.size() + w2.operations.size());

    int mid = this->operations.size(); // Store the end of first sorted range
    std::move(w2.operations.begin(), w2.operations.end(),
              std::back_inserter(this->operations));

    std::inplace_merge(this->operations.begin(), this->operations.begin() + mid,
                       this->operations.end());

    this->operations.emplace_back(this->units);

    w1.units = 0;
    w2.units = 0;
#if 0
    w2.operations.clear();
#endif
}

Wallet::Wallet(const char *str)
{
    unsigned long long int units = strToUnits(static_cast<std::string>(str));

    addToAllUnits(units);

    this->units = units;
    this->operations.emplace_back(units);
}

Wallet::Wallet(std::string str) : Wallet(str.c_str()) {}

Wallet::~Wallet()
{
    addToAllUnits(-units); // It is not possible then units > allUnits.

    // NOTE(MATEUSZ): This is c++. Run away as fast as you can, because this
    //                does not make any sense. More in depth: just in case
    //                someone will try to call descrutor explicitly for the
    //                object which is on the stack we might end up calling
    //                destructor twice so this is for safety.
    this->units = 0;
}

Wallet Wallet::fromBinary(const std::string &bstr)
{
    size_t sz{};
    int parsed_value{std::stoi(bstr, &sz, 2)}; // changed stoll into stoi

    // isspace(bstr[0]) is there because stoll will accept leading whitespaces
    //                  and we are forbidden to do so.
    if (isspace(bstr[0]) || sz < bstr.size() || parsed_value < 0)
        throw std::invalid_argument{bstr};

    return Wallet{parsed_value};
}

unsigned long long int Wallet::getUnits() const { return units; }

size_t Wallet::opSize() const { return operations.size(); }

bool operator==(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() == rhs.getUnits();
}

bool operator!=(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() != rhs.getUnits();
}

bool operator<(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() < rhs.getUnits();
}

bool operator<=(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() <= rhs.getUnits();
}

bool operator>(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() > rhs.getUnits();
}

bool operator>=(const Wallet &lhs, const Wallet &rhs)
{
    return lhs.getUnits() >= rhs.getUnits();
}

std::ostream &operator<<(std::ostream &stream, const Wallet &rhs)
{
    stream << "Wallet[" << rhs.getUnits() / UNITS_IN_COIN << ','
           << rhs.getUnits() % UNITS_IN_COIN << " B]";

    return stream;
}

const WalletOperation &Wallet::operator[](size_t idx) const
{
    return operations[idx];
}

// Static initailization to prove we understood something from the previous
// task.
const Wallet &Empty()
{
    static Wallet _empty{};
    return _empty;
}

Wallet operator+(Wallet &&lhs, Wallet &rhs)
{
    return std::move(lhs) + std::move(rhs);
}

Wallet operator+(Wallet &&lhs, Wallet &&rhs)
{
    Wallet retval{}; // TODO: too many operations?
    unsigned long long int units = lhs.units;
    lhs.units = 0;

    // HACK(M): We make an empty wallet and then move the lhs units and history
    //          inside, so that the additional history log is not created.
    retval.units = units;
    retval.operations = std::move(lhs.operations);
    retval += std::move(rhs);

    return retval;
}

Wallet operator-(Wallet &&lhs, Wallet &rhs)
{
    return std::move(lhs) - std::move(rhs);
}

Wallet operator-(Wallet &&lhs, Wallet &&rhs)
{
    Wallet retval{};
    unsigned long long int units = lhs.units;
    lhs.units = 0;

    // HACK(M): We make an empty wallet and then move the lhs units and history
    //          inside, so that the additional history log is not created.
    retval.units = units;
    retval.operations = std::move(lhs.operations);
    retval -= std::move(rhs);

    return retval;
}

Wallet &Wallet::operator+=(Wallet &&rhs)
{
    unsigned long long int units{rhs.units};

    this->units += units;
    this->operations.emplace_back(this->units); // this->units instead of units

    rhs.units = 0;
    rhs.operations.emplace_back(0);

    return *this;
}

Wallet &Wallet::operator+=(Wallet &rhs) { return (*this) += std::move(rhs); }

Wallet &Wallet::operator-=(Wallet &&rhs)
{
    unsigned long long int units{rhs.units};

    if (this->units < units) {
        throw std::range_error{"Units in a wallet cannot fall below 0"};
    }

    this->units -= units;
    this->operations.emplace_back(this->units);

    rhs.units += units;
    rhs.operations.emplace_back(rhs.units);

    return *this;
}

Wallet &Wallet::operator-=(Wallet &rhs) { return (*this) -= std::move(rhs); }

Wallet operator*(int n, const Wallet &self) { return self * n; }

Wallet Wallet::operator*(int n) const
{
    addToAllUnits(this->units * n);

    Wallet retval = Wallet();
    retval.units = this->units * n;
    retval.operations.emplace_back(retval.units);

    return retval;
}

Wallet &Wallet::operator*=(int n)
{
    // We may add to allUnits negative units here (n == 0), but it is fine.
    addToAllUnits(this->units * (n - 1));

    this->units *= n;
    this->operations.emplace_back(this->units);

    return *this;
}
