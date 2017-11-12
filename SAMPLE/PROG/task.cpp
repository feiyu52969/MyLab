#include <windows.h>

#include "task.h"

//�^�X�N�̎��̂��`
static  TCB     Task[MAX_TASK_COUNT];

extern void TaskHead(TCB*);

void InitTask()
{
    TCB*     tmpTCB;

    //�S�^�X�N��������
    ZeroMemory(Task, sizeof(TCB)*MAX_TASK_COUNT);

    tmpTCB = Task;

    tmpTCB->Exec        = TaskHead;     //�����֐����i�[
    tmpTCB->Prio        = 0x0000;       //�ŏ��̃^�X�N�Ȃ̂ōŗD��Ŏ��s
    tmpTCB->Prev        = NULL;         //�ŏ��̃^�X�N�Ȃ̂őO�����^�X�N�ւ̃����N�͖���
    tmpTCB->Next        = Task;         //���������̓^�X�N��1�Ȃ̂ŁA
                                        //���Ɏ��s����̂����̃^�X�N
    tmpTCB->Flag        = _USE;         //�^�X�N���g�p���ł���t���O
}

TCB* TaskMake(void (*exec)(TCB*),unsigned int prio)
{
    TCB*    newTCB;
    TCB*    prevTCB;
    TCB*    nextTCB;
    int     id;

    //�󂢂Ă���^�X�N�̈��T��
    for(id=0; id<MAX_TASK_COUNT; id++){
        if(!(Task[id].Flag & _USE))break;
    }
    //�󂫗̈悪�����c�@�������s
    if(id == MAX_TASK_COUNT)return NULL;

    newTCB = &Task[id]; //���������I

    //�}������D�揇�ʂ�����
    prevTCB = Task;
    //�^�X�N�̏I�[�܂ł�����
    while(prevTCB->Next->Prio != 0x0000){
        if(prevTCB->Next->Prio > prio)break;
        prevTCB = prevTCB->Next;
    }
    nextTCB = prevTCB->Next;


    ZeroMemory(newTCB, sizeof(TCB));        //�O�ŏ�����

    newTCB->Exec        = exec;
    newTCB->Prio        = prio;

    //�^�X�N���X�g�ւ̑}��
    newTCB->Prev        = prevTCB;
    newTCB->Next        = nextTCB;
    prevTCB->Next       = newTCB;
    nextTCB->Prev       = newTCB;

    newTCB->Flag        = _USE;             //��������

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
    killTCB->Flag       = 0;                //���E����
}

void TaskExec( void )
{
    TCB*    execTCB;

    execTCB = Task;
    do{
        execTCB->Exec(execTCB);             //�e�^�X�N������
        execTCB = execTCB->Next;
    }while(execTCB->Prio != 0x0000);        //���X�g���ꏄ������I��
}

