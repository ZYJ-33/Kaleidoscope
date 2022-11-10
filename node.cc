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
             case '<':
                {
                    auto boolres = builder->CreateFCmpULT(left_res, right_res, "less_tmp");
                    return builder->CreateUIToFP(boolres, llvm::Type::getDoubleTy(*ctx), "bool2float");
                }
             case '&':
                {
                    auto left_bool = builder->CreateFCmpONE(left_res, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "to_left_bool");
                    auto right_bool = builder->CreateFCmpONE(right_res, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "to_right_bool");
                    auto boolres = builder->CreateLogicalAnd(left_bool, right_bool, "logic_and");
                    return builder->CreateUIToFP(boolres, llvm::Type::getDoubleTy(*ctx), "bool2float");
                }
             case '|':
                {
                    auto left_bool = builder->CreateFCmpONE(left_res, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "to_left_bool");
                    auto right_bool = builder->CreateFCmpONE(right_res, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "to_right_bool");
                    auto boolres = builder->CreateLogicalOr(left_bool, right_bool, "logic_or");
                    return builder->CreateUIToFP(boolres, llvm::Type::getDoubleTy(*ctx), "bool2float");

                }
             case 'E':
                {
                   auto boolres = builder->CreateFCmpUEQ(left_res, right_res, "eq_tmp");
                   return builder->CreateUIToFP(boolres, llvm::Type::getDoubleTy(*ctx), "bool2float");
                }
             case 'U':
                std::cout<<"some bin operator not impl yet\n";
                return nullptr;
     }
     return nullptr;
}

llvm::Value* IfExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
       llvm::Value* condi = condition->gencode(ctx, mod, builder, symtab, decls);
       condi = builder->CreateFCmpONE(condi, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "cmp");

       llvm::Function* cur_func = builder->GetInsertBlock()->getParent();
       llvm::BasicBlock* cur_block = builder->GetInsertBlock();

       llvm::BasicBlock* then = llvm::BasicBlock::Create(*ctx, "then", cur_func);
       llvm::BasicBlock* merge = llvm::BasicBlock::Create(*ctx, "merge", cur_func);
       llvm::Value* else_value = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0));

       builder->CreateCondBr(condi, then, merge);
       
        
       builder->SetInsertPoint(then);
       llvm::Value* then_value = then_brach->gencode(ctx, mod, builder, symtab, decls);
       builder->CreateBr(merge);
       then = builder->GetInsertBlock();

       builder->SetInsertPoint(merge);
       llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*ctx), 2, "phi"); 
       phi->addIncoming(then_value, then);
       phi->addIncoming(else_value, cur_block);
       
       return phi;
}

llvm::Value* IfElseExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                    std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
        llvm::Value* condi = condition->gencode(ctx, mod, builder, symtab, decls);
        
        condi = builder->CreateFCmpONE(condi, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "cmp");

        llvm::Function* cur_func = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock * then = llvm::BasicBlock::Create(*ctx, "then", cur_func);
        llvm::BasicBlock* _else = llvm::BasicBlock::Create(*ctx, "else", cur_func);
        llvm::BasicBlock* merge = llvm::BasicBlock::Create(*ctx, "merge", cur_func);

        builder->CreateCondBr(condi, then, _else);
        
        builder->SetInsertPoint(then);
        llvm::Value* then_value = then_brach->gencode(ctx, mod, builder, symtab, decls);
        builder->CreateBr(merge);
        then = builder->GetInsertBlock();
        
        builder->SetInsertPoint(_else);
        llvm::Value* else_value = else_brach->gencode(ctx, mod, builder, symtab, decls);
        builder->CreateBr(merge);
        _else = builder->GetInsertBlock();

        builder->SetInsertPoint(merge);
        llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*ctx), 2, "phi");
        phi->addIncoming(then_value, then);
        phi->addIncoming(else_value, _else);

        return phi;
}

llvm::Value* ForExprAST::gencode(llvm::LLVMContext* ctx, llvm::Module* mod, llvm::IRBuilder<>* builder,
                                    std::map<std::string, llvm::Value*>* symtab, std::map<std::string, std::unique_ptr<PrototypeAST>>* decls)
{
        llvm::Function* cur_func = builder->GetInsertBlock()->getParent();
        llvm::BasicBlock* condi_block = llvm::BasicBlock::Create(*ctx, "for_condi", cur_func);
        llvm::BasicBlock* body_block = llvm::BasicBlock::Create(*ctx, "for_body", cur_func);
        llvm::BasicBlock* out_block = llvm::BasicBlock::Create(*ctx, "for_out", cur_func);

        llvm::Value* init_val = init->gencode(ctx, mod, builder, symtab, decls);
        llvm::BasicBlock* initblock_end = builder->GetInsertBlock();
        builder->CreateBr(condi_block);


        builder->SetInsertPoint(condi_block);
        llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*ctx), 2, "for_condi_phi");
        phi->addIncoming(init_val, initblock_end);

        llvm::Value* old = (*symtab)[*id];
        (*symtab)[*id] = phi;

        llvm::Value* condi = condition->gencode(ctx, mod, builder, symtab, decls);
        condi = builder->CreateFCmpONE(condi, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*ctx), llvm::APFloat(0.0)), "for_condi_cmp");
        builder->CreateCondBr(condi, body_block, out_block);


        builder->SetInsertPoint(body_block);
        body->gencode(ctx, mod, builder, symtab, decls);
        llvm::Value* howmuch = step->gencode(ctx, mod, builder, symtab, decls);
        llvm::Value* step_val = builder->CreateFAdd(phi, howmuch, "for_step_add");

        llvm::BasicBlock* bodyblock_end = builder->GetInsertBlock();
        phi->addIncoming(step_val, bodyblock_end);

        builder->CreateBr(condi_block);
         

        builder->SetInsertPoint(out_block);
        if(old)
                (*symtab)[*id] = old;
        else
                symtab->erase(*id);
        return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*ctx));
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

