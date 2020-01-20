/**
 * LLVM equivalent of:
 *
 * int sum(int a, int b) {
 *     return a + b;
 * }
 */

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    // Module creation
    LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

    // Function prototype creation
    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

    // Builder creation
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    // Instruction added to the builder
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    //Analysis
    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    // Choosing the triple
    // char triple[] = "x86_64";
    char* triple = LLVMGetDefaultTargetTriple(); // Using the triple of your machine
    char cpu[] = "";
    printf("%s\n",triple);

    // Initialization of the targets

    // ======================================================
    // NEED TO FIX TARGET CREATION AND INITIALIZATION
    // ======================================================
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllAsmPrinters();

    LLVMTargetRef targetRef;
    // LLVMInitializeNativeTarget();
    // LLVMTargetRef targetRef = LLVMGetFirstTarget();
    // ======================================================

    // Generating the target machine
    char** errPtrTriple;
    LLVMBool resTriple = LLVMGetTargetFromTriple(triple, &targetRef, errPtrTriple);
    if (resTriple != 0)
    {
        printf("%s\n",*errPtrTriple);
    }

    // LLVMCreateTargetMachine() signature
    /*LLVMTargetMachineRef T = LLVMCreateTargetMachine(LLVMTargetRef T,
    *                                                 const char* Triple,
    *                                                 const char* CPU,
    *                                                 const char* features,
    *                                                 LLVMCodeGenOptLevel Level,
    *                                                 LLVMRelocMode Reloc,
    *                                                 LLVMCodeModel CodeModel);
    */

    LLVMTargetMachineRef targetMachineRef = LLVMCreateTargetMachine(targetRef, triple, cpu, "", LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

    // Bitcode writing to file
    // LLVMTargetMachineEmitToFile() signature
    // LLVMTargetMachineEmitToFile(LLVMTargetMachineRef T, LLVMModuleRef M, char* filename, LLVMCodeGenFileType codegen, char** ErrorMessage)
    char** errPtrFile;
    LLVMBool resFile = LLVMTargetMachineEmitToFile(targetMachineRef, mod, "sum.bc", LLVMObjectFile, errPtrFile);
    if (resFile != 0)
    {
        printf("%s\n",*errPtrFile);
    }

    // // Bitcode writing to memory buffer
    // // LLVMTargetMachineEmitToMemoryBuffer(LLVMTargetMachineRef T, LLVMModuleRef M, LLVMCodeGenFileType codegen, char** ErrorMessage, LLVMMemoryBufferRef OutMemBuf)
    // char** errPtrMem;
    // LLVMTargetMachineEmitToMemoryBuffer(targetMachineRef, mod, LLVMObjectFile, errPtrMem, &mem);

    LLVMDisposeBuilder(builder);
}
