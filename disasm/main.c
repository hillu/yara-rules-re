#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <yara.h>

#include "yrvm.h"

int main(int argc, char** argv)
{
     if (argc != 2) {
          fprintf(stderr, "usage: %s <rule-file>\n", argv[0]);
          exit(1);
     }
     YR_COMPILER* c;
     YR_RULES* rs;
     int result;
     yr_initialize();

     result = yr_rules_load(argv[1], &rs);
     if (result != 0) {
          yr_compiler_create(&c);
          int fd = open(argv[1], O_RDONLY);
          if (fd < 0) {
               fprintf(stderr, "%s: open %s: %s\n", argv[0], argv[1], strerror(errno));
               exit(1);
          }
          result = yr_compiler_add_fd(c, fd, "", argv[1]);
          if (result != 0) {
               fprintf(stderr, "%s: yr_compiler_get_rules: %d\n", argv[0], result);
               exit(1);
          }
          result = yr_compiler_get_rules(c, &rs);
          if (result != 0) {
               fprintf(stderr, "%s: yr_compiler_get_rules: %d\n", argv[0], result);
               exit(1);
          }
     }
     yrvm_dump_code(STDOUT_FILENO, rs);
}
