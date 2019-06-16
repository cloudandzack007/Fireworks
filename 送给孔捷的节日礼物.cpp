// 送给孔捷的节日礼物.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <graphics.h>
#include <math.h>
#include <time.h>									// 使用该计时器必须包含的文件
#pragma comment ( lib, "Winmm.lib" )

using namespace std;

#define LONG	800													// 窗口长
#define WIDE	500													// 窗口宽
#define NUM		13													// 烟花种类数量宏定义


void Init_Fire();					// 初始化烟花
void Load_Image();					// 加载烟花图片
void Shoot();						// 发射烟花
void Chose(DWORD& t1);				// 筛选烟花
void Show(DWORD* pMem);				// 绽放烟花
void Erase(DWORD* pMem);			// 随机擦除像素点


// 烟花结构
struct FIRE
{
	int r;					// 当前爆炸半径
	int max_r;				// 爆炸中心距离边缘最大半径
	int x, y;				// 爆炸中心在窗口的坐标
	int cen_x, cen_y;		// 爆炸中心相对图片左上角的坐标
	int width, height;		// 图片的宽高
	int xy[240][240];		// 储存图片像素点

	bool show;				// 是否绽放
	bool draw;				// 开始输出像素点
	DWORD t1, t2, dt;		// 绽放速度
}Fire[NUM];

// 烟花弹结构
struct JET
{
	int x, y;				// 喷射点坐标
	int hx, hy;				// 最高点坐标------将赋值给 FIRE 里面的 x, y
	int height;				// 烟花高度
	bool shoot;				// 是否可以发射

	DWORD t1, t2, dt;		// 发射速度
	IMAGE img[2];			// 储存花弹一亮一暗图片
	byte n : 1;				// 图片下标
}Jet[NUM];


// 初始化烟花参数
void Init_Fire()
{
	// 分别为：烟花中心到图片边缘的最远距离、烟花中心到图片左上角的距离 (x、y) 两个分量
	int r[13] = { 120, 120, 155, 123, 130, 147, 138, 138, 130, 135, 140, 132, 155 };
	int x[13] = { 120, 120, 110, 117, 110, 93,  102, 102, 110, 105, 100, 108, 110 };
	int y[13] = { 120, 120, 85,  118, 120, 103, 105, 110, 110, 120, 120, 104, 85 };

	for (int i = 0; i < NUM; i++)		// 初始化烟花
	{
		Fire[i].x = 0;					// 烟花中心坐标
		Fire[i].y = 0;
		Fire[i].width = 240;			// 图片宽
		Fire[i].height = 240;			// 图片高
		Fire[i].max_r = r[i];			// 最大半径
		Fire[i].cen_x = x[i];			// 中心距左上角距离
		Fire[i].cen_y = y[i];
		Fire[i].show = false;			// 是否绽放
		Fire[i].dt = 5;					// 绽放时间间隔
		Fire[i].t1 = timeGetTime();
		Fire[i].r = 0;					// 从 0 开始绽放


		Jet[i].x = -240;				// 烟花弹左上角坐标
		Jet[i].y = -240;
		Jet[i].hx = -240;				// 烟花弹发射最高点坐标
		Jet[i].hy = -240;
		Jet[i].height = 0;				// 发射高度
		Jet[i].t1 = timeGetTime();
		Jet[i].dt = rand() % 10;		// 发射速度时间间隔
		Jet[i].n = 0;					// 烟花弹闪烁图片下标
		Jet[i].shoot = false;			// 是否发射
	}
}

// 加载图片
void Load_Image()
{
	IMAGE fm, gm;
	loadimage(&fm, _T("fire/flower.jpg"), 3120, 240);

	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&fm);
		getimage(&gm, i * 240, 0, 240, 240);
		SetWorkingImage(&gm);

		for (int a = 0; a < 240; a++)
			for (int b = 0; b < 240; b++)
				Fire[i].xy[a][b] = getpixel(a, b);
	}

	IMAGE sm;
	loadimage(&sm, _T("fire/shoot.jpg"), 200, 50);

	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&sm);
		int n = rand() % 5;

		getimage(&Jet[i].img[0], n * 20, 0, 20, 50);			// 暗
		getimage(&Jet[i].img[1], (n + 5) * 20, 0, 20, 50);		// 亮
	}

	SetWorkingImage();											// 设置回绘图窗口
}

// 在一定范围内筛选可发射的烟花，并初始化发射参数，输出烟花弹到屏幕，播放声音
void Chose(DWORD& t1)
{
	DWORD t2 = timeGetTime();

	if (t2 - t1 > 100)
	{
		int n = rand() % 20;

		if (n < 13 && Jet[n].shoot == false && Fire[n].show == false)
		{
			Jet[n].x = rand() % LONG;
			Jet[n].y = rand() % 100 + LONG / 2;
			Jet[n].hx = Jet[n].x;
			Jet[n].hy = rand() % LONG / 3;
			Jet[n].height = Jet[n].y - Jet[n].hy;
			Jet[n].shoot = true;
			putimage(Jet[n].x, Jet[n].y, &Jet[n].img[Jet[n].n], SRCINVERT);
		}
		t1 = t2;
	}
}

// 扫描烟花弹并发射
void Shoot()
{
	for (int i = 0; i < NUM; i++)
	{
		Jet[i].t2 = timeGetTime();

		if (Jet[i].t2 - Jet[i].t1 > Jet[i].dt && Jet[i].shoot == true)
		{
			putimage(Jet[i].x, Jet[i].y, &Jet[i].img[Jet[i].n], SRCINVERT);			// 烟花弹的上升

			if (Jet[i].y > Jet[i].hy)
			{
				Jet[i].n++;
				Jet[i].y -= 5;
			}

			putimage(Jet[i].x, Jet[i].y, &Jet[i].img[Jet[i].n], SRCINVERT);

			if ((Jet[i].y - Jet[i].hy) * 4 < Jet[i].height)							// 上升到高度的 3 / 4，减速
				Jet[i].dt = rand() % 4 + 10;
			if (Jet[i].y <= Jet[i].hy)												// 上升到最大高度
			{
				putimage(Jet[i].x, Jet[i].y, &Jet[i].img[Jet[i].n], SRCINVERT);		// 擦掉烟花弹
				Fire[i].x = Jet[i].hx + 10;											// 在烟花弹中间爆炸
				Fire[i].y = Jet[i].hy;												// 在最高点绽放
				Fire[i].show = true;												// 开始绽放
				Jet[i].shoot = false;												// 停止发射
			}
			Jet[i].t1 = Jet[i].t2;
		}
	}
}

// 绽放烟花
void Show(DWORD* pMem)
{
	// 烟花个阶段绽放时间间隔，制作变速绽放效果
	int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

	for (int i = 0; i < NUM; i++)
	{
		Fire[i].t2 = timeGetTime();

		// 增加爆炸半径，绽放烟花，增加时间间隔做变速效果
		if (Fire[i].t2 - Fire[i].t1 > Fire[i].dt && Fire[i].show == true)
		{
			if (Fire[i].r < Fire[i].max_r)
			{
				Fire[i].r++;
				Fire[i].dt = drt[Fire[i].r / 10];
				Fire[i].draw = true;
			}

			if (Fire[i].r >= Fire[i].max_r - 1)
			{
				Fire[i].draw = false;
				Init_Fire();
			}
			Fire[i].t1 = Fire[i].t2;
		}

		// 如果该号炮花可爆炸，根据当前爆炸半径画烟花，颜色值接近黑色的不输出。
		if (Fire[i].draw)
		{
			for (double a = 0; a <= 6.28; a += 0.01)
			{
				int x1 = (int)(Fire[i].cen_x + Fire[i].r * cos(a));					// 相对于图片左上角的坐标
				int y1 = (int)(Fire[i].cen_y - Fire[i].r * sin(a));

				if (x1 > 0 && x1 < Fire[i].width && y1 > 0 && y1 < Fire[i].height)	// 只输出图片内的像素点
				{
					int b = Fire[i].xy[x1][y1] & 0xff;
					int g = (Fire[i].xy[x1][y1] >> 8) & 0xff;
					int r = (Fire[i].xy[x1][y1] >> 16);

					// 烟花像素点在窗口上的坐标
					int xx = (int)(Fire[i].x + Fire[i].r * cos(a));
					int yy = (int)(Fire[i].y - Fire[i].r * sin(a));

					// 较暗的像素点不输出、防止越界
					if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < LONG && yy > 0 && yy < WIDE)
						pMem[yy * LONG + xx] = BGR(Fire[i].xy[x1][y1]);	// 显存操作绘制烟花
				}
			}
			Fire[i].draw = false;
		}
	}
}

void Erase(DWORD* pMem)
{
	// 随机选择 4000 个像素点擦除
	for (int clr = 0; clr < 1000; clr++)
	{
		for (int j = 0; j < 2; j++)
		{
			int px1 = rand() % LONG;
			int py1 = rand() % WIDE;

			if (py1 < 799)				// 防止越界
				pMem[py1 * LONG + px1] = pMem[py1 * LONG + px1 + 1] = BLACK;	// 对显存赋值擦出像素点
		}
	}
}

void main()
{
	initgraph(LONG, WIDE);
	srand(time(NULL));

	mciSendString(L"play fire/bk.mp3 repeat", 0, 0, 0);	// 播放背景音乐

	settextstyle(36, 0, _T("楷体"));
	setcolor(LIGHTMAGENTA);
	outtextxy(160, 200, _T("孔捷..六一快乐La...喜欢吗"));

	DWORD t1 = timeGetTime();			// 筛选烟花计时
	DWORD* pMem = GetImageBuffer();		// 获取窗口显存指针

	Init_Fire();						// 初始化烟花参数

	Load_Image();						// 将烟花图片信息加载进相应结构中
	BeginBatchDraw();					// 开始批量绘图

	while (true)
	{
		Sleep(5);

		Erase(pMem);
		Chose(t1);						// 筛选烟花
		Shoot();						// 发射烟花
		Show(pMem);						// 绽放烟花
		FlushBatchDraw();				// 显示前面的所有绘图操作
	}
}
