PUBLIC bench_CoroCompletedImpl
PUBLIC bench_ResumeImpl
PUBLIC bench_YieldImpl

.MODEL FLAT,C
.CODE

bench_CoroCompletedImpl PROC
	MOV ESP, [EBP + 12] ; coro.orig_stack
	POP EBX
	POP ESI
	POP EDI
	POP EBP

	MOV EAX, 1
	RET
bench_CoroCompletedImpl ENDP

bench_ResumeImpl PROC
	; save callee-saved registers onto original stack:
	PUSH EBP
	PUSH EDI
	PUSH ESI
	PUSH EBX

	MOV EAX, [ESP + 20] ; current_coro.state (this would be EBP+8 if we did a proper stack frame)
	MOV [EAX + 12], ESP ; save original stack into the coroutine

	; load coro ESP:
	MOV ESP, [EAX + 8] ; ESP = current_coro.state.coro_esp


	; load coro saved registers:
	POP EBX
	POP ESI
	POP EDI
	POP EBP

	RET
bench_ResumeImpl ENDP

bench_YieldImpl PROC
	PUSH EBP
	PUSH EDI
	PUSH ESI
	PUSH EBX

	MOV EAX, [ESP + 20] ; current_coro.state (this would be EBP+8 if we did a proper stack frame)
	MOV [EAX + 8], ESP ; save coro stack into the coroutine

	; load original ESP:
	MOV ESP, [EAX + 12]

	POP EBX
	POP ESI
	POP EDI
	POP EBP

	MOV EAX, 0
	RET
bench_YieldImpl ENDP

END
