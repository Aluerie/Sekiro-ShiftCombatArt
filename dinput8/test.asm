
.data

.code

tSekiroDebugMenuPrint PROC

	mov edx,[rbx]
	mov rcx,rsi
	mov [rax],edx
	sub rsp,088h

	add rsp,088h

	Return:


tSekiroDebugMenuPrint ENDP

END