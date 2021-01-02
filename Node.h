

typedef struct line_struct {
   char line[100];
   //int PID;
   struct line_struct* next;
} LineNode;


void CreateLineNode(LineNode* thisNode, char line[100], LineNode* nextLine);
void InsertLineAfter(LineNode* thisNode, LineNode* newNode);
void push(LineNode** thisNode, LineNode* newNode);
LineNode* GetNextNode(LineNode* thisNode);
// LineNode* FindLine(LineNode* theNode, int pid);


