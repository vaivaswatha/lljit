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

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include "JITD.h"

using namespace llvm;
using namespace ExampleVM;

namespace {

// Command line arguments parsed using LLVM's command line parser.
cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input file>"),
                                   cl::Required);
cl::opt<std::string> OutputFilename("o", cl::desc("Specify output filename"),
                                    cl::value_desc("filename"));
void versionPrinter(llvm::raw_ostream &OS) { OS << "lljit: v0.0.0\n"; }

ExitOnError ExitOnErr;

} // end of anonymous namespace

int main(int argc, char *argv[]) {
  cl::SetVersionPrinter(versionPrinter);
  cl::ParseCommandLineOptions(argc, argv);

  ExampleJIT::init();
  auto SJ = ExitOnErr(ExampleJIT::create(InputFilename));
  // Look for function named "jitentry" in LLVM-IR and run it.
  auto ExampleMainAddr = ExitOnErr(SJ->getAddressFor("jitentry"));
  auto ExampleMain = reinterpret_cast<void (*)()>(ExampleMainAddr);

  ExampleMain();

  return EXIT_SUCCESS;
}
