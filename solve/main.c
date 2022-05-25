#include<stdio.h>		// for strcpy_s, strcat_s
#include<string.h>		// for printf
#include<stdlib.h>		// for _countof
#include<math.h>
#include<time.h>

#define SUIT_LEN	8
#define ACTION_LEN	16
#define DECK_SIZE	108
#define SWAP_CNT	1000

#define CLOCKWISE		1
#define ANTICLOCKWISE	-1

typedef struct card_s {
	char	color[SUIT_LEN];
	int		value;
	char	action[ACTION_LEN];
	struct card_s* pt;

	struct card_s* prev;

	char	b_flower;
	char	b_act;
} card;

#define FLOWER_CNT	4
#define ACT_CNT		5

#define SKIP_ACTION			0
#define REVERSE_ACTION		1
#define DRW2_ACTION			2
#define WILD_ACTION			3
#define WILDDRAW4_ACTION	4

const char* acts[]		= { "Skip", "Reverse",	"Draw2",	"Wild",	"WildDraw4"};
const char* flowers[]	= { "Red",	"Yellow",	"Green",	"Blue" };

char	play_order		= CLOCKWISE;

card*	card_GetNext(card* thisNode);
void	card_Create(card* thisNode, int value, char* color, char* action, card* nextLoc);
void	card_InsertAfter(card* thisNode, card* newNode);
int		cardsinhand(card* headnode);
void	card_PrintNodeData(card* thisNode);
int		rand_gen(int count);
void	swap(card* ptr, int i, int j);
void	CreateDeckFromFile(card** head, card** tail, FILE* inFile);
void	shuffle(card* head);
void	draw(card** deckhead, card** playerhead);
bool	checkGameOver(card** players, int playernum);

char getActCode(char* _color) {
	int _len = strlen(_color);
	for (size_t i = 0; i < ACT_CNT; i++)
	{
		if (strlen(acts[i]) != _len) continue;
		if (memcmp(_color, acts[i], _len) == 0) return i;
	}
	return -1;
}

const char* getStrFromActCode(char _code) {
	if (_code >= 0 && _code < ACT_CNT)
		return acts[_code];
	return NULL;
}

char getFlowerCode(char* _color) {
	int _len = strlen(_color);
	for (size_t i = 0; i < FLOWER_CNT; i++)
	{
		if (strlen(flowers[i]) != _len) continue;
		if (memcmp(_color, flowers[i], _len) == 0) return i;
	}
	return -1;
}

const char* getStrFromFlowerCode(char _code) {
	if (_code >= 0 && _code < FLOWER_CNT)
		return flowers[_code];
	return NULL;
}

card* getNth(card* _head, int _n) {
	card* temp = _head;
	for (int o = 0; o < _n; o++) {
		temp = card_GetNext(temp);
		if (temp == NULL) return NULL;
	}
	return temp;
}

void card_Create(card* thisNode, int value, char* color, char* action, card* nextLoc) {
	memset(thisNode, 0, sizeof(card));
	thisNode->value = value;
	strcpy_s(thisNode->color, _countof(thisNode->color), color);
	strcpy_s(thisNode->action, _countof(thisNode->action), action);
	thisNode->b_flower = getFlowerCode(color);
	thisNode->b_act = getActCode(action);
	thisNode->pt = nextLoc;
}

void card_InsertAfter(card* thisNode, card* newNode) {
	card* tmpNext = NULL;

	tmpNext = thisNode->pt;
	thisNode->pt = newNode;
	newNode->pt = tmpNext;
	return;
}

card* card_GetNext(card* thisNode) {
	if (thisNode == NULL) return NULL;
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
//Red,Yellow,Green,Blue
//♠,♣,♦,♥
void card_PrintNodeData(card* thisNode) {
	if (thisNode->value != 10) {
		printf("%d", thisNode->value);
	}

	if (memcmp(thisNode->color,"*", 1) != 0) {
		printf("%s", thisNode->color);
	}
	
	if (thisNode->action[0] != 0) {
		printf(" %s,\t", thisNode->action);
	}
	else printf(",\t");
}

//generate random number between 1 and count
int rand_gen(int count) {
	double frac;
	frac = (double)rand() / ((double)RAND_MAX + 1);
	return floor(count * frac);
}

//swap two cards
void swap(card * ptr, int i, int j) {
	if (i >= DECK_SIZE || j>=DECK_SIZE || i < 0 || j < 0 || i == j) return;

	card* ni = ptr, * nj = ptr;
	int tempvalue;
	char tempsuit[SUIT_LEN];
	char tempaction[ACTION_LEN];

	for (int o = 0; o < i; ++o) {
		ni = card_GetNext(ni);
	}
	for (int o = 0; o < j; ++o) {
		nj = card_GetNext(nj);
	}

	tempvalue = ni->value;
	ni->value = nj->value;
	nj->value = tempvalue;

	strcpy_s(tempsuit, _countof(tempsuit),ni->color);
	strcpy_s(ni->color, _countof(ni->color), nj->color);
	strcpy_s(nj->color, _countof(nj->color), tempsuit);

	strcpy_s(tempaction, _countof(tempaction), ni->action);
	strcpy_s(ni->action, _countof(ni->action), nj->action);
	strcpy_s(nj->action, _countof(nj->action), tempaction);
}
//♠,♣,♦,♥
//load deck from file
void CreateDeckFromFile(card **head, card **tail, FILE *inFile) {
	card* temp;
	temp = *head;
	int value;
	char color[SUIT_LEN];
	char action[ACTION_LEN];

	card* prev = NULL;

	for (int i = 0; i < DECK_SIZE; ++i) {
		value = 10;
		memset(color,0,SUIT_LEN);
		memset(action,0,ACTION_LEN);

		fscanf_s(inFile, "%d", &value);
		if (value != 10) {
			fscanf_s(inFile, "%s ", color, _countof(color));	//
		}
		else { //if 10
			fscanf_s(inFile, "%s ", color, _countof(color));	//if color = * wild
			fscanf_s(inFile, "%s ", action, _countof(action));
		}
		card_Create(temp, value, color, action, NULL);
		card_InsertAfter(*tail, temp);
		*tail = temp;
		temp->pt = (card*)malloc(sizeof(card));
		temp = temp->pt;
	}

	(*tail)->pt = NULL;
}

//shuffle all cards
void shuffle(card * head) {
	for (int i = 0; i < SWAP_CNT; ++i) {
		swap(head, rand_gen(DECK_SIZE), rand_gen(DECK_SIZE));
	}
}

//printf all linked cards
void Deckprint(card * head) {
	card* temp = head;
	printf("\n");
	while (temp != NULL) {
		card_PrintNodeData(temp);
		temp = card_GetNext(temp);
	}
	printf("\n");
}

void Pileprint(card * head) {
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

//choose first 7 cards and give players that 7 cards
//and set global head value to 8th cards

void deal7cards(card **deckhead, card **playerhead) {
	card* temp = *deckhead;		//save first addr
	card* decktemp = *deckhead;	//
	card* playertemp = *playerhead;	

	//get 7 cards and set head to next 7th!
	for (int i = 0; i < 6; ++i) {
		*deckhead = (*deckhead)->pt;
	}

	decktemp = *deckhead;	//get 7th cards
	*deckhead = (*deckhead)->pt;//next cards- 8th
	decktemp->pt = NULL;	//set last

	//if players has no cards, set new 7 cards
	if (*playerhead == NULL) {
		*playerhead = temp;	//
	}
	else {	//if players has already some cards, add new 7 cards to tail
		while (playertemp->pt != NULL) {
			playertemp = card_GetNext(playertemp);
		}
		playertemp->pt = temp;
	}
}
//get beginning 1 card from deck of cards, insert it players tail
//set global head to 2th cards.
void draw(card **deckhead, card **playerhead) {
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

bool checkGameOver(card** players, int playernum) {
	//detect if any players win! - if anyone has no cards, he win!
	for (int i = 0; i < playernum; ++i) {
		if (players[i] == NULL) {
			printf("----------------Player %d wins!---------------", i + 1);
			return true;
		}
	}
	return false;
}

card* pop(card** _list, int _n) {
	if (_n < 0) return NULL;
	card* ret = NULL;
	if (_n == 0) {
		ret = *_list;
		*_list = (*_list)->pt;
	}
	else {
		card* prev = getNth(*_list, _n - 1);
		card* now = getNth(*_list, _n);
		card* next = getNth(*_list, _n + 1);

		prev->pt = next;
		now->pt = NULL;
		ret = now;
	}
	return ret;
}

int main(void) {
	card* head = NULL;
	card* temp = NULL;
	card* tail = NULL;
	
	FILE* inFile;
	fopen_s(&inFile, "text1.txt", "r");
	if (inFile == NULL) {
		printf_s("Can't read text file!\n");
		return 0;
	}
	head = (card*)malloc(sizeof(card));
	tail = (card*)malloc(sizeof(card));
	///////////////////////////////////////////////////

	int shuffleornot;

	printf("Let's Play a Game of UNO!\n");
	printf("Press 1 to shuffle the UNO deck or 2 to load a deck from a file: ");
	scanf_s("%d", &shuffleornot);

	CreateDeckFromFile(&head, &tail, inFile);
	if (shuffleornot == 1) 
		shuffle(head);

	Deckprint(head);
	//////////////////////////////////////////////////
	int playernum = 0;
	while (!(playernum > 1 && playernum < 11)) {
		printf("\nPlease enter the number of players (2-10): ");
		scanf_s("%d", &playernum);
	}
	
	card** players = (card **)calloc(playernum, sizeof(card*));

	for (int i = 0; i < playernum; ++i) {
		deal7cards(&head, &players[i]);
		printf("---- Player %d has below cards ----", i+1);
		Deckprint(players[i]); // test
	}
	/////////////////////////////////////////////////
	//card* pile = (card*)calloc(0, sizeof(card)); // pile is the junk card list
	//?? why set 0 ??
	card **pile = (card**)calloc(1, sizeof(card*)); // pile is the junk card list

	int p = 0;
	int thecard = -1;

	card* prev_card = NULL;
	int cnt = 0;
	
	while (true) {
		printf("Cards on Player %d's hand:", p + 1);
		card *player = players[p];
		Deckprint(player);
		
		int card_cnt = cardsinhand(player);
		printf("Press (1 - %d) to play any card from your hand, or press zero to draw a card: ", card_cnt);
		scanf_s("%d", &thecard);
		if (thecard < 0 && thecard > card_cnt) {
			printf("Please input number between (1 - %d) : \n",card_cnt);
			continue;
		}
		if (checkGameOver(players, playernum) == true) break;

		if (thecard == 0) {//draw card
			draw(&head, &player);
		}
		else { //processing selected cards!
			card* selcard = pop(&player, thecard - 1);
			players[p] = player;

			switch (selcard->b_act)
			{
				case SKIP_ACTION:
					//next player has to lose their turn
					p = (p + play_order) % playernum;
					break;
				case REVERSE_ACTION:
					//reverse order
					play_order = 0 - play_order;
					break;
				case DRW2_ACTION:

					break;
				case WILD_ACTION:

					break;
				case WILDDRAW4_ACTION:

					break;
				default:
					//pop one from player push it pile
					break;
			}
			draw(&selcard, pile);		//pus poped card to pile
		}
		printf("---- Piled Cards! ----");
		Deckprint(*pile);

		printf("Cards on Player %d's hand:", p + 1);
		Deckprint(player);

		p = (p + play_order + playernum * 4) % playernum;
		cnt++;
	}
	return 0;
}
