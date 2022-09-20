# Generals-Chess 

将军棋（Generals-chess）是[botzone](https://www.botzone.org.cn/)上面的双人游戏

## 简介

将军棋完整复刻了[generals.io](https://generals.io)网站中的双人对战模式，并在此基础上进行了一些规则上的改变，此游戏以调兵遣将为主要玩点，在游戏中体验对抗中的战术与心理博弈。



#### 作者：

裁判程序，播放器： *Tosey*

示例程序：*MOLYHECI*

仓库地址：https://github.com/Toseic/Generals-chess/



## 游戏规则

<img src="https://dev.generals.io/replay.gif" style="zoom:50%;" />

本游戏为'''双人回合制'''游戏, 两个玩家是'''对抗'''关系. 

两名玩家分别是: 0 号玩家(红方), 1 号玩家(蓝方).

游戏在一张地图中进行，地图由16*16个方格构成，方格的类型有山地，土地，城市。

双方玩家各自拥有一个大本营，大本营本质上是城市的特殊类型，某一方的大本营被另一方占据即可认为游戏结束。初始时地图上双方玩家的大本营保证连通并且不会被山地圈在小范围内，另外保证大本营的距离大到可玩的程度



### 概念：

#### step

游戏中的时间单位，每一个step玩家有机会执行一次移动的命令，当然也可以什么也不做，双方的命令被同时提交和执行。

#### 山地

不可停留且不可经过的位置

#### 土地

可以停留士兵，每隔8step每个土地生产一个士兵的位置。

#### 士兵

在游戏中属于战斗的资本，以方格内的数字表示个数，可以被玩家移动

#### 城市

在游戏初始时处于无人占据的状态，玩家需要花上一定的兵力才可以将其攻下（在未被任何一方占据时城市不会增长士兵数量），并为自己所有，在被任何一方攻下后城市每隔2step生产一个所属一方的士兵。

在游戏初始时处于无人占据的状态，玩家需要花上一定的兵力才可以将其攻下（在未被任何一方占据时城市不会增长士兵数量），并为自己所有，在被攻下后城市每隔2step生产一个士兵。注意：每隔16s土地会生产一次士兵，而且此时恰好也是城市生产士兵的时间，这两个生产并不重叠，也就是说土地在生产士兵时是将城市排除在外的，城市的生产由自己管理。

#### 迷雾 
在游戏进行时，玩家无法看到地图的所有内容，玩家所能看到的包括：

> 整张地图所有的山峰，
> 所有被己方占据的土地和城市的周围的3*3距离的范围的加和。

#### 占据

当一方在土地上存在士兵即认为是被此方占据，而另一方需要派出更多的士兵才能将其攻下，而双方的士兵在战斗中会等量损失，特殊情况：如果被对方攻至士兵数目为0但是对方还没有派出更多的士兵攻击，此时仍被视为未被攻下。

例：

1.某一处原本有红方的10个士兵，蓝方派出20个士兵进攻，最后土地被蓝方占据，土地最后有10个蓝方士兵。

2.某一处原本有红方的10个士兵，蓝方派出10个士兵进攻，最后土地未被攻下，仍归红方所有，土地上面的士兵数目为0，此后的士兵生产仍正常进行。

#### 移动

玩家可以把自己的士兵从一个位置移到相邻的位置，而且每次移动时会在原位置留下一个士兵，也就是说一个位置至少要有2个士兵才能执行移动的操作，此处与原游戏中有一个不同之处：若原位置有x个士兵，在我们的规则中只能移动x-1个士兵，而在原游戏中可以选择移动(x-1)/2个士兵，我们这里对规则进行了简化，取消了这种移动的选择。

另外请注意，进攻另一方时发生的战斗是在调出士兵后发生的，流程：调出士兵-放入士兵-战斗。

例：ABC三块土地依次左右相邻， A地有20个红方士兵，B地有20个蓝方士兵，C地未被任何人占据，

双方操作：红方从A地移动10个士兵到B地，蓝方从B地移动10个士兵到C地

接下来正确的执行过程：先各从AB两地提出10个士兵，此时A地10个士兵，B地10个士兵，接下来将10个红方士兵放入B地，10个蓝方士兵放入C地，B地双方战斗完剩下0个士兵，B地未被攻下仍归蓝方所有。

错误执行过程：先从A地提出10个士兵，此时A地10个士兵，B地20个士兵，接下来将10个红方士兵放入B地，B地双方战斗完剩下10个蓝方士兵，接下来从B地提出10个士兵，被判非法操作（移动时必须在原地留下一个士兵）。

#### 时间限制
为避免双方都不积极移动所导致的游戏无法终止，在botzone中此游戏存在时间限制，在500回合后若游戏尚未结束，视为平局。



## 游戏交互方式

### 人类参与对局

用鼠标选中你想要移动的起始位置，接下来按下按键以移动：

| 按键    | 方向          |
| ------- | ------------- |
| w/up    | 向上移动(x--) |
| s/down  | 向下移动(x++) |
| a/left  | 向左移动(y--) |
| d/right | 向右移动(y++) |
| q/Enter | 一动不动      |



### 程序对局

本游戏每步(每次程序运行)限时 1 秒. 具体规则请参看[Bot](https://wiki.botzone.org.cn/index.php?title=Bot)词条或提交程序处的提示.

建议使用长时运行方式降低初始化开销.

如果希望在回合间传递数据, 请参阅[Bot#交互](https://wiki.botzone.org.cn/index.php?title=Bot#.E4.BA.A4.E4.BA.92).

如果你不能理解以下交互方式,可以直接看[#样例程序](https://wiki.botzone.org.cn/index.php?title=Generals-chess#.E6.A0.B7.E4.BE.8B.E7.A8.8B.E5.BA.8F), 从 `main` 函数开始修改

本游戏与Botzone上其他游戏一样, 使用相同的交互方式: [Bot#交互](https://wiki.botzone.org.cn/index.php?title=Bot#.E4.BA.A4.E4.BA.92).

本游戏只支持[JSON](https://wiki.botzone.org.cn/index.php?title=JSON)交互.

调试时可以通过拖动进度条来快速调试.

### bot输入

每回合[Bot](https://wiki.botzone.org.cn/index.php?title=Bot)收到的request**不是字符串**, 而是一个[JSON](https://wiki.botzone.org.cn/index.php?title=JSON)数组或对象, 格式如下:

**第一回合**

```json
{
  "requests": [
    {
      "enemy": [10, 1],
      "generals": [13, 8],
      "map": [
        [
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, 0, -1, -1,...],
          ... ,
        ],
        [
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, 1, -1, -1, 1, -1, ...],
          ... ,
        ]
      ],
      "size": [16, 16],
      "time": x
    }
  ],
  "responses": []
}
```

**其他回合**

其中的request 1～x,response 1~x，是bot的历史输入和输出，最后一个request是新的输入。

```json
{
  "requests": [
    "request 1",
    "request 2",
    "...",
    "request x",
    {
      "enemy": [10, 1],
      "generals": [13, 8],
      "map": [
        [
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, -1, -1, -1,...],
          [-1, -1, -1, 0, -1, -1,...],
          ... ,
        ],
        [
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, -1, -1, -1, -1, ...],
          [-1, -1, -1, 1, -1, -1, 1, -1, ...],
          ... ,
        ]
      ],
      "size": [16, 16],
      "time": 0
    }
  ],
  "responses": [
    "response 1",
    "response 2",
    "...",
    "response x"
  ]
}
```



**request内容**

| 名称     | 意义                                                         |
| -------- | ------------------------------------------------------------ |
| map      | 地图                                                         |
| time     | 当前时间，游戏开始时为0                                      |
| size     | 地图高度,宽度                                                |
| Generals | 己方大本营起始位置                                           |
| enemy    | 包含两个元素，第一个是对方玩家的士兵数目，第二个是对方玩家的占有的土地数目 |

**size内容**

`"size": [height,width]`

**map内容**

`map[2][height][width]`是一个三维数组，`map[0]`表示地图上的士兵数目，山地、空地为0。

`map[1]`表示土地类型：

| type | 含义             |
| ---- | ---------------- |
| -1   | 被迷雾遮挡不显示 |
| 0    | 无人占领的土地   |
| 1    | 山地             |
| 2    | 无人占领的城市   |
| 3    | 己方占领的土地   |
| 4    | 敌方占领的土地   |
| 5    | 己方占领的城市   |
| 6    | 敌方占领的城市   |
| 7    | 己方的王城       |
| 8    | 敌方的王城       |



**输出**

```
response = [x,y,d]
```

x,y代表横坐标和纵坐标，从0开始计算，左上角为（0，0）

d代表方向

| d    | 方向 |
| ---- | ---- |
| 0    | 上   |
| 1    | 下   |
| 2    | 左   |
| 3    | 右   |

特别的，

```
[-1,-1,-1]
```

代表原地不动



## 样例程序

建议仔细阅读样例程序的注释.

**C++ [JSON](https://wiki.botzone.org.cn/index.php?title=JSON)交互样例程序**

```cpp
/*  Generals-Chess cpp示例bot程序
    author: MOLYHECI
*/
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <string>
#include <ctime> 
#include "jsoncpp/json.h"
#define ind Json::Value::ArrayIndex

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
    // freopen("../debug.in", "r", stdin);
    // freopen("../debug.json", "w", stdout);
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input,info, history, output;
    reader.parse(str, input);
    Game game;

    info = input["request"][input["request"].size()-1];
    game.height = info["size"][ind(0)].asInt();
    game.width = info["size"][ind(1)].asInt();
    game.currenttime = info["time"].asInt();
    fok(2) foi(SquareHeight) foj(SquareWidth)
    {
        game.map[k][i][j] = info["map"][k][i][j].asInt();
        if (game.map[1][i][j] == 7)
        {
            game.generals[0] = i;
            game.generals[1] = j;
        }
    }

    // history 按照时间顺序从前往后排

    const int historyNum = input["response"].size();
    
    int historyMove[historyNum][3];
    int historyTimestamp[historyNum];
    foi(historyNum)
    {
        history = input["response"][i];
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
    Json::Value ret;
    // ret["output"].append(moves[index][0]);
    // ret["output"].append(moves[index][1]);
    // ret["output"].append(moves[index][2]);
    ret[ind(0)] = moves[index][0];
    ret[ind(1)] = moves[index][1];
    ret[ind(2)] = moves[index][2];
    Json::FastWriter writer;
    ret["response"] = ret;
    cout << writer.write(ret) << endl;
    return 0;
}
```

