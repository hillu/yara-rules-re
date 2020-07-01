#include <stdio.h>
#include <unistd.h>
#include <search.h> /* tsearch */

#include "yrvm.h"

#define OP(name,sz,kind) {#name, OP_ ## name, sz, kind}

static YRVM_OP_DESCRIPTION op_descriptions[] = {
#if YR_VERSION_HEX >= 0x030700
     OP(NOP, 0, YRVM_OF_NONE),
#endif
     OP(HALT, 0, YRVM_OF_NONE),
#if YR_VERSION_HEX >= 0x040000
     OP(ITER_START_ARRAY, 0, YRVM_OF_NONE),
     OP(ITER_START_DICT, 0, YRVM_OF_NONE),
     OP(ITER_START_INT_RANGE, 0, YRVM_OF_NONE),
     OP(ITER_START_INT_ENUM, 0, YRVM_OF_NONE),
     OP(ITER_NEXT, 0, YRVM_OF_NONE),
#endif
     OP(PUSH, 8, YRVM_OF_UNKNOWN),
     OP(POP, 0, YRVM_OF_NONE),
     OP(CLEAR_M, 8, YRVM_OF_MEM),
     OP(ADD_M, 8, YRVM_OF_MEM),
     OP(INCR_M, 8, YRVM_OF_MEM),
     OP(PUSH_M, 8, YRVM_OF_MEM),
     OP(POP_M, 8, YRVM_OF_MEM),
#if YR_VERSION_HEX >= 0x030b00
     OP(SET_M, 8, YRVM_OF_UNKNOWN),
#endif
     OP(SWAPUNDEF, 8, YRVM_OF_UNKNOWN),
#if YR_VERSION_HEX >= 0x030b00
     OP(JNUNDEF, 4, YRVM_OF_JUMP),
#endif
#if YR_VERSION_HEX >= 0x040000
/*   OP(JUNDEF, 4, YRVM_OF_JUMP), */
     OP(JUNDEF_P, 4, YRVM_OF_JUMP),
     OP(JL_P, 4, YRVM_OF_JUMP),
#endif
#if YR_VERSION_HEX < 0x030b00
     OP(JLE, 4, YRVM_OF_JUMP),
#endif
#if YR_VERSION_HEX >= 0x030b00
     OP(JLE_P, 4, YRVM_OF_JUMP),
#endif
     OP(JTRUE, 4, YRVM_OF_JUMP),
#if YR_VERSION_HEX >= 0x040000
     OP(JTRUE_P, 4, YRVM_OF_JUMP),
#endif
#if YR_VERSION_HEX >= 0x030b00
     OP(JFALSE, 4, YRVM_OF_JUMP),
#endif
#if YR_VERSION_HEX >= 0x040000
     OP(JFALSE_P, 4, YRVM_OF_JUMP),
     OP(JZ, 4, YRVM_OF_JUMP),
     OP(JZ_P, 4, YRVM_OF_JUMP),
#endif
     OP(AND, 0, YRVM_OF_NONE),
     OP(OR, 0, YRVM_OF_NONE),
     OP(NOT, 0, YRVM_OF_NONE),
     OP(MOD, 0, YRVM_OF_NONE),
     OP(SHR, 0, YRVM_OF_NONE),
     OP(SHL, 0, YRVM_OF_NONE),
     OP(BITWISE_NOT, 0, YRVM_OF_NONE),
     OP(BITWISE_AND, 0, YRVM_OF_NONE),
     OP(BITWISE_OR, 0, YRVM_OF_NONE),
     OP(BITWISE_XOR, 0, YRVM_OF_NONE),
     OP(PUSH_RULE, 8, YRVM_OF_UNKNOWN),
#if YR_VERSION_HEX >= 0x040000
     OP(INIT_RULE, 8, YRVM_OF_INITRULE),
#else
     OP(INIT_RULE, sizeof(YR_INIT_RULE_ARGS), YRVM_OF_INITRULE),
#endif
     OP(MATCH_RULE, 8, YRVM_OF_RULE),
     OP(OBJ_LOAD, 8, YRVM_OF_CHARP),
     OP(OBJ_FIELD, 8, YRVM_OF_CHARP),
     OP(OBJ_VALUE, 0, YRVM_OF_NONE),
     OP(INDEX_ARRAY, 0, YRVM_OF_NONE),
     OP(LOOKUP_DICT, 0, YRVM_OF_NONE),
     OP(CALL, 8, YRVM_OF_CHARP),
     OP(FOUND, 0, YRVM_OF_NONE),
     OP(FOUND_AT, 0, YRVM_OF_NONE),
     OP(FOUND_IN, 0, YRVM_OF_NONE),
     OP(COUNT, 0, YRVM_OF_NONE),
     OP(OFFSET, 0, YRVM_OF_NONE),
     OP(LENGTH, 0, YRVM_OF_NONE),
     OP(OF, 0, YRVM_OF_NONE),
     OP(FILESIZE, 0, YRVM_OF_NONE),
     OP(ENTRYPOINT, 0, YRVM_OF_NONE),
     OP(INT8, 0, YRVM_OF_NONE),
     OP(INT16, 0, YRVM_OF_NONE),
     OP(INT32, 0, YRVM_OF_NONE),
     OP(UINT8, 0, YRVM_OF_NONE),
     OP(UINT16, 0, YRVM_OF_NONE),
     OP(UINT32, 0, YRVM_OF_NONE),
     OP(INT8BE, 0, YRVM_OF_NONE),
     OP(INT16BE, 0, YRVM_OF_NONE),
     OP(INT32BE, 0, YRVM_OF_NONE),
     OP(UINT8BE, 0, YRVM_OF_NONE),
     OP(UINT16BE, 0, YRVM_OF_NONE),
     OP(UINT32BE, 0, YRVM_OF_NONE),
     OP(CONTAINS, 0, YRVM_OF_NONE),
     OP(IMPORT, 8, YRVM_OF_CHARP),
     OP(MATCHES, 0, YRVM_OF_NONE),
     OP(INT_TO_DBL, 8, YRVM_OF_UNKNOWN),
     OP(STR_TO_BOOL, 0, YRVM_OF_NONE),
     OP(INT_EQ, 0, YRVM_OF_NONE),
     OP(INT_NEQ, 0, YRVM_OF_NONE),
     OP(INT_LT, 0, YRVM_OF_NONE),
     OP(INT_GT, 0, YRVM_OF_NONE),
     OP(INT_LE, 0, YRVM_OF_NONE),
     OP(INT_GE, 0, YRVM_OF_NONE),
     OP(INT_ADD, 0, YRVM_OF_NONE),
     OP(INT_SUB, 0, YRVM_OF_NONE),
     OP(INT_MUL, 0, YRVM_OF_NONE),
     OP(INT_DIV, 0, YRVM_OF_NONE),
     OP(INT_MINUS, 0, YRVM_OF_NONE),
     OP(DBL_LT, 0, YRVM_OF_NONE),
     OP(DBL_GT, 0, YRVM_OF_NONE),
     OP(DBL_LE, 0, YRVM_OF_NONE),
     OP(DBL_GE, 0, YRVM_OF_NONE),
     OP(DBL_EQ, 0, YRVM_OF_NONE),
     OP(DBL_NEQ, 0, YRVM_OF_NONE),
     OP(DBL_ADD, 0, YRVM_OF_NONE),
     OP(DBL_SUB, 0, YRVM_OF_NONE),
     OP(DBL_MUL, 0, YRVM_OF_NONE),
     OP(DBL_DIV, 0, YRVM_OF_NONE),
     OP(DBL_MINUS, 0, YRVM_OF_NONE),
     OP(STR_EQ, 0, YRVM_OF_NONE),
     OP(STR_NEQ, 0, YRVM_OF_NONE),
     OP(STR_LT, 0, YRVM_OF_NONE),
     OP(STR_LE, 0, YRVM_OF_NONE),
     OP(STR_GT, 0, YRVM_OF_NONE),
     OP(STR_GE, 0, YRVM_OF_NONE),
     OP(STR_EQ, 0, YRVM_OF_NONE),
     OP(STR_NEQ, 0, YRVM_OF_NONE),
     OP(STR_LT, 0, YRVM_OF_NONE),
     OP(STR_LE, 0, YRVM_OF_NONE),
     OP(STR_GT, 0, YRVM_OF_NONE),
     OP(STR_GE, 0, YRVM_OF_NONE),
     {NULL, 0, 0, 0},
};

#undef OP

YRVM_OP_DESCRIPTION* yrvm_get_op_description(uint8_t opcode)
{
     for(YRVM_OP_DESCRIPTION* od = op_descriptions; od->text != NULL; od++) {
          if (od->opcode == opcode) {
               return od;
          }
     }
     return NULL;
}

int tsearch_compar_int64(const void* pa, const void* pb)
{
     return (*(uint64_t *) pa - *(uint64_t *) pb);
}

/* yr_strings holds a search tree that stores addresses of YR_STRING entries. */
static void* yr_strings = NULL;

static void comment_for_operand(char* buf, size_t bufsz, YR_RULES* rules, int64_t operand)
{
     YR_RULE* rule;
     YR_STRING* string;
     if (yr_strings == NULL) {
          yr_rules_foreach(rules, rule) {
               yr_rule_strings_foreach(rule, string) {
                    uintptr_t *ptr = malloc(sizeof(uintptr_t));
                    *ptr = (uintptr_t)string;
                    void* v = tsearch(ptr, &yr_strings, tsearch_compar_int64);
                    if (*(uintptr_t**)v != ptr)
                         free(ptr);
               }
          }
     }
     if (yr_strings != NULL) {
          if (tfind(&operand, &yr_strings, tsearch_compar_int64) != NULL) {
#if YR_VERSION_HEX >= 0x040000
               uint32_t rule_idx = ((YR_STRING*)operand)->rule_idx;
               snprintf(buf, bufsz, "string <rule#%" PRIu32 " %s>.%s",
                        rule_idx,
                        rules->rules_list_head[rule_idx].identifier,
                        ((YR_STRING*)operand)->identifier);
#else
               snprintf(buf, bufsz, "string <rule %s>.%s",
                        ((YR_STRING*)operand)->rule->identifier,
                        ((YR_STRING*)operand)->identifier);
#endif
          }
     }
}

void yrvm_dump_code(int fd, YR_RULES* rules)
{
#if YR_VERSION_HEX >= 0x040000
     dprintf(fd,
             ";; arena: xrefs=%d buffers=%d\n"
             ";; code start: %08" PRIx64 "\n"
             ";; \n",
             rules->arena->xrefs, rules->arena->num_buffers);
#endif
     dprintf(fd, ";; code start: %08" PRIx64 "\n",
             (uint64_t)rules->code_start);

     const uint8_t* ip = rules->code_start;
     bool stop = 0;
     while(!stop) {
          uint8_t opcode = *ip;
          YRVM_OP_DESCRIPTION* od = yrvm_get_op_description(opcode);
          if (od == NULL) {
               dprintf(fd, "%08" PRIx32 " INVALID(%hhx)\n", (uint32_t)(ip-rules->code_start), opcode);
               ip++;
               continue;
          }
          char operand[40];
          char comment[128];
          int32_t addr;
          switch(od->operand_size) {
          case 0: operand[0] = 0; break;
          case 4:
               snprintf(operand, sizeof(operand), " 0x%08" PRIx32 , *(uint32_t*)(ip+1));
               break;
          case 8:
               snprintf(operand, sizeof(operand), " 0x%016" PRIx64 , *(uint64_t*)(ip+1));
               break;
          case 16:
               snprintf(operand, sizeof(operand), " 0x%016" PRIx64 ", %016" PRIx64,
                        *(uint64_t*)(ip+1), *(uint64_t*)(ip+9));
               break;
          default:
               snprintf(operand, sizeof(operand), " <%d bytes>", od->operand_size);
          }
          comment[0] = 0;
          switch(od->operand_kind) {
          case YRVM_OF_JUMP:
               addr = *(int32_t*)(ip+1);
               snprintf(comment, sizeof(comment), "-> %08" PRIx32 " (%+" PRIi32 ")",
                        (uint32_t)(ip-rules->code_start+addr), addr);
               break;
          case YRVM_OF_CHARP:
               snprintf(comment, sizeof(comment), "\"%s\"", *(char**)(ip+1));
               break;
          case YRVM_OF_MEM:
               snprintf(comment, sizeof(comment), "mem[%" PRIi64 "]", *(int64_t*)(ip+1));
               break;
          case YRVM_OF_UNKNOWN:
               if ( *(uint64_t*)(ip+1) == 0xFFFABADAFABADAFFLL ) {
                    snprintf(comment, sizeof(comment), "undefined");
               } else {
                    comment_for_operand(comment, sizeof(comment), rules, *(uint64_t*)(ip+1));
               }
               break;
          case YRVM_OF_RULE:
          {
#if YR_VERSION_HEX >= 0x040000
               uint64_t rule_idx = *(uint64_t*)(ip+1);
               YR_RULE* rule = rules->rules_list_head + rule_idx;
               snprintf(comment, sizeof(comment), "rule#%" PRIu64 " <%s.%s>",
                        rule_idx, rule->ns->name, rule->identifier);
#else
               YR_RULE* rule = *(YR_RULE**)(ip+1);
               snprintf(comment, sizeof(comment), "rule <%s.%s>",
                        rule->ns->name, rule->identifier);
#endif
          }
          break;
          case YRVM_OF_INITRULE:
#if YR_VERSION_HEX >= 0x040000
          {
               addr = *(int32_t*)(ip+1);
               uint32_t rule_idx = *(uint32_t*)(ip+5);
               snprintf(comment, sizeof(comment), "rule#%" PRIu32 " <%s>; next = %08" PRIx32 " (%+" PRIi32 ")",
                        rule_idx, rules->rules_list_head[rule_idx].identifier,
                        (uint32_t)(ip-rules->code_start+addr), addr);
          }
#else
          {
               YR_INIT_RULE_ARGS* init_arg = (YR_INIT_RULE_ARGS*)(ip+1);
               snprintf(comment, sizeof(comment), "rule <%s.%s>; next = %08" PRIx64,
                        init_arg->rule->ns->name, init_arg->rule->identifier,
                        init_arg->jmp_addr - rules->code_start);
          }
#endif
          break;
          default:
               break;
          }
          dprintf(fd, "%08" PRIx32 ": %12s%s", (uint32_t)(ip-rules->code_start), od->text, operand);
          if (comment[0]) {
               if (od->operand_kind == YRVM_OF_JUMP) {
                    dprintf(fd, "        ");
               }
               dprintf(fd, " ; %s", comment);
          }
          write(fd, "\n", 1);
          if (opcode == OP_HALT) {
               stop = 1;
          }
          ip += 1+od->operand_size;
     }
}
