#define _CRT_SECURE_NO_WARNINGS 1  

#include <graphics.h>     // EasyX 图形库
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mmsystem.h>     // 多媒体头文件
#pragma comment(lib, "winmm.lib") 


// 高校信息结构体
typedef struct {
    char name[50];   // 学校名称
    int score;       // 录取分数线
    int rank;        // 全省位次
} University;

// 高校数据数组及数量
University unis[1200];
int count = 0;

// 当前“全部院校”列表显示起始索引
int listStart = 0;


// 播放背景音乐
void playBGM() {
    mciSendString("open \"bgm.mp3\" type mpegvideo alias BGM", NULL, 0, NULL);
    mciSendString("play BGM repeat", NULL, 0, NULL);
}

// 读取高校数据
void loadData() {
    FILE* fp = fopen("data.txt", "r");
    if (!fp) {
        MessageBox(NULL, "未找到 data.txt 文件！", "文件错误", MB_ICONERROR);
        return;
    }
    count = 0;
    while (fscanf(fp, "%s %d %d", unis[count].name, &unis[count].score, &unis[count].rank) != EOF) {
        count++;
    }
    fclose(fp);
}

// 排序函数
void sortData() {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (unis[j].score < unis[j + 1].score) {
                University temp = unis[j];
                unis[j] = unis[j + 1];
                unis[j + 1] = temp;
            }
        }
    }
}

// 绘制系统主界面
void drawInterface() {
    setbkcolor(WHITE);
    cleardevice();

    // 顶部标题栏
    setfillcolor(RGB(0, 102, 204));
    fillrectangle(0, 0, 800, 70);
    settextcolor(WHITE);
    settextstyle(30, 0, "微软雅黑");
    setbkmode(TRANSPARENT);
    outtextxy(250, 15, "高考志愿智能辅助系统");

    // 位次评估按钮
    setfillcolor(RGB(255, 153, 51));
    fillroundrect(200, 100, 380, 150, 10, 10);
    settextcolor(BLACK);
    settextstyle(20, 0, "微软雅黑");
    outtextxy(245, 115, "位次评估");

    // 院校搜索按钮
    setfillcolor(RGB(34, 177, 76));
    fillroundrect(420, 100, 600, 150, 10, 10);
    outtextxy(465, 115, "院校搜索");

    // 全部院校按钮
    setfillcolor(RGB(153, 204, 255));
    fillroundrect(310, 170, 490, 220, 10, 10);
    outtextxy(340, 185, "全部院校");

    // 底部提示
    settextcolor(DARKGRAY);
    settextstyle(16, 0, "微软雅黑");
    outtextxy(280, 560, "操作提示：点击按钮操作");
}

// 按关键字搜索高校
void searchUniversity(char* keyword) {
    drawInterface();

    settextcolor(BLACK);
    settextstyle(22, 0, "微软雅黑");
    char title[100];
    sprintf(title, ">> 搜索关键词 [%s] 的结果如下：", keyword);
    outtextxy(50, 180, title);

    int y = 230;
    for (int i = 0; i < count; i++) {
        
        if (strstr(unis[i].name, keyword) != NULL) {
            char info[150];
            settextcolor(BLUE);
            settextstyle(18, 0, "Consolas");
            sprintf(info, "ID:%-3d | %-20s | 线:%-4d | 位次:%-6d",
                i + 1, unis[i].name, unis[i].score, unis[i].rank);
            outtextxy(50, y, info);
            y += 35;
            if (y > 540) break; // 防止超出窗口
        }
    }
}

// 根据位次推荐院校
void showRecommend(int myRank) {
    drawInterface();

    settextcolor(BLACK);
    settextstyle(22, 0, "微软雅黑");
    outtextxy(50, 180, ">> 位次推荐院校（冲 / 稳 / 保）：");

    int y = 230;
    for (int i = 0; i < count; i++) {
        char info[150];
        COLORREF color;
        char type[10];

        if (myRank <= unis[i].rank - 1500) {
            color = RGB(0, 153, 0); strcpy(type, "[保]");
        }
        else if (myRank <= unis[i].rank + 1500) {
            color = RGB(204, 102, 0); strcpy(type, "[稳]");
        }
        else if (myRank <= unis[i].rank + 4000) {
            color = RGB(204, 0, 0); strcpy(type, "[冲]");
        }
        else continue;

        settextcolor(color);
        settextstyle(18, 0, "Consolas");
        sprintf(info, "%s %-20s | 分数线:%-4d | 位次:%-6d",
            type, unis[i].name, unis[i].score, unis[i].rank);
        outtextxy(50, y, info);
        y += 35;
        if (y > 540) break;
    }
}

// 显示所有高校信息
void showAllUniversities() {
    drawInterface();

    settextcolor(BLACK);
    settextstyle(22, 0, "微软雅黑");
    outtextxy(50, 180, ">> 所有高校信息（滚轮上下滑动）：");

    int y = 230;
    int maxShow = 8; // 一页显示 8 所

    for (int i = listStart; i < count && i < listStart + maxShow; i++) {
        char info[200];
        settextcolor(BLUE);
        settextstyle(18, 0, "Consolas");

        sprintf(info, "ID:%-3d | %-20s | 分数线:%-4d | 位次:%-6d",
            i + 1, unis[i].name, unis[i].score, unis[i].rank);
        outtextxy(50, y, info);
        y += 35;
    }
}


int main() {
    initgraph(800, 600);  // 初始化图形窗口
    loadData();           // 读取高校数据
    sortData();           // 排序
    playBGM();            // 播放背景音乐
    drawInterface();      // 绘制主界面

    ExMessage m;
    while (true) {
        if (peekmessage(&m, EM_MOUSE)) {

            // 左键点击事件
            if (m.message == WM_LBUTTONDOWN) {

                // 位次评估按钮
                if (m.x >= 200 && m.x <= 380 && m.y >= 100 && m.y <= 150) {
                    char input[20];
                    if (InputBox(input, 20, "请输入您的全省位次：", "位次查询", "", 0, 0, false)) {
                        int r = atoi(input);
                        if (r > 0) showRecommend(r);
                    }
                }
                // 院校搜索按钮
                else if (m.x >= 420 && m.x <= 600 && m.y >= 100 && m.y <= 150) {
                    char keyword[50];
                    if (InputBox(keyword, 50, "请输入要搜索的学校名称：", "院校搜索", "", 0, 0, false)) {
                        if (strlen(keyword) > 0) searchUniversity(keyword);
                    }
                }
                // 全部院校按钮
                else if (m.x >= 310 && m.x <= 490 && m.y >= 170 && m.y <= 220) {
                    listStart = 0; // 点击按钮时重置起始索引
                    showAllUniversities();
                }
            }

            // 鼠标滚轮
            else if (m.message == WM_MOUSEWHEEL) {
                if (m.wheel > 0 && listStart > 0) // 向上滚
                    listStart--;
                else if (m.wheel < 0 && listStart < count - 1) // 向下滚
                    listStart++;

                showAllUniversities(); // 刷新列表显示
            }
        }
    }

    mciSendString("close BGM", NULL, 0, NULL); // 关闭音乐
    closegraph(); // 关闭图形窗口
    return 0;
}
