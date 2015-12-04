#include "ConsoleLocker.h"

ConsoleLocker *ConsoleLocker::sharedInstance;
once_flag ConsoleLocker::instanceInitialized;

void ConsoleLocker::init() {
	if(sharedInstance == nullptr) sharedInstance = new ConsoleLocker();
}

ConsoleLocker* ConsoleLocker::getInstance() {
	call_once(instanceInitialized, &ConsoleLocker::init);
	return sharedInstance;
}

void ConsoleLocker::execute(function<void(void)> consoleFunction) {
	lock_guard<recursive_mutex> guard(commonLock);
	consoleFunction();
}

void ConsoleLocker::tearDown() {
	if(sharedInstance != nullptr) delete sharedInstance;
}
