#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LLTest
#include <boost/test/unit_test.hpp>

#include "JITD.h"

// Compiles InputFilename and executes "void jitentry(void)" in it.
void execLLFile(const std::string &InputFilename) {
  using namespace ExampleVM;

  ExampleJIT::init();
  auto SJ = ExampleJIT::create(InputFilename);
  if (!SJ) {
    auto Err = SJ.takeError();
    auto M = llvm::toString(std::move(Err));
    BOOST_FAIL(M);
  }
  // Look for function named "jitentry" in LLVM-IR and run it.
  auto ExampleMainAddr = (*SJ)->getAddressFor("jitentry");
  if (!ExampleMainAddr) {
    auto Err = SJ.takeError();
    auto M = llvm::toString(std::move(Err));
    BOOST_FAIL(M);
  }
  auto ExampleMain = reinterpret_cast<void (*)()>(*ExampleMainAddr);

  ExampleMain();

  // Without this dummy try-catch, test case "test_throw" will segfault.
  try {
    throw std::exception();
  } catch (std::exception &) {
    ;
  }
}

BOOST_AUTO_TEST_SUITE(lljit_testsuite)

BOOST_AUTO_TEST_CASE( lljit_helloworld ) {
  execLLFile("examples/helloworld.ll");
}

BOOST_AUTO_TEST_CASE( test_throw ) {
  BOOST_CHECK_THROW(
    throw std::exception(),
    std::exception
    );
}

BOOST_AUTO_TEST_SUITE_END()
