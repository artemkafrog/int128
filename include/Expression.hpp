#pragma once

#include "Int128.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>

class Expression 
{
public:
    virtual Int128 eval(const std::unordered_map<std::string, Int128>& variables) const = 0;
    virtual std::unique_ptr<Expression> clone() const = 0;
    virtual void print(std::ostream& output_stream) const = 0;
    
    friend std::ostream& operator<<(std::ostream& output_stream, const Expression& expression)
    {
        expression.print(output_stream);
        return output_stream;
    }

    virtual ~Expression() = default;
};

class Const : public Expression 
{
    Int128 value;
    
public:
    explicit Const(Int128 val) : value(val) {}
    
    Int128 eval(const std::unordered_map<std::string, Int128>&) const override { return value; }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Const>(value); }
    void print(std::ostream& output_stream) const override { output_stream << value; }
};

class Variable : public Expression 
{
    std::string name;
    
public:
    explicit Variable(const std::string& var_name) : name(var_name) {}
    
    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override
    {
        auto iterator = variables.find(name);
        return (iterator != variables.end()) ? iterator->second : Int128(0);
    }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Variable>(name); }
    void print(std::ostream& output_stream) const override { output_stream << name; }
};

class Negate : public Expression 
{
    std::unique_ptr<Expression> operand;
    
public:
    explicit Negate(std::unique_ptr<Expression> expr) : operand(std::move(expr)) {}
    
    Negate(const Negate& other) : operand(other.operand->clone()) {}
    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override { return -(operand->eval(variables)); }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Negate>(operand->clone()); }
    void print(std::ostream& output_stream) const override
    {
        output_stream << "(-";
        operand->print(output_stream);
        output_stream << ")";
    }
};

class Add : public Expression 
{
    std::unique_ptr<Expression> left_operand;
    std::unique_ptr<Expression> right_operand;
    
public:
    Add(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : left_operand(std::move(left)), right_operand(std::move(right)) {}
    Add(const Add& other) : left_operand(other.left_operand->clone()), right_operand(other.right_operand->clone()) {}

    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override { return left_operand->eval(variables) + right_operand->eval(variables); }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Add>(left_operand->clone(), right_operand->clone()); }
    void print(std::ostream& output_stream) const override
    {
        output_stream << "(";
        left_operand->print(output_stream);
        output_stream << " + ";
        right_operand->print(output_stream);
        output_stream << ")";
    }
};

class Subtract : public Expression 
{
    std::unique_ptr<Expression> left_operand;
    std::unique_ptr<Expression> right_operand;
    
public:
    Subtract(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : left_operand(std::move(left)), right_operand(std::move(right)) {}
    Subtract(const Subtract& other) : left_operand(other.left_operand->clone()), right_operand(other.right_operand->clone()) {}
    
    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override { return left_operand->eval(variables) - right_operand->eval(variables); }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Subtract>(left_operand->clone(), right_operand->clone()); }
    void print(std::ostream& output_stream) const override
    {
        output_stream << "(";
        left_operand->print(output_stream);
        output_stream << " - ";
        right_operand->print(output_stream);
        output_stream << ")";
    }
};

class Multiply : public Expression 
{
    std::unique_ptr<Expression> left_operand;
    std::unique_ptr<Expression> right_operand;
    
public:
    Multiply(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : left_operand(std::move(left)), right_operand(std::move(right)) {}
    Multiply(const Multiply& other) : left_operand(other.left_operand->clone()), right_operand(other.right_operand->clone()) {}

    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override { return left_operand->eval(variables) * right_operand->eval(variables); }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Multiply>(left_operand->clone(), right_operand->clone()); }
    void print(std::ostream& output_stream) const override
    {
        output_stream << "(";
        left_operand->print(output_stream);
        output_stream << " * ";
        right_operand->print(output_stream);
        output_stream << ")";
    }
};

class Divide : public Expression 
{
    std::unique_ptr<Expression> left_operand;
    std::unique_ptr<Expression> right_operand;
    
public:
    Divide(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : left_operand(std::move(left)), right_operand(std::move(right)) {} 
    Divide(const Divide& other) : left_operand(other.left_operand->clone()), right_operand(other.right_operand->clone()) {}
    
    Int128 eval(const std::unordered_map<std::string, Int128>& variables) const override { return left_operand->eval(variables) / right_operand->eval(variables); }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Divide>(left_operand->clone(), right_operand->clone()); }
    void print(std::ostream& output_stream) const override
    {
        output_stream << "(";
        left_operand->print(output_stream);
        output_stream << " / ";
        right_operand->print(output_stream);
        output_stream << ")";
    }
};

inline std::unique_ptr<Expression> operator+(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
{
    return std::make_unique<Add>(std::move(left), std::move(right));
}

inline std::unique_ptr<Expression> operator-(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
{
    return std::make_unique<Subtract>(std::move(left), std::move(right));
}

inline std::unique_ptr<Expression> operator*(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
{
    return std::make_unique<Multiply>(std::move(left), std::move(right));
}

inline std::unique_ptr<Expression> operator/(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
{
    return std::make_unique<Divide>(std::move(left), std::move(right));
}

inline std::unique_ptr<Expression> operator-(std::unique_ptr<Expression> operand)
{
    return std::make_unique<Negate>(std::move(operand));
}

inline std::unique_ptr<Expression> operator+(int64_t value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Add>(std::make_unique<Const>(value), std::move(right));
}

inline std::unique_ptr<Expression> operator-(int64_t value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Subtract>(std::make_unique<Const>(value), std::move(right));
}

inline std::unique_ptr<Expression> operator*(int64_t value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Multiply>(std::make_unique<Const>(value), std::move(right));
}

inline std::unique_ptr<Expression> operator/(int64_t value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Divide>(std::make_unique<Const>(value), std::move(right));
}

inline std::unique_ptr<Expression> operator+(std::unique_ptr<Expression> left, int64_t value)
{
    return std::make_unique<Add>(std::move(left), std::make_unique<Const>(value));
}

inline std::unique_ptr<Expression> operator-(std::unique_ptr<Expression> left, int64_t value)
{
    return std::make_unique<Subtract>(std::move(left), std::make_unique<Const>(value));
}

inline std::unique_ptr<Expression> operator*(std::unique_ptr<Expression> left, int64_t value)
{
    return std::make_unique<Multiply>(std::move(left), std::make_unique<Const>(value));
}

inline std::unique_ptr<Expression> operator+(int value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Add>(std::make_unique<Const>(Int128(value)), std::move(right));
}

inline std::unique_ptr<Expression> operator-(int value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Subtract>(std::make_unique<Const>(Int128(value)), std::move(right));
}

inline std::unique_ptr<Expression> operator*(int value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Multiply>(std::make_unique<Const>(Int128(value)), std::move(right));
}

inline std::unique_ptr<Expression> operator/(int value, std::unique_ptr<Expression> right)
{
    return std::make_unique<Divide>(std::make_unique<Const>(Int128(value)), std::move(right));
}

inline std::unique_ptr<Expression> operator+(std::unique_ptr<Expression> left, int value)
{
    return std::make_unique<Add>(std::move(left), std::make_unique<Const>(Int128(value)));
}

inline std::unique_ptr<Expression> operator-(std::unique_ptr<Expression> left, int value)
{
    return std::make_unique<Subtract>(std::move(left), std::make_unique<Const>(Int128(value)));
}

inline std::unique_ptr<Expression> operator*(std::unique_ptr<Expression> left, int value)
{
    return std::make_unique<Multiply>(std::move(left), std::make_unique<Const>(Int128(value)));
}

inline std::unique_ptr<Expression> constant(Int128 value)
{
    return std::make_unique<Const>(value);
}

inline std::unique_ptr<Expression> variable(const std::string& name)
{
    return std::make_unique<Variable>(name);
}