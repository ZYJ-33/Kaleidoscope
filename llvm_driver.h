#ifndef LLVMDRIVER
#define LLVMDRIVER
#include <map>
#include "./node.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Error.h"
#include "llvm/Target/TargetMachine.h"
#include "./KaleidoscopeJIT.h"
#include <iostream>
#include <cstdio>

extern "C" double print_double(double x)
{
    fputc(x, stdout);
    return 0;
}

class LLVM_Core_Context
{
    public:
        std::unique_ptr<llvm::LLVMContext> ctx;
        std::unique_ptr<llvm::Module> mod;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        LLVM_Core_Context()
        {
            ctx = std::make_unique<llvm::LLVMContext>();
            mod = std::make_unique<llvm::Module>("zyj_KS", *ctx);
            builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
        }
};

class LLVM_Driver
{
    public:
            std::vector<LLVM_Core_Context> ctxs;
            std::map<std::string, llvm::Value*> symtab;
            std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>> funcs; 
            std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls;
            std::map<std::string, std::unique_ptr<PrototypeAST>> func_decls;
            llvm::ExitOnError ExitOnErr;
            std::unique_ptr<llvm::orc::KaleidoscopeJIT> JIT; 

            LLVM_Driver(std::unique_ptr<std::vector<std::unique_ptr<DeclAST>>> decls, 
                        std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>> funcs): funcs(std::move(funcs)),
                                                                                       decls(std::move(decls))
            {
                    llvm::InitializeNativeTarget();
                    llvm::InitializeNativeTargetAsmPrinter();
                    llvm::InitializeNativeTargetAsmParser();
                    JIT = ExitOnErr(llvm::orc::KaleidoscopeJIT::Create()); 

                    ctxs.emplace_back();
                    LLVM_Core_Context& last_ctx = ctxs[ctxs.size()-1];

                    for(auto cur = this->decls->begin(); cur != this->decls->end(); cur++)
                        (*(*cur)).gencode((last_ctx.ctx.get()), (last_ctx.mod.get()), (last_ctx.builder.get()), &symtab, &func_decls);

                    for(auto cur = this->funcs->begin(); cur != this->funcs->end(); cur++)
                    {
                        ctxs.emplace_back();
                        LLVM_Core_Context& last_ctx = ctxs[ctxs.size()-1];
                        (*(*cur)).gencode((last_ctx.ctx.get()), (last_ctx.mod.get()), (last_ctx.builder.get()), &symtab, &func_decls);
                        last_ctx.mod->print(llvm::outs(), nullptr); 
                        ExitOnErr(JIT->addModule(
                                      llvm::orc::ThreadSafeModule(std::move(last_ctx.mod), std::move(last_ctx.ctx))
                                 ));
                    }
                    
                                        
                    auto main_func = ExitOnErr(JIT->lookup("main"));
                    double (*main)() = (double (*)()) (uint64_t) main_func.getAddress();
                    std::cout<< "res: "<< (*main)()<< std::endl;
//                    mod.print(llvm::outs(), nullptr);
            }
};

#endif

