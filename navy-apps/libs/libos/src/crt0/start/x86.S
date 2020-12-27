.globl  _start
_start:
  movl $0, %ebp
  #movl $0x1e4b00, %esp
  call call_main
