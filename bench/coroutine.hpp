#pragma once
#include <bench/common.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

static constexpr int COROUTINE_STACK_SIZE_BYTES = (8 << 10);
static constexpr int COROUTINE_STACK_SIZE_DWORDS = COROUTINE_STACK_SIZE_BYTES / 4;

struct Coroutine;

class CoroutineHandle {
public:
	Coroutine* state = nullptr;

	CoroutineHandle();
	CoroutineHandle(Coroutine*);
	CoroutineHandle(const CoroutineHandle& other);
	CoroutineHandle(CoroutineHandle&& other) noexcept;
	~CoroutineHandle();
	CoroutineHandle& operator=(const CoroutineHandle& other);
	CoroutineHandle& operator=(CoroutineHandle&& other) noexcept;
	operator Coroutine*() const;
};

CoroutineHandle CreateCoroutine(void (BENCHCOROAPI *entry)(CoroutineHandle coro, void* userdata), void* userdata);
void Yield(Coroutine* coro);
bool ResumeCoroutine(Coroutine* coro);
void ScheduleCoroutine(Coroutine* coro);
bool ExecScheduledCoroutines();

}