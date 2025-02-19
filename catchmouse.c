#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_CARDS 120
#define MIN_PLAYERS 4
#define MAX_PLAYERS 8
#define INITIAL_HAND 9  // 每位玩家起始手牌數

typedef struct {
    char suit;  // 花色 (A~L)
    int value;  // 數值 (1~10)
    int position;
} Card;

typedef struct {
    Card card[INITIAL_HAND];
    int gotpair[INITIAL_HAND];
    int money;
} Player;

void initializeDeck(Card deck[]) {
    int index = 0;
    for (char suit = 'A'; suit <= 'L'; suit++) {
        for (int value = 1; value <= 10; value++) {
            deck[index].suit = suit;
            deck[index].value = value;
            deck[index].position = -1;
            index++;
        }
    }
}

void initializePlayers(Player *players, const int numPlayers){
    int i,j;
    for(i = 0; i < numPlayers; i++) {
        players[i].money = 1000;
        for(j = 0; j < INITIAL_HAND; j++) {
            players[i].gotpair[j] = 0;
        }
    }
}

void shuffleDeck(Card *deck) {
    srand(time(NULL));
    for (int i = TOTAL_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

int dealCards(Card *deck, Player *players, const int numPlayers) {
    int i, j, pivot = TOTAL_CARDS-1;

    for(i = 0; i < numPlayers; i++) {
        for(j = 0; j < INITIAL_HAND; j++) {
            players[i].card[j] = deck[pivot--];
        }
    }
    return pivot+1;
}
void playCards(Card *deck, Player *players, const int numPlayers, int table_cards_num, int *cur_winner) {
/*
    player1 : ABACEDDGF   ->    AABCDDEFG
    player2 : AAAHIJKLL

*/
}
void playGame(Card *deck, Player *players, const int numPlayers) {
    int gameover = 0;
    int table_cards_num = 0;
    int cur_winner = 0;
    while(!gameover) {
        shuffleDeck(deck);
        table_cards_num = dealCards(deck, players, numPlayers);// 牌堆區 剩下幾張
        playCards(deck, players, numPlayers, table_cards_num, &cur_winner);
    }
}
int main() {
    int numPlayers;
    Card deck[TOTAL_CARDS];

    // 讓玩家輸入人數
    do {
        printf("請輸入參加者人數 (4~8): \n");
        fflush(stdout);
        scanf("%d", &numPlayers);
    } while (numPlayers < MIN_PLAYERS || numPlayers > MAX_PLAYERS);
    
    Player *players = (Player *)malloc(numPlayers * sizeof(Player));
    // 初始化、洗牌、發牌
    initializeDeck(deck);
    initializePlayers(players, numPlayers);
    // 進入遊戲
    playGame(deck, players, numPlayers);
    free(players);
    return 0;
}
