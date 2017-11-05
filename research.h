#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

/**
 *科研项目信息链结点结构
 */
typedef struct prog_node {
    char prog_num[15];         	/**< 项目编号*/
    char type;                  /**< 项目类别*/
    char st_date[8];            /**< 起始时间*/
    char pmgr_name[12];         /**< 项目负责人姓名*/
    char team_name[30];         /**< 所属团队*/
    float prog_fee;           	/**< 项目经费*/
    struct prog_node *next;     /**< 指向下一结点的指针*/
} PROG_NODE;

/**
 科研团队信息链结点结构
 */
typedef struct team_node {
    char team_name[30];         /**< 团队名称*/
    char tmgr_name[12];         /**< 团队负责人姓名*/
    int team_prof_amount;       /**< 团队教师数*/
    int team_grd_amount;        /**< 团队研究生数*/
    struct prog_node *pnext;    /**< 指向科研项目基本信息支链的指针*/
    struct team_node *next;     /**< 指向下一结点的指针*/
    char dept_name[20];         /**< 所属院系名称*/
} TEAM_NODE;

/**
 *院系信息链结点结构
 */
 typedef struct dept_node {
    char dept_name[20];          /**< 院系名称*/
    char dmgr_name[12];          /**< 院系负责人*/
    char telno[15];              /**< 联系电话*/
    struct dept_node *next;      /**< 指向下一结点的指针*/
    struct team_node *tnext;     /**< 指向科研团队基本信息支链的指针*/
} DEPT_NODE;

/**
 *院系学生教师人数比统计信息链结点结构
 */
typedef struct rate_node {
    char dept_name[20];          /**< 院系名称*/
    int dept_prof_amount;        /**< 团队教师数*/
    int dept_grd_amount;         /**< 团队研究生数*/
    float proportion;            /**< 学生教师人数比*/
    struct rate_node *next;   	 /**< 指向下一结点的指针*/
} RATE_NODE;


/**
 *科研项目和经费信息链结点结构
 */
typedef struct dept_prog_node {
    int dept_prog_amount1;          /**< 973 项目数*/
    int dept_prog_amount3;          /**< 863 项目数*/
    float dept_fee;                 /**< 科研总经费*/
    int dept_prog_amount;           /**< 科研项目数*/
    char dept_name[20];             /**< 院系名称*/
    struct dept_prog_node *next;    /**< 指向下一结点的指针*/
} DEPT_PROG_NODE;

/**
 *国家自然基金科研项目最多的团队信息链结点结构
 */
typedef struct most_prog_node {
    char team_name[30];            	/**< 团队名称*/
    int team_prog_amount2;         	/**< 国家基金项目数*/
    float prog_fee2;          	    /**< 国家基金科研经费*/
    struct most_prog_node *next;     /**< 指向下一结点的指针*/
} MOST_PROG_NODE;

/**
 *项目教师人数比高的的团队信息链结点结构
 */
typedef struct prog_prof_rate_node {
    char team_name[30];            	/**< 团队名称*/
    int team_prof_amount;         	/**< 团队教师人数*/
    int team_prog_amount;         	/**< 团队科研项目数*/
    float team_prog_prof_rate;      /**< 项目教师人数比*/
    struct prog_prof_rate_node *next;     	/**< 指向下一结点的指针*/
} PROG_PROF_RATE_NODE;

/**
 *经费最充足的信息链结点结构
 */
typedef struct prog_fee_node {
    char team_name[30];            	/**< 所属团队名称*/
    char pmgr_name[12];            	/**< 项目负责人姓名*/
    char prog_num[15];            	/**< 项目编号*/
    float prog_fee;                 /**< 项目经费*/
    struct prog_fee_node *next;     /**< 指向下一结点的指针*/
} PROG_FEE_NODE;

/**
 *屏幕窗口信息链结点结点结构
 */
typedef struct layer_node {
    char LayerNo;            /**< 弹出窗口层数*/
    SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;

/**
 *标签束结构
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**
 *热区结构
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
    char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /**< 热区序号数组首地址*/
    int num;               /**< 热区个数*/
} HOT_AREA;

/*…………*/

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
DEPT_NODE *gp_head = NULL;                     /*主链头指针*/

char *gp_sys_name = "科研项目信息管理系统";    /*系统名称*/
char *gp_team_info_filename = "Team.dat";      /*科研团队基本信息数据文件*/
char *gp_prog_info_filename = "Prog.dat";      /*科研项目信息数据文件*/
char *gp_dept_info_filename = "Dept.dat";      /*院系信息数据文件*/
char *gp_type_code_filename = "type.dat";      /*项目类别代码数据文件*/

char *ga_main_menu[] = {"文件(F)",             /*系统主菜单名*/
    	                "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "帮助(H)"
                       };

char *ga_sub_menu[] = {"[S] 数据保存",          /*系统子菜单名*/
                       "[X] 退出    Alt+X",
                       "[D] 院系信息",
                       "[P] 科研团队信息",
                       "[C] 科研项目信息",
                       "[D] 院系信息",
                       "[P] 科研团队信息",
                       "[C] 科研项目信息",
                       "[I] 院系信息统计",
                       "[T] 科研团队信息统计",
                       "[C] 科研项目方面统计",
                       "[T] 帮助主题",
                       "",
                       "[A] 科研项目简介"
                      };

int ga_sub_menu_count[] = {2, 3, 3, 3, 3};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/
char *gp_dept_code = NULL;  /*存放院系基本信息表的数据缓冲区*/
char *gp_prog_code = NULL;  /*存放科研项目信息表的数据缓冲区*/
char *gp_team_code = NULL;  /*存放科研团队信息表的数据缓冲区*/
char gc_sys_state = '\0';   /*用来保存系统状态的字符*/

unsigned long gul_dept_code_len = 0;   /*院系信息代码表长度*/
unsigned long gul_prog_code_len = 0;   /*科研项目信息代码表长度*/
unsigned long gul_team_code_len = 0;   /*科研团队信息代码表长度*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/

int CreatList(DEPT_NODE **pphead);
void InitInterface(void);                 /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int DealInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(DEPT_NODE **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
unsigned long num_written;                  /* 该变量用于控制台输出函数 */
void CloseSys(DEPT_NODE *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);
void gotoxy(int x, int y);

BOOL LoadData(void);           /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RemoveData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/
BOOL HelpTopic(void);          /*帮助主题*/
BOOL AboutResearch(void);      /*关于科研项目的介绍*/

BOOL MaintainDeptInfo(void);         /*维护院系信息*/
BOOL MaintainProgInfo(void);         /*维护科研项目基本信息*/
BOOL MaintainTeamInfo(void);         /*维护科研团队基本信息*/

BOOL QueryDeptInfo(void);    /*查询院系基本信息*/
BOOL QueryProgInfo(void);    /*查询项目基本信息*/
BOOL QueryTeamInfo(void);    /*查询团队基本信息*/

BOOL StatDept(void);         /*统计院系有关信息*/
BOOL StatTeam(void);         /*统计团队有关信息*/
BOOL StatProg(void);         /*统计项目有关信息*/

void InsertDeptNode(DEPT_NODE* hd);  /*插入院系信息结点*/
void InsertTeamNode(DEPT_NODE* hd);  /*插入团队信息结点*/
void InsertProgNode(DEPT_NODE* hd);  /*插入项目信息结点*/
BOOL DelDeptNode(DEPT_NODE *hd);     /*删除院系信息结点*/
BOOL DelTeamNode(DEPT_NODE *hd);     /*删除团队信息结点*/
BOOL DelProgNode(DEPT_NODE *hd);     /*删除项目信息结点*/
BOOL ModifDeptNode(DEPT_NODE *hd);   /*修改院系信息结点*/
BOOL ModifTeamNode(DEPT_NODE *hd);   /*修改院系信息结点*/
BOOL ModifProgNode(DEPT_NODE *hd);   /*修改院系信息结点*/
void StatRateInfo(void);
void SortRateInfo(RATE_NODE *rate_hd); /*统计院系师生数目有关信息*/
void StatDeptInfoPre(void);
void SortDeptProgInfo(DEPT_PROG_NODE* dp_hd);
DEPT_PROG_NODE* StatDeptInfo(DEPT_NODE *hd, char year[5]);  /*统计院系项目数有关信息*/
void StatMostProg(void);
void SortMostProgInfo(MOST_PROG_NODE *mp_hd);                     /*统计国家自然基金最多的十个科研团队*/
void StatProgRate(void);
void SortProgProfInfo(PROG_PROF_RATE_NODE *pp_hd);                /*统计项目教师人数比*/
void StatRichProg(void);
void SortRichProg(PROG_FEE_NODE *pf_hd);                    /*统计科研经费最为充足的项目*/

/*test部分*/
void SeekDept_All(DEPT_NODE * head);
void SeekTeam_All(DEPT_NODE * head);
void SeekProg_All(DEPT_NODE * head);

void SeekDmgr(void);                                    /*根据输入的院系负责人姓名查找院系基本信息*/
void SeekDeptPart(void);                                /*以院系名称模糊查找院系基本信息*/
void SeekTeamPart(void);                                /*以团队名称模糊查找团队基本信息*/
void SeekTeamProf(void);                                /*以教师人数为条件查找满足条件的科研团队基本信息*/
void SeekProgNum(void);                                 /*按项目编号为条件查找科研项目信息*/
void SeekProgTeam(void);                                /*按所属团队为条件查找满足条件的科研项目信息*/

BOOL SaveSysData(DEPT_NODE *phd);                       /*保存系统数据*/
BOOL BackupSysData(DEPT_NODE *phd, char *filename);     /*备份系统数据*/
BOOL RemoveSysData(DEPT_NODE **pphd, char *filename);  /*恢复系统数据*/

/*介绍科研项目内容*/
BOOL AboutResearch(void);
void Intro973(void);
void Intro863(void);
void IntroNationSci(void);

int MyMenu4(char **pString, int n);
int MyMenu3(char **pString, int n);
void MakeDel();                                         /*提醒用户删除信息*/
void EscExit();                                         /*按下Esc键后清屏回主菜单*/

#endif /**< TYPE_H_INCLUDED*/
