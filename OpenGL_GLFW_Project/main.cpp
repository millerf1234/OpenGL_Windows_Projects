//main.h
//
//Create the application and launch it
//

#include "Application.h"
#include "LoggingMessageTargets.h"

enum class Test { TEST1, TEST2, IT_IS_WORKING};

int main() {
	Test t1, t2, t3;
	t1 = Test::TEST1;
	t2 = Test::TEST2;
	t3 = Test::IT_IS_WORKING;

	printf("%s %s %s\n", ENUM_TO_STRING(t1), ENUM_TO_STRING(t2), ENUM_TO_STRING(t3));
	std::cin.get();
	return 0;

	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	if (app) 
		app->launch();
}