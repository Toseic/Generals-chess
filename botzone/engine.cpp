#include <iostream>

using namespace std;

bool statecheck(bool & isend) {
    return false;
}

int main() {
    bool isend = false;
    // deploy 
    // judge bot1 bot2 state
    system("(./deploy begin 0) > ./runfile/judgein.json ");
    // system("./deploy begin 0");
    // return 0;
    // system("(./a begin)< ain.in > aout.out");
    while (!isend) {
        system("../judge < ./runfile/judgein.json > ./runfile/judgeout.json");
        // system("pause");
        return 0;
        system("(./deploy bot0 in) < ./runfile/judgeout.json > ./runfile/bot0in.json");
        system("(./deploy bot1 in) < ./runfile/judgeout.json > ./runfile/bot1in.json");
        system("./bot < ./runfile/bot0in.json > ./runfile/bot0out.json");
        system("./bot < ./runfile/bot1in.json > ./runfile/bot1out.json");
        system("./judge < ./runfile/bot0out.json > ./runfile/judgeout.json");
        system("(./deploy bot0 out) < ./runfile/judgeout.json > ./runfile/state");
        statecheck(isend);
        system("./judge < ./runfile/bot1out.json > ./runfile/judgeout.json");
        system("(./deploy bot1 out) < ./runfile/judgeout.json > ./runfile/state");
        statecheck(isend);
    }

    return 0;
}