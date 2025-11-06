PUBLIC bench_Resume_CoroCompleted
PUBLIC bench_ResumeCoroutine
PUBLIC bench_Yield

.MODEL FLAT,C
.CODE

bench_Resume_CoroCompleted PROC
	; PUSH EBP ; coro.state
	; CALL CheckStackSmash
	; ADD ESP, 4

	MOV ESP, [EBP + 12] ; coro.orig_stack

	POP EBX
	POP ESI
	POP EDI
	POP EBP

	MOV EAX, 1
	RET
bench_Resume_CoroCompleted ENDP

bench_ResumeCoroutine PROC
	; save callee-saved registers onto original stack:
	PUSH EBP
	MOV EBP, ESP
	PUSH EDI
	PUSH ESI
	PUSH EBX

	MOV EAX, [EBP + 8] ; current_coro.state
	MOV [EAX + 12], ESP ; save original stack into the coroutine

	; load coro ESP:
	MOV ESP, [EAX + 8] ; ESP = current_coro.state.coro_esp


	; load coro saved registers:
	POP EBX
	POP ESI
	POP EDI
	POP EBP

	RET
bench_ResumeCoroutine ENDP

bench_Yield PROC
	PUSH EBP
	MOV EBP, ESP

	; PUSH [EBP + 8] ; coro.state
	; CALL CheckStackSmash
	; ADD ESP, 4

	PUSH EDI
	PUSH ESI
	PUSH EBX

	MOV EAX, [EBP + 8] ; current_coro.state
	MOV [EAX + 8], ESP ; save coro stack into the coroutine

	; load original ESP:
	MOV ESP, [EAX + 12]

	POP EBX
	POP ESI
	POP EDI
	POP EBP

	MOV EAX, 0
	RET
bench_Yield ENDP

END
