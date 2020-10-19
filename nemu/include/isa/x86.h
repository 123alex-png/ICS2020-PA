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
  union{
    union{
        uint32_t _32;
        uint16_t _16;
        uint8_t _8[2];
    } gpr[8];
    

  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
  * in PA2 able to directly access these registers.
   */
  struct{
    union{
      rtlreg_t eax;
      union{
        uint16_t ax;
        struct{
          uint8_t al;
          uint8_t ah;
        };
      };
    };

    union{
      rtlreg_t ecx;
      union{
        uint16_t cx;
        struct{
          uint8_t cl;
          uint8_t ch;
        };
      };
    };

    union{
      rtlreg_t edx;
      union{
        uint16_t dx;
        struct{
          uint8_t dl;
          uint8_t dh;
        };
      };
    };

    union{
      rtlreg_t ebx;
      union{
        uint16_t bx;
        struct{
          uint8_t bl;
          uint8_t bh;
        };
      };
    };

    union{
      rtlreg_t esp;
      uint16_t sp;
    };

    union{
      rtlreg_t ebp;
      uint16_t bp;
    };

    union{
      rtlreg_t esi;
      uint16_t si;
    };

    union{
      rtlreg_t edi;
      uint16_t di;
    };
  };
  };
  
  // struct{
    rtlreg_t CF:1;
    rtlreg_t ZF:1;
    rtlreg_t SF:1;
    rtlreg_t IF:1;
    rtlreg_t OF:1;
  // }eflag;
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
