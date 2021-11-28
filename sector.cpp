#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
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
        memory->at(num).index = index;
        memory->at(num).vaild = vaild;
    }
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

void Table_init(int Table[], int size){
    for(int i = 0; i < size; i++){
        Table[i] = -1;
    }
}

void Print_table(vector<Sector>* memory, int Table[], int size){
    for(int i = 0; i < size; i++){
        if(Table[i] != -1)
            cout << Table[i] << " " << memory->at(Table[i]).data<< " " <<memory->at(Table[i]).index<< " " << memory->at(Table[i]).vaild << "\n";
        else
            cout << Table[i] << "\n";
    }
}

int init(int num){
    cout << num <<" megabytes \n";
    return num * 2048 + 32;  // 32 is a spare Block
}

pair<int, pair<int, int>> FTL_write(vector<Sector>* memory, int Table[], int num, char a, int &cnt, int size){ 
    int W_counter = 0, E_counter = 0, R_counter = 0;
    pair<int, pair<int, int>> counter;
    if(cnt == size - 32){
        int b_size = 0;
        int i = 0;
        cnt = 0;
        while(b_size < (size / 32) - 1){
            int b_cnt = 0;
            while(b_cnt < 32){
                if(memory->at(i).vaild){
                    Flash_write(memory, (size - 32) + b_cnt, memory->at(i).data, memory->at(i).index, true);
                    W_counter++;
                    R_counter++;
                    b_cnt++;
                    i++;
                }
                else if(i == size - 32)
                    break;
            }
            Flash_erase(memory, b_size);
            b_size++;
            E_counter++;
            int t = 0;
            while(true){         
                if(Flash_read(memory, (size - 32) + t) != 0){
                    Table[memory->at((size - 32) + t).index] = cnt++;
                    Flash_write(memory, Table[memory->at((size - 32) + t).index], memory->at((size - 32) + t).data, memory->at((size - 32) + t).index, true);
                    R_counter++;
                    W_counter++;
                    t++;
                    if(t == 32){ 
                        Flash_erase(memory, (size / 32) - 1);
                        E_counter++;
                        break;
                    }   
                }
                else{
                    Flash_erase(memory, (size / 32) - 1);
                    E_counter++;
                    cout << "E\n";
                    break;
                }
            }
        }
    }
    else{
        if(Table[num] == -1){
            Table[num] = cnt;
            cnt++;
            Flash_write(memory, Table[num], a, num, true);
            W_counter++;
        }
        else if(memory->at(Table[num]).data != 0 && num == memory->at(Table[num]).index){
            memory->at(Table[num]).vaild = false;
            Table[num] = cnt;
            cnt++;
            Flash_write(memory, Table[num], a, num, true);
            R_counter++;
            W_counter++;
        }
        else{
            cout << "error\n";
        }
    }
    counter.first = W_counter;
    counter.second.first = E_counter;
    counter.second.second = R_counter;
    return counter;
}

int FTL_read(vector<Sector>* memory, int Table[], int num){ 
    cout << Table[num] << " " << Flash_read(memory, Table[num]) << "\n";
    return 1;
}

void Print_counter(int &W_couter, int &E_couter, int &R_couter){
    cout << "write : " << W_couter << " erase : " << E_couter << " read : " << R_couter << "\n";
}

void Print_Flash(vector<Sector>* memory, int size){
    for(int i = 0; i < size; i++){
        cout << memory->at(i).data << memory->at(i).vaild << memory->at(i).index << "\n";
    }
}
    // file input, output 함수 추가 구현 필요
pair<int, pair<int, int>> File_input(vector<Sector>* memory, int Table[], int &cnt, int size){
    ifstream readfromFile;
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    readfromFile.open("input.txt");
    // 불러올 파일이 있을 시 시행
    if(readfromFile.is_open()){
        while(!readfromFile.eof()){
            char w, a;
            int num;
            readfromFile >> w;
            readfromFile >> num;
            readfromFile >> a;

            counter = FTL_write(memory, Table, num, a, cnt, size);
            W_counter += counter.first;
            E_counter += counter.second.first;
            R_counter += counter.second.second;
        }
        readfromFile.close();
    }
    counter.first = W_counter;
    counter.second.first = E_counter;
    counter.second.second += R_counter;
    return counter;
}

int main(){
    char c , p;
    int num, size;
    int cnt = 0;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    pair<int, pair<int, int>> counter;
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
    Table_init(Table, size);     // -1은 비어있는 것으로 취급
    counter = File_input(&memory, Table, cnt, size);
    W_counter += counter.first;
    E_counter += counter.second.first;
    R_counter += counter.second.second;
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num >> p;
            if(num >= size - 32){ // spare block의 크기 만큼 빼야 됨
                cout << "out of range\n";
                continue;
            }
            counter = FTL_write(&memory, Table, num, p, cnt, size);  //cnt 인자를 reference 형태로 넘겨줘야 됨
            W_counter += counter.first;
            E_counter += counter.second.first;
            R_counter += counter.second.second;
        }

        else if(c == 'e'){
            cin >> num;
            Flash_erase(&memory, num);
        }

        else if(c == 'r'){
            cin >> num;
            if(num >= size - 32){
                cout << "out of range\n";
                continue;
            }
            R_counter += FTL_read(&memory, Table, num);
        }

        else if(c == 'p'){
            Print_table(&memory, Table, size);
        }

        else if(c == 'f'){
            Print_Flash(&memory, size);
        }
        //탈출
        else if(c == 'x'){
            cout << W_counter << " " << E_counter << " " << R_counter << "\n";
            break;
        }

        else{
            continue;
        }
    }
    return 0;
}