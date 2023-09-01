static const char *purpose = "demo checker for smoke tests";

static const char *description =
    "This is a stupid example that pretends to run some algorithm on an optional specimen. In actuality, it does nothing "
    "but check its command-line and print a message.";

#include <rose.h>

using namespace Rose;

int
main(int argc, char *argv[]) {
    std::string algorithm = "none";

    //implementation goes here

    std::cout <<"ran algorithm " <<algorithm <<" on " << argc - 1 << "input(s)" <<"\n";
}
