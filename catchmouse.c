#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define TotalSuitNum 12
#define TotalCardsPerSuit 10
#define TotalCardsNum (TotalSuitNum*TotalCardsPerSuit)
#define MinPlayerNum 4
#define MaxPlayerNum 8

typedef struct _cards {
    int cardIndex;
    int suitIndex;
    int holdby;
    char suit;

} cards;

const char suitl[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
void initial_cards(cards* card)
{
    for(int i = 0; i < TotalCardsNum; i++) {
        card[i].cardIndex = i;
        card[i].suitIndex = i % TotalCardsPerSuit;
        card[i].suit = suitl[i / TotalCardsPerSuit];
    }
}

void shuffle(cards* deck) {
    for (int i = TotalCardsNum - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // 產生 0 ~ i 之間的隨機數
        // 交換 deck[i] 和 deck[j]
        cards temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void printCards(cards* card)
{
    for (int i = 0; i < TotalCardsNum; i++) {
        printf("Card %d: Suit %c, Index %d, suitIndex %d\n", i, card[i].suit, card[i].cardIndex, card[i].suitIndex);
    }
}
void game(cards* card, const int totalPlayerNum)
{
    int gameover = 0;
    int cur_winner = 0;
    while(!gameover) {
        shuffle(card);
        //printCards(card);
        
        gameover = 1;
    }
}
int main(){
    int totalPlayerNum = -1;
    cards card[TotalCardsNum] = {0};

    initial_cards(card);
    // 測試輸出
    /*
    for (int i = 0; i < 30; i++) {
        printf("Card %d: Suit %c, Index %d, suitIndex %d\n", i, card[i].suit, card[i].cardIndex, card[i].suitIndex);
    }*/

    while(totalPlayerNum < MinPlayerNum || totalPlayerNum > MaxPlayerNum) {
        printf("Input Total player numbers ( %d ~ %d): ", MinPlayerNum, MaxPlayerNum);
        scanf("%d", &totalPlayerNum);
    }
    printf("Total player numbers is : %d\n===== START THE GAME !!! ===== \n", totalPlayerNum);
    srand(time(NULL));
    game(card, totalPlayerNum);

}
