#ifndef _NODE
#define _NODE
#include<memory>
#include<string>
#include<vector>

class AST
{
       public:
            virtual ~AST() = default;
};

class ExprAST:public AST
{

};

class AssignExprAST: public ExprAST
{
    std::unique_ptr<std::string> id;
    std::unique_ptr<ExprAST> expr;
    AssignExprAST(std::unique_ptr<std::string> id, std::unique_ptr<ExprAST> expr) : id(std::move(id)), expr(std::move(expr))
    {}
};

class NumberExprAST : public ExprAST
{
        double val;
        public:
            NumberExprAST(double val) : val(val) {}
};

class VariableExprAST : public ExprAST
{
        std::unique_ptr<std::string> id;
        public:
            VariableExprAST(std::unique_ptr<std::string> id) : id(std::move(id)) {}
};

class BinOpExprAST: public ExprAST
{
        char op;
        std::unique_ptr<ExprAST> left;
        std::unique_ptr<ExprAST> right;
        public:
            BinOpExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): 
                    op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

class IfExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        public:
            IfExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then): 
                    condition(std::move(condi)), then_brach(std::move(then)){}
};

class IfElseExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        std::unique_ptr<ExprAST> else_brach;
        public:
            IfElseExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then, std::unique_ptr<ExprAST> _else):
                    condition(std::move(condi)), then_brach(std::move(then)), else_brach(std::move(_else)) {}
};

class CallExprAST:  public ExprAST
{
        std::unique_ptr<std::string> id;
        std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs;
        public:
            CallExprAST(std::unique_ptr<std::string> id, std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs): id(std::move(id)), exprs(std::move(exprs))
            {
            }
};

class PrototypeAST : public AST
{
    std::unique_ptr<std::string> id;
    std::unique_ptr<std::vector<std::unique_ptr<std::string>>> paras;
    public:
        PrototypeAST(std::unique_ptr<std::string> id, std::unique_ptr<std::vector<std::unique_ptr<std::string>>> paras) :id(std::move(id)), paras(std::move(paras))
        {
        }
};

class FuncAST: public AST
{
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> expr;

    public:
    FuncAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> expr):
            proto(std::move(proto)), expr(std::move(expr))
    {}
};
#endif
