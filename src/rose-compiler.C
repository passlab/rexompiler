#include <iostream>
#include "sage3basic.h"
#include "omp_simd.h"

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);
  std::string simd = "";
  
  for (unsigned int i = 0; i < args.size(); i++) {
    std::string arg = args.at(i);
    if (arg.find("--simd-target=") == 0) {
        int pos = arg.find("=") + 1;
        simd = arg.substr(pos);
        args.erase(args.begin()+i);
    }
  }
  
  if (simd == "intel-avx512") {
    simd_arch = Intel_AVX512;
  } else if (simd == "arm-sve") {
    simd_arch = Arm_SVE2;
  } else if (simd == "3addr") {
    simd_arch = Addr3;
  } else if (simd != "") {
    std::cout << "Error: Unknown SIMD architecture." << std::endl;
    simd_arch = Nothing;
  }

#if defined(ROSE_COMPILER_FOR_LANGUAGE)
  std::string language(ROSE_COMPILER_FOR_LANGUAGE);
  bool has_dialect = false;
  for (std::vector<std::string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
    if (arg->find("-std=") == 0) {
      has_dialect = true;
      break;
    }
  }
  if (!has_dialect) {
    args.insert(args.begin()+1, "-std="+language);
  }
#endif

  return backend(frontend(args));
}

