#include "Int128.hpp"
#include <algorithm>
#include <cctype>

Int128::Int128() : left(0), right(0) {}

Int128::Int128(int64_t a) : left(0), right(static_cast<uint64_t>(a))
{
    if (a < 0)
    {
        left = -1;
        right = static_cast<uint64_t>(a);
    }
}

Int128::Int128(std::string_view a) : left(0), right(0)
{
    bool is_negative = false;
    size_t start = 0;

    if (a[0] == '-')
    {
        is_negative = true;
        start = 1;
    }

    for (size_t i = start; i < a.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(a[i])))
        {
            break;
        }
        int digit = a[i] - '0';
        *this = *this * Int128(10) + Int128(static_cast<int64_t>(digit));
    }

    if (is_negative)
    {
        *this = -(*this);
    }
}

Int128::operator int64_t() const
{
    return static_cast<int64_t>(right);
}

Int128::operator double() const
{
    const double TWO_TO_64 = 18446744073709551616.0;
    double result = static_cast<double>(right);
    
    if (left > 0)
    {
        result += static_cast<double>(left) * TWO_TO_64;
    }
    else if (left < 0)
    {
        result -= static_cast<double>(-left) * TWO_TO_64;
    }
    
    return result;
}

std::string Int128::str() const
{
    if (isZero()) return "0";

    bool is_negative = isNegative();
    uint64_t l = static_cast<uint64_t>(left);
    uint64_t r = right;

    if (is_negative)
    {
        l = ~l;
        r = ~r;
        r += 1;
        if (r == 0) l += 1;
    }

    std::string result;
    while (l > 0 || r > 0)
    {
        uint64_t rem = 0;
        uint64_t next_l = l / 10;
        rem = l % 10;
        
        uint64_t next_r = 0;
        for (int i = 63; i >= 0; --i)
        {
            rem = (rem << 1) | ((r >> i) & 1);
            if (rem >= 10)
            {
                rem -= 10;
                next_r |= (1ULL << i);
            }
        }

        result.push_back(static_cast<char>('0' + rem));
        l = next_l;
        r = next_r;
    }

    if (is_negative) result.push_back('-');
    std::reverse(result.begin(), result.end());
    return result;
}

Int128 Int128::operator+() const { return *this; }

Int128 Int128::operator-() const
{
    uint64_t l = ~static_cast<uint64_t>(left);
    uint64_t r = ~right;
    r += 1;

    if (r == 0)
    {
        l += 1;
    }

    Int128 result;
    result.left = static_cast<int64_t>(l);
    result.right = r;
    
    return result;
}

Int128 Int128::operator+(const Int128& other) const
{
    Int128 result;
    result.right = right + other.right;
    uint64_t carry = (result.right < right) ? 1 : 0;
    result.left = left + other.left + static_cast<int64_t>(carry);
    return result;
}

Int128 Int128::operator-(const Int128& other) const
{
    return *this + (-other);
}

Int128 Int128::operator*(const Int128& other) const
{
    bool result_negative = (left < 0) != (other.left < 0);

    Int128 a = (left < 0) ? -(*this) : *this;
    Int128 b = (other.left < 0) ? -(other) : other;

    uint64_t a3 = static_cast<uint64_t>(a.left) >> 32;
    uint64_t a2 = static_cast<uint64_t>(a.left) & 0xFFFFFFFF;
    uint64_t a1 = a.right >> 32;
    uint64_t a0 = a.right & 0xFFFFFFFF;

    uint64_t b3 = static_cast<uint64_t>(b.left) >> 32;
    uint64_t b2 = static_cast<uint64_t>(b.left) & 0xFFFFFFFF;
    uint64_t b1 = b.right >> 32;
    uint64_t b0 = b.right & 0xFFFFFFFF;

    Int128 res(0);

    uint64_t p0 = a0 * b0;
    res.right = p0;

    auto add_part = [&](uint64_t val, int shift_bits)
    {
        Int128 part;
        if (shift_bits < 64)
        {
            part.right = val << shift_bits;
            part.left = val >> (64 - shift_bits);
        }
        else
        {
            part.right = 0;
            part.left = val << (shift_bits - 64);
        }
        res = res + part;
    };

    add_part(a0 * b1, 32);
    add_part(a1 * b0, 32);
    add_part(a0 * b2, 64);
    add_part(a1 * b1, 64);
    add_part(a2 * b0, 64);
    add_part(a0 * b3, 96);
    add_part(a1 * b2, 96);
    add_part(a2 * b1, 96);
    add_part(a3 * b0, 96);

    return result_negative ? -res : res;
}

Int128 Int128::operator/(const Int128& other) const
{
    if (other.isZero()) return Int128(0);
    
    bool result_negative = (left < 0) != (other.left < 0);

    Int128 dividend = (left < 0) ? -(*this) : *this;
    Int128 divisor = (other.left < 0) ? -other : other;

    if (dividend < divisor) return Int128(0);

    Int128 quotient(0);
    Int128 remainder(0);

    for (int i = 127; i >= 0; --i)
    {
        uint64_t carry_rem = (remainder.right >> 63) & 1;
        remainder.right <<= 1;
        remainder.left = (static_cast<uint64_t>(remainder.left) << 1) | carry_rem;

        uint64_t bit;
        if (i >= 64)
        {
            bit = (static_cast<uint64_t>(dividend.left) >> (i - 64)) & 1;
        }
        else
        {
            bit = (dividend.right >> i) & 1;
        }
        
        remainder.right |= bit;

        bool gte = false;
        if (static_cast<uint64_t>(remainder.left) > static_cast<uint64_t>(divisor.left))
        {
            gte = true;
        }
        else if (static_cast<uint64_t>(remainder.left) == static_cast<uint64_t>(divisor.left))
        {
            if (remainder.right >= divisor.right) gte = true;
        }

        if (gte)
        {
            uint64_t old_l = remainder.right;
            remainder.right -= divisor.right;
            uint64_t borrow = (remainder.right > old_l) ? 1 : 0;
            remainder.left -= (divisor.left + borrow);

            if (i >= 64)
            {
                quotient.left |= (1LL << (i - 64));
            }
            else
            {
                quotient.right |= (1ULL << i);
            }
        }
    }

    return result_negative ? -quotient : quotient;
}

Int128& Int128::operator+=(const Int128& other)
{
    *this = *this + other;
    return *this;
}

Int128& Int128::operator-=(const Int128& other)
{
    *this = *this - other;
    return *this;
}

Int128& Int128::operator*=(const Int128& other)
{
    *this = *this * other;
    return *this;
}

Int128& Int128::operator/=(const Int128& other)
{
    *this = *this / other;
    return *this;
}

bool Int128::operator==(const Int128& other) const
{
    return left == other.left && right == other.right;
}

bool Int128::operator!=(const Int128& other) const
{
    return !(*this == other);
}

bool Int128::operator<(const Int128& other) const
{
    if (left != other.left)
    {
        return left < other.left;
    }
    return right < other.right;
}

bool Int128::operator<=(const Int128& other) const
{
    return *this < other || *this == other;
}

bool Int128::operator>(const Int128& other) const
{
    return !(*this <= other);
}

bool Int128::operator>=(const Int128& other) const
{
    return !(*this < other);
}

Int128 Int128::abs() const
{
    if (isNegative()) return -(*this);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Int128& value)
{
    os << value.str();
    return os;
}

Int128 operator+(int64_t a, const Int128& b)
{
    return Int128(a) + b;
}

Int128 operator-(int64_t a, const Int128& b)
{
    return Int128(a) - b;
}

Int128 operator*(int64_t a, const Int128& b)
{
    return Int128(a) * b;
}

Int128 operator/(int64_t a, const Int128& b)
{
    return Int128(a) / b;
}