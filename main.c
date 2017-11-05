#include "research.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

    LoadData();                   /*���ݼ���*/
    InitInterface();              /*�����ʼ��*/
    RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(gp_head);            /*�˳�ϵͳ*/

    return 0;
}

/**
 * ��������: LoadData
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
 *           ���������ķ���ֵ����ΪTRUE.
 *
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪFALSEʱ, ������������.
 */
BOOL LoadData()
{
    int Re = 0;

    Re = CreatList(&gp_head);

    if (Re < 4 + 8 + 16)
    {
        /*���ݼ�����ʾ��Ϣ*/
        printf("\nϵͳ�������ݲ�����!\n");
    }

    printf("\n������������û�����\n");
    getch();

    return TRUE;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_INTENSITY | FOREGROUND_BLUE
               | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;  /*��ɫǰ���Ͱ�ɫ����*/

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
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

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    }
    while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
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

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
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

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
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

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(DEPT_NODE *hd)
{
    DEPT_NODE *pDeptNode1 = hd, *pDeptNode2;
    TEAM_NODE *pTeamNode1, *pTeamNode2;
    PROG_NODE *pProgNode1, *pProgNode2;

    while (pDeptNode1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
    {
        pDeptNode2 = pDeptNode1->next;
        pTeamNode1 = pDeptNode1->tnext;
        while (pTeamNode1 != NULL) /*�ͷſ����Ŷ���Ϣ֧���Ķ�̬�洢��*/
        {
            pTeamNode2 = pTeamNode1->next;
            pProgNode1 = pTeamNode1->pnext;

            while (pProgNode1 != NULL) /*�ͷſ�����Ŀ��Ϣ֧���Ķ�̬�洢��*/
            {
                pProgNode2 = pProgNode1->next;
                free(pProgNode1);
                pProgNode1 = pProgNode2;
            }

            free(pTeamNode1);
            pTeamNode1 = pTeamNode2;
        }

        free(pDeptNode1);  /*�ͷ��������Ķ�̬�洢��*/
        pDeptNode1 = pDeptNode2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬������Ŀ���������Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(DEPT_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            {
                /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
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
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
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
            else if ((asc-vkc == 0) || (asc-vkc == 32))   /*������ͨ��*/
            {
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                    case 70: /*f��F*/
                        PopMenu(1);
                        break;
                    case 77: /*m��M*/
                        PopMenu(2);
                        break;
                    case 81: /*q��Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s��S*/
                        PopMenu(4);
                        break;
                    case 72: /*h��H*/
                        PopMenu(5);
                        break;
                    case 13: /*�س�*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            {
                                /*���ƥ��ɹ�*/
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
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {
            /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &num_written);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &num_written);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }

    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }

    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }

    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {
        /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
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
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
        /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }

    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }

    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;

    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+', '-', '|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {
        /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {
        /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/

    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {
        /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {
        /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {
        /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    CONSOLE_CURSOR_INFO lpCur;
    DWORD res;
    COORD pos = {0, 0};
    int iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc;       /*vkc:���������*/

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
                /*���������������*/
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
                /* ������ڷ��ȵ����� */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
            }
        }
        /*����Ǽ�������*/
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;

            switch (vkc)
            {
            case 37 :/* �����ϡ���� */
            case 38 :
                --*piHot;
                if(*piHot < 1)
                {
                    *piHot = pHotArea->num;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 13 :/*�س�����ո��ʾ���µ�ǰ��ť*/
            case 32 :
                if(pHotArea->pSort[*piHot-1] == 0)
                {
                    //��Ϊ��ť
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
                    iRet = *piHot;
                    return iRet;
                }
            case 39 :/* �����ҡ��¼� */
            case 40 :
                ++*piHot;
                if(*piHot > pHotArea->num)
                {
                    *piHot = 1;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 27 :/* ��ESC�� */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
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

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {
        /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {
            /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {
        /*�����������ǰ�ť��*/
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);    /*���ع��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {
        /*�������������ı�����*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
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

    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

/**
 * ��������: SaveSysData
 * ��������: ����ϵͳ�����������������.
 * �������: hd ����ͷ���ָ��
 * �������:
 * �� �� ֵ: BOOL����
 *
 * ����˵��:
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
        printf("Ժϵ�ļ�����ʧ�ܣ�\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*����Ժϵ��Ϣ*/
        fwrite(pdept_node, sizeof(DEPT_NODE), 1, pfout);
    }
    fclose(pfout);

    pfout = fopen(gp_team_info_filename, "wb");
    if(!pfout)
    {
        printf("�����Ŷ��ļ�����ʧ�ܣ�\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*��������Ŷ���Ϣ*/
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
        printf("������Ŀ�ļ�����ʧ�ܣ�\n");
        return FALSE;
    }
    for (pdept_node = hd; pdept_node != NULL; pdept_node = pdept_node->next)
    {
        /*���������Ŀ��Ϣ*/
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
 * ��������: CreatList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ���Ժϵ��Ϣ���ݣ��޿����Ŷ���Ϣ�Ϳ�����Ŀ��Ϣ����
 *           12 �Ѽ���Ժϵ��Ϣ�Ϳ����Ŷ���Ϣ���ݣ��޿�����Ŀ��Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
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
        printf("Ժϵ��Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("Ժϵ��Ϣ�����ļ��򿪳ɹ�!\n");

    /*ÿ�ζ�ȡ��СΪһ��DEPT_NODE��ռ�ڴ��Ժϵ��Ϣ���*/
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
        printf("Ժϵ��Ϣ�����ļ�����ʧ��!\n");
        return re;
    }
    printf("Ժϵ��Ϣ�����ļ����سɹ�!\n");
    *phead = hd;
    re = re + 4;

    if ((pFile = fopen(gp_team_info_filename, "rb")) == NULL)
    {
        printf("�����Ŷӻ�����Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("�����Ŷӻ�����Ϣ�����ļ��򿪳ɹ�!\n");
    re = re + 8;

    /*�������ļ��ж�ȡ�����Ŷӻ�����Ϣ���ݣ�����������Ӧ���Ŀ����Ŷӻ�����Ϣ֧����*/
    while (fread(&tmp2, sizeof(TEAM_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж����Ŀ����Ŷӻ�����Ϣ*/
        pTeamNode = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
        *pTeamNode = tmp2;
        pTeamNode->pnext = NULL;

        /*�������ϲ��Ҹ��Ŷ�����Ժϵ��Ӧ���������*/
        pDeptNode = hd; /*λ��ָ��pDeptNode��λ������ͷ��*/
        while (pDeptNode != NULL
                && strcmp(pDeptNode->dept_name, pTeamNode->dept_name) != 0)
        {
            pDeptNode = pDeptNode->next;
        }
        if (pDeptNode != NULL) /*����ҵ����򽫽���Ժ���ȳ���ʽ�����Ŷ���Ϣ֧��*/
        {
            pTeamNode->next = pDeptNode->tnext;
            pDeptNode->tnext = pTeamNode;
        }
        else  /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pTeamNode);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_prog_info_filename, "rb")) == NULL)
    {
        printf("������Ŀ��Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("������Ŀ��Ϣ�����ļ��򿪳ɹ�!\n");
    re = re + 16;

    /*�������ļ��ж�ȡ������Ŀ��Ϣ���ݣ���������Ŷӻ�����Ϣ֧����Ӧ���Ľɷ�֧����*/
    while (fread(&tmp3, sizeof(PROG_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж����Ŀ�����Ŀ��Ϣ*/
        pProgNode = (PROG_NODE *)malloc(sizeof(PROG_NODE));
        *pProgNode = tmp3;

        /*����ѧ����Ϣ֧���϶�ӦԺϵ��Ϣ���*/
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
        if (find)  /*����ҵ����򽫽���Ժ���ȳ���ʽ���������Ŀ��Ϣ֧����*/
        {
            pProgNode->next = pTeamNode->pnext;
            pTeamNode->pnext = pProgNode;
        }
        else /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pProgNode);
        }
    }
    fclose(pFile);

    return re;
}

/**
 * ��������: SeekTeamNode
 * ��������: ���Ŷ�����(������)����ѧ��������Ϣ���.
 * �������: hd ����ͷָ��
 *           team_name �Ŷ�����
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��:
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
 * ��������: SeekDeptNode
 * ��������: ��Ժϵ����(������)����ѧ��������Ϣ���.
 * �������: hd ����ͷָ��
 *           dept_name Ժϵ����
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��: ��ΪInsertDeptNode���Ӻ�������
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
 * ��������: SeekProgNode
 * ��������: ���ͽɷ����ڲ��ҽɷ���Ϣ���.
 * �������: hd ����ͷָ��
 *           team_name �����Ŷ�
 *           prog_num ��Ŀ���
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��:
 */
PROG_NODE *SeekProgNode(DEPT_NODE *hd, char *team_name, char *prog_num)
{
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node;
    int find = 0;
    pteam_node = SeekTeamNode(hd, team_name);/*���Ȳ��������Ŷ���Ϣ��֧��*/

    /*����ҵ������ڿ�����Ŀ֧���ϼ�������ָ������Ŀ���*/
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
 * ��������: DelProgNode
 * ��������: ��ʮ��������ɾ��ָ���Ŀ�����Ŀ��Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��: ������Ŀ��ź������Ŷӿ���ȷ��Ψһ�Ľɷ���Ϣ
 */
BOOL DelProgNode(DEPT_NODE *hd)
{
    TEAM_NODE *pteam_node;
    PROG_NODE *pprog_node_prior;
    PROG_NODE *pprog_node_current;
    char prog_num[15];
    char team_name[30];

    gotoxy(28, 6);
    printf("��������Ҫɾ������Ŀ��Ϣ����Ŀ��ţ�");
    gotoxy(64, 6);
    scanf("%s", prog_num);
    getchar();

    gotoxy(28, 7);
    printf("��������Ҫɾ������Ŀ��Ϣ�����Ŷӣ�");
    gotoxy(62, 7);
    scanf("%s", team_name);
    getchar();

    pteam_node = SeekTeamNode(hd, team_name);
    if (pteam_node != NULL)/*���ҿ�����Ŀ֧���ϸ�����Ŀ��ŵĿ�����Ŀ���*/
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
            if (pprog_node_prior == NULL) /*λ�ڵ�һ������λ��*/
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
            char *warn0[] = {"���Ŷ�û����Ŀ ",
                             "ɾ��������Ϣʧ�� ",
                             "ȷ��"
                            };
            ClearScreen();
            ShowModule(warn0, 3);
            ShowMenu();
            return FALSE;
        }
    }
    else
    {
        char *warn1[] = {"�����������Ľ�� ",
                         "ɾ��������Ϣʧ�� ",
                         "ȷ��"
                        };

        ClearScreen();
        ShowModule(warn1, 3);
        ShowMenu();

        return FALSE;
    }

    char *sucinfo[] = {"ɾ���ɹ�,��Ϣ�ѱ���",
                       "ȷ��"
                       };
    SaveSysData(gp_head);
    ShowModule(sucinfo, 2);
    ClearScreen();
    ShowMenu();

    return TRUE;
}

/**
 * ��������: DelTeamNode
 * ��������: ��ʮ��������ɾ��ָ���Ŀ����Ŷ���Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��: �����Ŷ����ƺ�����Ժϵ����ȷ��Ψһ�Ľɷ���Ϣ
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

    char *notice[] = {"ɾ���ɹ�����Ϣ�ѱ���",
                      "ȷ��"
                     };

    gotoxy(28, 6);
    printf("��������Ҫɾ�����Ŷ���Ϣ�����ƣ�");
    gotoxy(60, 6);
    scanf("%s", team_name);
    getchar();

    gotoxy(28, 7);
    printf("��������Ҫɾ�����Ŷ���Ϣ����Ժϵ��");
    gotoxy(62, 7);
    scanf("%s", dept_name);
    getchar();

    pdept_node = SeekDeptNode(hd, dept_name);
    if (pdept_node != NULL)/*���ҿ�����Ŀ֧���ϸ�����Ŀ��ŵĿ�����Ŀ���*/
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
            if (pteam_node_prior == NULL)/*λ�ڵ�һ������λ��*/
            {
                pdept_node->tnext = pteam_node_current->next;
            }
            else
            {
                pteam_node_prior->next = pteam_node_current->next;
            }
            pprog_node_prior = NULL;
            pprog_node_current = pteam_node_current->pnext;
            MakeDel();/*ʹ�û��ٴ�ȷ��*/
            while (pprog_node_current != NULL) /*ɾ���Ŷ���������*/
            {
                pprog_node_prior = pprog_node_current;
                pprog_node_current = pprog_node_current->next;
                free(pprog_node_prior);
            }
            free(pteam_node_current);
        }
        else
        {
            char *warn1[] = {"�����������Ľ�� ",
                             "ɾ��������Ϣʧ�� ",
                             "ȷ��"
                            };
            ShowModule(warn1, 3);
            ClearScreen();
            ShowMenu();
            return FALSE;
        }
    }
    else
    {
        char *warn1[] = {"�����������Ľ�� ",
                         "ɾ��������Ϣʧ�� ",
                         "ȷ��"
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
 * ��������: DelDeptNode
 * ��������: ��ʮ��������ɾ��ָ���Ŀ����Ŷ���Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��: ����Ժϵ���ƿ���ȷ��Ψһ��Ժϵ��Ϣ
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
    char *notice[] = {"ɾ���ɹ�����Ϣ�ѱ���",
                      "ȷ��"
                     };

    gotoxy(28, 6);
    printf("��������Ҫɾ����Ժϵ�����ƣ�");
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

    if (pdept_node_current != NULL)/*���ҿ�����Ŀ֧���ϸ�����Ŀ��ŵĿ�����Ŀ���*/
    {
        pteam_node_prior = NULL;
        pteam_node_current = pdept_node_current->tnext;
        if (pdept_node_prior == NULL) /*λ�ڵ�һ������λ��*/
        {
            gp_head = pdept_node_current -> next;
        }
        else
        {
            pdept_node_prior->next = pdept_node_current->next;
        }
        MakeDel();/*ʹ�û��ٴ�ȷ��*/
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
        char *warn1[] = {"������������Ժϵ ",
                         "ɾ��������Ϣʧ�� ",
                         "ȷ��"
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
 * ��������: MakeDel
 * ��������: �����û��Ƿ���Ҫɾ��
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾȷ��ɾ��, FALSE��ʾȡ��ɾ��
 *
 * ����˵��:
 */
void MakeDel()
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�Ƿ�Ҫɾ����ǰ���ݣ�",
                   "  ȷ��   ȡ��"
                  };
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;           /*������������ұ߿�λ�����*/
    pos.Y = 7;                             /*������������±߿�λ�����*/
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;/*pCh��λ��*/

    areas.num = 2;
    SMALL_RECT aArea[] = {{
            rcPop.Left + 7, rcPop.Top + 5,
            rcPop.Left + 10, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top + 5,
            rcPop.Left + 17, rcPop.Top + 5
        }
    };                                         /*��ɫѡ�п��С��λ��*/
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);  /*�ָ���*/

    if (DealInput(&areas, &iHot) == 1 && iHot == 1)
    {
        PopOff();
        char *plabel_name1[] = {"�����Ѿ�ɾ����",
                                "ȷ��"
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
 * ��������: ModifProgNode
 * ��������: ��ָ������Ŀ��Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifProgNode(DEPT_NODE *hd)
{
    PROG_NODE *pprog_node_temp;/*������Ϊԭ���ı��޸Ľ�㣬��pprog_node���*/
    PROG_NODE *pprog_node_next;/*������ʱ���ָ����*/
    PROG_NODE *pprog_node = (PROG_NODE *)malloc(sizeof(PROG_NODE));

    gotoxy(28, 6);
    printf("��������Ҫ�޸ĵ���Ŀ��Ϣ����Ŀ��ţ�");
    gotoxy(32, 8);
    scanf("%s", pprog_node->prog_num);
    getchar();

    gotoxy(28, 10);
    printf("��������Ҫ�޸ĵ���Ŀ��Ϣ�����Ŷӣ�");
    gotoxy(32, 12);
    scanf("%s", pprog_node->team_name);
    getchar();

    pprog_node_temp = SeekProgNode(hd, pprog_node->team_name, pprog_node->prog_num);/*����ָ��������Ŀ���*/
    if (pprog_node_temp != NULL)
    {
        gotoxy(28, 14);
        printf("������������");
        gotoxy(40, 14);
        scanf("%s", pprog_node->pmgr_name);
        getchar();

        gotoxy(28, 16);
        printf("��Ŀ���");
        gotoxy(38, 16);
        scanf("%c", &pprog_node->type);
        getchar();

        gotoxy(28, 18);
        printf("��Ŀ����: ");
        gotoxy(38, 18);
        scanf("%f", &pprog_node->prog_fee);
        getchar();

        gotoxy(28, 20);
        printf("��ʼʱ�䣺");
        gotoxy(38, 20);
        scanf("%s", pprog_node->st_date);
        getchar();

        gotoxy(28, 22);
        printf("��Ŀ��ţ�");
        gotoxy(38, 22);
        scanf("%s", pprog_node->prog_num);
        getchar();

        pprog_node_next = pprog_node_temp->next;/*����ָ����*/
        *pprog_node_temp = *pprog_node;
        pprog_node_temp->next = pprog_node_next;
        free(pprog_node);

        char *sucinfo[] = {"�޸ĳɹ�����Ϣ�ѱ���",
                           "ȷ��"
                          };
        SaveSysData(gp_head);
        ShowModule(sucinfo, 2);
        ClearScreen();
        ShowMenu();
        return TRUE;
    }
    else
    {
        char *warn1[] = {"����Ŀ������ ",
                         "���������Ϣʧ�� ",
                         "ȷ��"
                        };

        ClearScreen();
        ShowModule(warn1, 3);
        free(pprog_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * ��������: ModifTeamNode
 * ��������: ��ָ�����Ŷ���Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifTeamNode(DEPT_NODE *hd)
{
    TEAM_NODE *pteam_node_temp;/*������Ϊpteam_node�����*/
    TEAM_NODE *pteam_node_next;/*������ʱ���ָ����*/
    TEAM_NODE *pteam_node = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
    PROG_NODE *pteam_node_pnext;

    gotoxy(29, 6);
    printf("��������Ҫ�޸ĵ��Ŷ���Ϣ���Ŷ����ƣ�");
    gotoxy(32, 8);
    scanf("%s", pteam_node->team_name);
    getchar();

    pteam_node_temp = SeekTeamNode(hd, pteam_node->team_name);/*����ָ��������Ŀ���*/
    if (pteam_node_temp != NULL)
    {
        gotoxy(29, 10);
        printf("�Ŷ����ƣ�");
        gotoxy(40, 10);
        scanf("%s", pteam_node->team_name);
        getchar();

        gotoxy(29, 12);
        printf("��ʦ������");
        gotoxy(40, 12);
        scanf("%d", &pteam_node->team_prof_amount);
        getchar();

        gotoxy(29, 14);
        printf("�о�������: ");
        gotoxy(42, 14);
        scanf("%d", &pteam_node->team_grd_amount);
        getchar();

        gotoxy(29, 16);
        printf("�ŶӸ�����: ");
        gotoxy(41, 16);
        scanf("%s", pteam_node->tmgr_name);
        getchar();

        gotoxy(29, 18);
        printf("����Ժϵ��");
        gotoxy(38, 18);
        scanf("%s", pteam_node->dept_name);
        getchar();

        pteam_node_next = pteam_node_temp->next;/*����ָ����*/
        pteam_node_pnext = pteam_node_temp->pnext;/*����ָ����Ŀ����ָ����*/
        *pteam_node_temp = *pteam_node;
        pteam_node_temp->next = pteam_node_next;
        pteam_node_temp->pnext = pteam_node_pnext;
        free(pteam_node);

        char *sucinfo[] = {"�޸ĳɹ�����Ϣ�ѱ���",
                           "ȷ�� "
                          };
        SaveSysData(gp_head);
        ShowModule(sucinfo, 2);
        ClearScreen();
        ShowMenu();
        return TRUE;
    }
    else
    {
        char *warn1[] = {"���ŶӲ����� ",
                         "���������Ϣʧ�� ",
                         "ȷ��"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pteam_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * ��������: ModifDeptNode
 * ��������: ��ָ�����Ŷ���Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifDeptNode(DEPT_NODE *hd)
{
    DEPT_NODE *pdept_node_temp;/*������Ϊpteam_node�����*/
    DEPT_NODE *pdept_node_next;/*������ʱ���ָ����*/
    DEPT_NODE *pdept_node = (DEPT_NODE *)malloc(sizeof(DEPT_NODE));
    TEAM_NODE *pdept_node_tnext;

    gotoxy(28, 6);
    printf("��������Ҫ�޸ĵ�Ժϵ�����ƣ�");
    gotoxy(56, 6);
    scanf("%s", pdept_node->dept_name);
    getchar();

    pdept_node_temp = SeekDeptNode(hd, pdept_node->dept_name);/*����ָ��Ժϵ���*/
    if (pdept_node_temp != NULL)
    {
        gotoxy(28, 8);
        printf("������������");
        gotoxy(40, 8);
        scanf("%s", pdept_node->dmgr_name);/*���븺��������*/
        getchar();

        gotoxy(28, 10);
        printf("��ϵ�绰��");
        gotoxy(40, 10);
        scanf("%s", pdept_node->telno);/*������ϵ�绰*/
        getchar();

        pdept_node_next = pdept_node_temp->next;/*����ָ����*/
        pdept_node_tnext = pdept_node_temp->tnext;/*����ָ���Ŷ�����ָ����*/
        *pdept_node_temp = *pdept_node;
        pdept_node_temp->next = pdept_node_next;
        pdept_node_temp->tnext = pdept_node_tnext;

        char *sucinfo[] = {"�޸ĳɹ�����Ϣ�ѱ���",
                           "ȷ�� "
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
        char *warn1[] = {"��Ժϵ������ ",
                         "���������Ϣʧ�� ",
                         "ȷ��"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        free(pdept_node);
        ShowMenu();

        return FALSE;
    }
}

/**
 * ��������: RemoveSysData
 * ��������: ��ָ�������ļ��лָ������������.
 * �������: phead ����ͷ���ָ��ĵ�ַ
 *           filename ��ű������ݵ������ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
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

    /*��ȡ���ֻ���������Ϣ�ļ�¼��*/
    read(handle, (char *)&dept_node_num, sizeof(dept_node_num));
    read(handle, (char *)&team_node_num, sizeof(team_node_num));
    read(handle, (char *)&prog_node_num, sizeof(prog_node_num));

    /*��ȡԺϵ��Ϣ������ʮ������*/
    for(ulloop=1; ulloop<=dept_node_num; ulloop++)
    {
        pdept_node = (DEPT_NODE *)malloc(sizeof(DEPT_NODE));
        read(handle, (char *)pdept_node, sizeof(DEPT_NODE));
        pdept_node -> tnext = NULL;
        pdept_node -> next = hd;
        hd = pdept_node;
    }
    *phead = hd;

    /*��ȡ�����Ŷӻ�����Ϣ�����������Ŷ�֧��*/
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

    /*��ȡ������Ŀ��Ϣ������������Ŀ��Ϣ����*/
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
    SaveSysData(hd);    /*���ڴ������ݱ��浽�����ļ�*/

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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
 * ��������: SaveData
 * ��������:
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL SaveData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"��ȷ�ϼ���������",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 2);
    SaveSysData(gp_head);
    char *plabel_name1[] = {"�����ѱ���",
                            "����"
                           };

    ShowModule(plabel_name1, 2);

    return bRet;
}

/**
 * ��������: ExitSys
 * ��������: �������˳�ϵͳ
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�˳��ɹ�, FALSE��ʾ�˳�ʧ��
 *
 * ����˵��:
 */
BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�Ƿ���Ҫ�˳�ϵͳ��", "ȷ��    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
    char *pString[] = {"����Ժϵ������Ϣ",
                       "�޸�Ժϵ������Ϣ",
                       "ɾ��Ժϵ������Ϣ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*ѡ��Ҫ���еĲ���*/
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
    char *pString[] = {"�����Ŷӻ�����Ϣ",
                       "�޸��Ŷӻ�����Ϣ",
                       "ɾ���Ŷӻ�����Ϣ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*ѡ��Ҫ���еĲ���*/
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
    char *pString[] = {"������Ŀ������Ϣ",
                       "�޸���Ŀ������Ϣ",
                       "ɾ����Ŀ������Ϣ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu4(pString, 4);/*ѡ��Ҫ���еĲ���*/
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
 * ��������: MyMenu4
 * ��������: ������3���ַ�����1��"����"�ַ����ı�ǩ�����ó��������ȴ��û�����
 * �������: pString ָ�룬n��ǩ����
 * �������: ��
 * �� �� ֵ: ����굥�������س���ո�ʱ���ص�ǰ�������
 *
 * ����˵��:
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

    /*�ҵ�����ַ����Ա���ȷ��pop�Ĵ�С*/
    for (i=0, maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    /*ȷ��popλ�úʹ�С*/
    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    /*ȷ�� ȡ����λ��*/
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
 * ��������: MyMenu3
 * ��������: ������2���ַ�����1��"����"�ַ����ı�ǩ�����ó��������ȴ��û�����
 * �������: pString ָ�룬n��ǩ����
 * �������: ��
 * �� �� ֵ: ����굥�������س���ո�ʱ���ص�ǰ�������
 *
 * ����˵��:
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
 * ��������: gotoxy
 * ��������: ʹ��궨λ����Ļ��ĳ����
 * �������: xΪ�����꣬yΪ������
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void gotoxy(int x, int y)
{
    COORD pos;
    pos.X = x - 1;
    pos.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

/**
 * ��������: InsertDeptNode
 * ��������: ������ʮ�������в���һ��Ժϵ��Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InsertDeptNode(DEPT_NODE* hd)
{
    DEPT_NODE *pdept_node;
    pdept_node = (DEPT_NODE*)malloc(sizeof(DEPT_NODE));

    gotoxy(25, 4);
    printf("����������Ҫ��ӵ�Ժϵ�Ļ�����Ϣ��");

    gotoxy(29, 6);
    printf("Ժϵ���ƣ�");
    gotoxy(39, 6);
    scanf("%s", pdept_node -> dept_name);/*����Ժϵ����*/
    getchar();

    if (strlen(pdept_node -> dept_name) > 20)
    {
        char *warn1[] = {"����Ժϵ���ƹ���",
                         "����Ժϵ��Ϣʧ��",
                         "ȷ��"
                        };

        ShowModule(warn1, 3);
        ClearScreen();
        ShowMenu();
        free(pdept_node);

        return;
    }

    /*���Ժϵ�Ѿ����ڣ���������ʾ*/
    if (SeekDeptNode(hd, pdept_node->dept_name)!=NULL)
    {
        char *warn2[] = {"��Ժϵ�Ѵ���",
                         "�����޸�Ժϵ��Ϣ����ѡ���޸�Ժϵ��Ϣ",
                         "ȷ��"
                        };

        ShowModule(warn2, 3);
        ClearScreen();
        ShowMenu();
        free(pdept_node);

        return;
    }

    gotoxy(29, 8);
    printf("������������");
    gotoxy(41, 8);
    scanf("%s", pdept_node->dmgr_name);/*���븺��������*/
    getchar();

    gotoxy(29, 10);
    printf("��ϵ�绰��");
    gotoxy(39, 10);
    scanf("%s", pdept_node->telno);/*������ϵ�绰*/
    getchar();

    pdept_node->tnext = NULL;     /*������ͷ����ÿ�*/
    pdept_node->next = hd;
    gp_head = pdept_node;         /*�½������Ϊ�µ�������ͷ���*/
    char *sucinfo[] = {"����ɹ�����Ϣ�ѱ���",
                       "ȷ��"
                      };
    SaveSysData(gp_head);
    ShowModule(sucinfo, 2);
    ClearScreen();
    ShowMenu();

    return;
}

/**
 * ��������: InsertTeamNode
 * ��������: ��Ժϵ��Ϣ����ʮ�������в����Ŷӻ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InsertTeamNode(DEPT_NODE* hd)
{
    DEPT_NODE *pdept_node;
    TEAM_NODE *pteam_node;
    pteam_node = (TEAM_NODE*)malloc(sizeof(TEAM_NODE));

    gotoxy(26, 4);
    printf("����������Ҫ��ӵ��Ŷӻ�����Ϣ��");

    gotoxy(30, 6);
    printf("����Ժϵ: ");
    gotoxy(40, 6);
    scanf("%s", pteam_node -> dept_name);
    getchar();

    /*�ж�Ժϵ���������Ƿ���Ϲ淶*/
    if(strlen(pteam_node->dept_name)>20)
    {
        char *warn2[] =  {"Ժϵ���ƹ��� ",
                          "���������Ϣʧ�� ",
                          "ȷ��"
                         };

        ShowModule(warn2, 3);
        free(pteam_node);
        ClearScreen();
        ShowMenu();

        return;
    }

    /*��ʮ�������ҵ���Ӧ��Ժϵ��Ϣ���*/
    pdept_node = SeekDeptNode(hd, pteam_node->dept_name);
    if(pdept_node == NULL)
    {
        char *warn1[] = {"��Ժϵ������ ",
                         "���������Ϣʧ�� ",
                         "ȷ��"
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
        printf("�Ŷ����ƣ�");
        gotoxy(42, 8);
        scanf("%s", pteam_node->team_name);
        getchar();

        gotoxy(30, 10);
        printf("��ʦ������");
        gotoxy(40, 10);
        scanf("%d", &pteam_node->team_prof_amount);
        getchar();

        gotoxy(30, 12);
        printf("�о�������: ");
        gotoxy(42, 12);
        scanf("%d", &pteam_node->team_grd_amount);
        getchar();

        gotoxy(30, 14);
        printf("�ŶӸ�����: ");
        gotoxy(44, 14);
        scanf("%s", pteam_node->tmgr_name);
        getchar();

        pteam_node->pnext = NULL;
        pteam_node->next = pdept_node -> tnext;
        pdept_node->tnext = pteam_node;

        gotoxy(30, 16);
        printf("���������������\n");
        getch();

        char *sucinfo[] = {"����ɹ�����Ϣ�ѱ���",
                           "ȷ��"
                          };
        SaveSysData(gp_head);
        ClearScreen();
        ShowMenu();
        ShowModule(sucinfo, 2);

        return;
    }
}

/**
 * ��������: InsertProgNode
 * ��������: ���Ŷ���Ϣ��ʮ�������в�����Ŀ������Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InsertProgNode(DEPT_NODE* hd)
{
    PROG_NODE *pprog_node;
    TEAM_NODE *pteam_node;
    pprog_node = (PROG_NODE*)malloc(sizeof(PROG_NODE));

    gotoxy(26, 4);
    printf("����������Ҫ��ӵ���Ŀ������Ϣ��");

    gotoxy(30, 6);
    printf("�����Ŷ�: ");
    gotoxy(40, 6);
    scanf("%s", pprog_node->team_name);
    getchar();

    /*�ж��Ŷ����������Ƿ���Ϲ淶*/
    if(strlen(pprog_node->team_name) > 30)
    {
        char *warn2[] =  {"�����Ŷ����ƹ��� ",
                          "���������Ϣʧ�� ",
                          "ȷ��"
                         };

        ShowModule(warn2, 3);
        free(pprog_node);
        ClearScreen();
        ShowMenu();

        return;
    }

    /*��ʮ�������ҵ���Ӧ���Ŷ���Ϣ���*/
    pteam_node = SeekTeamNode(hd, pprog_node->team_name);
    if(pteam_node == NULL)
    {
        char *warn1[] = {"���ŶӲ����� ",
                         "���������Ϣʧ�� ",
                         "ȷ��"
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
        printf("������������");
        gotoxy(42, 8);
        scanf("%s", pprog_node->pmgr_name);
        getchar();

        gotoxy(30, 10);
        printf("��Ŀ��ţ�");
        gotoxy(40, 10);
        scanf("%s", pprog_node->prog_num);
        getchar();

        gotoxy(30, 12);
        printf("��Ŀ����: ");
        gotoxy(40, 12);
        scanf("%f", &pprog_node->prog_fee);
        getchar();

        gotoxy(30, 14);
        printf("��Ŀ���: ");
        gotoxy(40, 14);
        scanf("%c", &pprog_node->type);
        getchar();

        /*�ж���Ŀ������������Ƿ���Ϲ淶*/
        if(pprog_node->type!='1' && pprog_node->type!='2' && pprog_node->type!='3' && pprog_node->type!='4' && pprog_node->type!='5')
        {
            char *warn3[] =  {"û�д���Ŀ��� ���������Ϣʧ�� ",
                              "ȷ��"
                             };

            ShowModule(warn3, 2);
            free(pprog_node);
            ClearScreen();
            ShowMenu();

            return;
        }

        gotoxy(30, 16);
        printf("��ʼʱ��: ");
        gotoxy(40, 16);
        scanf("%s", pprog_node->st_date);
        getchar();

        pprog_node->next = pteam_node->pnext;
        pteam_node->pnext = pprog_node;

        gotoxy(30, 18);
        printf("���������������\n");
        getch();

        char *sucinfo[] = {"����ɹ�����Ϣ�ѱ���",
                           "ȷ��"
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
    char *pString[] = {"��Ժ�����˲�ѯ",
                       "��Ժϵ���ƹؼ��ֲ�ѯ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
    case 1:
        SeekDmgr();/*��Ժ�����˲�ѯ*/
        break;
    case 2:
        SeekDeptPart();/*��Ժϵ���ƵĲ��ֻ�ȫ��Ϊ��������*/
        break;
    case 3:
        break;/*����*/
    }

    return TRUE;
}


BOOL QueryTeamInfo(void)
{
    char *pString[] = {"���Ŷӹؼ��ֲ�ѯ",
                       "����ʦ������ѯ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
        case 1:
            SeekTeamPart();/*���Ŷӹؼ��ֲ�ѯ�Ŷ�*/
            break;
        case 2:
            SeekTeamProf();/*���ݽ�ʦ������ѯ�����������Ŷ���Ϣ*/
            break;
        case 3:
            break;/*����*/
    }

    return TRUE;
}


BOOL QueryProgInfo(void)
{
    char *pString[] = {"����Ŀ��Ų�ѯ",
                       "�������ŶӲ�ѯ",
                       "����"
                      };
    int hotnum = 0;
    hotnum = MyMenu3(pString,3);
    switch (hotnum)
    {
        case 1:
            SeekProgNum();/*����Ŀ��Ų�ѯ*/
            break;
        case 2:
            SeekProgTeam();/*�������ŶӲ�ѯ*/
            break;
        case 3:
            break;/*����*/
    }

    return TRUE;
}

/**
 * ��������: SeekDmgr
 * ��������: ���������Ժ������������������������Ժϵ��Ϣ
 * �������: head ����ͷָ��
 * �������: ����������Ժϵ��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekDmgr()
{
    char dmgr_name[12];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *find = head;
    int flag = 0;

    gotoxy(27, 2);
    printf("������Ժ������������");
    gotoxy(47, 2);
    scanf("%s", dmgr_name);
    getchar();
    printf("\n\t   Ժϵ����\t\tԺ������\t��ϵ�绰");

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
        printf("\n\t\t\t����������������Ժϵ ");
    }

    printf("\n\t\t\t\t���������������");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * ��������: SeekDeptPart
 * ��������: ���������Ժϵ���ֻ�ȫ�����Ʋ���Ժϵ��Ϣ
 * �������: ��
 * �������: ����������Ժϵ��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekDeptPart()
{
    char dept_name[20];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *find = head;
    int flag = 0;    /*����Ƿ��ҵ�����һ������������Ժϵ*/

    gotoxy(27, 2);
    printf("����Ժϵ���ƹؼ��֣�");
    gotoxy(47, 2);
    scanf("%s", dept_name);
    getchar();
    printf("\n\t   Ժϵ����\t\tԺ������\t��ϵ�绰");

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
        printf("\n\t\t\t����������������Ժϵ����");
    }

    printf("\n\t\t\t���������������  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * ��������: SeekTeamPart
 * ��������: �������ŶӲ��ֻ�ȫ�����Ʋ���Ժϵ��Ϣ
 * �������: ��
 * �������: �����������Ŷ���Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekTeamPart()
{
    char team_name[20];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*����Ƿ��ҵ�����һ�������������Ŷ�*/

    gotoxy(27, 2);
    printf("�����Ŷ����ƹؼ��֣�");
    gotoxy(47, 2);
    scanf("%s", team_name);
    getchar();
    printf("\n\t�Ŷ�����\t\t�Ӹ�����\t����Ժϵ\t��ʦ����\t�о�������");

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
        printf("\n\t\t\t�����������������Ŷ�");
    }

    printf("\n\t\t\t\t���������������  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * ��������: SeekProgNum
 * ��������: �������������Ŀ��Ų�����Ŀ��Ϣ
 * �������: ��
 * �������: �����������Ŷ���Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekProgNum()
{
    char prog_num[15];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*����Ƿ��ҵ�����һ�������������Ŷ�*/
    char *prog_type;

    gotoxy(27, 2);
    printf("�����Ŷ���Ŀ��ţ�");
    gotoxy(45, 2);
    scanf("%s", prog_num);
    getchar();
    printf("\n\t�����Ŷ�\t��ʼʱ��\t��Ŀ���\t��Ŀ����\t��Ŀ������\t��Ŀ���\t\t");

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
                        prog_type = "973�ƻ�";
                        break;
                    case '2':
                        prog_type = "������Ȼ��ѧ������Ŀ";
                        break;
                    case '3':
                        prog_type = "863�ƻ�";
                        break;
                    case '4':
                        prog_type = "���ʺ�����Ŀ";
                        break;
                    case '5':
                        prog_type = "������Ŀ";
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
        printf("\n\t\t\t\t������������������Ŀ");
    }

    printf("\n\t\t\t\t���������������  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
 * ��������: SeekProgTeam
 * ��������: ���������ŶӲ�����Ŀ��Ϣ
 * �������: ��
 * �������: ������������Ŀ��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekProgTeam()
{
    char team_name[15];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*����Ƿ��ҵ�����һ��������������Ŀ*/
    char *prog_type;

    gotoxy(27, 2);
    printf("������Ŀ�����Ŷӣ�");
    gotoxy(45, 2);
    scanf("%s", team_name);
    getchar();
    printf("\n\t�����Ŷ�\t��ʼʱ��\t��Ŀ���\t��Ŀ����\t��Ŀ������\t��Ŀ���\t\t");

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
                        prog_type = "973�ƻ�";
                        break;
                    case '2':
                        prog_type = "������Ȼ��ѧ������Ŀ";
                        break;
                    case '3':
                        prog_type = "863�ƻ�";
                        break;
                    case '4':
                        prog_type = "���ʺ�����Ŀ";
                        break;
                    case '5':
                        prog_type = "������Ŀ";
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
        printf("\n\t\t\t\t����������������������Ŀ");
    }

    printf("\n\t\t\t���������������  ");
    getch();
    ClearScreen();
    ShowMenu();
}


/**
 * ��������: SeekTeamProf
 * ��������: ���ݽ�ʦ���������Ŷ���Ϣ
 * �������: ��
 * �������: �����������Ŷ���Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekTeamProf(void)
{
    char form_sign;
    int form_num;
    char form[10];
    DEPT_NODE *head = gp_head;
    DEPT_NODE *dept_sech = head;
    int flag = 0;    /*����Ƿ��ҵ�����һ�������������Ŷ�*/


    gotoxy(27, 2);
    printf("�������ʦ������ѯ��ʽ����ʽΪ��>��,'<','='���ϷǸ�������");
    gotoxy(31, 3);
    scanf("%c%s", &form_sign, form);
    getchar();
    printf("\n\t�Ŷ�����\t\t�Ӹ�����\t����Ժϵ\t��ʦ����\t�о�������");
    form_num = atoi(form);

    if (form_sign!='>'&&form_sign!='<'&&form_sign!='=')
	{
	    printf("\n");
		printf("\t\t\t\t�����������\n");
        printf("\n\t\t\t\t���������������  ");
        getch();
        ClearScreen();
        ShowMenu();
        return;
	}

	if (form_num<0)
    {
        printf("\n");
        printf("\t\t\t\t������������������Ǹ�����\n");
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
        printf("\n\t\t\t�����������������Ŷӡ���");
    }

    printf("\n\t\t\t\t���������������  ");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* ��������: StatRateInfo
* ��������: �����ͳ������Ժϵ��ʦ������
* �������: ��
* �������: ��
* �� �� ֵ: ��Ҫ�����ͳ�ƽ��
*
* ����˵��:
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
    printf("���水ѧ����ʦ���������Ժϵ��Ϣ��");
    printf("\n\t   Ժϵ����\t�о�������\t��ʦ����\tѧ����ʦ������");

    prate_node_temp = prate_node_ret;
    while(prate_node_temp!=NULL)
    {
        if(prate_node_temp->dept_prof_amount==0)
        {
            printf("\n");
            printf("\t   %-20s\t%-8d\t%-8d\t��ʦ��������Ϊ0", prate_node_temp->dept_name, prate_node_temp->dept_grd_amount, prate_node_temp->dept_prof_amount);
            prate_node_temp = prate_node_temp->next;
            continue;
        }
        printf("\n");
        printf("\t   %-20s\t%-8d\t%-8d\t%-8.2f", prate_node_temp->dept_name, prate_node_temp->dept_grd_amount, prate_node_temp->dept_prof_amount, prate_node_temp->proportion);
        prate_node_temp = prate_node_temp->next;
    }

    printf("\n\t\t\t     ���������������\n");
    getch();
    ClearScreen();
    ShowMenu();
    return;
}

/**
* ��������: StatRichProg
* ��������: ͳ�ƿ��о�����ߵ���Ŀ��������о�����ߵ�10����Ŀ
* �������: ��
* �������: ��Ҫ�����ͳ�ƽ��
* �� �� ֵ: ��
*
* ����˵��:
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
    printf("���水���о��ѳ���̶����10����Ŀ��Ϣ��\n");
    printf("\t�����Ŷ�\t��Ŀ����\t��Ŀ����������\t��Ŀ���");

    pprog_fee_node_temp = pprog_fee_node_ret;
    while(pprog_fee_node_temp!=NULL&&count<10)
    {
        count++;
        printf("\n");
        printf("\t%-12s\t%-8.2f\t%-14s\t%-15s",pprog_fee_node_temp->team_name,pprog_fee_node_temp->prog_fee,pprog_fee_node_temp->pmgr_name,pprog_fee_node_temp->prog_num);
        pprog_fee_node_temp = pprog_fee_node_temp->next;
    }

    printf("\n\t\t\t     ���������������\n");
    getch();
    ClearScreen();
    ShowMenu();

    return;
}

/**
* ��������: SortRichProg
* ��������: �Կ��о��ѳ�����Ŀ��Ϣ����������
* �������: pf_hd ���о��ѳ�����Ŀ��Ϣ��ͷָ��
* �������: pf_hd ������ͨ��ͷָ�뷵��
* �� �� ֵ: ��
*
* ����˵��:����StatRichProgĸ��������Ϊ�Ӻ�������
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

    /*����ʱ�����ڽ��������Ϣ����ʱ���*/
    ppf_node_temp = (PROG_FEE_NODE *) malloc(sizeof(PROG_FEE_NODE));
    while (ppf_node_prior->next != NULL) /*ѡ������*/
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

        /*����������ָ���򲻽���*/
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
* ��������: SortRateInfo
* ��������: ��Ժϵʦ����Ϣ����������
* �������: rate_hd Ժϵʦ����Ϣ��ͷָ��
* �������: rate_hd ������ͨ��ͷָ�뷵��
* �� �� ֵ: ��
*
* ����˵��:����StatRateInfoĸ��������Ϊ�Ӻ�������
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

    /*����ʱ�����ڽ��������Ϣ����ʱ���*/
    prate_node_temp = (RATE_NODE *) malloc(sizeof(RATE_NODE));
    while (prate_node_prior->next != NULL) /*ѡ������*/
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

        /*����������ָ���򲻽���*/
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
* ��������: StatDeptInfoPre
* ��������: ��Ժϵͳ��ĳ��ȵĿ�����Ŀ��Ϣ
* �������: ��
* �������: ��
* �� �� ֵ: ��Ҫ�����ͳ�ƽ��
*
* ����˵��:
*/
void StatDeptInfoPre(void)
{
    char year[5];
    printf("\n\t\t     ��������Ҫ��ѯ����ݣ���2017����");
    scanf("%s",year);
    getchar();
    if(strlen(year) != 4)
    {
        printf("\n\t\t     ��ݸ�ʽ������� \n\n\t\t     ���������������");
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
    printf("���水Ժϵ��Ŀ�����Ժϵ��Ϣ��\n");
    printf("\tԺϵ����\t973��Ŀ��\t863��Ŀ��\t������Ŀ����\t�����ܾ���");

    pdp_node_temp = head;
    while(pdp_node_temp!=NULL)
    {
        printf("\n");
        printf("\t%-12s\t%-8d\t%-8d\t%-8d\t%-.2f", pdp_node_temp->dept_name, pdp_node_temp->dept_prog_amount1, pdp_node_temp->dept_prog_amount3, pdp_node_temp->dept_prog_amount, pdp_node_temp->dept_fee);
        pdp_node_temp = pdp_node_temp->next;
    }

    printf("\n\t\t\t     ���������������\n");
    getch();
    ClearScreen();
    ShowMenu();

    return;
}


/**
 * ��������: StatDeptInfo
 * ��������: ��Ժϵͳ��ĳ��ȵĿ�����Ŀ��Ϣ
 * �������: ��
 * �������: ��
 * �� �� ֵ: DEPT_PROG_NODE *���͵�ͷ���
 *
 * ����˵��:
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
* ��������: SortDeptProgInfo
* ��������: ��Ժϵ��Ŀ��Ϣ����������
* �������: dp_hd Ժϵ��Ŀ��Ϣ��ͷָ��
* �������: dp_hd ������ͨ��ͷָ�뷵��
* �� �� ֵ: ��
*
* ����˵��:
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

    /*����ʱ�����ڽ��������Ϣ����ʱ���*/
    pdept_prog_node_temp = (DEPT_PROG_NODE *) malloc(sizeof(DEPT_PROG_NODE));
    while (pdept_prog_node_prior->next != NULL) /*ѡ������*/
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

        /*����������ָ���򲻽���*/
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
    char *pString[] = {"Ժϵʦ����Ŀ���",
                       "Ժϵ��Ŀ�������",
                       "����"
                      };
    hotnum = MyMenu3(pString, 3);/*ѡ��ͳ�ƶ���ķ�Χ*/
    switch (hotnum)
    {
        case 1:
            StatRateInfo();   /*ͳ��Ժϵʦ����Ŀ���*/
            break;
        case 2:
            StatDeptInfoPre();/*ͳ��Ժϵ��Ŀ�����*/
            break;
        case 3:
            break;
    }

    return TRUE;
}

BOOL StatTeam(void)
{
    int hotnum = 0;
    char *pString[] = {"��Ȼ������Ŀ���",
                       "��Ŀ��ʦ���������",
                       "����"
                      };
    hotnum = MyMenu3(pString, 3);/*ѡ��ͳ�ƶ���ķ�Χ*/
    switch (hotnum)
    {
        case 1:
            StatMostProg();/*ͳ�ƹ�����Ȼ������Ŀ�Ŷ����*/
            break;
        case 2:
            StatProgRate();/*ͳ���Ŷӿ�����Ŀ���ͽ�ʦ�����ı�ֵ���*/
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
                       "������ߵ���Ŀ",
                       "����"
                      };
    hotnum = MyMenu3(pString, 3);/*ѡ��ͳ�ƶ���ķ�Χ*/
    switch (hotnum)
    {
        case 1:
        case 2:
            StatRichProg();/*ͳ�ƾ��ѽϸߵĿ�����Ŀ���*/
            break;
        case 3:
            break;
    }

    return TRUE;
}



/**
* ��������: StatMostProg
* ��������: ��������Ȼ������Ŀ������������
* �������: ��
* �������: ����Ҫ�������
* �� �� ֵ: ��
*
* ����˵��:
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
    printf("���水������Ȼ������Ŀ��������Ŷ���Ϣ��");
    printf("\n\t�Ŷ�����\t\t������Ȼ������Ŀ����\t������Ȼ������Ŀ�ܾ���");

    pmost_prog_node_temp = pmost_prog_node_ret;
    while (pmost_prog_node_temp!=NULL && count<10)
    {
        count++;
        printf("\n");
        printf("\t    %-20s\t%-8d\t%-8.2f", pmost_prog_node_temp->team_name, pmost_prog_node_temp->team_prog_amount2, pmost_prog_node_temp->prog_fee2);
        pmost_prog_node_temp = pmost_prog_node_temp->next;
    }

    printf("\n\t\t\t���������������");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* ��������: StatProgRate
* ��������: ���Ŷ���Ŀ��ʦ��������������
* �������: ��
* �������: ����Ҫ�������
* �� �� ֵ: ��
*
* ����˵��:
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
    printf("���水�Ŷ���Ŀ��ʦ���������5���Ŷ���Ϣ��");
    printf("\n\t�Ŷ�����\t������Ŀ��\t��ʦ����\t��Ŀ��ʦ������");

    prog_prof_node_temp = prog_prof_node_ret;
    while (prog_prof_node_temp!=NULL && count<5)
    {
        count++;
        if(prog_prof_node_temp->team_prof_amount==0)
        {
            printf("\n");
            printf("\t%-12s\t%-8d\t%-8d\t��ʦ��������Ϊ0", prog_prof_node_temp->team_name, prog_prof_node_temp->team_prog_amount, prog_prof_node_temp->team_prof_amount);
            continue;
        }
        printf("\n");
        printf("\t%-12s\t%-8d\t%-8d\t%-8.2f", prog_prof_node_temp->team_name, prog_prof_node_temp->team_prog_amount, prog_prof_node_temp->team_prof_amount, prog_prof_node_temp->team_prog_prof_rate);
        prog_prof_node_temp = prog_prof_node_temp->next;
    }

    printf("\n\t\t\t\t���������������");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* ��������: SortProgProfInfo
* ��������: ���Ŷ���Ŀ��ʦ��������Ϣ����������
* �������: pp_hd �Ŷ���Ŀ��ʦ��������Ϣ��ͷָ��
* �������: pp_hd ������ͨ��ͷָ�뷵��
* �� �� ֵ: pp_hd ������ͨ��ͷָ�뷵��
*
* ����˵��:
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

    /*����ʱ�����ڽ��������Ϣ����ʱ���*/
    prog_prof_node_temp = (PROG_PROF_RATE_NODE *) malloc(sizeof(PROG_PROF_RATE_NODE));
    while (prog_prof_node_prior->next != NULL) /*ѡ������*/
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

        /*����������ָ���򲻽���*/
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
* ��������: SortMostProgInfo
* ��������: �Կ����Ŷӹ��һ�����Ŀ��Ϣ����������
* �������: mp_hd �����Ŷӹ��һ�����Ŀ��Ϣ��ͷָ��
* �������: mp_hd ������ͨ��ͷָ�뷵��
* �� �� ֵ: ��
*
* ����˵��:
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

    /*����ʱ�����ڽ��������Ϣ����ʱ���*/
    pmost_prog_temp = (MOST_PROG_NODE *) malloc(sizeof(MOST_PROG_NODE));
    while (pmost_prog_prior->next != NULL) /*ѡ������*/
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

        /*����������ָ���򲻽���*/
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
    char *pString[] = {"973��Ŀ�ص����",
                       "863��Ŀ�ص����",
                       "������Ȼ��ѧ������Ŀ����",
                       "����"
                      };
    while(hotnum != 4)
    {
        hotnum = MyMenu4(pString, 4);/*ѡ��ͳ�ƶ���ķ�Χ*/
        switch (hotnum)
        {
            case 1:
                Intro973();/*����973��Ŀ*/
                break;
            case 2:
                Intro863();/*����863��Ŀ*/
                break;
            case 3:
                IntroNationSci();/*���ܹ�����Ȼ��ѧ������Ŀ*/
                break;
        }
    }

    return TRUE;
}

void Intro973(void)
{
    char *info973[] = {"����ģʽ��ѧ��ר����ѯ��������������",
                       "����ģʽ��ѧ����Ŀ�����뾭�ѹ����л����",
                       "��֯ģʽ���ƣ��ಿ����ҵ��ͬ�ƽ��ƻ���չ",
                       "����"
                      };

    ShowModule(info973, 4);

    return;
}

void Intro863(void)
{
    char *info973[] = {"1.����ҹ�������������",
                       "2.���ս���ԡ�ǰ���Ժ�ǰհ��",
                       "3.�ٽ������о������Ŀ����",
                       "����"
                      };

    ShowModule(info973, 4);

    return;
}

void IntroNationSci(void)
{
    char *infonationsci[] = {"ս�Զ�λ��֧�ֻ����о������ӵ�������",
                             "�������룺�������¡�����δ��",
                             "ս�ԣ�Դͷ���£����촴�»���",
                             "����"
                            };

    ShowModule(infonationsci, 4);

    return;
}

BOOL HelpTopic(void)
{
    char *plabel_name[] = {"��ALT+X����ֱ���˳�����",
                           "��ALT+��ĸ���Կ��ѡ�����˵���",
                           "��F1�����Ի�ȡ�����˵�",
                           "���Ӳ˵������������esc�����˳��Ӳ˵�",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 5);

    return TRUE;
}

/**
 * ��������: SeekDept_All
 * ��������: ����Ժϵ��Ϣ�����������Ժϵ��Ϣ
 * �������: head ����ͷָ��
 * �������: ���е�Ժϵ������Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:���Ժ������ǲ���״̬������
 */
void SeekDept_All(DEPT_NODE *head)
{
    DEPT_NODE *pdept = head;
    while(pdept != NULL)
    {
        printf("\t\t\t     %-15s\t%-10s\n", pdept->dept_name, pdept->dmgr_name);
        pdept = pdept->next;
    }

    printf("\n\t\t\t\t�����������������");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* ��������: SeekTeam_All
* ��������: ������������л�����Ϣ
* �������: hd ����ͷָ��
* �������: �����Ŷӻ�����Ϣ
* �� �� ֵ: ��
*
* ����˵��:���Ժ������ǲ���״̬������
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
            /*��������������л�����Ϣ*/

            printf("%-15s\t%-15s\n", pt->dept_name, pt->team_name);
            pt = pt->next;
        }
        pd = pd->next;
    }

    printf("\n\t\t\t     �����������������\n");
    getch();
    ClearScreen();
    ShowMenu();
}

/**
* ��������: SeekProg_ALL
* ��������: ���������Ŀ������Ϣ�����ǲ��Ժ���
* �������: head ����ͷָ��
* �������: ���������Ŀ������Ϣ
* �� �� ֵ: ��
*
* ����˵��:���Ժ������ǲ���״̬������
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

    printf("\n\t\t\t     �����������������\n");
    getch();
    ClearScreen();
    ShowMenu();
}
