#if !defined(FORKFILE_H)
#define FORKFILE_H
#include "forkfile-datastructure.h"

Forkfile read_forkfile(const char filename[]);
int lookup_target(Forkfile forkfile, const char target_name[]);
void print_action(Forkfile forkfile, int rule_num);
void print_forkfile(Forkfile forkfile);
int num_dependencies(Forkfile forkfile, int rule_num);
char *get_dependency(Forkfile forkfile, int rule_num, int dependency_num);
int exists(const char filename[]);
int is_older(const char filename1[], const char filename2[]);
int do_action(Forkfile forkfile, int rule_num);

#endif
