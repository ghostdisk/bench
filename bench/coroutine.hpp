#pragma once
#include <bench/common.hpp>
#include <bench/utils/refhandle.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

static constexpr int COROUTINE_STACK_SIZE_BYTES = (8 << 10);
static constexpr int COROUTINE_STACK_SIZE_DWORDS = COROUTINE_STACK_SIZE_BYTES / 4;

struct Coroutine;
using CoroutineHandle = RefHandle<Coroutine>;

CoroutineHandle StartCoroutine(void (BENCHCOROAPI *entry)(CoroutineHandle coro, void* userdata), void* userdata = nullptr);
void Yield(Coroutine* coro);
bool ResumeCoroutine(Coroutine* coro);
void ScheduleCoroutine(Coroutine* coro);
bool ExecScheduledCoroutines();

}