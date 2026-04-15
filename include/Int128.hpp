#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>

class Int128
{
private:
    int64_t left = 0;
    uint64_t right = 0;
    
    bool isNegative() const { return left < 0; }
    bool isZero() const { return left == 0 && right == 0; }
    Int128 abs() const;
public:
    Int128();
    Int128(int64_t a);
    explicit Int128(std::string_view a);

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