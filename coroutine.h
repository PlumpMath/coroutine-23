/**
The MIT License (MIT)

Copyright (c) 2014 Liang Xuan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 600
#endif

#include <ucontext.h>
#include <stdlib.h>
#include <stdint.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class coroutine
{
#ifdef _LP64
    static const size_t STACK_SIZE=2097152+16384;
#else
    static const size_t STACK_SIZE=16384;
#endif
public:
	coroutine(void (*roroutineFunc)(coroutine<T>&,T),T argument,T defaultRetval);
	~coroutine();
	void yield();
	void run();
	void setRetval(T retval){m_retval=retval;}
	T getRetval(){return m_retval;}
private:
	coroutine();
	static void roroutineFunc(uint32_t halfPointerLeft, uint32_t halfPointerRight);
private:
	const coroutine<T>& self;
	T m_argument;
	void (*m_roroutineFunc)(coroutine<T>& self,T);
	T m_retval;
	char* m_stack;
	ucontext_t m_callee;
	ucontext_t m_caller;
};

template <typename T>
void coroutine<T>::roroutineFunc(uint32_t halfPointerLeft, uint32_t halfPointerRight)
{
	coroutine<T> &self=*reinterpret_cast<coroutine<T> *>(
		(static_cast<size_t>(halfPointerLeft) << 32) + halfPointerRight);
	self.m_roroutineFunc(self,self.m_argument);
	self.yield();
}

template <typename T>
coroutine<T>::coroutine()
:self(*this)
{

}

template <typename T>
coroutine<T>::coroutine(void (*roroutineFunc)(coroutine<T>&,T),T argument,T defaultRetval)
:self(*this)
,m_roroutineFunc(roroutineFunc)
,m_argument(argument)
,m_retval(defaultRetval)
,m_stack(NULL)
{
	m_stack=new char[self.STACK_SIZE];
	getcontext(&m_callee);

	m_callee.uc_link=NULL;
	m_callee.uc_stack.ss_sp=m_stack;
	m_callee.uc_stack.ss_size=self.STACK_SIZE;
	m_callee.uc_stack.ss_flags=0;
	makecontext(
		&m_callee,
		reinterpret_cast<void (*)(void)>(&coroutine<T>::roroutineFunc),
		2,
		reinterpret_cast<size_t>(this) >> 32,
		this);
}


template <typename T>
coroutine<T>::~coroutine()
{
	delete [] m_stack;
}

template <typename T>
void coroutine<T>::yield()
{
	swapcontext(&m_callee, &m_caller);
}

template <typename T>
void coroutine<T>::run()
{
	swapcontext(&m_caller, &m_callee);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
template <>
class coroutine <void>
{
#ifdef _LP64
    static const size_t STACK_SIZE=(2097152+16384);
#else
    static const size_t STACK_SIZE=16384;
#endif
public:
	coroutine(void (*roroutineFunc)(coroutine<void>&))
	:self(*this)
	,m_roroutineFunc(roroutineFunc)
	,m_stack(NULL)
	{
		m_stack=new char[self.STACK_SIZE];
		getcontext(&m_callee);

		m_callee.uc_link=NULL;
		m_callee.uc_stack.ss_sp=m_stack;
		m_callee.uc_stack.ss_size=self.STACK_SIZE;
		m_callee.uc_stack.ss_flags=0;
		makecontext(
			&m_callee,
			reinterpret_cast<void (*)(void)>(&coroutine<void>::roroutineFunc),
			2,
			reinterpret_cast<size_t>(this) >> 32,
			this);
	}
	~coroutine()
	{
		delete [] m_stack;
	}
	void yield()
	{
		swapcontext(&m_callee, &m_caller);
	}
	void run()
	{
		swapcontext(&m_caller, &m_callee);
	}
private:
	coroutine()
	:self(*this)
	{

	}
	static void roroutineFunc(uint32_t halfPointerLeft, uint32_t halfPointerRight)
	{
		coroutine<void> &self=*reinterpret_cast<coroutine<void> *>(
			(static_cast<size_t>(halfPointerLeft) << 32) + halfPointerRight);
		self.m_roroutineFunc(self);
		self.yield();
	}
private:
	const coroutine<void>& self;
	void (*m_roroutineFunc)(coroutine<void>& self);
	char* m_stack;
	ucontext_t m_callee;
	ucontext_t m_caller;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
