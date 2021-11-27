#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct Sector{
    char data = 0;
    int index = 0;  //spare
    bool vaild = false; //spare
};

void Flash_write(vector<Sector>* memory, int num, char a){
    if(memory->at(num).data != 0)
        cout << "error" << "\n";
    else
        memory->at(num).data = a;
}

void Flash_write(vector<Sector>* memory, int num, char a, int index, bool vaild){
    if(memory->at(num).data != 0)
        cout << "error" << "\n";
    else{
        memory->at(num).data = a;
        memory->at(num).index = num;
        memory->at(num).vaild = vaild;
    }
}

void Flash_erase(vector<Sector>* memory, int num){
    //num = num * 32;
    num = num * 4; //for test
    for(int i = 0; i < 32; i++){
        memory->at(num + i).data = 0;
        memory->at(num).index = 0;
        memory->at(num).vaild = false;
    }
}

char Flash_read(vector<Sector>* memory, int num){
    return memory->at(num).data;
}

void Table_init(int Table[], int size){
    for(int i = 0; i < size; i++){
        Table[i] = 0;
    }
}

void Print_table(int Table[], int size){
    for(int i = 0; i < size; i++){
        cout << Table[i] << "\n";
    }
}

int init(int num){
    cout << num <<" megabytes \n";
    //return num * 2048 + 32;  // 32 is spare Block
    return num * 32 + 4; // for test
}

void FTL_write(vector<Sector>* memory, int Table[], int num, char a, int &cnt, int size){  // 일어난 횟수 셀 수 있도록 수정해야함
    if(cnt == size){
        int b_size = 0;
        int i = 0;
        cnt = 0;
        // while(b_size < size / 32){ 
        while(b_size < size / 4){ // for test
            int b_cnt = 0;
            //while(b_cnt < 32 && i != size - 1){
            while(b_cnt < 32 && i != size){ //for test
                if(memory->at(i).vaild){
                    Flash_write(memory, size + b_cnt, memory->at(i).data, memory->at(i).index, true);
                    b_cnt++;
                    i++;
                }
            }
            Flash_erase(memory, b_size++);
            while(true){
                int t = 0;
                if(Flash_read(memory, size + t)){
                    Table[memory->at(size + t).index] = cnt++;
                    Flash_write(memory, Table[num], memory->at(size + t).data, memory->at(size + t).index, true);
                    t++;
                }
                else{
                    //Flash_erase(memory, (size / 32));
                    Flash_erase(memory, (size / 4)); //for test
                    break;
                }
            }
        }
    }
    else{  //1에는 접근이 안됨
        if(cnt == 0){
            Table[num] = cnt;
            cnt++;
            Flash_write(memory, Table[num], a, num, true);
        }
        if(memory->at(Table[num]).data != 0 && num == memory->at(Table[num]).index){
            memory->at(Table[num]).vaild = false;
            Table[num] = cnt;
            cnt++;
            Flash_write(memory, Table[num], a, num, true);
        }
        else{
            Table[num] = cnt;
            cnt++;
            Flash_write(memory, Table[num], a, num, true);
        }
    }
}

void FTL_read(vector<Sector>* memory, int Table[], int num){ 
    cout << Flash_read(memory, Table[num]) << "\n";
}

int main(){
    char c;
    int num;
    char p;
    int size;
    int cnt = 0;
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

    vector<Sector> memory(size);
    int *Table = new int[size];  // 몇개의 값은 0이 아닌 쓰레기 값이 저장됨
    Table_init(Table, size);     // 0은 비어있는 것으로 취급
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num >> p;
            FTL_write(&memory, Table, num, p, cnt, size);  //cnt 인자를 reference 형태로 넘겨줘야 됨
        }

        else if(c == 'e'){
            cin >> num;
            Flash_erase(&memory, num);
        }

        else if(c == 'r'){
            cin >> num;
            FTL_read(&memory, Table, num);
        }

        else if(c == 'p'){
            Print_table(Table, size);
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