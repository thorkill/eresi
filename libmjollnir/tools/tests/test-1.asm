;
; 2008 (C) ERESI Project
;
; Taken from http://www.int80h.org/bsdasm/

; First simple 'hello world' test 

%include	'system.inc'

section	.data
hello	db	'Hello, World!', 0Ah
hbytes	equ	hello

section	.text
global	_start
_start:
    push	dword hbytes
    push	dword hello
    push	dword stdout
sys.write
    push	dword 0
sys.exit
