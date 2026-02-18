#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LEN 20
#define MAX_NAME_LEN 50
#define MAX_PWD_LEN 30
#define DATA_FILE "students.dat"   // 二进制数据文件
#define USER_FILE "users.dat"      // 用户账号文件
#define PAGE_SIZE 10               // 每页显示行数

#define ReGister   1
#define PwdReset    2//密码找回和密码重置时都在2里面，或者说，密码找回就是密码重置
#define Appeal      3

typedef struct Student {
    char id[MAX_ID_LEN];        // 学号
    char name[MAX_NAME_LEN];    // 姓名
    char gender[10];            // 性别
    int age;                    
    float score_c;              
    float score_math;           
    float score_eng;            
    float total;                
    float average;             
    
    struct Student *prev;       
    struct Student *next;       
} Student;

typedef struct User {
    char username[30];
    char password[MAX_PWD_LEN];
    char id[MAX_ID_LEN];
    int role; //1:学生, 2:教师, 3:管理员
} User;

typedef struct {
    int id;             //流水号
    int type;           //类型
    char sender[30];    //申请人ID
    char content[200];  //申诉内容
    int status;
} ToDoItem;

//全局变量
Student *head = NULL;
User currentUser;       

// 清除输入缓冲区
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 1. 输入辅助

void readString(char *buffer, int maxLen) {
    char temp[1024];
    if (fgets(temp, sizeof(temp), stdin)) {
        int len = strlen(temp);
        if (len > 0 && temp[len - 1] == '\n') {
            temp[len - 1] = '\0';
        }else {
            clearBuffer();//读取的超过maxlen了，最后一个不是换行符，要清除输入缓冲区
        }

        if (strlen(temp) >= maxLen) {
            temp[maxLen - 1] = '\0';
        }
        strcpy(buffer, temp);
    }
}

int readInt(char *prompt, int min, int max) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            //sscanf返回成功匹配并赋值的个数
            //从buffer匹配出整数并赋值给value
            if (sscanf(buffer, "%d", &val) == 1) {
                if (val >= min && val <= max) {
                    return val;
                }else {
                    printf("输入错误: 输入从 %d 到 %d 之间的整数。\n", min, max);
                }
            }else {
                printf("输入错误:输入有效的数字。\n");
            }
        }
    }
}

float readFloat(const char *prompt, float min, float max) {
    float val;
    char buffer[100];

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%f", &val) == 1) {
                if (val >= min && val <= max) {
                    return val;
                }else {
                    printf("输入错误: 数值必须在 %d 和 %d 之间\n", min, max);
                }
            }else {
                printf("输入错误: 请输入有效数字\n");
            }
        }
    }
}

void pauseStytem() {
    printf("\n按回车键继续...");
    getchar();
}

//2. 
//增删改查、创建dummyHead、创建新节点和数据的计算

void initList() {
    head = (Student*)malloc(sizeof(Student));
    if (head == NULL) {
        printf("内存分配失败。\n");
        exit(1);  //程序终止，1为退出码（错误），0为正常退出
    }
    head->prev = NULL;
    head->next = NULL;
    strcpy(head->id, "HEAD");//标记一下虚拟头节点
}

Student* createNode() {
    Student *node = (Student*)malloc(sizeof(Student));
    if (node == NULL) return NULL;
    memset(node, 0, sizeof(Student));
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void calculateStats(Student *node) {
    if (node) {
        node->total = node->score_c + node->score_eng + node->score_math;
        node->average = node->total / 3.0;
    }
}

void appendStudent(Student *newNode) {
    Student *cur = head;
    while (cur->next) {
        cur = cur->next;
    }
    
    cur->next = newNode;
    newNode->next = NULL;
    newNode->prev = cur;
}

Student* findStudentById(const char *id) {
    Student *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            return p;
        } 
        p = p->next;
    }
    return NULL;
}

int deleteStudentNode(const char *id) {
    Student *tar = findStudentById(id);
    if (tar == NULL) {
        return 0;
    }
    if (tar->prev) {
        tar->prev->next = tar->next;
    }
    if (tar->next ) {
        tar->next->prev = tar->prev;
    }

    free(tar);
    return 1; 
}

void freeAllList() {
    Student *p = head->next;
    Student *temp;
    while (p) {
        temp = p;
        p = p->next;
        free(temp);
    }
    head->next = NULL;
}

