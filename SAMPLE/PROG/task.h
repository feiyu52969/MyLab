#ifndef  _TASK_H_
#define  _TASK_H_

//フラグ
#define _USE        (1<<0)          //TCB使用中判定フラグ

#define MAX_TASK_COUNT  64          //タスクの最大数
#define TASK_WORK_SIZE  1024        //TCB内のワークサイズ Byte単位で指定

//タスク管理領域（今回はタスクそのもの）
struct TCB{
    void            (*Exec)(TCB*);  //タスクを処理する関数
    TCB*            Prev;
    TCB*            Next;
    unsigned int    Flag;
    unsigned int    Prio;
    int             Work[(TASK_WORK_SIZE+3)/4];
};

void InitTask();
TCB* TaskMake(void (*)(TCB*),unsigned int);
void TaskKill(TCB*);
void TaskChange(TCB*,void (*)(TCB*));
void TaskExec();
TCB* GetTaskTop();

#endif
