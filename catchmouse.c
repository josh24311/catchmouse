#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_SUIT 12
#define CARDS_PERSUIT 10
#define TOTAL_CARDS 120
#define MIN_PLAYERS 4
#define MAX_PLAYERS 8
#define INITIAL_HAND 9  // 每位玩家起始手牌數

const char SUITS[TOTAL_SUIT] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};

typedef struct {
    char suit;  // 花色 (A~L)
    int value;  // 數值 (1~10)
    int position;
} Card;

typedef struct {
    Card card[INITIAL_HAND];
    int gotpair[INITIAL_HAND];
    char lookfor[INITIAL_HAND];
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
        memset(players[i].gotpair, 0, sizeof(players[i].gotpair));
        memset(players[i].lookfor, 0, sizeof(players[i].lookfor));
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
int playerWant(char *lookfor, const char cur_card_suit) { 
    int i, j;
    for (i = 0; lookfor[i] != '\0'; i++) {
        if (lookfor[i] == cur_card_suit) {
            // 移動字串內容，刪除該字母
            for (j = i; lookfor[j] != '\0'; j++) {
                lookfor[j] = lookfor[j + 1];
            }
            return 1;
        }
    }
    return 0;
}

void playCards(Card *deck, Player *players, const int numPlayers, int table_cards_num, int *cur_winner) {
/*
    player1 : ABACEDDGF   ->    [1,0,1,0,0,1,1,0,0] -> look B C E G F
    player2 : CAAAHIKLL

*/
    int suit_exist[TOTAL_SUIT];
    int round_over = 0, i, j , cur_player, card_index;
    cur_player = *cur_winner;
    card_index = table_cards_num - 1;
    char cur_card_suit;
    while(!round_over) {
        // check pair
        for (j = 0; j < numPlayers; j++) {
            int xor_result = 0;
            for(i = 0; i < INITIAL_HAND; i++) {
                xor_result ^= (1 << (players[j].card[i].suit - 'A'));//xor_result 會剩下不成對的那些子母對應位元
            }
            int lookfor_index = 0;
            for (i = 0; i < TOTAL_SUIT; i++) {
                if (xor_result & (1 << i)) {
                    players[j].lookfor[lookfor_index++] = 'A' + i;
                }
            }
            players[j].lookfor[lookfor_index] = '\0';  // 以 '\0' 結尾，標記字串結束
        }
        // draw card
        cur_card_suit = deck[card_index--].suit;
        // check who want
        int check;
        for (check = 0; check < numPlayers; check++) {
            int player_index = (cur_player + check) % numPlayers; 
            if (playerWant(players[player_index].lookfor, cur_card_suit)) {
                printf("Player %d wants and takes card %c\n", player_index, cur_card_suit);
                break;  // 一旦有玩家拿走，這張牌就不再流通
            }
        }

    }

}
int checkPlayerStatus(Player *players, const int numPlayers) {
    int i;
    for(i = 0; i < numPlayers; i++) {
        if (players[i].money <= 0)
            return (i+1);
    }
    return 0;
}
void playGame(Card *deck, Player *players, const int numPlayers) {
    int gameover = 0;
    int table_cards_num = 0;
    int cur_winner = 0;
    while(!gameover) {
        shuffleDeck(deck);
        table_cards_num = dealCards(deck, players, numPlayers);// 牌堆區 剩下幾張
        playCards(deck, players, numPlayers, table_cards_num, &cur_winner);
        gameover = checkPlayerStatus(players, numPlayers);
    }
    printf("Player %d already bankrupt , GAMEOVER\n", gameover);
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
