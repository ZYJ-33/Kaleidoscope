#include "./node.h"
#include "./ast.tab.hh"

llvm::Function* getFunction(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, std::map<std::string, llvm::Value*>* symtab,
                            std::map<std::string, std::unique_ptr<PrototypeAST>>* decls, const std::string& name)
{
        llvm::Function* func = mod->getFunction(name);
        if(func == nullptr)
        {
            auto res = decls->find(name);
            if(res != decls->end())
                    return res->second->gencode(ctx, mod, builder, symtab, decls);
            return nullptr;
            
        }
        return func;
}

llvm::Value* logerror(const char* error)
{
    std::cerr<<"error: "<<error<<std::endl;
    return nullptr;
}

llvm::Value* AssignExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                    std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    return nullptr;
}

llvm::Value* NumberExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                                    std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
     return llvm::ConstantFP::get(*ctx, llvm::APFloat(val));
}

llvm::Value* VariableExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                      std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
        llvm::Value* val = (*symtab)[*id];
        if(val == nullptr)
            return logerror("undefined variable");
        return val;
}

llvm::Value* BinOpExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                   std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
     llvm::Value* left_res = left->gencode(ctx, mod, builder, symtab, decls);
     llvm::Value* right_res = right->gencode(ctx, mod, builder, symtab, decls);
     
     switch(op)
     {
             case '+':
                return builder->CreateFAdd(left_res, right_res, "add_tmp");
             case '-':
                return builder->CreateFSub(left_res, right_res, "sub_tmp");
             case '*':
                return builder->CreateFMul(left_res, right_res, "mul_tmp");
             case '/':
                return builder->CreateFDiv(left_res, right_res, "div_tmp");
             case '&':
             case '|':
             case 'E':
             case 'U':
                std::cout<<"some bin operator not impl yet\n";
                return nullptr;
     }
     return nullptr;
}

llvm::Value* IfExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    return nullptr;
}

llvm::Value* IfElseExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                    std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    return nullptr;
}

llvm::Value* CallExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                                  std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    llvm::Function* callee = getFunction(ctx, mod, builder, symtab, decls, *id);
    if(callee == nullptr)
            return logerror("calling a undef func");
    if(callee->arg_size() != exprs->size())
            return logerror("args did not match");
    
    std::vector<llvm::Value*> argvs;

    for(uint64_t i=0;i<exprs->size(); i++)
    {
        auto expr_ptr = (*exprs)[i].get();
        argvs.push_back(expr_ptr->gencode(ctx, mod, builder, symtab, decls));
    }

    return builder->CreateCall(callee, argvs, "calltmp");
}

llvm::Function* PrototypeAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                                      std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    std::vector<llvm::Type*> arg_types(paras->size(), llvm::Type::getDoubleTy(*ctx));
    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(*ctx), arg_types, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, *(id.get()), mod);
    uint32_t i = 0;
    for(auto& arg: func->args())
            arg.setName(*((*paras)[i++]));
    return func;
}

llvm::Function* DeclAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                 std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
    auto save = decl->gencode(ctx, mod, builder, symtab, decls);    
    decls->insert(std::pair<std::string, std::unique_ptr<PrototypeAST>>(decl->get_id(), std::move(decl)));
    return save;
}

llvm::Function* FuncAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder, 
                std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
        PrototypeAST& proto_ref = *proto;
        decls->insert(std::pair<std::string, std::unique_ptr<PrototypeAST>>(proto_ref.get_id(), std::move(proto)));

        llvm::Function* func = getFunction(ctx, mod, builder, symtab, decls, proto_ref.get_id());

        llvm::BasicBlock* bb = llvm::BasicBlock::Create(*ctx, "entry", func);
        builder->SetInsertPoint(bb);

        for(auto& arg : func->args())
            (*symtab)[std::string(arg.getName())] = &arg;

        llvm::Value* ret = expr->gencode(ctx, mod, builder, symtab, decls);
        symtab->clear();

        if(ret != nullptr)
        {
            builder->CreateRet(ret);
            llvm::verifyFunction(*func);
            return func;
        }
        func->eraseFromParent();
        return nullptr;
}

