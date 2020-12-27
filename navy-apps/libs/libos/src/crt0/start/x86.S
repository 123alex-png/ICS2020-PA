.globl  _start
_start:
  movl $0, %ebp
  #mov %eax, %esp
  call call_main
