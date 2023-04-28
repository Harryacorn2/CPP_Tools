#include <iostream>
#include <thread>
#include "AutoLock.h"
#include "Profiler.h"

using namespace std;

mutex printLock;

/*
sayThreeTimes is a function that will take a message and print it out three
times. It has no locking mechanism and as such, if it is ran on multiple
threads messages may merge together.
*/
void sayThreeTimes(std::string msg)
{
	PROFILE_SCOPE(msg);
	for (int i = 0; i < 3; i++) {
		std::cout << "Task says: ";
		std::cout << msg;
		std::cout << std::flush;
	}
}

/*
sayThreeTimesManualLock is the same as sayThreeTimes, but it uses AutoLock's
manual locking function. "PRINT" could be any string of alphabetical characters
and any "LOCK/UNLOCK(x)" statement with that same string will interact with it.
*/
void sayThreeTimesManualLock(std::string msg)
{
	LOCK(PRINT);
	PROFILE_SCOPE(msg);
	for (int i = 0; i < 3; i++) {
		std::cout << "Task says: ";
		std::cout << msg;
		std::cout << std::flush;
	}
	UNLOCK(PRINT);
}

/*
sayThreeTimesScopeLock is the same as sayThreeTimesManualLock but instead of
unlocking manually, the lock is locked with SCOPELOCK is called, and unlocks when
the program exits scope (by use of the automatic destructor call) automatically.
*/
void sayThreeTimesScopeLock(std::string msg)
{
	LOCK_SCOPE(PRINT);
	PROFILE_SCOPE(msg);
	for (int i = 0; i < 3; i++) {
		std::cout << "Task says: ";
		std::cout << msg;
		std::cout << std::flush;
	}
}

int main(int argc, char *argv[]) {
	PROFILE_SCOPE(main);
	
	{
		PROFILE_SCOPE(first);
		std::thread t1(sayThreeTimes, "-----\n");
		std::thread t2(sayThreeTimes, "^^^^^\n");
		std::thread t3(sayThreeTimes, "00000\n");
		std::thread t4(sayThreeTimes, "!!!!!\n");
		std::thread t5(sayThreeTimes, "$$$$$\n");
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
	}
	
	std::cout << endl << endl;
	
	{
		PROFILE_SCOPE(second);
		std::thread l1(sayThreeTimesManualLock, "-----\n");
		std::thread l2(sayThreeTimesManualLock, "^^^^^\n");
		std::thread l3(sayThreeTimesManualLock, "00000\n");
		std::thread l4(sayThreeTimesManualLock, "!!!!!\n");
		std::thread l5(sayThreeTimesManualLock, "$$$$$\n");
		l1.join();
		l2.join();
		l3.join();
		l4.join();
		l5.join();
	}
	
	std::cout << endl << endl;
	
	{
		PROFILE_SCOPE(third);
		std::thread s1(sayThreeTimesScopeLock, "-----\n");
		std::thread s2(sayThreeTimesScopeLock, "^^^^^\n");
		std::thread s3(sayThreeTimesScopeLock, "00000\n");
		std::thread s4(sayThreeTimesScopeLock, "!!!!!\n");
		std::thread s5(sayThreeTimesScopeLock, "$$$$$\n");
		s1.join();
		s2.join();
		s3.join();
		s4.join();
		s5.join();
	}
}