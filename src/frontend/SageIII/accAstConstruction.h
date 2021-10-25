#include "OpenACCIR.h"

bool checkOpenACCIR(OpenACCDirective*);
SgStatement* convertOpenACCDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*>);
SgUpirBaseStatement* convertOpenACCBodyDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*>);
SgOmpExpressionClause* convertOpenACCExpressionClause(SgStatement*, std::pair<SgPragmaDeclaration*, OpenACCDirective*>, OpenACCClause*);
SgOmpVariablesClause* convertOpenACCClause(SgStatement*, std::pair<SgPragmaDeclaration*, OpenACCDirective*>, OpenACCClause*);
