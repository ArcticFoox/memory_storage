#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct Sector{
    char data = 0;
    char spare[16];  //spare
};

void Flash_write(vector<Sector>* memory, int num, char a){
    if(memory->at(num).data != 0)
        cout << "error" << "\n";
    else
        memory->at(num).data = a;
}

void Flash_erase(vector<Sector>* memory, int num){
    num = num * 32;
    for(int i = 0; i < 32; i++){
        memory->at(num + i).data = 0;
    }
}

char Flash_read(vector<Sector>* memory, int num){
    return memory->at(num).data;
}


int init(int num){
    cout << num <<" megabytes \n";
    return num * 2048;
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

    vector<Sector> memory(size);
    while(true){
        cin >> c;
        if(c == 'w'){
            cin >> num >> p;
            Flash_write(&memory, num, p);
        }

        else if(c == 'e'){
            cin >> num;
            Flash_erase(&memory, num);
        }

        else if(c == 'r'){
            cin >> num;
            cout << Flash_read(&memory, num) << "\n";
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