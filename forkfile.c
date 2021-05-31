/* Within this program, we develop a data structure and corresponding
   functions to attempt to replicate a makefile, which we call a forkfile
   (since we utilize a fork() call (child process) to actually perform the
   command of a certain rule in the makefile). This forkfile data structure
   is composed of a linked list of rules (all of the type Forkfile), wherein
   there is no dummy head node. Each rule is composed of a string called
   target which stores the target string of the  particular rule we're at and
   two arrays of strings (dependency_arr and action_arr), which respectively
   store all the dependencies and the singular words of the action command
   within them. The next parameter refers to a pointer to the next value in
   the linked list, which is terminated with NULL. We have a total of 9
   functions that correspond to this data structure: reading in a forkfile and
   storing it into our forkfile data structure, returning it; returning the
   rule number of a particular target within the forkfile passed in;
   printing a certain action in one of the forkfile's rules; printing the
   forkfile in its entirety, in the same format as a normal makefile/forkfile;
   returning the number of dependencies in a particular rule of a forkfile;
   returning the ith dependency of the jth rule of the forkfile; and
   performing the actual action itself of a certain rule. The other two
   functions aren't directly related, but test whether two files exist and
   which one is older than the other in regards to date/time created.  */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "forkfile.h"
#include "safe-fork.h"
#include "split.h"

Forkfile read_forkfile(const char filename[]) {
  Forkfile *curr;
  FILE *file;
  Forkfile *prev = NULL;
  Forkfile *new_forkfile = malloc(sizeof(Forkfile));
  if (filename == NULL)
    return *new_forkfile;
  file = fopen(filename, "r");
  if (file == NULL)
    return *new_forkfile;
  
  curr = new_forkfile;
  while (1) {
    char **dependencies;
    char **action;
    char *dependency_line = malloc(strlen("a") * 1000);
    char *action_line = malloc(strlen("a") * 1000);
    char *resultant = fgets(dependency_line, 1000, file);
    
    if (resultant == NULL) {
      free(curr);
      prev->next = NULL;
      return *new_forkfile;
    }

    if (resultant[0] != '\n' && resultant[0] != '#') {
      dependencies = split(dependency_line);

      curr->target = malloc(strlen(*dependencies) + 1);
      strcpy(curr->target, *dependencies);
      dependencies++;

      fgets(action_line, 1000, file);
      action = split(action_line);
      
      curr->dependency_arr = dependencies;
      curr->action_arr = action;
      prev = curr;
      curr->next = malloc(sizeof(Forkfile));
      curr = curr->next; 
    }
  }   
}

/* Returns the corresponding rule number of a target, if the rule itself
   exists within the Forkfile. If not, returns -1 */ 
int lookup_target(Forkfile forkfile, const char target_name[]) {
  int counter = 0;
  Forkfile *curr = &forkfile;
  if (target_name == NULL)
    return -1;
 
  while (curr != NULL) {
    if (!strcmp(curr->target, target_name))
      return counter;

    counter++;
    curr = curr->next;
  }
  return -1;
}

/* Prints the certain action string corresponding to the (rule_num + 1)st rule,
   not including the tab at the beginning of this action */
void print_action(Forkfile forkfile, int rule_num) {
  int i;
  Forkfile *curr = &forkfile;
  /* Traverses to proper rule */
  for (i = 0; i < rule_num && curr != NULL; i++)
    curr = curr->next;

  if (curr != NULL) {
    char **arr = curr->action_arr;

    /* Prints first value, without initial space */
    if (*arr != NULL) {
      printf(*arr);
      arr++;
    }

    /* Prints rest */
    while (*arr != NULL) {
      printf(" %s", *arr);
      arr++;
    }
    printf("\n");
  }
}

/* Prints the entire forkfile in the same format that it was entered in the
   file named by filename in the read_forkfile() function. However, it does
   this without all the potential unnecessary/extra whitespace that may have
   been included in the filename file. Prints each rule in the two lines
   it's meant to format, and separates all the rules with singular empty
   lines */
void print_forkfile(Forkfile forkfile) {
  Forkfile *curr = &forkfile;
  while (curr != NULL) {
    char **action_arr;
    char **arr = curr->dependency_arr;
    printf("%s:", curr->target);

    while (*arr != NULL) {
      printf(" %s", *arr);
      arr++;
    }

    printf("\n\t");
    action_arr = curr->action_arr;
    if (*action_arr != NULL) {
      printf(*action_arr);
      action_arr++;
    }
    while (*action_arr != NULL) {
      printf(" %s", *action_arr);
      action_arr++;
    }
    
    curr = curr->next;
    printf("\n");
    if (curr != NULL)
      printf("\n");
  }
}

int num_dependencies(Forkfile forkfile, int rule_num) {
  int i;
  Forkfile *curr = &forkfile;

  for (i = 0; i < rule_num && curr != NULL; i++)
    curr = curr->next;

  if (curr != NULL) {
    int counter = 0;
    char **arr = curr->dependency_arr;
    while (*arr != NULL) {
      arr++;
      counter++;
    }
    return counter;
  }

  return -1;
}

/* Returns a string (pointer to char / char array) to the
   (dependency_num + 1)st dependency of the (rule_num + 1)st rule, if both of
   these exist */
char *get_dependency(Forkfile forkfile, int rule_num, int dependency_num) {
  int i;
  Forkfile *curr = &forkfile;
  for (i = 0; i < rule_num && curr != NULL; i++)
    curr = curr->next;
  if (curr != NULL) {
    char **arr = curr->dependency_arr;
    for (i = 0; i < dependency_num && *arr != NULL; i++)
      arr++;
    if (*arr != NULL)
      return *arr;
  }
  return NULL;
}

/* Checks to see whether the file named by filename exists or not, returning
   1 if it does and 0 otherwise */
int exists(const char filename[]) {
  struct stat stat_param;
  int result;
  errno = 0;

  if (filename == NULL)
    return 0;
  
  result = stat(filename, &stat_param);
  if(result == -1 && errno == ENOENT)
    return 0;
  return 1;
}

/* Returns 1 if the file named filename1 was creeated at an earlier date than
   filename2 (if it's older), and returns 0 otherwise, even if one of the
   files don't even exist */
int is_older(const char filename1[], const char filename2[]) {
  if (exists(filename1) && exists(filename2)) {
    struct stat stat1;
    struct stat stat2;
    stat(filename1, &stat1);
    stat(filename2, &stat2);
    if (stat1.st_mtime < stat2.st_mtime)
      return 1;
  }
  return 0;
}

/* Does the particular action denoted by the (rule_num + 1)st rule of the
   forkfile data structure. The child process we create by forking replaces
   itself with the command corresponding to this rule and executes, wherein
   the parent process returns the exit status of the child process. */
int do_action(Forkfile forkfile, int rule_num) {
  if (num_dependencies(forkfile, rule_num) != -1) {
    pid_t pid = safe_fork();
    if (pid > 0) {
      int status;
      wait(&status);
      if (WIFEXITED(status))
	return status;
    }
    else if (!pid) {
      int i;
      Forkfile *curr = &forkfile;
      
      /* Traverses to proper rule */
      for (i = 0; i < rule_num && curr != NULL; i++)
        curr = curr->next;

      /* Replaces the child process with the command to be run */
      execvp(curr->action_arr[0], curr->action_arr);

      /* Returns -1, as execv should not return if it executes properly */
      return -1;
    }
  }
  return -1;
}
