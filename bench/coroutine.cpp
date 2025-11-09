#pragma once
#include <bench/coroutine.hpp>
#include <bench/sys.hpp>
#include <bench/arraylist.hpp>
#include <stdlib.h>
#include <atomic>

namespace bench {

extern "C" bool __cdecl bench_Resume_CoroCompleted(bench::Coroutine* coro);


struct Coroutine {
	U8* reserve_start;
	U32* stack_end;
	U32 coro_esp;
	U32 orig_esp;
	std::atomic<int> refcount;
	std::atomic<int> blockers_count;
	std::atomic<Coroutine*> next_in_pool = nullptr;
};

struct CoroutinePool {
	std::atomic<Coroutine*> first_free = nullptr;
	std::atomic<int> count = 0;

	Coroutine* Pop() {
		Coroutine* stack = nullptr;
		do {
			stack = first_free;
			if (!first_free)
				return nullptr;
		} while (!first_free.compare_exchange_weak(stack, stack->next_in_pool));

		count--;
		return stack;
	}

	void Push(Coroutine* new_coro) {
		count++;
		Coroutine* old_first_free = nullptr;
		do {
			old_first_free = this->first_free.load();
			new_coro->next_in_pool = old_first_free;
		} while (!first_free.compare_exchange_weak(old_first_free, new_coro));
	}
};

static ArrayList<CoroutineHandle> g_scheduled_coroutines;
static CoroutinePool g_coro_pool;

template <>
void AddRef(Coroutine* coro) {
	++coro->refcount;
}

template <>
void RemoveRef(Coroutine* coro) {
	if (--coro->refcount == 0) {
		g_coro_pool.Push(coro);
	}
}

CoroutineHandle CreateCoroutine(void** out_userdata_storage, CoroutineProc** out_proc_location) {
	Coroutine* coro = nullptr;
	
	coro = g_coro_pool.Pop();
	if (!coro) {
		constexpr int COROUTINE_STACK_SIZE_WITH_GUARD_PAGE = COROUTINE_STACK_SIZE + 4096;

		U8* reserve_start = (U8*)VirtualAlloc(nullptr, COROUTINE_STACK_SIZE_WITH_GUARD_PAGE, VirtualAllocType::RESERVE, 0);
		U8* commit_start = reserve_start + 4096;
		U8* commit_end = reserve_start + COROUTINE_STACK_SIZE_WITH_GUARD_PAGE;

		void* commit_start_2 = VirtualAlloc(commit_start, COROUTINE_STACK_SIZE, VirtualAllocType::COMMIT, VirtualMemoryProtection::READ | VirtualMemoryProtection::WRITE);
		AssertAlways(commit_start_2, "Out of memory");

		coro = (Coroutine*)(commit_end - sizeof(Coroutine));
		coro->reserve_start = reserve_start;
		coro->stack_end = (U32*)coro;
	}

	// entry arg1 is of type CoroutineHandle and will decrement refcount when the coroutine completes,
	// but it was never constructed, so we have to manually increment refcount:
	coro->refcount = 1;
	coro->blockers_count = 0;

	// Setup stack:
	*(coro->stack_end - 1) = (U32)coro->reserve_start + 4096;   // entry arg2 (userdata)
	*(coro->stack_end - 2) = (U32)coro;                         // entry arg1 (coroutine handle)
	*(coro->stack_end - 3) = (U32)bench_Resume_CoroCompleted;   // ret addr when coro completes
	// *(coro->stack_end - 4) = (U32)nullptr;                   // ret addr for the first Resume()
	*(coro->stack_end - 5) = (U32)coro;                         // EBP save slot/value of EBP after coro completes
	// *(stack->stack_end - 6) = 0;                             // EDI save slot
	// *(stack->stack_end - 7) = 0;                             // ESI save slot
	// *(stack->stack_end - 8) = 0;                             // EBX save slot
	coro->coro_esp = (U32)(coro->stack_end - 8);

	*out_userdata_storage = coro->reserve_start + 4096;
	*out_proc_location = (CoroutineProc*)(coro->stack_end - 4);

	g_scheduled_coroutines.Push(coro);
	return coro;
}

bool ExecScheduledCoroutines() {
	ArrayList<CoroutineHandle> coroutines = Move(g_scheduled_coroutines);

	for (const CoroutineHandle& handle : coroutines) {
		ResumeCoroutine(handle);
	}

	return coroutines.m_size > 0;
}

void BlockCoroutine(Coroutine* coro, int blockers_count) {
	coro->blockers_count += blockers_count;
}

void UnblockCurrentCoroutine(Coroutine* coro, int blockers_count) {
	coro->blockers_count -= blockers_count;
}

bool UnblockCoroutine(Coroutine* coro, int blockers_count) {
	if ((coro->blockers_count -= blockers_count) == 0) {
		g_scheduled_coroutines.Emplace(coro);
		return true;
	}
	return false;
}

}
