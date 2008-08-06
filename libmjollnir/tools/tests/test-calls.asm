;
; 2008 (C) ERESI Project
;
; Taken from http://www.int80h.org/bsdasm/

; First simple 'hello world' test 

section	.text
global	_start
_start:
	call	function1
	mov	eax, 0x1
	int	0x80
	ret


function1:
	mov	eax,0x1
	mov	ebx,0x2
	add	eax,ebx
	call 	function2
	ret

function2:
	test	eax,eax
	call	function3
	ret

function3:
	test	ebx,ebx
	ret
