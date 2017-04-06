#include <list>
#include <iostream>
#include <Windows.h>
#include "ThreadSafeObjectPool.h"
using namespace std;

struct Test1 {
	int a : 2 ; 
	int b : 3; 
	virtual void foo(int i) = 0;
	virtual void bar(int i) = 0;
	char c;
	bool d;
	int e; 
	virtual ~Test1() {};
}; // size 16
struct Test2 : public Test1 {
	Test2(int f_, int g_) :f(f_), g(g_) {}
	int f;
	virtual void foo(int i) { e = i; }
	virtual void bar(int i) { c = i; }
	char g;
	virtual ~Test2() {};
}; // size 24
int main() {
	DWORD start, stop;
	start = GetTickCount();
	ThreadSafeObjectPool<Test2> pool;
	std::list<Test2 *> results;
	
	for (int i = 0; i < 3000000; ++i) {
		if (i % 2 == 0) {
			auto res = pool.create(i, 'p');
			res->foo(i);
			results.push_back(res);
		} else {
			auto res = pool.create(i, 'i');
			res->bar('e');
			results.push_front(res);
		}
		if (i % 3 == 0 && !results.empty()) {
			pool.destroy(results.front());
			results.pop_front();
		}
	}
	for (auto &e : results) {
		pool.destroy(e);
	}
	stop = GetTickCount();
	printf("time: %ld s\n", (stop - start) / 1000);
	
	return 0;
}