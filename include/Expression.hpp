#pragma once

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <memory>
#include "Int128.hpp"

class Expression;
using ExprPtr = std::unique_ptr<Expression>;

class Expression
{
public:
    virtual Expression* clone() const = 0;
    virtual Int128 eval(const std::map<std::string, Int128>& values) const = 0;
    virtual std::string str() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Expression& expr)
    {
        return os << expr.str();
    }

    virtual ~Expression() = default;
};

class Const : public Expression
{
public:
    explicit Const(Int128 v) : value(v) {}

    Int128 eval(const std::map<std::string, Int128>&) const override
    {
        return value;
    }

    Expression* clone() const override
    {
        return new Const(value);
    }

    std::string str() const override
    {
        return value.str();
    }

private:
    Int128 value;
};

class Variable : public Expression
{
public:
    explicit Variable(std::string_view n) : name(n) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        auto it = values.find(name);
        if (it != values.end())
        {
            return it->second;
        }
        return Int128(0);
    }

    Expression* clone() const override
    {
        return new Variable(name);
    }

    std::string str() const override
    {
        return name;
    }

private:
    std::string name;
};

class Negate : public Expression
{
public:
    explicit Negate(Expression* e) : expr(ExprPtr(e)) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        return -expr->eval(values);
    }

    Expression* clone() const override
    {
        return new Negate(expr->clone());
    }

    std::string str() const override
    {
        return "-" + expr->str();
    }

private:
    ExprPtr expr;
};

class Add : public Expression
{
public:
    Add(Expression* l, Expression* r) : left(ExprPtr(l)), right(ExprPtr(r)) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        return left->eval(values) + right->eval(values);
    }

    Expression* clone() const override
    {
        return new Add(left->clone(), right->clone());
    }

    std::string str() const override
    {
        return "(" + left->str() + " + " + right->str() + ")";
    }

private:
    ExprPtr left;
    ExprPtr right;
};

class Subtract : public Expression
{
public:
    Subtract(Expression* l, Expression* r) : left(ExprPtr(l)), right(ExprPtr(r)) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        return left->eval(values) - right->eval(values);
    }

    Expression* clone() const override
    {
        return new Subtract(left->clone(), right->clone());
    }

    std::string str() const override
    {
        return "(" + left->str() + " - " + right->str() + ")";
    }

private:
    ExprPtr left;
    ExprPtr right;
};

class Multiply : public Expression
{
public:
    Multiply(Expression* l, Expression* r) : left(ExprPtr(l)), right(ExprPtr(r)) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        return left->eval(values) * right->eval(values);
    }

    Expression* clone() const override
    {
        return new Multiply(left->clone(), right->clone());
    }

    std::string str() const override
    {
        return "(" + left->str() + " * " + right->str() + ")";
    }

private:
    ExprPtr left;
    ExprPtr right;
};

class Divide : public Expression
{
public:
    Divide(Expression* l, Expression* r) : left(ExprPtr(l)), right(ExprPtr(r)) {}

    Int128 eval(const std::map<std::string, Int128>& values) const override
    {
        return left->eval(values) / right->eval(values);
    }

    Expression* clone() const override
    {
        return new Divide(left->clone(), right->clone());
    }

    std::string str() const override
    {
        return "(" + left->str() + " / " + right->str() + ")";
    }

private:
    ExprPtr left;
    ExprPtr right;
};

// Операторы
inline Add operator+(const Expression& l, const Expression& r)
{
    return Add(l.clone(), r.clone());
}

inline Subtract operator-(const Expression& l, const Expression& r)
{
    return Subtract(l.clone(), r.clone());
}

inline Negate operator-(const Expression& e)
{
    return Negate(e.clone());
}

inline Multiply operator*(const Expression& l, const Expression& r)
{
    return Multiply(l.clone(), r.clone());
}

inline Divide operator/(const Expression& l, const Expression& r)
{
    return Divide(l.clone(), r.clone());
}