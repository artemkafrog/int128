#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>

class Int128 
{
private:
    uint64_t low;
    int64_t high;

    explicit Int128(uint64_t l, int64_t h) : low(l), high(h) {}
    
    bool isNegative() const { return high < 0; }
    bool isZero() const { return high == 0 && low == 0; }
    
    Int128 abs() const;
    
public:
    Int128();
    Int128(int64_t value);
    explicit Int128(std::string_view str);
    
    explicit operator int64_t() const;
    explicit operator double() const;
    std::string str() const;
    
    Int128 operator+() const;
    Int128 operator-() const;
    Int128 operator+(const Int128& other) const;
    Int128 operator-(const Int128& other) const;
    Int128 operator*(const Int128& other) const;
    Int128 operator/(const Int128& other) const;
    
    Int128& operator+=(const Int128& other);
    Int128& operator-=(const Int128& other);
    Int128& operator*=(const Int128& other);
    Int128& operator/=(const Int128& other);
    
    bool operator==(const Int128& other) const;
    bool operator!=(const Int128& other) const;
    bool operator<(const Int128& other) const;
    bool operator<=(const Int128& other) const;
    bool operator>(const Int128& other) const;
    bool operator>=(const Int128& other) const;
    
    friend std::ostream& operator<<(std::ostream& os, const Int128& value);
};

Int128 operator+(int64_t a, const Int128& b);
Int128 operator-(int64_t a, const Int128& b);
Int128 operator*(int64_t a, const Int128& b);
Int128 operator/(int64_t a, const Int128& b);