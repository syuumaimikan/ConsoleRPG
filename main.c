#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // gotoxyのために必要

#define WIDTH 40
#define HEIGHT 25

void gotoxy(int x, int y);
int KEY(int *Kn, int *Y, int *X);
void hideCursor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; // カーソルを非表示に設定
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
int main(void)
{
    hideCursor();
    int kn;                // 入力キー
    int sty, stx;          // キャラクターの座標
    int fy, fx;            // マップ範囲
    int jm[HEIGHT][WIDTH]; // 移動可不判定
    char mapc[3] = {0};    // 複写されたマップ構成情報

    int y, x;                     // 判定生成、座標生成･描写用
    char mc[2][3] = {"　", "壁"}; // マップチップ

    // マップ構成
    char map[][81] = {
        "壁壁壁壁壁壁壁壁壁壁",
        "壁　壁　　　　　　壁",
        "壁　壁壁壁壁　壁　壁",
        "壁　壁壁　　　壁　壁",
        "壁　　壁　壁壁壁　壁",
        "壁壁　壁　壁　　　壁",
        "壁　　壁　壁　壁壁壁",
        "壁　壁壁　壁　　　壁",
        "壁　　　　壁　壁　壁",
        "壁壁壁壁壁壁壁壁壁壁"};

    char st[1][3] = {"人"}; // キャラクター

    sty = 8, stx = 8; // キャラクター初期座標指定

    fy = sizeof(map) / 81;   // 行
    fx = strlen(map[0]) / 2; // 列

    // 判定生成
    for (y = 0; y < fy; y++)
    {
        for (x = 0; x < fx; x++)
        {
            strncpy(mapc, &map[y][(x) * 2], 2);
            jm[y][x] = (strncmp(mc[1], mapc, 2) == 0) ? 1 : 0;
        }
    }

    while (1)
    {
        // 画面の描写
        for (y = 0; y < fy; y++)
        {
            for (x = 0; x < fx; x++)
            {
                strncpy(mapc, &map[y][(x) * 2], 2);
                gotoxy(x * 2 + 1, y + 1); // カーソルを設定
                if (jm[y][x] == 0)
                {
                    if (y == sty && x == stx)
                    {
                        printf("%s", st[0]); // キャラ表示
                    }
                    else
                    {
                        printf("%s", mapc); // マップを表示
                    }
                }
                else // 移動不可
                {
                    printf("%s", mapc); // マップを表示
                }
            }
        }

        // 入力キー、移動座標出力
        KEY(&kn, &sty, &stx);

        // 壁、マップ外への侵入防止
        if (kn == 0x1b) // 終了操作
        {
            printf("▼終了します。\n");
            break;
        }

        // キャラクターの移動
        int old_sty = sty; // 移動前のY座標
        int old_stx = stx; // 移動前のX座標

        if (kn == 0x4b && stx > 0 && jm[sty][stx - 1] == 0)
            stx--; // 左
        else if (kn == 0x4D && stx < fx - 1 && jm[sty][stx + 1] == 0)
            stx++; // 右
        else if (kn == 0x48 && sty > 0 && jm[sty - 1][stx] == 0)
            sty--; // 上
        else if (kn == 0x50 && sty < fy - 1 && jm[sty + 1][stx] == 0)
            sty++; // 下

        // 移動があった場合の処理
        if (old_sty != sty || old_stx != stx)
        {
            gotoxy(old_stx * 2 + 1, old_sty + 1); // 前の位置に戻す
            printf("　");                         // 空白で消す
            gotoxy(stx * 2 + 1, sty + 1);         // 新しい位置に移動
            printf("%s", st[0]);                  // キャラを表示
        }
    }
    return 0;
}

// 入力キー、移動座標出力
int KEY(int *Kn, int *Y, int *X)
{
    while (1)
    {
        *Kn = getch(); // 1:通常キー
        if ((*Kn == 0x00) || (*Kn == 0xe0))
        {
            *Kn = getch(); // 2:特殊キー
            break;
        }
        else if (*Kn == 0x1b) // Esc
        {
            break; // Escキーの処理を終了
        }
    }
    return 0;
}

// カーソルを移動する関数
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x - 1; // Windowsでは座標は0から始まるため1を引く
    coord.Y = y - 1; // 同様に1を引く
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}