/*  Generals-Chess cpp示例bot程序
    author: Tosey
*/
#include <iostream>
#include <cstring>
#include "jsoncpp/json.h"

#define ind Json::Value::ArrayIndex


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
    -1, 0, 1,
    -1, 0, 1,
    -1, 0, 1
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
    int map[2][SquareHeight][SquareWidth]; // map

    Game(int height_ = SquareHeight, int width_ = SquareWidth)
    {
        height = height_;
        width = width_;
        memset(map, 0, sizeof(map));
    }
};

int main()
{
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input,info, history, output;
    reader.parse(str, input);
    Game game;
    game.height = info["size"][ind(0)].asInt();
    game.width = info["size"][ind(1)].asInt();
    fok(2) foi(SquareHeight) foj(SquareWidth)
    {
        game.map[k][i][j] = info["map"][ind(k)][ind(i)][ind(j)].asInt();
    }


    const int historyNum = input["responses"].size();
    
    int movenum = 0; // 可以使用的移动方案（不包括原地不动）
    int lastmove[3];  // 上次的移动
    int lastmoveend[2];  // 上次移动的终点
    int antilastmove[3]; // 与上次移动相反的路径
    int antistep[4] = {1,0,3,2}; // 相反方向
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
                // 没有越界
                (historyNum == 0 || (i != antilastmove[0] || j != antilastmove[1] || k != antilastmove[2])) &&
                // 没有重复上次的路径（来回走）
                (game.map[0][i][j] > 1 ) &&
                // 起始兵力足够
                (game.map[1][i][j] == 3 || game.map[1][i][j] == 5 || game.map[1][i][j] == 7) &&
                // 起始位置属于自己
                (game.map[1][x][y] != 1 )
                // 不会移动到山上
            ){
                moves[movenum][0] = i, moves[movenum][1] = j, moves[movenum][2] = k;
                movenum++;                       

            }
        }
    }
    if (movenum == 0) {
        cout << "[-1,-1,-1]" << endl;
        return 0;
    }

    // >>>>> 你可以从这里开始修改选择方案的代码  <<<<<<
    srand((unsigned)time(0));
    int index = rand() % movenum;
    Json::Value ret,ret1;
    ret[ind(0)] = moves[index][0];
    ret[ind(1)] = moves[index][1];
    ret[ind(2)] = moves[index][2];
    // >>>>> 你可以从这里结束修改选择方案的代码  <<<<<<
    
    Json::FastWriter writer;
    ret1["response"] = ret;
    cout << writer.write(ret1) << endl;
    return 0;
}