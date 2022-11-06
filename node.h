#ifndef _NODE
#define _NODE
#include<memory>
#include<string>
#include<vector>
#include<map>
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

class AST
{
       public:
            virtual ~AST() = default;
};

class ExprAST:public AST
{
        public:
            virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab) = 0;
};


class AssignExprAST: public ExprAST
{
    public:
    std::unique_ptr<std::string> id;
    std::unique_ptr<ExprAST> expr;
    AssignExprAST(std::unique_ptr<std::string> id, std::unique_ptr<ExprAST> expr) : id(std::move(id)), expr(std::move(expr))
    {}
    virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class NumberExprAST : public ExprAST
{
        double val;
        public:
            NumberExprAST(double val) : val(val) {}
        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class VariableExprAST : public ExprAST
{
        std::unique_ptr<std::string> id;
        public:
            VariableExprAST(std::unique_ptr<std::string> id) : id(std::move(id)) {}
        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class BinOpExprAST: public ExprAST
{
        char op;
        std::unique_ptr<ExprAST> left;
        std::unique_ptr<ExprAST> right;
        public:
            BinOpExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): 
                    op(op), left(std::move(lhs)), right(std::move(rhs)) {}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);

};

class IfExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        public:
            IfExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then): 
                    condition(std::move(condi)), then_brach(std::move(then)){}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class IfElseExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        std::unique_ptr<ExprAST> else_brach;
        public:
            IfElseExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then, std::unique_ptr<ExprAST> _else):
                    condition(std::move(condi)), then_brach(std::move(then)), else_brach(std::move(_else)) {}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class CallExprAST:  public ExprAST
{
        std::unique_ptr<std::string> id;
        std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs;
        public:
            CallExprAST(std::unique_ptr<std::string> id, std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs): id(std::move(id)), exprs(std::move(exprs))
            {
            }

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class PrototypeAST : public AST
{
    std::unique_ptr<std::string> id;
    std::unique_ptr<std::vector<std::unique_ptr<std::string>>> paras;
    public:
        PrototypeAST(std::unique_ptr<std::string> id, std::unique_ptr<std::vector<std::unique_ptr<std::string>>> paras) :id(std::move(id)), paras(std::move(paras))
        {
        }

        const std::string& get_id()
        {
            return *id;
        }

        virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};

class DeclAST:public AST
{
        private:
                std::unique_ptr<PrototypeAST> decl;
        public:
                DeclAST(std::unique_ptr<PrototypeAST> decl): decl(std::move(decl)){}
                virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);

};


class FuncAST: public AST
{
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> expr;

    public:
    FuncAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> expr):
            proto(std::move(proto)), expr(std::move(expr))
    {}

        virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab);
};
#endif
