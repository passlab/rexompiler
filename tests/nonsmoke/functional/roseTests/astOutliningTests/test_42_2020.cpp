// This is the paper test_42.cpp
// SageInterface::convertRefToInitializedName() on SgFunctionCallExp

struct C
{
  bool var_1;
};

class iterator 
{
  public:
    C* operator->() const;
};

void foobar()
{
#pragma rose_outline
  {
    iterator var_6;
    bool var_7 = var_6->var_1;
  }
}

