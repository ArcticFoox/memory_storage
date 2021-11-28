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
        //Sector sector[32] = {0, };
        Sector sector[4] = {0, }; //for test
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
    //for(int i = 0; i < 32; i++){
    for(int i = 0; i < 4; i++){ //for test
        sector[i].data = 0;
    }
}

char Block::Flash_read(int num){
    return sector[num].data;
}


int init(int num){
    cout << num <<" megabytes \n";
    //return num * 64 + 1; // 추가 block 생성
    return num * 8 + 1; //for test
}

pair<int, int> convert(int num){
    pair<int, int> div;
    div.first = num / 4;    //for test
    div.second = num % 4;   //for test
    return div;
}

void Table_init(int Table[], int size){
    for(int i = 0; i < size; i++){
        Table[i] = -1;
    }
}

void FTL_write(vector<Block>& memory, int Table[], int num, int num2, char a, int vaild){
    int cnt = 0;
    int s_cnt = 0;
    if(Table[num] == -1){
        Table[num] = cnt;
        cnt++;
        memory[Table[num]].Flash_write(0, a);
    }

    else if(Table[num] != -1){

    }
}

void FTL_read(){

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
    int *Table = new int[size]; //for test
    Table_init(Table, size);
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