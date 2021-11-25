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
    else
        memory->at(num).data = a;
        memory->at(num).index = num;
        memory->at(num).vaild = vaild;
}

void Flash_erase(vector<Sector>* memory, int num){
    num = num * 32;
    for(int i = 0; i < 32; i++){
        memory->at(num + i).data = 0;
        memory->at(num).index = 0;
        memory->at(num).vaild = false;
    }
}

char Flash_read(vector<Sector>* memory, int num){
    return memory->at(num).data;
}


int init(int num){
    cout << num <<" megabytes \n";
    return num * 2048 + 32;  // spare Block
}

void FTL_write(vector<Sector>* memory, int *Table, int num, char a, int cnt, int size){
    if(cnt == size - 1){
        int b_size = 0;
        int i = 0;
        cnt = 0;
        while(b_size < size / 32){
            int b_cnt = 0;
            while(b_cnt < 32 && i != size - 1){
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
                    Flash_erase(memory, (size / 32));
                    break;
                }
            }
        }
    }
    else{
       if(Flash_read(memory, Table[num])){
            memory->at(Table[num]).vaild = false;
            Table[num] = cnt++;
            Flash_write(memory, Table[num], a, num, true);
        }
       else{
            Table[num] = cnt++;
            Flash_write(memory, Table[num], a, num, true);
        }
    }
}

void FTL_read(vector<Sector>* memory, int Table[], int num){
    Flash_read(memory, Table[num]);
}

int main(){
    char c;
    int num;
    char p;
    int size;
    int cnt = 0;
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

    vector<Sector> memory(size);
    int *Table = new int[size];
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num >> p;
            FTL_write(&memory, Table, num, p, cnt, size);
        }

        else if(c == 'e'){
            cin >> num;
            Flash_erase(&memory, num);
        }

        else if(c == 'r'){
            cin >> num;
            cout << FTL_read << "\n";
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