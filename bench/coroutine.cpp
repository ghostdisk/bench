#pragma once
#include <bench/coroutine.hpp>
#include <stdlib.h>
#include <Windows.h>
#include <atomic>
#include <vector>
#undef Yield // windows...

namespace bench {

struct Coroutine {
	U32* stack_low;
	U32* stack_high;
	U32 coro_esp;
	U32 orig_esp;
	std::atomic<int> refcount;
	void (*entry)(CoroutineHandle coro, void* userdata);
};

static bool Resume_CoroCompleted();

static std::vector<CoroutineHandle> k_scheduled_coroutines;

CoroutineHandle::CoroutineHandle() {
	this->state = nullptr;
}

CoroutineHandle::CoroutineHandle(Coroutine* state) {
	this->state = state;
	this->state->refcount++;
}

CoroutineHandle::CoroutineHandle(const CoroutineHandle& other) {
	this->state = other.state;
	this->state->refcount++;
}

CoroutineHandle::CoroutineHandle(CoroutineHandle&& other) noexcept {
	this->state = other.state;
	other.state = nullptr;
}

CoroutineHandle::~CoroutineHandle() {
	if (this->state && --this->state->refcount == 0) {
		VirtualFree(this->state->stack_low, 0, MEM_RELEASE);
		delete this->state;
	}
}

CoroutineHandle& CoroutineHandle::operator=(const CoroutineHandle& other) {
	return *this;
}

CoroutineHandle& CoroutineHandle::operator=(CoroutineHandle&& other) noexcept {
	state = other.state;
	other.state = nullptr;
	return *this;
}

CoroutineHandle::operator Coroutine*() const {
	return this->state;
}

CoroutineHandle CreateCoroutine(void (BENCHCOROAPI *entry)(CoroutineHandle coro, void* userdata), void* userdata) {
	Coroutine* coro = new Coroutine();

	coro->entry = entry;
	coro->stack_low = (U32*)VirtualAlloc(0, COROUTINE_STACK_SIZE_BYTES, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	coro->stack_high = coro->stack_low + COROUTINE_STACK_SIZE_DWORDS;
	AssertAlways(coro->stack_low, "Out of memory");

#ifdef BENCH_DEBUG
	memset(coro->stack_low, 0x14, COROUTINE_STACK_SIZE_BYTES);
#endif

	// ------------------------------------------------------------
	// Setup stack:

	coro->stack_low[0] = 0x13371337; // stack overflow guard

	*(coro->stack_high - 1) = (U32)userdata; // entry arg2
	*(coro->stack_high - 2) = (U32)coro; // entry arg1
	*(coro->stack_high - 3) = (U32)Resume_CoroCompleted; // ret addr when coro completes
	*(coro->stack_high - 4) = (U32)entry; // ret addr for the first Resume()
	*(coro->stack_high - 5) = (U32)coro; // value of EBP after coro completes
	// *(coro->stack_hgih - 6) = 0; // EDI
	// *(coro->stack_hgih - 7) = 0; // ESI
	// *(coro->stack_hgih - 8) = 0; // EBX 
	coro->coro_esp = (U32)(coro->stack_high - 8);

	// entry arg1 is of type CoroutineHandle and will decrement refcount when the coroutine completes,
	// but it was never constructed, so we have to manually increment refcount:
	coro->refcount++;

	return CoroutineHandle(coro);
}

static void CheckStackSmash(Coroutine* coro) {
	AssertAlways(coro->stack_low[0] == 0x13371337, "Coroutine stack overflow");
}

#pragma runtime_checks("", off)

static __declspec(naked) bool Resume_CoroCompleted() { // EBP: coro state
	__asm {
		PUSH EBP // coro.state
		CALL CheckStackSmash
		ADD ESP, 4

		MOV ESP, [EBP + 12] // coro.orig_stack

		POP EBX
		POP ESI
		POP EDI
		POP EBP

		MOV EAX, 1
		RET
	}
}

__declspec(naked) bool ResumeCoroutine(Coroutine* coro) {
	__asm {
		// save callee-saved registers onto original stack:
		PUSH EBP
		PUSH EDI
		PUSH ESI
		PUSH EBX

		MOV EAX, [ESP + 20] // current_coro.state
		MOV [EAX + 12], ESP // save original stack into the coroutine

		// load coro ESP:
		MOV ESP, [EAX + 8] // ESP = current_coro.state.coro_esp

		// load coro saved registers:
		POP EBX
		POP ESI
		POP EDI
		POP EBP

		RET
	}
}

void __declspec(naked) Yield(Coroutine* coro) {
	__asm {
		PUSH EBP
		MOV EBP, ESP

		PUSH [EBP + 8] // coro.state
		CALL CheckStackSmash
		ADD ESP, 4

		PUSH EDI
		PUSH ESI
		PUSH EBX

		MOV EAX, [EBP + 8] // current_coro.state
		MOV [EAX + 8], ESP // save coro stack into the coroutine

		// load original ESP:
		MOV ESP, [EAX + 12]

		POP EBX
		POP ESI
		POP EDI
		POP EBP

		MOV EAX, 0
		RET
	}
}

#pragma runtime_checks("", restore)

void ScheduleCoroutine(Coroutine* coro) {
	k_scheduled_coroutines.emplace_back(coro);
}

bool ExecScheduledCoroutines() {
	std::vector<CoroutineHandle> coroutines = std::move(k_scheduled_coroutines);
	k_scheduled_coroutines.clear();

	for (const CoroutineHandle& handle : coroutines) {
		ResumeCoroutine(handle);
	}

	return coroutines.size() > 0;
}

}
