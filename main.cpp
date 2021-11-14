#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

class Block{
    private:
        Sector sto[3]; // test시 3으로 지정(32b를 기본으로 함)
    public:
        void sector_write(int, char);
        void sector_erase(int, char);
        void sector_read(int);
};

struct Sector{
    char data;
    char spare[1]; //test시 1로 지정 (16b를 기본으로 함)
};

void Block::sector_write(int num, char a){
    if(sto[num].data != 0)
        cout << "error" << "\n";
    else
        sto[num].data = a;
}

void Block::sector_erase(int num, char a){
    Sector tmp[3];
    for(int i = 0; i < 3; i++){
        if(num == i){
            tmp[i].data = 0;
        }
        else{
            tmp[i].data = sto[i].data;
        }
    }

    for(int i = 0; i < 3; i++){
        sto[i].data = tmp[i].data;
    }
}

void Block::sector_read(int num){
    cout << sto[num].data;
}


void init(vector<Block> *api, int n){
    api->reserve(n);
}

void Block_write(vector<Block> *api){
    char c, m;
    int num;
    cin >> num >> m;
    int quotient = num / 3;
    int rest = num % 3;
    
    api[quotient][1].sector_write(rest, m);
}

void Block_erase(vector<Block> *api){
    char c, m;
    int num;
    cin >> num >> m;
    int quotient = num / 3;
    int rest = num % 3;
    
    api[quotient][1].sector_erase(rest, m);
}

int main(){
    vector<Block> api(3);
    char c;
    int num;
    cout << "enter the capacity" << "\n";
    cin >> num;
    init(&api, num);

    while(true){
        cin >> c;
        if(c == 'w'){
           Block_write(&api);
        }

        else if(c == 'e'){
           Block_erase(&api);
        }

        else if(c == 'r'){
            cin >> num >> m;
            quotient = num / 3;
            rest = num % 3;
            api[quotient].sector_read(rest);
        }

        else if(c == 'x'){
            break;
        }
        else{
            continue;
        }
    }
    return 0;
}