#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
using namespace std;

struct Sector{
    char data = 0;
    int index = -1;  //spare
    bool valid = false; //spare
};
//기본적인 쓰기
void Flash_write(vector<Sector>* memory, int num, char a){
    if(memory->at(num).data != 0)
        cout << "error" << "\n";
    else
        memory->at(num).data = a;
}
//유효 판단 및 맵핑 전 값을 쓰는 함수
void Flash_write(vector<Sector>* memory, int num, char a, int index, bool valid){
    if(memory->at(num).data != 0)
        cout << "error" << "\n";
    else{
        memory->at(num).data = a;
        memory->at(num).index = index;
        memory->at(num).valid = valid;
    }
}
//지우기 함수
void Flash_erase(vector<Sector>* memory, int num){
    num = num * 32;
    for(int i = 0; i < 32; i++){
        memory->at(num + i).data = 0;
        memory->at(num).index = -1;
        memory->at(num).valid = false;
    }
}
//읽기 함수
char Flash_read(vector<Sector>* memory, int num){
    return memory->at(num).data;
}
//맵핑 테이블 활성화 함수
void Table_init(int Table[], int size){
    for(int i = 0; i < size - 32; i++){
        Table[i] = -1;
    }
}
//테이블 출력 함수
void Print_table(vector<Sector>* memory, int Table[], int size){
    for(int i = 0; i < size; i++){
        if(Table[i] != -1){
            cout << Table[i] << " " << memory->at(Table[i]).data<< " ";
            cout << memory->at(Table[i]).index<< " " << memory->at(Table[i]).valid << "\n";
        }
        else
            cout << Table[i] << "\n";
    }
}
// 메모리 활성화 함수
int init(int num){
    cout << num <<" megabytes \n";
    return num * 2048 + 32;  // 32 is a spare Block
}
//FTL_write 함수
pair<int, pair<int, int>> FTL_write(vector<Sector>* memory, int Table[], int num, char a, int &cnt, int size){ 
    int W_counter = 0, E_counter = 0, R_counter = 0;
    pair<int, pair<int, int>> counter;
    //sector의 마지막일 경우
    if(cnt == size - 32){
        int b_size = 0;
        int i = 0;
        cnt = 0;
        //block으로 표현하기 위한 b_size
        while(b_size < (size / 32) - 1){
            int b_cnt = 0;
            //추가 블럭이 다 찰때까지 추가블록에 복사
            int valid_count = 0;
            while(b_cnt < 32){
                R_counter++;
                if(memory->at(i).valid){
                    Flash_write(memory, (size - 32) + valid_count, memory->at(i).data, memory->at(i).index, true);
                    W_counter++;
                    b_cnt++;
                    i++;
                    valid_count++;
                }
                else{
                    b_cnt++;
                    i++;
                    continue;
                }

                if(i == size - 32)
                    break;
            }
            //위에서부터 지우기 연산 시행
            Flash_erase(memory, b_size);
            b_size++;
            E_counter++;
            valid_count = 0;
            int t = 0;
            if(Flash_read(memory, (size - 32)) == 0){
                continue;
            }
            //추가블럭에 있던 값들 다시 sector 처음부터 붙여놓기 시작
            while(true){         
                if(Flash_read(memory, (size - 32) + t) != 0){
                    Table[memory->at((size - 32) + t).index] = cnt;
                    cnt++;
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
                    break;
                }
            }
        }
    }
    
    //테이블의 값이 정해지지 않았을 경우
    if(Table[num] == -1){
        Table[num] = cnt;
        cnt++;
        Flash_write(memory, Table[num], a, num, true);
        W_counter++;
    }
    //테이블에 값이 있는 경우
    else if(memory->at(Table[num]).data != 0){
        memory->at(Table[num]).valid = false;
        Table[num] = cnt;
        cnt++;
        Flash_write(memory, Table[num], a, num, true);
        R_counter++;
        W_counter++;
    }
    else{
        cout << "error\n";
        
    
    }
    counter.first = W_counter;
    counter.second.first = E_counter;
    counter.second.second = R_counter;
    return counter;
}
//FTL_read 함수
int FTL_read(vector<Sector>* memory, int Table[], int num){ 
    cout << Table[num] << " " << Flash_read(memory, Table[num]) << "\n";
    return 1;
}
// file input 함수
pair<int, pair<int, int>> File_input(vector<Sector>* memory, int Table[], int &cnt, int size){
    ifstream readfromFile;
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    readfromFile.open("Input.txt");
    // 불러올 파일이 있을 시 시행
    if(readfromFile.is_open()){
        while(!readfromFile.eof()){
            char w, a;
            int num;
            readfromFile >> w;
            if(w == 'x'){
                break;
            }
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
// file output 함수
void File_output(int W_counter, int E_counter, int R_counter){
    ofstream writeFile;
    writeFile.open("output.txt");
    writeFile.clear();
    writeFile << W_counter;
    writeFile.write(" ", 1);
    writeFile << E_counter;
    writeFile.write(" ", 1);
    writeFile << R_counter;
    writeFile.close(); 
}
// table output 함수
void File_output_table(int Table[], int size){
    ofstream writeFile;
    writeFile.open("output_table.txt");
    writeFile.clear();
    for(int i = 0; i < size - 32; i++){
        writeFile << i;
        writeFile.write(" ", 1);
        writeFile << Table[i];
        writeFile.write("\n", 1);
    }
    writeFile.close(); 
}
// memory output 함수
void File_output_memory(vector<Sector>&memory, int size){
    ofstream writeFile;
    writeFile.open("output_memory.txt");
    writeFile.clear();
    for(int i = 0; i < size - 32; i++){
        writeFile << i;
        writeFile.write(" ", 1);
        writeFile << memory[i].data;
        writeFile.write(" ", 1);
        writeFile << memory[i].index;
        writeFile.write(" ", 1);
        writeFile << memory[i].valid;
        writeFile.write("\n", 1);
    }
    writeFile.close(); 
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
    //파일이 있으면 파일 input 시행
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
        //삭제 함수
        else if(c == 'e'){
            cin >> num;
            Flash_erase(&memory, num);
        }
        //읽기
        else if(c == 'r'){
            cin >> num;
            if(num >= size - 32){
                cout << "out of range\n";
                continue;
            }
            R_counter += FTL_read(&memory, Table, num);
        }
        //테이블 출력
        else if(c == 'p'){
            Print_table(&memory, Table, size);
        }
        //탈출
        else if(c == 'x'){
            File_output(W_counter, E_counter, R_counter);
            File_output_table(Table, size);
            File_output_memory(memory, size);
            cout << cnt;
            break;
        }

        else{
            continue;
        }
    }
    return 0;
}