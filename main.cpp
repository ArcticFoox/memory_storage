#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct Sector{
    char data = 0;
    char spare[16];
};

class Block{
    private:
        Sector sector[32] = {0, }; // test시 3으로 지정(32b를 기본으로 함)
    public:
        Block(){}
        ~Block(){}
        void Flash_write(int, char);
        void Flash_erase();
        char Flash_read(int);
};

void Block::Flash_write(int num, char a){
    if(sector[num].data != 0)
        cout << "error" << "\n";
    else
        sector[num].data = a;
}

void Block::Flash_erase(){
    for(int i = 0; i < 3; i++){
        sector[i].data = 0;
    }
}

char Block::Flash_read(int num){
    return sector[num].data;
}


int init(){
    int num;
    cout << "enter the capacity" << "\n";
    cin >> num;
    return num * 64;
}

pair<int, int> convert(int num){
    pair<int, int> div;
    div.first = num / 32;
    div.second = num % 32;
    return div;
}

int main(){
    char c;
    int num;
    char p;
    int size;
    pair<int, int> div;
    while(true){
        string input;
        cin >> input;
        if(input == "init"){
            size = init();
            break;
        }
        else
            continue;
    }

    vector<Block> memory(size);
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num >> p;
            div = convert(num);
            memory[div.first].Flash_write(div.second, p);
        }

        else if(c == 'e'){
            cin >> num >> p;
            div = convert(num);
            memory[num].Flash_erase();
        }

        else if(c == 'r'){
            cin >> num ;
            div = convert(num);
            cout << memory[div.first].Flash_read(div.second);
        }

        else if(c == 'x'){
            break;
        }

        else if(c == 'a'){
            for(int i = 0; i < size; i++){
                for(int j = 0; j < 32; j++){
                    if(memory[i].Flash_read(j) == 0){
                        cout << '0' << " ";
                    }
                    else
                        cout << memory[i].Flash_read(j) << " ";
                }
                cout << "\n";
            }
        }
        else{
            continue;
        }
        cout << "\n";
    }
    return 0;
}