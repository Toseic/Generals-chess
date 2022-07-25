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
    system("echo "" > ./runfile/bot0in.json");
    system("echo "" > ./runfile/bot1in.json");
    system("echo "" > ./runfile/log.json");
    system("echo "" > ./runfile/judgein.json");
    int count = 0;
    // system("./deploy begin 0");
    // return 0;
    // system("(./a begin)< ain.in > aout.out");
    while (!isend) {
        // if (count +1 == 2) return 0;
        system("../judge < ./runfile/log.json > ./runfile/judgeout.json");
        // return 0;

        
        // cout << "2" << endl;  // return 0;
        // system("pause");
        system("(./deploy log state) < ./runfile/judgeout.json > ./runfile/state");
        // cout << "3" << endl;
        statecheck(isend);
        if (isend) break;
        system("python3 ../debug.py < ./runfile/judgeout.json");
        // cout << "4" << endl;
        // return 0;
        system("(./deploy log judge) < ./runfile/judgeout.json");
        // cout << "5" << endl;
        // return 0;
        if (count == 0)
            system("(./deploy log initdata) < ./runfile/judgeout.json");
        // return 0;

        system("(./deploy bot0 in) < ./runfile/judgeout.json > ./runfile/bot0in.json");
        system("(./deploy bot1 in) < ./runfile/judgeout.json > ./runfile/bot1in.json");
        // return 0;
        // cout << "6" << endl;
        // if (count +1 == 2) return 0;

        system("../bot/randbot < ./runfile/bot0in.json > ./runfile/bot0out.json");
        system("../bot/randbot < ./runfile/bot1in.json > ./runfile/bot1out.json");
        // return 0;
        // cout << "7" << endl;
        system("(./deploy log bot0) < ./runfile/bot0out.json > ./runfile/bot0in.json");
        system("(./deploy log bot1) < ./runfile/bot1out.json > ./runfile/bot1in.json");
        // cout << "8" << endl;

        // return 0;

        // system("(./deploy merge history) > history.json ");
        // system("../judge < ./runfile/log.json > ./runfile/judgeout.json");
        count ++;
        cout << count << endl;
        if (count >=600) break;
        // return 0;

    }

    return 0;
}