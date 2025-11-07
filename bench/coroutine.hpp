#pragma once
#include <bench/common.hpp>
#include <bench/utils/refhandle.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

struct Coroutine;


static constexpr int COROUTINE_STACK_SIZE_BYTES = (8 << 10);
static constexpr int COROUTINE_STACK_SIZE_DWORDS = COROUTINE_STACK_SIZE_BYTES / 4;

struct Coroutine;
using CoroutineHandle = RefHandle<Coroutine>;

CoroutineHandle StartCoroutine(void (BENCHCOROAPI *entry)(CoroutineHandle coro, void* userdata), void* userdata = nullptr);
bool ExecScheduledCoroutines();

extern "C" void __cdecl bench_Yield(Coroutine* coro);
extern "C" bool __cdecl bench_ResumeCoroutine(Coroutine* coro);

void BlockCoroutine(Coroutine* coro, int blockers_count);
bool UnblockCoroutine(Coroutine* coro, int blockers_count);

inline void Yield(bench::Coroutine* coro) {
	bench_Yield(coro);
}

inline bool ResumeCoroutine(bench::Coroutine* coro) {
	return bench_ResumeCoroutine(coro);
}

}
