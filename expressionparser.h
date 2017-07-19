#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include <QString>
#include <QVector>
#include <QStack>
#include <QMap>

// all available tokens
enum TOKEN {INVALID_TOKEN, OP_PLUS, OP_MINUS, OP_TIMES, OP_DIVIDE, PAR_LEFT, PAR_RIGHT,
            OP_UNARY_MINUS, OP_SIN, OP_COS, OP_ASIN, OP_ACOS, OP_TAN, OP_ATAN, OP_RADS,
            OP_DEGS, OP_SQRT, OP_EXP
           };
// some constants
const double PI = 3.141592653589793238463;
const double E = 2.718281828459045;
const double SIXTYFIVE = 65;
// Excpetion class
class UnknownStringError {};

class ExpressionParser
{
private:
    // this class encapsulates both numeric and token values
    //  so both can be used in same containers
    //  (this code is actually ported from python, so this is an "emulation"
    //   of python's dynamic typing)
    class EPListItem
    {
        enum TYPE {T, N};
        TYPE type;
        TOKEN token_value;
        double double_value;

    public:
        EPListItem(TOKEN token);
        EPListItem(double value);
        TOKEN getToken();
        double getValue();
        bool isToken();
        bool isNumber();
        bool isOperator();
        bool isBinaryOperator();
    }; // EPListItem

    // shortening of a commonly-used container
    typedef QVector<EPListItem*> vec;
    // dicts which map chars to tokens or constant values
    QMap<QChar, TOKEN> single_map;
    QMap<QString, TOKEN> multi_map;
    QMap<QString, double> const_map;

public:
    ExpressionParser();
    QString calc(const QString &expr);
private:
    int getPrecedence(const TOKEN token);
    vec* tokenize(const QString &expr);
    vec* convert(vec* tokenized);
    QString eval(vec &converted);

}; // ExpressionParser

#endif // EXPRESSIONPARSER_H
