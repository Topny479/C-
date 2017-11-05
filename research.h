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

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

/**
 *������Ŀ��Ϣ�����ṹ
 */
typedef struct prog_node {
    char prog_num[15];         	/**< ��Ŀ���*/
    char type;                  /**< ��Ŀ���*/
    char st_date[8];            /**< ��ʼʱ��*/
    char pmgr_name[12];         /**< ��Ŀ����������*/
    char team_name[30];         /**< �����Ŷ�*/
    float prog_fee;           	/**< ��Ŀ����*/
    struct prog_node *next;     /**< ָ����һ����ָ��*/
} PROG_NODE;

/**
 �����Ŷ���Ϣ�����ṹ
 */
typedef struct team_node {
    char team_name[30];         /**< �Ŷ�����*/
    char tmgr_name[12];         /**< �ŶӸ���������*/
    int team_prof_amount;       /**< �Ŷӽ�ʦ��*/
    int team_grd_amount;        /**< �Ŷ��о�����*/
    struct prog_node *pnext;    /**< ָ�������Ŀ������Ϣ֧����ָ��*/
    struct team_node *next;     /**< ָ����һ����ָ��*/
    char dept_name[20];         /**< ����Ժϵ����*/
} TEAM_NODE;

/**
 *Ժϵ��Ϣ�����ṹ
 */
 typedef struct dept_node {
    char dept_name[20];          /**< Ժϵ����*/
    char dmgr_name[12];          /**< Ժϵ������*/
    char telno[15];              /**< ��ϵ�绰*/
    struct dept_node *next;      /**< ָ����һ����ָ��*/
    struct team_node *tnext;     /**< ָ������Ŷӻ�����Ϣ֧����ָ��*/
} DEPT_NODE;

/**
 *Ժϵѧ����ʦ������ͳ����Ϣ�����ṹ
 */
typedef struct rate_node {
    char dept_name[20];          /**< Ժϵ����*/
    int dept_prof_amount;        /**< �Ŷӽ�ʦ��*/
    int dept_grd_amount;         /**< �Ŷ��о�����*/
    float proportion;            /**< ѧ����ʦ������*/
    struct rate_node *next;   	 /**< ָ����һ����ָ��*/
} RATE_NODE;


/**
 *������Ŀ�;�����Ϣ�����ṹ
 */
typedef struct dept_prog_node {
    int dept_prog_amount1;          /**< 973 ��Ŀ��*/
    int dept_prog_amount3;          /**< 863 ��Ŀ��*/
    float dept_fee;                 /**< �����ܾ���*/
    int dept_prog_amount;           /**< ������Ŀ��*/
    char dept_name[20];             /**< Ժϵ����*/
    struct dept_prog_node *next;    /**< ָ����һ����ָ��*/
} DEPT_PROG_NODE;

/**
 *������Ȼ���������Ŀ�����Ŷ���Ϣ�����ṹ
 */
typedef struct most_prog_node {
    char team_name[30];            	/**< �Ŷ�����*/
    int team_prog_amount2;         	/**< ���һ�����Ŀ��*/
    float prog_fee2;          	    /**< ���һ�����о���*/
    struct most_prog_node *next;     /**< ָ����һ����ָ��*/
} MOST_PROG_NODE;

/**
 *��Ŀ��ʦ�����ȸߵĵ��Ŷ���Ϣ�����ṹ
 */
typedef struct prog_prof_rate_node {
    char team_name[30];            	/**< �Ŷ�����*/
    int team_prof_amount;         	/**< �Ŷӽ�ʦ����*/
    int team_prog_amount;         	/**< �Ŷӿ�����Ŀ��*/
    float team_prog_prof_rate;      /**< ��Ŀ��ʦ������*/
    struct prog_prof_rate_node *next;     	/**< ָ����һ����ָ��*/
} PROG_PROF_RATE_NODE;

/**
 *������������Ϣ�����ṹ
 */
typedef struct prog_fee_node {
    char team_name[30];            	/**< �����Ŷ�����*/
    char pmgr_name[12];            	/**< ��Ŀ����������*/
    char prog_num[15];            	/**< ��Ŀ���*/
    float prog_fee;                 /**< ��Ŀ����*/
    struct prog_fee_node *next;     /**< ָ����һ����ָ��*/
} PROG_FEE_NODE;

/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node {
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

/*��������*/

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
DEPT_NODE *gp_head = NULL;                     /*����ͷָ��*/

char *gp_sys_name = "������Ŀ��Ϣ����ϵͳ";    /*ϵͳ����*/
char *gp_team_info_filename = "Team.dat";      /*�����Ŷӻ�����Ϣ�����ļ�*/
char *gp_prog_info_filename = "Prog.dat";      /*������Ŀ��Ϣ�����ļ�*/
char *gp_dept_info_filename = "Dept.dat";      /*Ժϵ��Ϣ�����ļ�*/
char *gp_type_code_filename = "type.dat";      /*��Ŀ�����������ļ�*/

char *ga_main_menu[] = {"�ļ�(F)",             /*ϵͳ���˵���*/
    	                "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(H)"
                       };

char *ga_sub_menu[] = {"[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
                       "[X] �˳�    Alt+X",
                       "[D] Ժϵ��Ϣ",
                       "[P] �����Ŷ���Ϣ",
                       "[C] ������Ŀ��Ϣ",
                       "[D] Ժϵ��Ϣ",
                       "[P] �����Ŷ���Ϣ",
                       "[C] ������Ŀ��Ϣ",
                       "[I] Ժϵ��Ϣͳ��",
                       "[T] �����Ŷ���Ϣͳ��",
                       "[C] ������Ŀ����ͳ��",
                       "[T] ��������",
                       "",
                       "[A] ������Ŀ���"
                      };

int ga_sub_menu_count[] = {2, 3, 3, 3, 3};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char *gp_dept_code = NULL;  /*���Ժϵ������Ϣ������ݻ�����*/
char *gp_prog_code = NULL;  /*��ſ�����Ŀ��Ϣ������ݻ�����*/
char *gp_team_code = NULL;  /*��ſ����Ŷ���Ϣ������ݻ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/

unsigned long gul_dept_code_len = 0;   /*Ժϵ��Ϣ�������*/
unsigned long gul_prog_code_len = 0;   /*������Ŀ��Ϣ�������*/
unsigned long gul_team_code_len = 0;   /*�����Ŷ���Ϣ�������*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int CreatList(DEPT_NODE **pphead);
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(DEPT_NODE **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
unsigned long num_written;                  /* �ñ������ڿ���̨������� */
void CloseSys(DEPT_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);
void gotoxy(int x, int y);

BOOL LoadData(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RemoveData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*��������*/
BOOL AboutResearch(void);      /*���ڿ�����Ŀ�Ľ���*/

BOOL MaintainDeptInfo(void);         /*ά��Ժϵ��Ϣ*/
BOOL MaintainProgInfo(void);         /*ά��������Ŀ������Ϣ*/
BOOL MaintainTeamInfo(void);         /*ά�������Ŷӻ�����Ϣ*/

BOOL QueryDeptInfo(void);    /*��ѯԺϵ������Ϣ*/
BOOL QueryProgInfo(void);    /*��ѯ��Ŀ������Ϣ*/
BOOL QueryTeamInfo(void);    /*��ѯ�Ŷӻ�����Ϣ*/

BOOL StatDept(void);         /*ͳ��Ժϵ�й���Ϣ*/
BOOL StatTeam(void);         /*ͳ���Ŷ��й���Ϣ*/
BOOL StatProg(void);         /*ͳ����Ŀ�й���Ϣ*/

void InsertDeptNode(DEPT_NODE* hd);  /*����Ժϵ��Ϣ���*/
void InsertTeamNode(DEPT_NODE* hd);  /*�����Ŷ���Ϣ���*/
void InsertProgNode(DEPT_NODE* hd);  /*������Ŀ��Ϣ���*/
BOOL DelDeptNode(DEPT_NODE *hd);     /*ɾ��Ժϵ��Ϣ���*/
BOOL DelTeamNode(DEPT_NODE *hd);     /*ɾ���Ŷ���Ϣ���*/
BOOL DelProgNode(DEPT_NODE *hd);     /*ɾ����Ŀ��Ϣ���*/
BOOL ModifDeptNode(DEPT_NODE *hd);   /*�޸�Ժϵ��Ϣ���*/
BOOL ModifTeamNode(DEPT_NODE *hd);   /*�޸�Ժϵ��Ϣ���*/
BOOL ModifProgNode(DEPT_NODE *hd);   /*�޸�Ժϵ��Ϣ���*/
void StatRateInfo(void);
void SortRateInfo(RATE_NODE *rate_hd); /*ͳ��Ժϵʦ����Ŀ�й���Ϣ*/
void StatDeptInfoPre(void);
void SortDeptProgInfo(DEPT_PROG_NODE* dp_hd);
DEPT_PROG_NODE* StatDeptInfo(DEPT_NODE *hd, char year[5]);  /*ͳ��Ժϵ��Ŀ���й���Ϣ*/
void StatMostProg(void);
void SortMostProgInfo(MOST_PROG_NODE *mp_hd);                     /*ͳ�ƹ�����Ȼ��������ʮ�������Ŷ�*/
void StatProgRate(void);
void SortProgProfInfo(PROG_PROF_RATE_NODE *pp_hd);                /*ͳ����Ŀ��ʦ������*/
void StatRichProg(void);
void SortRichProg(PROG_FEE_NODE *pf_hd);                    /*ͳ�ƿ��о�����Ϊ�������Ŀ*/

/*test����*/
void SeekDept_All(DEPT_NODE * head);
void SeekTeam_All(DEPT_NODE * head);
void SeekProg_All(DEPT_NODE * head);

void SeekDmgr(void);                                    /*���������Ժϵ��������������Ժϵ������Ϣ*/
void SeekDeptPart(void);                                /*��Ժϵ����ģ������Ժϵ������Ϣ*/
void SeekTeamPart(void);                                /*���Ŷ�����ģ�������Ŷӻ�����Ϣ*/
void SeekTeamProf(void);                                /*�Խ�ʦ����Ϊ�����������������Ŀ����Ŷӻ�����Ϣ*/
void SeekProgNum(void);                                 /*����Ŀ���Ϊ�������ҿ�����Ŀ��Ϣ*/
void SeekProgTeam(void);                                /*�������Ŷ�Ϊ�����������������Ŀ�����Ŀ��Ϣ*/

BOOL SaveSysData(DEPT_NODE *phd);                       /*����ϵͳ����*/
BOOL BackupSysData(DEPT_NODE *phd, char *filename);     /*����ϵͳ����*/
BOOL RemoveSysData(DEPT_NODE **pphd, char *filename);  /*�ָ�ϵͳ����*/

/*���ܿ�����Ŀ����*/
BOOL AboutResearch(void);
void Intro973(void);
void Intro863(void);
void IntroNationSci(void);

int MyMenu4(char **pString, int n);
int MyMenu3(char **pString, int n);
void MakeDel();                                         /*�����û�ɾ����Ϣ*/
void EscExit();                                         /*����Esc�������������˵�*/

#endif /**< TYPE_H_INCLUDED*/
