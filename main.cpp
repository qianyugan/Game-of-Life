#include <iostream>
#include <windows.h>
#include <cstring>
#include <string.h>

using namespace std;

/*
   注意：h （高）和 w （宽）需要根据控制台设置
   使得生命状态图边界不超过控制台长宽。否则显示会出问题
*/

const int h = 55;
const int w = 95;

const string live = " ■";
/*
   存活生命用实心方块表示，注意：
   本程序运行于 Windows 11 的 Windows Terminal下
   故方块前加了个空格以使显示效果整齐
*/

const string die = "  ";     // 死亡状态

// 设置当前生命状态图 surface 和下一新生命状态图 new_surface
int surface[h][w], new_surface[h][w];

// 设置生命状态图填充内容
void putVal(int val){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            surface[i][j] = val;
            new_surface[h][w] = val;
        }
    }
}

// 清空生命状态图
void clearSurface(){
    putVal(0);
}

// 填满生命状态图
void fullSurface(){
    putVal(1);
}

// 设置输出游标，只针对被修改的地方 (x, y) 输出
void updatePos(int x, int y) {
	COORD pos = {y * 2, x};
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
	printf("%s", (surface[x][y] != 0) ? live.c_str() : die.c_str());
}


// 展示生命状态图
void showSurface(){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            printf("%s", (surface[i][j] != 0) ? live.c_str() : die.c_str());
        }
        printf("\n");
    }
}

// 随机生成生命状态图，p 为初始存活概率
void randomSeed(int p){
    clearSurface();
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if((rand() % 100) <= p){
                surface[i][j] = 1;
                new_surface[h][w] = 1;
            }
        }
    }
}

// 判断生命位置是否临界以及临界位置，不在生命状态图边界返回-1，否则返回从生命状态图左上顺时针旋转对应 0~7
int pdPos(int x, int y){
    if(x == 0){
        if(y == 0) return 0;
        else if(y == (w - 1)) return 2;
        else return 1;
    }else if(x == (h - 1)){
        if(y == (w - 1)) return 4;
        else if(y == 0) return 6;
        else return 5;
    }else if(y == 0) return 7;
    else if(y == (w - 1)) return 3;
    else return -1;
}

// code 状态码即 pdPos() 得到的生命位置
int pdAround(int x, int y, int code){
    if(code == -1){
        return (
                (surface[x - 1][y - 1] == 1)
              + (surface[x - 1][y] == 1)
              + (surface[x - 1][y + 1] == 1)
              + (surface[x][y + 1] == 1)
              + (surface[x + 1][y + 1] == 1)
              + (surface[x + 1][y] == 1)
              + (surface[x + 1][y - 1] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 0){
        return (
                (surface[x][y + 1] == 1)
              + (surface[x + 1][y + 1] == 1)
              + (surface[x + 1][y] == 1)
                );
    }
    if(code == 1){
        return (
                (surface[x][y + 1] == 1)
              + (surface[x + 1][y + 1] == 1)
              + (surface[x + 1][y] == 1)
              + (surface[x + 1][y - 1] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 2){
        return (
                (surface[x + 1][y] == 1)
              + (surface[x + 1][y - 1] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 3){
        return (
                (surface[x - 1][y - 1] == 1)
              + (surface[x - 1][y] == 1)
              + (surface[x + 1][y] == 1)
              + (surface[x + 1][y - 1] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 4){
        return (
                (surface[x - 1][y - 1] == 1)
              + (surface[x - 1][y] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 5){
        return (
                (surface[x - 1][y - 1] == 1)
              + (surface[x - 1][y] == 1)
              + (surface[x - 1][y + 1] == 1)
              + (surface[x][y + 1] == 1)
              + (surface[x][y - 1] == 1)
                );
    }
    if(code == 6){
        return (
                (surface[x - 1][y] == 1)
              + (surface[x - 1][y + 1] == 1)
              + (surface[x][y + 1] == 1)
                );
    }
    if(code == 7){
        return (
                (surface[x - 1][y] == 1)
              + (surface[x - 1][y + 1] == 1)
              + (surface[x][y + 1] == 1)
              + (surface[x + 1][y + 1] == 1)
              + (surface[x + 1][y] == 1)
                );
    }
    return -999;
}

// 设置位于 (x, y) 处生命的状态
void setState(int x, int y){
    // 如果无生命
    if(surface[x][y] == 0){
        // 当周围生命数量为 3 时产生新生命（模拟繁殖）
        if(pdAround(x, y, pdPos(x, y)) == 3){
            new_surface[x][y] = 1;
            return;
        }
    }else{
        // 如果已存在生命，当周围生命数量低于 2 或大于 3 死亡，分别模拟生命数量稀少和生命数量过多
        if((pdAround(x, y, pdPos(x, y)) < 2) || (pdAround(x, y, pdPos(x, y)) > 3)){
            new_surface[x][y] = 0;
            return;
        }
    }
}

// 刷新当前生命状态图为新生命状态图
void refreshSurface(){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if(surface[i][j] != new_surface[i][j]){
                surface[i][j] = new_surface[i][j];
                updatePos(i, j);
            }
        }
    }
}

// 一次生命状态图模拟
void fun(){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            setState(i, j);
        }
    }
    refreshSurface();
}

int main() {
    int p = 80;
    cout << "输入初始存活概率（0~100，建议不要过高）：___\b\b\b";
    cin >> p;
    system("cls");
    clearSurface();  // 清空生命状态图
    randomSeed(p);    // 随机产生生命状态图
    showSurface();   // 输出初始生命状态图
    Sleep(1000);
    int j = 100;
    while(true){
        fun();          // 一次生命状态图模拟
    }
    return 0;
}
