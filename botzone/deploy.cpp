#include <iostream>
#include <string>

#include "../jsoncpp/json.h"

using namespace std;

void begin_game() {
    cout << "{\"log\":[],\"initdata\":\"\"}" << endl;
}
char beginn[] = "begin";



int main(int args, char * argv[]) {
    if (args != 3) { 
        cout << "error args num" << endl;
        return 0;   
    }
    if (strcmp(argv[1], beginn) == 0) {
        begin_game();
        return 0;
    } 
    string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,output;
	reader.parse(str, input);
    return 0;

}