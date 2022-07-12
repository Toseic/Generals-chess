#include <iostream>
#include <fstream>
#include <string>

using namespace std;
ifstream ifs;


void statecheck(bool & isend) {
    string st;
    ifs.open("./runfile/state");
    ifs >> st;
    ifs.close();
    if (st == "open") {
        cout << "open here" << endl;
        isend = false;
    } else {
        isend = true;
    }
}

int main() {
    bool isend = false;
    // deploy 
    // judge bot1 bot2 state
    system("(./deploy begin 0) > ./runfile/log.json ");
    int count = 0;
    // system("./deploy begin 0");
    // return 0;
    // system("(./a begin)< ain.in > aout.out");
    while (!isend) {
        system("../judge < ./runfile/log.json > ./runfile/judgeout.json");
        system("(./deploy log state) < ./runfile/judgeout.json > ./runfile/state");
        statecheck(isend);
        if (isend) break;
        // return 0;
        system("(./deploy log judge) < ./runfile/judgeout.json");
        system("(./deploy log initdata) < ./runfile/judgeout.json");
        // return 0;
        system("(./deploy bot0 in) < ./runfile/judgeout.json > ./runfile/bot0in.json");
        system("(./deploy bot1 in) < ./runfile/judgeout.json > ./runfile/bot1in.json");
        // return 0;
        system("../bot/bot < ./runfile/bot0in.json > ./runfile/bot0out.json");
        system("../bot/bot < ./runfile/bot1in.json > ./runfile/bot1out.json");
        // return 0;
        system("(./deploy log bot0) < ./runfile/bot0out.json");
        system("(./deploy log bot1) < ./runfile/bot1out.json");
        // return 0;

        // system("(./deploy merge history) > history.json ");
        // system("../judge < ./runfile/log.json > ./runfile/judgeout.json");
        count ++;
        cout << count << endl;
        if (count >= 10) break;
        // return 0;

        // return 0;
        // system("(./deploy log state) < ./runfile/bot0out.json");

        // // system("(./deploy bot0 out) < ./runfile/judgeout.json > ./runfile/state");
        // return 0;
        // statecheck(isend);
        // system("./judge < ./runfile/bot1out.json > ./runfile/judgeout.json");
        // system("(./deploy bot1 out) < ./runfile/judgeout.json > ./runfile/state");
        // statecheck(isend);
    }

    return 0;
}