.globl  _start
_start:
  movl $0, %ebp
  push %eax
  call call_main
