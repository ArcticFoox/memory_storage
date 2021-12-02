#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
using namespace std;

struct Sector{
    char data = 0;
    int vaild = 0;  //spare
};

class Block{
    private:
        //Sector sector[32] = {0, };
        Sector sector[32] = {0, }; //for test
    public:
        Block(){}
        ~Block(){}
        void Flash_write(int, char);
        void Flash_write(int, char, int);
        void Flash_write_vaild(int, int);
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

void Block::Flash_write_vaild(int num, int vaild){
    sector[num].vaild = vaild;
}

void Block::Flash_erase(){
    //for(int i = 0; i < 32; i++){
    for(int i = 0; i < 32; i++){ //for test
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
    return num * 64 + 1; //for test
}

pair<int, int> convert(int num){
    pair<int, int> div;
    div.first = num / 32;    //for test
    div.second = num % 32;   //for test
    return div;
}

void Table_init(int Table[], int size){
    for(int i = 0; i < size; i++){
        Table[i] = -1;
    }
}

pair<int, pair<int, int>> FTL_write(vector<Block>& memory, int Table[], int num, int num2, char a, int& cnt, int size, bool& toggle){
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    if(toggle){
        if(num2 == 0){
            memory[size].Flash_write(num2, a, 1);
            W_counter++;
        }
        else{
            memory[size].Flash_write_vaild(0, 1);
            memory[size].Flash_write(num2, a);
            W_counter += 2;
        }
        for(int i = 0; i < 32; i++){
            R_counter++;
            if(num2 == i)
                continue;
            else if(memory[Table[num]].Flash_read(i)){
                memory[size].Flash_write(i, memory[Table[num]].Flash_read(i));
                W_counter++;
            }
            else
                continue;
        }

        memory[Table[num]].Flash_erase();
        E_counter++;

        for(int i = 0; i < 32; i++){
            R_counter++;
            if(i == 0){
                if(memory[size].Flash_read(i)){
                    memory[Table[num]].Flash_write(i, memory[size].Flash_read(i), 1);
                    W_counter++;
                }
                else{
                    memory[Table[num]].Flash_write_vaild(i, 1);
                    W_counter++;
                }
            }
            else{
                if(memory[size].Flash_read(i)){
                    memory[Table[num]].Flash_write(i, memory[size].Flash_read(i));
                    W_counter++;
                }
                else
                    continue;
            }
        }
    }
    else{
        if(Table[num] == -1){
            Table[num] = cnt;
            cnt++;
            if(!memory[Table[num]].Flash_read_vaild(0)){
                R_counter++;
                if(num2 == 0){
                    memory[Table[num]].Flash_write(num2, a, 1);
                    W_counter++;
                }
                else{
                    memory[Table[num]].Flash_write_vaild(0, 1);
                    memory[Table[num]].Flash_write(num2, a);
                    W_counter += 2;
                }
            }
            else{
                memory[Table[num]].Flash_write(num2, a);
                W_counter++;
            }
        }

        else{
            if(memory[Table[num]].Flash_read(num2)){
                R_counter++;
                if(cnt < size){
                    int prev = Table[num];
                    if(memory[cnt].Flash_read_vaild(0) == 0){
                        R_counter++;
                        if(num2 == 0){
                            memory[cnt].Flash_write(num2, a, 1);
                            W_counter++;
                        }
                        else{
                            memory[cnt].Flash_write_vaild(0, 1);
                            memory[cnt].Flash_write(num2, a);
                            W_counter += 2;
                        }
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
                    memory[prev].Flash_write_vaild(0, 2);
                    W_counter++;
                    Table[num] = cnt;
                    }
                    else
                        cnt++;
                }

                else if(cnt == size - 1){
                    int toggle_cnt = 0;
                    for(int i = 0; i < size; i++){
                        R_counter++;
                        if(memory[i].Flash_read_vaild(0) == 1){
                            toggle_cnt++;
                        }
                    }

                    if(toggle_cnt == size - 1){
                        toggle = 1;
                    }
                    else{
                        for(int i = 0; i < size; i++){
                            R_counter++;
                            if(memory[i].Flash_read_vaild(0) == 2){
                                memory[i].Flash_erase();
                            }
                        }
                    }
                }
            }
            else{
                if(!memory[Table[num]].Flash_read_vaild(0)){
                    R_counter++;
                    if(num2 = 0){
                        memory[Table[num]].Flash_write(num2, a, 1);
                        W_counter++;
                    }
                    else{
                        memory[Table[num]].Flash_write_vaild(0, 1);
                        memory[Table[num]].Flash_write(num2, a);
                        W_counter += 2;
                    }
                }
                else
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
    cout << memory[Table[num]].Flash_read(num2) << "\n";
}

pair<int, pair<int, int>> File_input(vector<Block>& memory, int Table[], int &cnt, int size, bool& toggle){
    ifstream readfromFile;
    pair<int, pair<int, int>> counter;
    int W_counter = 0, E_counter = 0, R_counter = 0;
    readfromFile.open("input.txt");
    // 불러올 파일이 있을 시 시행
    if(readfromFile.is_open()){
        while(!readfromFile.eof()){
            char w, a;
            int num;
            int num2, div;
            readfromFile >> w;
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
            cin >> num >> p;
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
            break;
        }

        else{
            continue;
        }
    }
    return 0;
}