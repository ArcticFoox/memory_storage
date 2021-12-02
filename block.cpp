#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct Sector{
    char data = 0;
    int vaild = 0;  //spare
};

class Block{
    private:
        //Sector sector[32] = {0, };
        Sector sector[4] = {0, }; //for test
    public:
        Block(){}
        ~Block(){}
        void Flash_write(int, char);
        void Flash_write(int, char, int);
        void Flash_write(int, int);
        void Flash_erase();
        char Flash_read(int);
        int Flash_read_vaild(int);
};

void Block::Flash_write(int num, char a){
    if(sector[num].data != 0)
        cout << "error" << "\n";
    else
        sector[num].data = a;
}

void Block::Flash_write(int num, char a,  int vaild){
    if(sector[num].data != 0)
        cout << "error" << "\n";
    else{
        sector[num].data = a;
        sector[num].vaild = vaild;
    }
}

void Block::Flash_write(int num, int vaild){
    sector[num].vaild = vaild;
}

void Block::Flash_erase(){
    //for(int i = 0; i < 32; i++){
    for(int i = 0; i < 4; i++){ //for test
        sector[i].data = 0;
    }
    sector[0].vaild = 0;
}

char Block::Flash_read(int num){
    return sector[num].data;
}

int Block::Flash_read_vaild(int num){
    return sector[num].vaild;
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

void FTL_write(vector<Block>& memory, int Table[], int num, int num2, char a, int& cnt, int size){
    int s_cnt = 0;
    if(Table[num] == -1){
        Table[num] = cnt;
        cnt++;
        if(!memory[Table[num]].Flash_read_vaild(0)){
            if(num2 = 0)
                memory[Table[num]].Flash_write(num2, a, 1);
            else{
                memory[Table[num]].Flash_write(num2, 0, 1);
                memory[Table[num]].Flash_write(num2, a);
            }
        }
        else
            memory[Table[num]].Flash_write(num2, a);
    }

    else{
        if(memory[Table[num]].Flash_read(num2)){
            if(cnt < size){
                int prev = Table[num];
                Table[num] = cnt;
                cnt++;
                memory[Table[num]].Flash_write(num2, a);
                for(int i = 0; i < 4; i++){
                    if(num2 == i)
                        continue;
                    else if(memory[prev].Flash_read(i)){
                        memory[Table[num]].Flash_write(i, memory[prev].Flash_read(i));
                    }
                    else
                        continue;
               }
            }

            else if(cnt == size - 1){
                int toggle_cnt = 0;
                for(int i = 0; i < size; i++){
                    if(memory[i].Flash_read_vaild(0) == 1){
                        toggle_cnt++;
                    }
                }
            }
        }
        else{
            if(!memory[Table[num]].Flash_read_vaild(0)){
                if(num2 = 0)
                    memory[Table[num]].Flash_write(num2, a, 1);
                else{
                    memory[Table[num]].Flash_write(num2, 0, 1);
                    memory[Table[num]].Flash_write(num2, a);
                }
            }
            else
                memory[Table[num]].Flash_write(num2, a);
            }
    }
}

int FTL_read(vector<Block>& memory, int Table[], int num){
    
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