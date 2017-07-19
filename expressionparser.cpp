#include "expressionparser.h"

ExpressionParser::ExpressionParser()
{
    // init of dicts from chars to tokens
    // one-char operators
    single_map['+'] = OP_PLUS;
    single_map['-'] = OP_MINUS;
    single_map['*'] = OP_TIMES;
    single_map['/'] = OP_DIVIDE;
    single_map['('] = PAR_LEFT;
    single_map[')'] = PAR_RIGHT;
    // multi-char operators
    multi_map["sin"] = OP_SIN;
    multi_map["cos"] = OP_COS;
    multi_map["tan"] = OP_TAN;
    multi_map["asin"] = OP_ASIN;
    multi_map["acos"] = OP_ACOS;
    multi_map["atan"] = OP_ATAN;
    multi_map["degs"] = OP_DEGS;
    multi_map["rads"] = OP_RADS;
    multi_map["sqrt"] = OP_SQRT;
    multi_map["exp"] = OP_EXP;
    // some constants
    const_map["pi"] = PI;
    const_map["e"] = E;
    const_map["sixtyfive"] = SIXTYFIVE;
}

int ExpressionParser::getPrecedence(const TOKEN token)
{
    // lower number means higher precedence
    switch(token)
    {
    case OP_UNARY_MINUS:
    case OP_SIN:
    case OP_COS:
    case OP_TAN:
    case OP_ASIN:
    case OP_ACOS:
    case OP_ATAN:
    case OP_DEGS:
    case OP_RADS:
    case OP_SQRT:
    case OP_EXP:
        return 1;
    case OP_DIVIDE:
    case OP_TIMES:
        return 2;
    case OP_PLUS:
    case OP_MINUS:
        return 3;
    default:
        return 999;
    }
}

// converts expression string into a list of tokens and numbers
ExpressionParser::vec* ExpressionParser::tokenize(const QString &expr)
{
    vec* v = new vec;

    // a copy of input string that can be modified
    QString workable_str = QString(expr);

    // flags to determine whether '-' is an unary minus operator
    bool lastForUnaryMinus = false;
    bool isFirstChar = true;


    while (!workable_str.isEmpty())
    {
        // taking the first symbol in string
        QChar first_char = workable_str.at(0);
        // if it is an one-char operator, add corresponding token to return vector
        if (single_map.find(first_char) != single_map.end())
        {
            TOKEN toAdd;
            // detection of an unary minus
            // a '-' means unary minus if it's first char in initial string
            //  or if it's right after any operator but ')'
            if (first_char == '-' && (lastForUnaryMinus || isFirstChar))
            {
                toAdd = OP_UNARY_MINUS;
            }
            else
            {
                // if it's not an unary minus, simply use the dict to determine token
                toAdd = single_map[first_char];
            }
            lastForUnaryMinus = (first_char == ')') ? false : true;
            // add a new item to return vector and discard the symbol
            EPListItem* newToken = new EPListItem(toAdd);
            v->push_back(newToken);
            workable_str.remove(0, 1);
        }
        // whitespaces are completely ignored
        else if (first_char == ' ')
        {
            workable_str.remove(0, 1);
        }
        // detection of multi-char operators, constants and numbers
        else
        {
            int i = 0;
            QChar curr_char = workable_str.at(i);
            // all symbols next to detected one that are not whitespaces nor one-char operators
            //   are glued together to one string
            while (curr_char != ' ' && single_map.find(curr_char) == single_map.end())
            {
                i++;
                if (i == workable_str.length())
                {
                    break;
                }
                curr_char = workable_str.at(i);
            }
            QString part = workable_str.left(i);
            // the resulting substring is successively tested whether it's a multi-char operator,
            //   a constant, or a number
            if (part.length() > 0)
            {
                // testing for multi-char operator
                if (multi_map.find(part) != multi_map.end())
                {
                    EPListItem* newToken = new EPListItem(multi_map[part]);
                    v->push_back(newToken);
                    lastForUnaryMinus = true;
                }
                // testing for constant
                else if (const_map.find(part) != const_map.end())
                {
                    EPListItem* newToken = new EPListItem(const_map[part]);
                    v->push_back(newToken);
                    lastForUnaryMinus = false;
                }
                // else it is a number or an syntax error
                else
                {
                    // flag of successful double parsing
                    bool conv_ok;
                    double value = part.toDouble(&conv_ok);
                    if (conv_ok)
                    {
                        EPListItem* newDouble = new EPListItem(value);
                        v->push_back(newDouble);
                        lastForUnaryMinus = false;
                    }
                    else
                    {
                        throw UnknownStringError();
                    }
                }
                workable_str.remove(0, i);
            }
            else
            {
                break;
            }
        }
        if (isFirstChar) isFirstChar = false;
    }
    return v;
}
// converts list in infix notation to postfix notation using shunting-yard algorithm
// see https://en.wikipedia.org/wiki/Shunting-yard_algorithm
ExpressionParser::vec* ExpressionParser::convert(vec* tokenized)
{
   QStack<EPListItem*> stack;
   vec* out = new vec;
   while(!tokenized->isEmpty())
   {
       // get a new item from vector
       EPListItem* current_token = tokenized->first();
       tokenized->removeFirst();
       // if it's a token
       if (current_token->isToken())
       {
           // if it's a (
           if (current_token->getToken() == PAR_LEFT)
           {
               // push it to stack
               stack.append(current_token);
           }
           // if it's a )
           else if (current_token->getToken() == PAR_RIGHT)
           {
               if (!stack.isEmpty())
               {
                   while (!stack.isEmpty() && stack.top()->getToken() != PAR_LEFT)
                   {
                       // push everything in stack to out until a corresponding ( pops off
                       out->append(stack.pop());
                   }
                   stack.pop();
               }
           }
           // if it's any other operator
           else
           {
               if (!stack.isEmpty())
               {
                   while(!stack.isEmpty() && stack.top()->isOperator())
                   {
                       // take a peek to the stack,
                       //   if stack's top is a token and has a higher precedence,
                       //   pop it off to out
                       TOKEN current = current_token->getToken();
                       TOKEN next = stack.top()->getToken();
                       // binary operators require >= to work
                       //   with > some expressions evaluate incorrectly
                       if (current_token->isBinaryOperator())
                       {
                           if (getPrecedence(current) >= getPrecedence(next))
                           {
                               out->append(stack.pop());
                           }
                           else
                           {
                               break;
                           }
                       }
                       // unary operators require > to work
                       // with >= "--6" gets converted to <unary_minus> 6 <unary_minus>
                       //   which is not correct
                       else
                       {
                           if (getPrecedence(current) > getPrecedence(next))
                           {
                               out->append(stack.pop());
                           }
                           else
                           {
                               break;
                           }
                       }
                   }
               }
               // push current token to stock regardless of tests above
               stack.append(current_token);
           }
       }
       // if it's a number, just push it to out
       else
       {
           out->append(current_token);
       }
   }
   // push off any remains in stack
   while(!stack.isEmpty())
   {
       out->append(stack.pop());
   }
   return out;

}
// evaluates a vector in postfix notation
QString ExpressionParser::eval(vec &converted)
{
    QStack<EPListItem*> stack;

    for (int i = 0; i < converted.size(); i++)
    {
        double a, b;
        double value;
        // if it's a token
        if (converted[i]->isToken())
        {
            // parentheses check, very unlikely to happen
            if (!converted[i]->isOperator())
            {
                return "Parsing error: Unexpected parenthesis in RPN";
            }
            else
            {
                // any operator has at least one operand,
                //  so we try to get one from stack
                TOKEN token = converted[i]->getToken();
                if (!stack.isEmpty())
                {
                    a = stack.pop()->getValue();
                }
                else
                {
                    return "Syntax error";
                }
                // if it's a binary operator,
                //   second operand gets pulled the same way
                if (converted[i]->isBinaryOperator())
                {
                    if (!stack.isEmpty())
                    {
                        b = stack.pop()->getValue();
                    }
                    else
                    {
                        return "Syntax error";
                    }
                }
                // do the math corresponing to the token
                switch(token)
                {
                case OP_PLUS:
                    value = b + a;
                    break;
                case OP_MINUS:
                    value = b - a;
                    break;
                case OP_TIMES:
                    value = b * a;
                    break;
                case OP_DIVIDE:
                    value = b / a;
                    break;
                case OP_UNARY_MINUS:
                    value = -a;
                    break;
                case OP_SIN:
                    value = sin(a);
                    break;
                case OP_COS:
                    value = cos(a);
                    break;
                case OP_TAN:
                    value = tan(a);
                    break;
                case OP_ASIN:
                    value = asin(a);
                    break;
                case OP_ACOS:
                    value = acos(a);
                    break;
                case OP_ATAN:
                    value = atan(a);
                    break;
                case OP_DEGS:
                    value = 180*a/PI;
                    break;
                case OP_RADS:
                    value = PI*a/180;
                    break;
                case OP_SQRT:
                    value = sqrt(a);
                    break;
                case OP_EXP:
                    value = exp(a);
                    break;
                default:
                    return "Parsing error: Invalid token";
                }
            // return result to the stack
            stack.append(new EPListItem(value));
            }
        }
        // if it's a number, push it to stack
        else
        {
            stack.append(converted[i]);
        }
    }
    // after vector processing, top of the stack should have the answer
    if (!stack.isEmpty())
    {
        double result = stack.top()->getValue();
        return QString::number(result);
    }
    else
    {
        return "Runtime error";
    }
}
// calls tokenize, convert and eval in succession to calculate an expression
QString ExpressionParser::calc(const QString &expr)
{
    QString value;
    try
    {
        vec* v = tokenize(expr);
        vec* c = convert(v);
        value = eval(*c);
    }
    catch(UnknownStringError)
    {
        return "Evaluation error";
    }

    return value;
}


// EPListItem methods below
// double constructor
ExpressionParser::EPListItem::EPListItem(double value)
{
    double_value = value;
    type = N;
}
// token constructor
ExpressionParser::EPListItem::EPListItem(TOKEN token)
{
    token_value = token;
    type = T;
}
// value getters
TOKEN ExpressionParser::EPListItem::getToken()
{
    if (type == T)
        return token_value;
    else
        return INVALID_TOKEN;
}

double ExpressionParser::EPListItem::getValue()
{
    if (type == N)
        return double_value;
    else
        return 0;
}
// some checks
bool ExpressionParser::EPListItem::isNumber()
{
    return type == N;
}

bool ExpressionParser::EPListItem::isToken()
{
    return type == T;
}

bool ExpressionParser::EPListItem::isOperator()
{
    return (isToken() && token_value != PAR_LEFT && token_value != PAR_RIGHT);
}

bool ExpressionParser::EPListItem::isBinaryOperator()
{
    return (token_value >= OP_PLUS && token_value <= OP_DIVIDE);
}
