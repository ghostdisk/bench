#pragma once
#include <bench/coroutine.hpp>
#include <stdlib.h>
#include <Windows.h>
#undef Yield // windows...

namespace bench {

struct CoroutineState {
	U32* stack_low;
	U32* stack_high;
	U32 coro_esp;
	U32 orig_esp;
	void (*entry)(Coroutine coro, void* userdata);
};

static bool Resume_CoroCompleted();

Coroutine CreateCoroutine(void (BENCHCOROAPI *entry)(Coroutine coro, void* userdata), void* userdata) {
	CoroutineState* state = new CoroutineState();

	state->entry = entry;

	state->stack_low = (U32*)VirtualAlloc(0, COROUTINE_STACK_SIZE_BYTES, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	state->stack_low = (U32*)malloc(COROUTINE_STACK_SIZE_BYTES);
	memset(state->stack_low, 0x14, COROUTINE_STACK_SIZE_BYTES);
	state->stack_high = state->stack_low + COROUTINE_STACK_SIZE_DWORDS;

	// ------------------------------------------------------------
	// Setup stack:

	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 1] = (U32)userdata; // arg1
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 2] = (U32)state; // arg2
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 3] = (U32)Resume_CoroCompleted; // ret addr when coro completes
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 4] = (U32)entry; // ret addr for the first Resume()
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 5] = (U32)state; // value of EBP after coro completes
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 6] = 0; // EDI dummy
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 7] = 0; // ESI dummy
	state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 8] = 0; // EBX dummy

	state->stack_low[0] = 0x13371337; // guard value
	state->coro_esp = (U32)&state->stack_low[COROUTINE_STACK_SIZE_DWORDS - 8];

	Coroutine coro = {};
	coro.state = state;
	return coro;
}

void DestroyCoroutine(Coroutine coro) {
	free(coro.state->stack_low);
	delete coro.state;
}

static void CheckStackSmash(Coroutine coro) {
	AssertAlways(coro.state->stack_low[0] == 0x13371337, "Coroutine stack overflow");
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

__declspec(naked) bool ResumeCoroutine(Coroutine coro) {
	__asm {
		PUSH EBP
		MOV EBP, ESP

		// save callee-saved registers onto original stack:
		PUSH EDI
		PUSH ESI
		PUSH EBX

		MOV EAX, [EBP + 8] // current_coro.state
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

void __declspec(naked) Yield(Coroutine current_coro) {
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

}
