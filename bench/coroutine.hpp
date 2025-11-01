#pragma once
#include <bench/common.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

static constexpr int COROUTINE_STACK_SIZE_BYTES = (8 << 10);
static constexpr int COROUTINE_STACK_SIZE_DWORDS = COROUTINE_STACK_SIZE_BYTES / 4;

struct CoroutineState;

struct Coroutine {
	CoroutineState* state = nullptr;
};

Coroutine CreateCoroutine(void (BENCHCOROAPI *entry)(Coroutine coro, void* userdata), void* userdata);
void DestroyCoroutine(Coroutine coro);
void Yield(Coroutine current_coro);
bool ResumeCoroutine(Coroutine coro);

}