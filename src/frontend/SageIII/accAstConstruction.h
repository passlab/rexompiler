#include "OpenACCIR.h"

bool checkOpenACCIR(OpenACCDirective*);
SgStatement* convertOpenACCDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*>);
SgUpirBodyStatement* convertOpenACCBodyDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*>);
SgOmpExpressionClause* convertOpenACCExpressionClause(SgStatement*, std::pair<SgPragmaDeclaration*, OpenACCDirective*>, OpenACCClause*);
SgOmpClause* convertOpenACCClause(SgStatement*, std::pair<SgPragmaDeclaration*, OpenACCDirective*>, OpenACCClause*);
