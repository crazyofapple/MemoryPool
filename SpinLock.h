#pragma once
#include <atomic>
using namespace std;
class SpinLock
{
public:
	SpinLock() :atmoicLock(false) {}
	void lock() {
		while (atmoicLock.exchange(true));
	}
	void unlock() {
		atmoicLock = false;
	}
	bool isLocked() const {
		return atmoicLock;
	}
	
private:
	atomic_bool atmoicLock;
};

