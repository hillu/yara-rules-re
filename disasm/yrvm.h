#ifndef _DISASM_H
#define _DISASM_H

#include <yara.h>
// #include <yara/exec.h>

typedef enum YRVM_OPERAND_FORMAT
{
 YRVM_OF_UNKNOWN = 0,
 YRVM_OF_NONE,
 YRVM_OF_JUMP,
 YRVM_OF_CHARP,
 YRVM_OF_MEM,
 YRVM_OF_RULE,
 YRVM_OF_INITRULE,
} YRVM_OPERAND_FORMAT;

typedef struct YRVM_OP_DESCRIPTION
{
  char* text;
  uint8_t opcode;
  uint8_t operand_size;
  YRVM_OPERAND_FORMAT operand_kind;
} YRVM_OP_DESCRIPTION;

YRVM_OP_DESCRIPTION* yrvm_get_op_description(uint8_t opcode);
void yrvm_dump_code(int fd, YR_RULES* rs);

#endif /* _DISASM_H */
