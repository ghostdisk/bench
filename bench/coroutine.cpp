#pragma once
#include <bench/coroutine.hpp>
#include <bench/sys.hpp>
#include <bench/arraylist.hpp>
#include <stdlib.h>
#include <atomic>

namespace bench {

extern "C" bool __cdecl bench_Resume_CoroCompleted(bench::Coroutine* coro);

struct CoroutineStack {
	std::atomic<CoroutineStack*> next_stack = nullptr;
	U32* low;
	U32* high;
};

struct Coroutine {
	CoroutineStack* stack = nullptr;
	void* pad0 = nullptr;
	U32 coro_esp;
	U32 orig_esp;
	std::atomic<int> refcount;
	std::atomic<int> blockers_count;
};

struct StackPool {
	std::atomic<CoroutineStack*> first_free = nullptr;
	std::atomic<int> count = 0;

	CoroutineStack* Pop() {
		CoroutineStack* stack = nullptr;
		do {
			stack = first_free;
			if (!first_free)
				return nullptr;
		} while (!first_free.compare_exchange_weak(stack, stack->next_stack));

		count--;
		return stack;
	}

	void Push(CoroutineStack* new_stack) {
		count++;
		CoroutineStack* old_first_free = nullptr;
		do {
			old_first_free = this->first_free.load();
			new_stack->next_stack = old_first_free;
		} while (!first_free.compare_exchange_weak(old_first_free, new_stack));
	}
};

static ArrayList<CoroutineHandle> g_scheduled_coroutines;
static StackPool g_stack_pool;

CoroutineStack* AcquireStack() {
	CoroutineStack* from_pool = g_stack_pool.Pop();
	if (from_pool) {
		return from_pool;
	}
	else {
		CoroutineStack* stack = new CoroutineStack();

		stack->low = (U32*)VirtualAlloc(nullptr, COROUTINE_STACK_SIZE_BYTES, VirtualAllocType::COMMIT, VirtualMemoryProtection::READ | VirtualMemoryProtection::WRITE);
		stack->high = stack->low + COROUTINE_STACK_SIZE_DWORDS;
		return stack;
	}
}

void ReleaseStack(CoroutineStack* stack) {
	g_stack_pool.Push(stack);
}

template <>
void AddRef(Coroutine* coro) {
	++coro->refcount;
}

template <>
void RemoveRef(Coroutine* coro) {
	if (--coro->refcount == 0) {
		ReleaseStack(coro->stack);
		delete coro;
	}
}


CoroutineHandle CreateCoroutine(void** out_userdata_storage, CoroutineProc** out_proc_location) {
	Coroutine* coro = new Coroutine();
	CoroutineStack* stack = AcquireStack();
	coro->stack = stack;

#ifdef BENCH_DEBUG
	memset(coro->stack_low, 0x14, COROUTINE_STACK_SIZE_BYTES);
#endif

	// ------------------------------------------------------------
	// Setup stack:

	*(stack->high - 1) = (U32)stack->low;                 // entry arg2 (userdata)
	*(stack->high - 2) = (U32)coro;                       // entry arg1 (coroutine handle)
	*(stack->high - 3) = (U32)bench_Resume_CoroCompleted; // ret addr when coro completes
	*(stack->high - 4) = (U32)nullptr;                    // ret addr for the first Resume()
	*(stack->high - 5) = (U32)coro;                       // EBP save slot/value of EBP after coro completes
	// *(stack->high - 6) = 0;                            // EDI save slot
	// *(stack->high - 7) = 0;                            // ESI save slot
	// *(stack->high - 8) = 0;                            // EBX save slot
	coro->coro_esp = (U32)(stack->high - 8);

	*out_userdata_storage = stack->low;
	*out_proc_location = (CoroutineProc*)(stack->high - 4);

	// entry arg1 is of type CoroutineHandle and will decrement refcount when the coroutine completes,
	// but it was never constructed, so we have to manually increment refcount:
	coro->refcount++;

	// g_scheduled_coroutines.Push(coro);
	return coro;
}

bool ExecScheduledCoroutines() {
	ArrayList<CoroutineHandle> coroutines = Move(g_scheduled_coroutines);

	// TODO!!! If we do `const CoroutineHandle&` here, MSVC generates broken code on /O2.
	for (const CoroutineHandle& handle : coroutines) {
		ResumeCoroutine(handle);
	}

	return coroutines.m_size > 0;
}

void BlockCoroutine(Coroutine* coro, int blockers_count) {
	coro->blockers_count += blockers_count;
}

bool UnblockCoroutine(Coroutine* coro, int blockers_count) {
	if ((coro->blockers_count -= blockers_count) == 0) {
		g_scheduled_coroutines.Emplace(coro);
		return true;
	}
	return false;
}

}
