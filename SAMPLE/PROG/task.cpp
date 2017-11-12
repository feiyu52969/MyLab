#include <windows.h>

#include "task.h"

//タスクの実体を定義
static  TCB     Task[MAX_TASK_COUNT];

extern void TaskHead(TCB*);

void InitTask()
{
    TCB*     tmpTCB;

    //全タスクを初期化
    ZeroMemory(Task, sizeof(TCB)*MAX_TASK_COUNT);

    tmpTCB = Task;

    tmpTCB->Exec        = TaskHead;     //処理関数を格納
    tmpTCB->Prio        = 0x0000;       //最初のタスクなので最優先で実行
    tmpTCB->Prev        = NULL;         //最初のタスクなので前処理タスクへのリンクは無し
    tmpTCB->Next        = Task;         //初期化時はタスクは1つなので、
                                        //次に実行するのもこのタスク
    tmpTCB->Flag        = _USE;         //タスクが使用中であるフラグ
}

TCB* TaskMake(void (*exec)(TCB*),unsigned int prio)
{
    TCB*    newTCB;
    TCB*    prevTCB;
    TCB*    nextTCB;
    int     id;

    //空いているタスク領域を探す
    for(id=0; id<MAX_TASK_COUNT; id++){
        if(!(Task[id].Flag & _USE))break;
    }
    //空き領域が無い…　生成失敗
    if(id == MAX_TASK_COUNT)return NULL;

    newTCB = &Task[id]; //生成成功！

    //挿入する優先順位を検索
    prevTCB = Task;
    //タスクの終端までを検索
    while(prevTCB->Next->Prio != 0x0000){
        if(prevTCB->Next->Prio > prio)break;
        prevTCB = prevTCB->Next;
    }
    nextTCB = prevTCB->Next;


    ZeroMemory(newTCB, sizeof(TCB));        //０で初期化

    newTCB->Exec        = exec;
    newTCB->Prio        = prio;

    //タスクリストへの挿入
    newTCB->Prev        = prevTCB;
    newTCB->Next        = nextTCB;
    prevTCB->Next       = newTCB;
    nextTCB->Prev       = newTCB;

    newTCB->Flag        = _USE;             //生成完了

    return newTCB;
}

TCB* GetTaskTop( void )
{
    return Task;
}

void TaskChange(TCB* chgTCB,void (*nextexec)(TCB*))
{
    chgTCB->Exec        = nextexec;
}

void TaskKill(TCB* killTCB)
{
    killTCB->Prev->Next = killTCB->Next;
    killTCB->Next->Prev = killTCB->Prev;
    killTCB->Flag       = 0;                //抹殺完了
}

void TaskExec( void )
{
    TCB*    execTCB;

    execTCB = Task;
    do{
        execTCB->Exec(execTCB);             //各タスクを処理
        execTCB = execTCB->Next;
    }while(execTCB->Prio != 0x0000);        //リストが一巡したら終了
}

