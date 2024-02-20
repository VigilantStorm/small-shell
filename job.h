#include "smallsh.h"
#include <string.h>

struct processInformation {
  pid_t pid;
  int status;
  int whereground;
  char cmmds[100];
  int cmdCount;
  int jobnumber;
}newProcess;

struct nodeType {
  struct nodeType *link;
  struct processInformation data;
};

struct nodeType *head = NULL;
struct nodeType *tail = NULL;

void insertBefore(struct processInformation data){
  struct nodeType *newNode;
  newNode = (struct nodeType *) malloc(sizeof(struct nodeType));
  if(head == NULL) //If the linked list is empty do below
  {
    head = newNode; //Have linked list head point to newNode
    tail = newNode;//Have linked list tail point to newNode
    newNode->data = data;
  }

  else if(head != NULL) //If the linked list has nodes already do below
  {
    newNode->link = head;
    head = newNode; //Inserts node at the begininng of the linked lists
    newNode->data = data;
  }
}

void insertAfter(struct processInformation data){
  struct nodeType *newNode;
  if (head == NULL){
    head = newNode;
    tail = newNode; 
    newNode->data = data;
  }
  else if (head != NULL){
    tail->link = newNode;
    tail = newNode;
    newNode->data = data; 
  } 
}

void deleteNode(int key){

    // Store the head node in the current variable
    struct nodeType *current = head;
    struct nodeType *prev = NULL;

    // If the head node is itself the key to be deleted
    if (current != NULL && current->data.pid == key){
        head = current->link; // Changed the head to the current->next
        free(current);            // free the old head
        return;
    }

    // Search for the key to be deleted and also keep track of the previous node
    else{
        while (current != NULL && current->data.pid != key){
            prev = current;
            current = current->link;
        }

        // If the key to be deleted is not present in the linked list
        if (current == NULL)
            return;

        // Key is found, so unlink the current node
        prev->link = current->link;

        // Delete the current node
        free(current);
    }
}

void printLinkedList()
{
    if(head==NULL)    //condition to check whether list is empty
    {
        printf("list is empty\n");
        return;
    }
    struct nodeType *cur=head;
    int count;
    count=0;
    printf("Job#\tPid\tStatus\tGround\t\tCommand\n");
    while(cur!=NULL)                  //the loop traverse until it gets any NULL node
    {
    	char tempS1[20] = "";
    	char tempS2[20] = "";

    	if(cur->data.status == 0){
    		strcpy(tempS1, "Stopped");
    	}else{
    		strcpy(tempS1, "Running");
    	}

    	if(cur->data.ground == 1){
    		strcpy(tempS2, "Foreground");
    	}else{
    		strcpy(tempS2, "Background");
    	}

        printf("%d\t%d\t%s\t%s\t%s\n",cur->data.jobnumber ,cur->data.pid, tempS1, tempS2, cur->data.cmmds);


        count++;                      //counts the number of nodes or elements present in list
        cur=cur->next;                //moves cur pointer to next node to check and get value
    }
}

void deleteatbegin(){
   head = head->next;
}
void deleteatend(){
   struct nodeType *linkedlist = head;
   while (linkedlist->next->next != NULL)
      linkedlist = linkedlist->next;
   linkedlist->next = NULL;
}

void deletenode(pid_t key){
   struct nodeType *temp = head, *prev;
   if (temp != NULL && temp->data.pid == key) {
      head = temp->next;
      return;
   }

   // Find the key to be deleted
   while (temp != NULL && temp->data.pid != key) {
      prev = temp;
      temp = temp->next;
   }

   // If the key is not present
   if (temp == NULL) return;

   // Remove the node
   prev->next = temp->next;
}

int contBgStopFg() // Changes fg process status to stopped and ground to bg
{
    struct nodeType* temp ;
	temp = head;
	if(temp == NULL){
		return -1;
	}
	while (temp != NULL) // Finds fg process
	{
		if (temp->data.ground == 0){ // If bg process
			if(temp->data.status == 0){ // if bg process was already stopped before ctr Z
				temp = temp->next; // Go to next node/process
				continue;
			}
			temp->data.status = 1; // if bg process was running
			kill(temp->data.pid, SIGCONT);
		}else if(temp->data.ground == 1){ // if fg process
			temp->data.status = 0;
			temp->data.ground = 0;
			kill(temp->data.pid, SIGSTOP);
		}
		temp = temp->next;
	}
	return 0 ;
}

void bgToRun(int pid) // change stopped bg process to running bg process
{
    if(head==NULL) // if there is no node/job
        printf("list is empty and nothing to delete\n");

    struct nodeType* cur=head; // node ptr(cur) points to first node/job

    while(cur->data.pid != pid) // Will move cur ptr until pid is found
    {
        cur=cur->next;
    }

    cur->data.status = 1; // Changed node/job status value to running

}


int bgTofg(int pid) // Changes stopped or running bg process to run in fg
{
    if(head==NULL){ // if there is no node/job
        printf("list is empty and nothing to delete\n");
        return -1;
    }
    struct nodeType* cur=head; // node ptr(cur) points to first node/job

    while(cur->data.pid != pid) // Will move cur ptr until pid is found
    {
    	if(cur->next == NULL){
    		break;
    	}
        cur=cur->next;
    }


    if(cur->data.status == 0){ // if bg process was stopped
    	cur->data.status = 1; // Changed job status value to running
    	cur->data.ground = 1; // Changed job to foreground
    	printLinkedList();
    	return 1; // If 1, then bg process was stopped before bgTofg
    }

    cur->data.ground = 1; // Changed to foreground
    printLinkedList();
    return 2; // If 2, then bg process was running before bgTofg
