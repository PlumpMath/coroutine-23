/**
*test
*/
#include <stdio.h>
#include "coroutine.h"
void test1(coroutine<void>& self);
void test2(coroutine<int>& self,int arg);
void test3(coroutine<int>& self,int arg);
void test4(coroutine<int>& self,int arg);
int main(int argc, char const *argv[])
{
	coroutine<int> co1(test2,0,0);
	coroutine<int> co2(test3,1,1);
	coroutine<int> co3(test4,2,2);

	for (int i = 0; i < 10; ++i)
	{
		co1.run();
	}
	
	return 0;
}


void test1(coroutine<void>& self)
{
	for (int i = 0; i < 5; ++i)
	{
		// printf("test11:line:%d\n",__LINE__);
		self.yield();
		printf("test12:line:%d\n",__LINE__);
	}
}

void test2(coroutine<int>& self,int arg)
{
	for (int i = 0; i < 5; ++i)
	{
		self.setRetval(arg++);
		self.yield();
		printf("test12:line:%d\n",__LINE__);
	}
}

void test3(coroutine<int>& self,int arg)
{
	for (int i = 0; i < 5; ++i)
	{
		self.setRetval(arg++);
		self.yield();
		printf("test12:line:%d\n",__LINE__);
	}
}

void test4(coroutine<int>& self,int arg)
{
	for (int i = 0; i < 5; ++i)
	{
		self.setRetval(arg++);
		self.yield();
		printf("test12:line:%d\n",__LINE__);
	}
}

