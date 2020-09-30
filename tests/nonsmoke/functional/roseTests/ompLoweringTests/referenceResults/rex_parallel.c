#include <assert.h> 
#include "rex_kmp.h"

void outlined_function_7(int* i, int* j) {
    int __i = *i;
    assert(__i == 100);
    assert(*j == 77);
}

void outlined_function_12(int* j) {
    int i;
    assert(i != 100);
}

int main(void)
{
  int i = 100;
  int j = 77;

  outlined_function_7(&i, &j);

  outlined_function_12(&j);

  return 0;
}
