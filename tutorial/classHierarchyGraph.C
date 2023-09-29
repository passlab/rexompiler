#include "rose.h"
#include "CallGraph.h"


using namespace std;

int main(int argc, char * argv[])
{
	SgProject* project = new SgProject(argc, argv);

	//Construct class hierarchy graph
	ClassHierarchyWrapper hier(project);

	//Display the ancestors of each class
	vector<SgClassDefinition*> allClasses = SageInterface::querySubTree<SgClassDefinition>(project, V_SgClassDefinition);
	for(SgClassDefinition* classDef: allClasses)
	{
		printf("\n%s subclasses: ", classDef->get_declaration()->get_name().str());
		for(SgClassDefinition* subclass: hier.getSubclasses(classDef))
		{
			printf("%s, ", subclass->get_declaration()->get_name().str());
		}
	}

	return 0;
}

