.globl  _start
_start:
  ret
  movl $0, %ebp
  push %eax
  call call_main
