#ifndef LLVMDRIVER
#define LLVMDRIVER
#include <map>
#include "./node.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "iostream"

class LLVM_Driver
{
    public:
            llvm::LLVMContext ctx;
            llvm::Module mod;
            llvm::IRBuilder<> builder;
            std::map<std::string, llvm::Value*> symtab;
            std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>> funcs; 
            std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls;

            LLVM_Driver(std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls, 
                        std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>> funcs): mod("zyj_KS", ctx),
                                                                                       builder(ctx),
                                                                                       funcs(std::move(funcs)),
                                                                                       decls(std::move(decls))
            {
                    for(auto cur = this->decls->begin(); cur != this->decls->end(); cur++)
                       (*(*cur)).gencode(&ctx, &mod, &builder, &symtab);
                    for(auto cur = this->funcs->begin(); cur != this->funcs->end(); cur++)
                        (*(*cur)).gencode(&ctx, &mod, &builder, &symtab);
                mod.print(llvm::outs(), nullptr);
            }
};

#endif

