#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Node.h"

/**
 *CommandNode.c
 *Author: Bill Andreopoulos
 *The function bodies for a linked list of commands
 *CS149 assignment 3 usage only
**/
/**
   Example of linked list initialization in code:
   //First command node (head of list)
   headCommand = (CommandNode*)malloc(sizeof(CommandNode));
   CreateCommandNode(headCommand, command, index, NULL);
    .....
   //Later command nodes
   nextCommand1 = (CommandNode*)malloc(sizeof(CommandNode));
   CreateCommandNode(nextCommand1, command, index, NULL);
   InsertCommandAfter(headCommand, nextCommand1);
**/

//create a new command node. usually nextCmd can be NULL and function InsertCommandAfter can be called to insert after head node.
void CreateLineNode(LineNode* thisNode, char newLine[100], LineNode* nextNode) {
   //this is useful if you store a string (char *): strcpy(thisNode->command, cmd);
   for (int i = 0; i < 100; i++)
         thisNode->line[i] = newLine[i];
   thisNode->next = nextNode;
   return;
}

//insert node newNode after thisNode 
void InsertLineAfter(LineNode* thisNode, LineNode* newNode) {
   LineNode* tmpNext = NULL;
   tmpNext = thisNode->next;
   thisNode->next = newNode;
   newNode->next = tmpNext;
   return;
}

void push(LineNode** thisNode, LineNode* newNode) {
    
    newNode->next = *thisNode;
    *thisNode = newNode;
    return;
}

//get next command node in linked list
LineNode* GetNextNode(LineNode* thisNode) {
   return thisNode->next;
}

//find a line based on the pid
// LineNode* FindLine(LineNode* theNode, char newLine[100]) {
//    LineNode* tmpNext = theNode;
//    while (tmpNext != NULL) {
//        if (tmpNext->PID == pid) { return tmpNext; }
//        tmpNext = tmpNext->next;
//    }
//    return NULL;
// }
