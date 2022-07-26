#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>
#include "jsoncpp/json.h"
#define ind Json::Value::ArrayIndex

#define TTO 500
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
// [i,j,d]
// winner:
// 0: user1 
// 1: user2
// 2: draw
// -1: unknown
class Game
{
public:
    int height, width;
    int generals[2][2];
    int map[2][SquareHeight][SquareWidth];
    int mapVision1[2][SquareHeight][SquareWidth];
    int mapVision2[2][SquareHeight][SquareWidth];
    int mapVisionBoth[2][SquareHeight][SquareWidth];
    int fogDisplay1[SquareHeight][SquareWidth];
    int fogDisplay2[SquareHeight][SquareWidth];
    int fogDisplayBoth[SquareHeight][SquareWidth];
    int army[2];
    int land[2];

    bool finish;
    int winner,errorcode;
    int currenttime;
    string errormessage[2];
    
    Game(int height_ = SquareHeight, int width_ = SquareWidth)
    {
        height = height_;
        width = width_;
        winner = -1;
        currenttime = 0;
        finish = false;
        errormessage[0] = "";
        errormessage[1] = "";
        memset(map,0,sizeof(map));
        memset(mapVision1,0,sizeof(mapVision1));
        memset(mapVision2,0,sizeof(mapVision2));
        memset(mapVisionBoth,0,sizeof(mapVisionBoth));
    }
    void initMap();
    bool generalsCheck();
    bool mapCheck();
    void bfs(int[][SquareWidth], int, int);
    inline int sum(int[][SquareWidth]);
    void fight(int &, int &);
    void fight(int &, int &, int &);
    void scheduleAdd();
    int move(int[3], int[3], bool);
    void move(int[3], int user, int moveN);
    int move(Json::Value, Json::Value, bool);
    void mapCompute();
    void loadMap(Json::Value);
    void printFinish();
    void printError();
};

bool Game::generalsCheck()
{
    float distance = sqrt((generals[0][0] - generals[1][0]) * (generals[0][0] - generals[1][0]) +
                          (generals[0][1] - generals[1][1]) * (generals[0][1] - generals[1][1]));
    if (distance < (height + width) / 4)
    // if (distance > (height + width) / 6)
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
        srand((unsigned)time(NULL));
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
    if (n1<n2) {
        n2 -= n1; n1 = 0;
    } else {
        n1 -= n2; n2 = 0;
    }
}

void Game::fight(int &f1, int &n1, int &n2)
{
    int f1_=f1,n1_=n1,n2_=n2;
    int numb[] = {f1_, n1_, n2_};
    int point = 1 ? n1_ >= n2_ : 2;
    int lef1 = abs(numb[point] - f1_);
    int lef2 = abs(lef1 - numb[3 - point]);
    int Npoint = point ? numb[point] > f1_ : 0;
    Npoint = Npoint ? lef1 > numb[3 - point] : 0;
    int ret[] = {0, 0, 0};
    ret[Npoint] = lef2;
    f1 = ret[0];
    n1 = ret[1];
    n2 = ret[2];
}

void Game::scheduleAdd()
{
    if (currenttime % 8 == 0)
    {
        foi(height) foj(width) if (map[1][i][j] >= 3)++ map[0][i][j];
    }
    else if (currenttime % 2 == 0)
    {
        foi(height) foj(width) if (map[1][i][j] >= 5)++ map[0][i][j];
    }
}

void Game::move(int move[3], int user, int moveN) {
	int toi = move[0] + movei[move[2]], toj = move[1] + movej[move[2]];
	int toN = this->map[0][toi][toj], type=this->map[1][toi][toj];
	int r1 = toN, r2 = moveN;
	fight(r1, r2);
	int maxr = r1 + r2;
	bool occupy = r2 > 0;
	if (type == 0) {   // free square
		this->map[1][toi][toj] = 3 + user; // 3:user1 4:user2
		this->map[0][toi][toj] = moveN;
	}
	else if (type == 2) { // free city
		this->map[0][toi][toj] = maxr;
		// 只有当进攻的兵力有富余时才算占领了，剩余0不认为是占领
		if (occupy)
			this->map[1][toi][toj] = 5 + user;
	}
	else if (type == user + 3 || type == user + 5 || type == user + 7) {
		// 己方的土地或者city或者王城
		this->map[0][toi][toj] += moveN;
	}
	else if (type == 6 - user || type == 4 - user) {
		//  敌方的city或者土地
		this->map[0][toi][toj] = maxr;
		if (occupy) {
			if (user == 0)
				--this->map[1][toi][toj];
			else
				++this->map[1][toi][toj];
		}
	}
	else if (type == 8 - user) { // 敌方的王城
		this->map[0][toi][toj] = maxr;
		if (occupy) {
			this->map[1][toi][toj] = 7 + user;
			finish = true;
			winner = user;
		}
	} 
    // else {
    //     assert(0);
    // }
}


int Game::move(int move1[3], int move2[3],bool check=false)
{
    /*  return 3 OK
        return 0 user1 error
        return 1 user2 error
        return 2 both error
    */
    string messageMap[] = {
        "非法方向",
        "非法起始位置",
        "移动终点超出地图",
        "起始位置士兵数量不足",
        "移动终点不能为山地",
    };

    int *moveAll[2] = {move1, move2};
    int moveN[2] = {-1,-1};
    bool legal1 = true, legal2 = true;
	bool user0Stay = move1[0] == -1 && move1[1] == -1 && move1[2] == -1;
	bool user1Stay = move2[0] == -1 && move2[1] == -1 && move2[2] == -1;
    if (check) {
        // 方向非法
        if (move1[2] < -1 || move1[2]>3) {
            legal1 = false; move1[2]=0; 
            errormessage[0]+=messageMap[0];
        }
        if (move2[2] < -1 || move2[2]>3) {
            legal2 = false; move2[2]=0;
            errormessage[1]+=messageMap[0];
        }
        int to1[] = {move1[0] + movei[move1[2]], move1[1] + movej[move1[2]]};
        int to2[] = {move2[0] + movei[move2[2]], move2[1] + movej[move2[2]]};
        if (user0Stay) {legal1 = false; }
        if (user1Stay) {legal2 = false; }
        // 起始位置非法 
        if (legal1 && !((map[1][move1[0]][move1[1]] == 3) ||
            (map[1][move1[0]][move1[1]] == 5) ||
            (map[1][move1[0]][move1[1]] == 7))) {
                legal1 = false;
                errormessage[0]+=messageMap[1];
            }
            
        if (legal2 && !((map[1][move2[0]][move2[1]] == 4) ||
            (map[1][move2[0]][move2[1]] == 6) ||
            (map[1][move2[0]][move2[1]] == 8))) {
                legal2 = false;
                errormessage[1]+=messageMap[1];
            }
            
        // 非法越界
        if (legal1 && (to1[0] <0 || to1[0]>=height || to1[1]<0 || to1[1]>=width)) {
            legal1 = false;
            errormessage[0]+=messageMap[2];
        }
			
        if (legal2 && (to2[0] <0 || to2[0]>=height || to2[1]<0 || to2[1]>=width)) {
            legal2 = false;
            errormessage[1]+=messageMap[2];
        }
			


        // 数量不够
        if (legal1 && map[0][move1[0]][move1[1]] <= 1) {
            legal1 = false;
            errormessage[0]+=messageMap[3];
        }
            
        if (legal2 && map[0][move2[0]][move2[1]] <= 1) {
            legal2 = false;
            errormessage[1]+=messageMap[3];
        }
            
        // 移动到山上
        if (legal1 && map[1][to1[0]][to1[1]] == 1) {
            legal1 = false;
            errormessage[0]+=messageMap[4];
        }
            
        if (legal2 && map[1][to2[0]][to2[1]] == 1) {
            legal2 = false;
            errormessage[1]+=messageMap[4];
        }
            
    }
	// 不移动为合法
	if (user0Stay) legal1 = true; 
	if (user1Stay) legal2 = true;
    if (!legal1 && !legal2) {errorcode=2; return 2;}
    if (!legal1)  {errorcode = 0; return 0;}
    if (!legal2)  {errorcode = 1; return 1;}
    int to1[] = {move1[0] + movei[move1[2]], move1[1] + movej[move1[2]]};
    int to2[] = {move2[0] + movei[move2[2]], move2[1] + movej[move2[2]]};
    // 若选择移动，则先把兵力调出，再考虑放入的问题
    if (!user0Stay) {
        moveN[0] = map[0][move1[0]][move1[1]]-1;
        map[0][move1[0]][move1[1]] = 1; }
    if (!user1Stay) {
        moveN[1] = map[0][move2[0]][move2[1]]-1;
        map[0][move2[0]][move2[1]] = 1;
    } 
    if (!(
            to1[0] == to2[0] && to1[1] == to2[1] && 
            map[1][to1[0]][to1[1]] != 0 && (!user0Stay) && (!user1Stay)))
    {
        fouser {
            int *thismove = moveAll[user];
			if (((user == 0) && user0Stay) || (user == 1) && user1Stay) 
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
        int moveN1 = moveN[0],moveN2=moveN[1];
        // int moveN[] = {moveN1, moveN2};
        int toN = map[0][toi][toj], type=map[1][toi][toj];
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
    int move1[3],move2[3];
    foi(3) {
        move1[i] = move1_[i].asInt();
        move2[i] = move2_[i].asInt();
    }
    return move(move1,move2,check);
}

void Game::mapCompute() {
    memset(mapVision1,-1,sizeof(mapVision1));
    memset(mapVision2,-1,sizeof(mapVision2));
    memset(mapVisionBoth,-1,sizeof(mapVisionBoth));
    memset(army,0,sizeof(army));
    memset(land,0,sizeof(land));
    foi(height) foj(width) {
        fogDisplay1[i][j] = 1;
        fogDisplay2[i][j] = 1;
        fogDisplayBoth[i][j] = 1;
    }
    foi(height) foj(width) {
        int type = map[1][i][j];
        if (type == 1) {
            mapVision1[0][i][j] = map[0][i][j];
            mapVision1[1][i][j] = map[1][i][j];
            mapVision2[0][i][j] = map[0][i][j];
            mapVision2[1][i][j] = map[1][i][j];
            mapVisionBoth[0][i][j] = map[0][i][j];
            mapVisionBoth[1][i][j] = map[1][i][j];
        } else if (type >= 3) { //被占据的地方
            if (type % 2 == 1) { // 被user0占据
                army[0] += map[0][i][j];
                ++land[0];
                for (int i1=0;i1<9;++i1) {
                    int newi = i + square9i[i1], newj = j + square9j[i1];
                        if (newi>=0 && newi<SquareHeight && newj>=0 && newj<SquareWidth) {
                            mapVision1[0][newi][newj] = map[0][newi][newj];
                            mapVision1[1][newi][newj] = map[1][newi][newj];    
                            mapVisionBoth[0][newi][newj] = map[0][newi][newj];
                            mapVisionBoth[1][newi][newj] = map[1][newi][newj];
                            fogDisplay1[newi][newj] = 0;
                            fogDisplayBoth[newi][newj] = 0;
                        }
                }
            } else { // 被user1占据
                army[1] += map[0][i][j];
                ++land[1];
                for (int i1=0;i1<9;++i1) {
                    int newi = i + square9i[i1], newj = j + square9j[i1];
                        if (newi>=0 && newi<SquareHeight && newj>=0 && newj<SquareWidth) {
                            mapVision2[0][newi][newj] = map[0][newi][newj];
                            mapVision2[1][newi][newj] = map[1][newi][newj];  
                            mapVisionBoth[0][newi][newj] = map[0][newi][newj];
                            mapVisionBoth[1][newi][newj] = map[1][newi][newj];
                            fogDisplay2[newi][newj] = 0;
                            fogDisplayBoth[newi][newj] = 0;

                        }
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

void Game::printError() {
    // errorcode: 
    // 1 user1's error
    // 2 user2's error
    // 3 both error
    int score[2] ;
    if (errorcode == 0) {
        score[0] = -5; score[1] = 5;
    } else if (errorcode == 1) {
        score[0] = 5; score[1] = -5;
    } else if (errorcode == 2) {
        score[0] = -5; score[1] = -5;
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

    // display: TODO:

	Json::FastWriter writer;
	cout << writer.write(output) << endl;
}

void Game::printFinish() {
    // winner:
    // 0: user1 
    // 1: user2
    // 2: draw
    // -1: unknown
    int score[2] ;
    if (winner == 0) {
        score[0] = 5; score[1] = -5;
    } else if (winner == 1) {
        score[0] = -5; score[1] = 5;
    } else if (winner == 2) {
        score[0] = -1; score[1] = -1;
    }
	Json::Value output;
    output["command"] = "finish";
    output["content"]["0"] = score[0];
    output["content"]["1"] = score[1];
    output["display"]["finishcode"] = winner;
    this->mapCompute();
    foi(this->height) foj(this->width) {
        output["display"]["user0"]["fog"][ind(i)][ind(j)] = this->fogDisplay1[i][j];
        output["display"]["user1"]["fog"][ind(i)][ind(j)] = this->fogDisplay2[i][j];
        output["display"]["both"]["fog"][ind(i)][ind(j)] = this->fogDisplayBoth[i][j];
    }
    foi(2) {
        output["display"]["army"][ind(i)] = this->army[i];
        output["display"]["land"][ind(i)] = this->land[i];
    }
    fok(2) foi(this->height) foj(this->width) {
        output["display"]["full"]["map"][ind(k)][ind(i)][ind(j)] = this->map[k][i][j];
        output["display"]["user0"]["map"][ind(k)][ind(i)][ind(j)] = this->mapVision1[k][i][j];
        output["display"]["user1"]["map"][ind(k)][ind(i)][ind(j)] = this->mapVision2[k][i][j];
        output["display"]["both"]["map"][ind(k)][ind(i)][ind(j)] = this->mapVisionBoth[k][i][j];
    }
    output["display"]["status"] = "finish";
    output["display"]["time"] = this->currenttime;   


    // display TODO:
	Json::FastWriter writer;
	cout << writer.write(output) << endl;
}

inline int visionReverse(int num1) {
    if (num1 > 2) {
        if (num1 % 2 == 0) {
            num1 --;
        } else {
            num1 ++;
        }
    }
    return num1;
}

int main() {
	// freopen("debug.in","r",stdin);
	// freopen("debug.json","w",stdout);
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,log, output;
	reader.parse(str, input);
    Game game;
    output["content"]["0"]["size"][ind(0)] = SquareHeight;
    output["content"]["0"]["size"][ind(1)] = SquareWidth;
    output["content"]["1"]["size"][ind(0)] = SquareHeight;
    output["content"]["1"]["size"][ind(1)] = SquareWidth;
    log = input["log"];
    if (log.size() == 0) {
        // display: TODO:
        game.initMap();
        game.mapCompute();
        output["command"] = "request";
        output["initdata"]["height"] = game.height;
        output["initdata"]["width"] = game.width;
        output["display"]["status"] = "open";
        output["display"]["time"] = 0;
        output["display"]["army"][ind(0)] = 4; output["display"]["land"][ind(0)] = 1;
        output["display"]["army"][ind(1)] = 4; output["display"]["land"][ind(1)] = 1;
        
        fok(2) foi(game.height) foj(game.width) {
            output["content"]["0"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision1[k][i][j];
            if (k == 1)
                output["content"]["1"]["map"][ind(k)][ind(i)][ind(j)] = visionReverse(game.mapVision2[k][i][j]);
            else
                output["content"]["1"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision2[k][i][j];
            output["display"]["user0"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision1[k][i][j];
            output["display"]["user1"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision2[k][i][j];
            output["display"]["both"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVisionBoth[k][i][j];
            
            output["initdata"]["map"][ind(k)][ind(i)][ind(j)] = game.map[k][i][j];
            output["display"]["full"]["map"][ind(k)][ind(i)][ind(j)] = game.map[k][i][j];
        }

        output["content"]["0"]["time"] = 0;
        output["content"]["1"]["time"] = 0;

        foi(game.height) foj(game.width) {
            output["display"]["user0"]["fog"][ind(i)][ind(j)] = game.fogDisplay1[i][j];
            output["display"]["user1"]["fog"][ind(i)][ind(j)] = game.fogDisplay2[i][j];
            output["display"]["both"]["fog"][ind(i)][ind(j)] = game.fogDisplayBoth[i][j];
            
        }

        foi(2) foj(2) 
            output["initdata"]["generals"][ind(i)][ind(j)] = game.generals[i][j];
        foi(2) {
            output["content"]["0"]["generals"][ind(i)] = game.generals[0][i];
            output["content"]["1"]["generals"][ind(i)] = game.generals[1][i];
        } 
    } else {
        game.loadMap(input["initdata"]);
        Json::Value opt1, opt2;
        int inputSize = input["log"].size();
        int legalAns ;
        output["display"]["status"] = "open";
		for (ind i = 1; i < inputSize; i += 2) {
			opt1 = input["log"][i]["0"]["response"];
			opt2 = input["log"][i]["1"]["response"];
            if (i == inputSize-1)
                 legalAns = game.move(opt1,opt2,true);
            else 
                game.move(opt1,opt2);
            ++game.currenttime ;
            game.scheduleAdd();
        }
        output["display"]["time"] = game.currenttime;
        // fixTODO: 
        if (game.currenttime>=TTO) {
            game.finish = true;
            game.winner = 2;
            game.printFinish();
            return 0;
        }
        if (legalAns == 3) {
            if (!game.finish ) {
                output["command"] = "request";
                game.mapCompute();
                  
                foi(game.height) foj(game.width) {
                    output["content"]["0"]["map"][ind(0)][ind(i)][ind(j)] = game.mapVision1[0][i][j];
                    output["content"]["0"]["map"][ind(1)][ind(i)][ind(j)] = game.mapVision1[1][i][j];
                    output["content"]["1"]["map"][ind(0)][ind(i)][ind(j)] = game.mapVision2[0][i][j];
                    output["content"]["1"]["map"][ind(1)][ind(i)][ind(j)] = visionReverse(game.mapVision2[1][i][j]);

                    output["display"]["user0"]["fog"][ind(i)][ind(j)] = game.fogDisplay1[i][j];
                    output["display"]["user1"]["fog"][ind(i)][ind(j)] = game.fogDisplay2[i][j];
                    output["display"]["both"]["fog"][ind(i)][ind(j)] = game.fogDisplayBoth[i][j];
                }
                foi(2) {
                    output["content"]["0"]["generals"][ind(i)] = game.generals[0][i];
                    output["content"]["1"]["generals"][ind(i)] = game.generals[1][i];
                    output["display"]["army"][ind(i)] = game.army[i];
                    output["display"]["land"][ind(i)] = game.land[i];
                }
                fok(2) foi(game.height) foj(game.width) {
                    output["display"]["full"]["map"][ind(k)][ind(i)][ind(j)] = game.map[k][i][j];
                    output["display"]["user0"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision1[k][i][j];
                    output["display"]["user1"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVision2[k][i][j];
                    output["display"]["both"]["map"][ind(k)][ind(i)][ind(j)] = game.mapVisionBoth[k][i][j];
                }


                output["content"]["0"]["time"] = game.currenttime ;
                output["content"]["1"]["time"] = game.currenttime ;
                // display TODO:
            } else {
                game.printFinish();
                return 0;
            }
        }
        else {
            game.printError();
            return 0;
        }


    }
	Json::FastWriter writer;
	cout << writer.write(output) << endl;
    return 0;
}