#include "Int128.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>

Int128::Int128() : low(0), high(0) {}

Int128::Int128(int64_t value)
{
    if (value >= 0)
    {
        low = static_cast<uint64_t>(value);
        high = 0;
    }
    else
    {
        low = static_cast<uint64_t>(value);
        high = -1;
    }
}

Int128::Int128(std::string_view str) : low(0), high(0)
{
    bool is_negative = (str[0] == '-');
    size_t start = is_negative ? 1 : 0;
    
    for (size_t i = start; i < str.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
        {
            break;
        }
        
        *this = *this * Int128(10) + Int128(static_cast<int64_t>(str[i] - '0'));
    }
    
    if (is_negative)
    {
        *this = -(*this);
    }
}

Int128::operator int64_t() const { return static_cast<int64_t>(low); }

Int128::operator double() const
{
    double result = static_cast<double>(low);
    const double TWO_TO_64 = 18446744073709551616.0;
    
    if (high > 0)
    {
        result += static_cast<double>(high) * TWO_TO_64;
    }
    else if (high < 0)
    {
        result -= static_cast<double>(-high) * TWO_TO_64;
    }
    
    return result;
}

std::string Int128::str() const
{
    if (isZero()) return "0";
    
    Int128 temp = abs();
    std::string result;
    
    while (!temp.isZero())
    {
        uint64_t remainder = 0;
        Int128 quotient(0, 0);
        
        for (int i = 127; i >= 0; --i)
        {
            uint64_t bit = (i >= 64)
                ? ((temp.high >> (i - 64)) & 1)
                : ((temp.low >> i) & 1);
            
            remainder = (remainder << 1) | bit;
            
            if (remainder >= 10)
            {
                remainder -= 10;
                
                if (i >= 64)
                {
                    quotient.high |= (1ULL << (i - 64));
                }
                else
                {
                    quotient.low |= (1ULL << i);
                }
            }
        }
        
        result.push_back(static_cast<char>('0' + remainder));
        temp = quotient;
    }
    
    std::reverse(result.begin(), result.end());
    
    return isNegative() ? "-" + result : result;
}

Int128 Int128::operator+() const { return *this; }

Int128 Int128::operator-() const
{
    if (isZero()) return Int128(0, 0);
    return Int128(~low + 1, ~high);
}

Int128 Int128::operator+(const Int128& other) const
{
    uint64_t new_low = low + other.low;
    int64_t new_high = high + other.high;
    
    if (new_low < low) new_high += 1;
    return Int128(new_low, new_high);
}

Int128 Int128::operator-(const Int128& other) const { return *this + (-other); }

Int128 Int128::operator*(const Int128& other) const
{
    uint64_t a_low = low & 0xFFFFFFFF;
    uint64_t a_high = low >> 32;
    uint64_t b_low = other.low & 0xFFFFFFFF;
    uint64_t b_high = other.low >> 32;
    
    uint64_t r0 = a_low * b_low;
    uint64_t r1 = a_low * b_high;
    uint64_t r2 = a_high * b_low;
    uint64_t r3 = a_high * b_high;

    uint64_t mid1 = (r0 >> 32) + (r1 & 0xFFFFFFFF) + (r2 & 0xFFFFFFFF);
    uint64_t mid2 = (mid1 >> 32) + (r1 >> 32) + (r2 >> 32) + (r3 & 0xFFFFFFFF);
    
    uint64_t new_low = (mid1 << 32) | (r0 & 0xFFFFFFFF);
    int64_t new_high = static_cast<int64_t>((mid2 >> 32) + (r3 >> 32));
    
    Int128 result(new_low, new_high);
    
    bool is_negative = (isNegative() != other.isNegative());
    if (is_negative && !result.isZero())
    {
        result = -result;
    }
    
    return result;
}

Int128 Int128::operator/(const Int128& other) const
{
    if (other.isZero()) return Int128(0, 0); 
    
    Int128 dividend = *this;
    Int128 divisor = other;
    bool is_negative = (dividend.isNegative() != divisor.isNegative());
    
    if (dividend.isNegative()) dividend = dividend.abs();
    if (divisor.isNegative()) divisor = divisor.abs();
    
    if (dividend < divisor) return Int128(0, 0);
    
    Int128 quotient(0, 0);
    Int128 remainder(0, 0);
    
    for (int i = 127; i >= 0; --i)
    {
        remainder = remainder * Int128(2);
                
        if (i >= 64)
        {
            remainder.low |= ((dividend.high >> (i - 64)) & 1);
        }
        else
        {
            remainder.low |= ((dividend.low >> i) & 1);
        }
        
        if (remainder >= divisor)
        {
            remainder = remainder - divisor;
            
            if (i >= 64)
            {
                quotient.high |= (1ULL << (i - 64));
            }
            else
            {
                quotient.low |= (1ULL << i);
            }
        }
    }
    
    return is_negative ? -quotient : quotient;
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
    return low == other.low && high == other.high;
}

bool Int128::operator!=(const Int128& other) const { return !(*this == other); }

bool Int128::operator<(const Int128& other) const
{
    if (high != other.high) return high < other.high;
    return low < other.low;
}

bool Int128::operator<=(const Int128& other) const
{
    return *this < other || *this == other;
}

bool Int128::operator>(const Int128& other) const { return !(*this <= other); }

bool Int128::operator>=(const Int128& other) const { return !(*this < other); }

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

Int128 operator+(int64_t a, const Int128& b) { return Int128(a) + b; }
Int128 operator-(int64_t a, const Int128& b) { return Int128(a) - b; }
Int128 operator*(int64_t a, const Int128& b) { return Int128(a) * b; }
Int128 operator/(int64_t a, const Int128& b) { return Int128(a) / b; }