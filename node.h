#ifndef _NODE
#define _NODE
#include<memory>
#include<string>
#include<vector>
#include<map>
#include<iostream>
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

class AST
{
       public:
            virtual ~AST() = default;
            virtual void output() = 0;
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
        void output()
        {
            std::cout<<"Prototype "<<*id<<"( ";
            for(auto& para: *paras)
                    std::cout<<" " <<*para;
            std::cout<<" )\n";
        }

        virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                        std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
};

class ExprAST:public AST
{
        public:
            virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls) = 0;
};

class LocalVarExprAST: public AST
{
        public:
              std::unique_ptr<std::string> id;
              std::unique_ptr<ExprAST> expr;
              
              LocalVarExprAST(std::unique_ptr<std::string> id, std::unique_ptr<ExprAST> expr):id(std::move(id)), expr(std::move(expr)){}
              void gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
              void output()
              {
                std::cout<<"local var: "<<*id<<std::endl;
                expr->output();
              }
};

class AssignExprAST: public ExprAST
{
    public:
    std::unique_ptr<std::string> id;
    std::unique_ptr<ExprAST> expr;
    AssignExprAST(std::unique_ptr<std::string> id, std::unique_ptr<ExprAST> expr) : id(std::move(id)), expr(std::move(expr))
    {}
    
    void output()
    {
        std::cout<<*id <<" = ";
        expr->output();
        std::cout<<std::endl;
    }
    virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
};



class NumberExprAST : public ExprAST
{
        double val;
        public:
            NumberExprAST(double val) : val(val) {}
        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
        void output()
        {
            std::cout<<"num: "<<val<<std::endl;
        }
};

class VariableExprAST : public ExprAST
{
        std::unique_ptr<std::string> id;
        public:
            VariableExprAST(std::unique_ptr<std::string> id) : id(std::move(id)) {}
        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
        void output()
        {
            std::cout<<"variable: "<<*id<<std::endl;
        }
};

class BinOpExprAST: public ExprAST
{
        char op;
        std::unique_ptr<ExprAST> left;
        std::unique_ptr<ExprAST> right;
        public:
            BinOpExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): 
                    op(op), left(std::move(lhs)), right(std::move(rhs)) {}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
        void output()
        {
            left->output();
            std::cout<<" " <<op<<" ";
            right->output();
        }
};

class IfExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        public:
            IfExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then): 
                    condition(std::move(condi)), then_brach(std::move(then)){}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
        void output()
        {
            std::cout<<" IF " <<std::endl;
            condition->output();
            std::cout<<" THEN "<<std::endl;
            then_brach->output();
        }
};

class IfElseExprAST: public ExprAST
{
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> then_brach;
        std::unique_ptr<ExprAST> else_brach;
        public:
            IfElseExprAST(std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> then, std::unique_ptr<ExprAST> _else):
                    condition(std::move(condi)), then_brach(std::move(then)), else_brach(std::move(_else)) {}

        virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);

        void output()
        {
            std::cout<<"IF"<<std::endl;
            condition->output();
            std::cout<<"THEN"<<std::endl;
            then_brach->output();
            std::cout<<"ELSE"<<std::endl;
            else_brach->output();
            std::cout<<std::endl;
        }
};

class CallExprAST:  public ExprAST
{
        std::unique_ptr<std::string> id;
        std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs;
        public:
            CallExprAST(std::unique_ptr<std::string> id, std::unique_ptr<std::vector<std::unique_ptr<ExprAST>>> exprs): id(std::move(id)), exprs(std::move(exprs))
            {
            }

            virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);

            void output()
            {
                std::cout<<"function call " << *id << " ( ";
                for(auto& expr : *exprs)
                        expr->output();
            }
};

class ForExprAST: public ExprAST
{
        std::unique_ptr<std::string> id;
        std::unique_ptr<ExprAST> init;
        std::unique_ptr<ExprAST> condition;
        std::unique_ptr<ExprAST> step;
        std::unique_ptr<ExprAST> body;
        public:
            ForExprAST(std::unique_ptr<std::string> id, std::unique_ptr<ExprAST> init, std::unique_ptr<ExprAST> condi, std::unique_ptr<ExprAST> step, std::unique_ptr<ExprAST> body):
                    id(std::move(id)), init(std::move(init)), condition(std::move(condi)), step(std::move(step)), body(std::move(body)){}

            virtual llvm::Value* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);

           void output()
           {} 
};

class DeclAST:public AST
{
        private:
                std::unique_ptr<PrototypeAST> decl;
        public:
                DeclAST(std::unique_ptr<PrototypeAST> decl): decl(std::move(decl)){}
                virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                                                std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);
                void output()
                {}

};


class FuncAST: public AST
{
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<std::vector<std::unique_ptr<LocalVarExprAST>>> locals;
    std::unique_ptr<ExprAST> expr;

    public:
    FuncAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<std::vector<std::unique_ptr<LocalVarExprAST>>> locals, std::unique_ptr<ExprAST> expr):
            proto(std::move(proto)), expr(std::move(expr)), locals(std::move(locals)){}

    virtual llvm::Function* gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                                    std::map<std::string, llvm::AllocaInst*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls);

    void output()
    {
        proto->output();
        if(locals != nullptr)
        {
            for(auto& local: *locals)
                local->output();
        }
        std::cout<<std::endl;
        expr->output();
    }
};
#endif
