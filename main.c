// 22.cpp : 定义控制台应用,程序的入口点。
//
#include <stdio.h>
#include <conio.h>
#include <wchar.h>
#include <conio.h>
#include <windows.h>
#include <locale.h>
#define RED RGB(255,0,0)
#define YELLOW RGB(255,255,0)
#define GRAY RGB(128,128,128)
#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)
#define BLUE RGB(0,0,255)
#define STONE RGB(192,192,192)
#define DARKBLUE RGB(1,119,214)
#define LIGHTBLUE RGB(30,140,221)
#define ORINGE RGB(239,150,16)
#define FALSE 0
#define TRUE 1
#define W 50
#define H 20

typedef struct student
{
	char			name[20];
	int				student_id;
	unsigned int	age;
	char			sex[4];
	int				scores[5];
}student;

typedef HWND (*PROCGETCONSOLEWINDOW)();

typedef struct student_node
{
	struct student_node		*prev;
	student		            data;
	struct student_node		*next;
}student_node;


PROCGETCONSOLEWINDOW func;
HWND g_hwnd;
HDC g_hDc;
HDC g_temphDc; 
HANDLE g_hOut;
HBRUSH g_down_brush;
HBRUSH g_top_brush;
HBRUSH g_white_brush;
HBRUSH g_black_brush;
HBRUSH g_gray_brush;
HBRUSH g_stone_brush;
HBRUSH g_blue_brush;
HBRUSH g_oringe_brush;
HBRUSH g_light_blue_brush;
HPEN hPen;
HFONT hFont; 
int g_width;
int g_height;
int g_lstKey;
int g_slcKey;
int g_state = -1;
FILE *g_file;
student_node* g_header;
student_node* g_current;
student_node* g_last_list;
student_node* g_frist_list;
student_node* g_looking_for;
RECT g_window_rect;
RECT g_top_rect;
RECT g_down_rect;
RECT g_list_rect = {270,125+2,0,325};
RECT g_tips_rect= {370,25+1,0,125};
RECT g_menu_rect[5] =
{
	{ 30 ,2,30+W ,25 },
	{ 100,2,100+W,25 },
	{ 180,2,180+W,25 },
	{ 250,2,250+W,25 },
	{ 320,2,320+W,25 }
};
char *g_menu_text[10] = 
{ 
	"1.编辑",
	"2.查询",
	"3.统计",
	"4.排序",
	"5.退出"
};
RECT g_child_menu_rect[5] =
{
	{ 30,25,30+W,85},
	{ 100,25,100+W,85 },
	{ 180,25,180+W,125 },
	{ 250,25,250+W,125 },
	{ 320,25,320+W,45 }
};
char *g_child_menu_text[5][5] =
{
	"添加","编辑","删除",0,0,
	"列表","姓名","学号",0,0,
	"平均","最高","最低",0,0,
	"学号","工数","军理","电路",0,
	"退出",0,0,0,0,
};
char *g_subjects[3] = { "工数","军理","电路" };
char *g_tips;

int				main();				//main函数
void			press_enter();		//菜单选择时按下回车  									
void			draw();				//绘制菜单及下方状态栏
void			draw_menu(HDC);		//菜单下拉文本绘制    
int				init_console();		//初始化所有全局变量,并获取控制台句柄,为绘制图片提供数据
void			init_file();		//读取或创建文件    
void			paint_thread();		//每隔1s绘制菜单的线程
void			add_student();		//添加学生函数
void			press_vector(int);
void			goto_xy(int,int);	//移动控制台光标
void			set_rect(RECT*,int,int,int,int);//设置矩形，功能性函数
void			write_data();

//链表相关操作
int				is_empty(student_node*);					//是否为空,空返回1,不空返回0
int				add_node(student_node *, student );			//添加结点,成功返回1,不成功返回0
void			scores_node(student_node* header, int index);
void			swap_node(student_node* node1, student_node* node2);
void			max_node(student_node* header);
int				insert_node(student_node*, int,student);	//向序列为index的地方插入结点,序列起始为1
int				del_node(student_node* header, int i);		//删除序列为i的结点,序列起始为1
int				num_node(student_node *header);				//返回结点数量
int				list_node(student_node *header);			
student_node*	search_name(student_node*, char*, int*);	//通过名字寻找结点,index指针用于保存序列
student_node*	search_id(student_node*,int,int*);			//通过id寻找结点,index指针用于保存序列


//************************************
//主函数
//
//main()		进行初始化
//press_enter()	按下回车后的操作
//
//************************************
int main()
{

	HINSTANCE	hInstance = GetModuleHandle(NULL);
	wint_t		i;
	int			key;
	HMODULE 	hKernel32=GetModuleHandle("kernel32");
	func		= (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32,"GetConsoleWindow");
	init_file();
	init_console();
	goto_xy(20, 10);

	while (TRUE)
	{
		key = getch();
		if (key == 13) 
		{
		    press_enter();
		}
		if (key >= '1'&& key <= '5')
		{
			g_slcKey = 0;
		    g_state = key-'1';
		}
		if (key == 224)
		{
		    key = getch();
		    fflush(stdin);
		    press_vector(key);	    
		}		
		draw();
	}
	return 0;
}
void press_enter()
{
	char		name[20];
	int			i;
	int			k;
	int			id;
	int			index;
	int total;
	switch (g_state)
	{
	case 0:
		switch (abs(g_slcKey % 3))
		{
		case 0:
			g_state = 10;
			g_slcKey = 0;
			draw();
			add_student();
			g_state = -1;
			draw();
			break;
		case 2:
			g_state = 12;
			g_slcKey = 0;
			draw();
			goto_xy(0, 10);
			printf("待删除的学生姓名:");
			scanf("%s", name);
			g_looking_for = search_name(g_header, name, &index);
			if(g_looking_for)	del_node(g_header, index);
			system("cls");
			goto_xy(0, 10);
			break;
		default:
			break;
		}
		break;
	case 1:
		switch (abs(g_slcKey % 3))
		{
		case 0:
			g_state = 20;
			g_slcKey = 0;
			if (!is_empty(g_header))
			{
				g_frist_list = g_header->next;
				g_current = g_frist_list;
			}
			else
			{
				g_frist_list = NULL;
			}
			draw();
			break;
		case 1:
			g_state = 21;
			g_slcKey = 0;
			fflush(stdin);
			goto_xy(0, 10);
			printf("待查找的学生姓名:");
			g_looking_for = NULL;
			scanf("%s", name);
			g_looking_for = search_name(g_header, name, NULL);
			system("cls");
			goto_xy(0, 10);
			draw();
			break;
		case 2:
			g_state = 22;
			g_slcKey = 0;
			fflush(stdin);
			goto_xy(0, 10);
			printf("待查找的学生学号:");
			g_looking_for = NULL;
			scanf("%d", &id);
			g_looking_for = search_id(g_header, id, NULL);

			system("cls");
			goto_xy(0, 10);
			draw();
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (abs(g_slcKey % 3))
		{
		case 0:
			g_state = 30;
			g_slcKey = 0;
			draw();
			break;
		case 1:
			g_state = 31;
			g_slcKey = 0;
			draw();
			break;
		case 2:
			g_state = 32;
			g_slcKey = 0;
			draw();
			break;
		}
		break;
	case 3:	
		switch (abs(g_slcKey%4))
		{
		case 0:
			g_state = 40;
			g_slcKey = 0;
			total = num_node(g_header);
			for (i = 0;i<total;i++)
			{
				max_node(g_header);
			}

			draw();
			break;
		case 1:
			g_state = 41;
			g_slcKey = 0;
			total = num_node(g_header);
			for (i = 0; i < total; i++)
			{
				scores_node(g_header,0);
			}
			break;
		case 2:
			g_state = 41;
			g_slcKey = 0;
			total = num_node(g_header);
			for (i = 0; i < total; i++)
			{
				scores_node(g_header, 1);
			}
			break;
		case 3:
			g_state = 41;
			g_slcKey = 0;
			total = num_node(g_header);
			for (i = 0; i < total; i++)
			{
				scores_node(g_header, 2);
			}
			break;
		default:
			break;
		}

		break;
	case 4:
		switch (g_slcKey)
		{
		case 0:
			g_slcKey = 0;
			write_data();
			exit(1);
			break;
			draw();

		default:
			break;
		}
		break;
	default:
		break;

	}
}

void press_vector(int key)
{

	if (g_state >= 0 && g_state <= 4)
	{
		if (key == 72) g_slcKey--;
		if (key == 80) g_slcKey++;
	}
	switch (g_state)
	{
	case 20:
		if (key == 72)
		{
			if (g_current)
			{
					if (g_current->prev == g_header)
					{
						g_current = g_header->next;

					}
					else
					{
						if (g_current == g_frist_list)
						{
							g_frist_list = g_frist_list->prev;
							g_last_list = g_last_list->prev;
						}
						g_current = g_current->prev;
					}
				
			}
		}
		if (key == 80)
		{
			if (g_current)
			{
				if (g_current->next == NULL)
				{
					g_current = g_last_list;
				}
				else
				{
					if (g_current == g_last_list)
					{
						g_frist_list = g_frist_list->next;
						g_last_list = g_last_list->next;
					}
				
					g_current = g_current->next;
				}	
				
			}		
		}
		break;
	default:
		break;
	}

}
void add_student()
{
	int i = 0  ;
	student tmp_student;
	goto_xy(5, 10);
	scanf("%s", tmp_student.name);
	fflush(stdin);
	while (TRUE)
	{

		goto_xy(5, 11);
		fflush(stdin);
		scanf("%d", &tmp_student.age);
		if (tmp_student.age >= 0 && tmp_student.age <= 200)break;
		else
		{
			g_tips = "不合法的年龄";
			goto_xy(5, 11);
			printf("             ");
			draw();
		}
	}
	while (TRUE)
	{
		fflush(stdin);
		goto_xy(12, 12);
		scanf("%s", tmp_student.sex);
		fflush(stdin);
		if (!strcmp("男", tmp_student.sex) || !strcmp("女", tmp_student.sex)) break;
		else
		{
			g_tips = "性别必须为男或者女";
			goto_xy(12, 12);
			printf("             ");
			draw();
		}
	}	
	while (TRUE)
	{
		fflush(stdin);
		goto_xy(5, 13);
		scanf("%9d", &tmp_student.student_id);
		fflush(stdin);
		if (tmp_student.student_id >= 100000000) break;
		else
		{
			g_tips = "学号必须为9位";
			goto_xy(5, 13);
			printf("             ");
			draw();
		}
	}
	for (i = 0;i<3;i++)
	{
		while (TRUE)
		{
			fflush(stdin);
			goto_xy(5, 14+i);
			scanf("%d", &tmp_student.scores[i]);
			if (tmp_student.scores[i] <= 100&&tmp_student.scores[i]>=0) break;
			else
			{
				g_tips = "学生成绩应为0~100之内";
				goto_xy(5, 14+i);
				printf("             ");
				draw();
			}
		}
	}
	add_node(g_header, tmp_student);
	system("cls");
}
void list_student()
{


}
void write_data()
{
	student tmp_student;
	student_node* tmp_node = g_header->next;
	if (!is_empty(g_header))
	{
		fseek(g_file, 0, SEEK_SET);
		while(tmp_node)
		{
			tmp_student = tmp_node->data;

			fwrite(&tmp_student, sizeof(student), 1, g_file);
			fflush(g_file);
			tmp_node = tmp_node->next;
		}
	}
	fclose(g_file);

}

//************************************
//初始化相关函数
//
//init_console()	全局变量初始化,画刷初始化
//init_file()		数据文件初始化
//
//************************************

int init_console()
{
	COORD size;
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	SMALL_RECT rc;
	DWORD ThreadID;
	HANDLE paintThread;
	g_hOut= GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(g_hOut, &bInfo);
	size.X = 80;
	size.Y = 25;
	// 重新设置缓冲区大小
	SetConsoleScreenBufferSize(g_hOut, size);
	// 重置窗口位置和大小
	rc.Left		= 0;
	rc.Top		= 0;
	rc.Right	= 80 - 1;
	rc.Bottom	= 25 - 1;

	SetConsoleWindowInfo(g_hOut, TRUE, &rc);


	Sleep(1000);

	//全局变量初始化
	
	g_looking_for		= NULL;
	g_state				= -1;
	g_hwnd				= GetConsoleWindow();
	GetWindowRect(g_hwnd, &g_window_rect);
	g_hDc				= GetDC(g_hwnd);
	g_tips				= "数字键选择菜单,上下方向键选择命令,回车确定命令";
	g_lstKey			= 0;
	g_slcKey			= 0;
	g_top_brush			= CreateSolidBrush(DARKBLUE);
	g_down_brush		= CreateSolidBrush(GRAY);
	g_white_brush		= CreateSolidBrush(WHITE);
	g_black_brush		= CreateSolidBrush(BLACK);
	g_gray_brush		= CreateSolidBrush(GRAY);
	g_stone_brush		= CreateSolidBrush(STONE);
	g_blue_brush		= CreateSolidBrush(BLUE);
	g_light_blue_brush	= CreateSolidBrush(LIGHTBLUE);
	g_oringe_brush		= CreateSolidBrush(ORINGE);
	g_width				= g_window_rect.right - g_window_rect.left;
	g_height			= g_window_rect.bottom - g_window_rect.top;
	hPen				= CreatePen(0, 1, GRAY);
	g_tips_rect.right	= g_width-50;
	g_list_rect.right	= g_width-50;
	hFont = CreateFont(20, 8, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_STRING_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_MODERN, "微软雅黑");
	
	//开启绘制线程
	paintThread			= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)paint_thread, NULL, 0, &ThreadID);

	set_rect(&g_top_rect, 0, 0, g_width, 25);
	set_rect(&g_down_rect, 0, g_height - 105, g_width, g_height - 80);
	
	return 1;
}


void init_file()
{
	student* tmp_student;
	g_file			= fopen("student.dat", "ab+");
	g_header		= (student_node*)malloc(sizeof(student_node));
	g_header->next	= NULL;
	g_header->prev	= NULL;
	g_current		= g_header;
	tmp_student		= (student*)malloc(sizeof(student));
	while (fread(tmp_student,sizeof(student),1,g_file))
	{
		add_node(g_header, *tmp_student);
	}
	fclose(g_file);
	g_file			= fopen("student.dat", "rb+");
}





//************************************
// 控制台图形层GUI相关函数
// 
// draw(); 
// draw_menu();
// paint_thread();
//
//************************************
void draw()
{
	HBITMAP hbmp ;
	HBITMAP hOldBmp;
	HPEN hpen_old;
	HFONT hOldFont;
	COORD coordScreen = { 0, 0 }; //光标位置
	int x;
	int y;
	int i = 0;
	int k = 0;
	int sum_up;
	int count = 0;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	char list[600] = "";
	char row[20];
	char state[100] = "";
	char index[10];
	char name[20];
	student_node* tmp_student;
	RECT selctRect = {270,125,g_width-50,145};
	g_hDc = GetDC(g_hwnd);
	hbmp = CreateCompatibleBitmap(g_hDc, g_width, g_height-80);
	g_temphDc = CreateCompatibleDC(g_hDc);
	hOldBmp = (HBITMAP)SelectObject(g_temphDc, hbmp);
	hpen_old = (HPEN)SelectObject(g_temphDc, hPen);
	GetConsoleScreenBufferInfo(g_hOut, &csbi);
	x = csbi.dwCursorPosition.X;
	y = csbi.dwCursorPosition.Y;

	//绘制上方菜单栏
	FillRect(g_temphDc, &g_top_rect, g_top_brush);
	FillRect(g_temphDc, &g_down_rect, g_down_brush);
	//设置字体颜色
	SetTextColor(g_temphDc, WHITE);
	SetBkMode(g_temphDc, TRANSPARENT);
	//创建字体
	
	hOldFont = (HFONT)SelectObject(g_temphDc, hFont);

	for (i=0; i < 5; i++)
	{
		DrawText(g_temphDc, g_menu_text[i], -1, &g_menu_rect[i], 0);
	}
	draw_menu(g_temphDc);
	SetTextColor(g_temphDc, GRAY);
	DrawText(g_temphDc, g_tips, -1, &g_tips_rect, DT_LEFT | DT_WORDBREAK);
	
	switch (g_state)
	{
	case 10:
		goto_xy(0, 10);
		printf("姓名:");
		goto_xy(0, 11);
		printf("年龄:");
		goto_xy(0, 12);
		printf("性别(男/女):");
		goto_xy(0, 13);
		printf("学号:");
		goto_xy(0, 14);
		printf("工数:");
		goto_xy(0, 15);
		printf("军理:");
		goto_xy(0, 16);
		printf("电路:");
		goto_xy(x, y);
		strcpy(state,"状态:添加     当前学生序列:");
		sum_up = num_node(g_header)+1;
		_itoa(sum_up, index, 10);
		strcat(state, index);
		break;
	case 20:
		FrameRect(g_temphDc,&g_list_rect,g_gray_brush);
		tmp_student = g_frist_list;
		SetTextColor(g_temphDc,ORINGE);

		if(tmp_student)
		{
			strcat(list, "姓名\t学号\t\t工数\t军理\t电路\n");
			for (i=0;i<9;i++)
			{
				_itoa(tmp_student->data.student_id, row, 10);
				strcat(list, tmp_student->data.name);
				strcat(list, "\t");
				strcat(list, row);

				strcat(list, "\t");
				for (k = 0;k<3;k++)
				{
					_itoa(tmp_student->data.scores[k], row, 10);
					strcat(list, row);
					strcat(list, "\t");
				}
				if (tmp_student == g_current)
				{
					selctRect.top += 20 * (i + 1);
					selctRect.bottom += 20 * (i + 1);
					FillRect(g_temphDc, &selctRect, g_gray_brush);
				}
				if (tmp_student->next)
				{
					tmp_student = tmp_student->next;
					strcat(list, "\n");
					g_last_list = tmp_student->prev;
				}
				else
				{
					g_last_list = tmp_student;
					break;
				}
			}
			
		}
		else
		{
			strcat(list,"无学生数据！");
		}
		DrawText(g_temphDc,list,-1,&g_list_rect,DT_EXPANDTABS);
		break;
	case 21:
		if (g_looking_for)
		{
			selctRect.top += 20 ;
			selctRect.bottom += 20;
			SetTextColor(g_temphDc, WHITE);
			tmp_student = g_looking_for;
			strcat(list, "姓名\t学号\n");
			_itoa(tmp_student->data.student_id, row, 10);
			strcat(list, tmp_student->data.name);
			strcat(list, "\t");
			strcat(list, row);
			DrawText(g_temphDc, list, -1, &g_list_rect, DT_EXPANDTABS);
			memset(list, 0, sizeof(list));
			for (i = 0; i < 3; i++)
			{
				count = 0;
				sum_up = 0;
				int y = tmp_student->data.scores[i];
				set_rect(&selctRect, 270 + i * 30 + i * 40, 300 - y, 270 + i * 30 + i * 40 + 40, 300);
				FillRect(g_temphDc, &selctRect, g_oringe_brush);
				_itoa(y, row, 10);
				strcat(list, row);
				strcat(list, "          ");
			}
			strcat(list, "\n工数         军理        电路");
			set_rect(&selctRect, 270, 300, 270 + 180, 350);
			SetTextColor(g_temphDc, GRAY);
			DrawText(g_temphDc, list, -1, &selctRect, DT_EXPANDTABS);
		}
		break;
	case 22:
		if (g_looking_for)
		{
			selctRect.top += 20;
			selctRect.bottom += 20;
			SetTextColor(g_temphDc, WHITE);
			tmp_student = g_looking_for;
			strcat(list, "姓名\t学号\n");
			_itoa(tmp_student->data.student_id, row, 10);
			strcat(list, tmp_student->data.name);
			strcat(list, "\t");
			strcat(list, row);
			DrawText(g_temphDc, list, -1, &g_list_rect, DT_EXPANDTABS);
			memset(list, 0, sizeof(list));
			for (i = 0; i < 3; i++)
			{
				count = 0;
				sum_up = 0;
				int y = tmp_student->data.scores[i];
				set_rect(&selctRect, 270 + i * 30 + i * 40, 300 - y, 270 + i * 30 + i * 40 + 40, 300);
				FillRect(g_temphDc, &selctRect, g_oringe_brush);
				_itoa(y, row, 10);
				strcat(list, row);
				strcat(list, "          ");
			}
			strcat(list, "\n工数         军理        电路");
			set_rect(&selctRect, 270, 300, 270 + 180, 350);

			SetTextColor(g_temphDc,GRAY);
			DrawText(g_temphDc, list, -1, &selctRect, DT_EXPANDTABS);
		}
		break;
	case 30:
		if (!is_empty(g_header))
		{
			for (i = 0; i < 3; i++)
			{
				count = 0;
				sum_up = 0;
				tmp_student = g_header->next;
				while (tmp_student)
				{
					sum_up += tmp_student->data.scores[i];
					tmp_student = tmp_student->next;
					count++;
				}
				int y = ((float)sum_up / (float)count);
				set_rect(&selctRect, 270 + i * 30 + i * 40,300-y, 270 + i * 30 + i * 40 + 40, 300);
				FillRect(g_temphDc, &selctRect, g_oringe_brush);
				_itoa(y, row, 10);
				strcat(list, row);
				strcat(list, "          ");
			}
		}
		strcat(list, "\n工数         军理        电路");
		set_rect(&selctRect, 270, 300, 270 + 180, 350);
		DrawText(g_temphDc, list, -1, &selctRect, DT_EXPANDTABS);
		break;
	case 31:
		if (!is_empty(g_header))
		{
			for (i = 0; i < 3; i++)
			{
				count = 0;
				tmp_student = g_header->next;
				sum_up = tmp_student->data.scores[i];
				while (tmp_student)
				{
					if (sum_up >= tmp_student->data.scores[i])
					{

					}
					else
						sum_up = tmp_student->data.scores[i];
					tmp_student = tmp_student->next;
					count++;
				}
				set_rect(&selctRect, 270 + i * 30 + i * 40, 300 - sum_up, 270 + i * 30 + i * 40 + 40, 300);
				FillRect(g_temphDc, &selctRect, g_oringe_brush);
				_itoa(sum_up, row, 10);
				strcat(list, row);
				strcat(list, "          ");
			}
		}
		strcat(list, "\n工数         军理        电路");
		set_rect(&selctRect, 270, 300, 270 + 180, 350);
		DrawText(g_temphDc, list, -1, &selctRect, DT_EXPANDTABS);
		break;
	case 32:
		if (!is_empty(g_header))
		{
			for (i = 0; i < 3; i++)
			{
				count = 0;
				tmp_student = g_header->next;
				sum_up = tmp_student->data.scores[i];
				while (tmp_student)
				{
					if (sum_up <= tmp_student->data.scores[i])
					{

					}
					else
						sum_up = tmp_student->data.scores[i];
					tmp_student = tmp_student->next;
					count++;
				}
				set_rect(&selctRect, 270 + i * 30 + i * 40, 300 - sum_up, 270 + i * 30 + i * 40 + 40, 300);
				FillRect(g_temphDc, &selctRect, g_oringe_brush);
				_itoa(sum_up, row, 10);
				strcat(list, row);
				strcat(list, "          ");
			}
		}
		strcat(list, "\n工数         军理        电路");
		set_rect(&selctRect, 270, 300, 270 + 180, 350);
		DrawText(g_temphDc, list, -1, &selctRect, DT_EXPANDTABS);
		break;
	default:
		break;
	}
	strcat(state,"   当前数据文件student.dat");
	//状态栏
	SetTextColor(g_temphDc, RGB(0,0,0));
	DrawText(g_temphDc, state, -1, &g_down_rect, DT_LEFT);

	BitBlt(g_hDc, 270, 125, g_width - 50, 325, g_temphDc, 270, 125, SRCCOPY);
	BitBlt(g_hDc, 0, 0, g_width, 125, g_temphDc, 0, 0, SRCCOPY);
	BitBlt(g_hDc, 0, g_height - 105, g_width, 25,g_temphDc, 0, g_height - 105, SRCCOPY);

	SelectObject(g_temphDc,hOldBmp);
	SelectObject(g_temphDc,hOldFont);
	SelectObject(g_temphDc,hpen_old);
	//清除,防止内存泄漏
	DeleteObject(hbmp);
	DeleteDC(g_temphDc);
	ReleaseDC(g_hwnd,g_hDc);
}
void draw_menu(HDC hdc)
{
	int i = 0;
	int menu_num = 0;
	RECT menu_rect;
	RECT text_rect;
	if (g_state >= 0&&g_state <= 4)
	{

		FillRect(g_temphDc, &g_child_menu_rect[g_state], g_stone_brush);
		FrameRect(hdc, &g_child_menu_rect[g_state], g_gray_brush);
		SetTextColor(g_temphDc, BLACK);
		for (i = 0; i < 5; i++) if (g_child_menu_text[g_state ][i] != 0) menu_num++;
		for (i = 0; i < 5; i++)
		{
			if (g_child_menu_text[g_state][i] != 0)
			{                                                         
				menu_rect = g_child_menu_rect[g_state ];                
				set_rect(&text_rect, menu_rect.left, menu_rect.top + H * i, menu_rect.right, menu_rect.top + H + H * i);
				MoveToEx(hdc, text_rect.left, text_rect.bottom, NULL);   
				LineTo(hdc, text_rect.right, text_rect.bottom);          
				if (abs(g_slcKey%menu_num) == i)                              
				{                                                        
					FillRect(g_temphDc, &text_rect, g_gray_brush);          
				}                                                        
				DrawText(g_temphDc, g_child_menu_text[g_state][i], -1, &text_rect, DT_CENTER | DT_VCENTER | DT_EXTERNALLEADING);
			}                                                         
		}                                                          
	}                                                           
}

//绘制线程
void paint_thread()
{
	while (TRUE)
	{
		Sleep(1000);
		draw();
	}
}


//************************************
//内部数据结构-链表实现存储
//
//is_empty(student_node*)
//add_node(student_node *header, student data)
//insert_node(student_node* header, int index, student data)
//del_node(student_node* header, int i)
//num_node(student_node *header)
//list_node(student_node *header)
//search_name(student_node* header, char* name, int* index)
//search_id(student_node* header, int id, int* index)
//************************************


int is_empty(student_node *header)
{
	if (header->next == NULL) return 1;
	return 0;
}

int add_node(student_node *header, student data)
{
	student_node *tmp_node = header;
	student_node* new_node;
	while (tmp_node->next != NULL)
	{
		tmp_node = tmp_node->next;
	}
	new_node = (student_node*)malloc(sizeof(student_node));
	tmp_node->next = new_node;
	new_node->data = data;
	new_node->prev = tmp_node;
	new_node->next = NULL;
	return 1;
}

student_node* visit_node()
{
	
	if (g_current ==NULL)
	{
		g_current =  g_header;
		return NULL;
	}
	else
	{
		g_current = g_current->next;
		return g_current;
	}
	
}

/*int insert_node(student_node* header, int index, student data)
{
	student_node*   tmp_node = header;
	student_node*	pre_node;
	student_node*	new_node;
	int count = 0;

	if (is_empty(header)) return 0;
	if (index > num_node(header)) return 0;

	while (tmp_node->next != NULL)
	{
		if (count == index - 1)
		{
			break;
		}
		tmp_node = tmp_node->next;
		count++;
	}
	pre_node = tmp_node;
	new_node = (student_node*)malloc(sizeof(student_node));
	tmp_node = tmp_node->next;
	pre_node->next = new_node;
	new_node->data = data;
	new_node->next = tmp_node;

	return 1;

}*/

int del_node(student_node* header, int i)
{
	
	student_node*   tmp_node = header;
	student_node* pre_node;
	int count = 0;
	if (is_empty(header)) return 0;

	while (tmp_node->next != NULL)
	{
		if (count == i - 1)
		{
			break;
		}
		tmp_node = tmp_node->next;
		count++;
	}
	pre_node = tmp_node;
	tmp_node = tmp_node->next;
	tmp_node->next->prev = pre_node;
	pre_node->next = tmp_node->next;
	free(tmp_node);
	return 1;
}

int num_node(student_node *header)
{
	student_node *tmp_node = header;
	int count = 0;
	while (tmp_node != NULL)
	{
		count++;
		tmp_node = tmp_node->next;
	}
	return count - 1;
}

int list_node(student_node *header)
{
	student_node *tmp_node = header->next;
	if (is_empty(header)) return 0;
	while (tmp_node != NULL)
	{
		printf(tmp_node->data.name);
		tmp_node = tmp_node->next;
	}
	return 1;
}

student_node* search_name(student_node* header, char* name, int* index)
{
	student_node* tmp_node = header->next;
	int count = 1;
	if (is_empty(header)) return 0;


	while (tmp_node != NULL)
	{
		if (!strcmp(tmp_node->data.name, name))
		{
			break;
		}
		tmp_node = tmp_node->next;
		count++;
	}
	if (index != NULL)*index = count;
	return tmp_node;
}

student_node* search_id(student_node* header, int id, int* index)
{
	student_node* tmp_node = header->next;
	int count = 1;
	if (is_empty(header)) return 0;

	while (tmp_node != NULL)
	{
		if (tmp_node->data.student_id == id)
		{
			break;
		}
		tmp_node = tmp_node->next;
		count++;
	}
	if (index != NULL)*index = count;
	return tmp_node;
}

//定位光标位置到指定坐标

void goto_xy(int x, int y)
{
	HANDLE hOut;
	COORD pos = { x,y };
	CONSOLE_CURSOR_INFO s;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);	
	GetConsoleCursorInfo(hOut, &s);
	SetConsoleCursorPosition(hOut, pos);
}
void max_node(student_node* header)
{
	student_node* tmp_node;
	student_node* pre_node;
	if (is_empty(header)) return;
	tmp_node = header->next;
	while (tmp_node->next)
	{
		pre_node = tmp_node;
		tmp_node = tmp_node->next;
		if (pre_node->data.student_id < tmp_node->data.student_id) swap_node(pre_node,tmp_node);
	}
}
void scores_node(student_node* header,int index)
{
	student_node* tmp_node;
	student_node* pre_node;
	if (is_empty(header)) return;
	tmp_node = header->next;
	while (tmp_node->next)
	{
		pre_node = tmp_node;
		tmp_node = tmp_node->next;
		if (pre_node->data.scores[index] < tmp_node->data.scores[index]) swap_node(pre_node, tmp_node);
	}
}
void swap_node(student_node* node1, student_node* node2)
{
	student tmp = node1->data;
	node1->data = node2->data;
	node2->data = tmp;
}

void set_rect(RECT *rec, int left, int top, int right, int bottom)
{
	rec->left = left;
	rec->top = top;
	rec->right = right;
	rec->bottom = bottom;
}
