#pragma once
#include <bench/common.hpp>

#define BENCHCOROAPI __cdecl

namespace bench {

static constexpr int COROUTINE_STACK_SIZE_BYTES = (8 << 10);
static constexpr int COROUTINE_STACK_SIZE_DWORDS = COROUTINE_STACK_SIZE_BYTES / 4;

struct CoroutineState;

class Coroutine {
public:
	CoroutineState* state = nullptr;

	Coroutine();
	Coroutine(CoroutineState*);
	Coroutine(const Coroutine& other);
	Coroutine(Coroutine&& other) noexcept;
	~Coroutine();
	Coroutine& operator=(const Coroutine& other);
	Coroutine& operator=(Coroutine&& other) noexcept;
	operator CoroutineState*() const;
};

Coroutine CreateCoroutine(void (BENCHCOROAPI *entry)(Coroutine coro, void* userdata), void* userdata);
void Yield(CoroutineState* coro);
bool ResumeCoroutine(CoroutineState* coro);

}