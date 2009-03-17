;
; 2008 (C) ERESI Project
;
; Taken from http://www.int80h.org/bsdasm/

; First simple 'hello world' test 

section	.text
global	_start
_start:
	align	4
	mov	eax,0x4
.bkwjump:
	mov	ecx,0x1
	sub	eax,ecx
	cmp	eax,0x0
	jne	.bkwjump
	mov	eax,0x5
.loop:	
	cmp	eax,0x0
	jne	.fwdjump
	jmp	.exit
.fwdjump:
	mov	ecx,0x1
	sub	eax,ecx
	jmp	.loop
.exit:
	mov	eax,0x1
	int	0x80
	ret
