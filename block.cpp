#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
using namespace std;

struct Sector{
    char data = 0;
    int valid = 0;  //spare
};

class Block{
    private:
        Sector sector[32] = {0, };    
    public:
        Block(){}
        ~Block(){}
        void Flash_write(int, char);  //기본적인 쓰기
        void Flash_write(int, char, int); //0번 sector에 상태 표기를 위한 쓰기
        void Flash_write_valid(int, int); //0번 sector에 상태 표기를 위한 쓰기
        void Flash_erase();
        char Flash_read(int);
        int Flash_read_valid(int); //sector에 상태 확인을 위한 읽기
};
// 기본적인 쓰기
void Block::Flash_write(int num, char a){
    if(sector[num].data != 0)
        cout << "error" << "\n";
    else
        sector[num].data = a;
}
// 상태 표기를 위한 valid를 사용한 쓰기
void Block::Flash_write(int num, char a,  int valid){
    if(sector[num].data != 0)
        cout << "error" << "\n";
    else{
        sector[num].data = a;
        sector[num].valid = valid;
    }
}
// 상태 표기를 위한 valid를 사용한 쓰기
void Block::Flash_write_valid(int num, int valid){
    sector[num].valid = valid;
}
// 삭제 함수
void Block::Flash_erase(){
    for(int i = 0; i < 32; i++){
        sector[i].data = 0;
    }
    sector[0].valid = 0;
}
// 읽기 함수
char Block::Flash_read(int num){
    return sector[num].data;
}
// 상태를 읽기 위한 함수
int Block::Flash_read_valid(int num){
    return sector[num].valid;
}
// init 함수
int init(int num){
    cout << num <<" megabytes \n";
    return num * 64 + 1; // 1개의 추가 block 생성
}
// LBN 및 offset 생성하기 위한 함수
pair<int, int> convert(int num){
    pair<int, int> div;
    div.first = num / 32; //LBN 생성을 위한 과정
    div.second = num % 32; //offset 생성을 위한 과정
    return div;
}
//mapping table 생성
void Table_init(int Table[], int size){
    for(int i = 0; i < size; i++){
        Table[i] = -1;  // 0 또한 유효 인자이기 때문에 -1로 초기화
    }
}
// write, erase, read가 일어난 횟수를 저장하기 위해 자료형을 int 값 3개 담을 수 있는 것으로 선언
pair<int, pair<int, int>> FTL_write(vector<Block>& memory, int Table[], int num, int num2, char a, int& cnt, int size, bool& toggle){
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    // 모든 block이 유효값을 가지고 있는 경우
    if(toggle || cnt == -1){
        cout << "toggle on\n";
        memory[size - 1].Flash_write(num2, a);
        W_counter++;
        cout << "start to copy to additional block\n";
        //모든 block이 유효값을 갖기 때문에 추가블럭을 사용
        for(int i = 0; i < 32; i++){
            if(num2 == i)
                continue;
            if(memory[Table[num]].Flash_read(i)){
                memory[size - 1].Flash_write(i, memory[Table[num]].Flash_read(i));
                W_counter++;
            } 
            R_counter++;
        }

        memory[Table[num]].Flash_erase();
        E_counter++;
        cout << "start to paste to block\n";
        //추가 블럭에 있던 값들을 다시 PBN에 복사
        for(int i = 0; i < 32; i++){
            if(i == 0){
                // 0 번째는 상태 표기를 위해 따로 작성
                if(memory[size - 1].Flash_read(i)){
                    memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i), 1);
                    W_counter++;
                }
                // 0 번째에 값이 없더라도 상태 표기를 위해 따로 작성
                else{
                    memory[Table[num]].Flash_write_valid(i, 1);
                    W_counter++;
                }
            }
            else{
                if(memory[size - 1].Flash_read(i)){
                    memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i));
                    W_counter++;
                }
            }
            R_counter++;
        }

        memory[size - 1].Flash_erase();
        E_counter++;
        cnt = -1;
    }
    //모든 block이 유효값을 가지지 않은 경우
    else{
        //Table 활성화
        if(Table[num] == -1){
            Table[num] = cnt;
            cnt++;
        }
        //block안에 값이 있을 경우
        if(memory[Table[num]].Flash_read(num2)){
            R_counter++;
            //block의 끝에 도달한 경우
            if(cnt == size - 1){
                int toggle_cnt = 0;
                for(int i = 0; i < size - 1; i++){
                    R_counter++;
                    if(memory[i].Flash_read_valid(0) == 1){
                        toggle_cnt++;
                    }
                }

                if(toggle_cnt == size - 1){
                    toggle = 1;
                    memory[size - 1].Flash_write(num2, a);
                    W_counter++;
                    for(int i = 0; i < 32; i++){
                        if(num2 == i)
                            continue;
                        if(memory[Table[num]].Flash_read(i)){
                            memory[size - 1].Flash_write(i, memory[Table[num]].Flash_read(i));
                            W_counter++;
                        } 
                    R_counter++;
                    }

                    memory[Table[num]].Flash_erase();
                    E_counter++;
                    for(int i = 0; i < 32; i++){
                        if(i == 0){
                            if(memory[size - 1].Flash_read(i)){
                                memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i), 1);
                                W_counter++;
                            }
                            else{
                                memory[Table[num]].Flash_write_valid(i, 1);
                                W_counter++;
                            }
                        }
                        else{
                            if(memory[size - 1].Flash_read(i)){
                                memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i));
                                W_counter++;
                            }
                        }
                        R_counter++;
                    }

                    memory[size - 1].Flash_erase();
                    E_counter++;
                    cnt = -1;
                }
                else{
                    for(int i = 0; i < size - 1; i++){
                        R_counter++;
                        if(memory[i].Flash_read_valid(0) == 2){
                            memory[i].Flash_erase();
                            E_counter++;
                        }
                    }
                cnt = 0;
                }
            }
            //block의 끝이 아닌 경우
            if(cnt != -1){
                int prev = Table[num];
                //비어있는 block 검색
                while(cnt < size - 1){
                    if(memory[cnt].Flash_read_valid(0) == 0){
                        R_counter++;
                        if(num2 == 0){
                            memory[cnt].Flash_write(num2, a, 1);
                            W_counter++;
                        }
                        else{
                            memory[cnt].Flash_write_valid(0, 1);
                            W_counter++;
                            memory[cnt].Flash_write(num2, a);
                            W_counter++;
                        }
                        //비어있던 block에 값 복사
                        for(int i = 0; i < 32; i++){
                            R_counter++;
                            if(num2 == i)
                                continue;
                            else if(memory[prev].Flash_read(i)){
                                memory[cnt].Flash_write(i, memory[prev].Flash_read(i));
                                W_counter++;
                            }
                            else
                                continue;
                        }
                    //원본 block를 유효값이 없다고 표시
                    memory[prev].Flash_write_valid(0, 2);
                    W_counter++;
                    Table[num] = cnt;
                    cnt++;
                    break;
                    }
                    else
                        cnt++;
                }
                //다시 block의 용량 확인
                if(cnt == size - 1){
                    int toggle_cnt = 0;
                    for(int i = 0; i < size - 1; i++){
                        R_counter++;
                        if(memory[i].Flash_read_valid(0) == 1){
                            toggle_cnt++;
                        }
                    }

                    if(toggle_cnt == size - 1){
                        toggle = 1;
                        memory[size - 1].Flash_write(num2, a);
                        W_counter++;
                        for(int i = 0; i < 32; i++){
                            if(num2 == i)
                                continue;
                            if(memory[Table[num]].Flash_read(i)){
                                memory[size - 1].Flash_write(i, memory[Table[num]].Flash_read(i));
                                W_counter++;
                            } 
                        R_counter++;
                        }

                    memory[Table[num]].Flash_erase();
                    E_counter++;
                    for(int i = 0; i < 32; i++){
                        if(i == 0){
                            if(memory[size - 1].Flash_read(i)){
                                memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i), 1);
                                W_counter++;
                            }
                            else{
                                memory[Table[num]].Flash_write_valid(i, 1);
                                W_counter++;
                            }
                        }
                        else{
                            if(memory[size - 1].Flash_read(i)){
                                memory[Table[num]].Flash_write(i, memory[size - 1].Flash_read(i));
                                W_counter++;
                            }
                        }
                        R_counter++;
                    }

                    memory[size - 1].Flash_erase();
                    E_counter++;
                    cnt = -1;
                    }
                    else{
                        for(int i = 0; i < size - 1; i++){
                            R_counter++;
                            if(memory[i].Flash_read_valid(0) == 2){
                                memory[i].Flash_erase();
                                E_counter++;
                            }
                        }
                    cnt = 0;
                    }
                }
            }  
        }
        else{
            //offset이 비어있을 경우
            if(num2 == 0){
                memory[Table[num]].Flash_write(num2, a, 1);
                W_counter++;
            }
            else{
                if(memory[Table[num]].Flash_read_valid(0) == 0){
                    memory[Table[num]].Flash_write_valid(0, 1);
                    W_counter++;
                }
                memory[Table[num]].Flash_write(num2, a);
                W_counter++;
            }
        }
    }
    counter.first = W_counter;
    counter.second.first = E_counter;
    counter.second.second = R_counter;
    return counter;
}

void FTL_read(vector<Block>& memory, int Table[], int num, int num2){
    cout << Table[num] << " " << memory[Table[num]].Flash_read(num2) << "\n";
}

pair<int, pair<int, int>> File_input(vector<Block>& memory, int Table[], int &cnt, int size, bool& toggle){
    ifstream readfromFile;
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    readfromFile.open("Input.txt");
    // 불러올 파일이 있을 시 시행
    if(readfromFile.is_open()){
        while(!readfromFile.eof()){
            char w, a;
            int num;
            int num2, div;
            readfromFile >> w;
            if(w == 'x'){
                break;
            }
            readfromFile >> div;
            readfromFile >> a;

            num = div / 32;
            num2 = div % 32;
            counter = FTL_write(memory, Table, num, num2, a, cnt, size, toggle);
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

void File_output_Table(int Table[], int size){
    ofstream writeFile;
    writeFile.open("output_table.txt");
    writeFile.clear();
    for(int i = 0; i < size; i++){
        writeFile << i;
        writeFile.write(" ", 1);
        writeFile << Table[i];
        writeFile.write("\n", 1);
    }
    
    writeFile.close(); 
}

void File_output_memory(vector<Block>&memory, int size){
    ofstream writeFile;
    writeFile.open("output_memory.txt");
    writeFile.clear();
    for(int i = 0; i < size; i++){
        for(int j = 0; j < 32; j++){
            writeFile << i;
            writeFile.write(" ", 1);
            writeFile << j;
            writeFile.write(" ", 1);
            writeFile << memory[i].Flash_read(j);
            if(j == 0){
                writeFile.write(" ", 1);
                writeFile << memory[i].Flash_read_valid(j);
            }
            writeFile.write("\n", 1);
        }
    }
    
    writeFile.close(); 
}

int main(){
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    char c, p;
    int num, size;
    pair<int, int> div;
    bool toggle = false;
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

    vector<Block> memory(size);
    int *Table = new int[size]; //for test
    Table_init(Table, size);
    counter = File_input(memory, Table, cnt, size, toggle);
    W_counter += counter.first;
    E_counter += counter.second.first;
    R_counter += counter.second.second;
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num;
            if(num >= (size - 1) * 512){
                cout << "out of range\n";
                continue;
            }
            cin >> p;
            div = convert(num);
            counter = FTL_write(memory, Table, div.first, div.second, p, cnt, size, toggle);
            W_counter += counter.first;
            E_counter += counter.second.first;
            R_counter += counter.second.second;
        }

        else if(c == 'e'){
            cin >> num;
            memory[num].Flash_erase();
        }

        else if(c == 'r'){
            cin >> num;
            div = convert(num);
            FTL_read(memory, Table, div.first, div.second);
        }
        //탈출

        else if(c == 'p'){
            cout << W_counter << " " << E_counter << " " << R_counter << "\n";
        }

        else if(c == 'x'){
            File_output(W_counter, E_counter, R_counter);
            File_output_Table(Table, size);
            File_output_memory(memory, size);
            break;
        }

        else{
            continue;
        }
    }
    return 0;
}