#include "smallsh.h"
#include "job.h"
#include <signal.h>

static struct sigaction act1;
static struct sigaction act2;
static struct sigaction act3;

pid_t search(struct nodeType);

void catchint(int signo){
    pid_t processid = search(*head);
    kill(processid, signo);
    deleteNode(processid);
    printf("\n");
}

void childDies(int signo){
// printf("1 Signal %d Caught\n", signo);

	int tempPid = waitpid(-1, NULL , WNOHANG); // Eliminates zombie process
	// Doesn't block parent process
	// TempPid equals to process that died(if equals to zero, then no process died or was available)

	// printf("Trying to delete %d\n", tempPid);

	if(tempPid == 0 ){ // if signal was the command
		return;
	}

	if(tempPid > 0){
		deletenode(tempPid); // Delete pid from linked list after just dying
		printf("%d was deleted\n", tempPid);
	}

  
}

void catchTSTP(int signo){
  int temp;

  temp = contBgStopFg();
  if(temp == -1){
    printf("No process to stop\n");
  }
}



static char inpbuf[MAXBUF], tokbuf[2*MAXBUF], *ptr = inpbuf, *tok = tokbuf;

int inarg(char c);

int userin(char *p)
{
  int c, count;

  ptr = inpbuf;
  tok = tokbuf;

  printf("%s", p);

  count = 0;

  while(1)
    {
      if((c = getchar()) == EOF)
          return(EOF);
      if(count < MAXBUF)
        inpbuf[count++] = c;
      if(c == '\n' && count < MAXBUF)
      {
        inpbuf[count] = '\0';
        return count;
      }
      if(c == '\n')
      {
        printf("smallsh: input line too long\n");
        count = 0;
        printf("%s ",p);
      }
    }
}

int toktype = gettok(&ptr);

int gettok(char **outptr)
{
  int type;

  *outptr = tok;

  while(*ptr == ' ' || *ptr == '\t')
    ptr++;

  *tok++ = *ptr;

  switch(*ptr++){
    case '\n':
      type = EOL;
      break;
    case '&':
      type = AMPERSAND;
      break;
    case ';':
      type = SEMICOLON;
      break;
    default:
      type = ARG;
      while(inarg(*ptr))
        *tok++ = *ptr++;
  }
  *tok++ = '\0';
  return type;
}

static char special [] = {' ', '\t', '&', ';', '\n', '\0'};
inarg(char c)
{
  char *wrk;

  for(wrk = special; *wrk; wrk++)
    {
      if(c == *wrk)
        return (0);
    }
  return (1);
}

int procline(void)
{
  char *arg[MAXARG + 1];
  int toktype;
  int narg;
  int type;

  narg = 0;

  for(;;) //runs an infinte loop!! :)
    {
      switch(toktype = gettok(&arg[narg])){
        case ARG: if(narg < MAXARG)
          narg++;
          break;
        case EOL:
        case SEMICOLON:
        case AMPERSAND:
          if(toktype == AMPERSAND)
            type = BACKGROUND;
          else
            type = FOREGROUND;

          if(narg != 0)
          {
            arg[narg] = NULL;
            runcommand(arg, type);
          }

          if(toktype == EOL)
            return;

          narg = 0;
          break;
          
      }
    }
}

int runcommand(char **cline, int where, int argCount)
{
  pid_t pid;
  int status;

if(strcmp(*cline, "jobs") == 0){
  printLinkedList();
  return 1;
}
if(strcmp(cline[0], "bg") == 0){
  if(cline[1] != NULL){
    int processid = (int)strtol(cline[1], (char **)NULL, 10);
    bgToRun(processid);

    kill(processid, SIGCONT);
    return 1;
  }
}

if(strcmp(cline[0], "kill") == 0){
  if(cline[1] != NULL){
    int processid = (int)strtol(cline[1], (char **)NULL, 10);

    kill(processid, SIGKILL);
    return -1;
  }
}

if(strcmp(cline[0], "fg") == 0){
  if(cline[1] != NULL){
    int processid = (int)strtol(cline[1], (char **)NULL, 10);

			int hold;

			// Calls function to turn a stopped or running background process
			// into a foreground process
			int processid1 = bgTofg(processid);


			if(processid1 == -1){ // If linked list is empty
				return -1;
			}

			if(processid1 == 1){ // if bg process was stopped prior

				kill(processid, SIGCONT); // First continue the bg process

				// Then wait for the process to terminate and stop. Makes into fg process
				if((hold = waitpid(processid, &status, WUNTRACED)) == -1){

					// printf("1 Returned %d\n", hold);
					//return 1;
				}else{

					if(WIFSTOPPED(status)){
						return 1;
					}

					printf("%d was deleted\n", processid);
					deletenode(processid);
					// printf("2 Returned %d\n", hold);
					//return 2;
				}

				return 1;

  }
}


  
  switch(pid = fork())
    {
      case -1:
        perror("smallsh");
        return (-1);
      case 0:
        execvp(*cline, cline);
        perror(*cline);
        exit(1);
    }
  newProcess.pid = pid;
      newProcess.whereground = where;
  newProcess.status = 1;
for(int i = 0; i < argCount; i++)
		strcat(newprocess.cmmds, cline[i]);
  
  if(where == BACKGROUND)
  {
    printf("[Process id %d]\n", pid);
    insertAfter(newProcess); //Put process at the end of LL.
    return (0);
  }
  else {
    insertBefore(newProcess);//Put the foreground process in the beg
    printLinkedList();
  }
    
  if(waitpid(pid, &status,0) == -1)
    return (-1);
  else{
    deleteNode(pid);
    return (status);
  }
}

char *prompt = "Command> ";


pid_t search(struct nodeType *head){
  struct nodeType *current;
  current = head;
  current->link = head;
  return current->data.pid;
}

int main ()
{
  

    static struct sigaction act;
  
    void catchint(int);
    void catchSTP(int);
    void catchCONT(int);
  
    act1.sa_handler = catchint;
    act2.sa_handler = childDies;
    act3.sa_handler = catchSTP;
  
    sigfillset(&(act1.sa_mask));
    sigfillset(&(act2.sa_mask));
    sigfillset(&(act3.sa_mask));
  
    while(userin(prompt) != EOF){
      //procline();
    sigaction(SIGINT, &act1, NULL); //^c
    sigaction(SIGCHILD, &act2, NULL); //^z stop
    sigaction(SIGTSTP, &act3, NULL); //fg resume paused pro
    procline();
    }

  return 0;
}

