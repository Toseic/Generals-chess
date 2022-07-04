import numpy as np
from enum import Enum
import random
from random import shuffle, randint
from math import sqrt
import time
import json

SquareSize = 16
InitGeneralsNum = 4
movei, movej = [-1, 1, 0, 0], [0, 0, -1, 1]


class status(Enum):
    begin = 1
    running = 2
    stop = 3


class Game:
    '''
    color:
    user1 red
    user2 blue
    ### self.map:   
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
    ### move: list
    [i,j,w,d]
    '''

    def __init__(self) -> None:

        self.height = SquareSize
        self.width = SquareSize
        self.generals = [[-1, -1], [-1, -1]]
        self.map = np.zeros((2, self.height, self.width))
        self.finish = [False, -1]
        self.time = 0

    def bfs(self, map, visit, i, j):
        if (i < 0 or i >= self.height or j < 0 or j >= self.width):
            return
        if map[(i, j)] in [1, 2] or visit[(i, j)]:
            return
        visit[(i, j)] = 1
        for i1 in range(4):
            self.bfs(map, visit, i+movei[i1], j+movej[i1])

    def mapCheck(self, map: np.ndarray, generals: list) -> bool:
        assert map.shape[0] == self.height and map.shape[1] == self.width, "error"
        visit1, visit2 = np.zeros((self.height, self.width)), np.zeros(
            (self.height, self.width))
        self.bfs(map, visit1, generals[0][0], generals[0][1])
        self.bfs(map, visit2, generals[1][0], generals[1][1])
        if visit1[(generals[1][0], generals[1][1])] != 1:  # 双方大本营不连通
            return False
        if np.sum(visit1) < self.height*self.width/2 or np.sum(visit2) < self.height*self.width/2:
            return False
        return True

    def generalsCheck(self, generals: list) -> bool:
        distance = sqrt((generals[0][0]-generals[1][0]) **
                        2 + (generals[0][1]-generals[1][1])**2)
        if distance < (self.height+self.width) / 4:
            return False
        return True

    def initMap(self):  # cost time = 7/10000s (M1 pro)
        area = self.height * self.width
        mountainNum = int(area * 1 / 5)
        cityNum = int(area * 1 / 20)
        legal = False
        while True:
            self.generals = [[randint(0, self.height-1), randint(0, self.width-1)], [
                randint(0, self.height-1), randint(0, self.width-1)]]
            if self.generalsCheck(self.generals):
                break
        while True:
            _map_1d = np.array([0]*(area-mountainNum-cityNum) +
                               [1]*mountainNum + [2]*cityNum)
            shuffle(_map_1d)
            _map = _map_1d.reshape((self.height, self.width))
            _map[(self.generals[0][0], self.generals[0][1])] = 7
            _map[(self.generals[1][0], self.generals[1][1])] = 8
            if self.mapCheck(_map, self.generals):
                self.map[1] = _map
                break
        for i in range(self.height):
            for j in range(self.width):
                if self.map[(1, i, j)] == 2:
                    self.map[(0, i, j)] = randint(40, 50)
                elif self.map[(1, i, j)] in [7, 8]:
                    self.map[(0, i, j)] = InitGeneralsNum

    def fight(self, *numb):
        '''
        numb:
             [free,user1,user2]
             [free, user1]
             [free, user2]
             [user1, user2]
        '''
        lenN = len(numb)
        if lenN == 2:
            [n1, n2] = numb
            r1 = 0 if n1 < n2 else n1-n2
            r2 = 0 if n2 < n1 else n2-n1
            return r1, r2
        else:
            [freeN, user1N, user2N] = numb
            point = 1 if user1N >= user2N else 2
            lef1 = abs(numb[point]-freeN)
            lef2 = abs(lef1-numb[3-point])
            Npoint = point if numb[point] > freeN else 0
            Npoint = Npoint if lef1 > numb[3-point] else 0

            ret = [0, 0, 0]
            ret[Npoint] = lef2
            assert ret[0] >= 0 and ret[1] >= 0 and ret[2] >= 0, "error"
            return ret[0], ret[1], ret[2]

    def move(self, move1: list, move2: list) -> bool:

        move1 = [move1[:][0], move1[:][1], move1[:][0]+movei[move1[:]
                                                             [3]], move1[:][1]+movej[move1[:][3]], move1[:][2]]
        move2 = [move2[:][0], move2[:][1], move2[:][0]+movei[move2[:]
                                                             [3]], move2[:][1]+movej[move2[:][3]], move1[:][2]]

        if not (
            self.map[(1, move1[0], move1[1])] in [3, 5, 7] and
            self.map[(1, move2[0], move2[1])] in [4, 6, 8]
        ):
            raise Exception("error")  # 非法操作
        if move1[4] <= 0 or move2[4] <= 0:
            raise Exception("error")  # 非法数量
        if self.map[(0, move1[0], move1[1])] <= move1[4] or self.map[(0, move2[0], move2[1])] <= move2[4]:
            raise Exception("error")  # 数量不够
        if self.map[(1, move1[2], move1[3])] == 1 or self.map[(1, move2[2], move2[3])] == 1:
            raise Exception("error")  # mountain
        # more check TODO
        self.map[(0, move1[0], move1[1])] -= move1[4]
        self.map[(0, move2[0], move2[1])] -= move2[4]
        if not (move1[2] == move2[2] and move1[3] == move2[3] and self.map[(1, move[2], move[3])] != 0):
            for user in range(2):
                move = [move1, move2][user]
                toi, toj, moveN = move[2], move[3],  move[4]
                toN, type_ = self.map[(0, toi, toj)], self.map[(1, toi, toj)]
                r1, r2 = self.fight(toN, moveN)
                maxr, occupy = r1+r2, r2 > 0
                if type_ == 0:  # free square
                    self.map[(1, toi, toj)] = 3+user  # 3:user1 4:user2
                    self.map[(0, toi, toj)] = moveN
                elif type_ == 2:  # free city
                    self.map[(0, toi, toj)] = maxr
                    # 只有当进攻的兵力有富余时才算占领了，剩余0不认为是占领
                    if occupy:
                        self.map[(1, toi, toj)] = 5+user
                elif type_ in [user+3, user + 5, user+7]:  
                    # 己方的city或者王城or square
                    self.map[(0, toi, toj)] += moveN
                elif type_ == 6-user:  # 敌方的city
                    self.map[(0, toi, toj)] = maxr
                    if occupy:
                        self.map[(1, toi, toj)] = 5+user
                elif type_ == 4-user:  # 敌方的土地
                    self.map[(0, toi, toj)] = maxr
                    if occupy:
                        self.map[(1, toi, toj)] = 3+user
                elif type_ == 8-user:  # 敌方王城
                    self.map[(0, toi, toj)] = maxr
                    if occupy:
                        self.map[(1, toi, toj)] = 7+user
                        self.finish = [True, user]

        # 当双方同时到达一块土地或者城市，以及某一方的王城时，需额外考虑
        # 若为无人占领的城市被进攻的情况，先让双方中人数多的先进攻，人数少的随之再进攻
        # 已被占领的城市同时被进攻和增援，需要额外考虑的原因是如果最后城市剩余士兵为0
        #   ，应当认为是未被敌方占据，仍为己方所有，如果把它视为普通情况，可能会被判断为已被占据
        # 若为王城被进攻的情况，让增援部队先加入，再让对方进攻
        else:
            toi, toj, moveN1, moveN2 = move1[2], move1[3], move1[4], move2[4]
            toN, type_ = self.map[(0, toi, toj)], self.map[(1, toi, toj)]
            r1, r2, r3 = self.fight(toN, moveN1, moveN2)
            occupy1, occupy2 = r2 > 0, r3 > 0
            if type_ == 2:  # free city
                self.map[(0, toi, toj)] = r1+r2+r3
                if occupy1 or occupy2:
                    self.map[(1, toi, toj)] = 5 if occupy1 else 6
            else:  # 已被某一方占据的土地
                belong = 1 if type_ % 2 == 0 else 0
                defenceN, enemyN = [moveN1, moveN2][belong], [
                    moveN1, moveN2][1-belong]
                toN += defenceN  # 先加上增援部队
                occupy = enemyN > toN
                self.map[(0, toi, toj)] = abs(enemyN-toN)
                if occupy:
                    self.map[(1, toi, toj)] = type_ + \
                        1 if belong == 0 else type_-1
                if occupy and type_ in [7, 8]:
                    self.finish = [True, 1-belong]

    def scheduleAdd(self):
        if self.time % 16 == 0:
            threeMask = np.ones((self.height, self.width))*3
            # 筛选出土地的type大于等于3的部分
            needAdd = self.map[1] >= threeMask
            needAdd = np.int32(needAdd)
            self.map[0] += needAdd
        elif self.time % 2 == 0:
            fiveMask = np.ones((self.height, self.width))*5
            # 筛选出土地的type大于等于5的部分
            needAdd = self.map[1] >= fiveMask
            needAdd = np.int32(needAdd)
            self.map[0] += needAdd

    def loadMap(self):
        # 从json中读取map信息
        # seedRandom = str(randint(0, 2147483647))
        # if "seed" in jsonfile["initdata"]:
        #     seedRandom = jsonfile["initdata"]["seed"]
        # random.seed(seedRandom)
        if "map" in jsonfile["initdata"] and "generals" in jsonfile["initdata"]:
            self.map = np.array(jsonfile["initdata"]["map"])
            self.generals = jsonfile["initdata"]["generals"]

    def maskExpand(self, mask: np.ndarray):
        # 迷雾中显示的范围由每个方块往外拓展3*3
        mask1 = mask.copy()
        for i in range(self.height):
            for j in range(self.width):
                if mask1[(i, j)]:
                    for i1 in range(4):
                        newi = i+movei[i1]
                        newj = j+movej[i1]
                        if (newi >= 0 and newi < self.height and newj >= 0 and newj < self.width):
                            mask[(newi, newj)] = 1
        # mask = np.int32(mask)

    def mapWithFog(self):
        mountainMask = np.ones((self.height, self.width))
        twoMask = np.ones((self.height, self.width))*2
        mountainShowMask = self.map[1] == mountainMask
        showMask = self.map[1] > twoMask  # 所有被占领的城市，土地，王城
        showSquare = np.multiply(self.map.copy(), showMask)
        showMask1 = ((showSquare[1] % twoMask) != 0) & (showSquare[1]>twoMask) # user1
        showMask2 = ((showSquare[1] % twoMask) == 0) & (showSquare[1]>twoMask) # user2
        self.maskExpand(showMask1)
        self.maskExpand(showMask2)
        showMask1 += mountainShowMask
        showMask2 += mountainShowMask
        showSquare1 = np.multiply(self.map, showMask1)
        showSquare1[1] += np.int32(~showMask1)*-1
        showSquare2 = np.multiply(self.map, showMask2)
        showSquare2[1] += np.int32(~showMask2)*-1
        return showSquare1.tolist(), showSquare2.tolist()


if __name__ == "__main__":
    game = Game()
    jsonfile = json.loads(input())
    log = jsonfile["log"]
    lenlog = len(log)
    if lenlog == 0:
        game.initMap()
        map1,map2 = game.mapWithFog()
        print(json.dumps({
            "command": "request",
            "content": {
                "0": map1,
                "1": map2
            },
            "display": {
                "0": [map1, game.generals[0]],
                "1": [map2, game.generals[1]]
            },
            "initdata": {
                "map": game.map.copy().tolist(),
                "generals": game.generals
            }
        }))
        exit(0) # 退出程序
    else:
        pass

