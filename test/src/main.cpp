#include "TestHarness.h"
const char *LOG_NAME = "testing";
int main(int argc, char **argv)
{
	TestResult tr;
	TestRegistry::runAllTests(tr);
	return 0;
}
