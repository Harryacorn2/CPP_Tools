#ifndef AUTO_LOCK_H
#define AUTO_LOCK_H

#include <iostream>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <condition_variable>
#include <unistd.h>

#define LOCK_SCOPE(x) AutoLock lock##x(#x)
#define LOCK(x) AutoLock::ManualLock(#x)
#define UNLOCK(x) AutoLock::ManualUnlock(#x)

class AutoLock {
public:
	
	AutoLock(std::string name) {
		mName = name;
		
		mResourceLock.lock();
		if (locks.find(mName) != locks.end()) {
			mResourceLock.unlock();
			locks[mName].lock();
			
		} else {
			locks[mName].lock();
			mResourceLock.unlock();
		}
	}
	
	~AutoLock() {
		locks[mName].unlock();
	}
	
	static void ManualLock(std::string name) {
		mResourceLock.lock();
		if (locks.find(name) != locks.end()) {
			mResourceLock.unlock();
			locks[name].lock();
			
		} else {
			locks[name].lock();
			mResourceLock.unlock();
		}
	}
	
	static void ManualUnlock(std::string name) {
		locks[name].unlock();
	}
	
	
private:
	std::string mName;
	static std::unordered_map<std::string, std::mutex> locks;
	static std::mutex mResourceLock;
};

std::mutex AutoLock::mResourceLock{};
std::unordered_map<std::string, std::mutex> AutoLock::locks{};

#endif