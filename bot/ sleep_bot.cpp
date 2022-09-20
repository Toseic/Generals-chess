/*  Generals-Chess cpp示例bot程序
    author: Tosey
*/
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>
#include <ctime> 
#include <unistd.h>
#include "jsoncpp/json.h"
#define ind Json::Value::ArrayIndex
// typedef ind int;

// #include "../jsoncpp/json.h"

#define foi(n) for (int i = 0; i < n; ++i)
#define foj(n) for (int j = 0; j < n; ++j)
#define fok(n) for (int k = 0; k < n; ++k)
#define fol(n) for (int l = 0; l < n; ++l)
#define fouser for (int user = 0; user < 2; ++user)

using namespace std;
const int SquareHeight = 16;
const int SquareWidth = 16;
int InitGeneralsNum = 4;
int moves[1100][3];
int movei[] = {-1, 1, 0, 0, 0};
int movej[] = {0, 0, -1, 1, 0};
int square9i[9] = {
    -1, -1, -1,
    0, 0, 0,
    1, 1, 1};
int square9j[9] = {
    -1,
    0,
    1,
    -1,
    0,
    1,
    -1,
    0,
    1,

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
    int generals[2];                       // generals所在坐标
    int map[2][SquareHeight][SquareWidth]; // map

    bool finish;
    int currenttime;
    Game(int height_ = SquareHeight, int width_ = SquareWidth)
    {
        height = height_;
        width = width_;
        currenttime = 0;
        memset(map, 0, sizeof(map));
    }
    void initMap();
    inline int sum(int[][SquareWidth]);
    void fight(int &, int &);
    void fight(int &, int &, int &);
    void scheduleAdd();
    int move(int[3], int[3], bool);
    int move(Json::Value, Json::Value, bool);
};

int main()
{
    // freopen("debug.in", "r", stdin);
    // freopen("debug.json", "w", stdout);
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input,info, history, output;
    reader.parse(str, input);
    Game game;
    int inputsize = input["requests"].size();
    info = input["requests"][int(inputsize-1)];
    game.height = info["size"][ind(0)].asInt();
    game.width = info["size"][ind(1)].asInt();
    game.currenttime = info["time"].asInt();
    fok(2) foi(SquareHeight) foj(SquareWidth)
    {
        game.map[k][i][j] = info["map"][ind(k)][ind(i)][ind(j)].asInt();
        if (game.map[1][i][j] == 7)
        {
            game.generals[0] = i;
            game.generals[1] = j;
        }
    }

    // history 按照时间顺序从前往后排

    const int historyNum = input["responses"].size();
    
    int historyMove[historyNum][3];
    int historyTimestamp[historyNum];
    foi(historyNum)
    {
        history = input["responses"][ind(i)];
        foj(3)
            historyMove[i][j] = history[j].asInt();
        // historyTimestamp[i] = history["time"].asInt();
    }
    int movenum = 0;
    int lastmove[3];
    int lastmoveend[2];
    int antilastmove[3];
    int antistep[4] = {1,0,3,2};
    if(historyNum>0) {
        foi(3) {
            lastmove[i] = input["response"][historyNum-1][i].asInt();
        }
        lastmoveend[0] = lastmove[0]+movei[lastmove[2]];
        lastmoveend[1] = lastmove[1]+movej[lastmove[2]];
        antilastmove[0] = lastmoveend[0];
        antilastmove[1] = lastmoveend[1];
        antilastmove[2] = antistep[lastmove[2]];        
    }

    foi(game.height) foj(game.width)
    {
        fok(4)
        {
            int x = i + movei[k];
            int y = j + movej[k];
            if (x >= 0 && x < game.height && y >= 0 && y < game.width &&
                (historyNum == 0 || (i != antilastmove[0] || j != antilastmove[1] || k != antilastmove[2])) &&
                (game.map[0][i][j] > 1 ) &&
                (game.map[1][i][j] == 3 || game.map[1][i][j] == 5 || game.map[1][i][j] == 7) &&
                (game.map[1][x][y] != 1 ))
            {
                // debug >>>>>>>>
                int repeat = 1;
                int type_ = game.map[1][x][y];
                if (type_ == 0 ) repeat = 2;
                if (type_ == 2 ) repeat = 5;
                if (type_>=4 && type_%2==0) repeat = 6;

                fol(repeat) {
                    moves[movenum][0] = i, moves[movenum][1] = j, moves[movenum][2] = k;
                    movenum++;                    
                }

                // debug >>>>>>>>
                // moves[movenum][0] = i, moves[movenum][1] = j, moves[movenum][2] = k;
                // movenum++;    

            }
        }
    }
    if (movenum == 0) {
        cout << "[-1,-1,-1]" << endl;
        return 0;
    }
    srand((unsigned)time(0));
    int index = rand() % movenum;
    Json::Value ret,ret1;
    // ret["output"].append(moves[index][0]);
    // ret["output"].append(moves[index][1]);
    // ret["output"].append(moves[index][2]);
    ret[ind(0)] = moves[index][0];
    ret[ind(1)] = moves[index][1];
    ret[ind(2)] = moves[index][2];
    Json::FastWriter writer;
    ret1["response"] = ret;
    usleep(800000);
    cout << writer.write(ret1) << endl;
    return 0;
}