#include "llvmUtils.h"

#include <sys/stat.h>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#if D_LLVM_VERSION_ORD >= 36
#include "llvm/Transforms/Utils/Cloning.h"
#endif

#if D_LLVM_VERSION_ORD <= 32
#include "llvm/Support/Path.h"
#endif

#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"

#include "../Units/Units.h"

namespace dale {
std::string getTriple() {
#if D_LLVM_VERSION_ORD >= 32
    return llvm::sys::getDefaultTargetTriple();
#else
    return llvm::sys::getHostTriple();
#endif
}

void linkModule(llvm::Linker *linker, llvm::Module *mod) {
    std::string error;
    bool result;
#if D_LLVM_VERSION_ORD <= 32
    result = linker->LinkInModule(mod, &error);
#elif D_LLVM_VERSION_ORD <= 35
    result = linker->linkInModule(mod, &error);
#elif D_LLVM_VERSION_ORD <= 37
    result = linker->linkInModule(mod);
#elif D_LLVM_VERSION_ORD <= 60
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(mod));
    result = linker->linkInModule(move(module_ptr));
#else
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(*mod));
    result = linker->linkInModule(move(module_ptr));
#endif
    assert(!result && "unable to link module");
    _unused(result);
}

void addDataLayout(PassManager *pass_manager, llvm::Module *mod) {
#if D_LLVM_VERSION_ORD >= 37
#elif D_LLVM_VERSION_ORD >= 36
    pass_manager->add(new llvm::DataLayoutPass());
#elif D_LLVM_VERSION_ORD >= 35
    pass_manager->add(new llvm::DataLayoutPass(mod));
#elif D_LLVM_VERSION_ORD >= 32
    pass_manager->add(new llvm::DataLayout(mod));
#else
    pass_manager->add(new llvm::TargetData(mod));
#endif
}

#if D_LLVM_VERSION_ORD <= 34
std::auto_ptr<llvm::TargetMachine> target_sp;
#else
std::shared_ptr<llvm::TargetMachine> target_sp;
#endif
llvm::TargetMachine *getTargetMachine(llvm::Module *last_module) {
    llvm::Triple triple(last_module->getTargetTriple());
    if (triple.getTriple().empty()) {
        triple.setTriple(getTriple());
    }

    std::string Err;
    const llvm::Target *target =
        llvm::TargetRegistry::lookupTarget(triple.getTriple(), Err);
    assert(target && "cannot auto-select target for module");

#if D_LLVM_VERSION_ORD >= 32
    llvm::TargetOptions target_options;
#endif

    std::string Features;
    target_sp =
#if D_LLVM_VERSION_ORD <= 34
        std::auto_ptr<llvm::TargetMachine>
#else
        std::shared_ptr<llvm::TargetMachine>
#endif
        (target->createTargetMachine(
            triple.getTriple(), llvm::sys::getHostCPUName(), Features
#if D_LLVM_VERSION_ORD >= 32
            ,
            target_options
#endif
#if D_LLVM_VERSION_ORD >= 39
            ,
            llvm::Optional<llvm::Reloc::Model>()
#endif
                ));  // NOLINT

    llvm::TargetMachine *tm = target_sp.get();
#if D_LLVM_VERSION_ORD <= 35
    tm->setAsmVerbosityDefault(true);
#endif
    return tm;
}

void setDataLayout(llvm::Module *module, bool is_x86_64) {
#if D_LLVM_VERSION_ORD <= 36
    static const char *x86_64_layout =
        "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:"
        "32:32-"
        "f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-"
        "n8:16:"
        "32:64-S128";
    static const char *x86_32_layout =
        "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:"
        "32:32-"
        "f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32";
    module->setDataLayout((is_x86_64) ? x86_64_layout : x86_32_layout);
#else
    llvm::TargetMachine *target_machine = getTargetMachine(module);
    module->setDataLayout(target_machine->createDataLayout());
#endif
}

void populateLTOPassManager(
    llvm::PassManagerBuilder *pass_manager_builder,
    PassManager *pass_manager) {
#if D_LLVM_VERSION_ORD <= 35
    pass_manager_builder->populateLTOPassManager(*pass_manager, true,
                                                 true);
#else
    pass_manager_builder->populateLTOPassManager(*pass_manager);
#endif
}

llvm_formatted_ostream *getFormattedOstream(
    llvm::raw_fd_ostream *ostream) {
#if D_LLVM_VERSION_ORD <= 36
    llvm_formatted_ostream *ostream_formatted =
        new llvm::formatted_raw_ostream(
            *ostream, llvm::formatted_raw_ostream::DELETE_STREAM);
    return ostream_formatted;
#else
    return ostream;
#endif
}

void addPrintModulePass(PassManager *pass_manager,
                        llvm::raw_fd_ostream *ostream) {
#if D_LLVM_VERSION_ORD <= 34
    pass_manager->add(llvm::createPrintModulePass(ostream));
#else
    pass_manager->add(llvm::createPrintModulePass(*ostream));
#endif
}

void moduleDebugPass(llvm::Module *mod) {
#if D_LLVM_VERSION_ORD <= 40
    mod->dump();
#else
    mod->print(llvm::outs(), nullptr);
#endif
#if D_LLVM_VERSION_ORD >= 35
    if (llvm::verifyModule(*mod, &(llvm::errs()))) {
        abort();
    }
#endif
}

void functionDebugPass(llvm::Function *fn) {
#if D_LLVM_VERSION_ORD >= 35
    llvm::dbgs() << *fn << "\n";
    if (llvm::verifyFunction(*fn, &(llvm::errs()))) {
        abort();
    }
#endif
}

llvm::BasicBlock::iterator instructionToIterator(
    llvm::Instruction *inst) {
#if D_LLVM_VERSION_ORD <= 37
    llvm::BasicBlock::iterator bi = inst;
    return bi;
#else
    llvm::BasicBlock::iterator bi = inst->getIterator();
    return bi;
#endif
}

void setInsertPoint(llvm::IRBuilder<> *builder,
                    llvm::BasicBlock::iterator iter) {
#if D_LLVM_VERSION_ORD <= 37
    builder->SetInsertPoint(iter);
#else
    builder->SetInsertPoint(&*iter);
#endif
}

uint64_t variableToAddress(llvm::ExecutionEngine *ee, Variable *var) {
#if D_LLVM_VERSION_ORD <= 35
    return (uint64_t)ee->getPointerToGlobal(
        llvm::cast<llvm::GlobalValue>(var->value));
#else
    return ee->getGlobalValueAddress(var->symbol.c_str());
#endif
}

uint64_t functionToAddress(Unit *unit, Function *fn) {
#if D_LLVM_VERSION_ORD <= 35
    Context *ctx = unit->ctx;
    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_pvoid, nullNode(), false);
    if (!llvm_return_type) {
        return 0;
    }
    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);
    std::string new_name;
    unit->getUnusedFunctionName(&new_name);

    llvm::Function *llvm_fn = llvm::Function::Create(
        ft, ctx->toLLVMLinkage(Linkage::Extern_C),
        new_name.c_str(), unit->module);
    llvm_fn->setCallingConv(llvm::CallingConv::C);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", llvm_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(fn->llvm_function);
    builder.CreateRet(llvm::cast<llvm::Value>(fn->llvm_function));
    std::vector<llvm::GenericValue> values;
    llvm::GenericValue res = unit->ee->runFunction(llvm_fn, values);
    uint64_t address = (uint64_t)res.PointerVal;
    llvm_fn->eraseFromParent();
#else
    uint64_t address =
        unit->ee->getGlobalValueAddress(fn->symbol.c_str());
#endif
    return address;
}

void cloneModuleIfRequired(Unit *unit) {
#if D_LLVM_VERSION_ORD >= 36
    std::vector<Function *> global_functions;
    while (Function *globfn = unit->getGlobalFunction()) {
        global_functions.push_back(globfn);
        if (llvm::Function *gfn = globfn->llvm_function) {
            gfn->removeFromParent();
        }
        unit->popGlobalFunction();
    }
#if D_LLVM_VERSION_ORD == 36
    std::unique_ptr<llvm::Module> module_ptr(
        llvm::CloneModule(unit->module));
    unit->ee->addModule(move(module_ptr));
#elif D_LLVM_VERSION_ORD == 37
    std::unique_ptr<llvm::Module> module_ptr(
        llvm::CloneModule(unit->module));
    unit->ee->addModule(move(module_ptr));
#elif D_LLVM_VERSION_ORD <= 60
    unit->ee->addModule(llvm::CloneModule(unit->module));
#else
    unit->ee->addModule(llvm::CloneModule(*(unit->module)));
#endif
    for (std::vector<Function *>::reverse_iterator
             b = global_functions.rbegin(),
             e = global_functions.rend();
         b != e; ++b) {
        Function *globfn = *b;
        if (llvm::Function *gfn = globfn->llvm_function) {
            unit->module->getFunctionList().push_back(gfn);
        }
        unit->pushGlobalFunction(globfn);
    }
#endif
}

void setStandardAttributes(llvm::Function *fn) {
#if D_LLVM_VERSION_ORD == 32
    fn->addFnAttr(llvm::Attributes::NoUnwind);
    fn->addFnAttr(llvm::Attributes::ReadOnly);
    fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    fn->addFnAttr(llvm::Attribute::ReadOnly);
    fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
}

void addInlineAttribute(llvm::Function *fn) {
#if D_LLVM_VERSION_ORD == 32
    fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
    fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
}

llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type *> const &v,
                                    bool b) {
    llvm::ArrayRef<llvm::Type *> array_ref(v);
    return llvm::FunctionType::get(t, array_ref, b);
}

llvm::Constant *getStringConstantArray(const char *data) {
    return llvm::cast<llvm::Constant>(
#if D_LLVM_VERSION_ORD < 32
        llvm::ConstantArray::get(
#else
        llvm::ConstantDataArray::getString(
#endif
            *getContext(), data, true));
}

llvm::ConstantPointerNull *getNullPointer(llvm::Type *type) {
    return llvm::ConstantPointerNull::get(
        llvm::cast<llvm::PointerType>(type));
}

void linkFile(llvm::Linker *linker, const char *path) {
#if D_LLVM_VERSION_ORD <= 32
    const llvm::sys::Path bb(path);
    bool is_native = false;
    bool res = linker->LinkInFile(bb, is_native);
    assert(!res && "unable to link bitcode file");
#elif D_LLVM_VERSION_ORD <= 35
    llvm::SMDiagnostic sm_error;
    llvm::Module *path_mod =
        llvm::ParseIRFile(path, sm_error, *getContext());
    std::string error;
    bool res = linker->linkInModule(path_mod, &error);
    assert(!res && "unable to link bitcode file module");
#elif D_LLVM_VERSION_ORD <= 37
    llvm::SMDiagnostic sm_error;
    std::unique_ptr<llvm::Module> module_ptr(
        llvm::parseIRFile(path, sm_error, *getContext()));
    bool res = linker->linkInModule(module_ptr.get());
    assert(!res && "unable to link bitcode file module");
#else
    llvm::SMDiagnostic sm_error;
    std::unique_ptr<llvm::Module> module_ptr(
        llvm::parseIRFile(path, sm_error, *getContext()));
    bool res = linker->linkInModule(move(module_ptr));
    assert(!res && "unable to link bitcode file module");
#endif
    _unused(res);
}

llvm::Linker *newLinker(const char *path, llvm::Module *mod) {
#if D_LLVM_VERSION_ORD <= 32
    return new llvm::Linker(path, mod, false);
#elif D_LLVM_VERSION_ORD <= 37
    return new llvm::Linker(mod);
#else
    return new llvm::Linker(*mod);
#endif
}

llvm::Module *loadModule(std::string *path) {
#if D_LLVM_VERSION_ORD <= 34
    llvm::OwningPtr<llvm::MemoryBuffer> buffer;
#endif

#if D_LLVM_VERSION_ORD <= 33
    const llvm::sys::Path sys_path(*path);
    llvm::MemoryBuffer::getFileOrSTDIN(sys_path.c_str(), buffer);
#elif D_LLVM_VERSION_ORD <= 34
    llvm::MemoryBuffer::getFileOrSTDIN(*path, buffer);
#else
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer> > eo_buffer =
        llvm::MemoryBuffer::getFileOrSTDIN(*path);
    assert(!eo_buffer.getError() && "cannot load module");
    std::unique_ptr<llvm::MemoryBuffer> buffer =
        std::move(eo_buffer.get());
#endif

#if D_LLVM_VERSION_ORD <= 34
    std::string error_msg;
    llvm::Module *module = llvm::getLazyBitcodeModule(
        buffer.get(), *getContext(), &error_msg);
#elif D_LLVM_VERSION_ORD <= 35
    std::string error_msg;
    llvm::ErrorOr<llvm::Module *> eo_module =
        llvm::getLazyBitcodeModule(buffer.get(), *getContext());
    llvm::Module *module = eo_module.get();
    if (!module) {
        error_msg = eo_module.getError().message();
    }
    buffer.release();
#elif D_LLVM_VERSION_ORD <= 36
    std::string error_msg;
    llvm::ErrorOr<llvm::Module *> eo_module =
        llvm::getLazyBitcodeModule(move(buffer), *getContext());
    llvm::Module *module = eo_module.get();
    if (!module) {
        error_msg = eo_module.getError().message();
    }
    buffer.release();
#elif D_LLVM_VERSION_ORD <= 39
    std::string error_msg;
    llvm::ErrorOr<std::unique_ptr<llvm::Module> > eo_module =
        llvm::getLazyBitcodeModule(move(buffer), *getContext());
    llvm::Module *module = eo_module.get().get();
    if (!module) {
        error_msg = eo_module.getError().message();
    }
    buffer.release();
#else
    std::string error_msg("Unable to load module");
    llvm::Expected<std::unique_ptr<llvm::Module> > e_module =
        llvm::getLazyBitcodeModule(buffer->getMemBufferRef(),
                                   *getContext());
    if (!e_module) {
        fprintf(stderr, "Unable to load module");
        abort();
    }
    llvm::Module *module = e_module.get().get();
    buffer.release();
#endif

    assert(module && "cannot load module");

#if D_LLVM_VERSION_ORD <= 34
    bool materialized = module->MaterializeAll(&error_msg);
#elif D_LLVM_VERSION_ORD <= 35
    std::error_code ec = module->materializeAllPermanently();
    bool materialized = static_cast<bool>(ec);
    if (ec) {
        error_msg = ec.message();
    }
#elif D_LLVM_VERSION_ORD <= 39
    std::error_code ec = module->materializeAll();
    bool materialized = static_cast<bool>(ec);
    if (ec) {
        error_msg = ec.message();
    }
#else
    llvm::Error ec = module->materializeAll();
    bool materialized = static_cast<bool>(ec);
#endif
    assert(!materialized && "failed to materialize module");
    _unused(materialized);

    return module;
}

llvm::LLVMContext *getContext() {
#if D_LLVM_VERSION_ORD <= 38
    return &llvm::getGlobalContext();
#else
    static llvm::LLVMContext *context = NULL;
    if (context) {
        return context;
    } else {
        context = new llvm::LLVMContext();
        return context;
    }
#endif
}

Function *createFunction(Units *units, Type *type, Node *top) {
    Context *ctx = units->top()->ctx;
    llvm::Type *llvm_return_type = ctx->toLLVMType(type, top, true);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);

    std::string new_name;
    units->top()->getUnusedFunctionName(&new_name);

    llvm::Function *llvm_fn = llvm::Function::Create(
        ft, ctx->toLLVMLinkage(Linkage::Extern_C), new_name.c_str(),
        units->top()->module);

    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    std::vector<Variable *> args;
    Function *fn = new Function(type, &args, llvm_fn, 0, &new_name);

    fn->linkage = Linkage::Intern;

    return fn;
}

void findIndirectObjectsForInst(const llvm::Instruction *I,
                                llvm::Module *mod,
                                std::vector<llvm::Function *> *indirect_functions,
                                std::vector<llvm::GlobalVariable *> *indirect_variables);

void findIndirectObjectsForOp(const llvm::Operator *opt,
                              llvm::Module *mod,
                              std::vector<llvm::Function *> *indirect_functions,
                              std::vector<llvm::GlobalVariable *> *indirect_variables) {
    for (const llvm::Value *operand : opt->operands()) {
        if (const llvm::GlobalVariable *gv = llvm::dyn_cast<llvm::GlobalVariable>(operand)) {
            llvm::StringRef name = gv->getName();
            llvm::GlobalVariable *mod_gv = mod->getGlobalVariable(name, true);
            if (mod_gv) {
                indirect_variables->push_back(mod_gv);
            } else {
                fprintf(stderr, "did not find variable\n");
                abort();
            }
        } else if (const llvm::Function *fn = llvm::dyn_cast<llvm::Function>(operand)) {
            llvm::StringRef name = fn->getName();
            llvm::Function *mod_fn = mod->getFunction(name);
            if (mod_fn) {
                indirect_functions->push_back(mod_fn);
            } else {
                fprintf(stderr, "did not find function\n");
                abort();
            }
        } else if (const llvm::Instruction *sub_inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
            findIndirectObjectsForInst(sub_inst, mod,
                indirect_functions, indirect_variables);
        } else if (const llvm::Operator *sub_opt = llvm::dyn_cast<llvm::Operator>(operand)) {
            findIndirectObjectsForOp(sub_opt, mod,
                indirect_functions, indirect_variables);
        }
    }
}

void findIndirectObjectsForInst(const llvm::Instruction *inst,
                                llvm::Module *mod,
                                std::vector<llvm::Function *> *indirect_functions,
                                std::vector<llvm::GlobalVariable *> *indirect_variables) {
    for (const llvm::Value *operand : inst->operands()) {
        if (const llvm::GlobalVariable *gv = llvm::dyn_cast<llvm::GlobalVariable>(operand)) {
            llvm::StringRef name = gv->getName();
            llvm::GlobalVariable *mod_gv = mod->getGlobalVariable(name, true);
            if (mod_gv) {
                indirect_variables->push_back(mod_gv);
            } else {
                fprintf(stderr, "did not find variable\n");
                abort();
            }
        } else if (const llvm::Function *fn = llvm::dyn_cast<llvm::Function>(operand)) {
            llvm::StringRef name = fn->getName();
            llvm::Function *mod_fn = mod->getFunction(name);
            if (mod_fn) {
                indirect_functions->push_back(mod_fn);
            } else {
                fprintf(stderr, "did not find function\n");
                abort();
            }
        } else if (const llvm::Instruction *sub_inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
            findIndirectObjectsForInst(sub_inst, mod,
                indirect_functions, indirect_variables);
        } else if (const llvm::Operator *sub_opt = llvm::dyn_cast<llvm::Operator>(operand)) {
            findIndirectObjectsForOp(sub_opt, mod,
                indirect_functions, indirect_variables);
        }
    }
}

void findIndirectObjectsForFunction(llvm::Function *llvm_fn,
                         std::vector<llvm::Function *> *indirect_functions,
                         std::vector<llvm::GlobalVariable *> *indirect_variables) {
    for (const llvm::BasicBlock &block : *llvm_fn) {
        for (const llvm::Instruction &inst : block) {
            findIndirectObjectsForInst(&inst, llvm_fn->getParent(),
                                       indirect_functions,
                                       indirect_variables);
            if (inst.getOpcode() == llvm::Instruction::Call) {
                const llvm::CallInst *call_inst =
                    llvm::dyn_cast<llvm::CallInst>(&inst);
                llvm::Function *call_inst_fn =
                    call_inst->getCalledFunction();
                if (call_inst_fn) {
                    indirect_functions->push_back(call_inst_fn);
                }
            }
        }
    }
}

bool addFunction(llvm::Function *llvm_fn, llvm::Module *mod,
                 llvm::ValueToValueMapTy *vmap) {
    std::string name = llvm_fn->getName().str();
    void *fn_pointer =
        llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(
            name.c_str()
        );
    llvm::Function *new_fn =
        llvm::Function::Create(
            llvm_fn->getFunctionType(),
            (fn_pointer ? llvm::GlobalValue::ExternalLinkage
                        : llvm_fn->getLinkage()),
            name.c_str(),
            mod
        );
    vmap->insert(std::pair<const llvm::Value *, llvm::Value *>(llvm_fn, new_fn));
    llvm::Function::arg_iterator DestI = new_fn->arg_begin();
    for (const llvm::Argument & I : llvm_fn->args()) {
        if (vmap->count(&I) == 0) {
            DestI->setName(I.getName());
            vmap->insert(std::pair<const llvm::Value *, llvm::Value *>(&I, &*DestI++));
        }
    }

    return (!fn_pointer);
}

bool addVariable(llvm::GlobalVariable *gv, llvm::Module *mod,
                 llvm::ValueToValueMapTy *vmap) {
    std::string name = gv->getName().str();
    llvm::Type *pt = gv->getType()->getPointerElementType();
    llvm::GlobalVariable *llvm_var =
	llvm::cast<llvm::GlobalVariable>(
	    mod->getOrInsertGlobal(name.c_str(),
			           pt)
	);
    llvm_var->setLinkage(gv->getLinkage());
    if (gv->isConstant()) {
	llvm_var->setConstant(true);
    }
    vmap->insert(std::pair<const llvm::Value *, llvm::Value *>(gv, llvm_var));
    return true;
}

void setInitializer(Context *ctx,
                    llvm::GlobalVariable *from,
                    llvm::GlobalVariable *to) {
    llvm::Type *pt = from->getType()->getPointerElementType();
    if (from->hasInitializer()) {
        to->setInitializer(from->getInitializer());
    } else {
        llvm::Constant *init = NULL;
        if (pt->isPointerTy()) {
            init = getNullPointer(pt);
        } else if (pt->isStructTy()) {
            init = llvm::ConstantAggregateZero::get(pt);
        } else if (pt->isIntegerTy()) {
            init = ctx->nt->getConstantInt(llvm::dyn_cast<llvm::IntegerType>(pt), "0");
        }
        if (init) {
            to->setInitializer(init);
        } else {
            llvm::dbgs() << "No initializer for " << *to << "\n";
            abort();
        }
    }
}

void findIndirectObjectsForVariable(llvm::GlobalVariable *gv,
    std::vector<llvm::Function *> *indirect_functions,
    std::vector<llvm::GlobalVariable *> *indirect_variables) {
    if (gv->hasInitializer()) {
        llvm::Constant *c = gv->getInitializer();
        if (llvm::ConstantStruct *cs =
                llvm::dyn_cast<llvm::ConstantStruct>(c)) {
            int els = cs->getNumOperands();
            for (int i = 0; i < els; i++) {
                llvm::Constant *c = cs->getAggregateElement(i);
                if (llvm::GlobalVariable *sub_gv =
                        llvm::dyn_cast<llvm::GlobalVariable>(c)) {
                    indirect_variables->push_back(sub_gv);
                } else if (llvm::Operator *opt =
                        llvm::dyn_cast<llvm::Operator>(c)) {
                    findIndirectObjectsForOp(opt, gv->getParent(),
                                             indirect_functions,
                                             indirect_variables);
                }
            }
        } else if (llvm::GlobalVariable *sub_gv =
                llvm::dyn_cast<llvm::GlobalVariable>(c)) {
            indirect_variables->push_back(sub_gv);
        }
    }
}

void linkRetrievedObjects(llvm::Module *mod, Node *top,
                          std::vector<Function *> *arg_functions,
                          std::vector<Variable *> *arg_variables) {
    std::set<std::string> seen_functions;
    std::set<std::string> seen_variables;

    std::vector<llvm::Function *> functions;
    std::vector<llvm::GlobalVariable *> variables;

    std::vector<llvm::Function *> functions_to_clone;
    std::vector<llvm::GlobalVariable *> variables_to_init;

    for (std::vector<Function *>::iterator
            b = arg_functions->begin(),
            e = arg_functions->end();
            b != e;
            ++b) {
        functions.push_back((*b)->llvm_function);
    }
    for (std::vector<Variable *>::iterator
            b = arg_variables->begin(),
            e = arg_variables->end();
            b != e;
            ++b) {
        variables.push_back(llvm::dyn_cast<llvm::GlobalVariable>((*b)->value));
    }

    llvm::ValueToValueMapTy vmap;

    while (functions.size() || variables.size()) {
        if (functions.size()) {
            llvm::Function *fn = functions.back();
            functions.pop_back();
            if (seen_functions.find(fn->getName().str()) != seen_functions.end()) {
                continue;
            }
            bool needs_clone = addFunction(fn, mod, &vmap);
            if (needs_clone) {
                functions_to_clone.push_back(fn);
            }
            findIndirectObjectsForFunction(
                fn,
                &functions,
                &variables
            );
            seen_functions.insert(fn->getName().str());
            continue;
        }
        if (variables.size()) {
            llvm::GlobalVariable *gv = variables.back();
            variables.pop_back();
            if (seen_variables.find(gv->getName().str()) != seen_variables.end()) {
                continue;
            }
            addVariable(gv, mod, &vmap);
            findIndirectObjectsForVariable(
                gv,
                &functions,
                &variables
            );
            seen_variables.insert(gv->getName().str());
            variables_to_init.push_back(gv);
        }
    }

    for (std::vector<llvm::GlobalVariable *>::iterator
            b = variables_to_init.begin(),
            e = variables_to_init.end();
            b != e;
            ++b) {
        llvm::GlobalVariable *gv = *b;
        llvm::GlobalVariable *new_gv = mod->getGlobalVariable(
            gv->getName(), true
        );
        if (!new_gv) {
            fprintf(stderr, "unable to find new global variable %s\n",
                gv->getName().str().c_str());
            abort();
        }
	if (gv->hasInitializer()) {
	    new_gv->setInitializer(MapValue(gv->getInitializer(), vmap));
        }
    }

    for (std::vector<llvm::Function *>::iterator
            b = functions_to_clone.begin(),
            e = functions_to_clone.end();
            b != e;
            ++b) {
        llvm::Function *fn = *b;
        std::string name = fn->getName().str();
        if (name == "llvm.va_end") {
            continue;
        }
        if (name == "llvm.va_start") {
            continue;
        }
        llvm::Function *new_fn = mod->getFunction(
            fn->getName().str()
        );
        llvm::SmallVector<llvm::ReturnInst*, 8> returns;

        if (fn->size() > 0) {
            CloneFunctionInto(new_fn, fn, vmap, false, returns);
        }
    }

    for (std::vector<Function *>::iterator
            b = arg_functions->begin(),
            e = arg_functions->end();
            b != e;
            ++b) {
        Function *fn = *b;
        llvm::Function *new_fn = mod->getFunction(
            fn->symbol.c_str()
        );
        fn->llvm_function = new_fn;
    }

    for (std::vector<Variable *>::iterator
            b = arg_variables->begin(),
            e = arg_variables->end();
            b != e;
            ++b) {
        Variable *var = *b;
        llvm::GlobalVariable *new_var = mod->getGlobalVariable(
            var->symbol, true
        );
        var->value = new_var;
    }
}
}
