#include <iostream>
#include <string>
#include <fstream>

#include "../jsoncpp/json.h"

using namespace std;

Json::Reader reader;
Json::Value input,output,logg;

ifstream ifs;
ofstream ofs;

void begin_game() {
    cout << "{\"log\":[],\"num\":0,\"initdata\":\"\"}" << endl;
    // ofs.open("./runfile/log.json");
    // ofs << "{\"num\":0,\"log\":[]}";
    // ofs.close();
}
void userin(int user) {
    string st="1";
    if (user == 0) st = "0";
    output["map"] = input["content"][st]["map"];
    output["size"] = input["content"][st]["size"];
    Json::FastWriter writer;
	cout << writer.write(output) << endl;
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
    logg["log"][lengt] = input["content"];
    logg["num"] = logg["num"].asInt() + 1;
    ofs.open("./runfile/log.json");
    Json::FastWriter writer;
	ofs << writer.write(logg);
    ofs.close();
}

void logbot(int user) {
    Json::FastWriter writer;

    // string st = "0";
    // if (user == 1) st = "1";
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
    }
    else 
        logg["log"][lengt-1]["1"]["response"] = input;
    
	ofs << writer.write(logg) ;
    ofs.close();

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