#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct Sector{
    char data = 0;
    char spare[16];  //spare
};

class Block{
    private:
        Sector sector[32] = {0, }; 
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
    for(int i = 0; i < 32; i++){
        sector[i].data = 0;
    }
}

char Block::Flash_read(int num){
    return sector[num].data;
}


int init(int num){
    cout << num <<" megabytes \n";
    return num * 64 + 1; // 추가 block 생성
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
            int num;
            cin >> num;
            size = init(num);
            break;
        }
        else{
            cout << "please init a repository \n";
            continue;
        }
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
            cin >> num;
            memory[num].Flash_erase();
        }

        else if(c == 'r'){
            cin >> num;
            div = convert(num);
            cout << memory[div.first].Flash_read(div.second) << "\n";
        }
        //탈출
        else if(c == 'x'){
            break;
        }

        else{
            continue;
        }
    }
    return 0;
}