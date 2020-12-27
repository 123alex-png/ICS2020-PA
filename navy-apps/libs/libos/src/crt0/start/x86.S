.globl  _start
_start:
  movl $0, %ebp
  #mov $_stack_pointer, %esp
  call call_main
