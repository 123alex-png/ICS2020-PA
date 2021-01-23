.globl  _start
_start:
  movl $0, %ebp
  #movl %ebx, %esp
  push %eax
  call call_main
