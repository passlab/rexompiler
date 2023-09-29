#include <vector>

#include "boost/shared_ptr.hpp"

class X;

void foobar() 
   {
     int num = 0;
     std::vector<boost::shared_ptr<X> > *X_ptr = 0L;

     for(boost::shared_ptr<X> procInfo: *X_ptr) 
        {
          num++;
        }
   }


