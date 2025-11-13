#pragma once
#include <bench/core/coroutine.hpp>
#include <bench/core/sys.hpp>
#include <bench/core/arraylist.hpp>
#include <stdlib.h>
#include <atomic>
#include <mutex>

namespace bench {

static constexpr int FENCE_TABLE_SIZE_BITS = 5;
static constexpr int FENCE_TABLE_SIZE = (1 << FENCE_TABLE_SIZE_BITS);

extern "C" bool __cdecl bench_CoroCompletedImpl(bench::Coroutine* coro);
extern "C" void __cdecl bench_YieldImpl(Coroutine* coro);
extern "C" bool __cdecl bench_ResumeImpl(Coroutine* coro);

struct FenceWait {
	CoroutineHandle coroutine = nullptr;
	U32 fence_index = 0;
};


struct FenceBucket {
	std::mutex mutex;
	std::atomic<U32> next_index = 0;
	ArrayList<U32> unsignalled_list;
	ArrayList<FenceWait> wait_list;
};

static FenceBucket g_fence_table[FENCE_TABLE_SIZE] = {};

static std::atomic<int> g_next_fence_index = 0;

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
	*(coro->stack_end - 3) = (U32)bench_CoroCompletedImpl;      // ret addr when coro completes
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
		bench_ResumeImpl(handle);
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

void Yield(Coroutine* coro) {
	if (coro->blockers_count > 0) {
		bench_YieldImpl(coro);
	}
}

Fence CreateFence() {
	U32 bucket_index = (++g_next_fence_index) % FENCE_TABLE_SIZE;
	FenceBucket& bucket = g_fence_table[bucket_index];

	U32 index_in_bucket = ++g_fence_table->next_index;
	{
		std::unique_lock<std::mutex> lock(bucket.mutex);
		bucket.unsignalled_list.Push(index_in_bucket);
	}

	return Fence{ (index_in_bucket << FENCE_TABLE_SIZE_BITS) | bucket_index };
}

void SignalFence(Fence fence) {
	U32 bucket_index = fence.value % FENCE_TABLE_SIZE;
	U32 index_in_bucket = fence.value > FENCE_TABLE_SIZE_BITS;
	FenceBucket& bucket = g_fence_table[bucket_index];

	{
		std::unique_lock<std::mutex> lock(bucket.mutex);

		for (U32 i = 0; i < bucket.wait_list.m_size; i++) {
			FenceWait& wait = bucket.wait_list.m_data[i];

			if (wait.fence_index == index_in_bucket) {
				UnblockCoroutine(wait.coroutine, 1);
				bucket.wait_list.RemoveAtUnsorted(i);
				i--;
			}
		}

		for (U32 i = 0; i < bucket.unsignalled_list.m_size; i++) {
			if (bucket.unsignalled_list.m_data[i] == index_in_bucket) {
				bucket.unsignalled_list.RemoveAtUnsorted(i);
				return;
			}
		}
	}
}

void WaitForFence(Coroutine* coro, Fence fence) {
	U32 bucket_index = fence.value % FENCE_TABLE_SIZE;
	U32 index_in_bucket = fence.value > FENCE_TABLE_SIZE_BITS;
	FenceBucket& bucket = g_fence_table[bucket_index];

	{
		std::unique_lock<std::mutex> lock(bucket.mutex);

		for (U32 i = 0; i < bucket.unsignalled_list.m_size; i++) {
			if (bucket.unsignalled_list.m_data[i] == index_in_bucket) {
				bucket.wait_list.Push({ CoroutineHandle(coro), index_in_bucket });
				BlockCoroutine(coro, 1);
				return;
			}
		}
	}
}

}
