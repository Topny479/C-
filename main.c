#include "research.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

    SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/

    LoadData();                   /*数据加载*/
    InitInterface();              /*界面初始化*/
    RunSys(&gp_head);             /*系统功能模块的选择及运行*/
    CloseSys(gp_head);            /*退出系统*/

    return 0;
}

/**
 * 函数名称: LoadData
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
 *           其他函数的返回值必须为TRUE.
 *
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为FALSE时, 结束程序运行.
 */
BOOL LoadData()
{
    int Re = 0;

    Re = CreatList(&gp_head);

    if (Re < 4 + 8 + 16)
    {
        /*数据加载提示信息*/
        printf("\n系统基础数据不完整!\n");
    }

    printf("\n按任意键进入用户界面\n");
    getch();

    return TRUE;
}

/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InitInterface()
{
    WORD att = FOREGROUND_INTENSITY | FOREGROUND_BLUE
               | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;  /*蓝色前景和白色背景*/

    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}

/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    }
    while (i<5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}

/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu = {0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
    {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*去除选中菜单项前面的菜单项选中标记*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*在选中菜单项上做标记，黑底白字*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*去除选中菜单项后面的菜单项选中标记*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void CloseSys(DEPT_NODE *hd)
{
    DEPT_NODE *pDeptNode1 = hd, *pDeptNode2;
    TEAM_NODE *pTeamNode1, *pTeamNode2;
    PROG_NODE *pProgNode1, *pProgNode2;

    while (pDeptNode1 != NULL) /*释放十字交叉链表的动态存储区*/
    {
        pDeptNode2 = pDeptNode1->next;
        pTeamNode1 = pDeptNode1->tnext;
        while (pTeamNode1 != NULL) /*释放科研团队信息支链的动态存储区*/
        {
            pTeamNode2 = pTeamNode1->next;
            pProgNode1 = pTeamNode1->pnext;

            while (pProgNode1 != NULL) /*释放科研项目信息支链的动态存储区*/
            {
                pProgNode2 = pProgNode1->next;
                free(pProgNode1);
                pProgNode1 = pProgNode2;
            }

            free(pTeamNode1);
            pTeamNode1 = pTeamNode2;
        }

        free(pDeptNode1);  /*释放主链结点的动态存储区*/
        pDeptNode1 = pDeptNode2;
    }

    ClearScreen();        /*清屏*/

    /*释放存放菜单条、状态条、项目类别代码等信息动态存储区*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(DEPT_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
            {
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            }
            else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
            {
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
                    {
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                }
                else /*层号不为0，表明该位置被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
                    {
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) /*如果按下F1键*/
            {
                if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            {
                /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                    case 88:  /*Alt+X 退出*/
                        if (gp_top_layer->LayerNo != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        bRet = ExeFunction(1, 4);
                        break;
                    case 70:  /*Alt+F*/
                        PopMenu(1);
                        break;
                    case 77: /*Alt+M*/
                        PopMenu(2);
                        break;
                    case 81: /*Alt+Q*/
                        PopMenu(3);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(4);
                        break;
                    case 72: /*Alt+H*/
                        PopMenu(5);
                        break;
                }
            }
            else if (asc == 0) /*其他控制键的处理*/
            {
                if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu == 0)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu == 6)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*已弹出子菜单时*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = ga_sub_menu_count[gi_sel_menu-1];
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 5)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > ga_sub_menu_count[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32))   /*按下普通键*/
            {
                if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                    case 70: /*f或F*/
                        PopMenu(1);
                        break;
                    case 77: /*m或M*/
                        PopMenu(2);
                        break;
                    case 81: /*q或Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s或S*/
                        PopMenu(4);
                        break;
                    case 72: /*h或H*/
                        PopMenu(5);
                        break;
                    case 13: /*回车*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            {
                                /*如果匹配成功*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*串长为0，表明为空串*/
        {
            /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &num_written);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }

    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }

    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }

    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {
        /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                         | (pHotArea->pTag[i] << 2);
        }
    }

    return;
}

/**
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {
        /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }

    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }

    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;

    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+', '-', '|'};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {
        /*此循环画边框左边线和右边线*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*画下边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/

    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {
        /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {
        /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {
        /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: DealInput
 * 函数功能: 在弹出窗口区域设置热区, 等待并相应用户输入.
 * 输入参数: pHotArea
 *           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
 * 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
 * 返 回 值:
 *
 * 调用说明:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    CONSOLE_CURSOR_INFO lpCur;
    DWORD res;
    COORD pos = {0, 0};
    int iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc;       /*vkc:虚拟键代码*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
                (inRec.Event.MouseEvent.dwButtonState
                 == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                /*如果点在热区上面*/
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = cTag;
                    return iRet;
                }
            }
            else if(cTag == 0)
            {
                /* 如果点在非热点区域 */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
            }
        }
        /*如果是键盘输入*/
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;

            switch (vkc)
            {
            case 37 :/* 处理上、左键 */
            case 38 :
                --*piHot;
                if(*piHot < 1)
                {
                    *piHot = pHotArea->num;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 13 :/*回车键或空格表示按下当前按钮*/
            case 32 :
                if(pHotArea->pSort[*piHot-1] == 0)
                {
                    //若为按钮
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
                    iRet = *piHot;
                    return iRet;
                }
            case 39 :/* 处理右、下键 */
            case 40 :
                ++*piHot;
                if(*piHot > pHotArea->num)
                {
                    *piHot = 1;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 27 :/* 按ESC键 */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
                iRet = 27;
                return iRet;
            }
        }
    }
    return iRet;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2;
    int i, width;

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    for (i=0; i<pHotArea->num; i++)
    {
        /*将按钮类热区置为白底黑字*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {
            /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {
        /*被激活热区是按钮类*/
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);    /*隐藏光标*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {
        /*被激活热区是文本框类*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 *
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
    pFunction[0] = SaveData;
    pFunction[1] = ExitSys;
    pFunction[2] = MaintainDeptInfo;
    pFunction[3] = MaintainTeamInfo;
    pFunction[4] = MaintainProgInfo;
    pFunction[5] = QueryDeptInfo;
    pFunction[6] = QueryTeamInfo;
    pFunction[7] = QueryProgInfo;
    pFunction[8] = StatDept;
    pFunction[9] = StatTeam;
    pFunction[10] = StatProg;
    pFunction[11] = HelpTopic;
    pFunction[12] = NULL;
    pFunction[13] = AboutResearch;

    for (i=1,loc=0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}

/**
 * 函数名称: SaveSysData
 * 函数功能: 保存系统代码表和三类基础数据.
 * 输入参数: hd 主链头结点指针
 * 输出参数:
 * 返 回 值: BOOL类型
 *
 * 调用说明:
 */
BOOL SaveSysData(DEPT_NODE *hd)
{
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    FILE *pfout;

    pfout = fopen(gp_dept_info_filename, "wb");
    if(!pfout)
    {
        printf("院系文件保存失败！\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*保存院系信息*/
        fwrite(pdept_node, sizeof(DEPT_NODE), 1, pfout);
    }
    fclose(pfout);

    pfout = fopen(gp_team_info_filename, "wb");
    if(!pfout)
    {
        printf("科研团队文件保存失败！\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*保存科研团队信息*/
        pteam_node = pdept_node->tnext;
        while(pteam_node != NULL)
        {
            fwrite(pteam_node, sizeof(TEAM_NODE), 1, pfout);
            pteam_node = pteam_node->next;
        }
    }
    fclose(pfout);

    pfout = fopen(gp_prog_info_filename, "wb");
    if(!pfout)
    {
        printf("科研项目文件保存失败！\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*保存科研项目信息*/
        pteam_node = pdept_node->tnext;
        while(pteam_node != NULL)
        {
            pprog_node = pteam_node->pnext;
            while(pprog_node != NULL)
            {
                fwrite(pprog_node, sizeof(PROG_NODE), 1, pfout);
                pprog_node = pprog_node->next;
            }
            pteam_node = pteam_node->next;
        }
    }

    fclose(pfout);

    return TRUE;
}

/**
 * 函数名称: CreatList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载院系信息数据，无科研团队信息和科研项目信息数据
 *           12 已加载院系信息和科研团队信息数据，无科研项目信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
 */
int CreatList(DEPT_NODE **phead)
{
    DEPT_NODE *hd = NULL, *pDeptNode, tmp1;
    TEAM_NODE *pTeamNode, tmp2;
    PROG_NODE *pProgNode, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_dept_info_filename, "rb")) == NULL)
    {
        printf("院系信息数据文件打开失败!\n");
        return re;
    }
    printf("院系信息数据文件打开成功!\n");

    /*每次读取大小为一个DEPT_NODE所占内存的院系信息结点*/
    while (fread(&tmp1, sizeof(DEPT_NODE), 1, pFile) == 1)
    {
        pDeptNode = (DEPT_NODE *)malloc(sizeof(DEPT_NODE));
        *pDeptNode = tmp1;
        pDeptNode->tnext = NULL;
        pDeptNode->next = hd;
        hd = pDeptNode;
    }
    fclose(pFile);
    if (hd == NULL)
    {
        printf("院系信息数据文件加载失败!\n");
        return re;
    }
    printf("院系信息数据文件加载成功!\n");
    *phead = hd;
    re = re + 4;

    if ((pFile = fopen(gp_team_info_filename, "rb")) == NULL)
    {
        printf("科研团队基本信息数据文件打开失败!\n");
        return re;
    }
    printf("科研团队基本信息数据文件打开成功!\n");
    re = re + 8;

    /*从数据文件中读取科研团队基本信息数据，存入主链对应结点的科研团队基本信息支链中*/
    while (fread(&tmp2, sizeof(TEAM_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的科研团队基本信息*/
        pTeamNode = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
        *pTeamNode = tmp2;
        pTeamNode->pnext = NULL;

        /*在主链上查找该团队所在院系对应的主链结点*/
        pDeptNode = hd; /*位移指针pDeptNode定位到链表头部*/
        while (pDeptNode != NULL
                && strcmp(pDeptNode->dept_name, pTeamNode->dept_name) != 0)
        {
            pDeptNode = pDeptNode->next;
        }
        if (pDeptNode != NULL) /*如果找到，则将结点以后进先出方式插入团队信息支链*/
        {
            pTeamNode->next = pDeptNode->tnext;
            pDeptNode->tnext = pTeamNode;
        }
        else  /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pTeamNode);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_prog_info_filename, "rb")) == NULL)
    {
        printf("科研项目信息数据文件打开失败!\n");
        return re;
    }
    printf("科研项目信息数据文件打开成功!\n");
    re = re + 16;

    /*从数据文件中读取科研项目信息数据，存入科研团队基本信息支链对应结点的缴费支链中*/
    while (fread(&tmp3, sizeof(PROG_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的科研项目信息*/
        pProgNode = (PROG_NODE *)malloc(sizeof(PROG_NODE));
        *pProgNode = tmp3;

        /*查找学生信息支链上对应院系信息结点*/
        pDeptNode = hd;
        find = 0;
        while (pDeptNode != NULL && find == 0)
        {
            pTeamNode = pDeptNode->tnext;
            while (pTeamNode != NULL && find == 0)
            {
                if (strcmp(pTeamNode->team_name, pProgNode->team_name) == 0)
                {
                    find = 1;
                    break;
                }
                pTeamNode = pTeamNode->next;
            }
            pDeptNode = pDeptNode->next;
        }
        if (find)  /*如果找到，则将结点以后进先出方式插入科研项目信息支链中*/
        {
            pProgNode->next = pTeamNode->pnext;
            pTeamNode->pnext = pProgNode;
        }
        else /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pProgNode);
        }
    }
    fclose(pFile);

    return re;
}

/**
 * 函数名称: SeekTeamNode
 * 函数功能: 按团队名称(无重名)查找学生基本信息结点.
 * 输入参数: hd 主链头指针
 *           team_name 团队名称
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明:
 */
TEAM_NODE *SeekTeamNode(DEPT_NODE *hd, char *team_name)
{
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    int find = 0;

    for (pdept_node=hd; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        pteam_node = pdept_node->tnext;
        for(; pteam_node!=NULL; pteam_node=pteam_node->next)
        {
            if (strcmp(team_name, pteam_node->team_name) == 0)
            {
                find = 1;
                break;
            }
        }
        if (find)
        {
            break;
        }
    }
    if (find)
    {
        return pteam_node;
    }
    else
    {
        return NULL;
    }
}

/**
 * 函数名称: SeekDeptNode
 * 函数功能: 按院系名称(无重名)查找学生基本信息结点.
 * 输入参数: hd 主链头指针
 *           dept_name 院系名称
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明: 作为InsertDeptNode的子函数调用
 */
DEPT_NODE *SeekDeptNode(DEPT_NODE *hd, char *dept_name)
{
    DEPT_NODE *pdept_node;
    int find = 0;

    for (pdept_node=hd; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        if (strcmp(dept_name, pdept_node->dept_name) == 0)
        {
            find = 1;
            break;
        }
    }
    if (find)
    {
        return pdept_node;
    }
    else
    {
        return NULL;
    }
}

/**
 * 函数名称: SeekProgNode
 * 函数功能: 按和缴费日期查找缴费信息结点.
 * 输入参数: hd 主链头指针
 *           team_name 所属团队
 *           prog_num 项目编号
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明:
 */
PROG_NODE *SeekProgNode(DEPT_NODE *hd, char *team_name, char *prog_num)
{
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    int find = 0;
    pteam_node = SeekTeamNode(hd, team_name);/*首先查找满足团队信息的支链*/

    /*如果找到，则在科研项目支链上继续查找指定的项目结点*/
    if (pteam_node != NULL)
    {
        pprog_node = pteam_node->pnext;
        while (pprog_node != NULL)
        {
            if (strcmp(pprog_node->prog_num, prog_num)==0)
            {
                find = 1;
                break;
            }
            pprog_node = pprog_node->next;
        }
        if (find)
        {
            return pprog_node;
        }
        else
        {
            return NULL;
        }
    }

    return NULL;
}

/**
 * 函数名称: DelProgNode
 * 函数功能: 从十字链表中删除指定的科研项目信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明: 根据项目编号和所属团队可以确定唯一的缴费信息
 */
BOOL DelProgNode(DEPT_NODE *hd)
{
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node_prior;
    PROG_NODE *pprog_node_current;
    char prog_num[15];
    char team_name[30];

    gotoxy(28, 6);
    printf("请输入您要删除的项目信息的项目编号：");
    gotoxy(64, 6);
    scanf("%s", prog_num);
    getchar();

    gotoxy(28, 7);
    printf("请输入您要删除的项目信息所属团队：");
    gotoxy(62, 7);
    scanf("%s", team_name);
    getchar();

    pteam_node = SeekTeamNode(hd, team_name);
    if (pteam_node != NULL)/*查找科研项目支链上给定项目编号的科研项目结点*/
    {
        pprog_node_prior = NULL;
        pprog_node_current = pteam_node->pnext;
        while (pprog_node_current != NULL
                && strcmp(pprog_node_current->prog_num, prog_num) != 0)
        {
            pprog_node_prior = pprog_node_current;
            pprog_node_current = pprog_node_current->next;
        }

        if (pprog_node_current != NULL)
        {
            if (pprog_node_prior == NULL) /*位于第一个结点的位置*/
            {
                pteam_node->pnext = pprog_node_current->next;
            }
            else
            {
                pprog_node_prior->next = pprog_node_current->next;
            }
            free(pprog_node_current);
        }
        else
        {
            char *warn0[] = {"该团队没有项目 ",
                             "删除基本信息失败 ",
                             "确认"
                            };
            ClearScreen();
            ShowModule(warn0, 3);
            ShowMenu();
            return FALSE;
        }
    }
    else
    {
        char *warn1[] = {"不存在这样的结点 ",
                         "删除基本信息失败 ",
                         "确认"
                        };

        ClearScreen();
        ShowModule(warn1, 3);
        ShowMenu();

        return FALSE;
    }

    char *sucinfo[] = {"删除成功,信息已保存",
                       "确认"
                       };
    SaveSysData(gp_head);
    ShowModule(sucinfo, 2);
    ClearScreen();
    ShowMenu();

    return TRUE;
}

/**
 * 函数名称: DelTeamNode
 * 函数功能: 从十字链表中删除指定的科研团队信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明: 根据团队名称和所属院系可以确定唯一的缴费信息
 */
BOOL DelTeamNode(DEPT_NODE *hd)
{
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node_prior;
    TEAM_NODE *pteam_node_current;
    char team_name[30];
    char dept_name[20];
    PROG_NODE *pprog_node_prior;
    PROG_NODE *pprog_node_current;

    char *notice[] = {"删除成功，信息已保存",
                      "确认"
                     };

    gotoxy(28, 6);
    printf("请输入您要删除的团队信息的名称：");
    gotoxy(60, 6);
    scanf("%s", team_name);
    getchar();

    gotoxy(28, 7);
    printf("请输入您要删除的团队信息所属院系：");
    gotoxy(62, 7);
    scanf("%s", dept_name);
    getchar();

    pdept_node = SeekDeptNode(hd, dept_name);
    if (pdept_node != NULL)/*查找科研项目支链上给定项目编号的科研项目结点*/
    {
        pteam_node_prior = NULL;
        pteam_node_current = pdept_node->tnext;
        while (pteam_node_current != NULL
                && strcmp(pteam_node_current->team_name, team_name) != 0)
        {
            pteam_node_prior = pteam_node_current;
            pteam_node_current = pteam_node_current->next;
        }

        if (pteam_node_current != NULL)
        {
            if (pteam_node_prior == NULL)/*位于第一个结点的位置*/
            {
                pdept_node->tnext = pteam_node_current->next;
            }
            else
            {
                pteam_node_prior->next = pteam_node_current->next;
            }
            pprog_node_prior = NULL;
            pprog_node_current = pteam_node_current->pnext;
            MakeDel();/*使用户再次确认*/
            while (pprog_node_current != NULL) /*删除团队链的子链*/
            {
                pprog_node_prior = pprog_node_current;
                pprog_node_current = pprog_node_current->next;
                free(pprog_node_prior);
            }
            free(pteam_node_current);
        }
        else
        {
            char *warn1[] = {"不存在这样的结点 ",
                             "删除基本信息失败 ",
                             "确认"
                            };
            ShowModule(warn1, 3);
            ClearScreen();
            ShowMenu();
            return FALSE;
        }
    }
    else
    {
        char *warn1[] = {"不存在这样的结点 ",
                         "删除基本信息失败 ",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        ShowMenu();
        return FALSE;
    }
    SaveSysData(gp_head);
    ShowModule(notice, 2);
    ClearScreen();
    ShowMenu();

    return TRUE;
}

/**
 * 函数名称: DelDeptNode
 * 函数功能: 从十字链表中删除指定的科研团队信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明: 根据院系名称可以确定唯一的院系信息
 */
BOOL DelDeptNode(DEPT_NODE *hd)
{
    DEPT_NODE *pdept_node_current;
    DEPT_NODE *pdept_node_prior;
    TEAM_NODE *pteam_node_prior;
    TEAM_NODE *pteam_node_current;
    char dept_name[20];
    PROG_NODE *pprog_node_prior;
    PROG_NODE *pprog_node_current;
    char *notice[] = {"删除成功，信息已保存",
                      "确认"
                     };

    gotoxy(28, 6);
    printf("请输入您要删除的院系的名称：");
    gotoxy(56, 6);
    scanf("%s", dept_name);
    getchar();

    pdept_node_prior = NULL;
    pdept_node_current = hd;
    while (pdept_node_current != NULL
            && strcmp(pdept_node_current->dept_name, dept_name) != 0)
    {
        pdept_node_prior = pdept_node_current;
        pdept_node_current = pdept_node_current->next;
    }

    if (pdept_node_current != NULL)/*查找科研项目支链上给定项目编号的科研项目结点*/
    {
        pteam_node_prior = NULL;
        pteam_node_current = pdept_node_current->tnext;
        if (pdept_node_prior == NULL) /*位于第一个结点的位置*/
        {
            gp_head = pdept_node_current -> next;
        }
        else
        {
            pdept_node_prior->next = pdept_node_current->next;
        }
        MakeDel();/*使用户再次确认*/
        while (pteam_node_current != NULL)
        {
            pteam_node_prior = pteam_node_current;
            pteam_node_current = pteam_node_current->next;
            pprog_node_prior = NULL;
            pprog_node_current = pteam_node_prior->pnext;
            while (pprog_node_current != NULL)
            {
                pprog_node_prior = pprog_node_current;
                pprog_node_current = pprog_node_current->next;
                free(pprog_node_prior);
            }
            free(pteam_node_prior);
        }
        free(pdept_node_current);
    }
    else
    {
        char *warn1[] = {"不存在这样的院系 ",
                         "删除基本信息失败 ",
                         "确认"
                        };
        ShowModule(warn1, 3);
        ClearScreen();
        ShowMenu();
        return FALSE;
    }
    SaveSysData(gp_head);
    ShowModule(notice, 2);
    ClearScreen();
    ShowMenu();

    return TRUE;
}

/**
 * 函数名称: MakeDel
 * 函数功能: 提醒用户是否需要删除
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示确认删除, FALSE表示取消删除
 *
 * 调用说明:
 */
void MakeDel()
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"是否要删除当前数据？",
                   "  确定   取消"
                  };
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;           /*与矩形区域左右边框位置相关*/
    pos.Y = 7;                             /*与矩形区域上下边框位置相关*/
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;/*pCh的位置*/

    areas.num = 2;
    SMALL_RECT aArea[] = {{
            rcPop.Left + 7, rcPop.Top + 5,
            rcPop.Left + 10, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top + 5,
            rcPop.Left + 17, rcPop.Top + 5
        }
    };                                         /*黑色选中框大小及位置*/
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);  /*分割线*/

    if (DealInput(&areas, &iHot) == 1 && iHot == 1)
    {
        PopOff();
        char *plabel_name1[] = {"数据已经删除！",
                                "确认"
                               };

        ShowModule(plabel_name1, 2);
    }
    else
    {
        PopOff();
    }
    ClearScreen();
    ShowMenu();
    return;
}

/**
 * 函数名称: ModifProgNode
 * 函数功能: 对指定的项目信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifProgNode(DEPT_NODE *hd)
{
    PROG_NODE *pprog_node_temp;/*用于作为原来的被修改结点，被pprog_node替代*/
    PROG_NODE *pprog_node_next;/*用于暂时存放指针域*/
    PROG_NODE *pprog_node = (PROG_NODE *)malloc(sizeof(PROG_NODE));

    gotoxy(28, 6);
    printf("请输入您要修改的项目信息的项目编号：");
    gotoxy(32, 8);
    scanf("%s", pprog_node->prog_num);
    getchar();

    gotoxy(28, 10);
    printf("请输入您要修改的项目信息所属团队：");
    gotoxy(32, 12);
    scanf("%s", pprog_node->team_name);
    getchar();

    pprog_node_temp = SeekProgNode(hd, pprog_node->team_name, pprog_node->prog_num);/*查找指定科研项目结点*/
    if (pprog_node_temp != NULL)
    {
        gotoxy(28, 14);
        printf("负责人姓名：");
        gotoxy(40, 14);
        scanf("%s", pprog_node->pmgr_name);
        getchar();

        gotoxy(28, 16);
        printf("项目类别：");
        gotoxy(38, 16);
        scanf("%c", &pprog_node->type);
        getchar();

        gotoxy(28, 18);
        printf("项目经费: ");
        gotoxy(38, 18);
        scanf("%f", &pprog_node->prog_fee);
        getchar();

        gotoxy(28, 20);
        printf("起始时间：");
        gotoxy(38, 20);
        scanf("%s", pprog_node->st_date);
        getchar();

        gotoxy(28, 22);
        printf("项目编号：");
        gotoxy(38, 22);
        scanf("%s", pprog_node->prog_num);
        getchar();

        pprog_node_next = pprog_node_temp->next;/*保存指针域*/
        *pprog_node_temp = *pprog_node;
        pprog_node_temp->next = pprog_node_next;
        free(pprog_node);

        char *sucinfo[] = {"修改成功，信息已保存",
                           "确认"
                          };
        SaveSysData(gp_head);
        ShowModule(sucinfo, 2);
        ClearScreen();
        ShowMenu();
        return TRUE;
    }
    else
    {
        char *warn1[] = {"此项目不存在 ",
                         "插入基本信息失败 ",
                         "确认"
                        };

        ClearScreen();
        ShowModule(warn1, 3);
        free(pprog_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * 函数名称: ModifTeamNode
 * 函数功能: 对指定的团队信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifTeamNode(DEPT_NODE *hd)
{
    TEAM_NODE *pteam_node_temp;/*用于作为pteam_node的替代*/
    TEAM_NODE *pteam_node_next;/*用于暂时存放指针域*/
    TEAM_NODE *pteam_node = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
    PROG_NODE *pteam_node_pnext;

    gotoxy(29, 6);
    printf("请输入您要修改的团队信息的团队名称：");
    gotoxy(32, 8);
    scanf("%s", pteam_node->team_name);
    getchar();

    pteam_node_temp = SeekTeamNode(hd, pteam_node->team_name);/*查找指定科研项目结点*/
    if (pteam_node_temp != NULL)
    {
        gotoxy(29, 10);
        printf("团队名称：");
        gotoxy(40, 10);
        scanf("%s", pteam_node->team_name);
        getchar();

        gotoxy(29, 12);
        printf("教师人数：");
        gotoxy(40, 12);
        scanf("%d", &pteam_node->team_prof_amount);
        getchar();

        gotoxy(29, 14);
        printf("研究生人数: ");
        gotoxy(42, 14);
        scanf("%d", &pteam_node->team_grd_amount);
        getchar();

        gotoxy(29, 16);
        printf("团队负责人: ");
        gotoxy(41, 16);
        scanf("%s", pteam_node->tmgr_name);
        getchar();

        gotoxy(29, 18);
        printf("所属院系：");
        gotoxy(38, 18);
        scanf("%s", pteam_node->dept_name);
        getchar();

        pteam_node_next = pteam_node_temp->next;/*保存指针域*/
        pteam_node_pnext = pteam_node_temp->pnext;/*保存指向项目链的指针域*/
        *pteam_node_temp = *pteam_node;
        pteam_node_temp->next = pteam_node_next;
        pteam_node_temp->pnext = pteam_node_pnext;
        free(pteam_node);

        char *sucinfo[] = {"修改成功，信息已保存",
                           "确认 "
                          };
        SaveSysData(gp_head);
        ShowModule(sucinfo, 2);
        ClearScreen();
        ShowMenu();
        return TRUE;
    }
    else
    {
        char *warn1[] = {"此团队不存在 ",
                         "插入基本信息失败 ",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pteam_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * 函数名称: ModifDeptNode
 * 函数功能: 对指定的团队信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifDeptNode(DEPT_NODE *hd)
{
    DEPT_NODE *pdept_node_temp;/*用于作为pteam_node的替代*/
    DEPT_NODE *pdept_node_next;/*用于暂时存放指针域*/
    DEPT_NODE *pdept_node = (DEPT_NODE *)malloc(sizeof(DEPT_NODE));
    TEAM_NODE *pdept_node_tnext;

    gotoxy(28, 6);
    printf("请输入您要修改的院系的名称：");
    gotoxy(56, 6);
    scanf("%s", pdept_node->dept_name);
    getchar();

    pdept_node_temp = SeekDeptNode(hd, pdept_node->dept_name);/*查找指定院系结点*/
    if (pdept_node_temp != NULL)
    {
        gotoxy(28, 8);
        printf("负责人姓名：");
        gotoxy(40, 8);
        scanf("%s", pdept_node->dmgr_name);/*输入负责人姓名*/
        getchar();

        gotoxy(28, 10);
        printf("联系电话：");
        gotoxy(40, 10);
        scanf("%s", pdept_node->telno);/*输入联系电话*/
        getchar();

        pdept_node_next = pdept_node_temp->next;/*保存指针域*/
        pdept_node_tnext = pdept_node_temp->tnext;/*保存指向团队链的指针域*/
        *pdept_node_temp = *pdept_node;
        pdept_node_temp->next = pdept_node_next;
        pdept_node_temp->tnext = pdept_node_tnext;

        char *sucinfo[] = {"修改成功，信息已保存",
                           "确认 "
                          };
        SaveSysData(gp_head);
        ShowModule(sucinfo, 2);
        ClearScreen();
        ShowMenu();
        free(pdept_node);
        return TRUE;
    }
    else
    {
        char *warn1[] = {"此院系不存在 ",
                         "插入基本信息失败 ",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pdept_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * 函数名称: RemoveSysData
 * 函数功能: 从指定数据文件中恢复三类基础数据.
 * 输入参数: phead 主链头结点指针的地址
 *           filename 存放备份数据的数据文件名
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
 */
BOOL RemoveSysData(DEPT_NODE **phead, char *filename)
{
    DEPT_NODE *hd = NULL;
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    unsigned long dept_node_num = 0;
    unsigned long team_node_num = 0;
    unsigned long prog_node_num = 0;
    unsigned long ulloop;
    int handle;
    int find;

    if((handle = open(filename,O_RDONLY | O_BINARY)) == -1)
    {
        handle = open(filename,O_CREAT | O_BINARY,S_IREAD);
    }

    /*读取三种基本数据信息的记录数*/
    read(handle, (char *)&dept_node_num, sizeof(dept_node_num));
    read(handle, (char *)&team_node_num, sizeof(team_node_num));
    read(handle, (char *)&prog_node_num, sizeof(prog_node_num));

    /*读取院系信息，建立十字主链*/
    for(ulloop=1; ulloop<=dept_node_num; ulloop++)
    {
        pdept_node = (DEPT_NODE *)malloc(sizeof(DEPT_NODE));
        read(handle, (char *)pdept_node, sizeof(DEPT_NODE));
        pdept_node -> tnext = NULL;
        pdept_node -> next = hd;
        hd = pdept_node;
    }
    *phead = hd;

    /*读取科研团队基本信息，建立科研团队支链*/
    for(ulloop=1; ulloop<=team_node_num; ulloop++)
    {
        pteam_node = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
        read(handle, (char *)pteam_node, sizeof(TEAM_NODE));
        pteam_node -> pnext = NULL;
        pdept_node = hd;
        while(pdept_node != NULL
                && strcmp(pdept_node -> dept_name, pteam_node->dept_name))
        {
            pdept_node = pdept_node -> next;
        }

        if(pdept_node != NULL)
        {
            pteam_node -> next = pdept_node -> tnext;
            pdept_node -> tnext = pteam_node;
        }
        else
        {
            free(pteam_node);
        }
    }

    /*读取科研项目信息，建立科研项目信息链表*/
    for(ulloop=1; ulloop<=prog_node_num; ulloop++)
    {
        pprog_node = (PROG_NODE *)malloc(sizeof(PROG_NODE));
        read(handle, (char *)pprog_node, sizeof(PROG_NODE));
        pdept_node = hd;
        find = 0;

        while(pdept_node != NULL && find == 0)
        {
            pteam_node = pdept_node -> tnext;
            while(pteam_node != NULL && find == 0)
            {
                if(strcmp(pteam_node->team_name, pprog_node->team_name) == 0)
                {
                    find = 1;
                    break;
                }
                pteam_node = pteam_node -> next;
            }
            pdept_node = pdept_node -> next;
        }

        if(find)
        {
            pprog_node -> next = pteam_node -> pnext;
            pteam_node -> pnext = pprog_node;
        }
        else
        {
            free(pprog_node);
        }
    }

    close(handle);
    SaveSysData(hd);    /*将内存中数据保存到数据文件*/

    return TRUE;
}

BOOL ShowModule(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 1;
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}

/**
 * 函数名称: SaveData
 * 函数功能:
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
 */
BOOL SaveData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"按确认键保存数据",
                           "确认"
                          };

    ShowModule(plabel_name, 2);
    SaveSysData(gp_head);
    char *plabel_name1[] = {"数据已保存",
                            "返回"
                           };

    ShowModule(plabel_name1, 2);

    return bRet;
}

/**
 * 函数名称: ExitSys
 * 函数功能: 清屏后退出系统
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示退出成功, FALSE表示退出失败
 *
 * 调用说明:
 */
BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"是否需要退出系统？", "确定    取消"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top + 5
        },
        {
            rcPop.Left + 13, rcPop.Top + 5,
            rcPop.Left + 16, rcPop.Top + 5
        }
    };
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 1 && iHot == 1)
    {
        bRet = FALSE;
        PopOff();
    }
    else
    {
        PopOff();
        bRet = TRUE;
    }

    return bRet;
}

BOOL MaintainDeptInfo(void)
{
    char *pString[] = {"插入院系基本信息",
                       "修改院系基本信息",
                       "删除院系基本信息",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*选择要进行的操作*/
    switch (hotnum)
    {
    case 1:
        InsertDeptNode(gp_head);
        break;
    case 2:
        ModifDeptNode(gp_head);
        break;
    case 3:
        DelDeptNode(gp_head);
        break;
    case 4:
        PopOff();
        break;
    }
    return TRUE;
}

BOOL MaintainTeamInfo(void)
{
    char *pString[] = {"插入团队基本信息",
                       "修改团队基本信息",
                       "删除团队基本信息",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*选择要进行的操作*/
    switch (hotnum)
    {
    case 1:
        InsertTeamNode(gp_head);
        break;
    case 2:
        ModifTeamNode(gp_head);
        break;
    case 3:
        DelTeamNode(gp_head);
        break;
    case 4:
        PopOff();
        break;
    }

    return TRUE;
}

BOOL MaintainProgInfo(void)
{
    char *pString[] = {"插入项目基本信息",
                       "修改项目基本信息",
                       "删除项目基本信息",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*选择要进行的操作*/
    switch (hotnum)
    {
        case 1:
            InsertProgNode(gp_head);
            break;
        case 2:
            ModifProgNode(gp_head);
            break;
        case 3:
            DelProgNode(gp_head);
            break;
        case 4:
            PopOff();
            break;
    }

    return TRUE;
}

/**
 * 函数名称: MyMenu4
 * 函数功能: 将含有3个字符串与1个"返回"字符串的标签束设置成热区并等待用户输入
 * 输入参数: pString 指针，n标签个数
 * 输出参数: 无
 * 返 回 值: 用鼠标单击、按回车或空格时返回当前热区编号
 *
 * 调用说明:
 */
int MyMenu4(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len, x;

    /*找到最长的字符串以便于确定pop的大小*/
    for (i=0, maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    /*确定pop位置和大小*/
    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    /*确定 取消的位置*/
    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 4;
    SMALL_RECT aArea[] = {{
            aLoc[0].X, aLoc[0].Y,
            aLoc[0].X + maxlen - 1, aLoc[0].Y
        },
        {
            aLoc[1].X, aLoc[1].Y,
            aLoc[1].X + maxlen - 1, aLoc[1].Y
        },
        {
            aLoc[2].X, aLoc[2].Y,
            aLoc[2].X + maxlen - 1, aLoc[2].Y
        },
        {
            aLoc[3].X, aLoc[3].Y,
            aLoc[3].X + 3, aLoc[3].Y
        }
    };

    char aSort[] = {0, 0, 0, 0};
    char aTag[] = {1, 2, 3, 4};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &num_written);
    x = DealInput(&areas, &iHot);
    PopOff();
    return x;
}

/**
 * 函数名称: MyMenu3
 * 函数功能: 将含有2个字符串与1个"返回"字符串的标签束设置成热区并等待用户输入
 * 输入参数: pString 指针，n标签个数
 * 输出参数: 无
 * 返 回 值: 用鼠标单击、按回车或空格时返回当前热区编号
 *
 * 调用说明:
 */
int MyMenu3(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len,x;

    for (i=0, maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 3;
    SMALL_RECT aArea[] = {{
            aLoc[0].X, aLoc[0].Y,
            aLoc[0].X + maxlen - 1, aLoc[0].Y
        },
        {
            aLoc[1].X, aLoc[1].Y,
            aLoc[1].X + maxlen - 1, aLoc[1].Y
        },
        {
            aLoc[2].X, aLoc[2].Y,
            aLoc[2].X + 3, aLoc[2].Y
        }
    };

    char aSort[] = {0, 0, 0};
    char aTag[] = {1, 2, 3};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &num_written);
    x = DealInput(&areas, &iHot);
    PopOff();
    return x;
}

/**
 * 函数名称: gotoxy
 * 函数功能: 使光标定位到屏幕的某个点
 * 输入参数: x为横坐标，y为纵坐标
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void gotoxy(int x, int y)
{
    COORD pos;
    pos.X = x - 1;
    pos.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

/**
 * 函数名称: InsertDeptNode
 * 函数功能: 在三向十字链表中插入一个院系信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InsertDeptNode(DEPT_NODE* hd)
{
    DEPT_NODE *pdept_node;
    pdept_node = (DEPT_NODE*)malloc(sizeof(DEPT_NODE));

    gotoxy(25, 4);
    printf("请输入您所要添加的院系的基本信息：");

    gotoxy(29, 6);
    printf("院系名称：");
    gotoxy(39, 6);
    scanf("%s", pdept_node -> dept_name);/*输入院系名称*/
    getchar();

    if (strlen(pdept_node -> dept_name) > 20)
    {
        char *warn1[] = {"输入院系名称过长",
                         "插入院系信息失败",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        ShowMenu();
        free(pdept_node);

        return;
    }

    /*如果院系已经存在，则做出提示*/
    if (SeekDeptNode(hd, pdept_node->dept_name)!=NULL)
    {
        char *warn2[] = {"该院系已存在",
                         "如欲修改院系信息，请选择修改院系信息",
                         "确认"
                        };

        ShowModule(warn2, 3);
        ClearScreen();
        ShowMenu();
        free(pdept_node);

        return;
    }

    gotoxy(29, 8);
    printf("负责人姓名：");
    gotoxy(41, 8);
    scanf("%s", pdept_node->dmgr_name);/*输入负责人姓名*/
    getchar();

    gotoxy(29, 10);
    printf("联系电话：");
    gotoxy(39, 10);
    scanf("%s", pdept_node->telno);/*输入联系电话*/
    getchar();

    pdept_node->tnext = NULL;     /*将子链头结点置空*/
    pdept_node->next = hd;
    gp_head = pdept_node;         /*新建结点作为新的主链的头结点*/
    char *sucinfo[] = {"插入成功，信息已保存",
                       "确认"
                      };
    SaveSysData(gp_head);
    ShowModule(sucinfo, 2);
    ClearScreen();
    ShowMenu();

    return;
}

/**
 * 函数名称: InsertTeamNode
 * 函数功能: 在院系信息主链十字链表中插入团队基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InsertTeamNode(DEPT_NODE* hd)
{
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    pteam_node = (TEAM_NODE*)malloc(sizeof(TEAM_NODE));

    gotoxy(26, 4);
    printf("请输入您所要添加的团队基本信息：");

    gotoxy(30, 6);
    printf("所属院系: ");
    gotoxy(40, 6);
    scanf("%s", pteam_node -> dept_name);
    getchar();

    /*判断院系名称输入是否符合规范*/
    if(strlen(pteam_node->dept_name)>20)
    {
        char *warn2[] =  {"院系名称过长 ",
                          "插入基本信息失败 ",
                          "确认"
                         };

        ShowModule(warn2, 3);
        free(pteam_node);
        ClearScreen();
        ShowMenu();

        return;
    }

    /*在十字链中找到对应的院系信息结点*/
    pdept_node = SeekDeptNode(hd, pteam_node->dept_name);
    if(pdept_node == NULL)
    {
        char *warn1[] = {"此院系不存在 ",
                         "插入基本信息失败 ",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pteam_node);
        ShowMenu();

        return;
    }
    else
    {
        gotoxy(30, 8);
        printf("团队名称：");
        gotoxy(42, 8);
        scanf("%s", pteam_node->team_name);
        getchar();

        gotoxy(30, 10);
        printf("教师人数：");
        gotoxy(40, 10);
        scanf("%d", &pteam_node->team_prof_amount);
        getchar();

        gotoxy(30, 12);
        printf("研究生人数: ");
        gotoxy(42, 12);
        scanf("%d", &pteam_node->team_grd_amount);
        getchar();

        gotoxy(30, 14);
        printf("团队负责人: ");
        gotoxy(44, 14);
        scanf("%s", pteam_node->tmgr_name);
        getchar();

        pteam_node->pnext = NULL;
        pteam_node->next = pdept_node -> tnext;
        pdept_node->tnext = pteam_node;

        gotoxy(30, 16);
        printf("按任意键继续……\n");
        getch();

        char *sucinfo[] = {"插入成功，信息已保存",
                           "确认"
                          };
        SaveSysData(gp_head);
        ClearScreen();
        ShowMenu();
        ShowModule(sucinfo, 2);

        return;
    }
}

/**
 * 函数名称: InsertProgNode
 * 函数功能: 在团队信息链十字链表中插入项目基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InsertProgNode(DEPT_NODE* hd)
{
    PROG_NODE *pprog_node;
    TEAM_NODE *pteam_node;
    pprog_node = (PROG_NODE*)malloc(sizeof(PROG_NODE));

    gotoxy(26, 4);
    printf("请输入您所要添加的项目基本信息：");

    gotoxy(30, 6);
    printf("所属团队: ");
    gotoxy(40, 6);
    scanf("%s", pprog_node->team_name);
    getchar();

    /*判断团队名称输入是否符合规范*/
    if(strlen(pprog_node->team_name) > 30)
    {
        char *warn2[] =  {"科研团队名称过长 ",
                          "插入基本信息失败 ",
                          "确认"
                         };

        ShowModule(warn2, 3);
        free(pprog_node);
        ClearScreen();
        ShowMenu();

        return;
    }

    /*在十字链中找到对应的团队信息结点*/
    pteam_node = SeekTeamNode(hd, pprog_node->team_name);
    if(pteam_node == NULL)
    {
        char *warn1[] = {"此团队不存在 ",
                         "插入基本信息失败 ",
                         "确认"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pprog_node);
        ShowMenu();

        return;
    }
    else
    {
        gotoxy(30, 8);
        printf("负责人姓名：");
        gotoxy(42, 8);
        scanf("%s", pprog_node->pmgr_name);
        getchar();

        gotoxy(30, 10);
        printf("项目编号：");
        gotoxy(40, 10);
        scanf("%s", pprog_node->prog_num);
        getchar();

        gotoxy(30, 12);
        printf("项目经费: ");
        gotoxy(40, 12);
        scanf("%f", &pprog_node->prog_fee);
        getchar();

        gotoxy(30, 14);
        printf("项目类别: ");
        gotoxy(40, 14);
        scanf("%c", &pprog_node->type);
        getchar();

        /*判断项目类别名称输入是否符合规范*/
        if(pprog_node->type!='1' && pprog_node->type!='2' && pprog_node->type!='3' && pprog_node->type!='4' && pprog_node->type!='5')
        {
            char *warn3[] =  {"没有此项目类别 插入基本信息失败 ",
                              "确认"
                             };

            ShowModule(warn3, 2);
            free(pprog_node);
            ClearScreen();
            ShowMenu();

            return;
        }

        gotoxy(30, 16);
        printf("起始时间: ");
        gotoxy(40, 16);
        scanf("%s", pprog_node->st_date);
        getchar();

        pprog_node->next = pteam_node->pnext;
        pteam_node->pnext = pprog_node;

        gotoxy(30, 18);
        printf("按任意键继续……\n");
        getch();

        char *sucinfo[] = {"插入成功，信息已保存",
                           "确认"
                          };
        SaveSysData(gp_head);
        ClearScreen();
        ShowMenu();
        ShowModule(sucinfo, 2);

        return;
    }
}

BOOL QueryDeptInfo(void)
{
    char *pString[] = {"按院负责人查询",
                       "按院系名称关键字查询",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
    case 1:
        SeekDmgr();/*按院负责人查询*/
        break;
    case 2:
        SeekDeptPart();/*以院系名称的部分或全部为条件查找*/
        break;
    case 3:
        break;/*返回*/
    }

    return TRUE;
}


BOOL QueryTeamInfo(void)
{
    char *pString[] = {"按团队关键字查询",
                       "按教师人数查询",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
        case 1:
            SeekTeamPart();/*按团队关键字查询团队*/
            break;
        case 2:
            SeekTeamProf();/*根据教师人数查询满足条件的团队信息*/
            break;
        case 3:
            break;/*返回*/
    }

    return TRUE;
}


BOOL QueryProgInfo(void)
{
    char *pString[] = {"按项目编号查询",
                       "按所属团队查询",
                       "返回"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
        case 1:
            SeekProgNum();/*按项目编号查询*/
            break;
        case 2:
            SeekProgTeam();/*按所属团队查询*/
            break;
        case 3:
            break;/*返回*/
    }

    return TRUE;
}

/**
 * 函数名称: SeekDmgr
 * 函数功能: 根据输入的院负责人姓名（无重名）查找院系信息
 * 输入参数: head 主链头指针
 * 输出参数: 满足条件的院系信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekDmgr()
{
    char dmgr_name[12];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *find = head;
    int flag = 0;

    gotoxy(27, 2);
    printf("请输入院负责人姓名：");
    gotoxy(47, 2);
    scanf("%s", dmgr_name);
    getchar();
    printf("\n\t   院系名称\t\t院负责人\t联系电话");

    for(find=head; find!=NULL; find=find->next)
    {
        if(strcmp(find->dmgr_name, dmgr_name) == 0)
        {
            printf("\n");
            flag = 1;
            printf("\t   %-20s\t%-12s\t%-15s", find->dept_name, find->dmgr_name, find->telno);

        }
    }

    if(flag == 0)
    {
        printf("\n\t\t\t不存在满足条件的院系 ");
    }

    printf("\n\t\t\t\t按任意键继续……");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * 函数名称: SeekDeptPart
 * 函数功能: 根据输入的院系部分或全部名称查找院系信息
 * 输入参数: 无
 * 输出参数: 满足条件的院系信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekDeptPart()
{
    char dept_name[20];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *find = head;
    int flag = 0;    /*标记是否找到至少一个符合条件的院系*/

    gotoxy(27, 2);
    printf("输入院系名称关键字：");
    gotoxy(47, 2);
    scanf("%s", dept_name);
    getchar();
    printf("\n\t   院系名称\t\t院负责人\t联系电话");

    for(find=head; find!=NULL; find=find->next)
    {
        if(strstr(find->dept_name, dept_name))
        {
            printf("\n");
            flag = 1;
            printf("\t   %-20s\t%-12s\t%-15s", find->dept_name, find->dmgr_name, find->telno);
        }
    }

    if(!flag)
    {
        printf("\n\t\t\t不存在满足条件的院系……");
    }

    printf("\n\t\t\t按任意键继续……  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * 函数名称: SeekTeamPart
 * 函数功能: 根据输团队部分或全部名称查找院系信息
 * 输入参数: 无
 * 输出参数: 满足条件的团队信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekTeamPart()
{
    char team_name[20];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*标记是否找到至少一个符合条件的团队*/

    gotoxy(27, 2);
    printf("输入团队名称关键字：");
    gotoxy(47, 2);
    scanf("%s", team_name);
    getchar();
    printf("\n\t团队名称\t\t队负责人\t所属院系\t教师人数\t研究生人数");

    for(dept_sech=head; dept_sech!=NULL; dept_sech=dept_sech->next)
    {
        TEAM_NODE *team_sech = dept_sech->tnext;
        while (team_sech != NULL)
        {
            if(strstr(team_sech->team_name, team_name))
            {
                printf("\n");
                flag = 1;
                printf("\t%-20s\t%-12s\t%-16s%-16d%-8d", team_sech->team_name, team_sech->tmgr_name, team_sech->dept_name, team_sech->team_prof_amount, team_sech->team_grd_amount);
            }
            team_sech = team_sech->next;
        }
    }

    if(!flag)
    {
        printf("\n\t\t\t不存在满足条件的团队");
    }

    printf("\n\t\t\t\t按任意键继续……  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * 函数名称: SeekProgNum
 * 函数功能: 根据所输入的项目编号查找项目信息
 * 输入参数: 无
 * 输出参数: 满足条件的团队信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekProgNum()
{
    char prog_num[15];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*标记是否找到至少一个符合条件的团队*/
    char *prog_type;

    gotoxy(27, 2);
    printf("输入团队项目编号：");
    gotoxy(45, 2);
    scanf("%s", prog_num);
    getchar();
    printf("\n\t所属团队\t起始时间\t项目编号\t项目经费\t项目负责人\t项目类别\t\t");

    for(dept_sech=head; dept_sech!=NULL; dept_sech=dept_sech->next)
    {
        TEAM_NODE *team_sech = dept_sech->tnext;
        while (team_sech != NULL)
        {
            PROG_NODE *prog_sech = team_sech->pnext;
            while (prog_sech != NULL)
            {
                switch(prog_sech->type)
                {
                    case '1':
                        prog_type = "973计划";
                        break;
                    case '2':
                        prog_type = "国家自然科学基金项目";
                        break;
                    case '3':
                        prog_type = "863计划";
                        break;
                    case '4':
                        prog_type = "国际合作项目";
                        break;
                    case '5':
                        prog_type = "横向项目";
                        break;
                }
                if(!strcmp(prog_sech->prog_num, prog_num))
                {
                    printf("\n");
                    flag = 1;
                    printf("\t%-12s\t%-8s\t%-16s%-16.2f%-16s%-21s", prog_sech->team_name, prog_sech->st_date, prog_sech->prog_num, prog_sech->prog_fee, prog_sech->pmgr_name, prog_type);
                }
                prog_sech = prog_sech->next;
            }
            team_sech = team_sech->next;
        }
    }

    if(!flag)
    {
        printf("\n\t\t\t\t不存在满足条件的项目");
    }

    printf("\n\t\t\t\t按任意键继续……  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * 函数名称: SeekProgTeam
 * 函数功能: 根据所属团队查找项目信息
 * 输入参数: 无
 * 输出参数: 满足条件的项目信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekProgTeam()
{
    char team_name[15];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*标记是否找到至少一个符合条件的项目*/
    char *prog_type;

    gotoxy(27, 2);
    printf("输入项目所属团队：");
    gotoxy(45, 2);
    scanf("%s", team_name);
    getchar();
    printf("\n\t所属团队\t起始时间\t项目编号\t项目经费\t项目负责人\t项目类别\t\t");

    for(dept_sech=head; dept_sech!=NULL; dept_sech=dept_sech->next)
    {
        TEAM_NODE *team_sech = dept_sech->tnext;
        while (team_sech != NULL)
        {
            PROG_NODE *prog_sech = team_sech->pnext;
            while (prog_sech != NULL)
            {
                 switch(prog_sech->type)
                {
                    case '1':
                        prog_type = "973计划";
                        break;
                    case '2':
                        prog_type = "国家自然科学基金项目";
                        break;
                    case '3':
                        prog_type = "863计划";
                        break;
                    case '4':
                        prog_type = "国际合作项目";
                        break;
                    case '5':
                        prog_type = "横向项目";
                        break;
                }
                if(!strcmp(prog_sech->team_name, team_name))
                {
                    printf("\n");
                    flag = 1;
                    printf("\t%-12s\t%-8s\t%-16s%-16.2f%-16s%-21s", prog_sech->team_name, prog_sech->st_date, prog_sech->prog_num, prog_sech->prog_fee, prog_sech->pmgr_name, prog_type);
                }
                prog_sech = prog_sech->next;
            }
            team_sech = team_sech->next;
        }
    }

    if(!flag)
    {
        printf("\n\t\t\t\t……不存在满足条件的项目");
    }

    printf("\n\t\t\t按任意键继续……  ");
    getch();
    ClearScreen();
    ShowMenu();
}


/**
 * 函数名称: SeekTeamProf
 * 函数功能: 根据教师人数查找团队信息
 * 输入参数: 无
 * 输出参数: 满足条件的团队信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekTeamProf(void)
{
    char form_sign;
    int form_num;
    char form[10];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*标记是否找到至少一个符合条件的团队*/


    gotoxy(27, 2);
    printf("请输入教师人数查询公式，格式为‘>’,'<','='加上非负整数：");
    gotoxy(31, 3);
    scanf("%c%s", &form_sign, form);
    getchar();
    printf("\n\t团队名称\t\t队负责人\t所属院系\t教师人数\t研究生人数");
    form_num = atoi(form);

    if (form_sign!='>'&&form_sign!='<'&&form_sign!='=')
	{
	    printf("\n");
		printf("\t\t\t\t输入符号有误\n");
        printf("\n\t\t\t\t按任意键继续……  ");
        getch();
        ClearScreen();
        ShowMenu();
        return;
	}

	if (form_num<0)
    {
        printf("\n");
        printf("\t\t\t\t输入数字有误，请输入非负整数\n");
        getch();
        ClearScreen();
        ShowMenu();
        return;
    }

    for(dept_sech=head; dept_sech!=NULL; dept_sech=dept_sech->next)
    {
        TEAM_NODE *team_sech = dept_sech->tnext;
        while (team_sech != NULL)
        {
            switch (form_sign)
            {
                case '>':
                    if (team_sech->team_prof_amount>form_num)
                    {
                        printf("\n");
                        flag = 1;
                        printf("\t%-20s\t%-12s\t%-16s%-16d%-8d", team_sech->team_name, team_sech->tmgr_name, team_sech->dept_name, team_sech->team_prof_amount, team_sech->team_grd_amount);
                    }
                    break;
                case '<':
                    if (team_sech->team_prof_amount<form_num)
                    {
                        printf("\n");
                        flag = 1;
                        printf("\t%-20s\t%-12s\t%-16s%-16d%-8d", team_sech->team_name, team_sech->tmgr_name, team_sech->dept_name, team_sech->team_prof_amount, team_sech->team_grd_amount);
                    }
                    break;
                case '=':
                    if (team_sech->team_prof_amount==form_num)
                    {
                        printf("\n");
                        flag = 1;
                        printf("\t%-20s\t%-12s\t%-16s%-16d%-8d", team_sech->team_name, team_sech->tmgr_name, team_sech->dept_name, team_sech->team_prof_amount, team_sech->team_grd_amount);
                    }
                    break;
            }
            team_sech = team_sech->next;
        }
    }

    if(!flag)
    {
        printf("\n\t\t\t不存在满足条件的团队……");
    }

    printf("\n\t\t\t\t按任意键继续……  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* 函数名称: StatRateInfo
* 函数功能: 按类别统计所有院系的师生人数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 按要求输出统计结果
*
* 调用说明:
*/
void StatRateInfo(void)
{
    DEPT_NODE *head = gp_head;
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    RATE_NODE *prate_node_temp;
    RATE_NODE *prate_node_ret = NULL;

    for(pdept_node=head; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        int dept_prof_amount = 0, dept_grd_amount = 0;
        prate_node_temp = (RATE_NODE *)malloc(sizeof(RATE_NODE));

        pteam_node = pdept_node->tnext;
        while (pteam_node != NULL)
        {
            dept_grd_amount = dept_grd_amount + pteam_node->team_grd_amount;
            dept_prof_amount = dept_prof_amount + pteam_node->team_prof_amount;
            pteam_node = pteam_node->next;
        }

        strcpy(prate_node_temp->dept_name, pdept_node->dept_name);
        prate_node_temp->dept_grd_amount = dept_grd_amount;
        prate_node_temp->dept_prof_amount = dept_prof_amount;
        prate_node_temp->proportion = dept_grd_amount/1.0/dept_prof_amount;
        prate_node_temp->next = prate_node_ret;
        prate_node_ret = prate_node_temp;
    }

    SortRateInfo(prate_node_ret);

    gotoxy(28, 3);
    printf("下面按学生教师人数比输出院系信息：");
    printf("\n\t   院系名称\t研究生人数\t教师人数\t学生教师人数比");

    prate_node_temp = prate_node_ret;
    while(prate_node_temp!=NULL)
    {
        if(prate_node_temp->dept_prof_amount==0)
        {
            printf("\n");
            printf("\t   %-20s\t%-8d\t%-8d\t教师人数不可为0", prate_node_temp->dept_name, prate_node_temp->dept_grd_amount, prate_node_temp->dept_prof_amount);
            prate_node_temp = prate_node_temp->next;
            continue;
        }
        printf("\n");
        printf("\t   %-20s\t%-8d\t%-8d\t%-8.2f", prate_node_temp->dept_name, prate_node_temp->dept_grd_amount, prate_node_temp->dept_prof_amount, prate_node_temp->proportion);
        prate_node_temp = prate_node_temp->next;
    }

    printf("\n\t\t\t     按任意键继续……\n");
    getch();
    ClearScreen();
    ShowMenu();
    return;
}

/**
* 函数名称: StatRichProg
* 函数功能: 统计科研经费最高的项目，输出科研经费最高的10个项目
* 输入参数: 无
* 输出参数: 按要求输出统计结果
* 返 回 值: 无
*
* 调用说明:
*/
void StatRichProg(void)
{
    DEPT_NODE *head = gp_head;
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    PROG_FEE_NODE *pprog_fee_node_temp;
    PROG_FEE_NODE *pprog_fee_node_ret = NULL;
    int count = 0;

    for(pdept_node=head; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        pteam_node = pdept_node->tnext;
        while (pteam_node != NULL)
        {
            pprog_node = pteam_node->pnext;
            while (pprog_node != NULL)
            {
                pprog_fee_node_temp = (PROG_FEE_NODE *)malloc(sizeof(PROG_FEE_NODE));
                strcpy(pprog_fee_node_temp->pmgr_name, pprog_node->pmgr_name);
                strcpy(pprog_fee_node_temp->prog_num, pprog_node->prog_num);
                strcpy(pprog_fee_node_temp->team_name, pprog_node->team_name);
                pprog_fee_node_temp->prog_fee = pprog_node->prog_fee;
                pprog_node = pprog_node->next;
                pprog_fee_node_temp->next = pprog_fee_node_ret;
                pprog_fee_node_ret = pprog_fee_node_temp;
            }
            pteam_node = pteam_node->next;
        }
    }

    SortRichProg(pprog_fee_node_ret);

    gotoxy(28, 3);
    printf("下面按科研经费充足程度输出10条项目信息：\n");
    printf("\t所属团队\t项目经费\t项目负责人姓名\t项目编号");

    pprog_fee_node_temp = pprog_fee_node_ret;
    while(pprog_fee_node_temp!=NULL&&count<10)
    {
        count++;
        printf("\n");
        printf("\t%-12s\t%-8.2f\t%-14s\t%-15s",pprog_fee_node_temp->team_name,pprog_fee_node_temp->prog_fee,pprog_fee_node_temp->pmgr_name,pprog_fee_node_temp->prog_num);
        pprog_fee_node_temp = pprog_fee_node_temp->next;
    }

    printf("\n\t\t\t     按任意键继续……\n");
    getch();
    ClearScreen();
    ShowMenu();

    return;
}

/**
* 函数名称: SortRichProg
* 函数功能: 对科研经费充足项目信息链进行排序
* 输入参数: pf_hd 科研经费充足项目信息链头指针
* 输出参数: pf_hd 排序结果通过头指针返回
* 返 回 值: 无
*
* 调用说明:仅在StatRichProg母函数中作为子函数调用
*/
void SortRichProg(PROG_FEE_NODE *pf_hd)
{
    PROG_FEE_NODE *ppf_node_prior;
    PROG_FEE_NODE *ppf_node_after;
    PROG_FEE_NODE *ppf_node_cur;
    PROG_FEE_NODE *ppf_node_temp;
    ppf_node_prior = pf_hd;
    if (ppf_node_prior == NULL)
    {
        return;
    }

    /*排序时，用于交换结点信息的临时结点*/
    ppf_node_temp = (PROG_FEE_NODE *) malloc(sizeof(PROG_FEE_NODE));
    while (ppf_node_prior->next != NULL) /*选择法排序*/
    {
        ppf_node_cur = ppf_node_prior;
        ppf_node_after = ppf_node_prior->next;
        while (ppf_node_after != NULL)
        {
            if (ppf_node_cur->prog_fee < ppf_node_after->prog_fee)
            {
                ppf_node_cur = ppf_node_after;
            }
            ppf_node_after = ppf_node_after->next;
        }

        /*交换数据域，指针域不交换*/
        if (ppf_node_cur != ppf_node_prior)
        {
            *ppf_node_temp = *ppf_node_prior;
            *ppf_node_prior = *ppf_node_cur;
            ppf_node_prior->next = ppf_node_temp->next;
            ppf_node_temp->next = ppf_node_cur->next;
            *ppf_node_cur = *ppf_node_temp;
        }
        ppf_node_prior = ppf_node_prior->next;
    }
    free(ppf_node_temp);

    return;
}

/**
* 函数名称: SortRateInfo
* 函数功能: 对院系师生信息链进行排序
* 输入参数: rate_hd 院系师生信息链头指针
* 输出参数: rate_hd 排序结果通过头指针返回
* 返 回 值: 无
*
* 调用说明:仅在StatRateInfo母函数中作为子函数调用
*/
void SortRateInfo(RATE_NODE *rate_hd)
{
    RATE_NODE *prate_node_prior;
    RATE_NODE *prate_node_after;
    RATE_NODE *prate_node_cur;
    RATE_NODE *prate_node_temp;
    prate_node_prior = rate_hd;
    if (prate_node_prior == NULL)
    {
        return;
    }

    /*排序时，用于交换结点信息的临时结点*/
    prate_node_temp = (RATE_NODE *) malloc(sizeof(RATE_NODE));
    while (prate_node_prior->next != NULL) /*选择法排序*/
    {
        prate_node_cur = prate_node_prior;
        prate_node_after = prate_node_prior->next;
        while (prate_node_after != NULL)
        {
            if (prate_node_cur->proportion < prate_node_after->proportion)
            {
                prate_node_cur = prate_node_after;
            }
            prate_node_after = prate_node_after->next;
        }

        /*交换数据域，指针域不交换*/
        if (prate_node_cur != prate_node_prior)
        {
            *prate_node_temp = *prate_node_prior;
            *prate_node_prior = *prate_node_cur;
            prate_node_prior->next = prate_node_temp->next;
            prate_node_temp->next = prate_node_cur->next;
            *prate_node_cur = *prate_node_temp;
        }
        prate_node_prior = prate_node_prior->next;
    }
    free(prate_node_temp);
    return;
}

/**
* 函数名称: StatDeptInfoPre
* 函数功能: 按院系统计某年度的科研项目信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 按要求输出统计结果
*
* 调用说明:
*/
void StatDeptInfoPre(void)
{
    char year[5];
    printf("\n\t\t     请输入需要查询的年份（例2017）：");
    scanf("%s",year);
    getchar();
    if(strlen(year) != 4)
    {
        printf("\n\t\t     年份格式输入错误 \n\n\t\t     按任意键继续……");
        getch();
        ClearScreen();
        ShowMenu();
        return;
    }

    DEPT_NODE *hd = gp_head;
    DEPT_PROG_NODE *head;
    DEPT_PROG_NODE *pdp_node_temp;
    head = StatDeptInfo(hd, year);

    SortDeptProgInfo(head);

    gotoxy(28, 3);
    printf("下面按院系项目数输出院系信息：\n");
    printf("\t院系名称\t973项目数\t863项目数\t该年项目总数\t科研总经费");

    pdp_node_temp = head;
    while(pdp_node_temp!=NULL)
    {
        printf("\n");
        printf("\t%-12s\t%-8d\t%-8d\t%-8d\t%-.2f", pdp_node_temp->dept_name, pdp_node_temp->dept_prog_amount1, pdp_node_temp->dept_prog_amount3, pdp_node_temp->dept_prog_amount, pdp_node_temp->dept_fee);
        pdp_node_temp = pdp_node_temp->next;
    }

    printf("\n\t\t\t     按任意键继续……\n");
    getch();
    ClearScreen();
    ShowMenu();

    return;
}


/**
 * 函数名称: StatDeptInfo
 * 函数功能: 按院系统计某年度的科研项目信息
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: DEPT_PROG_NODE *类型的头结点
 *
 * 调用说明:
 */
DEPT_PROG_NODE* StatDeptInfo(DEPT_NODE *hd, char year[5])
{
    DEPT_NODE *pdept_node = hd;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    DEPT_PROG_NODE *pdept_prog_node_temp;
    DEPT_PROG_NODE *pdept_prog_node_ret = NULL;
    char prog_year[5];

    for(pdept_node=hd; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        int prog973 = 0, prog863 = 0;
        int progsum = 0;
        float dept_fee = 0;
        pdept_prog_node_temp = (DEPT_PROG_NODE *)malloc(sizeof(DEPT_PROG_NODE));

        pteam_node = pdept_node->tnext;
        while (pteam_node != NULL)
        {
            pprog_node = pteam_node->pnext;
            while (pprog_node != NULL)
            {
                strncpy(prog_year, pprog_node->st_date, 4);
                prog_year[4] = '\0';
                if (strcmp(prog_year, year))
                {
                    pprog_node = pprog_node->next;
                    continue;
                }
                dept_fee += pprog_node->prog_fee;
                if (pprog_node->type == '1')
                {
                    prog973++;
                }
                if (pprog_node->type == '3')
                {
                    prog863++;
                }
                progsum++;
                pprog_node = pprog_node->next;
            }
            pteam_node = pteam_node->next;
        }

        pdept_prog_node_temp->dept_fee = dept_fee;
        pdept_prog_node_temp->dept_prog_amount1 = prog973;
        pdept_prog_node_temp->dept_prog_amount3 = prog863;
        strcpy(pdept_prog_node_temp->dept_name, pdept_node->dept_name);
        pdept_prog_node_temp->dept_prog_amount = progsum;
        pdept_prog_node_temp->next = pdept_prog_node_ret;
        pdept_prog_node_ret = pdept_prog_node_temp;
    }

    return pdept_prog_node_ret;
}

/**
* 函数名称: SortDeptProgInfo
* 函数功能: 对院系项目信息链进行排序
* 输入参数: dp_hd 院系项目信息链头指针
* 输出参数: dp_hd 排序结果通过头指针返回
* 返 回 值: 无
*
* 调用说明:
*/
void SortDeptProgInfo(DEPT_PROG_NODE *dp_hd)
{
    DEPT_PROG_NODE *pdept_prog_node_prior;
    DEPT_PROG_NODE *pdept_prog_node_after;
    DEPT_PROG_NODE *pdept_prog_node_cur;
    DEPT_PROG_NODE *pdept_prog_node_temp;
    pdept_prog_node_prior = dp_hd;
    if (pdept_prog_node_prior == NULL)
    {
        return;
    }

    /*排序时，用于交换结点信息的临时结点*/
    pdept_prog_node_temp = (DEPT_PROG_NODE *) malloc(sizeof(DEPT_PROG_NODE));
    while (pdept_prog_node_prior->next != NULL) /*选择法排序*/
    {
        pdept_prog_node_cur = pdept_prog_node_prior;
        pdept_prog_node_after = pdept_prog_node_prior->next;
        while (pdept_prog_node_after != NULL)
        {
            if (pdept_prog_node_cur->dept_prog_amount < pdept_prog_node_after->dept_prog_amount)
            {
                pdept_prog_node_cur = pdept_prog_node_after;
            }
            pdept_prog_node_after = pdept_prog_node_after->next;
        }

        /*交换数据域，指针域不交换*/
        if (pdept_prog_node_cur != pdept_prog_node_prior)
        {
            *pdept_prog_node_temp = *pdept_prog_node_prior;
            *pdept_prog_node_prior = *pdept_prog_node_cur;
            pdept_prog_node_prior->next = pdept_prog_node_temp->next;
            pdept_prog_node_temp->next = pdept_prog_node_cur->next;
            *pdept_prog_node_cur = *pdept_prog_node_temp;
        }
        pdept_prog_node_prior = pdept_prog_node_prior->next;
    }
    free(pdept_prog_node_temp);
    return;
}

BOOL StatDept(void)
{
    int hotnum = 0;
    char *pString[] = {"院系师生数目情况",
                       "院系项目数量情况",
                       "返回"
                      };
    hotnum = MyMenu3(pString, 3);/*选择统计对象的范围*/
    switch (hotnum)
    {
        case 1:
            StatRateInfo();   /*统计院系师生数目情况*/
            break;
        case 2:
            StatDeptInfoPre();/*统计院系项目数情况*/
            break;
        case 3:
            break;
    }

    return TRUE;
}

BOOL StatTeam(void)
{
    int hotnum = 0;
    char *pString[] = {"自然基金项目情况",
                       "项目教师人数比情况",
                       "返回"
                      };
    hotnum = MyMenu3(pString, 3);/*选择统计对象的范围*/
    switch (hotnum)
    {
        case 1:
            StatMostProg();/*统计国家自然基金项目团队情况*/
            break;
        case 2:
            StatProgRate();/*统计团队科研项目数和教师人数的比值情况*/
            break;
        case 3:
            break;
    }

    return TRUE;
}

BOOL StatProg(void)
{
    int hotnum = 0;
    char *pString[] = {"--------------",
                       "经费最高的项目",
                       "返回"
                      };
    hotnum = MyMenu3(pString, 3);/*选择统计对象的范围*/
    switch (hotnum)
    {
        case 1:
        case 2:
            StatRichProg();/*统计经费较高的科研项目情况*/
            break;
        case 3:
            break;
    }

    return TRUE;
}



/**
* 函数名称: StatMostProg
* 函数功能: 按国家自然基金项目数量排序后输出
* 输入参数: 无
* 输出参数: 按照要求按序输出
* 返 回 值: 无
*
* 调用说明:
*/
void StatMostProg(void)
{
    DEPT_NODE *head = gp_head;
    DEPT_NODE *pdept_node = head;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    MOST_PROG_NODE *pmost_prog_node_temp;
    MOST_PROG_NODE *pmost_prog_node_ret = NULL;
    int count = 0;

    for(pdept_node=head; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        pteam_node = pdept_node->tnext;
        while (pteam_node != NULL)
        {
            int progamount2 = 0;
            float progfee2 = 0;
            pmost_prog_node_temp = (MOST_PROG_NODE *)malloc(sizeof(MOST_PROG_NODE));
            pprog_node = pteam_node->pnext;
            while(pprog_node != NULL)
            {
                if (pprog_node->type == '2')
                {
                    progfee2 += pprog_node->prog_fee;
                    progamount2++;
                }
                pprog_node = pprog_node->next;
            }
            pmost_prog_node_temp->prog_fee2 = progfee2;
            pmost_prog_node_temp->team_prog_amount2 = progamount2;
            strcpy(pmost_prog_node_temp->team_name, pteam_node->team_name);
            pteam_node = pteam_node->next;
            pmost_prog_node_temp->next = pmost_prog_node_ret;
            pmost_prog_node_ret = pmost_prog_node_temp;
        }
    }

    SortMostProgInfo(pmost_prog_node_ret);

    gotoxy(28, 3);
    printf("下面按国家自然基金项目总数输出团队信息：");
    printf("\n\t团队名称\t\t国家自然基金项目总数\t国家自然基金项目总经费");

    pmost_prog_node_temp = pmost_prog_node_ret;
    while (pmost_prog_node_temp!=NULL && count<10)
    {
        count++;
        printf("\n");
        printf("\t    %-20s\t%-8d\t%-8.2f", pmost_prog_node_temp->team_name, pmost_prog_node_temp->team_prog_amount2, pmost_prog_node_temp->prog_fee2);
        pmost_prog_node_temp = pmost_prog_node_temp->next;
    }

    printf("\n\t\t\t按任意键继续……");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* 函数名称: StatProgRate
* 函数功能: 按团队项目教师人数比排序后输出
* 输入参数: 无
* 输出参数: 按照要求按序输出
* 返 回 值: 无
*
* 调用说明:
*/
void StatProgRate(void)
{
    DEPT_NODE *head = gp_head;
    DEPT_NODE *pdept_node = head;
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    PROG_PROF_RATE_NODE *prog_prof_node_temp;
    PROG_PROF_RATE_NODE *prog_prof_node_ret = NULL;
    int count = 0;

    for(pdept_node=head; pdept_node!=NULL; pdept_node=pdept_node->next)
    {
        pteam_node = pdept_node->tnext;
        while (pteam_node != NULL)
        {
            int prog_num = 0;
            float pprate;
            prog_prof_node_temp = (PROG_PROF_RATE_NODE *)malloc(sizeof(PROG_PROF_RATE_NODE));
            pprog_node = pteam_node->pnext;
            while(pprog_node != NULL)
            {
                prog_num++;
                pprog_node = pprog_node->next;
            }

            pprate = prog_num*1.0/pteam_node->team_prof_amount;

            prog_prof_node_temp->team_prof_amount = pteam_node->team_prof_amount;
            prog_prof_node_temp->team_prog_amount = prog_num;
            prog_prof_node_temp->team_prog_prof_rate = pprate;
            strcpy(prog_prof_node_temp->team_name, pteam_node->team_name);
            pteam_node = pteam_node->next;
            prog_prof_node_temp->next = prog_prof_node_ret;
            prog_prof_node_ret = prog_prof_node_temp;
        }
    }

    SortProgProfInfo(prog_prof_node_ret);

    gotoxy(28, 3);
    printf("下面按团队项目教师人数比输出5条团队信息：");
    printf("\n\t团队名称\t科研项目数\t教师人数\t项目教师人数比");

    prog_prof_node_temp = prog_prof_node_ret;
    while (prog_prof_node_temp!=NULL && count<5)
    {
        count++;
        if(prog_prof_node_temp->team_prof_amount==0)
        {
            printf("\n");
            printf("\t%-12s\t%-8d\t%-8d\t教师人数不可为0", prog_prof_node_temp->team_name, prog_prof_node_temp->team_prog_amount, prog_prof_node_temp->team_prof_amount);
            continue;
        }
        printf("\n");
        printf("\t%-12s\t%-8d\t%-8d\t%-8.2f", prog_prof_node_temp->team_name, prog_prof_node_temp->team_prog_amount, prog_prof_node_temp->team_prof_amount, prog_prof_node_temp->team_prog_prof_rate);
        prog_prof_node_temp = prog_prof_node_temp->next;
    }

    printf("\n\t\t\t\t按任意键继续……");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* 函数名称: SortProgProfInfo
* 函数功能: 对团队项目教师人数比信息链进行排序
* 输入参数: pp_hd 团队项目教师人数比信息链头指针
* 输出参数: pp_hd 排序结果通过头指针返回
* 返 回 值: pp_hd 排序结果通过头指针返回
*
* 调用说明:
*/
void SortProgProfInfo(PROG_PROF_RATE_NODE *pp_hd)
{
    PROG_PROF_RATE_NODE *prog_prof_node_prior;
    PROG_PROF_RATE_NODE *prog_prof_node_after;
    PROG_PROF_RATE_NODE *prog_prof_node_cur;
    PROG_PROF_RATE_NODE *prog_prof_node_temp;
    prog_prof_node_prior = pp_hd;
    if (prog_prof_node_prior == NULL)
    {
        return;
    }

    /*排序时，用于交换结点信息的临时结点*/
    prog_prof_node_temp = (PROG_PROF_RATE_NODE *) malloc(sizeof(PROG_PROF_RATE_NODE));
    while (prog_prof_node_prior->next != NULL) /*选择法排序*/
    {
        prog_prof_node_cur = prog_prof_node_prior;
        prog_prof_node_after = prog_prof_node_prior->next;
        while (prog_prof_node_after != NULL)
        {
            if (prog_prof_node_cur->team_prog_prof_rate < prog_prof_node_after->team_prog_prof_rate)
            {
                prog_prof_node_cur = prog_prof_node_after;
            }
            prog_prof_node_after = prog_prof_node_after->next;
        }

        /*交换数据域，指针域不交换*/
        if (prog_prof_node_cur != prog_prof_node_prior)
        {
            *prog_prof_node_temp = *prog_prof_node_prior;
            *prog_prof_node_prior = *prog_prof_node_cur;
            prog_prof_node_prior->next = prog_prof_node_temp->next;
            prog_prof_node_temp->next = prog_prof_node_cur->next;
            *prog_prof_node_cur = *prog_prof_node_temp;
        }
        prog_prof_node_prior = prog_prof_node_prior->next;
    }
    free(prog_prof_node_temp);
    return;
}

/**
* 函数名称: SortMostProgInfo
* 函数功能: 对科研团队国家基金项目信息链进行排序
* 输入参数: mp_hd 科研团队国家基金项目信息链头指针
* 输出参数: mp_hd 排序结果通过头指针返回
* 返 回 值: 无
*
* 调用说明:
*/
void SortMostProgInfo(MOST_PROG_NODE *mp_hd)
{
    MOST_PROG_NODE *pmost_prog_prior;
    MOST_PROG_NODE *pmost_prog_after;
    MOST_PROG_NODE *pmost_prog_cur;
    MOST_PROG_NODE *pmost_prog_temp;
    pmost_prog_prior = mp_hd;
    if (pmost_prog_prior == NULL)
    {
        return;
    }

    /*排序时，用于交换结点信息的临时结点*/
    pmost_prog_temp = (MOST_PROG_NODE *) malloc(sizeof(MOST_PROG_NODE));
    while (pmost_prog_prior->next != NULL) /*选择法排序*/
    {
        pmost_prog_cur = pmost_prog_prior;
        pmost_prog_after = pmost_prog_prior->next;
        while (pmost_prog_after != NULL)
        {
            if (pmost_prog_cur->team_prog_amount2 < pmost_prog_after->team_prog_amount2)
            {
                pmost_prog_cur = pmost_prog_after;
            }
            pmost_prog_after = pmost_prog_after->next;
        }

        /*交换数据域，指针域不交换*/
        if (pmost_prog_cur != pmost_prog_prior)
        {
            *pmost_prog_temp = *pmost_prog_prior;
            *pmost_prog_prior = *pmost_prog_cur;
            pmost_prog_prior->next = pmost_prog_temp->next;
            pmost_prog_temp->next = pmost_prog_cur->next;
            *pmost_prog_cur = *pmost_prog_temp;
        }
        pmost_prog_prior = pmost_prog_prior->next;
    }
    free(pmost_prog_temp);
    return;
}


BOOL AboutResearch(void)
{
    int hotnum = 0;
    char *pString[] = {"973项目特点介绍",
                       "863项目特点介绍",
                       "国家自然科学基金项目介绍",
                       "返回"
                      };
    while(hotnum != 4)
    {
        hotnum = MyMenu4(pString, 4);/*选择统计对象的范围*/
        switch (hotnum)
        {
            case 1:
                Intro973();/*介绍973项目*/
                break;
            case 2:
                Intro863();/*介绍863项目*/
                break;
            case 3:
                IntroNationSci();/*介绍国家自然科学基金项目*/
                break;
        }
    }

    return TRUE;
}

void Intro973(void)
{
    char *info973[] = {"决策模式科学：专家咨询与政府决策相结合",
                       "管理模式科学：项目管理与经费管理有机结合",
                       "组织模式完善：多部门行业共同推进计划发展",
                       "返回"
                      };

    ShowModule(info973, 4);

    return;
}

void Intro863(void)
{
    char *info973[] = {"1.提高我国自主创新能力",
                       "2.坚持战略性、前沿性和前瞻性",
                       "3.促进基础研究与国家目标结合",
                       "返回"
                      };

    ShowModule(info973, 4);

    return;
}

void IntroNationSci(void)
{
    char *infonationsci[] = {"战略定位：支持基础研究，发挥导向作用",
                             "工作方针：激励创新、引领未来",
                             "战略：源头创新，打造创新环境",
                             "返回"
                            };

    ShowModule(infonationsci, 4);

    return;
}

BOOL HelpTopic(void)
{
    char *plabel_name[] = {"按ALT+X可以直接退出程序",
                           "按ALT+字母可以快捷选择主菜单。",
                           "按F1键可以获取帮助菜单",
                           "在子菜单弹出的情况按esc键可退出子菜单",
                           "确认"
                          };

    ShowModule(plabel_name, 5);

    return TRUE;
}

/**
 * 函数名称: SeekDept_All
 * 函数功能: 遍历院系信息链表，输出所有院系信息
 * 输入参数: head 主链头指针
 * 输出参数: 所有的院系基本信息
 * 返 回 值: 无
 *
 * 调用说明:测试函数，非测试状态不调用
 */
void SeekDept_All(DEPT_NODE *head)
{
    DEPT_NODE *pdept = head;
    while(pdept != NULL)
    {
        printf("\t\t\t     %-15s\t%-10s\n", pdept->dept_name, pdept->dmgr_name);
        pdept = pdept->next;
    }

    printf("\n\t\t\t\t按任意键继续。。。");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* 函数名称: SeekTeam_All
* 函数功能: 无条件输出所有基本信息
* 输入参数: hd 主链头指针
* 输出参数: 所有团队基本信息
* 返 回 值: 无
*
* 调用说明:测试函数，非测试状态不调用
*/
void SeekTeam_All(DEPT_NODE *head)
{
    DEPT_NODE *pd = head;
    TEAM_NODE *pt = NULL;

    while(pd != NULL)
    {
        pt = pd->tnext;
        while(pt != NULL)
        {
            /*遍历链表输出所有基本信息*/

            printf("%-15s\t%-15s\n", pt->dept_name, pt->team_name);
            pt = pt->next;
        }
        pd = pd->next;
    }

    printf("\n\t\t\t     按任意键继续。。。\n");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* 函数名称: SeekProg_ALL
* 函数功能: 输出所有项目基本信息，这是测试函数
* 输入参数: head 主链头指针
* 输出参数: 输出所有项目基本信息
* 返 回 值: 无
*
* 调用说明:测试函数，非测试状态不调用
*/
void SeekProg_All(DEPT_NODE *head)
{
    DEPT_NODE *pdept = head;
    TEAM_NODE *pteam = NULL;
    PROG_NODE *pprog = NULL;

    while(pdept != NULL)
    {
        pteam = pdept->tnext;
        while(pteam != NULL)
        {
            pprog = pteam->pnext;
            while(pprog != NULL)
            {
                printf("%-12s\t%-20s\n", pprog->pmgr_name, pprog->team_name);
                pprog = pprog->next;
            }
            pteam = pteam->next;
        }
        pdept = pdept->next;
    }

    printf("\n\t\t\t     按任意键继续。。。\n");
    getch();
    ClearScreen();
    ShowMenu();
}
