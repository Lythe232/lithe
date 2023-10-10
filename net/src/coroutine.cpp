#include "include/coroutine.h"
#include "include/coScheduler.h"
#include <assert.h>

namespace lithe
{
static auto g_logger = LOG_ROOT();

static thread_local Coroutine* t_run_co = nullptr;
static thread_local std::shared_ptr<Coroutine> t_main_co_ = nullptr;
Coroutine::Coroutine() :
	ctx_(),
	state_(INIT),
	func_(nullptr),
	size_(0),
	ss_sp_(nullptr)
{
	t_run_co = this;
	if(getcontext(&ctx_))
	{
		//TODO LOG
		assert(0);
	}
}

Coroutine::~Coroutine()
{
	if(ss_sp_)
	{
		StackAlloc::co_free(ss_sp_);
		ss_sp_ = nullptr;
	}
}

Coroutine::Coroutine(std::function<void()> func, size_t size) :
	ctx_(),
	state_(INIT),
	func_(func),
	size_(size),
	ss_sp_(nullptr)
{
	state_ = INIT;
	// size_ = 256 * 1024;
	if(size_ <= 0)
	{
		size_ = 128 * 1024;
	}
	else if(size_ > 1024 * 1024 * 8)
	{
		size_ = 1024 * 1024 * 8;
	}
	if(size_ & 0xFFF)
	{
		size_ &= ~0xFFF;
		size_ += 0x1000;
	}
	if(getcontext(&ctx_))
	{
		//TODO LOG
		assert(0);
	}
	ss_sp_ = StackAlloc::co_alloc(size_);
	// ss_sp_ = malloc(size_);

	ctx_.uc_stack.ss_sp = ss_sp_;
	ctx_.uc_stack.ss_size = size_;
	ctx_.uc_stack.ss_flags = 0;
	ctx_.uc_link = nullptr;
	makecontext(&ctx_, reinterpret_cast<void (*)(void)>(coSchedulerMainFunc), 1, this);
	// makecontext(&ctx_, (void (*)(void))coMainFunc, 1, this);
}

void Coroutine::resume()
{
	if(this == t_run_co)
	{
		LOG_INFO(g_logger) << "RESUME T_RUN_CO.";
		return;
	}
	getRunCo();
	state_ = RUNNING;
	t_run_co = this;
	ucontext_t& pctx = getMainCo()->getctx();
	if(swapcontext(&pctx, &ctx_))
	{
		//TODO LOG
		assert(0);
	}
}

void Coroutine::yield()
{
	if(this != t_run_co)
	{
		//TODO LOG
		return ;
	}
	t_run_co = t_main_co_.get();
	ucontext_t& nctx = getMainCo()->getctx();
	if(swapcontext(&ctx_, &nctx))
	{
		//TODO LOG
		assert(0);
	}

}
void Coroutine::reset(std::function<void()> func)
{
	if(!ss_sp_)
	{
		LOG_ERROR(g_logger) << "Coroutine::reset() error.";
		return ;
	}
	assert(ss_sp_);
	assert(state_ == INIT ||
			state_ == EXCEP ||
			state_ == END);
	func_ = func;
	if(getcontext(&ctx_))
	{
		//TODO LOG
		assert(0);
	}
	ctx_.uc_link = nullptr;
	ctx_.uc_stack.ss_sp = ss_sp_;
	ctx_.uc_stack.ss_size = size_;

	makecontext(&ctx_, (void (*)(void))coSchedulerMainFunc, 1, this);
	// makecontext(&ctx_, (void (*)(void))coMainFunc, 1, this);
	state_ = INIT;
}

void Coroutine::swapIn()
{
	t_run_co = this;
	state_ = RUNNING;
	ucontext_t& pctx = CoScheduler::getMainCo()->getctx();
	if(swapcontext(&pctx, &ctx_))
	{
		//TODO LOG
		assert(0);
	}
}
void Coroutine::swapOut()
{
	t_run_co = CoScheduler::getMainCo();
	ucontext_t& nctx = CoScheduler::getMainCo()->getctx();
	if(swapcontext(&ctx_, &nctx))
	{
		//TODO LOG
		assert(0);
	}
}
std::shared_ptr<Coroutine> Coroutine::getMainCo()
{
	if(!t_main_co_)
	{
		getRunCo();
	}
	return t_main_co_;
}
std::shared_ptr<Coroutine> Coroutine::getRunCo()
{
	if(t_run_co)
	{
		return t_run_co->shared_from_this();
	}
	std::shared_ptr<Coroutine> mainCo(new Coroutine());
	assert(t_run_co == mainCo.get());
	t_main_co_ = mainCo;
	return t_run_co->shared_from_this();
}

void Coroutine::coMainFunc(Coroutine *cor)
{
	if(!cor)
	{
		//TODO LOG
		return;
	}
	try
	{
		cor->func_();	
		cor->func_ = nullptr;
		cor->setState(END);
	}
	catch(std::exception& e)
	{

		cor->setState(Coroutine::EXCEP);
		LOG_ERROR(g_logger) << cor << ": EXCEPT HAPPENED." << " Message: " << e.what();
	}
	cor->yield();
	assert(0);
}


void Coroutine::coSchedulerMainFunc(Coroutine *cor)
{
	if(!cor)
	{
		//TODO LOG
		return;
	}
	try
	{
		if(cor->func_)
		{
			cor->func_();	
			cor->func_ = nullptr;
			cor->setState(END);
		}
	}
	catch(std::exception& e)
	{
		cor->setState(Coroutine::EXCEP);
		LOG_FATAL(g_logger) << cor << ": EXCEPT HAPPENED." << " Message: " << e.what();
	}

	// cor->yield();
	cor->swapOut();

	assert(0);
}


}	//namespace lithe