#pragma once

#include "log/log.h"
#include <ucontext.h>
#include <functional>
namespace lithe
{

class StackAlloc
{
public:
	static void* co_alloc(size_t size)
	{
		return malloc(size);
	}
	static void co_free(void* p)
	{
		free(p);
	}
};

class Coroutine : public std::enable_shared_from_this<Coroutine>
{
public:
	enum STATE
	{
		INIT, 
		READY,
		RUNNING,
		PAUSE,
		EXCEP,
		END
	};
	Coroutine(std::function<void()> func, size_t size = 0);
	~Coroutine();
	void resume();
	void yield();

	void reset(std::function<void()> func);

	ucontext_t& getctx(){ return ctx_; }
	STATE getState() { return state_; } 
	void setState(STATE state) { state_ = state; }
	static std::shared_ptr<Coroutine> getMainCo();
	static std::shared_ptr<Coroutine> getRunCo();

	static std::string toString(STATE state)
	{
		switch(state)
		{
#define XX(str) case str: return #str;
			XX(INIT)
			XX(READY)
			XX(RUNNING)
			XX(PAUSE)
			XX(EXCEP)
			XX(END)
#undef XX
			default:
				return "UNKOWN";
		}
	}

	static void coMainFunc(Coroutine* cor);
	static void coSchedulerMainFunc(Coroutine* cor);

public:
	friend class CoScheduler;
	friend class IOManager;


private:
	Coroutine();

	//Scheduler use
	void swapIn();
	void swapOut();

private:
	ucontext_t ctx_;
	STATE state_;
	std::function<void()> func_;
	size_t size_;
	void* ss_sp_;
};

}	//namespace lithe