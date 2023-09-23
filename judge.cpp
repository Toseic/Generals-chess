/*

Author: Tosey
Datetime: 2022-07-27
Github: https://github.com/Toseic/Generals-chess

*/

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <random>
#include "jsoncpp/json.h"
#define ind Json::Value::ArrayIndex

#define foi(n) for (int i = 0; i < n; ++i)
#define foj(n) for (int j = 0; j < n; ++j)
#define fok(n) for (int k = 0; k < n; ++k)
#define fouser for (int user = 0; user < 2; ++user)

using namespace std;

const int MaxSquareHeight = 25;
const int MaxSquareWidth = 25;
const int DefaultSquareHeight = 25;
const int DefaultMaxSquareWidth = 25;
const int TTO = 500;
const int InitGeneralsNum = 10;
const int squareScheduleAddTime = 8;
const bool randomap = true;

const int sizeLowBond = 14;
const int sizeHighBond = 18;
const int cityArmyLowBond = 40;
const int cityArmyHighBond = 50;

// 游戏结束后是否判分数
const bool NOSCORECHANGE = false;

// c++ 11
default_random_engine randeng((unsigned)time(NULL));
uniform_int_distribution<unsigned >u_size(sizeLowBond,sizeHighBond);
uniform_int_distribution<unsigned >u_cityArmy(cityArmyLowBond,cityArmyHighBond);

int movei[] = {-1, 1, 0, 0, 0};
int movej[] = {0, 0, -1, 1, 0};

int square9i[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
int square9j[9] = {
    -1, 0, 1, -1, 0, 1, -1, 0, 1,

};
int errorPerson = -1;


/*color:
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
    3: user1占领的土地
    4: user2占领的土地
    5: user1占领的城市
    6: user2占领的城市
    7: user1的王城
    8: user2的王城
move: list
[i,j,d]
winner:
0: user1
1: user2
2: draw
-1: unknown */


class Game {
   public:
    int height, width;
    int generals[2][2];
    int map[2][MaxSquareHeight][MaxSquareWidth];
    int mapVision1[2][MaxSquareHeight][MaxSquareWidth];
    int mapVision2[2][MaxSquareHeight][MaxSquareWidth];
    int army[2];
    int land[2];

    bool finish;
    int winner, errorcode, currenttime;
    string errormessage[2];

    Game(int height_ = DefaultSquareHeight, int width_ = DefaultMaxSquareWidth) {
        height = height_;
        width = width_;
        winner = -1;
        currenttime = 0;
        finish = false;
        errormessage[0] = "";
        errormessage[1] = "";
        memset(map, 0, sizeof(map));
        memset(mapVision1, 0, sizeof(mapVision1));
        memset(mapVision2, 0, sizeof(mapVision2));
        // memset(mapVisionBoth,0,sizeof(mapVisionBoth));
    }
    void initMap();
    bool generalsCheck();
    bool mapCheck();
    void bfs(int[][MaxSquareWidth], int, int);
    inline int sum(int[][MaxSquareWidth]);
    void fight(int&, int&);
    void fight(int&, int&, int&);
    void scheduleAdd();
    int move(int[3], int[3], bool);
    void move(int[3], int user, int moveN);
    int move(Json::Value, Json::Value, bool);
    void mapCompute();
    void loadMap(Json::Value);
    void printFinish();
    void printError();
};

bool Game::generalsCheck() {
    float distance = sqrt(
        (generals[0][0] - generals[1][0]) * (generals[0][0] - generals[1][0]) +
        (generals[0][1] - generals[1][1]) * (generals[0][1] - generals[1][1]));
    if (distance < (height + width) / 4)
        return false;
    return true;
}

void Game::bfs(int visit[][MaxSquareWidth], int i, int j) {
    if (i < 0 || i >= height || j < 0 || j >= width)
        return;
    if (map[1][i][j] == 1 || map[1][i][j] == 2 || visit[i][j])
        return;
    visit[i][j] = 1;
    for (int i1 = 0; i1 < 4; ++i1)
        bfs(visit, i + movei[i1], j + movej[i1]);
}

inline int Game::sum(int visit[][MaxSquareWidth]) {
    int num = 0;
    foi(height) foj(width) {
        if (visit[i][j])
            ++num;
    }
    return num;
}

bool Game::mapCheck() {
    int visit1[MaxSquareHeight][MaxSquareWidth] = {},
        visit2[MaxSquareHeight][MaxSquareWidth] = {};
    bfs(visit1, generals[0][0], generals[0][1]);
    bfs(visit2, generals[1][0], generals[1][1]);
    if (visit1[generals[1][0]][generals[1][1]] != 1)  // 双方大本营不连通
        return false;
    if (sum(visit1) < height * width / 2 || sum(visit2) < height * width / 2)
        return false;
    return true;
}

void Game::initMap() {
    int area = height * width;
    int mountainNum = int(area * 1 / 5);
    int cityNum = int(area * 1 / 20);
    bool legal = false;
    int* map1d = new int[area];

    uniform_int_distribution<unsigned >u_height(0,height-1);
    uniform_int_distribution<unsigned >u_width(0,width-1);


    while (true) {
        // srand((unsigned)time(NULL));
        generals[0][0] = u_height(randeng);
        generals[0][1] = u_width(randeng);
        generals[1][0] = u_height(randeng);
        generals[1][1] = u_width(randeng);
        if (generalsCheck())
            break;
    }
    while (true) {
        // srand((unsigned)time(NULL));
        memset(map1d, 0, sizeof(int) * area);
        foi(mountainNum) map1d[i] = 1;
        foi(cityNum) map1d[i + mountainNum - 1] = 2;
        random_shuffle(map1d, map1d + area);
        foi(height) foj(width) map[1][i][j] = map1d[i * width + j];
        map[1][generals[0][0]][generals[0][1]] = 7;
        map[1][generals[1][0]][generals[1][1]] = 8;
        if (mapCheck())
            break;
    }
    delete[] map1d;
    foi(height) foj(width) {
        if (map[1][i][j] == 2)
            map[0][i][j] = u_cityArmy(randeng);
        else if (map[1][i][j] == 7 || map[1][i][j] == 8)
            map[0][i][j] = InitGeneralsNum;
    }
}

void Game::fight(int& n1, int& n2) {
    if (n1 < n2) {
        n2 -= n1;
        n1 = 0;
    } else {
        n1 -= n2;
        n2 = 0;
    }
}

// 此函数被用于处理当双方同时进攻无人占领的城市时的情况
// 双方兵力多者先战斗，少者后战斗
void Game::fight(int& f1, int& n1, int& n2) {
    int f1_ = f1, n1_ = n1, n2_ = n2;
    int numb[] = {f1_, n1_, n2_};
    int first_person = n1_ >= n2_ ? 1 : 2;
    int lef1 = abs(numb[first_person] - f1_);
    int lef2 = abs(lef1 - numb[3 - first_person]);
    int Npoint = numb[first_person] > f1_ ? first_person : 0;
    Npoint = lef1 > numb[3 - first_person] ? Npoint : 0;
    int ret[] = {0, 0, 0};
    ret[Npoint] = lef2;
    f1 = ret[0];
    n1 = ret[1];
    n2 = ret[2];
}

// 定时增加兵力
void Game::scheduleAdd() {
    if (currenttime % squareScheduleAddTime == 0) {
        foi(height) foj(width) if (map[1][i][j] >= 3)++ map[0][i][j];
    } else if (currenttime % 2 == 0) {
        foi(height) foj(width) if (map[1][i][j] >= 5)++ map[0][i][j];
    }
}

// 移动士兵，情况：
// 双方行动不相互干扰，在此前已完成合法性检查
void Game::move(int move[3], int user, int moveN) {
    int toi = move[0] + movei[move[2]], toj = move[1] + movej[move[2]];
    int toN = this->map[0][toi][toj], type = this->map[1][toi][toj];
    int r1 = toN, r2 = moveN;
    fight(r1, r2);
    int maxr = r1 + r2;
    bool occupy = r2 > 0;
    if (type == 0) {                        // free square
        this->map[1][toi][toj] = 3 + user;  // 3:user1 4:user2
        this->map[0][toi][toj] = moveN;
    } else if (type == 2) {  // free city
        this->map[0][toi][toj] = maxr;
        // 只有当进攻的兵力有富余时才算占领了，剩余0不认为是占领
        if (occupy)
            this->map[1][toi][toj] = 5 + user;
    } else if (type == user + 3 || type == user + 5 || type == user + 7) {
        // 己方的土地或者city或者王城
        this->map[0][toi][toj] += moveN;
    } else if (type == 6 - user || type == 4 - user) {
        //  敌方的city或者土地
        this->map[0][toi][toj] = maxr;
        if (occupy) {
            if (user == 0)
                --this->map[1][toi][toj];
            else
                ++this->map[1][toi][toj];
        }
    } else if (type == 8 - user) {  // 敌方的王城
        this->map[0][toi][toj] = maxr;
        if (occupy) {
            this->map[1][toi][toj] = 7 + user;
            finish = true;
            winner = user;
        }
    }
}

// 移动士兵，情况：
// 总处理函数，在双方不相互干扰时，使用另一个move分别处理各人
// 在相互干扰时，在此函数中完成处理
int Game::move(int move1[3], int move2[3], bool check = false) {
    /*  return 3 OK
        return 0 user1 error
        return 1 user2 error
        return 2 both error
    */
    string messageMap[] = {
        "非法方向",         "非法起始位置",         "起始位置土地不可被你移动",
        "移动终点超出地图", "起始位置士兵数量不足", "移动终点不能为山地",
    };

    int* moveAll[2] = {move1, move2};
    int moveN[2] = {-1, -1};
    bool legal1 = true, legal2 = true;

    bool user0Stay = move1[0] == -1 && move1[1] == -1 && move1[2] == -1;
    bool user1Stay = move2[0] == -1 && move2[1] == -1 && move2[2] == -1;
    if (check) {
        // 方向非法
        if (move1[2] < -1 || move1[2] > 3) {
            legal1 = false;
            move1[2] = 0;
            errormessage[0] += messageMap[0];
        }
        if (move2[2] < -1 || move2[2] > 3) {
            legal2 = false;
            move2[2] = 0;
            errormessage[1] += messageMap[0];
        }
        int to1[] = {move1[0] + movei[move1[2]], move1[1] + movej[move1[2]]};
        int to2[] = {move2[0] + movei[move2[2]], move2[1] + movej[move2[2]]};
        if (user0Stay) {  // 若是不移动，则绕过后面的检查
            legal1 = false;
        }
        if (user1Stay) {
            legal2 = false;
        }
        // 起始位置并非你所有
        if (legal1 && ((move1[0] < 0 || move1[0] >= height || move1[1] < 0 ||
                        move1[1] >= width))) {
            legal1 = false;
            errormessage[0] += messageMap[1];
        }

        if (legal2 && (move2[0] < 0 || move2[0] >= height || move2[1] < 0 ||
                       move2[1] >= width)) {
            legal2 = false;
            errormessage[1] += messageMap[1];
        }
        // 起始位置非法
        if (legal1 && !((map[1][move1[0]][move1[1]] == 3) ||
                        (map[1][move1[0]][move1[1]] == 5) ||
                        (map[1][move1[0]][move1[1]] == 7))) {
            legal1 = false;
            errormessage[0] += messageMap[2];
        }

        if (legal2 && !((map[1][move2[0]][move2[1]] == 4) ||
                        (map[1][move2[0]][move2[1]] == 6) ||
                        (map[1][move2[0]][move2[1]] == 8))) {
            legal2 = false;
            errormessage[1] += messageMap[2];
        }

        // 非法越界
        if (legal1 &&
            (to1[0] < 0 || to1[0] >= height || to1[1] < 0 || to1[1] >= width)) {
            legal1 = false;
            errormessage[0] += messageMap[3];
        }

        if (legal2 &&
            (to2[0] < 0 || to2[0] >= height || to2[1] < 0 || to2[1] >= width)) {
            legal2 = false;
            errormessage[1] += messageMap[3];
        }

        // 数量不够
        if (legal1 && map[0][move1[0]][move1[1]] <= 1) {
            legal1 = false;
            errormessage[0] += messageMap[4];
        }

        if (legal2 && map[0][move2[0]][move2[1]] <= 1) {
            legal2 = false;
            errormessage[1] += messageMap[4];
        }

        // 移动到山上
        if (legal1 && map[1][to1[0]][to1[1]] == 1) {
            legal1 = false;
            errormessage[0] += messageMap[5];
        }

        if (legal2 && map[1][to2[0]][to2[1]] == 1) {
            legal2 = false;
            errormessage[1] += messageMap[5];
        }
    }
    // 不移动为合法
    if (user0Stay)
        legal1 = true;
    if (user1Stay)
        legal2 = true;
    // 处理errorcode
    if (!legal1 && !legal2) {
        errorcode = 2;
        return 2;
    }
    if (!legal1) {
        errorcode = 0;
        return 0;
    }
    if (!legal2) {
        errorcode = 1;
        return 1;
    }
    //   <<<<< 在此以后运行时已被视为合法 >>>>>
    int to1[] = {move1[0] + movei[move1[2]], move1[1] + movej[move1[2]]};
    int to2[] = {move2[0] + movei[move2[2]], move2[1] + movej[move2[2]]};
    // 若选择移动，则先把兵力调出，再考虑放入的问题
    if (!user0Stay) {
        moveN[0] = map[0][move1[0]][move1[1]] - 1;
        map[0][move1[0]][move1[1]] = 1;
    }
    if (!user1Stay) {
        moveN[1] = map[0][move2[0]][move2[1]] - 1;
        map[0][move2[0]][move2[1]] = 1;
    }
    // 此判断为：若终点重合，而且双方均不选择不动，则由后面的else处理
    // 这里的type==0的情况可能有点问题 FIXME:
    if (!(to1[0] == to2[0] && to1[1] == to2[1] && map[1][to1[0]][to1[1]] != 0 &&
          (!user0Stay) && (!user1Stay))) {
        fouser {
            int* thismove = moveAll[user];
            if (((user == 0) && user0Stay) || ((user == 1) && user1Stay))
                continue;
            move(thismove, user, moveN[user]);
        }
    }
    // 当双方同时到达一块土地或者城市，以及某一方的王城时，需被额外考虑
    // 若为无人占领的城市被进攻的情况，先让双方中人数多的先进攻，人数少的随之再进攻
    // 已被占领的城市同时被进攻和增援，需要额外考虑的原因是如果最后城市剩余士兵为0
    //     ，应当认为是未被敌方占据，仍为己方所有，如果把它视为普通情况，可能会被判断为已被占据
    // 若为王城被进攻的情况，让增援部队先加入，再让对方进攻
    else {
        int toi = to1[0], toj = to1[1];
        int moveN1 = moveN[0], moveN2 = moveN[1];
        // int moveN[] = {moveN1, moveN2};
        int toN = map[0][toi][toj], type = map[1][toi][toj];
        int r1 = toN, r2 = moveN1, r3 = moveN2;
        fight(r1, r2, r3);
        bool occupy1 = r2 > 0, occupy2 = r3 > 0;
        if (type == 2) {  // 共同攻击未被占领的城市
            map[0][toi][toj] = r1 + r2 + r3;
            if (occupy1 || occupy2) {
                map[1][toi][toj] = occupy1 ? 5 : 6;
            }
        } else {  // 已被某一方占据的土地
            int belong = type % 2 == 0 ? 1 : 0;
            int defenceN = moveN[belong], enemyN = moveN[1 - belong];
            toN += defenceN;  // 先加上增援部队
            bool occupy = enemyN > toN;
            map[0][toi][toj] = abs(enemyN - toN);
            if (occupy) {
                if (belong == 0) {
                    this->map[1][toi][toj] = type + 1;
                } else {
                    this->map[1][toi][toj] = type - 1;
                }
                // this->map[1][toi][toj] = (type +1) ? (belong == 0):(type-1);
            }

            if (occupy && type >= 7) {  // 王城被攻下
                finish = true;
                winner = 1 - belong;
            }
        }
    }
    return 3;
}

// 将json格式转换，而且做一些安全检查
int Game::move(Json::Value move1_, Json::Value move2_, bool check = false) {
    int move1[3], move2[3];
    bool legal1 = true, legal2 = true;
    string errorMessage = "非法输入";
    if (check) {
        foi(3) {
            try {
                move1[i] = move1_[i].asInt();
            } catch (...) {
                legal1 = false;
            }
            try {
                move2[i] = move2_[i].asInt();
            } catch (...) {
                legal2 = false;
            }
        }
        if ((!legal1) && (!legal2)) {
            errormessage[0] = errorMessage;
            errormessage[1] = errorMessage;
            errorcode = 2;
            return 2;
        }
        if ((!legal1)) {
            errormessage[0] = errorMessage;
            errorcode = 0;
            return 0;
        }
        if ((!legal2)) {
            errormessage[1] = errorMessage;
            errorcode = 1;
            return 1;
        }
    } else {
        foi(3) {
            move1[i] = move1_[i].asInt();
            move2[i] = move2_[i].asInt();
        }
    }
    return move(move1, move2, check);
}

// 计算双方能看到的视野，并且计算双方兵力，领土
void Game::mapCompute() {
    memset(mapVision1, -1, sizeof(mapVision1));
    memset(mapVision2, -1, sizeof(mapVision2));
    memset(army, 0, sizeof(army));
    memset(land, 0, sizeof(land));

    foi(height) foj(width) {
        int type = map[1][i][j];
        if (type == 1) {
            mapVision1[0][i][j] = map[0][i][j];
            mapVision1[1][i][j] = map[1][i][j];
            mapVision2[0][i][j] = map[0][i][j];
            mapVision2[1][i][j] = map[1][i][j];
        } else if (type >= 3) {   // 被占据的地方
            if (type % 2 == 1) {  // 被user0占据
                army[0] += map[0][i][j];
                ++land[0];
                for (int i1 = 0; i1 < 9; ++i1) {
                    int newi = i + square9i[i1], newj = j + square9j[i1];
                    if (newi >= 0 && newi < height && newj >= 0 &&
                        newj < width) {
                        mapVision1[0][newi][newj] = map[0][newi][newj];
                        mapVision1[1][newi][newj] = map[1][newi][newj];
                    }
                }
            } else {  // 被user1占据
                army[1] += map[0][i][j];
                ++land[1];
                for (int i1 = 0; i1 < 9; ++i1) {
                    int newi = i + square9i[i1], newj = j + square9j[i1];
                    if (newi >= 0 && newi < height && newj >= 0 &&
                        newj < width) {
                        mapVision2[0][newi][newj] = map[0][newi][newj];
                        mapVision2[1][newi][newj] = map[1][newi][newj];
                    }
                }
            }
        }
    }
}

// 从init数据中重建地图
void Game::loadMap(Json::Value value) {
    fok(2) foi(height) foj(width) map[k][i][j] = value["map"][k][i][j].asInt();
    foi(2) foj(2) generals[i][j] = value["generals"][i][j].asInt();
}

// 当有错误时，这个函数负责处理回传的数据
void Game::printError() {
    // errorcode:
    // 0 user1's error
    // 1 user2's error
    // 2 both error
    int score[2];
    if (errorcode == 0) {
        score[0] = -5;
        score[1] = 5;
    } else if (errorcode == 1) {
        score[0] = 5;
        score[1] = -5;
    } else if (errorcode == 2) {
        score[0] = -5;
        score[1] = -5;
    }
    if (NOSCORECHANGE) {
        score[0] = 0;
        score[1] = 0;   
    }
    Json::Value output;
    output["command"] = "finish";
    output["content"]["0"] = score[0];
    output["content"]["1"] = score[1];
    output["display"]["errorcode"] = this->errorcode;
    output["display"]["errorinfo"][ind(0)] = this->errormessage[0];
    output["display"]["errorinfo"][ind(1)] = this->errormessage[1];
    output["display"]["status"] = "error";
    output["display"]["time"] = this->currenttime;

    Json::FastWriter writer;
    cout << writer.write(output) << endl;
}

// 游戏结束时负责处理回传的数据
void Game::printFinish() {
    // winner:
    // 0: user1
    // 1: user2
    // 2: draw
    // -1: unknown
    int score[2];
    if (winner == 0) {
        score[0] = 5;
        score[1] = -5;
    } else if (winner == 1) {
        score[0] = -5;
        score[1] = 5;
    } else if (winner == 2) {
        score[0] = -1;
        score[1] = -1;
    }
    if (NOSCORECHANGE) {
        score[0] = 0;
        score[1] = 0;   
    }
    Json::Value output;
    output["command"] = "finish";
    output["content"]["0"] = score[0];
    output["content"]["1"] = score[1];
    output["display"]["finishcode"] = winner;
    this->mapCompute();

    foi(2) {
        output["display"]["army"][ind(i)] = this->army[i];
        output["display"]["land"][ind(i)] = this->land[i];
    }
    fok(2) foi(this->height) foj(this->width) {
        output["display"]["map"][ind(k)][ind(i)][ind(j)] = this->map[k][i][j];
    }
    output["display"]["size"][ind(0)] = this->height;
    output["display"]["size"][ind(1)] = this->width;
    output["display"]["status"] = "finish";
    output["display"]["time"] = this->currenttime;

    Json::FastWriter writer;
    cout << writer.write(output) << endl;
}

// 第二个玩家在获取数据时，数据需要被翻转为他自己是第一个玩家的视角
inline int visionReverse(int num1) {
    if (num1 > 2) {
        if (num1 % 2 == 0) {
            num1--;
        } else {
            num1++;
        }
    }
    return num1;
}

void initmapsize(int& height, int& width, bool random) {
    if (!random) {
        height = 16;
        width = 16;
        return ;
    }
    height = u_size(randeng);
    width = u_size(randeng);
    return ;
}

int main() {
    // freopen("debug.in","r",stdin);
    // freopen("debug.json","w",stdout);
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input, log, output;
    reader.parse(str, input);
    int height, width;
    
    log = input["log"];

    if (log.size() == 0) {
        initmapsize(height, width, randomap);
    } else {
        height = input["initdata"]["height"].asInt();
        width = input["initdata"]["width"].asInt();
    }
    Game game(height,width);
        


    output["content"]["0"]["size"][ind(0)] = height;
    output["content"]["0"]["size"][ind(1)] = width;
    output["content"]["1"]["size"][ind(0)] = height;
    output["content"]["1"]["size"][ind(1)] = width;

    if (log.size() == 0) {  // init game
        game.initMap();
        game.mapCompute();
        output["command"] = "request";
        output["initdata"]["height"] = height;
        output["initdata"]["width"] = width;

        output["content"]["0"]["enemy"][ind(0)] = InitGeneralsNum;
        output["content"]["0"]["enemy"][ind(1)] = 1;
        output["content"]["1"]["enemy"][ind(0)] = InitGeneralsNum;
        output["content"]["1"]["enemy"][ind(1)] = 1;

        output["display"]["status"] = "open";
        output["display"]["time"] = 0;
        output["display"]["army"][ind(0)] = 4;
        output["display"]["land"][ind(0)] = 1;
        output["display"]["army"][ind(1)] = 4;
        output["display"]["land"][ind(1)] = 1;
        output["display"]["size"][ind(0)] = height;
        output["display"]["size"][ind(1)] = width;

        fok(2) foi(height) foj(width) {
            output["content"]["0"]["map"][ind(k)][ind(i)][ind(j)] =
                game.mapVision1[k][i][j];
            if (k == 1)
                output["content"]["1"]["map"][ind(k)][ind(i)][ind(j)] =
                    visionReverse(game.mapVision2[k][i][j]);
            else
                output["content"]["1"]["map"][ind(k)][ind(i)][ind(j)] =
                    game.mapVision2[k][i][j];
            output["initdata"]["map"][ind(k)][ind(i)][ind(j)] =
                game.map[k][i][j];
            output["display"]["map"][ind(k)][ind(i)][ind(j)] =
                game.map[k][i][j];
        }

        output["content"]["0"]["time"] = 0;
        output["content"]["1"]["time"] = 0;

        foi(2) foj(2) output["initdata"]["generals"][ind(i)][ind(j)] =
            game.generals[i][j];
        foi(2) {
            output["content"]["0"]["generals"][ind(i)] = game.generals[0][i];
            output["content"]["1"]["generals"][ind(i)] = game.generals[1][i];
        }
    } else {  // continue
        game.loadMap(input["initdata"]);
        Json::Value opt1, opt2;
        bool verdict1, verdict2;
        int inputSize = input["log"].size();
        int legalAns;
        output["display"]["status"] = "open";
        for (ind i = 1; i < inputSize; i += 2) {
            opt1 = input["log"][i]["0"]["response"];
            opt2 = input["log"][i]["1"]["response"];

            if (i == inputSize - 1) {  // 最后一次移动，需要安全检查
                verdict1 = strcmp(input["log"][i]["0"]["verdict"].asCString(),
                                  "OK") == 0;
                verdict2 = strcmp(input["log"][i]["1"]["verdict"].asCString(),
                                  "OK") == 0;
                if (!(verdict1 && verdict2)) {
                    game.errorcode = 2;
                    if (verdict1)
                        game.errorcode = 1;
                    if (verdict2)
                        game.errorcode = 0;
                    if (!verdict1) {
                        game.errormessage[0] =
                            input["log"][i]["0"]["verdict"].asString();
                    }
                    if (!verdict2) {
                        game.errormessage[1] =
                            input["log"][i]["1"]["verdict"].asString();
                    }

                    game.printError();
                    return 0;
                }
                legalAns = game.move(opt1, opt2, true);
            } else {  // 之前检查过了是否安全
                game.move(opt1, opt2);
            }
            ++game.currenttime;
            game.scheduleAdd();
        }
        output["display"]["time"] = game.currenttime;

        if (game.currenttime >= TTO) {  // time to over
            game.finish = true;
            game.winner = 2;
            game.printFinish();
            return 0;
        }
        if (legalAns == 3) {
            if (!game.finish) {
                output["command"] = "request";
                game.mapCompute();

                foi(game.height) foj(game.width) {
                    output["content"]["0"]["map"][ind(0)][ind(i)][ind(j)] =
                        game.mapVision1[0][i][j];
                    output["content"]["0"]["map"][ind(1)][ind(i)][ind(j)] =
                        game.mapVision1[1][i][j];
                    output["content"]["1"]["map"][ind(0)][ind(i)][ind(j)] =
                        game.mapVision2[0][i][j];
                    output["content"]["1"]["map"][ind(1)][ind(i)][ind(j)] =
                        visionReverse(game.mapVision2[1][i][j]);
                }
                foi(2) {
                    output["content"]["0"]["generals"][ind(i)] =
                        game.generals[0][i];
                    output["content"]["1"]["generals"][ind(i)] =
                        game.generals[1][i];
                    output["display"]["army"][ind(i)] = game.army[i];
                    output["display"]["land"][ind(i)] = game.land[i];
                }
                fok(2) foi(game.height) foj(game.width) {
                    output["display"]["map"][ind(k)][ind(i)][ind(j)] =
                        game.map[k][i][j];
                }
                output["display"]["size"][ind(0)] = game.height;
                output["display"]["size"][ind(1)] = game.width;

                output["content"]["0"]["time"] = game.currenttime;
                output["content"]["1"]["time"] = game.currenttime;
                output["content"]["0"]["enemy"][ind(0)] = game.army[1];
                output["content"]["0"]["enemy"][ind(1)] = game.land[1];
                output["content"]["1"]["enemy"][ind(0)] = game.army[0];
                output["content"]["1"]["enemy"][ind(1)] = game.land[0];
            } else {  // game is over
                game.printFinish();
                return 0;
            }
        } else {  // error action
            game.printError();
            return 0;
        }
    }
    Json::FastWriter writer;
    cout << writer.write(output) << endl;
    return 0;
}