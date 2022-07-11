#include <iostream>
#include <string>

#include "../jsoncpp/json.h"
Json::Reader reader;
Json::Value input,output;

using namespace std;

void begin_game() {
    cout << "{\"log\":[],\"initdata\":\"\"}" << endl;
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

    return 0;

}