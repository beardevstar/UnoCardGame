#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

typedef struct card_s {
   char suit[7];
   int value;
   char action[15];
   struct card_s *pt;
}card;

void card_Create(card* thisNode, int value, char* suit, char* action, card* nextLoc) {
   thisNode->value = value;
   strcpy(thisNode->suit, suit);
   strcpy(thisNode->action, action);
   thisNode->pt = nextLoc;
   return;
}

void card_InsertAfter(card* thisNode, card* newNode) {
   card* tmpNext = NULL;

   tmpNext = thisNode->pt;
   thisNode->pt = newNode;
   newNode->pt = tmpNext;
   return;
}

card* card_GetNext(card* thisNode) {
   return thisNode->pt;
}

int cardsinhand(card* headnode) {
   card* temp = headnode;
   int count = 0;
   while (temp != NULL) {
       count = count + 1;
       temp = temp->pt;
   }
   return count;
}

void card_PrintNodeData(card* thisNode) {
   if (thisNode->value != 10) {
       printf("%d", thisNode->value);
   }
   if (thisNode->suit != "*") {
       printf("%s", thisNode->suit);
   }
   if (thisNode->action != "") {
       printf("%s\t", thisNode->action);
   }
   return;
}

int rand_gen(int count) {
   double frac;
   frac = (double)rand() / ((double)RAND_MAX + 1);
   return floor(count* frac);
}

void swap(card* ptr, int i, int j) {
   card *ni = ptr, *nj = ptr;
   int tempvalue;
   char tempsuit[7];
   char tempaction[15];

   for (int o = 0; o < i; ++o) {
       ni = card_GetNext(ni);
   }
   for (int o = 0; o < j; ++o) {
       nj = card_GetNext(nj);
   }

   tempvalue = ni->value;
   ni->value = nj->value;
   nj->value = tempvalue;

   strcpy(tempsuit, ni->suit);
   strcpy(ni->suit, nj->suit);
   strcpy(nj->suit, tempsuit);

   strcpy(tempaction, ni->action);
   strcpy(ni->action, nj->action);
   strcpy(nj->action, tempaction);
}

void CreateDeckFromFile(card **head, card **tail, FILE* inFile) {
   card* temp;
   temp = *head;
   int value;
   char suit[7];
   char action[15];

   for (int i = 0; i < 108; ++i) {
       value = 10;
       strcpy(suit, "");
       strcpy(action, "");

       fscanf(inFile, "%d", &value);
       if (value != 10) {
           fscanf(inFile, "%s ", suit);
       }
       else {
           fscanf(inFile, "%s ", suit);
           fscanf(inFile, "%s ", action);
       }
       card_Create(temp, value, suit, action, NULL);
       card_InsertAfter(*tail, temp);
       *tail = temp;
       temp->pt = (card*)malloc(sizeof(card));
       temp = temp->pt;
   }
  
   (*tail)->pt = NULL;
}

void shuffle(card* head) {
   srand((int)time(NULL));
   for (int i = 0; i < 1000; ++i) {
       swap(head, rand_gen(108), rand_gen(108));
   }
}

void Deckprint(card* head) {
   card* temp = head;
   while (temp != NULL) {
       card_PrintNodeData(temp);
       temp = card_GetNext(temp);
   }
}

void Pileprint(card* head) {
   card* temp = head;
   int num = cardsinhand(head);
   if (num <= 5) {
       while (temp != NULL) {
           card_PrintNodeData(temp);
           temp = temp->pt;
       }
   }
   else {
       for (int i = 0; i < num - 5; ++i) {
           temp = temp->pt;
       }
       while (temp != NULL) {
           card_PrintNodeData(temp);
           temp = temp->pt;
       }
   }
}

void deal7cards(card** deckhead, card** playerhead) {
   card* temp = *deckhead;
   card* decktemp = *deckhead;
   card* playertemp = *playerhead;
   for (int i = 0; i<6; ++i) {
       *deckhead = (*deckhead)->pt;
   }
   decktemp = *deckhead;
   *deckhead = (*deckhead)->pt;
   decktemp->pt = NULL;
   if (*playerhead == NULL){
       *playerhead = temp;
   }
   else {
       while (playertemp->pt != NULL) {
           playertemp = card_GetNext(playertemp);
       }
       playertemp->pt = temp;
   }
}

void draw(card** deckhead, card** playerhead) {
   card* temp = *deckhead;
   card* decktemp = *deckhead;
   card* playertemp = *playerhead;
  
   *deckhead = (*deckhead)->pt;
   decktemp = *deckhead;
   temp->pt = NULL;
   if (*playerhead == NULL) {
       *playerhead = temp;
   }
   else {
       while (playertemp->pt != NULL) {
           playertemp = card_GetNext(playertemp);
       }
       playertemp->pt = temp;
   }
}

int main(void) {
   card* head = NULL;
   card* temp = NULL;
   card* tail = NULL;
   FILE* inFile = fopen("text1.txt", "r");

   head = (card*)malloc(sizeof(card));
   tail = (card*)malloc(sizeof(card));
   ///////////////////////////////////////////////////

   int shuffleornot;

   printf("Let's Play a Game of UNO\n");
   printf("Press 1 to shuffle the UNO deck or 2 to load a deck from a file: ");
   scanf("%d", &shuffleornot);
  
   if (shuffleornot == 1) {
       CreateDeckFromFile(&head, &tail, inFile);
       shuffle(head);
       Deckprint(head);
   }
   else {
       CreateDeckFromFile(&head, &tail, inFile);
       Deckprint(head);
   }
/////////////////////////


   int playernum;

   printf("\n\nPlease enter the number of players (2-10): ");
   scanf("%d", &playernum);
   card **player = (card**)calloc(playernum, sizeof(card*));

   for (int i = 0; i < playernum; ++i) {
       deal7cards(&head, &player[i]);
       Deckprint(player[i]); // test
       printf("\n");
   }
/////////////////////////////////////////////////


   card *pile = (card*)calloc(0, sizeof(card)); // pile is the junk card list
   draw(&head, &pile);
   draw(&head, &pile);    // test
   Deckprint(pile);

   int p = 0;
   int thecard;

   while (1 == 1) {
      printf("player %d hand: ", p + 1);
     Deckprint(player[p]);
     printf("press (1-%d) to play any card from your hand, or press zero to draw a card: ", cardsinhand(player[p]));
     scanf("%d", &thecard);
     if (p < playernum) {
         p = p + 1;
     }
     else if (p = playernum) {
         p = 0;
     }
     for (int i = 0; i < playernum; ++i) {
         if (player[i] == NULL) {
             printf("Player %d wins", i + 1);
            break;
        }
     }
   
}

   return 0;
}
