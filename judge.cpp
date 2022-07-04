#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>
#include "ignore/kingz/jsoncpp/json.h"

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
int errorPerson = -1;

inline int randint(int low, int high)
{
    float num;
    num = (rand() % 1000) * 0.001;
    int target = num * (high - low + 1) + low;
    return target;
}

// color:
// user1 red
// user2 blue
// map:
// map[0]:
//     表示方格中的值
// map[1]: 表示方格的类型
//     -1: 被迷雾遮挡不显示
//     0: 无人占领的土地
//     1: 山地
//     2: 无人占领的城市
//     3: user1占领的土地
//     4: user2占领的土地
//     5: user1占领的城市
//     6: user2占领的城市
//     7: user1的王城
//     8: user2的王城
// move: list
// [i,j,w,d]
class Game
{
public:
    int height, width;
    int generals[2][2];
    int map[2][SquareHeight][SquareWidth];
    int mapVision1[2][SquareHeight][SquareWidth];
    int mapVision2[2][SquareHeight][SquareWidth];

    bool finish;
    int winner;
    int currenttime;
    Game(int height_ = SquareHeight, int width_ = SquareWidth)
    {
        height = height_;
        width = width_;
    }
    void initMap();
    bool generalsCheck();
    bool mapCheck();
    void bfs(int[][SquareWidth], int, int);
    inline int sum(int[][SquareWidth]);
    void fight(int &, int &);
    void fight(int &, int &, int &);
    void scheduleAdd();
    int move(int[4], int[4], bool);
    int move(Json::Value, Json::Value, bool);
    void mapWithFog();
    void loadMap(Json::Value);
};

bool Game::generalsCheck()
{
    float distance = sqrt((generals[0][0] - generals[1][0]) * (generals[0][0] - generals[1][0]) +
                          (generals[0][1] - generals[1][1]) * (generals[0][1] - generals[1][1]));
    if (distance < (height + width) / 4)
        return false;
    return true;
}

void Game::bfs(int visit[][SquareWidth], int i, int j)
{
    if (i < 0 || i >= height || j < 0 || j >= width)
        return;
    if (map[1][i][j] == 1 || map[1][i][j] == 2 || visit[i][j])
        return;
    visit[i][j] = 1;
    for (int i1 = 0; i1 < 4; ++i1)
        bfs(visit, i + movei[i1], j + movej[i1]);
}

inline int Game::sum(int visit[][SquareWidth])
{
    int num = 0;
    foi(height) foj(width)
    {
        if (visit[i][j])
            ++num;
    }
    return num;
}

bool Game::mapCheck()
{
    int visit1[SquareHeight][SquareWidth] = {}, visit2[SquareHeight][SquareWidth] = {};
    bfs(visit1, generals[0][0], generals[0][1]);
    bfs(visit2, generals[1][0], generals[1][1]);
    if (visit1[generals[1][0]][generals[1][1]] != 1) // 双方大本营不连通
        return false;
    if (sum(visit1) < height * width / 2 || sum(visit2) < height * width / 2)
        return false;
    return true;
}

void Game::initMap()
{
    int area = height * width;
    int mountainNum = int(area * 1 / 5);
    int cityNum = int(area * 1 / 20);
    bool legal = false;
    int *map1d = new int[area];
    while (true)
    {
        generals[0][0] = randint(0, height - 1);
        generals[0][1] = randint(0, width - 1);
        generals[1][0] = randint(0, height - 1);
        generals[1][1] = randint(0, width - 1);
        if (generalsCheck())
            break;
    }
    while (true)
    {
        srand((unsigned)time(NULL));
        memset(map1d, 0, sizeof(int) * area);
        foi(mountainNum) map1d[i] = 1;
        foi(cityNum) map1d[i + mountainNum - 1] = 2;
        random_shuffle(map1d, map1d + area);
        foi(height) foj(width)
            map[1][i][j] = map1d[i * width + j];
        map[1][generals[0][0]][generals[0][1]] = 7;
        map[1][generals[1][0]][generals[1][1]] = 8;
        if (mapCheck())
            break;
    }
    delete[] map1d;
    foi(height) foj(width)
    {
        if (map[1][i][j] == 2)
            map[0][i][j] = randint(40, 50);
        else if (map[1][i][j] == 7 || map[1][i][j] == 8)
            map[0][i][j] = InitGeneralsNum;
    }
}

void Game::fight(int &n1, int &n2)
{
    n1 = 0 ? n1 < n2 : n1 - n2;
    n2 = 0 ? n2 < n1 : n2 - n1;
}

void Game::fight(int &f1, int &n1, int &n2)
{
    int numb[] = {f1, n1, n2};
    int point = 1 ? n1 >= n2 : 2;
    int lef1 = abs(numb[point] - f1);
    int lef2 = abs(lef1 - numb[3 - point]);
    int Npoint = point ? numb[point] > f1 : 0;
    Npoint = Npoint ? lef1 > numb[3 - point] : 0;
    int ret[] = {0, 0, 0};
    ret[Npoint] = lef2;
    f1 = ret[0];
    n1 = ret[1];
    n2 = ret[2];
}

void Game::scheduleAdd()
{
    if (currenttime % 16 == 0)
    {
        foi(height) foj(width) if (map[1][i][j] >= 3)++ map[0][i][j];
    }
    else if (currenttime % 2 == 0)
    {
        foi(height) foj(width) if (map[1][i][j] >= 5)++ map[0][i][j];
    }
}

int Game::move(int move1[4], int move2[4],bool check=false)
{
    /*  return 3 OK
        return 0 user1 error
        return 1 user2 error
        return 2 both error
    */
    int to1[] = {move1[0] + movei[move1[3]], move1[1] + movej[move1[3]]};
    int to2[] = {move2[0] + movei[move2[3]], move2[1] + movej[move2[3]]};
    int *moveAll[2] = {move1, move2};
    bool legal1 = true, legal2 = true;
    if (check) {
        
        // 起始位置非法 TODO: 越界违法
        if (!((map[1][move1[0]][move1[1]] == 3) ||
            (map[1][move1[0]][move1[1]] == 5) ||
            (map[1][move1[0]][move1[1]] == 7)))
            legal1 = false;
        if (!((map[1][move2[0]][move2[1]] == 4) ||
            (map[1][move2[0]][move2[1]] == 6) ||
            (map[1][move2[0]][move2[1]] == 8)))
            legal2 = false;
        // 非法数量
        if (move1[2] <= 0)
            legal1 = false;
        if (move2[2] <= 0)
            legal2 = false;
        // 数量不够
        if (move1[2] >= map[0][move1[0]][move1[1]])
            legal1 = false;
        if (move2[2] >= map[0][move2[0]][move2[1]])
            legal2 = false;
        // 移动到山上
        if (map[1][to1[0]][to1[1]] == 1)
            legal1 = false;
        if (map[1][to2[0]][to2[1]] == 1)
            legal2 = false;
    }
    if (!legal1 && !legal2) return 2;
    if (!legal1) return 0;
    if (!legal2) return 1;

    map[0][move1[0]][move1[1]] -= move1[2];
    map[0][move2[0]][move2[1]] -= move2[2];
    if (!(
            to1[0] == to2[0] && to1[1] == to2[1] && map[1][to1[0]][to1[1]] != 0))
    {
        fouser {
            int *move = moveAll[user];
            int toi = move[0] + movei[move[3]], toj = move[1] + movej[move[3]], moveN = move[3];
            int toN, type = map[0][toi][toj], map[1][toi][toj];
            int r1 = toN, r2 = moveN;
            fight(r1, r2);
            int maxr = r1 + r2;
            bool occupy = r2 > 0;
            if (type == 0) {                                // free square
                map[1][toi][toj] = 3 + user; // 3:user1 4:user2
                map[0][toi][toj] = moveN;
            }
            else if (type == 0) { // free city
                map[0][toi][toj] = maxr;
                // 只有当进攻的兵力有富余时才算占领了，剩余0不认为是占领
                if (occupy)
                    map[1][toi][toj] = 5 + user;
            }
            else if (type == user + 3 || type == user + 5 || type == user + 7) {
                // 己方的土地或者city或者王城
                map[0][toi][toj] += moveN;
            }
            else if (type == 6 - user || type == 4 - user) {
                //  敌方的city或者土地
                map[0][toi][toj] = maxr;
                if (occupy) {
                    if (user == 0)
                        --map[1][toi][toj];
                    else
                        ++map[1][toi][toj];
                }
            }
            else if (type == 8 - user) { // 敌方的王城
                map[0][toi][toj] = maxr;
                if (occupy) {
                    map[1][toi][toj] = 7 + user;
                    finish = true;
                    winner = user;
                }
            }
        }
    }
    // 当双方同时到达一块土地或者城市，以及某一方的王城时，需被额外考虑
    // 若为无人占领的城市被进攻的情况，先让双方中人数多的先进攻，人数少的随之再进攻
    // 已被占领的城市同时被进攻和增援，需要额外考虑的原因是如果最后城市剩余士兵为0
    //     ，应当认为是未被敌方占据，仍为己方所有，如果把它视为普通情况，可能会被判断为已被占据
    // 若为王城被进攻的情况，让增援部队先加入，再让对方进攻
    else {
        int toi = move1[0], toj = move1[1];
        int moveN1, moveN2 = move1[3], move2[3];
        int moveN[] = {moveN1, moveN2};
        int toN, type = map[0][toi][toj], map[1][toi][toj];
        int r1 = toN, r2 = moveN1, r3 = moveN2;
        fight(r1, r2, r3);
        bool occupy1 = r2 > 0, occupy2 = r3 > 0;
        if (type == 2) {
            map[0][toi][toj] = r1 + r2 + r3;
            if (occupy1 || occupy2) {
                map[0][toi][toj] = 5 ? occupy1 : 6;
            }
        }
        else { // 已被某一方占据的土地
            int belong = 1 ? type % 2 == 0 : 0;
            int defenceN=moveN[belong], enemyN = moveN[1-belong];
            toN += defenceN ; // 先加上增援部队
            bool occupy = enemyN > toN;
            map[0][toi][toj] = abs(enemyN-toN);
            if (occupy)
                map[1][toi][toj] = type +1 ? belong == 0:type-1;
            if (occupy && type >= 7) { // 王城被攻下
                finish = true;
                winner = 1-belong;
            }
                
        }
    }
    return 3;
}

int Game::move(Json::Value move1_, Json::Value move2_,bool check=false) {
    int move1[4],move2[4];
    foi(4) {
        move1[i] = move1_[i].asInt();
    }
    return move(move1,move2,check);
}

void Game::mapWithFog() {
    memset(mapVision1,0,sizeof(mapVision1));
    memset(mapVision2,0,sizeof(mapVision2));
    foi(height) foj(width) {
        int type = map[1][i][j];
        if (type == 1) {
            mapVision1[0][i][j] = map[0][i][j];
            mapVision1[1][i][j] = map[1][i][j];
        } else if (type >= 3) { //被占据的地方
            if (type % 2 == 1) { // 被user1占据
                for (int i1=0;i1<5;++i1) {
                    int newi = i + movei[i1], newj = j + movej[i1];
                        mapVision1[0][i][j] = map[0][i][j];
                        mapVision1[1][i][j] = map[1][i][j];
                }
            } else {
                for (int i1=0;i1<5;++i1) {
                    int newi = i + movei[i1], newj = j + movej[i1];
                        mapVision2[0][i][j] = map[0][i][j];
                        mapVision2[1][i][j] = map[1][i][j];
                }
            }
        }
    }
}

void Game::loadMap(Json::Value value) {
    fok(2) foi(height) foj(width) 
        map[k][i][j] = value["map"][k][i][j].asInt();
    foi(2) foj(2)
        generals[i][j] = value["generals"][i][j].asInt();
}

int main() {
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,log, output;
	reader.parse(str, input);
    Game game;

    log = input["log"];
    if (log.size() == 0) {
        // display: TODO:
        game.initMap();
        game.mapWithFog();
        output["command"] = "request";
        fok(2) foi(game.height) foj(game.width) {
            output["content"]["0"]["map"][k][i][j] = game.mapVision1[k][i][j];
            output["content"]["1"]["map"][k][i][j] = game.mapVision2[k][i][j];
            
            output["initdata"][k][i][j] = game.map[k][i][j];
        }
        foi(2) foj(2) 
            output["initdata"]["generals"][i][j] = game.generals[i][j];
        foi(2) {
            output["content"]["0"]["generals"][i] = game.generals[0][i];
            output["content"]["1"]["generals"][i] = game.generals[1][i];
        }
    } else {
        game.loadMap(input["initdata"]);
        Json::Value opt1, opt2;
        int inputSize = input.size();
        int legalAns ;
		for (int i = 1; i < inputSize; i += 2) {
			opt1 = input[i]["0"]["response"];
			opt2 = input[i]["1"]["response"];
            if (i == inputSize-1)
                 legalAns = game.move(opt1,opt2,true);
            else 
                game.move(opt1,opt2);
        }
        if (legalAns == 3) {
            if (!game.finish ) {
                output["command"] = "request";
                fok(2) foi(game.height) foj(game.width) {
                    output["content"]["0"]["map"][k][i][j] = game.mapVision1[k][i][j];
                    output["content"]["1"]["map"][k][i][j] = game.mapVision2[k][i][j];
                }
                foi(2) {
                    output["content"]["0"]["generals"][i] = game.generals[0][i];
                    output["content"]["1"]["generals"][i] = game.generals[1][i];
                }
                // display TODO:
            } else {

            }
        }
        else {
            // error TODO:
        }


    }
}