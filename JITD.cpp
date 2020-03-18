/*
 * Copyright (C) 2020 Vaivaswatha N
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/ObjectCache.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

#include <memory>

#include "JITD.h"

using namespace llvm;

namespace {

struct FunctionsMap {
  const char *FName;
  const void *FAddr;
};
std::vector<FunctionsMap> getFunctions(void)
{
  FunctionsMap m[] = {
    {"printf", (void *)printf},
    {"malloc", (void *)malloc}
  };

  return std::vector<FunctionsMap>(std::begin(m), std::end(m));
}

// Add functions in SRTL that the JIT'ed code can access.
Error addExampleBuiltins(orc::ExecutionSession &ES, const DataLayout &DL) {
  orc::SymbolMap M;
  orc::MangleAndInterner Mangle(ES, DL);
  // Register every symbol that can be accessed from the JIT'ed code.
  auto ExampleFuncs = getFunctions();
  for (auto fa : ExampleFuncs) {
    M[Mangle(fa.FName)] = JITEvaluatedSymbol(
        pointerToJITTargetAddress(fa.FAddr), JITSymbolFlags());
  }

  if (auto Err = (ES.getMainJITDylib().define(absoluteSymbols(M))))
    return Err;

  return Error::success();
}

} // end of anonymous namespace

namespace ExampleVM {

using namespace orc;

void ExampleJIT::init() {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
}

Expected<std::unique_ptr<ExampleJIT>>
ExampleJIT::create(const std::string &Filename, ObjectCache *OC) {

  // Create an LLJIT instance with a custom CompileFunction.
  auto J = orc::LLJITBuilder()
               .setCompileFunctionCreator(
                   [&](JITTargetMachineBuilder JTMB)
                       -> Expected<IRCompileLayer::CompileFunction> {
                     auto TM = JTMB.createTargetMachine();
                     if (!TM)
                       return TM.takeError();
                     return IRCompileLayer::CompileFunction(
                         TMOwningSimpleCompiler(std::move(*TM), OC));
                   })
               .create();

  if (!J)
    return J.takeError();

  if (auto Err =
          addExampleBuiltins((*J)->getExecutionSession(), (*J)->getDataLayout()))
    return std::move(Err);

  auto *THIS = new ExampleJIT(std::move(*J));

  auto Ctx = llvm::make_unique<LLVMContext>();
  SMDiagnostic Smd;
  auto M = parseIRFile(Filename, Smd, *Ctx);
  if (!M) {
    std::string ErrMsg;
    raw_string_ostream OS(ErrMsg);
    Smd.print("lljit", OS);
    auto Err = createStringError(inconvertibleErrorCode(), OS.str().c_str());
    return std::move(Err);
  }

  ThreadSafeModule TSM(std::move(M), std::move(Ctx));
  if (auto Err = THIS->Jitter->addIRModule(std::move(TSM))) {
    return std::move(Err);
  }

  return std::unique_ptr<ExampleJIT>(THIS);
}

Expected<void *> ExampleJIT::getAddressFor(const std::string &Symbol) {

  auto SA = Jitter->lookup(Symbol);
  if (auto Err = SA.takeError()) {
    return std::move(Err);
  }

  return reinterpret_cast<void *>((*SA).getAddress());
}

} // namespace ExampleVM
