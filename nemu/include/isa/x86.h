#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0

// reg

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  struct {
    uint32_t _32;
    uint16_t _16;
    uint8_t _8[2];
  } gpr[8];

  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
  * in PA2 able to directly access these registers.
   */
  rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  union{
	gpr[0]._32 eax;
	gpr[0]._16 ax;
	gpr[0]._8 ah;
	gpr[0]._8 al;
  };
  union{
	gpr[1]._32 edx;
	gpr[1]._16 dx;
    gpr[1]._8 dh;
	gpr[1]._8 dl;
  }; 
  union{
	gpr[2]._32 ecx;
    gpr[2]._16 cx;
	gpr[2]._8 ch;
    gpr[2]._8 cl;
  };
  union{
	gpr[3]._32 ebx;
	gpr[3]._16 bx;
	gpr[3]._8 bh;
	gpr[3]._8 bl;
  };
  union{
	gpr[4]._32 ebp;
	gpr[4]._16 bp;
  };
  union{
	gpr[5]._32 esi;
	gpr[5]._16 si;
  };
  union{
	gpr[6]._32 edi;
	gpr[6]._16 di;
  };
  union{
	gpr[7]._32 es;
	gpr[7]._16 sp;
  };

  vaddr_t pc;
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
