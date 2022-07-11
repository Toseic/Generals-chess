/*  Generals-Chess cpp示例bot程序
    author: Tosey
*/
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>
#include "jsoncpp/json.h"

#define foi(n) for (int i = 0; i < n; ++i)
#define foj(n) for (int j = 0; j < n; ++j)
#define fok(n) for (int k = 0; k < n; ++k)
#define fouser for (int user = 0; user < 2; ++user)

using namespace std;
const int SquareHeight = 16;
const int SquareWidth = 16;
int InitGeneralsNum = 4;
int movei[] = {-1, 1, 0, 0, 0};
int movej[] = {0, 0, -1, 1, 0};
int square9i[9] = {
    -1,-1,-1,
    0 ,0 ,0,
    1 ,1 ,1
};
int square9j[9] = {
    -1,0,1,
    -1,0,1,
    -1,0,1,

};
/*
color:
user1 red
user2 blue
map:
map[0]:
    表示方格中的值
map[1]: 表示方格的类型
    -1: 被迷雾遮挡不显示
    0: 无人占领的土地
    1: 山地
    2: 无人占领的城市
    3: 己方占领的土地
    4: 敌方占领的土地
    5: 己方占领的城市
    6: 敌方占领的城市
    7: 己方的王城
    8: 敌方的王城
*/

class Game
{
public:
    int height, width;
    int generals[2]; // generals所在坐标
    int map[2][SquareHeight][SquareWidth]; // map

    bool finish;
    int currenttime;
    Game(int height_ = SquareHeight, int width_ = SquareWidth)
    {
        height = height_;
        width = width_;
        currenttime = 0;
        memset(map,0,sizeof(map));
    }
    void initMap();
    inline int sum(int[][SquareWidth]);
    void fight(int &, int &);
    void fight(int &, int &, int &);
    void scheduleAdd();
    int move(int[3], int[3], bool);
    int move(Json::Value, Json::Value, bool);
};

int main() {
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,history, output;
	reader.parse(str, input);
    Game game;

    game.currenttime = input["time"].asInt();
    fok(2) foi(SquareHeight) foj(SquareWidth) {
        game.map[k][i][j] = input["map"][k][i][j].asInt();
    }
    foi(2) {
        game.generals[i] = input["generals"][i].asInt();
    } 
    // history 按照时间顺序从前往后排
    
    const int historyNum = input["history"]["num"].asInt();
    int historyMap[historyNum][SquareHeight][SquareWidth];
    int historyMove[historyNum][3];
    int historyTimestamp[historyNum];
    foi(historyNum) {
        history = input["history"]["list"][i];
        foj(3) 
            historyMove[i][j] = history["move"][j].asInt();
        foj(SquareHeight) fok(SquareWidth) {
            historyMap[i][j][k] = history["map"][j][k].asInt();
        }
        historyTimestamp[i] = history["time"].asInt();
    }
    
    

}
