#if !defined(FORKFILE_DATASTRUCTURE_H)
#define FORKFILE_DATASTRUCTURE_H

/* HONOR PLEDGE: I pledge on my honor that
   I have not given or received any unauthorized
   assistance on this assignment.
   Name: Arman Bolouri
   TerpConnect ID: abolouri
   UID: 117292084
   Discussion Section Number: 0206
*/

/* This forkfile data structure is composed of a linked list of rules (all
   of the type Forkfile), wherein there is no dummy head node. Each rule is
   composed of a string called target which stores the target string of the
   particular rule we're at and two arrays of strings (dependency_arr and
   action_arr), which respectively store all the dependencies and the singular
   words of the action command within them. The next parameter refers to a
   pointer to the next value in the linked list, which is terminated with
   NULL */  
typedef struct forkfile {
  char *target;
  char **dependency_arr;
  char **action_arr;
  struct forkfile *next;
} Forkfile;

#endif
