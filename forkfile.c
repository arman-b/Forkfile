/* HONOR PLEDGE: I pledge on my honor that
   I have not given or received any unauthorized
   assistance on this assignment.
   Name: Arman Bolouri
   TerpConnect ID: abolouri
   UID: 117292084
   Discussion Section Number: 0206
*/

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

/* Reads in a forkfile of rules from the file by the name of filename,
   assigning the target, dependencies, and action command of each rule to a
   particular entry within our linked list data structure of rules connected
   to one another. Returns the new Forkfile data structure itself. */
Forkfile read_forkfile(const char filename[]) {
  Forkfile *curr;
  FILE *file;
  Forkfile *prev = NULL;
  
  /* Dynamically allocates memory for the new forkfile (which we actually
     don't need to technically do since we're not returning the pointer
     itself, but it's good practice if the address of the forkfile is ever
     needed or we need to free it later in main) */
  Forkfile *new_forkfile = malloc(sizeof(Forkfile));
  
  /* If the parameter file is unreadable, return null */
  if (filename == NULL)
    return *new_forkfile;

  /* If the file is unable to be read, return null */
  file = fopen(filename, "r");
  if (file == NULL)
    return *new_forkfile;
  
  curr = new_forkfile;
  /* Iterates through all the rules of the passed in forkfile, storing each
     target, list of dependencies, and action command as a singular rule
     within our data structure */
  while (1) {
    /* Declarations to utilize the split() function on the dependency and
       action lines */
    char **dependencies;
    char **action;
    char *dependency_line = malloc(strlen("a") * 1000);
    char *action_line = malloc(strlen("a") * 1000);
    
    /* Gets the singular string of dependencies (in addition to the target in
       front) */
    char *resultant = fgets(dependency_line, 1000, file);
    
    /* If we have reached the end of the file, fgets returns NULL, so we free
       the current rule we're at since it's not an actual rule of the file
       and return the head of the forkfile. */
    if (resultant == NULL) {
      free(curr);
      prev->next = NULL;
      return *new_forkfile;
    }

    /* If the first character of this line is either a newline or #, we
       know that the line is either a blank line or a comment, so we skip
       the current line and run the loop again */
    if (resultant[0] != '\n' && resultant[0] != '#') {
      /* We pass this line into the split function and get a pointer
	 signifying an array of the individual strings within the dependency
	 line separated by whitespace */
      dependencies = split(dependency_line);

      /* The target is the first element of this array, so we set the current
	 rule's target to that and increment dependencies by 1 to signify
	 a pointer to the start of the actual dependencies themselves
	 (not the target) */
      curr->target = malloc(strlen(*dependencies) + 1);
      strcpy(curr->target, *dependencies);
      dependencies++;

      /* Gets the singular string of the action command, and passes it into
	 split to get a pointer signifying the the array of each
	 individual word within this command seperated by whitespace. */ 
      fgets(action_line, 1000, file);
      action = split(action_line);
      
      /* Assigns the dependency and action arrays to the current rule in our
	 data structure, then iterates to the next rule utilizing a Tom and
	 Jerry traversal with both a curr and prev pointer (so we can set the
	 last entry of the forkfile data structure as NULL */
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
  
  /* Traverses through the rules of the forkfile, incrementing counter by 1
     for each rule found, and returning the current value of counter if a rule
     with the target of target_name is encountered. If we have gone through
     the entire LL and found no rule by the name of target_name, we
     return -1 */ 
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

  /* If the rule itself exists, traverses through the list of each individual
     word in the action command corresponding to this rule, linking them
     together with whitespace. We print the first element without a
     preceding space */
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

  /* Traverses through all the rules of the forkfile, printing the target,
     dependencies, and action line (all properly formatted) for each */
  while (curr != NULL) {
    char **action_arr;
    char **arr = curr->dependency_arr;
    printf("%s:", curr->target);

    /* Iterates through all the dependencies of the current rule, printing
       them all seprated by a space in between */
    while (*arr != NULL) {
      printf(" %s", *arr);
      arr++;
    }

    /* Prints the action line, same procedure as above
       but more efficient to not call the above function*/
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
    /* Prints a double newline if we're not at the last rule. This is
       essentially a blank line that puts the stdout pointer to the line
       after this blank line, so we can print the next rule. Prints just
       one newline if we're at the end of the forkfile */ 
    printf("\n");
    if (curr != NULL)
      printf("\n");
  }
}

/* Returns the number of dependencies ofr a certain rule, if the rule exists,
   returning -1 otherwise */
int num_dependencies(Forkfile forkfile, int rule_num) {
  int i;
  Forkfile *curr = &forkfile;

  /* Traverses to the (rule_num + 1)st rule, if this traversal is possible
     (meaning that all previous values are not null */
  for (i = 0; i < rule_num && curr != NULL; i++)
    curr = curr->next;

  /* Checks to see if this rule exists, otherwise, returns -1 */
  if (curr != NULL) {
    int counter = 0;
    /* Sets arr to the head of this rule's dependency array */
    char **arr = curr->dependency_arr;

    /* Traverses the dependency list of the current rule, incrementing counter
       by one for each dependency found, and returning counter (signifying the
       total number of dependencies), when the next element of the last
       dependency is reached (when *arr == NULL) */
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

  /* Traverses to the (rule_num + 1)st rule, if this traversal is possible
     (meaning that all previous values are not null */
  for (i = 0; i < rule_num && curr != NULL; i++)
    curr = curr->next;

  /* Checks to see if this rule exists, otherwise, returns NULL */
  if (curr != NULL) {
    /* Sets arr to the head of this rule's dependency array */
    char **arr = curr->dependency_arr;
    /* Traverses to the (dependency_num + 1)st dependency, if this traversal
       is possible */
    for (i = 0; i < dependency_num && *arr != NULL; i++)
      arr++;

    /* If this dependency does exist, we return the pointer to the char
       array (string) signifying it. Otherwise, we return null */
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
  
  /* sets the return value of stat into result */
  result = stat(filename, &stat_param);

  /* If stat() returned -1 and set errno to ENOENT, we kknow that the file does
     not exist, so we return 0. We return 1 otherwise. */
  if(result == -1 && errno == ENOENT)
    return 0;
  return 1;
}

/* Returns 1 if the file named filename1 was creeated at an earlier date than
   filename2 (if it's older), and returns 0 otherwise, even if one of the
   files don't even exist */
int is_older(const char filename1[], const char filename2[]) {
  /* Checks to make sure that both files exist, returns 0 otherwise. We check
     if both parameters are null in the exists() function already */
  if (exists(filename1) && exists(filename2)) {
    struct stat stat1;
    struct stat stat2;

    /* Calls stat() on both files */
    stat(filename1, &stat1);
    stat(filename2, &stat2);

    /* If the first file is older than the second file, returns 1; otherwise,
       returnns 0 */
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
  /* Checks to see that the rule itself exists within our data structure */
  if (num_dependencies(forkfile, rule_num) != -1) {
    pid_t pid = safe_fork();

    /* If we're at the parent process, we wait for the child process to
       finish, then return the exit status of the child, if it executed
       properly */
    if (pid > 0) {
      int status;
      wait(&status);
      if (WIFEXITED(status))
	return status;
    }

    /* If we're at the child process, we traverse to the correct rule,
       then replace the child process with the action command as written in the
       action_arr element/parameter of the current rule of the forkfile */
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
