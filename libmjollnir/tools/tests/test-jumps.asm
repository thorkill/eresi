;
; 2008 (C) ERESI Project
;
; Taken from http://www.int80h.org/bsdasm/

; First simple 'hello world' test 

section	.text
global	_start
_start:
	align	4
	test	ebx,ebx
	mov	eax,0x3
	test	ecx,ecx
.bkwjump:
	mov	ecx,0x3
	sub	eax,ecx
	test	eax,eax
	cmp	ecx,eax
	je	.bkwjump
	mov	eax,0x1
	int	0x80
	ret
