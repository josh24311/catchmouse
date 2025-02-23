/*
Author : josh24311
Rule :

1. 一副牌有120張卡, 共有12種花色(A~L), 每花色10張

2. 讓使用者輸入要有幾個玩家(4~8人)

3. 初始玩家狀態:
	a. 每個玩家有1000元籌碼
	b. 每個玩家最多拿9張手牌

3. 初始遊戲: 
	a. default 初始玩家為player 0 ,(假設共有4個玩家, 則依序為player 0~ 3)
	b. 洗牌 : 120張牌重新洗牌
	c. 發牌 : 每個玩家發9張初始手牌
	
4. 整理手牌:
	a. 每個玩家整理手上持有的牌, 若為成對花色,則視為不再需要, ex: player0初始手牌 AAABBCDEF, 則該玩家仍需要A,C,D,E,F才能湊對(稱為需湊對牌)
4. 判斷牌的流向 : 每個玩家都看得到其他玩家從牌堆抽出來的牌
	a. 初始玩家抽牌, 揭曉其花色
	 i.  所有人將其需湊對牌中對應該花色的牌丟到自方"排隊區" 排隊等待湊對
	 ii.  初始玩家判斷若該牌與其手中需湊對牌任一花色相同,則湊對成功,更新該玩家需湊對牌(即消除該花色), 並另外從需湊對牌丟一張牌
	 iii. 若該牌與初始玩家手中需湊對牌任一花色不同, 則將該牌照順序交給其他玩家判斷是否需要湊對, (循環順序 1-2-3-0-1-2...)
	 	甲. 若有人的排隊區需要該牌,則將該牌交給那個人, 湊對成功一樣更新該玩家需湊對牌(即消除該花色), 並另外從需湊對牌丟一張牌
	 	乙. 若都沒有人需要該牌, 則當前玩家轉移為下一玩家, 繼續回到a
        丙. 若有玩家洽需要那張牌即可完成湊對(胡牌), 則宣告該玩家勝利, 不再循環判斷給牌
5. 局結束條件: 有人的需湊對牌以及他的排隊區都為空, 則此局該玩家勝利
	a. 其他玩家給勝利玩家30元籌碼
	b. 重新洗牌
	c. 勝利玩家為新一輪起始玩家
6. 遊戲結束條件 : 任一玩家籌碼 <= 0元


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 遊戲相關常數
#define TOTAL_SUIT 12       // 12 種花色 (A~L)
#define CARDS_PERSUIT 10    // 每種花色有 10 張
#define TOTAL_CARDS 120     // 總共 120 張牌
#define MIN_PLAYERS 4       // 最少 4 名玩家
#define MAX_PLAYERS 8       // 最多 8 名玩家
#define INITIAL_HAND 9      // 每位玩家初始手牌數
#define INITIAL_MONEY 1000  // 玩家初始金額
#define PASS_MONEY  80      // 每回合輸家給贏家的籌碼

// 定義卡牌結構
typedef struct {
    char suit;  // 花色 (A~L)
} Card;

// 定義玩家結構
typedef struct {
    Card hand[INITIAL_HAND]; // 玩家手牌
    int money;               // 玩家金錢
    char neededSuits[TOTAL_SUIT]; // 需要湊對的花色
    int lineup;
} Player;

// 定義牌堆結構
typedef struct {
    Card cards[TOTAL_CARDS]; // 120 張牌
    int topIndex;            // 當前發牌位置
} Deck;

// 定義遊戲結構
typedef struct {
    Player players[MAX_PLAYERS]; // 最多 8 名玩家
    Deck deck;                   // 牌堆
    int currentPlayer;            // 當前玩家索引
    int numPlayers;               // 遊戲玩家數
    Card floatCard;
} Game;

int isFirstTurn = 1;
int total_turn = 0;

// 初始化牌堆 (創建 120 張牌)
void initializeDeck(Deck *deck) {
    char suits[TOTAL_SUIT] = "ABCDEFGHIJKL"; // 12 種花色
    int index = 0;

    for (int i = 0; i < TOTAL_SUIT; i++) {
        for (int j = 0; j < CARDS_PERSUIT; j++) {
            deck->cards[index].suit = suits[i];
            index++;
        }
    }
}

// 洗牌
void shuffleDeck(Deck *deck) {
    for (int i = TOTAL_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
    deck->topIndex = 0;
}

// 發牌
void dealCards(Game *game) {
    for (int i = 0; i < game->numPlayers; i++) {
        memset(game->players[i].neededSuits, 0, sizeof(game->players[i].neededSuits));
        game->players[i].lineup = 0;
        for (int j = 0; j < INITIAL_HAND; j++) {
            if (game->deck.topIndex + INITIAL_HAND <= TOTAL_CARDS) {
                game->players[i].hand[j] = game->deck.cards[game->deck.topIndex++];
            }
        }
    }
}

// 初始化遊戲
void initializeGame(Game *game, int numPlayers) {
    if (numPlayers < MIN_PLAYERS || numPlayers > MAX_PLAYERS) {
        printf("玩家數必須在 %d 到 %d 之間！\n", MIN_PLAYERS, MAX_PLAYERS);
        exit(1);
    }

    game->numPlayers = numPlayers;
    game->currentPlayer = 0; // 預設 0 號玩家開始

    // 初始化玩家
    for (int i = 0; i < numPlayers; i++) {
        game->players[i].money = INITIAL_MONEY;
    }
    // 初始化牌堆
    initializeDeck(&game->deck);
}

int checkGameStatus(Game *game) {
    int i;
    for(i = 0; i < game->numPlayers; i++) {
        if (game->players[i].money <= 0){
            printf("[%d] Player \033[1;32m%d\033[0m  已破產!\n",__LINE__,i);
            return i+1;
        }
        if (game->players[i].money >= (game->numPlayers)*1000) {
            printf("[%d] Player \033[1;32m%d\033[0m  贏光所有人的錢!\n",__LINE__,i);
            return i+1;
        }
    }
    return 0;
}

// 所有玩家整理手牌，記錄需要湊對的花色
void organizeHand(Game *game) {
    for(int j = 0; j < game->numPlayers; j++) {
        int suitCount[TOTAL_SUIT] = {0}; // 記錄每種花色的數量
        for (int i = 0; i < INITIAL_HAND; i++) {
            suitCount[game->players[j].hand[i].suit - 'A']++;
        }
        int neededIndex = 0;
        for (int i = 0; i < TOTAL_SUIT; i++) {
            if (suitCount[i] % 2 == 1) { // 若該花色出現奇數次，表示仍需湊對
                game->players[j].neededSuits[neededIndex++] = 'A' + i;
            }
        }
        game->players[j].neededSuits[neededIndex] = '\0'; // 以 '\0' 結尾
    }
}

// 抽牌
Card drawCard(Game *game) {
    return game->deck.cards[game->deck.topIndex++];
}

int checkTurnStatus(Game *game) {
    for(int i = 0; i < game->numPlayers; i++) {
        int currentIndex = (i+game->currentPlayer)%game->numPlayers;
        int len = strlen(game->players[currentIndex].neededSuits);
        if (game->players[currentIndex].neededSuits[0] == '\0') {
            printf("[%d] currentPlayer %d HAS NO neededSuits!\n",__LINE__, game->currentPlayer);
            return currentIndex + 1;
        }
        if (len == 1 && game->players[currentIndex].neededSuits[0] == game->floatCard.suit) {
            return currentIndex + 1;
        }
    }
    return 0;
}

Card chooseCard(Game *game) {
    int len = strlen(game->players[game->currentPlayer].neededSuits);
    // 確保 neededSuits 非空
    if (len == 0) {
        return game->players[game->currentPlayer].hand[rand() % INITIAL_HAND];
    }
    // 隨機選擇 neededSuits 內的一個花色
    int choose_index;
    choose_index = (game->players[game->currentPlayer].lineup > 0)? 0:(rand() % len);
    char target_suit = game->players[game->currentPlayer].neededSuits[choose_index];

    // 在手牌中尋找該花色的牌
    for (int i = 0; i < INITIAL_HAND; i++) {
        if (game->players[game->currentPlayer].hand[i].suit == target_suit) {
            return game->players[game->currentPlayer].hand[i];
        }
    }
    // 照理來說不會執行到這裡，因為 neededSuits 是從 hand 建構出來的
    return game->players[game->currentPlayer].hand[0];  
}
void updateNeedSuit(Game *game, char ssuit) {
    int needSuitNum, i, j, k, gotNeed = 0;
    int currentIndex = game->currentPlayer;
    needSuitNum = strlen(game->players[currentIndex].neededSuits);
    if (needSuitNum == 0) {
        printf("[%d] 當前玩家 \033[1;32m%d\033[0m 目標表格為空!\n",__LINE__, game->currentPlayer);
        return;
    }
    for (i = 0; i < needSuitNum; i++) {
        if (ssuit == game->players[currentIndex].neededSuits[i]) {
            gotNeed = 1;
            printf("[%d] 玩家 \033[1;32m%d\033[0m 需要 \033[1;31m%c \033[0m !\n",__LINE__,currentIndex, ssuit);
            if (needSuitNum == 1) {
                game->players[currentIndex].neededSuits[0] = '\0'; // 直接清空
                printf("[%d] 玩家 \033[1;32m%d\033[0m 獲得最後一張湊對牌!\n",__LINE__,currentIndex);
                return; 
            } else {
                printf("[%d] 玩家 \033[1;32m%d\033[0m 移除手牌 \033[1;31m%c \033[0m\n",__LINE__,currentIndex, ssuit);
                for (j = i; j < needSuitNum - 1; j++) {
                    game->players[currentIndex].neededSuits[j] = game->players[currentIndex].neededSuits[j + 1];
                }
                game->players[currentIndex].neededSuits[j] = '\0'; // 設置結尾
                // 检查是否需要减少 lineup

                // 如果该花色在 lineup 内，则减少 lineup
                if (i < game->players[currentIndex].lineup) {
                    printf("[%d] 此牌已經在排隊區, 縮短排隊區總數\n", __LINE__);
                    game->players[currentIndex].lineup -= 1;
                    printf("[%d] 排隊區總數 %d\n", __LINE__, game->players[currentIndex].lineup);
                    printf("[%d] 玩家 \033[1;32m%d\033[0m 移除  \033[1;31m%c \033[0m 且更新排隊區總數\n", __LINE__, currentIndex, ssuit);
                }
                printf("[%d] 玩家 \033[1;32m%d\033[0m 需湊對手牌:  ",__LINE__, currentIndex, ssuit);
                for (j = 0; j < needSuitNum - 1; j++) {
                    if (j < game->players[currentIndex].lineup) {  
                        printf("\033[1;33m%c \033[0m", game->players[currentIndex].neededSuits[j]);  // 黃色
                    } else {
                        printf("%c ", game->players[currentIndex].neededSuits[j]);
                    }
                    
                }
                printf("\n");
            }
            break;
        }
    }
}
int strategy(Game *game, Card seenCard) {
    int i,j,k,needSuitNum, gotNeed = 0;
    printf("[%d] 最優先玩家: \033[1;32m%d\033[0m, 流動牌 \033[1;31m%c \033[0m\n",__LINE__,game->currentPlayer, seenCard.suit);
    if (isFirstTurn) {
        //updateAndThrow logic
        needSuitNum = strlen(game->players[game->currentPlayer].neededSuits);
        if (needSuitNum == 0) {
            printf("[%d] 當前玩家 \033[1;32m%d\033[0m 目標表格為空!\n",__LINE__, game->currentPlayer);
            return checkTurnStatus(game);
        }
        for (i = 0; i < needSuitNum; i++) {
            if (seenCard.suit == game->players[game->currentPlayer].neededSuits[i]) {
                gotNeed = 1;
                printf("[%d] 當前玩家 \033[1;32m%d\033[0m 需要 %c \n",__LINE__,game->currentPlayer, seenCard.suit);
                // 若只剩一個花色，不需要移動，直接清空
                if (needSuitNum == 1) {
                    game->players[game->currentPlayer].neededSuits[0] = '\0'; // 直接清空
                    printf("[%d] 當前玩家 \033[1;32m%d\033[0m 獲得最後一張湊對牌!\n",__LINE__,game->currentPlayer);
                    return game->currentPlayer + 1;
                } else {
                    printf("[%d] 玩家 \033[1;32m%d\033[0m 移除手牌 %c\n",__LINE__,game->currentPlayer, seenCard.suit);
                    // 若有多個花色，移動後面的元素
                    for (j = i; j < needSuitNum - 1; j++) {
                        game->players[game->currentPlayer].neededSuits[j] = game->players[game->currentPlayer].neededSuits[j + 1];
                    }
                    game->players[game->currentPlayer].neededSuits[j] = '\0'; // 設置結尾
                    printf("[%d] 玩家 \033[1;32m%d\033[0m 更新目標表格\n",__LINE__,game->currentPlayer, seenCard.suit);
                    for (j = 0; j < needSuitNum - 1; j++) {
                        if (j < game->players[game->currentPlayer].lineup) {
                            printf("\033[1;33m%c \033[0m", game->players[game->currentPlayer].neededSuits[j]);  // 黃色
                        } else {
                            printf("%c ", game->players[game->currentPlayer].neededSuits[j]);
                        }
                    }
                    printf("\n");
                }
                break; // 一旦處理完就結束循環
            }
        }
        //throwCard
        game->floatCard = gotNeed ? chooseCard(game) : seenCard;
        printf("[%d] 當前玩家 \033[1;32m%d\033[0m 丟出 \033[1;31m%c \033[0m 成流動牌 !\n",__LINE__,game->currentPlayer, game->floatCard.suit);
        updateNeedSuit(game, game->floatCard.suit); // 丟牌之後再更新一次 NeedSuit
        return checkTurnStatus(game);
    } else {
        /*Not 1st run logic*/
        gotNeed = 0;
        for(i = 0; i < game->numPlayers; i++) {
            needSuitNum = strlen(game->players[i].neededSuits);
            for (j = 0; j < needSuitNum; j++) {
                if (seenCard.suit == game->players[i].neededSuits[j]) {
                    gotNeed = 1;
                    char tmp = game->players[i].neededSuits[j];
                    game->players[i].neededSuits[j] = game->players[i].neededSuits[game->players[i].lineup];
                    game->players[i].neededSuits[game->players[i].lineup] = tmp;
                    game->players[i].lineup += 1;
                    break;
                }
            }
        }
        for(i = 0; i < game->numPlayers; i++) { //\033[1;33m%c \033[0m
            needSuitNum = strlen(game->players[i].neededSuits);
            printf("[%d] 玩家 \033[1;32m%d\033[0m 需湊對手牌: ",__LINE__,i);
            for(j = 0; j < needSuitNum; j++) {
                if (j < game->players[i].lineup) {
                    printf(" \033[1;33m%c\033[0m",game->players[i].neededSuits[j]);
                } else {
                    printf(" %c",game->players[i].neededSuits[j]);
                }
            }
            printf("\n");
        }
        // 如果至少有一位玩家配對成功
        if (gotNeed) {
            printf("[%d] 至少一位玩家需要 \033[1;31m%c \033[0m\n",__LINE__,seenCard.suit);
            // 循環中找出第一個配對成功的玩家，並更新當前玩家
            for(i = 0; i < game->numPlayers; i++) {
                int currentIndex = (game->currentPlayer + i) % game->numPlayers;
                needSuitNum = strlen(game->players[currentIndex].neededSuits);
                for (j = 0; j < needSuitNum; j++) {
                    if (game->players[currentIndex].neededSuits[j] == seenCard.suit) {
                        printf("[%d] 玩家 \033[1;32m%d\033[0m 需要 \033[1;31m%c \033[0m\n",__LINE__,currentIndex, seenCard.suit);
                        // 配對成功，更新 currentPlayer 並選擇卡片
                        printf("[%d] 轉換最優先玩家 %d\n",__LINE__,currentIndex);
                        game->currentPlayer = currentIndex;
                        if (needSuitNum == 1) {
                            printf("[%d] 當前玩家 \033[1;32m%d\033[0m 獲得最後一張湊對牌!\n",__LINE__,game->currentPlayer);
                            game->players[currentIndex].neededSuits[0] = '\0'; // 直接清空
                            return currentIndex + 1;
                        } else {
                            printf("[%d] 玩家 \033[1;32m%d\033[0m 移除 \033[1;31m%c \033[0m\n",__LINE__,currentIndex, seenCard.suit);
                            // 若有多個花色，移動後面的元素
                            for (k = j; k < needSuitNum - 1; k++) {
                                game->players[currentIndex].neededSuits[k] = game->players[currentIndex].neededSuits[k + 1];
                            }
                            game->players[currentIndex].neededSuits[k] = '\0'; // 設置結尾
                            if (j < game->players[currentIndex].lineup) {
                                printf("[%d] 此牌已經在排隊區, 縮短排隊區總數\n", __LINE__);
                                game->players[currentIndex].lineup-=1;
                                printf("[%d] 排隊區總數 %d\n", __LINE__, game->players[currentIndex].lineup);
                            }
                            
                            printf("[%d] 玩家 \033[1;32m%d\033[0m 需湊對手牌:  ",__LINE__,currentIndex, seenCard.suit);
                            for (k = 0; k < needSuitNum - 1; k++) {
                                if (k < game->players[currentIndex].lineup) {
                                    printf("\033[1;33m%c \033[0m", game->players[currentIndex].neededSuits[k]);  // 黃色
                                } else {
                                    printf("%c ", game->players[currentIndex].neededSuits[k]);
                                }
                            }
                            printf("\n");
                        }
                        game->floatCard = chooseCard(game);
                        printf("[%d] 玩家 \033[1;32m%d\033[0m 丟出 \033[1;31m%c \033[0m 成流動牌 !\n",__LINE__,game->currentPlayer, game->floatCard.suit);
                        updateNeedSuit(game, game->floatCard.suit); // 丟牌之後再更新一次 NeedSuit
                        game->currentPlayer = (game->currentPlayer + 1)%(game->numPlayers);
                        printf("[%d] 轉換最優先玩家 %d\n", __LINE__, game->currentPlayer);
                        return checkTurnStatus(game);  // 結束回合，檢查回合狀態
                    }
                }
            }
        } else {
            // 如果沒有配對成功，則抽一張新卡並交給下一位玩家
            printf("[%d] 沒有人需要 \033[1;31m%c \033[0m !!\n", __LINE__, seenCard.suit);
            game->floatCard = drawCard(game);
            printf("[%d] 玩家 \033[1;32m%d\033[0m 抽了 \033[1;36m%c \033[0m 成流動牌 , 當前牌堆index: %d\n", __LINE__, game->currentPlayer,game->floatCard.suit, game->deck.topIndex);
            game->currentPlayer = (game->currentPlayer + 1) % game->numPlayers;
            printf("[%d] 轉換最優先玩家 \033[1;32m%d\033[0m\n", __LINE__, game->currentPlayer);
        }
    }
    return checkTurnStatus(game);
}

void playTurn(Game *game) {
    int currentPlayer = game->currentPlayer;
    int turnover = 0;
    Card drawnCard;

    if (isFirstTurn) {
        drawnCard = drawCard(game);
        printf("首輪, 玩家 \033[1;32m%d\033[0m 抽到 \033[1;36m%c \033[0m 牌, 當前牌堆index: %d\n", currentPlayer, drawnCard.suit, game->deck.topIndex);
        turnover = strategy(game, drawnCard);
        printf("[%d] 遊戲是否結束 ? %s\n", __LINE__, (turnover>0)? "Yes":"No");
        game->currentPlayer = (game->currentPlayer + 1)%(game->numPlayers);
        printf("[%d] 轉換最優先玩家 \033[1;32m%d\033[0m\n", __LINE__, game->currentPlayer);
        isFirstTurn = 0;
    }
    while(!turnover) {
        printf("[%d] 新的一輪\n", __LINE__);
        turnover = strategy(game, game->floatCard);
    }
    printf("[%d] 玩家 %d 贏得此輪 !\n", __LINE__, turnover-1);
    game->currentPlayer =  turnover-1;
    //game->players[currentPlayer].money = 0; //test

}

void givemoney(Game *game) {
    // 输出表格的头部
    printf("| 玩家 |    籌碼更新    |   獎金/賠款  |  結算後籌碼 |\n");
    printf("|------|-----------------|--------------|-------------|\n");
    
    // 遍历所有玩家并更新金額
    for (int i = 0; i < game->numPlayers; i++) {
        int initialMoney = game->players[i].money;  // 儲存玩家原始金額
        
        // 計算金額變化
        if (i == game->currentPlayer) {
            int reward = (game->numPlayers - 1) * PASS_MONEY;
            game->players[i].money += reward;
            printf("| %4d | 獲勝 +%-13d | +%-11d | %6d |\n", i, reward, reward, game->players[i].money);
        } else {
            game->players[i].money -= PASS_MONEY;
            printf("| %4d | 輸了 %-14d | -%-11d | %6d |\n", i, PASS_MONEY, PASS_MONEY, game->players[i].money);
        }
    }

    // 输出表格的分隔线
    printf("|------|-----------------|--------------|-------------|\n");
}

// 顯示遊戲狀態，顯示所有玩家的手牌和需要湊對的花色
void displayGameState(Game *game) {
    for (int i = 0; i < game->numPlayers; i++) {
        printf("玩家 %d 的手牌: ", i);
        // 顯示玩家的手牌
        for (int j = 0; j < INITIAL_HAND; j++) {
            printf("%c ", game->players[i].hand[j].suit);
        }
        printf("\n");
        //printf("玩家 %d 正在排隊花色張數: %d\n", i, game->players[i].lineup);
        printf("玩家 %d 需要湊對的花色: ", i);
        // 顯示玩家需要湊對的花色
        if (game->players[i].neededSuits[0] != '\0') {
            for (int k = 0; game->players[i].neededSuits[k] != '\0'; k++) {
                printf("\033[1;35m%c\033[0m ", game->players[i].neededSuits[k]);
            }
        } else {
            printf("無需湊對");
        }
        printf("\n");
    }
}
void displayMoney(Game *game) {
    printf("\n===== 玩家籌碼狀態 =====\n");
    for (int i = 0; i < game->numPlayers; i++) {
        printf("玩家 %d: \033[1;32m%d\033[0m 元\n", i, game->players[i].money); // 亮綠色顯示籌碼
    }
    printf("========================\n\n");
}
void playGame(Game *game) {
    while (!checkGameStatus(game)) {
        shuffleDeck(&game->deck);
        dealCards(game);
        organizeHand(game);
        displayGameState(game);
        isFirstTurn = 1;
        playTurn(game);
        total_turn++;
        for (int i = 0; i < game->numPlayers; i++) {
            printf("Player %d initial money: %d\n", i, game->players[i].money);
        }
        givemoney(game);
        displayMoney(game);
    }
}

int main() {
    Game game;
    int numPlayers;
    srand(time(NULL));
    printf("\n\n ===== 遊戲開始 ===== \n\n");
    printf("請輸入玩家數 (%d-%d): ", MIN_PLAYERS, MAX_PLAYERS);
    fflush(stdout);
    scanf("%d", &numPlayers);

    initializeGame(&game, numPlayers);
    for (int i = 0; i < game.numPlayers; i++) {
        printf("Player %d 初始籌碼: %d\n", i, game.players[i].money);
    }
    playGame(&game);

    printf("遊戲結束！ 共玩了 %d 輪\n",total_turn);
    return 0;
}
