#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int grid[4][4]={{0,0,0,0},
                {0,0,0,0},
                {0,0,0,0},
                {0,0,0,0}};
/*
{{1,2,3,0},
                {5,7,9,2},
                {4,6,5,7},
                {128,64,32,16}};
*/
int moves = 0;
int score = 0;
int moved;
int gameover = 0;
int stopped = 0;
int key;
int win = 0;
int cont = 0;
int rec[4];

void update_screen(int yes = 1){
    if (yes){
        for (int k=0;k<11;k++){
            cout << "\033[1F";
            cout << "\033[2K";
        }
    }
    cout << "       Moves: " << moves << "\t";
    cout << "Score: " << score << endl;
    cout << "      |---------------------------------|" << "\n";

    for (int i=0;i<4;i++){
        cout << "      |                                 |"<< "\n";
        cout << "      |";
        for (int j=0;j<4;j++){
            if(grid[i][j]==0){
                cout << "\t" << ".";
            }
            else{
                cout << "\t" << grid[i][j];
            }
        }
        cout << "\t|" << "\n";
    }
    cout << "      |---------------------------------|" << endl;

}
// past record
void past_best(int won = 0){
    ifstream r_data;
    ofstream w_data;
    string buf;
    r_data.open("game2048_record.txt");
    if (r_data.fail()){
        w_data.open("game2048_record.txt");
        w_data.close();
    }
    getline(r_data,buf);
    if (buf==""){
        w_data.open("game2048_record.txt");
        w_data << moves << "\n" << score << "\n";
        w_data << moves << "\n" << score << "\n";
        w_data.close();
        return;
    }
    r_data.seekg(0);
    for (int j=0;j<4;j++){
        getline(r_data,buf);
        rec[j] = stoi(buf);
    }
    r_data.close();
    // rewrite
    if (rec[1]<score){
        rec[0] = moves;
        rec[1] = score;
    }
    if (((rec[2]>moves) || (rec[2]==0)) && won){
        rec[2] = moves;
        rec[3] = score;
    }
    w_data.open("game2048_record.txt",ofstream::trunc);
    for (int k=0;k<4;k++){
        w_data << rec[k] << "\n";
    }
    w_data.close();
}
void gameover_screen(){
    if(win){
        past_best(1);
        cout << "\033[2K"<<"\033[33m       You made it!";
    }
    else if(stopped)
    {
        past_best(0);
        cout << "\033[2K"<<"       Game Paused!";
    }
    
    else{
        past_best(0);
        cout << "\033[2K"<<"       Game Over!";
    }
    // show record
    string disp;
    if (rec[0]<10){
        disp = to_string(rec[0])+"   ";
    }
    else{
        disp = to_string(rec[0]);
    }
    cout << "\n\033[36m<Highscore>\tMoves: " << disp << "\tScore: " << rec[1];
    if (rec[2]<10){
        disp = to_string(rec[2])+"   ";
    }
    else{
        disp = to_string(rec[2]);
    }
    cout << "\n<Least Moves>\tMoves: " << disp << "\tScore: " << rec[3] << endl;
}

// controls

void Up(int yes=1){

    int col_space[4] = {0,0,0,0};

    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            if(grid[i][j]==0){
                col_space[j]++;
                continue;
            }
            // falls x squares, x= no. of empty squares
            if(col_space[j]>0){
                grid[i-col_space[j]][j] = grid[i][j];
                grid[i][j] = 0;
                moved++;
            }
        }
    }
}
void Down(int yes=1){

    int col_space[4] = {0,0,0,0};

    for (int i=3;i>=0;i--){      // from bottom to up
        for (int j=0;j<4;j++){
            if(grid[i][j]==0){
                col_space[j]++;
                continue;
            }
            // falls x squares, x= no. of empty squares
            if(col_space[j]>0){
                grid[i+col_space[j]][j] = grid[i][j];
                grid[i][j] = 0;
                moved++;
            }
        }
    }
}
void Left(int yes=1){
    int row_space[4] = {0,0,0,0};

    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            if(grid[j][i]==0){
                row_space[j]++;
                continue;
            }
            // falls x squares, x= no. of empty squares
            if(row_space[j]>0){
                grid[j][i-row_space[j]] = grid[j][i];
                grid[j][i] = 0;
                moved++;
            }
        }
    }
}
void Right(int yes=1){

    int row_space[4] = {0,0,0,0};

    for (int i=3;i>=0;i--){
        for (int j=0;j<4;j++){
            if(grid[j][i]==0){
                row_space[j]++;
                continue;
            }
            // falls x squares, x= no. of empty squares
            if(row_space[j]>0){
                grid[j][i+row_space[j]] = grid[j][i];
                grid[j][i] = 0;
                moved++;
            }
        }
    }
}
// merging
void merge_up(){
    int merging[4] = {0,0,0,0}; // col
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            if(merging[j]!=0 && (grid[i][j]==merging[j])){
                score += grid[i][j]*2;
                grid[i][j] *= 2;
                grid[i-1][j] = 0;
                merging[j] = 0;     // cannot be target of merge
                continue;
            }
            if(grid[i][j]!=0){
                merging[j] = grid[i][j];
            }
        }
    }
    Up(0);   // stack all blocks, this move doesn't count
}
void merge_down(){
    int merging[4] = {0,0,0,0}; // col
    int i,j;
    for(i=3;i>=0;i--){
        for(j=0;j<4;j++){
            if(merging[j]!=0 && (grid[i][j]==merging[j])){
                score += grid[i][j]*2;
                grid[i][j] *= 2;
                grid[i+1][j] = 0;
                merging[j] = 0;     // cannot be target of merge
                continue;
            }
            if(grid[i][j]!=0){
                merging[j] = grid[i][j];
            }
        }
    }
    Down(0);   // stack all blocks, this move doesn't count
}
void merge_left(){
    int merging[4] = {0,0,0,0}; // col
    int i,j;
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            if(merging[j]!=0 && (grid[j][i]==merging[j])){
                score += grid[j][i]*2;
                grid[j][i] *= 2;
                grid[j][i-1] = 0;
                merging[j] = 0;     // cannot be target of merge
                continue;
            }
            if(grid[j][i]!=0){
                merging[j] = grid[j][i];
            }
        }
    }
    Left(0);   // stack all blocks, this move doesn't count
}
void merge_right(){
    int merging[4] = {0,0,0,0}; // col
    int i,j;
    for (int i=3;i>=0;i--){
        for (int j=0;j<4;j++){
            if(merging[j]!=0 && (grid[j][i]==merging[j])){
                score += grid[j][i]*2;
                grid[j][i] *= 2;
                grid[j][i+1] = 0;
                merging[j] = 0;     // cannot be target of merge
                continue;
            }
            if(grid[j][i]!=0){
                merging[j] = grid[j][i];
            }
        }
    }
    Right(0);   // stack all blocks, this move doesn't count
}
int try_merge(){
    int k = 0;
    int merging[4] = {0,0,0,0}; // col
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            if(merging[j]!=0 && (grid[i][j]==merging[j])){
                k++;
            }
            if(grid[i][j]!=0){
                merging[j] = grid[i][j];
            }
        }
    }
    for(int h=0;h<4;h++){
        merging[h] = 0;
    }
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            if(merging[j]!=0 && (grid[j][i]==merging[j])){
                k++;
            }
            if(grid[j][i]!=0){
                merging[j] = grid[j][i];
            }
        }
    }
    return k;
}

// generate blocks
void gen_block(){
    int row,col;
    int num;
    while(1){
        row = rand()%4;
        col = rand()%4;
        if(grid[row][col]!=0){
            continue;
        }
        if(rand()%8 == 1){
            num = 4;
        }
        else{
            num = 2;
        }
        grid[row][col] = num;
        break;
    }   
}
int win_lose(){
    int k = 0;
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            if(!cont && (grid[i][j]==2048)){
                win = 1;
                gameover = 1;
                return 1;
            }
            if(grid[i][j]!=0){
                k++;
            }
        }
    }
    if(try_merge()==0 && k==16){
        gameover = 1;
        return 1;
    }
    gameover = 0;
    return 0;
}
// save and load
void save_progress(){
    ofstream w_data;
    string buf;
    w_data.open("game2048_data.txt",ofstream::trunc);

    w_data<< "m" << moves << "s" << score << "\n";
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            w_data << grid[i][j] << " ";
        }
        w_data << "\n";
    }
    w_data.close();
    past_best();
    cout << "\033[2K"<<"       Progress saved!";
}
int load_progress(int yes = 1){
    ifstream r_data;
    ofstream w_data;
    string buf;
    string num_str;
    string ch;
    int pos;
    r_data.open("game2048_data.txt");
    if (r_data.fail()){
        w_data.open("game2048_data.txt");
        w_data.close();
        cout << "\033[2K"<<"       No previous game progress!";
        r_data.close();
        return 0;
    }
    getline(r_data,buf);

    if (buf == ""){
        cout << "\033[2K"<<"       No previous game progress!";
        r_data.close();
        return 0;
    }
    num_str = buf.substr(1,buf.find('s')-1);
    moves = stoi(num_str);
    num_str = buf.substr(buf.find('s')+1);
    score = stoi(num_str);
    for (int i=0;i<4;i++){
        getline(r_data,buf);
        pos = 0;
        for (int j=0;j<4;j++){
            num_str = "";
            for (int k=pos;k<buf.length();k++){
                ch = buf[k];
                if (ch!=" "){
                    num_str += ch;
                }
                else{
                    pos = k+1;
                    break;
                }
            }
            grid[i][j] = stoi(num_str);
        }
    }
    r_data.close();
    update_screen(yes);
    cout << "\033[2K"<<"       Progress loaded!";
    return 1;
}
// set up
void clear_all(){
    moves=0;
    score=0;
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            grid[i][j] = 0;
        }
    }
}
void new_game(){
    clear_all();
    gen_block();
    gen_block();
}
void intro(){
    cout << "\033[2K" << "\033[0F";
    cout << "\033[2K" << "\033[0F";
    cout << "Press N for new game, C to save, L to load. (If any progress exists)" << "\n";
    cout << "Please don't change the game data, otherwise the game will fail to run! QAQ"<< "\n";
}
void intro(int){
    for (int i=0;i<13;i++){
        cout << "\033[1F";
    }
    cout << "\033[2K"<< "Press N for new game, C to save, L to load. (If any progress exists)" << "\033[1E";
    cout << "\033[2K"<< "Please don't change the game data, otherwise the game will fail to run! QAQ";
    for (int i=0;i<12;i++){
        cout << "\033[1E";
    }
}
void welcome(){
    int finished = 0;
    cout << "\033[36mWelcome to 2048! Press WASD to move. ";
    cout << "Press X to end." << "\n";
    cout << "Press N for new game. Press L to load. (If any progress exists)" << "\n";
    cout << "Please don't change the game data, otherwise the game will fail to run! QAQ" << "\n";
    // cout << "Press C to save, L to load. (If any progress exists)" << "\n";
    while (!finished){
        switch(key=getch()){
            case 110:   // N
                new_game();
                intro();
                update_screen(0);
                finished = 1;
                break;
            case 108:   // L
                if(load_progress(0)){
                    intro(1);
                    finished = 1;
                }
                break;
            case 120:   // X
            cout << "\n";
                gameover = 1;
                stopped = 1;
                finished = 1;
                break;
        }
    }
    
}
bool cont_game(){
    int finished=0;
    cout << "Do you want to continue? <Y> Yes | <X> No\n";
    while (!finished){
        switch(key=getch()){
            case 121:   // y
                for (int i=0;i<4;i++){
                    cout << "\033[1F";
                    cout << "\033[2K";
                }
                return true;
            case 120:   // x
                return false;
        }
    }
    return false;
}
// main
void main_game(){
    if (gameover){
        return;
    }
    while(!gameover){
        update_screen();
        if(win_lose()){
            break;
        }
        switch(key=getch()){
            case 119:   // w
                Up();
                merge_up();
                break;
            case 97:    // a
                Left();
                merge_left();
                break;
            case 115:   // s
                Down();
                merge_down();
                break;
            case 100:   // d
                Right();
                merge_right();
                break;

            case 120:   // x
                gameover = 1;
                stopped = 1;
                break;
            case 110:   // n
                new_game();
                update_screen();
                break;
            case 99:    // c
                save_progress();
                break;
            case 108:   // l
                load_progress();
                break;
        }
        if(moved>0){
            gen_block();
            moves++;
            moved = 0;
        }
    }
}

int main(){
    srand(time(0));
    clear_all();
    welcome();
    main_game();
    gameover_screen();
    if (win && cont_game()){
        gameover = 0;
        win = 0;
        cont = 1;
        main_game();
        gameover_screen();
    }
    cout << "\033[0m";
    // cout << win;
    return 0;
}

/*
      |---------------------------------|
      |                                 |
      | .       4       .       .       |
      |                                 |
      | .       4       .       .       |
      |                                 |
      | .       4       .       .       |
      |                                 |
      | .       4       .       4       |
      |---------------------------------|
*/