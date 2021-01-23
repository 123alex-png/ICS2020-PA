.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %esp
  push %eax
  call call_main
