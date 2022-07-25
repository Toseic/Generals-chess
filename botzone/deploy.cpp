#include <iostream>
#include <string>
#include <fstream>

#include "../jsoncpp/json.h"
#define foi(n) for (int i = 0; i < n; ++i)
#define foj(n) for (int j = 0; j < n; ++j)
#define fok(n) for (int k = 0; k < n; ++k)
#define fouser for (int user = 0; user < 2; ++user)

using namespace std;

Json::Reader reader;
Json::Value input,output,logg;
Json::FastWriter writer;
ifstream ifs;
ofstream ofs;

void begin_game() {
    cout << "{\"log\":[],\"num\":0,\"initdata\":\"\"}" << endl;
    // ofs.open("./runfile/log.json");
    // ofs << "{\"num\":0,\"log\":[]}";
    // ofs.close();
}
void userin(int user) {
    string path="./runfile/bot0in.json",st="1";
    if (user == 0) {st = "0"; path = "./runfile/bot1in.json";} 
    
    ifs.open(path,ios_base::in);
    if (!ifs.is_open()) {cout << "error opening"; return;}
    string st1;
    ifs >> st1;
    reader.parse(st1, logg);
    int requestNum,responseNum;
    requestNum = logg["request"].size();
    responseNum = logg["response"].size();
    logg["request"][requestNum] = input["content"][st];
    
	cout << writer.write(logg) << endl;
}
void userout(int user) {
    cout << "userin " << user << endl;
}

void logjudge() {
    ifs.open("./runfile/log.json",ios_base::in);
    if (!ifs.is_open()) {cout << "error opening"; return;}
    string st1;
    ifs >> st1;
    reader.parse(st1, logg);
    
    int lengt = logg["num"].asInt();
    // cout << lengt << endl;
    logg["log"][lengt]["output"]["content"] = input["content"];
    logg["num"] = logg["num"].asInt() + 1;
    ofs.open("./runfile/log.json");

	ofs << writer.write(logg);
    ofs.close();
}

void logbot(int user) { 
    string path="./runfile/bot0in.json",st="1";
    if (user == 0) {st = "0"; path = "./runfile/bot1in.json";} 

    ifs.open("./runfile/log.json",ios_base::in);
    if (!ifs.is_open()) {cout << "error opening"; return;}
    string st1;
    ifs >> st1;
    ifs.close();
    ofs.open("./runfile/log.json");
    reader.parse(st1, logg);
    int lengt = logg["num"].asInt();
    if (user == 0) {
        logg["log"][lengt]["0"]["response"] = input;
        logg["num"] = logg["num"].asInt() + 1;        
    } else 
        logg["log"][lengt-1]["1"]["response"] = input;
	ofs << writer.write(logg) ;
    ofs.close();

    ifs.open(path,ios_base::in);
    if (!ifs.is_open()) {cout << "error opening"; return;}
    ifs >> st1; reader.parse(st1, logg);
    ifs.close();
    int responseNum = logg["response"].size();
    logg["response"][responseNum] = input;
    cout << writer.write(logg) << endl;
}

void loginitdata() {
    Json::FastWriter writer;
    ifs.open("./runfile/log.json",ios_base::in);
    if (!ifs.is_open()) {cout << "error opening"; return;}
    string st1;
    ifs >> st1;
    ifs.close();
    ofs.open("./runfile/log.json");
    reader.parse(st1, logg);
    int lengt = logg["num"].asInt();

    logg["initdata"] = input["initdata"];
    
	ofs << writer.write(logg) ;
    ofs.close();
}

void statecheck() {
    if (input["command"] == "request") {
        cout << "open";
    } else if (input["command"] == "finish") {
        cout << "finish";
    } else {
        cout << "error";
    }
}






int main(int args, char * argv[]) {
    string str;
    if (args != 3) { 
        cout << "error args num" << endl;
        return 0;   
    }
    if (strcmp(argv[1], "begin") == 0) {
        begin_game();
        return 0;
    } 
    
	getline(cin, str);
	reader.parse(str, input);

    if (strcmp(argv[1],"bot0") == 0) {
        if (strcmp(argv[2],"in") == 0) {
            userin(0);
        } else {
            userout(0);
        }
        return 0;
    } 
    if (strcmp(argv[1],"bot1") == 0) {
        if (strcmp(argv[2],"in") == 0) {
            userin(1);
        } else {
            userout(1);
        }
        return 0;
    } 

    if (strcmp(argv[1],"log") == 0) {
        if (strcmp(argv[2],"judge") == 0) {
            logjudge();
        } else if (strcmp(argv[2],"bot0") == 0){
            logbot(0);
        } else if (strcmp(argv[2],"bot1") == 0) {
            logbot(1);
        } else if (strcmp(argv[2],"initdata") == 0) {
            loginitdata();
        } else if (strcmp(argv[2],"state") == 0) {
            statecheck();
        }
        return 0;
    } 

    return 0;

}