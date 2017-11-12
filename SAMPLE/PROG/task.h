#ifndef  _TASK_H_
#define  _TASK_H_

//�t���O
#define _USE        (1<<0)          //TCB�g�p������t���O

#define MAX_TASK_COUNT  64          //�^�X�N�̍ő吔
#define TASK_WORK_SIZE  1024        //TCB���̃��[�N�T�C�Y Byte�P�ʂŎw��

//�^�X�N�Ǘ��̈�i����̓^�X�N���̂��́j
struct TCB{
    void            (*Exec)(TCB*);  //�^�X�N����������֐�
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
