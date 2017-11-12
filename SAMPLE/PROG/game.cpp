#include <windows.h>
#include <Wingdi.h>
#include <math.h>
#include <stdio.h>

#define sqrtf sqrt
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>

#include "main.h"
#include "task.h"



////////////////////////////////////////////////////
//����
////////////////////////////////////////////////////
typedef struct{
    float           X;
    float           Y;
    float           timer;
    float           Count;
    unsigned int    Frame;
    RECT*           SrcRect;
} SPRITE;

void SpriteDraw(SPRITE* pspr,int bitmap_id)
{
    D3DXVECTOR3 pos;

    pos.x = pspr->X;    //���W�̐ݒ�
    pos.y = pspr->Y;
    pos.z = 0;

    //�X�v���C�g�̕`��
    g_pSp->Draw( g_pTex[bitmap_id],pspr->SrcRect,NULL,&pos,0xffffffff);
}

typedef struct{
    int             X;
    int             Y;
} BACK_GROUND;

void BGDraw(BACK_GROUND* bg,int bitmap_id)
{
    D3DXVECTOR3 pos;

    pos.x = bg->X;    //���W�̐ݒ�
    pos.y = bg->Y;
    pos.z = 0;

    //�w�i�̂̕`��
    g_pSp->Draw( g_pTex[bitmap_id],NULL,NULL,&pos,0xffffffff);
}


#define FONT_SIZE_X 16
#define FONT_SIZE_Y 16

void FontPrint(int x, int y,unsigned char* str)
{
    RECT rect;
    unsigned char alphabet;
    int strx,stry;
    int dispx,dispy;            //�\�����W
    D3DXVECTOR3 pos;

    //�\�����W��ۑ�
    dispx = x;
    dispy = y;

    while(*str != NULL)
    {
        alphabet = *str++;

        if(alphabet == 0x0a)    //���s����
        {
            dispx = x;
            dispy += FONT_SIZE_Y;
            continue;
        }

        //�����̕\������
        alphabet -= ' ';
        strx = (alphabet % 0x10) * FONT_SIZE_X;   //���̃r�b�g�}�b�v���W���v�Z
        stry = (alphabet / 0x10) * FONT_SIZE_Y;
        rect.top    = stry;
        rect.bottom = stry + FONT_SIZE_Y;
        rect.left   = strx;
        rect.right  = strx + FONT_SIZE_X;

        pos.x = dispx;
        pos.y = dispy;
        pos.z = 0;
        g_pSp->Draw( g_pTex[FONT_BITMAP_ID],&rect,NULL,&pos,0xffffffff); //�X�v���C�g�ŕ����\��

        dispx += FONT_SIZE_X;
    }
}





////////////////////////////////////////////////////
//�R��
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�R�́|�S�`�T
////////////////////////////////////////////////////

#define SCENE_INIT          -1    //������
#define SCENE_CHANGE_WAIT   0     //�؂�ւ����̑҂����
#define SCENE_TITLE         1     //�^�C�g���V�[��
#define SCENE_GAME          2     //�Q�[����
#define SCENE_GAME_OVER     3     //�Q�[���I�[�o�[
#define SCENE_OPTION        4     //�I�v�V�������
#define SCENE_END           5     //�V�[�����I��������


typedef struct{
    int         SceneID;
    int         Time;
} EX3_SCENE_STRUCT;
static int   g_SCENE_CHANGE_COMM;

void exec03_SCENE_GAME(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

     FontPrint( 208, 160,"NOW GAME SCENE");
    //��莞�Ԃ��Ƃɓ_�ł��J��Ԃ�
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 208, 240,"PUSH 'Z' GAME OVER");
    }

    //�V�[���̐؂�ւ����V�[���Ǘ��^�X�N�ɓ`����
    if( g_DownInputBuff & KEY_Z )
    {
        g_SCENE_CHANGE_COMM = SCENE_END;
        TaskKill(thisTCB);
        return;
    }
}
void exec03_SCENE_GAME_OVER(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

     FontPrint( 208, 160,"GAME OVER");

    //��莞�Ԃ��Ƃɓ_�ł��J��Ԃ�
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 256,"PUSH 'Z' CONTINUE");
      FontPrint( 176, 272,"PUSH 'X' TITLE");
    }

    //�V�[���̐؂�ւ����V�[���Ǘ��^�X�N�ɓ`����
    if( g_DownInputBuff & KEY_Z )
    {
        g_SCENE_CHANGE_COMM = SCENE_GAME;    //�Q�[���𑱍s����
        TaskKill(thisTCB);
        return;
    }
    if( g_DownInputBuff & KEY_X )
    {
        g_SCENE_CHANGE_COMM = SCENE_TITLE;  //�^�C�g���֖߂�
        TaskKill(thisTCB);
        return;
    }
}
void exec03_SCENE_OPTION(TCB* thisTCB)
{
#define RANK_MAX 5
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;
    char str[128];

//�Q�[�����̃����N
//�{���͊O������Q�Ƃł���悤�O���[�o���ϐ��ɂ��邩
//�֐��ɂ�鏈���ɂ���
static int RANK = 2;


    //�����N�̑�������
    if( g_DownInputBuff & KEY_RIGHT ) RANK++;
    if( g_DownInputBuff & KEY_LEFT  ) RANK--;
    //�����N����l�A�����l�`�F�b�N
    if( RANK >= RANK_MAX ) RANK = RANK_MAX-1;
    if( RANK <  0        ) RANK = 0;



     //�\������
     FontPrint( 208, 160,"GAME OPTION");
     //�����N�̕\��
     sprintf( str, "SELECT RANK %d",RANK+1);
     FontPrint( 208, 208,str);

    //��莞�Ԃ��Ƃɓ_�ł��J��Ԃ�
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 240,"PUSH 'X' OPTION EXIT");
    }

    //�V�[���̐؂�ւ����V�[���Ǘ��^�X�N�ɓ`����
    if( g_DownInputBuff & KEY_X )
    {
        g_SCENE_CHANGE_COMM = SCENE_END;
        TaskKill(thisTCB);
        return;
    }
}

void exec03_SCENE_TITLE(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

     FontPrint( 176, 160,"GAME TITLE SCREEN");

    //��莞�Ԃ��Ƃɓ_�ł��J��Ԃ�
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 256,"PUSH 'Z' GAME START");
      FontPrint( 176, 272,"PUSH 'X' OPTION");
    }

    //�V�[���̐؂�ւ����V�[���Ǘ��^�X�N�ɓ`����
    if( g_DownInputBuff & KEY_Z )
    {
        //�Q�[����I��
        g_SCENE_CHANGE_COMM = SCENE_GAME;
        TaskKill(thisTCB);
        return;
    }
    if( g_DownInputBuff & KEY_X )
    {
        //�I�v�V������I��
        g_SCENE_CHANGE_COMM = SCENE_OPTION;
        TaskKill(thisTCB);
        return;
    }
}


void init03_SCENE_CTRL(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

    //���������̏��
     pEX3_SCENEwork->SceneID = SCENE_INIT;

}

void exec03_SCENE_CTRL(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

    switch( pEX3_SCENEwork->SceneID )
    {
        case SCENE_INIT: //��������
                pEX3_SCENEwork->SceneID = SCENE_TITLE;
                TaskMake( exec03_SCENE_TITLE, 0x2000 );
                g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                break;
        case SCENE_TITLE: //�^�C�g�����
                if( g_SCENE_CHANGE_COMM != SCENE_CHANGE_WAIT )
                {
                    if( g_SCENE_CHANGE_COMM == SCENE_GAME )
                    {
                        TaskMake( exec03_SCENE_GAME, 0x2000 );
                        pEX3_SCENEwork->SceneID = SCENE_GAME;
                    } else 
                    if( g_SCENE_CHANGE_COMM == SCENE_OPTION )
                    {
                        TaskMake( exec03_SCENE_OPTION, 0x2000 );
                        pEX3_SCENEwork->SceneID = SCENE_OPTION;
                    }
                        g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                }
                break;
        case SCENE_GAME: //�Q�[������
                if( g_SCENE_CHANGE_COMM != SCENE_CHANGE_WAIT )
                {
                    pEX3_SCENEwork->SceneID = SCENE_GAME_OVER;
                    TaskMake( exec03_SCENE_GAME_OVER, 0x2000 );
                    g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                }
                break;
        case SCENE_GAME_OVER:  //�Q�[���I�[�o�[����
                if( g_SCENE_CHANGE_COMM != SCENE_CHANGE_WAIT )
                {
                    if( g_SCENE_CHANGE_COMM == SCENE_GAME )
                    {
                        pEX3_SCENEwork->SceneID = SCENE_GAME;
                        TaskMake( exec03_SCENE_GAME, 0x2000 );
                    } else 
                    if( g_SCENE_CHANGE_COMM == SCENE_TITLE )
                    {
                        pEX3_SCENEwork->SceneID = SCENE_TITLE;
                        TaskMake( exec03_SCENE_TITLE, 0x2000 );
                    }
                    g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                }
                break;
        case SCENE_OPTION:  //�I�v�V�������
                if( g_SCENE_CHANGE_COMM != SCENE_CHANGE_WAIT )
                {
                    pEX3_SCENEwork->SceneID = SCENE_TITLE;
                    TaskMake( exec03_SCENE_TITLE, 0x2000 );
                    g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                }
                break;
    }
}



////////////////////////////////////////////////////
//�R�́|�k�h�r�s �P
////////////////////////////////////////////////////


typedef struct{
    SPRITE          sprt;
    float           AddX;
    float           AddY;
} EX3_1_STRUCT;


#define EX3_1_MOVE_COUNT   16


void exec03_01_rader(TCB* thisTCB)
{
#define RADER_POS_X  0
#define RADER_POS_Y  0

    EX3_1_STRUCT* pEX3_1pos_data;
    SPRITE sprt;
    int loop;
    RECT pix_rect = { 0, 0, 2, 2};

    sprt.X = RADER_POS_X;
    sprt.Y = RADER_POS_Y;
    sprt.SrcRect = NULL;
    SpriteDraw(&sprt,2);

    sprt.SrcRect = &pix_rect;

    for( loop = 0; loop < EX3_1_MOVE_COUNT; loop++)
    {
        //���[�N�Ɋi�[����Ă���͕̂\�����鑊��̃f�[�^�|�C���^
        pEX3_1pos_data =  (EX3_1_STRUCT*)thisTCB->Work[ loop ];

        //�\�����W���擾
        sprt.X = pEX3_1pos_data->sprt.X;
        sprt.Y = pEX3_1pos_data->sprt.Y;
        //���W�̕ϊ����s��
        sprt.X = sprt.X / 10 + RADER_POS_X;
        sprt.Y = sprt.Y / 10 + RADER_POS_Y;
        //���[�_�[�L�������X�v���C�g�ŕ\��
        SpriteDraw(&sprt,1);

    }

}



void exec03_01_move(TCB* thisTCB)
{
#define WALL_TOP    0       //�Ǐ���W
#define WALL_BOTTOM 480     //�ǉ����W
#define WALL_LEFT   0       //�ǉE���W
#define WALL_RIGHT  640     //�Ǎ����W

    EX3_1_STRUCT* pEX3_1work;
    pEX3_1work = (EX3_1_STRUCT*)thisTCB->Work;


    //�����A�i�s�����悪�ǂł���Ε����𔽓]������
    if( (pEX3_1work->sprt.X+32 + pEX3_1work->AddX >= WALL_RIGHT) ||       //�{�R�Q�͕\�����̑傫�����l��
        (pEX3_1work->sprt.X    + pEX3_1work->AddX <= WALL_LEFT ) )
    {
        pEX3_1work->AddX = -pEX3_1work->AddX;      
    }
    if( (pEX3_1work->sprt.Y+32 + pEX3_1work->AddY >= WALL_BOTTOM) ||
        (pEX3_1work->sprt.Y    + pEX3_1work->AddY <= WALL_TOP   ) )
    {
        pEX3_1work->AddY = -pEX3_1work->AddY;      
    }

    pEX3_1work->sprt.X += pEX3_1work->AddX;
    pEX3_1work->sprt.Y += pEX3_1work->AddY;

    SpriteDraw(&pEX3_1work->sprt,0);

}

void init03_01(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\PIX.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\RADER.png",&g_pTex[2] );
}

void exec03_01(TCB* thisTCB)
{

#define ADD_X 8.0
#define ADD_Y 8.0
#define ADD_SPEED 20.0
    TCB* pnew_task;
    TCB* prader_task;
    EX3_1_STRUCT* pnew_work;
    EX3_1_STRUCT* pEX3_1work = (EX3_1_STRUCT*)thisTCB->Work;
    EX3_1_STRUCT* pEX3_rader;

    int loop;
    //���[�_�[�����̍쐬
    prader_task = TaskMake(exec03_01_rader, 0x8000);

    //�����\�������̏������ƋN��
    for(loop = 0;loop < EX3_1_MOVE_COUNT; loop++)
    {
        pnew_task = TaskMake(exec03_01_move, 0x2000);
        pnew_work = (EX3_1_STRUCT*)pnew_task->Work;

        //���[�_�[�����ׂ̈ɓo�^
        prader_task->Work[ loop ] = (int)pnew_work;


        pnew_work->AddX  = (((rand() % 100) / 100.0) - 0.5) * ADD_SPEED;
        pnew_work->AddY  = (((rand() % 100) / 100.0) - 0.5) * ADD_SPEED;

        pnew_work->sprt.X = SCREEN_WIDTH / 2 - 32;           //�������W
        pnew_work->sprt.Y = SCREEN_HEIGHT / 2 - 32;
    }

    TaskKill(thisTCB);
}




////////////////////////////////////////////////////
//�R�́|�P�P  �p�b�P�[�W��
////////////////////////////////////////////////////
typedef struct{
    //�擪����̃t�@�C���̈ʒu
    unsigned int   FilePoint;
    //�t�@�C���T�C�Y
    unsigned int   FileSize;
    //�t�@�C����
    TCHAR          FileName[ MAX_PATH ];  
} EX03_11_DATA;
#define FILE_MAX    32
#define HEADER_SIZE (sizeof(EX03_11_DATA) * FILE_MAX + sizeof(int))

int EX03_11_archive_data( char* DirName, char* SaveName)
{
    int file_count = 0;    //�t�@�C����
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    //�t�@�C���擪����̈ʒu
    unsigned int file_point = HEADER_SIZE;

    FILE* archive_file;
    FILE* read_file;
    //��������t�@�C����
    char search_name[MAX_PATH];
    char new_name[MAX_PATH];
    unsigned int copy_byte;
    int loop;
    unsigned int copy_loop;

    //�A�[�J�C�u�t�@�C���̃w�b�_����
static EX03_11_DATA file_header[FILE_MAX];


    //��������t�@�C�������쐬
    //�t�H���_���̑S�t�@�C�����w�肷��
    strcpy(search_name,DirName);
    strcat(search_name,"*.*");

    //�ŏ��̂P�x�������s
    hFind = FindFirstFile( search_name, &FindFileData);
    //�ŏ��̃t�@�C���̓t�H���_�Ȃ̂ł͂���
    FindNextFile( hFind, &FindFileData);
    while(true)
    {
        //�t�@�C��������
        if( FindNextFile( hFind, &FindFileData) == 0)
        {//�G���[�`�F�b�N
            if( GetLastError () == ERROR_NO_MORE_FILES )
            {//�t�@�C���������Ȃ���Ό������[�v���I��
                break;
            }else{
              //�����łȂ���΃G���[�Ŗ߂�
                FindClose(hFind);
                return true ;
            }
        }

        //�w�b�_�ɏ�����������
        file_header[ file_count ].FilePoint = file_point;
        file_header[ file_count ].FileSize  = FindFileData.nFileSizeLow;
        //�t�@�C�����̃R�s�[
        strcpy( file_header[ file_count ].FileName, FindFileData.cFileName);
        //�i�[����t�@�C���̐擪����̈ʒu�v�Z
        file_point += FindFileData.nFileSizeLow;

        //�t�@�C�������J�E���g����
        file_count++;
    }
    //�����I��
    FindClose(hFind);

    //�o�C�i���������݃��[�h�ł܂Ƃ߂�t�@�C�����J��
    archive_file = fopen( SaveName, "w+b");

    //�ŏ��Ƀw�b�_�������L�^����
    //�t�@�C����
    fwrite( &file_count, sizeof(int) , 1, archive_file );
    //�w�b�_�{�́i�t�@�C�������L�^���������炷�j
    fwrite( file_header, HEADER_SIZE-sizeof(int) , 1, archive_file );

    for(loop=0; loop < file_count; loop++ )
    {//�ʂ̃t�@�C�����P�̃t�@�C���ɃR�s�[����
        strcpy(new_name,DirName);
        strcat(new_name, file_header[loop].FileName);
        read_file = fopen(new_name, "r+b");

        for(copy_loop = 0; copy_loop < file_header[loop].FileSize; copy_loop++)
        {//�t�@�C���T�C�Y�������J��Ԃ�
            fread(  &copy_byte, 1, 1, read_file );
            fwrite( &copy_byte, 1, 1, archive_file );
        }

        fclose( read_file );
    }

    //�t�@�C�������
    fclose( archive_file );

    //����I��
    return false;
}

void init03_11(TCB* thisTCB)
{
    //�܂Ƃ߂�t�H���_
    char* archive_file  = "..\\..\\data\\EX03_11\\";
    //�܂Ƃ߂��f�[�^�̃t�@�C����
    char* archive_name = "..\\..\\data\\EX03_11.DAT";

    EX03_11_archive_data( archive_file,archive_name );

}


int EX03_11_getFileCount( char* Buff )
{
    //�擪�Ƀt�@�C�������i�[����Ă���
    return  *((int*)Buff);
}

unsigned int EX03_11_getFileSize( char* Buff ,int ID)
{
    //�f�[�^�z��̐擪���|�C���g����
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //�t�@�C���T�C�Y��Ԃ�
    return data[ID].FileSize;
}

char* EX03_11_getFileName( char* Buff ,int ID)
{
    //�f�[�^�z��̐擪���|�C���g����
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //�t�@�C�����ւ̃|�C���^��Ԃ�
    return data[ID].FileName;
}

char* EX03_11_getFilePoint( char* Buff ,int ID)
{
    //�f�[�^�z��̐擪���|�C���g����
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //�o�b�t�@�|�C���^����t�@�C���i�[�ʒu�ւ̃|�C���^���Z�o
    return (char*)(data[ID].FilePoint+(int)Buff);
}



void exec03_11(TCB* thisTCB)
{
    //�t�@�C���̓ǂݍ��݃o�b�t�@�i32K)
    char read_buff[1024*32];
    int file_count;
    char* file_name;
    int   file_size;
    char* file_point;
    int loop;
    int archive_size;
    FILE* archive_file;
    FILE* write_file;

    //�ǂݍ��ݏ���
    archive_file = fopen( "..\\..\\data\\EX03_11.DAT","r+b");
    fseek( archive_file, 0, SEEK_END );
    //�t�@�C���T�C�Y���擾
    archive_size = ftell( archive_file);
    fseek( archive_file, 0, SEEK_SET );
    //�t�@�C����S���ǂݍ���
    fread( &read_buff, 1, archive_size, archive_file );
    //�ǂݍ��ݏI����t�@�C�������
    fclose( archive_file );


    //�t�@�C�����̎擾
    file_count = EX03_11_getFileCount(read_buff);
    for(loop = 0; loop< file_count; loop++)
    {//�܂Ƃ��t�@�C�����o���o���ɏ�������
        //�������݂ɕK�v�Ȋe������擾
        //�t�@�C����
        file_name  = EX03_11_getFileName (read_buff,loop);
        //�t�@�C���T�C�Y
        file_size  = EX03_11_getFileSize (read_buff,loop);
        //�i�[����Ă���t�@�C���ւ̃|�C���^
        file_point = EX03_11_getFilePoint(read_buff,loop);

        write_file = fopen(file_name ,"w+b");
        fwrite( file_point, file_size, 1, write_file);
        fclose( write_file );
    }

    //�����̏I��
    TaskKill(thisTCB);

}







////////////////////////////////////////////////////
//�R�́|�P�R  �|�[�Y��������
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
} EX03_13_STRUCT;

//�|�[�Y�t���O
int g_EX03_13_PAUSE_FLAG;

void exec03_13_Ball(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�{�[���̈ړ��������� �|�[�Y���ɂ͏������Ȃ�
    if( !g_EX03_13_PAUSE_FLAG )
    {
        work->Y += MOVE_SPEED;
        if( work->Y  > SCREEN_HEIGHT ) work->Y = 0;    //��ʊO�ɓ��B��������W��߂�
    }

    //�`�敔���̓|�[�Y���ł���ɕ`�悵������
    SpriteDraw(work,0);
}

void init03_13(TCB* thisTCB)
{
#define BALL_COUNT 32
    TCB*    tmp_tcb;
    SPRITE* ball_sprt;
    int loop;


    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //�{�[���̍쐬�ƍ��W�̏�����
    for( loop = 0; loop < BALL_COUNT; loop++)
    {
        tmp_tcb = TaskMake( exec03_13_Ball, 0x1000 );
        ball_sprt = (SPRITE*)tmp_tcb->Work;

        ball_sprt->X = rand() / (RAND_MAX / SCREEN_WIDTH);
        ball_sprt->Y = rand() / (RAND_MAX / SCREEN_HEIGHT);
    }
    //�|�[�Y�t���O�̏�����
    g_EX03_13_PAUSE_FLAG = 0;
}

void exec03_13(TCB* thisTCB)
{
    //�|�[�Y�t���O�̏���
    if( g_DownInputBuff & KEY_Z )
    {//�{�^�����������тɃt���O�𔽓]�����A�|�[�Y�ƃ|�[�Y�������s��
        g_EX03_13_PAUSE_FLAG = !g_EX03_13_PAUSE_FLAG;
    }
}






////////////////////////////////////////////////////
//�R�́|�P�S  �O���[�v�ʂɃ|�[�Y��������
////////////////////////////////////////////////////

//�|�[�Y�t���O
#define PAUSE_GROUP_A  (1 << 0)
#define PAUSE_GROUP_B  (1 << 1)
#define PAUSE_GROUP_C  (1 << 2)
#define PAUSE_GROUP_D  (1 << 3)
int g_EX03_14_PAUSE_FLAG;

//�{�[����ނ`
void exec03_14_BallA(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�{�[���̈ړ��������� �|�[�Y���ɂ͏������Ȃ�
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_A) )
    {
        work->Y += MOVE_SPEED;
        if( work->Y  > SCREEN_HEIGHT ) work->Y = 0;    //��ʊO�ɓ��B��������W��߂�
    }
    //�`�敔���̓|�[�Y���ł���ɕ`�悵������
    SpriteDraw(work,0);
}

//�{�[����ނa
void exec03_14_BallB(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�{�[���̈ړ��������� �|�[�Y���ɂ͏������Ȃ�
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_B) )
    {
        work->Y -= MOVE_SPEED;
        if( work->Y  < 0 ) work->Y = SCREEN_HEIGHT;    //��ʊO�ɓ��B��������W��߂�
    }
    //�`�敔���̓|�[�Y���ł���ɕ`�悵������
    SpriteDraw(work,0);
}

//�{�[����ނb
void exec03_14_BallC(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�{�[���̈ړ��������� �|�[�Y���ɂ͏������Ȃ�
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_C) )
    {
        work->X += MOVE_SPEED;
        if( work->X  > SCREEN_WIDTH ) work->X = 0;    //��ʊO�ɓ��B��������W��߂�
    }
    //�`�敔���̓|�[�Y���ł���ɕ`�悵������
    SpriteDraw(work,0);
}

//�{�[����ނc
void exec03_14_BallD(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�{�[���̈ړ��������� �|�[�Y���ɂ͏������Ȃ�
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_D) )
    {
        work->X -= MOVE_SPEED;
        if( work->X  < 0 ) work->X = SCREEN_WIDTH;    //��ʊO�ɓ��B��������W��߂�
    }
    //�`�敔���̓|�[�Y���ł���ɕ`�悵������
    SpriteDraw(work,0);
}

void init03_14(TCB* thisTCB)
{
#define BALL_COUNT 32
    TCB*    tmp_tcb;
    SPRITE* ball_sprt;
    int loop;


    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //�{�[���̍쐬�ƍ��W�̏�����
    for( loop = 0; loop < BALL_COUNT; loop++)
    {
        switch( loop & 0x03 )    //�{�[���̎�ނ��S�ɕ�����
        {
            case 0:  tmp_tcb = TaskMake( exec03_14_BallA, 0x1000 ); break;
            case 1:  tmp_tcb = TaskMake( exec03_14_BallB, 0x1000 ); break;
            case 2:  tmp_tcb = TaskMake( exec03_14_BallC, 0x1000 ); break;
            case 3:  tmp_tcb = TaskMake( exec03_14_BallD, 0x1000 ); break;
        }

        ball_sprt = (SPRITE*)tmp_tcb->Work;

        ball_sprt->X = rand() / (RAND_MAX / SCREEN_WIDTH);
        ball_sprt->Y = rand() / (RAND_MAX / SCREEN_HEIGHT);
    }
    //�|�[�Y�t���O�̏�����
    g_EX03_14_PAUSE_FLAG = 0;
}

void exec03_14(TCB* thisTCB)
{
    //�|�[�Y�t���O�̏����A�����L�[�ɍ��킹�ăt���O���]����
    //�e�L�[���������тɁA�|�[�Y�ƃ|�[�Y�������s��

    if( g_DownInputBuff & KEY_UP )
    {//��
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_A;
    }

    if( g_DownInputBuff & KEY_DOWN )
    {//��
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_B;
    }

    if( g_DownInputBuff & KEY_LEFT )
    {//��
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_C;
    }

    if( g_DownInputBuff & KEY_RIGHT )
    {//�E
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_D;
    }
}





////////////////////////////////////////////////////
//�R�́|�P�U  ���݂̂e�o�r��m��
////////////////////////////////////////////////////
#define SECOND 1000.0
static int g_EX03_16_FramePerSecond;

void CALLBACK EX03_16_time_callback( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
static float record_time;
    //�P�b�ԂɍX�V���ꂽ�t���[�������L�^
    g_EX03_16_FramePerSecond = g_Count - record_time;
    record_time = g_Count;
}

void init03_16(TCB* thisTCB)
{
    //�P�b���ɌĂяo���֐���ݒ肷��
    SetTimer( NULL, NULL, SECOND, EX03_16_time_callback );

    g_EX03_16_FramePerSecond = 0;
}

void exec03_16(TCB* thisTCB)
{
    float fps;
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    //�e�o�r�̕\��
    sprintf( str,"���݂̂e�o�r %d",g_EX03_16_FramePerSecond);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�R�́|�P�W  ���_�̊Ǘ�
////////////////////////////////////////////////////

typedef struct{
    int            Score;
    int            ScoreID;
} EX03_18_STRUCT;

int EX03_18_get_score( int score_id )
{
#define SPECIAL_SCORE1 -1
#define SPECIAL_SCORE2 -2
static int ScoreTable[] = 
{
    100,
    1000,
    SPECIAL_SCORE1,
    2550,
    1280,
    500,
    SPECIAL_SCORE2,
    10,
};
    int score;


    score = ScoreTable[ score_id ];

    if( score < 0 )
    {//�����Ȃ����X�R�A�Ƃ��ăX�R�A�v�Z
        switch ( score )
        {
            case SPECIAL_SCORE1: score = g_Count * 10; break;
            case SPECIAL_SCORE2: score = 10000 + g_Count * 50; break;
        }
    }

    return score;
}

void exec03_18(TCB* thisTCB)
{
    EX03_18_STRUCT* work = (EX03_18_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�y�L�[�ŃX�R�A�����Z
    if( g_DownInputBuff & KEY_Z ) work->Score += EX03_18_get_score( work->ScoreID );
    //�w�L�[��ID��؂�Ԃ�
    if( g_DownInputBuff & KEY_X )
    {
        work->ScoreID++;
        work->ScoreID &= 0x07;
    }

    //�X�R�A�̕\��
    sprintf( str,"SCORE: %8d  ID%d\n���Z�X�R�A�F%8d",work->Score,work->ScoreID,EX03_18_get_score( work->ScoreID ));
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�R�́|�P�X  �P���ȃZ�[�u�E���[�h
////////////////////////////////////////////////////

#define RANK_COUNT 5
#define DATA_FILE_NAME "EX03_19_SCORE_DATA.DAT"

typedef struct{
    int            Score;
    char           Name[4];
} EX03_19_STRUCT;

static EX03_19_STRUCT  gEX03_19_SAVE_DATA[ RANK_COUNT ];


void EX03_19_score_save( char* FILE_NAME, EX03_19_STRUCT* SaveData )
{
    FILE* save_file;

    //�o�C�i���������݃��[�h�Ńt�@�C�����I�[�v��
    save_file = fopen( FILE_NAME, "w+b");

    //�X�R�A�f�[�^���Z�[�u����
    fwrite( SaveData, sizeof( EX03_19_STRUCT ) , RANK_COUNT, save_file );

    //�t�@�C�������
    fclose( save_file );
}

int EX03_19_score_load( char* FileName, EX03_19_STRUCT* LoadData )
{
    FILE* load_file;

    //�o�C�i���ǂݍ��݃��[�h�Ńt�@�C�����I�[�v��
    load_file = fopen( FileName, "r+b");

    //�����t�@�C�����Ȃ���΃G���[��Ԃ�
    if( load_file == NULL )return true;

    //�X�R�A�f�[�^�����[�h����
    fread( LoadData, sizeof( EX03_19_STRUCT ) ,RANK_COUNT, load_file );

    //�t�@�C�������
    fclose( load_file );

    //����I��
    return false;
}

void init03_19(TCB* thisTCB)
{
    //���������ɃX�R�A�����[�h�����݂āA�t�@�C�����Ȃ���΃f�t�H���g�̃f�[�^��ǂݍ���
    if( EX03_19_score_load( DATA_FILE_NAME, gEX03_19_SAVE_DATA ) )
    {//�f�t�H���g�̃X�R�A�����[�h����
        EX03_19_score_load( "EX03_19_DEFAULT_DATA.DAT", gEX03_19_SAVE_DATA );
    }
}

void exec03_19(TCB* thisTCB)
{
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�y�L�[�ŃX�R�A�����[�h
    if( g_DownInputBuff & KEY_Z ) EX03_19_score_load( DATA_FILE_NAME, gEX03_19_SAVE_DATA );
    //�w�L�[�ŃX�R�A���Z�[�u
    if( g_DownInputBuff & KEY_X ) EX03_19_score_save( DATA_FILE_NAME, gEX03_19_SAVE_DATA );


    //�����L�[�ŃX�R�A�̑������s�Ȃ�
    if( g_DownInputBuff & KEY_UP)
    {
        for( loop = 0; loop < RANK_COUNT; loop++ )
        {
            gEX03_19_SAVE_DATA[ loop ].Score += 100;
        }
    }
    if( g_DownInputBuff & KEY_DOWN)
    {
        for( loop = 0; loop < RANK_COUNT; loop++ )
        {
            gEX03_19_SAVE_DATA[ loop ].Score -= 100;
        }
    }

    //�X�R�A�f�[�^�̕\��
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        sprintf( str,"RANK%d: %8d  %s",loop+1, gEX03_19_SAVE_DATA[ loop ].Score,gEX03_19_SAVE_DATA[ loop ].Name);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 32;
    }

        font_pos.top += 32;
        g_pFont->DrawText( NULL,"�y�L�[�Ń��[�h�A�w�L�[�ŃZ�[�u\n�����L�[�ŃX�R�A�ύX", -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�R�́|�Q�O  �������G�ȃZ�[�u�E���[�h
////////////////////////////////////////////////////

#define RANK_COUNT 5
#define DATA_FILE_NAME "EX03_20_SAVE_DATA.DAT"

typedef struct{
    EX03_19_STRUCT ScoreData[ RANK_COUNT ];
    char           MyShipCount;
    char           Difficulty;
} EX03_20_STRUCT;

static EX03_20_STRUCT  gEX03_20_SAVE_DATA;


void EX03_20_save( char* FILE_NAME, EX03_20_STRUCT* SaveData )
{
    FILE* save_file;

    //�o�C�i���������݃��[�h�Ńt�@�C�����I�[�v��
    save_file = fopen( FILE_NAME, "w+b");

    //�f�[�^���Z�[�u����
    fwrite( SaveData, sizeof( EX03_20_STRUCT ) , 1, save_file );

    //�t�@�C�������
    fclose( save_file );
}

void EX03_20_load( char* FILE_NAME, EX03_20_STRUCT* LoadData )
{
    FILE* load_file;

    //�o�C�i���ǂݍ��݃��[�h�Ńt�@�C�����I�[�v��
    load_file = fopen( FILE_NAME, "r+b");

    //�f�[�^�����[�h����
    fread( LoadData, sizeof( EX03_20_STRUCT ) , 1, load_file );

    //�t�@�C�������
    fclose( load_file );
}

int EX03_20_load_update( void )
{
    int loop;
    //���[�h��Ƀf�[�^���v���O�����ɔ��f�����鏈���̗�
//  SetMyShipCount( gEX03_20_SAVE_DATA.MyShip );
//  SetDifficulty ( gEX03_20_SAVE_DATA.Difficulty );

    //�X�R�A�̐���𒲂ׂ�i�ȒP�ȃ`�F�b�N�j
    for ( loop = 0; loop < RANK_COUNT - 1; loop++ )
    {
         if( gEX03_20_SAVE_DATA.ScoreData[ loop ].Score <
             gEX03_20_SAVE_DATA.ScoreData[ loop+1 ].Score )
         {
             return true;
         }
    }
    return false;
}
void EX03_20_save_update( void )
{
    //�Z�[�u�O�Ƀf�[�^���X�V���Ă�������
}

void init03_20(TCB* thisTCB)
{
    //�f�t�H���g�̃f�[�^�����[�h����
    EX03_20_load( "EX03_20_DEFAULT_DATA.DAT", &gEX03_20_SAVE_DATA );

}

void exec03_20(TCB* thisTCB)
{
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�y�L�[�Ń��[�h
    if( g_DownInputBuff & KEY_Z )
    {//���[�h
        EX03_20_load( DATA_FILE_NAME, &gEX03_20_SAVE_DATA );
     //���[�h��ɁA�������s�Ȃ��A�G���[�Ȃ�f�t�H���g�f�[�^��ǂݍ���
        if( EX03_20_load_update() )
          EX03_20_load( "EX03_20_DEFAULT_DATA.DAT", &gEX03_20_SAVE_DATA );
    }

    //�w�L�[�ŃZ�[�u
    if( g_DownInputBuff & KEY_X )
    {//�Z�[�u�O�ɁA�f�[�^�̍X�V����
        EX03_20_save_update();
     //�Z�[�u
        EX03_20_save( DATA_FILE_NAME, &gEX03_20_SAVE_DATA );
    }


    //�X�R�A�f�[�^�̕\��
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        sprintf( str,"RANK%d: %8d  %s",loop+1, gEX03_20_SAVE_DATA.ScoreData[ loop ].Score,gEX03_20_SAVE_DATA.ScoreData[ loop ].Name);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 32;
    }

        font_pos.top += 32;
        g_pFont->DrawText( NULL,"�y�L�[�Ń��[�h�A�w�L�[�ŃZ�[�u", -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�R�́|�Q�Q  �f�[�^���Í�������
////////////////////////////////////////////////////
void exec03_22(TCB* thisTCB)
{
#define NUM_DATA_COUNT 4
#define CODE_KEY 0xAA
#define RAND_CODE_KEY 0x10
    //���f�[�^
    unsigned char  in_num_data[]  = {0x00,0x01,0x02,0x03,};
    //�Í����f�[�^�i�[�p�ϐ�
    unsigned char  encode_num_data1[ NUM_DATA_COUNT ];
    unsigned char  encode_num_data2[ NUM_DATA_COUNT ];
    //�������f�[�^�i�[�p�ϐ�
    unsigned char  decode_num_data1[ NUM_DATA_COUNT ];
    unsigned char  decode_num_data2[ NUM_DATA_COUNT ];

    char  str[128];
    unsigned char  code_key;
    int  loop;
    RECT font_pos = {  0,  0,640,480,};


    //�Í��������P
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//�Í����̉��Z���s�Ȃ�
        encode_num_data1[ loop ] = in_num_data[ loop ] + CODE_KEY;
    }
    //�����������P
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//�Í����Ƃ͋t�̉��Z���s�Ȃ�
        decode_num_data1[ loop ] = encode_num_data1[ loop ] - CODE_KEY;
    }

    //�Í��������Q
    srand( RAND_CODE_KEY );
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//�Í����̉��Z���s�Ȃ�
        encode_num_data2[ loop ] = in_num_data[ loop ] ^ rand() % 255;
    }
    //�����������Q
    srand( RAND_CODE_KEY );
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//�Í����Ƃ͋t�̉��Z���s�Ȃ�
        decode_num_data2[ loop ] = encode_num_data2[ loop ] ^ rand() % 255;
    }


    //���f�[�^�\��
    sprintf( str,"�Í��O�f�[�^�@�F0x%02x 0x%02x 0x%02x 0x%02x",
    in_num_data[0], in_num_data[1], in_num_data[2], in_num_data[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 64;

    //�Í��f�[�^�P�\��
    sprintf( str,"�Í����f�[�^�P�F0x%02x 0x%02x 0x%02x 0x%02x",
    encode_num_data1[0], encode_num_data1[1], encode_num_data1[2], encode_num_data1[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 32;

    //�����f�[�^�P�\��
    sprintf( str,"�������f�[�^�P�F0x%02x 0x%02x 0x%02x 0x%02x",
    decode_num_data1[0], decode_num_data1[1], decode_num_data1[2], decode_num_data1[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);font_pos.top += 64;


    //�Í��f�[�^�Q�\��
    sprintf( str,"�Í����f�[�^�Q�F0x%02x 0x%02x 0x%02x 0x%02x",
    encode_num_data2[0], encode_num_data2[1], encode_num_data2[2], encode_num_data2[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 32;

    //�����f�[�^�Q�\��
    sprintf( str,"�������f�[�^�Q�F0x%02x 0x%02x 0x%02x 0x%02x",
    decode_num_data2[0], decode_num_data2[1], decode_num_data2[2], decode_num_data2[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);


}





////////////////////////////////////////////////////
//�R�́|�Q�T  �ȈՃX�N���v�g�����i�V���[�e�B���O�ҁj
////////////////////////////////////////////////////
typedef struct{
    SPRITE            Sprt;
    int               PC;        //�R�[�h�̎��s�ʒu�������J�E���^
    unsigned char*    pCode;     //�R�[�h�f�[�^���i�[���Ă���|�C���^
    unsigned char     Code;      //���ݎ��s���̖���
    unsigned char     Data;      //���ݎ��s���̖��߂̈���
} EX03_SCRIPT_SHOOT;

#define CODE_END        0x00
#define CODE_STOP       0x01
#define CODE_LEFT       0x02
#define CODE_RIGHT      0x03
#define CODE_SHOOT      0x04
#define CODE_RESET      0x05

#define MOVE_SPEED      8


void EX03_script_shot( TCB* thisTCB )
{
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //��ʉ��֌������Ēe�̈ړ�
    work->Y += 16;

    if( work->Y > SCREEN_HEIGHT )
    {//��ʊO�Ɉړ�������I��
        TaskKill( thisTCB );
    }else{
     //�\������
        SpriteDraw( work, 1 );
    }
}


void EX03_script_exec( EX03_SCRIPT_SHOOT* work )
{
    TCB*      shot_tcb;
    SPRITE*   shot_sprt;


    switch ( work->Code )
    {
        case CODE_END:  //�������I�����Ă������̓R�[�h�ƈ����f�[�^�̓ǂݍ��݂��s�Ȃ�
                work->Code = work->pCode[ work->PC++ ];
                work->Data = work->pCode[ work->PC++ ];
                break;

        case CODE_STOP: //�w��̎��Ԓ�~���s�Ȃ�
                //��~��A�f�[�^�̓ǂݍ��ݏ�����
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_LEFT: //��莞�ԍ��ֈړ�����

                work->Sprt.X -= MOVE_SPEED;
                //�ړ��I����A�f�[�^�̓ǂݍ��ݏ�����
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_RIGHT://��莞�ԉE�ֈړ�����

                work->Sprt.X += MOVE_SPEED;
                //�ړ��I����A�f�[�^�̓ǂݍ��ݏ�����
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_SHOOT://�e�𔭎˂���
                //�e�̍쐬
                shot_tcb = TaskMake( EX03_script_shot ,0x2000);
                shot_sprt= (SPRITE*)shot_tcb->Work;
                //�e�̏�����
                shot_sprt->X = work->Sprt.X;
                shot_sprt->Y = work->Sprt.Y;

                //���ˌ�A�f�[�^�̓ǂݍ��ݏ�����
                work->Code = CODE_END;
                break;

        case CODE_RESET://���Z�b�g�������s�Ȃ�
                work->Code = CODE_END;
                work->PC   = 0;
                break;
    }

}


void init03_script(TCB* thisTCB)
{
    EX03_SCRIPT_SHOOT* work = (EX03_SCRIPT_SHOOT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���W�ƁA���s�R�[�h�̏�����
    work->Sprt.X = SCREEN_WIDTH  / 2;
    work->Sprt.Y = SCREEN_HEIGHT / 3;

    work->Code = CODE_END;

}

void exec03_script(TCB* thisTCB)
{
    EX03_SCRIPT_SHOOT* work = (EX03_SCRIPT_SHOOT*)thisTCB->Work;

unsigned char  script_data[] = 
{//�s���C�x���g�f�[�^
    CODE_LEFT  , 0x10,  //���ړ�
    CODE_RIGHT , 0x20,  //�E�ړ�
    CODE_STOP  , 0x10,  //��~
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_LEFT  , 0x10,  //���ړ�
    CODE_RIGHT , 0x18,  //�E�ړ�
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_LEFT  , 0x18,  //���ړ�
    CODE_RESET , 0x00,  //�s�������Z�b�g���A�ŏ��ɖ߂�
};


    //���s����X�N���v�g�f�[�^
    work->pCode  = script_data;

    //�X�N���v�g�̎��s
    EX03_script_exec( work );

    //�\��
    SpriteDraw( &work->Sprt , 0);
}




////////////////////////////////////////////////////
//�R�́|�Q�V ��ԂƏ����̐؂�ւ�
////////////////////////////////////////////////////
#define MODE_A 0
#define MODE_B 1
#define CHANGE_TIME_A 200
#define CHANGE_TIME_B 100

typedef struct{
    EX03_SCRIPT_SHOOT    ScriptWork;
    int                  Time;
    int                  ModeID;
} EX03_27_STRUCT ;


void init03_27(TCB* thisTCB)
{
    EX03_27_STRUCT* work = (EX03_27_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���W�ƁA���s�R�[�h�̏�����
    work->ScriptWork.Sprt.X = SCREEN_WIDTH  / 2;
    work->ScriptWork.Sprt.Y = SCREEN_HEIGHT / 3;

    work->ScriptWork.Code = CODE_END;
}

void exec03_27(TCB* thisTCB)
{
    EX03_27_STRUCT* work = (EX03_27_STRUCT*)thisTCB->Work;

unsigned char  script_data_A[] = 
{//�s���f�[�^A
    CODE_LEFT  , 0x18,  //���ړ�
    CODE_RIGHT , 0x20,  //�E�ړ�
    CODE_STOP  , 0x10,  //��~
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_LEFT  , 0x10,  //���ړ�
    CODE_RIGHT , 0x18,  //�E�ړ�
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_LEFT  , 0x18,  //���ړ�
    CODE_RESET , 0x00,  //�s�������Z�b�g���A�ŏ��ɖ߂�
};
unsigned char  script_data_B[] = 
{//�s���f�[�^B
    CODE_LEFT  , 0x08,  //���ړ�
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_RIGHT , 0x08,  //�E�ړ�
    CODE_SHOOT , 0x00,  //�e�̔���
    CODE_RESET , 0x00,  //�s�������Z�b�g���A�ŏ��ɖ߂�
};

    //�Ǘ��p�̃^�C�}�[�����Z
    work->Time++;

    if( work->ModeID == MODE_A )
    {//�s�����[�hA
        //���s����X�N���v�g�f�[�^
        work->ScriptWork.pCode  = script_data_A;

        if(work->Time >= CHANGE_TIME_A)
        {//��������������珈����؂�ւ���
            work->ModeID = MODE_B;
            work->ScriptWork.Code = CODE_RESET;
            work->ScriptWork.pCode  = script_data_B;
            work->Time = 0;
        }
    }

    if( work->ModeID == MODE_B )
    {//�s�����[�hB
        //���s����X�N���v�g�f�[�^
        work->ScriptWork.pCode  = script_data_B;

        if(work->Time >= CHANGE_TIME_B)
        {//��������������珈����؂�ւ���
            work->ModeID = MODE_A;
            work->ScriptWork.Code = CODE_RESET;
            work->ScriptWork.pCode  = script_data_A;
            work->Time = 0;
        }
    }

    //�X�N���v�g�̎��s
    EX03_script_exec( &work->ScriptWork );

    //�\��
    SpriteDraw( &work->ScriptWork.Sprt , 0);
}

////////////////////////////////////////////////////
//�R�́|�Q�W �L�����I�����
////////////////////////////////////////////////////

#define CHR_MAX 4

typedef struct{
    SPRITE         MarkPoint;
    SPRITE         ChrGraph;
    int            SelectID;
    char           DispMess[64];
} EX03_28_STRUCT;

void init03_28(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_a.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_b.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_c.png",&g_pTex[2] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_d.png",&g_pTex[3] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_mark.png",&g_pTex[4] );
}

void exec03_28(TCB* thisTCB)
{
    EX03_28_STRUCT* work = (EX03_28_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];
    int  chr_pos[ CHR_MAX ][2] = 
{
    {  144,  240,},
    {  240,  256,},
    {  352,  256,},
    {  448,  240,},
};

    //�L�[���͂ɂ��L�����N�^�[�I��
    if( g_DownInputBuff & KEY_RIGHT )
    {
        work->SelectID++;
        //���KID�`�F�b�N
        if( work->SelectID >= CHR_MAX )work->SelectID = 0;
    }
    if( g_DownInputBuff & KEY_LEFT  )
    {
        work->SelectID--;
        //���KID�`�F�b�N
        if( work->SelectID < 0 ) work->SelectID = CHR_MAX-1;
    }

    if( g_DownInputBuff & KEY_Z  )
    {//�L�����N�^�[�̌���A�����ł̓��b�Z�[�W�̕\���̂�
         switch( work->SelectID )
         {
            case 0: strcpy(work->DispMess,"���肵���L�����N�^�[�͂`�ł��B"); break;
            case 1: strcpy(work->DispMess,"���肵���L�����N�^�[�͂a�ł��B"); break;
            case 2: strcpy(work->DispMess,"���肵���L�����N�^�[�͂b�ł��B"); break;
            case 3: strcpy(work->DispMess,"���肵���L�����N�^�[�͂c�ł��B"); break;
         }
    }

    //�L�����N�^�[�I���O���t�B�b�N�̕\��
    for( loop = 0; loop < CHR_MAX; loop++ )
    {
        work->ChrGraph.X = chr_pos[loop][0];
        work->ChrGraph.Y = chr_pos[loop][1];
        SpriteDraw(&work->ChrGraph,loop);
    }

    //�I���}�[�N�̕\��
    work->MarkPoint.X = chr_pos[work->SelectID][0];
    work->MarkPoint.Y = chr_pos[work->SelectID][1];

    SpriteDraw(&work->MarkPoint,4);

    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);

}




////////////////////////////////////////////////////
//�R�́|�Q�X �B���v���C���[�L�������o��������
////////////////////////////////////////////////////

#define CHR_MAX 6
#define HIDE_CHR_E 4
#define HIDE_CHR_F 5

typedef struct{
    SPRITE         MarkPoint;
    SPRITE         ChrGraph;
    int            SelectID;
    int            HideChr_E;
    int            HideChr_F;
    char           DispMess[64];
} EX03_29_STRUCT;

void init03_29(TCB* thisTCB)
{
    EX03_29_STRUCT* work = (EX03_29_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_a.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_b.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_c.png",&g_pTex[2] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_d.png",&g_pTex[3] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_e.png",&g_pTex[4] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_f.png",&g_pTex[5] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\chr_mark.png",&g_pTex[6] );
}

void exec03_29(TCB* thisTCB)
{
    EX03_29_STRUCT* work = (EX03_29_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];
    int  chr_pos[ CHR_MAX ][2] = 
{
    {  144,  240,},
    {  240,  256,},
    {  352,  256,},
    {  448,  240,},
    {  536,  224,},
    {  64 ,  224,},
};

    //�L�[���͂ɂ��L�����N�^�[�I��
    if( g_DownInputBuff & KEY_RIGHT )
    {
        work->SelectID++;
        if( work->SelectID >= HIDE_CHR_E )
        {//�B���L�����N�^�[���\������Ă��邩�`�F�b�N��
         //�\������Ă���Ȃ�AID�`�F�b�N�̍ۂɍl������
            //�B���L������\���Ȃ�A����ID�͕\�����Ȃ�
            if( (work->SelectID == HIDE_CHR_E) && !work->HideChr_E ) work->SelectID++;
            //����ID�����l
            if( (work->SelectID == HIDE_CHR_F) && !work->HideChr_F ) work->SelectID++;
        }

        if( work->SelectID >= CHR_MAX )work->SelectID = 0;
    }
    if( g_DownInputBuff & KEY_LEFT  )
    {
        work->SelectID--;
        if( work->SelectID < 0 ) work->SelectID = CHR_MAX-1;
        if( work->SelectID >= HIDE_CHR_E )
        {//���Z�������l�A�A���t���Ƀ`�F�b�N����
            //�B���L������\���Ȃ�A����ID�͕\�����Ȃ�
            if( (work->SelectID == HIDE_CHR_F) && !work->HideChr_F ) work->SelectID--;
            //����ID�����l
            if( (work->SelectID == HIDE_CHR_E) && !work->HideChr_E ) work->SelectID--;
        }
    }

    if( g_DownInputBuff & KEY_X  )
    {//�B���L�����N�^�[�d�̕\���A��\����؂�ւ���
        work->HideChr_E = !work->HideChr_E;
        work->SelectID = 0;
    }

    if( g_DownInputBuff & KEY_Z  )
    {//�B���L�����N�^�[�e�̕\���A��\����؂�ւ���
        work->HideChr_F = !work->HideChr_F;
        work->SelectID = 0;
    }

    //�L�����N�^�[�I���O���t�B�b�N�̕\��
    for( loop = 0; loop < CHR_MAX; loop++ )
    {
        work->ChrGraph.X = chr_pos[loop][0];
        work->ChrGraph.Y = chr_pos[loop][1];

        if( loop >= HIDE_CHR_E)
        {//�t���O�������Ă����ꍇ�̂݁A�B���L�������\������
            if(((loop == HIDE_CHR_E) && work->HideChr_E) |
               ((loop == HIDE_CHR_F) && work->HideChr_F) )
            {
                SpriteDraw(&work->ChrGraph,loop);
            }
        }else{
            SpriteDraw(&work->ChrGraph,loop);
        }
    }

    //�I���}�[�N�̕\��
    work->MarkPoint.X = chr_pos[work->SelectID][0];
    work->MarkPoint.Y = chr_pos[work->SelectID][1];

    SpriteDraw(&work->MarkPoint,6);

}




////////////////////////////////////////////////////
//�R�́|�R�O RPG�ł̓G�̏o��
////////////////////////////////////////////////////

#define MAP_ROAD      0
#define MAP_GRASS     1
#define MAP_DESERT    2
#define MAP_MOUNTAIN  3

typedef struct{
    char       EncountStep;
    char       RandRate;
    char       EnemyLevel;
} EX03_30_MAP_DATA;

typedef struct{
    int        EncountStep;
    int        MapDataIndex;
    char*      MessData;
    int aaa; 
} EX03_30_STRUCT;



void exec03_30(TCB* thisTCB)
{
    EX03_30_STRUCT* work = (EX03_30_STRUCT*)thisTCB->Work;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];
    int encount_check;
    char* map_name[] = { "��","����","����","�R�x",};

    EX03_30_MAP_DATA map_data[] = 
{
    {  8, 10, 0,},    //��
    {  6,  8, 1,},    //����
    {  4,  5, 2,},    //����
    {  4,  2, 3,},    //�R
};


    //�L�[���͂ɂ��G���J�E���g�`�F�b�N
    if( g_DownInputBuff & KEY_Z )
    {
        //�`�F�b�N���ɕ\�����b�Z�[�W���N���A
        work->MessData = NULL;

        //��萔�ړ�����܂ł́A�G�͏o�����Ȃ�
        if( work->EncountStep == 0 )
        {

            //�n�`�̃f�[�^�ɍ��킹�ė������擾���o���m�����v�Z����
            encount_check = rand() % map_data[work->MapDataIndex].RandRate;

            if( encount_check == 0)
            {//�G�̏o������
                //�n�`�ɍ��킹���G�̏o��
                switch( map_data[work->MapDataIndex].EnemyLevel )
                {
                    case MAP_ROAD:    work->MessData = "���̓G���o�����܂���"; break;
                    case MAP_GRASS:   work->MessData = "�����̓G���o�����܂���"; break;
                    case MAP_DESERT:  work->MessData = "�����̓G���o�����܂���"; break;
                    case MAP_MOUNTAIN:work->MessData = "�R�x�̓G���o�����܂���"; break;
                }
                //���ɏo������܂ł̍Œ�ړ�����ݒ�
                work->EncountStep = map_data[work->MapDataIndex].EncountStep;
            }
        } else {
            //�G���J�E���g�\�ȕ���������炷
            work->EncountStep--;
        }
    }

    //�`�F�b�N����n�`�f�[�^�̐؂�ւ�
    if( g_DownInputBuff & KEY_X  )
    {
        //�}�b�v�f�[�^��4���
        if(++work->MapDataIndex == 4) work->MapDataIndex = 0;
    }

    //�o�������b�Z�[�W�̕\��
    if(work->MessData != NULL) g_pFont->DrawText( NULL, work->MessData, -1, &font_pos, DT_LEFT, 0xffffffff);

    font_pos.top += 16;
    sprintf( str,"���݂̒n�`�f�[�^:  %s",map_name[work->MapDataIndex]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    font_pos.top += 32;
    g_pFont->DrawText( NULL, "�y�L�[�œG�̏o���`�F�b�N�A�w�L�[�Ńf�[�^�؂�ւ�", -1, &font_pos, DT_LEFT, 0xffffffff);

}




////////////////////////////////////////////////////
//�R�́|�R�Q  �ȈՃX�N���v�g�����i�A�h�x���`���[�ERPG�ҁj
////////////////////////////////////////////////////
typedef struct{
    unsigned char     FlagArea[16];
    char              MessBuff[128]; //������i�[�p�̃o�b�t�@
} EX03_SCRIPT_RPG;

typedef struct{
    unsigned char     Code;          //���s���閽��
    unsigned char     Data1;         //���s���閽�߂̂P�Ԗڂ̈���
    unsigned char     Data2;         //���s���閽�߂̂Q�Ԗڂ̈���
    char*             Message;       //�\���p�̕�����
} EX03_SCRIPT_RPG_DATA;

#define CODE_END        0x00
#define CODE_FLAG_SET   0x01
#define CODE_FLAG_CLR   0x02
#define CODE_JUMP_ON    0x03
#define CODE_JUMP_OFF   0x04
#define CODE_PRINT      0x05

#define MOVE_SPEED      8

void EX03_script_rpg_exec( EX03_SCRIPT_RPG* work, EX03_SCRIPT_RPG_DATA* pCode )
{
    int               flag_pos;      //�t���O�̈ʒu
    unsigned char     data1;         //���ݎ��s���̖��߂̈���
    unsigned char     data2;         //���ݎ��s���̖��߂̈���
    unsigned char*    pStr;          //�\���p������ւ̃|�C���^

    while( true ){
        //�f�[�^��ǂݏo��
        data1 = pCode->Data1;
        data2 = pCode->Data2;
        pStr  = pCode->Message;

        switch ( pCode->Code )
        {//�X�N���v�g���߂����s
            case CODE_END:  //�X�N���v�g�����̏I��
                    return;
                    break;

            case CODE_FLAG_SET: //�t���O�𗧂Ă�
                    //�o�C�g���̃t���O�ʒu���v�Z
                    flag_pos = data1 & 0x03;
                    //�t���O�Z�b�g
                    work->FlagArea[ data1 / 8 ]  |= 1 << flag_pos;
                    break;

            case CODE_FLAG_CLR: //�t���O������
                    //�o�C�g���̃t���O�ʒu���v�Z
                    flag_pos = data1 & 0x03;
                    //�t���O�N���A
                    work->FlagArea[ data1 / 8 ]  &= ~(1 << flag_pos);
                    break;

            case CODE_JUMP_ON: //�t���O���`�F�b�N���A�����Ă����珈���𕪊򂳂���
                    //�o�C�g���̃t���O�ʒu���v�Z
                    flag_pos = data1 & 0x03;
                    //�t���O�̃`�F�b�N�ƕ���
                    if( work->FlagArea[ data1 / 8 ]  & (1 << flag_pos) ) pCode += data2-1;
                    break;

            case CODE_JUMP_OFF: //�t���O���`�F�b�N���A�����Ă��Ȃ���Ώ����𕪊򂳂���
                    //�o�C�g���̃t���O�ʒu���v�Z
                    flag_pos = data1 & 0x03;
                    //�t���O�̃`�F�b�N�ƕ���
                    if( !(work->FlagArea[ data1 / 8 ]  & (1 << flag_pos)) ) pCode += data2-1;
                    break;

            case CODE_PRINT://���b�Z�[�W��\������i�O���ւ̏��o�́j
                    //���ۂ̓o�b�t�@�ɃR�s�[���邾���ŁA�\���͌Ăяo�����ōs�Ȃ�
                    strcpy( work->MessBuff, pStr );
                    break;
        }

        //��������X�N���v�g�f�[�^���P�i�߂�
        pCode++;
    }

}


void init03_script_rpg(TCB* thisTCB)
{
    EX03_SCRIPT_RPG* work = (EX03_SCRIPT_RPG*)thisTCB->Work;

EX03_SCRIPT_RPG_DATA script_data_init[] = 
{//�������f�[�^
    {CODE_PRINT   ,0,0,"����͏��������ɕ\������郁�b�Z�[�W�ł��B\n�܂��͂y�L�[�������Ă�������",},
    {CODE_END     ,},
};

    //�������p�̃X�N���v�g�ŏ�����
    EX03_script_rpg_exec( work ,script_data_init );

}

void exec03_script_rpg(TCB* thisTCB)
{
    EX03_SCRIPT_RPG* work = (EX03_SCRIPT_RPG*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
//�t���O��ތ���
#define FLAG_Z    0
#define FLAG_UP   1
#define FLAG_X    2

EX03_SCRIPT_RPG_DATA script_data_Z[] = 
{//�y�L�[�C�x���g�f�[�^
    {CODE_JUMP_ON ,FLAG_Z, 4},
    {CODE_PRINT   ,0,0,"�t���O�𗧂Ă܂����B���ɕ����L�[�̏�������Ă��������B",},
    {CODE_FLAG_SET,FLAG_Z,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"���łɃt���O�͗����Ă��܂��B",},
    {CODE_END     ,},
};

EX03_SCRIPT_RPG_DATA script_data_UP[] = 
{//�����L�[��C�x���g�f�[�^
    {CODE_JUMP_ON,FLAG_Z, 3},
    {CODE_PRINT   ,0,0,"�y�L�[���܂�������Ă��܂���B\n��ɂy�L�[�������Ă��������B",},
    {CODE_END     ,},

    {CODE_JUMP_ON ,FLAG_UP, 4},
    {CODE_PRINT   ,0,0,"�t���O�𗧂Ă܂����B\n�Ō�ɂw�L�[�������Ă��������B",},
    {CODE_FLAG_SET,FLAG_UP,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"���łɃt���O�͗����܂����̂ŁA���̃L�[�������K�v�͂���܂���B",},
    {CODE_END     ,},
};

EX03_SCRIPT_RPG_DATA script_data_X[] = 
{//�w�L�[�C�x���g�f�[�^
    {CODE_JUMP_ON,FLAG_UP, 3},
    {CODE_PRINT   ,0,0,"���̃L�[��������������������Ă��܂���B\n��L�[�������Ă݂Ă��������B",},
    {CODE_END     ,},

    {CODE_JUMP_ON ,FLAG_X, 4},
    {CODE_PRINT   ,0,0,"�Ō�̃t���O�𗧂Ă܂����B\n�ȏ�ŏ����͏I���ł��B",},
    {CODE_FLAG_SET,FLAG_X,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"���łɏ����͏I�����܂����B",},
    {CODE_END     ,},
};



    //�e�L�[���͂��C�x���g����ɂ��āA�X�N���v�g�����s����
    //�y�L�[�����������̏���
    if( g_DownInputBuff & KEY_Z ) EX03_script_rpg_exec( work ,script_data_Z );
    //�w�L�[�����������̏���
    if( g_DownInputBuff & KEY_X ) EX03_script_rpg_exec( work ,script_data_X );
    //�����L�[������������̏���
    if( g_DownInputBuff & KEY_UP) EX03_script_rpg_exec( work ,script_data_UP );

    //���b�Z�[�W�����̕\��
    g_pFont->DrawText( NULL, work->MessBuff, -1, &font_pos, DT_LEFT, 0xffffffff);

}





////////////////////////////////////////////////////
//�S�́@�C���^�[�t�F�[�X
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�S�́|�P Windows�ŃL�[�{�[�h���g��
////////////////////////////////////////////////////

void exec04_01(TCB* thisTCB)
{
    unsigned char KeyboardBuff[256];
    unsigned char InputBuff;

    GetKeyboardState(KeyboardBuff);    //�L�[�{�[�h���̎擾
    InputBuff = 0;
    if( KeyboardBuff[VK_UP]   & 0x80 ) InputBuff |= 1 << 0;
    if( KeyboardBuff[VK_DOWN] & 0x80 ) InputBuff |= 1 << 1;
    if( KeyboardBuff[VK_LEFT] & 0x80 ) InputBuff |= 1 << 2;
    if( KeyboardBuff[VK_RIGHT]& 0x80 ) InputBuff |= 1 << 3;
    if( KeyboardBuff[ 'Z' ]   & 0x80 ) InputBuff |= 1 << 4;
    if( KeyboardBuff[ 'X' ]   & 0x80 ) InputBuff |= 1 << 5;
}


////////////////////////////////////////////////////
//�S�́|�T �^������������ɂ�
////////////////////////////////////////////////////
typedef struct{
        int    accmulate_power_on;        //�{�^���n�m�̒~�ϗ�
        int    accmulate_power_off;       //�{�^���n�e�e�̒~�ϗ�
} EX04_05_STRUCT ;

void exec04_05_Success(TCB* thisTCB)
{
    FontPrint(128,224,"'X' ACCMULATE SUCCESS!");
    //��莞�Ԍ�ɏ���
    thisTCB->Work[0]++;
    if( thisTCB->Work[0] == 50 )
    {
        TaskKill( thisTCB );
    }
}

void exec04_06_Success(TCB* thisTCB)
{
    FontPrint(128,256,"'Z' ACCMULATE SUCCESS!");

    //��莞�Ԍ�ɏ���
    thisTCB->Work[0]++;
    if( thisTCB->Work[0] == 50 )
    {
        TaskKill( thisTCB );
    }
}

void exec04_05(TCB* thisTCB)
{
#define MAX_POWER   100
    EX04_05_STRUCT* work = (EX04_05_STRUCT*)thisTCB->Work;

    //�w�L�[�������Ă���Ԃ͗���
    if(g_InputBuff & KEY_X )
    {
        work->accmulate_power_on++;
        if( work->accmulate_power_on > MAX_POWER )
        {
            work->accmulate_power_on = MAX_POWER;
        }
    }

    //�������u�Ԃ̂ݏ�������
    if(g_UpInputBuff & KEY_X )
    {
        if( work->accmulate_power_on >= MAX_POWER)
        {
            TaskMake(exec04_05_Success,0x2000);
        }
        work->accmulate_power_on = 0;
    }

    //�y�L�[�𗣂��Ă���Ԃ͗���
    if(!(g_InputBuff & KEY_Z))
    {
         work->accmulate_power_off++;
         if( work->accmulate_power_off > MAX_POWER )
         {
             work->accmulate_power_off = MAX_POWER;
         }
    }

    //�������u�Ԃ̂ݏ�������
    if(g_DownInputBuff & KEY_Z )
    {
        if( work->accmulate_power_off >= MAX_POWER)
        {
            TaskMake(exec04_06_Success,0x2000);
        }
        work->accmulate_power_off = 0;

    }
    FontPrint( 128,32, "PUSH Z or X ACCMULATE");
}



////////////////////////////////////////////////////
//�S�́|�V �{�^���̓��������̔���
////////////////////////////////////////////////////

void exec04_07_Success(TCB* thisTCB)
{
    FontPrint(128,224,"BUTTONS SAME PRESS SUCESS!!");
    //��莞�Ԍ�ɏ���
    thisTCB->Work[0]++;
    if( thisTCB->Work[0] == 50 )
    {
        TaskKill( thisTCB );
    }
}

void exec04_07(TCB* thisTCB)
{
#define INPUT_HISTORY_COUNT 16
typedef struct{
    SPRITE          sprt;
    float           AddX;
    float           AddY;
    unsigned char   HistoryInputBuff[INPUT_HISTORY_COUNT];
} EX04_07_STRUCT;

    EX04_07_STRUCT* work = (EX04_07_STRUCT*)thisTCB->Work;

    int same_button_check = 0;
    int loop;

    //�L�[��Ԃ̗������X�V
    for( loop = INPUT_HISTORY_COUNT-1; loop>0; loop--)
    {
        work->HistoryInputBuff[loop] = work->HistoryInputBuff[loop-1];
    }
    //�ŐV�̃L�[������Ԃ�ۑ�
    work->HistoryInputBuff[0] = g_InputBuff;        //�P�t���[���O�̃L�[��Ԃ�ۑ�

    for(loop=0; loop<4; loop++) //�ߋ��S�t���[�����̗������`�F�b�N
    {
        same_button_check |= work->HistoryInputBuff[loop];
    }

    //������������
    if( same_button_check & ((1 << KEY_X)|(1 << KEY_Z)) )
    {
        TaskMake(exec04_07_Success,0x2000);
    }
}



////////////////////////////////////////////////////
//�S�́|�X �������Q�񉟂��ɂ����͔���
////////////////////////////////////////////////////
void exec04_09_Success(TCB* thisTCB)
{
    FontPrint(64,224,"SAME DIRECTION DOUBLE PUSH SUCESS!!");
    //��莞�Ԍ�ɏ���
    thisTCB->Work[0]++;
    if( thisTCB->Work[0] == 50 )
    {
        TaskKill( thisTCB );
    }
}

typedef struct{
    unsigned char   KeyTime[2];
} EX04_09_STRUCT;

void exec04_09(TCB* thisTCB)
{
#define INPUT_COUNT 12
#define KEY_INDEX_LEFT  0
#define KEY_INDEX_RIGHT 1
    EX04_09_STRUCT* work = (EX04_09_STRUCT*)thisTCB->Work;

    if( g_DownInputBuff & KEY_LEFT )
    {
        //��莞�ԓ��ɃL�[��������x���͂���Ă�������͐���
        if( work->KeyTime[ KEY_INDEX_LEFT ] != 0)
        {
            TaskMake(exec04_07_Success,0x2000);
        }else{
            work->KeyTime[ KEY_INDEX_LEFT ] = INPUT_COUNT;
        }
    }

    if( g_DownInputBuff & KEY_RIGHT )
    {
        if( work->KeyTime[ KEY_INDEX_RIGHT ] != 0)
        {
            TaskMake(exec04_09_Success,0x2000);
        }else{
            work->KeyTime[ KEY_INDEX_RIGHT ] = INPUT_COUNT;
        }
    }
    //�A�����͎��Ԃ̃J�E���g
    if( work->KeyTime[ KEY_INDEX_LEFT  ] != 0 ) work->KeyTime[ KEY_INDEX_LEFT  ]--;
    if( work->KeyTime[ KEY_INDEX_RIGHT ] != 0 ) work->KeyTime[ KEY_INDEX_RIGHT ]--;
}


////////////////////////////////////////////////////
//�S�́|�P�P �R�}���h����
////////////////////////////////////////////////////

void exec04_11_Success(TCB* thisTCB)
{
    switch(thisTCB->Work[0]){
         case 0:    FontPrint(144,224,"COMMAND 1 SUCCESSS!!");break;
         case 1:    FontPrint(144,240,"COMMAND 2 SUCCESSS!!");break;
         case 2:    FontPrint(144,256,"COMMAND 3 SUCCESSS!!");break;
    }
    //��莞�Ԍ�ɏ���
    thisTCB->Work[1]++;
    if( thisTCB->Work[1] == 50 )
    {
        TaskKill( thisTCB );
    }
}

void exec04_11(TCB* thisTCB)
{
#define HISTORY_COUNT 100

typedef struct{
    unsigned char   HistoryBuff[HISTORY_COUNT];
} EX04_11_STRUCT;

typedef struct{
    unsigned char   Time;
    unsigned char   Key[7];
} EX04_11_DATA;

EX04_11_DATA command_data[] = 
{
    { {30,},{ KEY_DOWN,  KEY_DOWN|KEY_RIGHT, KEY_RIGHT, 0,}, },   //�R�}���h�P �� �E�� �E
    { {30,},{ KEY_RIGHT, KEY_DOWN,  KEY_DOWN|KEY_RIGHT, 0,}, },   //�R�}���h�Q �E�A���A����
    { {30,},{ KEY_DOWN,  KEY_DOWN|KEY_LEFT,  KEY_LEFT,  0,}, },   //�R�}���h�R ���A�����A��
    { { 0,},{0,},},   //�R�}���h�I��
};

    EX04_11_STRUCT* work = (EX04_11_STRUCT*)thisTCB->Work;
    TCB* newTCB;
    int check_command = 0;
    int loop;
    unsigned char* key_data;

    FontPrint(192,160,"INPUT COMMAND");


    //�L�[��Ԃ̗������X�V
    for( loop = HISTORY_COUNT-1; loop>0; loop--)
    {
        work->HistoryBuff[loop] = work->HistoryBuff[loop-1];
    }

    //�ŐV�̃L�[��Ԃ�ۑ�
    work->HistoryBuff[0] = g_InputBuff & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT); //�����L�[�̂ݕۑ�

    //�y�L�[�������ꂽ���ɁA�R�}���h�̉�͂��J�n
    if( g_DownInputBuff & KEY_Z )
    {
        //�`�F�b�N����R�}���h���I������܂Ń��[�v
        while( command_data[ check_command ].Time )
        {
            //������̃L�[���̓p�^�[�����`�F�b�N
            key_data = command_data[ check_command ].Key;
            for(loop = 0;loop < command_data[ check_command ].Time; loop++)
            {
                if( *key_data == work->HistoryBuff[ command_data[ check_command ].Time - 1 - loop] )
                {   //�S���̃L�[�p�^�[�������͂���Ă�����R�}���h���͐���
                    key_data++;
                    if(*key_data == 0)break;
                }
            }
            //�R�}���h���͐����Ȃ�A�������s���`�F�b�N�I��
            if(*key_data == 0)
            {
                //�R�}���h��\������^�X�N�𐶐�
                newTCB = TaskMake(exec04_11_Success,0x2000);
                newTCB->Work[0] = check_command;
                break;
            }
            check_command++;

        }
    }
}





////////////////////////////////////////////////////
//�S�́|�P�Q �v���C���[���O�o�^
////////////////////////////////////////////////////

#define ALPHABET_NUM    40
//���͉\�Ȗ��O�̕�����
#define NAME_MAX        5
//�\�����镶����(��ɂ��鎖�j
#define DISP_ALPHABET   11
//�����̕\�����W��
#define SELECT_NAME_X   SCREEN_WIDTH / 2
#define SELECT_NAME_Y   SCREEN_HEIGHT/ 2
#define NAME_X          (SCREEN_WIDTH / 2 - 32)
#define NAME_Y          (SCREEN_HEIGHT/ 2 + 64)
#define SELECT_MARK_X   SELECT_NAME_X - 8
#define SELECT_MARK_Y   SELECT_NAME_Y - 8

#define DISPLAY_WIDTH   32

typedef struct{
    SPRITE  Mark;
    int     SelectAlphabetID;
    int     NameCount;
    //���͂������O�A�I�[�����p��+1
    char    Name[NAME_MAX + 1];
} EX04_12_STRUCT;

void EX04_12_name_display(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;

    //���肵�����O�̕\��
    FontPrint(NAME_X,NAME_Y,work->Name);
}

void init04_12(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\mark32x32.png",&g_pTex[0] );

    //�I���}�[�N�̕\�����W�̏�����
    work->Mark.X = SELECT_MARK_X;
    work->Mark.Y = SELECT_MARK_Y;
}

void exec04_12(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;
    int loop;
    int display_alphabet;
char* alphabet_data[ ALPHABET_NUM ] = 
{//�g�p���镶���ƁA���̕��т̃f�[�^
    "A", "B", "C", "D", "E", "F", "G", "H",
    "I", "J", "K", "L", "M", "N", "O", "P",
    "Q", "R", "S", "T", "U", "V", "W", "X",
    "Y", "Z", " ", "0", "1", "2", "3", "4",
    "5", "6", "7", "8", "9", "0", "!", "?",
};

    //�����̑I��
    if( g_DownInputBuff & KEY_RIGHT  )
    {//�I�𕶎���͈͓��Ɏ��߂�
        work->SelectAlphabetID++;
        if(work->SelectAlphabetID >= ALPHABET_NUM)
            work->SelectAlphabetID -= ALPHABET_NUM;
    }
    if( g_DownInputBuff & KEY_LEFT )
    {//�I�𕶎���͈͓��Ɏ��߂�
        work->SelectAlphabetID--;
        if(work->SelectAlphabetID < 0)
            work->SelectAlphabetID += ALPHABET_NUM;
    }

    //�I��p������̕\��
    //���߂ɕ\���J�n����������
    display_alphabet = work->SelectAlphabetID - (DISP_ALPHABET-1) / 2;
    //���l�ɔ͈͓��Ɏ��߂�
    if(display_alphabet < 0)
        display_alphabet += ALPHABET_NUM;

    for( loop = 0; loop < DISP_ALPHABET; loop++ )
    {
        FontPrint(
            //�\���ʒu�̌v�Z
            SELECT_NAME_X - ((DISP_ALPHABET-1) / 2 * DISPLAY_WIDTH) + loop * DISPLAY_WIDTH,
            SELECT_NAME_Y,
            alphabet_data[ display_alphabet ]
        );

        //�\�����镶�����X�V����
        display_alphabet++;
        if(display_alphabet >= ALPHABET_NUM)
            display_alphabet-= ALPHABET_NUM;
    }

    if( g_DownInputBuff & KEY_Z )
    {//�����̌���
        //���肵�������𖼑O������ɃR�s�[
        strcpy( &work->Name[ work->NameCount ],alphabet_data[ work->SelectAlphabetID ] );

        work->NameCount++;
        if(work->NameCount >= NAME_MAX )
        {//�K��̓��͐����z������I��
             //���O���͂̏I������
             //�����ł́A���O������\�����鏈���ɐ؂�ւ��Ă��܂�
             TaskChange( thisTCB,EX04_12_name_display );
        }
    }

    if( g_DownInputBuff & KEY_X )
    {//�����̃L�����Z��
        //���肵���������P�������ǂ�
        work->NameCount--;
        //�������O�ȉ��ɂ͂��Ȃ�
        if(work->NameCount < 0 ) work->NameCount = 0;
        //�����̏���
        work->Name[ work->NameCount ] = NULL;
    }

    //���肵�����O�̕\��
    FontPrint(NAME_X,NAME_Y,work->Name);

    //�I���}�[�N�̕\���i�ʒu�͌Œ�
    SpriteDraw( &work->Mark, 0);
}



////////////////////////////////////////////////////
//�T��  �O���t�B�b�N�̕\��
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�T�́|�P �L������\������
////////////////////////////////////////////////////
void init05_01(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_01(TCB* thisTCB)
{
    D3DXVECTOR3 pos;

    pos.x = 320;    //���W�̐ݒ�
    pos.y = 240;
    pos.z = 0;

    //�X�v���C�g�̕`��
    g_pSp->Draw( g_pTex[0],NULL,NULL,&pos,0xffffffff);
}

////////////////////////////////////////////////////
//�T�́|�Q ��������̃L������\������
////////////////////////////////////////////////////
void init05_02(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_02(TCB* thisTCB)
{
    D3DXVECTOR3 pos;
    int i;
static float posdata[3][2] = {
        { 320, 240, },            //�O�Ԗڂ̃L�����N�^�[�̕\�����W
        { 160, 360, },            //�P�Ԗڂ̃L�����N�^�[�̕\�����W
        { 448, 112, },            //�Q�Ԗڂ̃L�����N�^�[�̕\�����W
     };

    for( i=0; i<3; i++){
        pos.x = posdata[i][0];
        pos.y = posdata[i][1];
        g_pSp->Draw( g_pTex[0],NULL,NULL,&pos,0xffffffff);
    }
}



////////////////////////////////////////////////////
//�T�́|�R ���b�Z�[�W�̕\�����Ǘ�����
////////////////////////////////////////////////////

void exec05_03(TCB* thisTCB)
{
#define DISP_SPEED 5
typedef struct{
    int           StrPoint;
    int           Time;
} EX05_03_STRUCT ;

char sample_mess[] = 
    "����̓��b�Z�[�W�̕\�����Ǘ�����v���O�����́A\n"
    "�e�X�g�p������ł��B\n"
    "���̃v���O��������{�ɐF�X�ύX���Ă݂ĉ������B";

    EX05_03_STRUCT* work = (EX05_03_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    int mess_count;

    switch( sample_mess[work->StrPoint] )
    {
        case  '\n':  //���s���̏��� �y�L�[�Ŏ��s��
            if( g_DownInputBuff & KEY_Z  )
            {
                work->StrPoint += 1;
                work->Time = 0;
            }
            mess_count = work->StrPoint;
            break;
        case  NULL:   //������I��
            mess_count = -1;
            break;

        default: //�ʏ핶���̏���
            //��莞�Ԍo�ߌ�A���̕�����
            work->Time++;
            if( work->Time == DISP_SPEED )
            {
                work->StrPoint+=2;
                work->Time = 0;
            }
            mess_count = work->StrPoint;
            break;
    }

    g_pFont->DrawText( NULL, sample_mess, mess_count, &font_pos, DT_LEFT, 0xffffffff);

    //�w�L�[�ōŏ�����\��
    if( g_DownInputBuff & KEY_X  )
    {
        work->StrPoint = 0;
        work->Time     = 0;
    }

}






////////////////////////////////////////////////////
//�T�́|�S �w�i�̕\��
////////////////////////////////////////////////////

void init05_04(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_04(TCB* thisTCB)
{
    BACK_GROUND* bg;
    bg = (BACK_GROUND*)thisTCB->Work;

    BGDraw(bg,0);      //�w�i�̕`��
}



////////////////////////////////////////////////////
//�T�́|�T �w�i�Ƃ̓����蔻��
////////////////////////////////////////////////////

typedef struct{
    BACK_GROUND    Bg;
    SPRITE        Cursor;
} EX05_05_STRUCT ;

void init05_05(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\HIT_CHECK.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\CURSOR.png",&g_pTex[1] );
}

void exec05_05(TCB* thisTCB)
{
#define HIT_ACC 32          //�����蔻��̐��x

unsigned char check_data[ SCREEN_HEIGHT / HIT_ACC ][ SCREEN_WIDTH / HIT_ACC ] =
{//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //0
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //1
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //2
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, },  //3
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, },  //4
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },  //5
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },  //6
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },  //7
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },  //8
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, },  //9
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //10
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //11
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //12
{  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, },  //13
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },  //14
};

    EX05_05_STRUCT* work = (EX05_05_STRUCT*)thisTCB->Work;
    BACK_GROUND   bg;
    RECT font_pos = { 0, 0, 640, 480, };
    unsigned int check_x;
    unsigned int check_y;
    BOOL err = FALSE;

    bg.X = 0;
    bg.Y = 0;
    //�w�i�̕\�� 
    BGDraw( &bg, 0 );

    //�J�[�\���̈ړ�
    if( g_InputBuff & KEY_UP    ) work->Cursor.Y -= 8;
    if( g_InputBuff & KEY_DOWN  ) work->Cursor.Y += 8;
    if( g_InputBuff & KEY_RIGHT ) work->Cursor.X += 8;
    if( g_InputBuff & KEY_LEFT  ) work->Cursor.X -= 8;

    SpriteDraw( &work->Cursor, 1 );

    //�w�i�ڐG�f�[�^�̃`�F�b�N
    check_x =  work->Cursor.X / HIT_ACC;
    check_y =  work->Cursor.Y / HIT_ACC;

    //�f�[�^�͈͊O�̏ꍇ�G���[
    if( check_x >= SCREEN_WIDTH  / HIT_ACC) err = TRUE;
    if( check_y >= SCREEN_HEIGHT / HIT_ACC) err = TRUE;

    //�ڐG����
    if( !err ) //�G���[�̏ꍇ�̓`�F�b�N���Ȃ�
    {
        if( check_data[ check_y ][ check_x ] )
        {

            FontPrint(5,15,"HIT BACK GROUND");
        }
    }

}


////////////////////////////////////////////////////
//�T�́|�U �w�i�̃X�N���[��
////////////////////////////////////////////////////
void init05_06(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_06(TCB* thisTCB)
{
#define SCROLL_SPEED 2
    BACK_GROUND* bg;
    bg = (BACK_GROUND*)thisTCB->Work;

    //�X�N���[���͋t�����ɐi��
    bg->X -= SCROLL_SPEED;
    bg->X %= SCREEN_WIDTH;
    bg->Y = 0;

    BGDraw(bg,0);    //�w�i�̕`��
}

////////////////////////////////////////////////////
//�T�́|�V ���d�X�N���[��
////////////////////////////////////////////////////
#define STAR_MAX 32
typedef struct{
    BACK_GROUND   Bg0;
    BACK_GROUND   Bg1;
    float         StarPosX[STAR_MAX];
    float         StarPosY[STAR_MAX];
    float         StarSpeed[STAR_MAX];
    RECT*         StarPix;
} EX05_07_STRUCT ;

void init05_07(TCB* thisTCB)
{
    EX05_07_STRUCT* work = (EX05_07_STRUCT*)thisTCB->Work;
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\SPACE_BG.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\pix.png", &g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\GRAND_BG.png",&g_pTex[2] );

    //�X�^�[���W�̏�����
    for( loop = 0; loop < STAR_MAX; loop++)
    {
        work->StarPosX[ loop ]  = rand() / (RAND_MAX / SCREEN_WIDTH);
        work->StarPosY[ loop ]  = rand() / (RAND_MAX / SCREEN_HEIGHT);
        work->StarSpeed[ loop ] = rand() / (RAND_MAX / 2.0) + 1.0;
    }
    //��O�\���w�i�̂x���W
    work->Bg1.Y  = 416;
}

void exec05_07(TCB* thisTCB)
{
    EX05_07_STRUCT* work = (EX05_07_STRUCT*)thisTCB->Work;
    SPRITE sprt;
    RECT star_rect = { 0, 0, 2, 2 };
    int loop;

    //��ԉ��̔w�i
    BGDraw( &work->Bg0, 0 );

    //�X�^�[�`��
    sprt.SrcRect = &star_rect;

    for( loop = 0; loop < STAR_MAX; loop++)
    {
        work->StarPosX[ loop ]  -= work->StarSpeed[ loop ];
        //���[�ɓ��B������E�[�ɖ߂�
        if( work->StarPosX[ loop ]  < 0 )
            work->StarPosX[ loop ]  = SCREEN_WIDTH;

        sprt.X = work->StarPosX[ loop ];
        sprt.Y = work->StarPosY[ loop ];

        SpriteDraw( &sprt, 1 );
    }
    //��Ԏ�O�̔w�i
    work->Bg1.X  -= 4.0;
    //���[�ɓ��B������E�[�ɖ߂�
    if( work->Bg1.X < -(SCREEN_WIDTH / 2) ) work->Bg1.X = 0;

    BGDraw( &work->Bg1, 2 );
}



////////////////////////////////////////////////////
//�T�́|�W �g��A�k��
////////////////////////////////////////////////////

typedef struct{
    float           X;
    float           Y;
    D3DXVECTOR3     Center;
    D3DXVECTOR3     Scale;
    float           Count;
} EX5_8_STRUCT ;

void ex5_8_CustomDraw( EX5_8_STRUCT* work, int bitmap_id )
{
    D3DXVECTOR3 pos;
    D3DXMATRIX  matrix;


    pos.x = work->X / work->Scale.x;
    pos.y = work->Y / work->Scale.y;
    pos.z = 0;

    D3DXMatrixScaling( &matrix, work->Scale.x, work->Scale.y, 1.0 );
    g_pSp->SetTransform( &matrix );
    g_pSp->Draw( g_pTex[0], NULL, &work->Center, &pos, 0xffffffff);
}

void init05_08(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_08(TCB* thisTCB)
{
    EX5_8_STRUCT* work = (EX5_8_STRUCT*)thisTCB->Work;

    work->X = 320 - 32;
    work->Y = 240 - 32;
    work->Center.x = 32;
    work->Center.y = 32;

    work->Count += 0.125;
    work->Scale.x = 1.0 + sin( work->Count ) * 0.5;
    work->Scale.y = 1.0 + sin( work->Count ) * 0.5;


    ex5_8_CustomDraw(work,0);
}



//////////////////////////
//�T�́|�X ��]
//////////////////////////

typedef struct{
    float           X;
    float           Y;
    D3DXVECTOR3     Center;
    float           Rotate;
} EX5_9_STRUCT ;

void ex5_9_CustomDraw( EX5_9_STRUCT* work, int bitmap_id )
{
    D3DXVECTOR3 pos;
    D3DXMATRIX  matrix_a;
    D3DXMATRIX  matrix_b;
    D3DXMATRIX  matrix;

    pos.x = work->X;
    pos.y = work->Y;
    pos.z = 0;

    D3DXMatrixRotationZ( &matrix_a, work->Rotate );             //��]�s����擾

    D3DXMatrixTranslation( &matrix_b, pos.x, pos.y, 0 );        //���s�ړ��s����擾

    D3DXMatrixMultiply( &matrix, &matrix_a, &matrix_b );        //�s�������

    g_pSp->SetTransform( &matrix );


    g_pSp->Draw( g_pTex[bitmap_id], NULL, &work->Center, &pos, 0xffffffff);

    //�ݒ肵����]�s������ɖ߂�
    D3DXMatrixTranslation( &matrix, 0, 0, 0 );
    g_pSp->SetTransform( &matrix );

}

void init05_09(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_09(TCB* thisTCB)
{
#define ROTATE_SPEED 0.125
    EX5_9_STRUCT* work = (EX5_9_STRUCT*)thisTCB->Work;

    work->X = SCREEN_WIDTH / 2;
    work->Y = SCREEN_HEIGHT / 2;
    work->Center.x = work->X + 64;
    work->Center.y = work->Y + 64;

    work->Rotate += ROTATE_SPEED;

    ex5_9_CustomDraw(work,0);
}





////////////////////////////////////////////////////
//�T�́|�P�O ���]����
////////////////////////////////////////////////////

typedef struct{
    float           X;
    float           Y;
    D3DXVECTOR3     Center;
    D3DXVECTOR3     Scale;
    BOOL            ReversX;
    BOOL            ReversY;
} EX5_10_STRUCT ;

void ex5_10_CustomDraw( EX5_10_STRUCT* work, int bitmap_id )
{
    D3DXVECTOR3 pos;
    D3DXMATRIX  matrix;

    work->Scale.x = 1.0;
    work->Scale.y = 1.0;
    work->Center.x = 0;
    work->Center.y = 0;

    if( work->ReversX )  //�w���]����
    {
        work->Scale.x = -1.0;
        work->Center.x = 128;
    }

    if( work->ReversY ) //�x���]����
    {
        work->Scale.y = -1.0;
        work->Center.y = 128;
    }
    //���W�ݒ�
    pos.x = work->X / work->Scale.x;
    pos.y = work->Y / work->Scale.y;
    pos.z = 0;

    //�ϊ��s��̐ݒ�
    D3DXMatrixScaling( &matrix, work->Scale.x, work->Scale.y, 1.0 );
    g_pSp->SetTransform( &matrix );
    //�`��
    g_pSp->Draw( g_pTex[0], NULL, &work->Center, &pos, 0xffffffff);
}

void init05_10(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_10(TCB* thisTCB)
{
    EX5_10_STRUCT* work = (EX5_10_STRUCT*)thisTCB->Work;


    work->X = 64;
    work->Y = 64;
    work->ReversX = 0;
    work->ReversY = 0;
    ex5_10_CustomDraw(work,0);

    work->X = 384;
    work->Y = 64;
    work->ReversX = 1;
    work->ReversY = 0;
    ex5_10_CustomDraw(work,0);


    work->X = 64;
    work->Y = 288;
    work->ReversX = 0;
    work->ReversY = 1;
    ex5_10_CustomDraw(work,0);

    work->X = 384;
    work->Y = 288;
    work->ReversX = 1;
    work->ReversY = 1;
    ex5_10_CustomDraw(work,0);
}





////////////////////////////////////////////////////
//�T�́|�P�Q �E�B���h�E��\������
////////////////////////////////////////////////////

void exec5_12_WinDraw( RECT* pRect, int ID )
{
    D3DXVECTOR3 pos;
    D3DXMATRIX  matrix;
    RECT win_rect[] = 
{
    { 0, 0, 1, 1,},
    { 7, 1, 8, 2,},
    {10,10,11,11,},
};
    float scaleWidth;
    float scaleHeight;


    //�E�B���h�E���ƍ����ݒ�
    scaleWidth  = pRect->right  - pRect->left;
    scaleHeight = pRect->bottom - pRect->top;

    //�\�����W�ݒ�
    pos.x = pRect->left / scaleWidth;
    pos.y = pRect->top  / scaleHeight;
    pos.z = 0;

    D3DXMatrixScaling( &matrix, scaleWidth, scaleHeight, 1.0 );
    g_pSp->SetTransform( &matrix );
    g_pSp->Draw( g_pTex[0], &win_rect[ ID ], NULL, &pos, 0xffffffff);

    //�ݒ肵���s������ɖ߂�
    D3DXMatrixScaling( &matrix, 1.0, 1.0, 1.0 );
    g_pSp->SetTransform( &matrix );

}

void exec05_12_window3(TCB* thisTCB)
{
   RECT win_rect = { 128, 256, 512, 320 };
   char str[128];

    //�E�B���h�E�g�\��
    exec5_12_WinDraw( &win_rect, 1 );

    //�E�B���h�E�����̕����\��
    FontPrint( win_rect.left , win_rect.top, "SAMPLE\nWINDOW2\nCOLOR GRADATION\nTEST!!");
}

void exec05_12_window2(TCB* thisTCB)
{
   RECT win_rect = { 336, 264, 496, 416 };
   char str[128];
    //�E�B���h�E�g�\��
    exec5_12_WinDraw( &win_rect, 0 );

    //�E�B���h�E�����̕����\��
    FontPrint( win_rect.left , win_rect.top, "\n 1.MENU\n 2.STATUS\n 3.ITEM\n 4.HELP\n 5.OPTION\n 6.ETC\n 7.");
}

void init05_12(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\pix.png", &g_pTex[0] );
    //2�ڂ̃E�B���h�E
      TaskMake(exec05_12_window2,0x1000);
    //3�ڂ̃E�B���h�E
      TaskMake(exec05_12_window3,0x1000);
}

void exec05_12(TCB* thisTCB)
{
   RECT win_rect = { 64, 384, 576, 448 };
   char str[128];

    //�E�B���h�E�g�\��
    exec5_12_WinDraw( &win_rect, 2 );

    //�E�B���h�E�����̕����\��
    FontPrint( win_rect.left , win_rect.top, "\n SAMPLE WINDOW MAIN\n MAKE FIRST TASK");
}



////////////////////////////////////////////////////
//�T�́|�P�R �L���}�b�v��\������
////////////////////////////////////////////////////

typedef struct{
    int            ScrollX;
    int            ScrollY;
} EX05_13_STRUCT ;

void init05_13(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP.png",&g_pTex[0] );
}

void exec05_13(TCB* thisTCB)
{
#define CHIP_SIZE 32          //�}�b�v�`�b�v�̑傫��
#define MAP_SIZE_X 40         //�}�b�v�̃T�C�YX
#define MAP_SIZE_Y 15         //�}�b�v�̃T�C�YY

unsigned char map_data[ MAP_SIZE_Y ][ MAP_SIZE_X ] =
{//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //0
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //1
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //2
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //3
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //4
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //5
{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 0, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //6
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //7
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //8
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //9
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //10
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //11
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //12
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //13
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //14
};                                 

    EX05_13_STRUCT* work = (EX05_13_STRUCT*)thisTCB->Work;
    int loopX;
    int loopY;
    int map_dataX;        //�擾����}�b�v�f�[�^�̂w���W
    int map_dataY;        //�擾����}�b�v�f�[�^�̂x���W

    SPRITE  chip;
    RECT chip_ptn[4] = {
    {  0,   0,  32, 32},
    { 32,   0,  64, 32},
    { 64,   0,  96, 32},
    { 96,   0, 128, 32},
    };

    //�X�N���[�����W�̌v�Z����
    if( g_InputBuff & KEY_UP    ) work->ScrollY -= 8;
    if( g_InputBuff & KEY_DOWN  ) work->ScrollY += 8;
    if( g_InputBuff & KEY_RIGHT ) work->ScrollX += 8;
    if( g_InputBuff & KEY_LEFT  ) work->ScrollX -= 8;

    for( loopY = 0; loopY < SCREEN_HEIGHT / CHIP_SIZE + 2; loopY++ )
    {
        for( loopX = 0; loopX < SCREEN_WIDTH / CHIP_SIZE + 2; loopX++ )
        {
            //�`�b�v�f�[�^�̂w���W���v�Z
            map_dataX =  work->ScrollX / CHIP_SIZE + loopX;
            if( map_dataX < 0) map_dataX += MAP_SIZE_X * ( -map_dataX / MAP_SIZE_X + 1 );
            map_dataX %= MAP_SIZE_X;

            //�`�b�v�f�[�^�̂x���W���v�Z
            map_dataY =  work->ScrollY / CHIP_SIZE + loopY;
            map_dataY %= MAP_SIZE_Y;
            if( map_dataY < 0) map_dataY += MAP_SIZE_Y * ( -map_dataY / MAP_SIZE_Y + 1 );

            //�`�b�v�̕\�����W���v�Z
            chip.X = loopX * CHIP_SIZE - work->ScrollX % CHIP_SIZE - CHIP_SIZE;
            chip.Y = loopY * CHIP_SIZE - work->ScrollY % CHIP_SIZE - CHIP_SIZE;

            //�`�悷��`�b�v�̃f�[�^���擾
            chip.SrcRect = &chip_ptn[  map_data[ map_dataY ][ map_dataX ]  ];

            SpriteDraw( &chip, 0);
        }
    }

}



////////////////////////////////////////////////////
//�T�́|�P�S �w�i�̃A�j���[�V����
////////////////////////////////////////////////////

typedef struct{
    int            ScrollX;
    int            ScrollY;
    float          AnimCount;
} EX05_14_STRUCT ;

void init05_14(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_B.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_C.png",&g_pTex[2] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_D.png",&g_pTex[3] );
}

void exec05_14(TCB* thisTCB)
{
#define ANIM_SPEED 1.0/8.0    //�A�j���[�V�����X�s�[�h
#define ANIM_COUNT 4          //�A�j���[�V������
#define CHIP_SIZE 32          //�}�b�v�`�b�v�̑傫��
#define MAP_SIZE_X 20         //�}�b�v�̃T�C�YX
#define MAP_SIZE_Y 15         //�}�b�v�̃T�C�YY

unsigned char map_data[ MAP_SIZE_Y ][ MAP_SIZE_X ] =
{//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //0
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //1
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //2
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //3
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //4
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //5
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 0, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //6
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //7
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, },  //8
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //9
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //10
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //11
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //12
{  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, },  //13
{  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },  //14
};                                 

    EX05_14_STRUCT* work = (EX05_14_STRUCT*)thisTCB->Work;
    int loopX;
    int loopY;
    int map_chip_pattern;
    SPRITE  chip;

    RECT chip_ptn[4] = {
    {  0,   0,  32, 32},
    { 32,   0,  64, 32},
    { 64,   0,  96, 32},
    { 96,   0, 128, 32},
    };

    //�A�j���[�V�����Ő؂�ւ���}�b�v�`�b�v�̃p�^�[�����擾����
    work->AnimCount += ANIM_SPEED;
    //�A�j���[�V�����J�E���^��␳
    if( work->AnimCount >= ANIM_COUNT )work->AnimCount -= ANIM_COUNT;
    map_chip_pattern = work->AnimCount;

    for( loopY = 0; loopY < SCREEN_HEIGHT / CHIP_SIZE ; loopY++ )
    {
        for( loopX = 0; loopX < SCREEN_WIDTH / CHIP_SIZE ; loopX++ )
        {
            //�`�b�v�̕\�����W���v�Z
            chip.X = loopX * CHIP_SIZE;
            chip.Y = loopY * CHIP_SIZE;

            //�`�悷��`�b�v�̃f�[�^���擾
            chip.SrcRect = &chip_ptn[  map_data[ loopY ][ loopX ]  ];

            SpriteDraw( &chip, map_chip_pattern);
        }
    }

}






////////////////////////////////////////////////////
//�T�́|�P�T���X�^�X�N���[��
////////////////////////////////////////////////////

void ex5_RasterDraw( short*  raster_pos, int bitmap_id)
{
    int   loop;
    RECT  slice_work = { 0, 0, 1024, 0,};  //�����p�̃��[�N
    D3DXVECTOR3 pos;

    for(loop = 0;loop < SCREEN_HEIGHT; loop++ )
    {   //�X�N���[�����W�ݒ�
        pos.x = raster_pos[ loop ];
        pos.y = loop;
        pos.z = 0;
        //����
        slice_work.top   = loop;
        slice_work.bottom= loop + 1;
        //�`��
        g_pSp->Draw( g_pTex[bitmap_id], &slice_work, NULL, &pos, 0xffffffff);
    }

}


void init05_15(TCB* thisTCB)
{

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\BG.png",&g_pTex[0] );
}

void exec05_15(TCB* thisTCB)
{
#define RASTER_SIZE  128
    int   loop;
    float raseter_cycle;
    short raster_pos[SCREEN_HEIGHT];

    raseter_cycle = g_Count * 0.0125;
    for( loop = 0; loop < SCREEN_HEIGHT; loop++)
    {
        raseter_cycle += 0.0125;
        raster_pos[ loop ] = sin( raseter_cycle ) * RASTER_SIZE;
    }

    ex5_RasterDraw( raster_pos, 0);
}






////////////////////////////////////////////////////
//�T�́|�P�U ���s���̂���n�ʂ�\��
////////////////////////////////////////////////////

void init05_16(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\PERS_BG.png",&g_pTex[0] );
}

void exec05_16(TCB* thisTCB)
{
#define RASTER_START  256        //���X�^�[�X�N���[�����J�n����ʒu
#define RASTER_END    512        //���X�^�[�X�N���[�����I������ʒu
#define BACK_POS      180
#define SCROLL_SPEED  4
typedef struct{
    int            ScrollPos;
    short          RasterPos[SCREEN_HEIGHT];
} EX5_18_STRUCT ;

    EX5_18_STRUCT* work = (EX5_18_STRUCT*)thisTCB->Work;
    int   loop;

    //���E�w�X�N���[��
    if(!(g_InputBuff & KEY_LEFT))
    {
        work->ScrollPos += SCROLL_SPEED;
        if( work->ScrollPos >  BACK_POS ) work->ScrollPos = BACK_POS;
    }

    if(!(g_InputBuff & KEY_RIGHT))
    {
        work->ScrollPos -= SCROLL_SPEED;
        if( work->ScrollPos < -BACK_POS ) work->ScrollPos = -BACK_POS;
    }

    //���s�������𕪊�
    for( loop = 0; loop < SCREEN_HEIGHT; loop++)
    {   //�n�ʈȊO�̕����͌Œ�ʒu�ŁA�X�N���[�����Ȃ�
        work->RasterPos[ loop ] = -BACK_POS;

        //�n�ʕ������X�N���[�����W�ɂ��킹�Ęc�܂���
        if( loop >= RASTER_START )
        {
            work->RasterPos[ loop ] += work->ScrollPos * (loop / (float)(RASTER_END - RASTER_START) );
        }
    }

    ex5_RasterDraw( work->RasterPos, 0);
}






////////////////////////////////////////////////////
//�T�́|�P�V �t�F�[�h�C���E�A�E�g���s��
////////////////////////////////////////////////////
typedef struct{
    BACK_GROUND    BaseBg;
    BACK_GROUND    FadeBg;
    int            FadeMode;         //0.�t�F�[�h�����A1.�t�F�[�h�A�E�g -1.�t�F�[�h�C��
    int            FadeCount;
} EX05_17_STRUCT ;

void exec05_17_FadeDraw(BACK_GROUND* bg,int bitmap_id,unsigned int fade_level)
{
    D3DXVECTOR3 pos;
    DWORD       color;

    pos.x = bg->X;
    pos.y = bg->Y;
    pos.z = 0;
    //�t�F�[�h���x���ɂ���ă��l�ƐF��ݒ�
    color = D3DCOLOR_ARGB( fade_level ,0xff,0xff,0xff );

    g_pSp->Draw( g_pTex[bitmap_id],NULL,NULL,&pos, color);
}


void init05_17(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\bg01.png", &g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\change_bga.png", &g_pTex[1] );
}

void exec05_17(TCB* thisTCB)
{
#define FADE_IN   -1
#define FADE_OUT  1 
#define FADE_NONE 0
#define FADE_SPEED 4

    EX05_17_STRUCT* work = (EX05_17_STRUCT*)thisTCB->Work;

    //�t�F�[�h�����ǂ����ŏ����𕪂���
    if( work->FadeMode == FADE_NONE )
    {//�L�[���͂Ńt�F�[�h�̊J�n
        if( g_DownInputBuff & KEY_Z )
        {//�t�F�[�h�C�������J�n
            work->FadeMode  = FADE_IN;
            work->FadeCount = 0xff;
        }
        if( g_DownInputBuff & KEY_X )
        {//�t�F�[�h�A�E�g�����J�n
            work->FadeMode  = FADE_OUT;
            work->FadeCount = 0;
        }
    }else{
    //�t�F�[�h�C���E�A�E�g��
        if( work->FadeMode == FADE_IN )
        {//�t�F�[�h�C������
            work->FadeCount -= FADE_SPEED;
            if( work->FadeCount <= 0 )
            {
                work->FadeMode  = FADE_NONE;
                work->FadeCount = 0;
            }
        }
        if( work->FadeMode == FADE_OUT )
        {//�t�F�[�h�A�E�g����
            work->FadeCount += FADE_SPEED;
            if( work->FadeCount >= 0xff )
            {
                work->FadeMode  = FADE_NONE;
                work->FadeCount = 0xff;
            }
        }
   }

    //�Ώۂ̔w�i��\��
    BGDraw( &work->BaseBg, 0);

    //�t�F�[�h���̓t�F�[�h�p�̉摜��\��
    if( work->FadeMode != FADE_NONE ) exec05_17_FadeDraw( &work->FadeBg, 1,work->FadeCount );
}





////////////////////////////////////////////////////
//�T�́|�P�W �L�����N�^�[�Ɏc����t����
////////////////////////////////////////////////////


#define IMAGE_COUNT 8
#define MOVE_SPEED  8.0

typedef struct{
    SPRITE         Sprt;
    SPRITE         Image;
    D3DXVECTOR2    PosBuff[ IMAGE_COUNT ];  //�c���̍��W
    int            ImageCount;
} EX05_18_STRUCT;

void EX05_18_CustomDraw(SPRITE* pspr,int bitmap_id,int color_depth)
{
    D3DXVECTOR3 pos;

    pos.x = pspr->X;
    pos.y = pspr->Y;
    pos.z = 0;

    color_depth = D3DCOLOR_ARGB( 0xff,color_depth,color_depth,color_depth );
    g_pSp->Draw( g_pTex[bitmap_id],NULL,NULL,&pos, color_depth);
}

void init05_18(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
}

void exec05_18(TCB* thisTCB)
{
    EX05_18_STRUCT* work = (EX05_18_STRUCT*)thisTCB->Work;
    int loop;
    float add_color_depth;
    float color_depth = 0;

    //���W�����i�c���̕\�����W�j�����炷
    for( loop = IMAGE_COUNT-1; loop > 0; loop-- )
    {//��ԌÂ����W��������Ă���
        work->PosBuff[ loop ].x = work->PosBuff[ loop-1 ].x;
        work->PosBuff[ loop ].y = work->PosBuff[ loop-1 ].y;
    }

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;

    //���@�̈ړ����W�𗚗��ɋL�^����
    work->PosBuff[ 0 ].x = work->Sprt.X;
    work->PosBuff[ 0 ].y = work->Sprt.Y;

    //�c���̕\������ύX
    if( g_DownInputBuff & KEY_Z )
    {//�ő�\�������z���Ȃ��悤�ɕ␳
       if( ++work->ImageCount >= IMAGE_COUNT ) work->ImageCount = 0;
    }

    //�c���̐F�̔Z�����v�Z������Z�l���擾
    if(work->ImageCount != 0)
    {
       add_color_depth = 0xff / work->ImageCount;
    }

    //�w�肵���������c����\������
    for( loop = work->ImageCount; loop > 0; loop-- )
    {
        //���W��������\�����W���擾
        work->Image.X = work->PosBuff[ loop ].x;
        work->Image.Y = work->PosBuff[ loop ].y;

        //�c���̐F�̔Z�����v�Z
        color_depth += add_color_depth;

        //�F�̔Z���𔽉f�����p�̕`��֐��ŕ`��
        EX05_18_CustomDraw( &work->Image,0,(int)color_depth );
    }

    SpriteDraw( &work->Sprt, 0);

}



////////////////////////////////////////////////////
//�T�́|�P�X �L�����N�^�[�ɉe��t����
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Human;
    SPRITE         Shadow;
    float          Acc;
    int            JumpFlag;
} EX05_19_STRUCT;
#define MOVE_SPEED 4.0             //�ړ����x
#define JUMP_POWER 10.0            //�W�����v��
#define GRAVITY    0.35            //�d�͒l
#define GRAND_LINE SCREEN_HEIGHT/2 //�n�ʈʒu
#define CHR_HEIGHT 104.0           //�L�����̍���

void init05_19(TCB* thisTCB)
{
    EX05_19_STRUCT* work = (EX05_19_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\shadow.png",&g_pTex[1] );

    //���W�̏�����
    work->Human.X = SCREEN_WIDTH / 2;
    work->Human.Y = GRAND_LINE - CHR_HEIGHT;
}

void exec05_19(TCB* thisTCB)
{
    EX05_19_STRUCT* work = (EX05_19_STRUCT*)thisTCB->Work;

    //�L�����N�^�[�̈ړ�����
    if( g_InputBuff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;
    //�W�����v����
    if( !work->JumpFlag )
    {//���n���̏���
        if( g_DownInputBuff & KEY_Z    )
        {//�L�[���͂ŃW�����v
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
     }else{
      //�W�����v������
        work->Acc += GRAVITY;
        work->Human.Y += work->Acc;
        if( work->Human.Y > GRAND_LINE - CHR_HEIGHT)
        {//�W�����v�����I��
            work->Human.Y =  GRAND_LINE - CHR_HEIGHT;
            work->JumpFlag = false;
        }
    }

    //�e�̏���
    //�L�����N�^�[��X���W�̂ݒǏ]����
    work->Shadow.X = work->Human.X;
    //Y���W�͒n�ʂɍ��킹��i�����ł͌Œ�l�j
    work->Shadow.Y = GRAND_LINE;

    //�e�̕`����ɍs�Ȃ�
    SpriteDraw( &work->Shadow, 1);
    SpriteDraw( &work->Human, 0);
}



////////////////////////////////////////////////////
//�T�́|�Q�O �}�E�X�ȂǂŎ��R�ɃX�N���[��������
////////////////////////////////////////////////////

typedef struct{
    BACK_GROUND    BG;
} EX05_20_STRUCT;

typedef struct{
    int            AddX;           //�}�E�X�ɔ������������Z����w�̒l
    int            AddY;           //�}�E�X�ɔ������������Z����x�̒l
    RECT           MoveRect;       //�}�E�X����������̈�
} EX05_20_DATA;

void init05_20(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_20(TCB* thisTCB)
{
//�}�E�X���ړ�������|�C���g�̐�
#define POINT_COUNT 4

    EX05_20_STRUCT* work = (EX05_20_STRUCT*)thisTCB->Work;
    EX05_20_DATA  MovePoint[] = 
{//    X    Y    RECT
    {  0,   4, {   0,   0, 640,  16,},},      //�P�� ��
    {  0,  -4, {   0, 464, 640, 480,},},      //�Q�� ��
    { -4,   0, { 624,   0, 640, 480,},},      //�R�� �E
    {  4,   0, {   0,   0,  16, 480,},},      //�S�� ��
};
    int loop;

    //�}�E�X�J�[�\���ɔ�������|�C���g�̐��������[�v
    for( loop = 0; loop < POINT_COUNT ;loop++ ) 
    {//�}�E�X�J�[�\������������̈���`�F�b�N
        if( g_MousePos.y > MovePoint[ loop ].MoveRect.top    && 
            g_MousePos.y < MovePoint[ loop ].MoveRect.bottom &&
            g_MousePos.x > MovePoint[ loop ].MoveRect.left   &&
            g_MousePos.x < MovePoint[ loop ].MoveRect.right  )
        {//�̈���ɂ���΁A���W�����Z
            work->BG.X += MovePoint[ loop ].AddX;
            work->BG.Y += MovePoint[ loop ].AddY;
        }
    }

    //�X�N���[������ʂ͈͓̔��Ɏ��߂�
    if( work->BG.X > 0    ) work->BG.X = 0;
    if( work->BG.X < -640 ) work->BG.X = -640;
    if( work->BG.Y > 0    ) work->BG.Y = 0;
    if( work->BG.Y < -32  ) work->BG.Y = -32;

    BGDraw(&work->BG,0);
}





////////////////////////////////////////////////////
//�T�́|�Q�P �}�E�X�ȂǂŎ��R�ɃX�N���[��������Q
////////////////////////////////////////////////////
typedef struct{
    BACK_GROUND    BG;
    POINT          MouseOldPos;
} EX05_21_STRUCT;

void init05_21(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_21(TCB* thisTCB)
{
    EX05_21_STRUCT* work = (EX05_21_STRUCT*)thisTCB->Work;


    //�}�E�X���{�^�����`�F�b�N
    if( g_MouseButton & MOUSE_L )
    {//�h���b�O���̏���
        //�}�E�X�̈ړ��ʂ��Z�o�A�X�N���[���l�Ƃ��ĉ��Z����
        work->BG.X += g_MousePos.x - work->MouseOldPos.x;
        work->BG.Y += g_MousePos.y - work->MouseOldPos.y;
    }
    //1�t���[���O�̃}�E�X�ʒu��ۑ�
    work->MouseOldPos = g_MousePos;

    //�X�N���[������ʂ͈͓̔��Ɏ��߂�
    if( work->BG.X > 0    ) work->BG.X = 0;
    if( work->BG.X < -640 ) work->BG.X = -640;
    if( work->BG.Y > 0    ) work->BG.Y = 0;
    if( work->BG.Y < -32  ) work->BG.Y = -32;

    BGDraw(&work->BG,0);
}







////////////////////////////////////////////////////
//�U�� �ړ�
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�U�́|�Q  �ڕW�Ɍ������Ĉړ�
////////////////////////////////////////////////////

typedef struct{
    SPRITE         Ball;
    SPRITE         Target;               //�ڕW�̃X�v���C�g
    int            Time;
} EX06_02_STRUCT ;

void init06_02(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_02(TCB* thisTCB)
{
#define START_X     SCREEN_WIDTH/2
#define START_Y     SCREEN_HEIGHT/2
#define TARGET_X    480
#define TARGET_Y    400
#define MOVE_SPEED  10.0

    EX06_02_STRUCT* work = (EX06_02_STRUCT*)thisTCB->Work;
    float direction;    //�i�s����

    //�����������A��莞�Ԃ��Ƃɏ����������
    if( work->Time-- == 0 )
    {
        work->Ball.X   = START_X;
        work->Ball.Y   = START_Y;
        work->Target.X = TARGET_X;
        work->Target.Y = TARGET_Y;

        work->Time = 30;
    }
    //���W���瑊��ւ̕������v�Z
    direction = atan2( TARGET_Y - START_Y, TARGET_X - START_X );
    //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
    work->Ball.X += cos( direction ) * MOVE_SPEED;
    work->Ball.Y += sin( direction ) * MOVE_SPEED;

    SpriteDraw( &work->Ball, 1);
    SpriteDraw( &work->Target, 0);
}




////////////////////////////////////////////////////
//�U�́|�R  �L�������Ȑ��I�Ɉړ�
////////////////////////////////////////////////////

typedef struct{
    SPRITE         Ball;
    float          Time;
    float          AddX;
    float          SinNum;
} EX06_03_STRUCT ;

void init06_03(TCB* thisTCB)
{
#define MOVE_SPEED  2.0
    EX06_03_STRUCT* work = (EX06_03_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //���W�̏�����
    work->Ball.X   = SCREEN_WIDTH  / 3;
    //�ړ����x�̏�����
    work->AddX     = -MOVE_SPEED;
}

void exec06_03(TCB* thisTCB)
{
#define CHANGE_TIME 100      //���E�ړ��؂�ւ�����
#define SIN_WIDTH   50.0     //�J�[�u�̗h�ꕝ
#define SIN_SPEED   0.1      //�U���̑��x

    EX06_03_STRUCT* work = (EX06_03_STRUCT*)thisTCB->Work;


    //��莞�Ԃ��Ƃɍ��E�ړ���؂�ւ���
    if( work->Time-- <= 0 )
    {
        //�ړ������𔽓]������
        work->AddX *= -1;

        //���̐؂�ւ�����
        work->Time = CHANGE_TIME;
    }

    //�y�L�[�������Ă���Ԃ͋Ȑ��p�̒l�����Z
    if( g_InputBuff & KEY_Z ) work->SinNum += SIN_SPEED;

    //���E�ړ��͒����I�ȉ^���̂�
    work->Ball.X += work->AddX;

    //�㉺�ړ���Sin�֐���p���ċȐ���ɂ���
    work->Ball.Y = SCREEN_HEIGHT / 2; //��{���W
    work->Ball.Y += sin( work->SinNum ) * SIN_WIDTH;

    SpriteDraw( &work->Ball, 0);
}





////////////////////////////////////////////////////
//�U�́|�S  �X�N���[���ɍ��킹�ăL�����𓮂���
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    BACK_GROUND    BG;
    int            ScrollPosX;
    int            MyShipPosX;
    int            MyShipPosY;
} EX06_04_STRUCT;

void init06_04(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[1] );
}

void exec06_04(TCB* thisTCB)
{
#define MOVE_SPEED 8
#define SCROLL_SPEED 2
    EX06_04_STRUCT* work = (EX06_04_STRUCT*)thisTCB->Work;

    if( g_InputBuff & KEY_Z    )work->ScrollPosX -= SCROLL_SPEED;
    if( g_InputBuff & KEY_X    )work->ScrollPosX += SCROLL_SPEED;

    //�w�i�����[�v������
    work->BG.X = work->ScrollPosX % 640;
    BGDraw(&work->BG,0);


    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShipPosY -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShipPosY += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShipPosX += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShipPosX -= MOVE_SPEED;

    //�X�N���[���ɍ��킹��
    work->MyShip.X = work->ScrollPosX + work->MyShipPosX;
    work->MyShip.Y = work->MyShipPosY;

    SpriteDraw(&work->MyShip,1);

}






////////////////////////////////////////////////////
//�U�́|�T  �~�^��
////////////////////////////////////////////////////

void init06_05(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_05(TCB* thisTCB)
{
#define CIRCLE_CENTER_X 320-16   //��]���S�_�w
#define CIRCLE_CENTER_Y 240-16   //��]���S�_�x
#define CIRCLE_RADIUS   80       //��]���a
#define CIRCLE_SPEED    0.05     //��]���x

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;
    //��]���x
    pspr->Count += CIRCLE_SPEED;     
    //�~�^���v�Z
    pspr->X = CIRCLE_CENTER_X + sin(pspr->Count) * CIRCLE_RADIUS;
    pspr->Y = CIRCLE_CENTER_Y + cos(pspr->Count) * CIRCLE_RADIUS;

    SpriteDraw(pspr,0);
}





////////////////////////////////////////////////////
//�U�́|�U �ȉ~�^��
////////////////////////////////////////////////////
typedef struct{
  int        BallCount;
  int        Time;
} EX06_06_STRUCT;

static float EX06_06_gDist;
void exec06_06_ball(TCB* thisTCB)
{
#define CIRCLE_CENTER_X 320-16   //��]���S�_�w
#define CIRCLE_CENTER_Y 240-16   //��]���S�_�x
#define CIRCLE_RADIUS   80       //��]���a
#define CIRCLE_SPEED    0.05     //��]���x

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;

    //��]���x
    pspr->Count += CIRCLE_SPEED;

    //�ȉ~�^���v�Z
    pspr->X = CIRCLE_CENTER_X + sin(pspr->Count-EX06_06_gDist) * CIRCLE_RADIUS;
    pspr->Y = CIRCLE_CENTER_Y + cos(pspr->Count+EX06_06_gDist) * CIRCLE_RADIUS;

    SpriteDraw(pspr,0);
}

void init06_06(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    EX06_06_gDist = 0.0;
}

void exec06_06(TCB* thisTCB)
{
#define  BALL_COUNT 16 
    EX06_06_STRUCT* work = (EX06_06_STRUCT*)thisTCB->Work;

    //�W�t���[�����Ƀ{�[�����쐬
    if(work->Time++ == 0x08)
    {
        work->Time = 0;
        if(work->BallCount++ < BALL_COUNT)
        {//��萔�쐬����܂ŌJ��Ԃ�
            TaskMake(exec06_06_ball,0x1000);
        }
    }
    //�c�ݒl
    EX06_06_gDist += 0.01;
}



////////////////////////////////////////////////////
//�U�́|�V �������ړ�
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc_Speed;
    float          Speed;
} EX06_07_STRUCT ;

void exec06_07_acc(TCB* thisTCB)
{
#define STOP_LINE       380     //��~���C��

    EX06_07_STRUCT* work = (EX06_07_STRUCT*)thisTCB->Work;

    work->Speed   += work->Acc_Speed;
    work->sprt.Y  += work->Speed;

    if( work->sprt.Y > STOP_LINE )
    {
        work->Acc_Speed   = 0.0;
        work->Speed       = 0.0;
    }

    SpriteDraw(&work->sprt,0);
}

void init06_07(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_07(TCB* thisTCB)
{
#define START_LINE      64       //�J�n���C��
#define ACC_SPEED       0.1      //���x
    TCB* child_tcb;
    EX06_07_STRUCT* work;

    child_tcb = TaskMake(exec06_07_acc,0x2000);
    work = (EX06_07_STRUCT*)child_tcb->Work;
    work->sprt.X= 400;
    work->sprt.Y= START_LINE;
    work->Acc_Speed = ACC_SPEED;

    child_tcb = TaskMake(exec06_07_acc,0x2000);
    work = (EX06_07_STRUCT*)child_tcb->Work;
    work->sprt.X= 240;
    work->sprt.Y= START_LINE;
    work->Acc_Speed = -ACC_SPEED;
    work->Speed     = 8.0;        //�����l


    TaskKill( thisTCB );
}





////////////////////////////////////////////////////
//�U�́|�W �U���e�P  ��ɑ����_��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
} EX06_08_STRUCT ;


void exec06_08_horming(TCB* thisTCB)
{
#define MOVE_SPEED  2.0
    EX06_08_STRUCT* work = (EX06_08_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //�ڕW�̍��W���擾
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float direction;

    //�ڕW�̕������擾
    direction= atan2(  targetY - my_Y , targetX - my_X);

    //�����ւ̈ړ��l���擾
    work->sprt.X += cos(direction)*MOVE_SPEED;
    work->sprt.Y += sin(direction)*MOVE_SPEED;

    SpriteDraw(&work->sprt,1);
}

void exec06_08_target(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_08(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_08(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_08_STRUCT* tmp_work;

    //�ڕW�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_08_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;

    //�U���e�̏������ƁA�ڕW�X�v���C�g��ݒ�
    tmp_tcb = TaskMake( exec06_08_horming, 0x2000 );
    tmp_work = (EX06_08_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;


    //������A�����I��
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//�U�́|�X �U���e�Q  ���������đ����_��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
    float          SpeedX;               //���x
    float          SpeedY;
} EX06_09_STRUCT ;


void exec06_09_horming(TCB* thisTCB)
{
#define ACC_SPEED  1.0
    EX06_09_STRUCT* work = (EX06_09_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //�ڕW�̍��W���擾
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float acc_speedX;
    float acc_speedY;
    float direction;

    //�ڕW�Ƃ̑��Έʒu�����ɁA�����������擾
    if( targetX - my_X >= 0) acc_speedX =  ACC_SPEED;
    if( targetX - my_X <  0) acc_speedX = -ACC_SPEED;
    if( targetY - my_Y >= 0) acc_speedY =  ACC_SPEED;
    if( targetY - my_Y <  0) acc_speedY = -ACC_SPEED;


    //�����l�����ɑ��x���Z�o
    work->SpeedX += acc_speedX;
    work->SpeedY += acc_speedY;

    work->sprt.X += work->SpeedX;
    work->sprt.Y += work->SpeedY;

    SpriteDraw(&work->sprt,1);
}

void exec06_09_target(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_09(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_09(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_09_STRUCT* tmp_work;

    //�ڕW�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_09_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;


    //�U���e�̏������ƁA�ڕW�X�v���C�g��ݒ�
    tmp_tcb = TaskMake( exec06_09_horming, 0x2000 );
    tmp_work = (EX06_09_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;

    //������A�����I��
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//�U�́|�P�O �U���e�R  ��荞�ނ悤�ɑ_��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
    float          Direction;            //����
} EX06_10_STRUCT ;

void exec06_10_horming(TCB* thisTCB)
{
#define ROT_SPEED  0.05    //���񑬓x
#define MOVE_SPEED  4.0
    EX06_10_STRUCT* work = (EX06_10_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //�ڕW�̍��W���擾
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float direction;


    //�ڕW�̕������擾
    direction= atan2( targetY - my_Y , targetX - my_X );

    //�������}�o�h�͈͓̔��Ɋۂ߂�
    if( work->Direction - direction >  M_PI) work->Direction -= M_PI * 2;
    if( work->Direction - direction < -M_PI) work->Direction += M_PI * 2;

    //�����
    if( work->Direction - direction < 0 ) work->Direction += ROT_SPEED;
    //�E���
    if( work->Direction - direction > 0 ) work->Direction -= ROT_SPEED;


    //�i�s�����ւ̈ړ��l���擾
    work->sprt.X += cos(work->Direction)*MOVE_SPEED;
    work->sprt.Y += sin(work->Direction)*MOVE_SPEED;

    SpriteDraw(&work->sprt,1);
}

void exec06_10_target(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_10(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_10(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_10_STRUCT* tmp_work;

    //�ڕW�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_10_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;

    //�U���e�̏������ƁA�ڕW�X�v���C�g��ݒ�
    tmp_tcb = TaskMake( exec06_10_horming, 0x2000 );
    tmp_work = (EX06_10_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;

    //������A�����I��
    TaskKill( thisTCB );
}



////////////////////////////////////////////////////
//�U�́|�P�P  �f�[�^�ɉ����ăL�����𓮂���
////////////////////////////////////////////////////
#define MOVE_SPEED 8.0
#define END_DATA   -999.0
typedef struct{
    SPRITE      Sprt;
    int         Time;
    int         Point;
    //�ڕW�ւ̈ړ����x
    float     AddX;
    float     AddY;
} EX06_11_STRUCT;

void init06_11(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_11(TCB* thisTCB)
{
    EX06_11_STRUCT* work = (EX06_11_STRUCT*)thisTCB->Work;
    float direction;
    float distance;
    float posX;
    float posY;
//�ړ���f�[�^
float point_data[][2] = 
{//  X     Y
    { 320.0,   0.0,},  //0�������W
    { 320.0,  96.0,},  //1
    { 480.0,  96.0,},  //2
    { 448.0, 256.0,},  //3
    {  96.0, 256.0,},  //4
    {  96.0, 320.0,},  //5
    { 560.0, 400.0,},  //6
    { 560.0, 480.0,},  //7
    {END_DATA,END_DATA,},  //�I��
};

    //���������A�܂��͈�莞�ԂŖڕW���X�V
    if(work->Time-- <= 0)
    {
        //���̖ڕW���`�F�b�N
        if( point_data[work->Point+1][0] == END_DATA)
        {//�f�[�^���I�����Ă�����A�f�[�^�����߂ɖ߂�
         //(�T���v���̏ꍇ �ʏ�͂��̂܂܏I�������j
            work->Point = 0;
        }

        //�ړ��J�n���W���w��
        //�i�ړ��I�����̌덷�C���̈ӂ�����j
        work->Sprt.X = point_data[work->Point][0];
        work->Sprt.Y = point_data[work->Point][1];

        //�ڕW����̑��΍��W���v�Z
        posX = point_data[work->Point+1][0] - work->Sprt.X;
        posY = point_data[work->Point+1][1] - work->Sprt.Y;


        //�������v�����A���x����ڕW�ւ̏��v���Ԃ�����o��
        distance = sqrtf( (posX * posX) + (posY * posY) );
        work->Time = distance / MOVE_SPEED;

        //�ڕW�̕����v�Z
        direction = atan2( posY, posX);

        //��������ړ����x��ݒ�
        work->AddX = cos( direction ) * MOVE_SPEED;
        work->AddY = sin( direction ) * MOVE_SPEED;

        //���̃f�[�^
        work->Point++;
    }

    //�ړ�����
    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    //�{�[���\��
    SpriteDraw( &work->Sprt, 0);
}






////////////////////////////////////////////////////
//�U�́|�P�Q  �U��q�̂悤�ȓ���
////////////////////////////////////////////////////

void init06_12(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_12(TCB* thisTCB)
{
#define PENDULUM_CENTER_X 320-16   //���S�_�w
#define PENDULUM_CENTER_Y 240-16   //���S�_�x
#define PENDULUM_RADIUS   80       //�U��q�̔��a
#define PENDULUM_SPEED    0.15     //�������x
#define PENDULUM_RADIUS2   1.0     //�U��q�̉����̕�

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;
    //�U��q�̑��x
    pspr->Count += PENDULUM_SPEED;

    //�^���v�Z
    pspr->X = PENDULUM_CENTER_X + sin( sin( pspr->Count ) * PENDULUM_RADIUS2 ) * PENDULUM_RADIUS;
    pspr->Y = PENDULUM_CENTER_Y + cos( sin( pspr->Count ) * PENDULUM_RADIUS2 ) * PENDULUM_RADIUS;

    SpriteDraw(pspr,0);
}





////////////////////////////////////////////////////
//�U�́|�P�R �ړ��Ɋ���������
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Sprt;
    float          SpeedX;          //�w�����ւ̉����l
    float          SpeedY;          //�x�����ւ̉����l
} EX06_13_STRUCT ;


void init06_13(TCB* thisTCB)
{
    EX06_13_STRUCT* work = (EX06_13_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //���@�̃X�v���C�g
    work->Sprt.X = SCREEN_WIDTH/2;
    work->Sprt.Y = SCREEN_HEIGHT/2;
}

void exec06_13(TCB* thisTCB)
{
#define ACC_SPEED  1.0
    EX06_13_STRUCT* work = (EX06_13_STRUCT*)thisTCB->Work;

    float acc_speedX = 0;
    float acc_speedY = 0;

    //�����A�ړ����Ă����猸�����s��
    if(work->SpeedX > 0 ) acc_speedX = -ACC_SPEED;
    if(work->SpeedX < 0 ) acc_speedX =  ACC_SPEED;

    if(work->SpeedY > 0 ) acc_speedY = -ACC_SPEED;
    if(work->SpeedY < 0 ) acc_speedY =  ACC_SPEED;

    //�L�[���͂ɂ��������s��
    if( g_InputBuff & KEY_UP    ) acc_speedY = -ACC_SPEED;
    if( g_InputBuff & KEY_DOWN  ) acc_speedY =  ACC_SPEED;

    if( g_InputBuff & KEY_LEFT  ) acc_speedX = -ACC_SPEED;
    if( g_InputBuff & KEY_RIGHT ) acc_speedX =  ACC_SPEED;

    work->SpeedX += acc_speedX;
    work->SpeedY += acc_speedY;
    work->Sprt.X += work->SpeedX;
    work->Sprt.Y += work->SpeedY;


    SpriteDraw( &work->Sprt, 0);
}



////////////////////////////////////////////////////
//�U�́|�P�S �I�v�V�����P  �L�����̎��͂���񂷂�
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Myship;               //���@���
    int            OptionID;
    float          Rot;                  //��]
} EX06_14_STRUCT ;

void exec06_14_option(TCB* thisTCB)
{
#define ROT_SPEED    0.05        //��]���x
#define ROT_RADIUS   96          //��]���a
//�e�I�v�V�����̉�]�Ԋu
float option_table[] = { 0.0, M_PI * 0.5, M_PI, M_PI * 1.5,};

    EX06_14_STRUCT* work = (EX06_14_STRUCT*)thisTCB->Work;
    //���@�̍��W���擾
    float my_shipX = work->Myship->X + 16;
    float my_shipY = work->Myship->Y + 16;

    work->Rot += ROT_SPEED;     
    //���x�ቺ��h�����߂Ɋۂ߂�
    if( work->Rot >  M_PI) work->Rot -= M_PI * 2;

    work->sprt.X = my_shipX + cos(work->Rot + option_table[ work->OptionID ] ) * ROT_RADIUS;
    work->sprt.Y = my_shipY + sin(work->Rot + option_table[ work->OptionID ] ) * ROT_RADIUS;

    SpriteDraw(&work->sprt,1);

}

void exec06_14_my_ship(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_14(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_14(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* my_ship_sprt;
    EX06_14_STRUCT* tmp_work;
    int loop;


    //���@�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_14_my_ship, 0x1000 );
    my_ship_sprt = (SPRITE*)tmp_tcb->Work;
    my_ship_sprt->X = SCREEN_WIDTH/2;
    my_ship_sprt->Y = SCREEN_HEIGHT/2;

    //�I�v�V�������S�����A���@���[�N����ݒ�
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_14_option, 0x2000 );
        tmp_work = (EX06_14_STRUCT*)tmp_tcb->Work;
        tmp_work->Myship = my_ship_sprt;
        tmp_work->OptionID = loop;
    }
    //������A�����I��
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//�U�́|�P�T �I�v�V�����Q  ��ɃL������ǂ�������
////////////////////////////////////////////////////

typedef struct{
    SPRITE         sprt;
    int            PosPointer;
    D3DXVECTOR2    PosBuff[64];       //���W����
} EX06_15_MY_SHIP;

typedef struct{
    SPRITE            sprt;
    EX06_15_MY_SHIP*  Myship;               //���@���
    int               OptionID;
} EX06_15_OPTION;


void exec06_15_option(TCB* thisTCB)
{
    EX06_15_OPTION* work = (EX06_15_OPTION*)thisTCB->Work;
    int option_pos_pointer;

    //�I�v�V�����̂h�c����\��������W�̗������擾
    option_pos_pointer = work->Myship->PosPointer + (work->OptionID * 16 + 15);
    option_pos_pointer %= 64;

    work->sprt.X = work->Myship->PosBuff[ option_pos_pointer ].x + 16;
    work->sprt.Y = work->Myship->PosBuff[ option_pos_pointer ].y + 16;

    SpriteDraw(&work->sprt,1);
}

void exec06_15_my_ship(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    EX06_15_MY_SHIP* work = (EX06_15_MY_SHIP*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->sprt.X -= MOVE_SPEED;

    work->PosPointer++;
    work->PosPointer %= 64;
    //�ړ����W�𗚗��ɋL�^
    work->PosBuff[work->PosPointer].x = work->sprt.X;
    work->PosBuff[work->PosPointer].y = work->sprt.Y;

    SpriteDraw( &work->sprt, 0);
}

void init06_15(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_15(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    EX06_15_MY_SHIP* my_ship;
    EX06_15_OPTION*  tmp_work;
    int loop;


    //���@�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_15_my_ship, 0x1000 );
    my_ship = (EX06_15_MY_SHIP*)tmp_tcb->Work;
    my_ship->sprt.X = SCREEN_WIDTH/2;
    my_ship->sprt.Y = SCREEN_HEIGHT/2;

    //�I�v�V�������S�����A���@���[�N����ݒ�
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_15_option, 0x2000 );
        tmp_work = (EX06_15_OPTION*)tmp_tcb->Work;
        tmp_work->Myship = my_ship;
        tmp_work->OptionID = loop;
    }
    //������A�����I��
    TaskKill( thisTCB );
}



////////////////////////////////////////////////////
//�U�́|�P�U �I�v�V�����R
////////////////////////////////////////////////////

typedef struct{
    SPRITE         sprt;
    int            PosPointer;
    D3DXVECTOR2    PosBuff[64];             //���W����
} EX06_16_MY_SHIP;

typedef struct{
    SPRITE            sprt;
    EX06_16_MY_SHIP*  Myship;               //���@���
    int               OptionID;
} EX06_16_OPTION;


void exec06_16_option(TCB* thisTCB)
{
    EX06_16_OPTION* work = (EX06_16_OPTION*)thisTCB->Work;
    int option_pos_pointer;

    //�I�v�V�����̂h�c����\��������W�̗������擾
    option_pos_pointer = work->Myship->PosPointer + (work->OptionID * 16 + 15);
    option_pos_pointer %= 64;

    work->sprt.X = work->Myship->PosBuff[ option_pos_pointer ].x + 16;
    work->sprt.Y = work->Myship->PosBuff[ option_pos_pointer ].y + 16;

    SpriteDraw(&work->sprt,1);
}

void exec06_16_my_ship(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    EX06_16_MY_SHIP* work = (EX06_16_MY_SHIP*)thisTCB->Work;
    BOOL move_flag = FALSE;
    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    )
    {
        work->sprt.Y -= MOVE_SPEED;
        move_flag = TRUE;
    }

    if( g_InputBuff & KEY_DOWN  )
    {
        work->sprt.Y += MOVE_SPEED;
        move_flag = TRUE;
    }
    if( g_InputBuff & KEY_RIGHT )
    {
        work->sprt.X += MOVE_SPEED;
        move_flag = TRUE;
    }
    if( g_InputBuff & KEY_LEFT  )
    {
        work->sprt.X -= MOVE_SPEED;
        move_flag = TRUE;
    }

    //�ړ����s��ꂽ���̂ݗ������X�V
    if( move_flag )
    {
        work->PosPointer++;
        work->PosPointer %= 64;
        //�ړ����W�𗚗��ɋL�^
        work->PosBuff[work->PosPointer].x = work->sprt.X;
        work->PosBuff[work->PosPointer].y = work->sprt.Y;
    }
    SpriteDraw( &work->sprt, 0);
}

void init06_16(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_16(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    EX06_16_MY_SHIP* my_ship;
    EX06_16_OPTION*  tmp_work;
    int loop;

    //���@�̃X�v���C�g
    tmp_tcb = TaskMake( exec06_16_my_ship, 0x1000 );
    my_ship = (EX06_16_MY_SHIP*)tmp_tcb->Work;
    my_ship->sprt.X = SCREEN_WIDTH/2;
    my_ship->sprt.Y = SCREEN_HEIGHT/2;

    //�I�v�V�������S�����A���@���[�N����ݒ�
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_16_option, 0x2000 );
        tmp_work = (EX06_16_OPTION*)tmp_tcb->Work;
        tmp_work->Myship = my_ship;
        tmp_work->OptionID = loop;
    }
    //������A�����I��
    TaskKill( thisTCB );
}





////////////////////////////////////////////////////
//�U�́|�P�V �W�����v������ɂ�
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            JumpFlag;
} EX06_17_STRUCT;


void init06_17(TCB* thisTCB)
{
    EX06_17_STRUCT* work = (EX06_17_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_17(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //�W�����v��
#define GRAVITY  0.25     //�d�͒l

    EX06_17_STRUCT* work = (EX06_17_STRUCT*)thisTCB->Work;

    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {//���n���̏���
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //���n������W�����v�����I��
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {//���W���A�n�ʍ��W��艺�Ȃ�΁A���n�����Ƃ݂Ȃ�
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}


////////////////////////////////////////////////////
//�U�́|�P�W �o�E���h��������
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            BoundFlag;
} EX06_18_STRUCT;

void init06_18(TCB* thisTCB)
{
    EX06_18_STRUCT* work = (EX06_18_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_18(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //�W�����v��
#define GRAVITY  0.25     //�d�͒l
#define BOUND_DECR     1.0      //�o�E���h���̌���

    EX06_18_STRUCT* work = (EX06_18_STRUCT*)thisTCB->Work;

    //�o�E���h���͔������Ȃ�
    if( !work->BoundFlag )
    {
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�o�E���h�J�n������
            work->Acc = -JUMP_POWER;
            work->BoundFlag = true;
        }
     }

    if( work->BoundFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //�n�ʂɍĐڐG������A�o�E���h����
        if( work->sprt.Y > SCREEN_HEIGHT / 2 )
        {
            work->sprt.Y -= work->Acc;
            work->Acc = -( work->Acc - BOUND_DECR );    //�o�E���h���Ɍ�������

            if( work->Acc > -1.0 )
            { //�w�ǃo�E���h���Ȃ��Ȃ�����I��
                work->sprt.Y =  SCREEN_HEIGHT / 2;
                work->BoundFlag = false;
            }
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//�U�́|�P�X �{�^�������������ŃW�����v�̍�����ς���
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            JumpFlag;
    float          Gravity;
} EX06_19_STRUCT;
void init06_19(TCB* thisTCB)
{
    EX06_19_STRUCT* work = (EX06_19_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_19(TCB* thisTCB)
{
#define JUMP_POWER  10.0        //�W�����v��
#define PUSH_GRAVITY  0.25      //�������̏d�͒l
#define FALL_RATE     8.0       //�������x�̌v�Z�l

    EX06_19_STRUCT* work = (EX06_19_STRUCT*)thisTCB->Work;


    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {//���n���̏���
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
            work->Gravity = PUSH_GRAVITY;
        }
     }

    if( work->JumpFlag )
    {
        //�W�����v�{�^���������ꂽ���H
        if( g_UpInputBuff & KEY_Z  )
        {
            //�㏸����������
            if( work->Acc < 0 )
            {//�{�^���������ꂽ���̏㏸���x�ɉ����ė������x��ς���
                work->Gravity = -work->Acc / FALL_RATE;
            }
        }

        work->Acc += work->Gravity;
        work->sprt.Y += work->Acc;
        //���n������W�����v�����I��
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//�U�́|�Q�O �D���ȍ����ŃW�����v����
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    float          Gravity;
    int            JumpFlag;
} EX06_20_STRUCT;

void init06_20(TCB* thisTCB)
{
    EX06_20_STRUCT* work = (EX06_20_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;
}

float GetJumpPower( float Time, float Height )
{
    return Height / (Time * (Time + 1.0) * (1.0 / 2.0) );                //���� �� ���ԁ~���ԁ{�P �~ �P�^�Q
}


void exec06_20(TCB* thisTCB)
{
#define JUMP_POS_A  256.0  //�W�����v����
#define JUMP_POS_B  128.0  //�W�����v����
#define JUMP_TIME_A 30.0   //���_�܂ł̎���
#define JUMP_TIME_B 60.0   //���_�܂ł̎���

    EX06_20_STRUCT* work = (EX06_20_STRUCT*)thisTCB->Work;

    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������A
            work->Acc = -(GetJumpPower(JUMP_TIME_A , JUMP_POS_A) * JUMP_TIME_A);
            work->Gravity = GetJumpPower(JUMP_TIME_A , JUMP_POS_A);
            work->JumpFlag = true;

        }
        if( g_DownInputBuff & KEY_X    )
        {//�W�����v�J�n������B
            work->Acc = -(GetJumpPower(JUMP_TIME_B , JUMP_POS_B) * JUMP_TIME_B);
            work->Gravity = GetJumpPower(JUMP_TIME_B , JUMP_POS_B);
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += work->Gravity;
        work->sprt.Y += work->Acc;

        //���n������W�����v�����I��
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//�U�́|�Q�P �X�N���[������ƓG���o�Ă���l�ɂ���
////////////////////////////////////////////////////

typedef struct{
    BACK_GROUND    BG;
    int            ScrollPoint;
    int            DataIndex;
} EX06_21_STRUCT;

typedef struct{
  SPRITE     Sprt;
  int        InitFlag;
} EX06_21_BALL;

void exec06_21_ball(TCB* thisTCB)
{
#define MOVE_SPEED 16
    EX06_21_BALL* work = (EX06_21_BALL*)thisTCB->Work;

    if( work->InitFlag != TRUE )
    {//������
        //���W�̐ݒ�
        work->Sprt.X = rand() % SCREEN_WIDTH;
        //�������I���̃t���O
        work->InitFlag = TRUE;
    }
    //�ړ������A�ォ�痎������
    work->Sprt.Y += MOVE_SPEED;

    SpriteDraw( &work->Sprt ,0);
}


void init06_21(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\BG.png",&g_pTex[1] );
}

void exec06_21(TCB* thisTCB)
{
#define MOVE_SPEED 8
    EX06_21_STRUCT* work = (EX06_21_STRUCT*)thisTCB->Work;
    char str[256];
int scroll_table[] = 
{//�o���f�[�^
    30,  50,  60,  70,
    75,  80, 100, 110,
   111, 112, 113, 114,
   120, 150, 175, 180,
   188, 192, 224, 276,
   280, 292, 300, 304,
   309, 312, 322, 340,
   350, 389, 392, 448,
   -1,
};


    //�o���f�[�^�ƃX�N���[���|�C���g���r
    if( work->ScrollPoint == scroll_table[ work->DataIndex ] )
    {//��v������o��
        TaskMake( exec06_21_ball, 0x2000 );

        //���̔�r�f�[�^
        work->DataIndex++;
    }

    //�X�N���[������

    //�����L�[���͂�����Ώ���������
    if( g_DownInputBuff & KEY_Z    )
    {
        work->ScrollPoint = 0;
        work->DataIndex = 0;
    }

    //�X�N���[���|�C���g�����W�ɕϊ�
    work->BG.X = -work->ScrollPoint;
    //��ʒ[�ɓ��B���Ă��Ȃ���΃X�N���[��������
    if(work->BG.X >  -384 ) work->ScrollPoint++;

    BGDraw(&work->BG,1);

    sprintf( str,"'Z'KEY Scroll Reset!!\n SCROLL POINT %d",work->ScrollPoint );
    FontPrint( 128,128, str);

}




////////////////////////////////////////////////////
//�U�́|�Q�Q ���፷�̂���n�`���ړ�����ɂ�
////////////////////////////////////////////////////
#define DATA_SIZE 4
#define HEIGHT_DATA (SCREEN_WIDTH/DATA_SIZE)
#define HEIGHT_BASE_POS (SCREEN_HEIGHT-64)
#define UP_DOWN 32
#define MOVE_SPEED 4.0

#define HUMAN_CENTER_X 32
#define HUMAN_CENTER_Y 128

typedef struct{
    SPRITE   Human;
    SPRITE   GrandBar;
    float    X;
    float    Y;
    float    HeightData[ HEIGHT_DATA ];
} EX06_22_STRUCT;

void init06_22(TCB* thisTCB)
{
    EX06_22_STRUCT* work = (EX06_22_STRUCT*)thisTCB->Work;
    int loop;
    float sin_wave = 0;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\bar.png",&g_pTex[1] );

    //���W�̐ݒ�
    work->X = SCREEN_WIDTH / 2;

    for( loop=0;loop < HEIGHT_DATA; loop++)
    { //sin�֐��𗘗p���ď㉺�̂���n�`�𐶐�
        work->HeightData[loop] = 
            sin( sin_wave ) * UP_DOWN + HEIGHT_BASE_POS;
        sin_wave += 0.125;
    }

}

void exec06_22(TCB* thisTCB)
{
    EX06_22_STRUCT* work = (EX06_22_STRUCT*)thisTCB->Work;
    int loop;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;
    //�ړ��͈͂���ʓ��ɂ���
    if(work->X >= SCREEN_WIDTH - DATA_SIZE)
        work->X = SCREEN_WIDTH - DATA_SIZE;
    if(work->X <  0)work->X = 0;

    //�w���W����A�n�ʂ̍����f�[�^���擾
    work->Y = work->HeightData[(int)work->X/DATA_SIZE];

    //�\�����W��ݒ�
    work->Human.X = work->X - HUMAN_CENTER_X;
    work->Human.Y = work->Y - HUMAN_CENTER_Y;

    //�L�����N�^�[�̕\��
    SpriteDraw( &work->Human,0);

    for( loop=0;loop < HEIGHT_DATA; loop++)
    { //�n�`�f�[�^�̕\��
        work->GrandBar.X = loop * DATA_SIZE;
        work->GrandBar.Y = work->HeightData[loop];
        SpriteDraw( &work->GrandBar,1);
    }
}



////////////////////////////////////////////////////
//�U�́|�Q�R �K�w�̂���n�ʂɒ��n����ɂ�
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE         Land;
    float          Acc;
    int            JumpFlag;
} EX06_23_STRUCT;

#define GRAND            (SCREEN_HEIGHT / 2)  //�n�ʍ��W
#define LAND_AREA_TOP    GRAND - 144          //���n����K�w�̈�
#define LAND_AREA_BOTTOM GRAND - 128
#define JUMP_POWER  10.0  //�W�����v��
#define GRAVITY  0.25     //�d�͒l

void init06_23(TCB* thisTCB)
{
    EX06_23_STRUCT* work = (EX06_23_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[1] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = GRAND;
    //���n����K�w
    work->Land.X = SCREEN_WIDTH  / 2;
    work->Land.Y = 216;
}

void exec06_23(TCB* thisTCB)
{
    EX06_23_STRUCT* work = (EX06_23_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 128, 128, 640, 480, };

    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {//���n���̏���
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
        //������K�w�ŁA���L�[����͂��Ă�����A�������ĊK�w����~���
        if( g_DownInputBuff & KEY_DOWN  )
        {//���������J�n
            work->Acc = 0;
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //�K�w�ւ̒��n�����A�܂����~�������`�F�b�N����
        if( work->Acc > 0.0 )
        {//�������~���ɊK�w�̃G���A�ɒB������A���n�����Ƃ݂Ȃ�
            if( work->sprt.Y > LAND_AREA_TOP && work->sprt.Y < LAND_AREA_BOTTOM )
            {//�������A���L�[����͂��Ă���Ȃ�΁A�K�w����~��鏈���ׁ̈A�������Ȃ�
                if( !(g_InputBuff & KEY_DOWN))
                {//�K�w��ւ̒��n����
                    work->sprt.Y =  LAND_AREA_TOP;
                    work->JumpFlag = false;
                }
            }
        }

        //�n�ʂ̒��n����
        if( work->sprt.Y > GRAND)
        {//���W���A�n�ʍ��W��艺�Ȃ�΁A���n�����Ƃ݂Ȃ�
            work->sprt.Y =  GRAND;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //�K�w�\��
    SpriteDraw( &work->Land, 1);

    g_pFont->DrawText( NULL, "�y�L�[�ŃW�����v�A���L�[�ŊK�w����~���", -1, &font_pos, DT_LEFT, 0xffffffff);
}



////////////////////////////////////////////////////
//�U�́|�Q�S �ړ����镨�̂ɒ��n����ɂ�
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE         Land;
    float          LandMoveCount;
    float          Acc;
    int            JumpFlag;
} EX06_24_STRUCT;

#define GRAND            (SCREEN_HEIGHT / 2)  //�n�ʍ��W
#define HUMAN_WIDTH      64
#define HUMAN_HEIGHT     128
#define LAND_MOVE_SPEED  0.05;
#define LAND_WIDTH       64
#define LAND_HEIGHT      16
#define JUMP_POWER  10.0  //�W�����v��
#define GRAVITY  0.25     //�d�͒l

void init06_24(TCB* thisTCB)
{
    EX06_24_STRUCT* work = (EX06_24_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[1] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = GRAND;
    //���n���镨��
    work->Land.Y = 216;
}

void exec06_24(TCB* thisTCB)
{
    EX06_24_STRUCT* work = (EX06_24_STRUCT*)thisTCB->Work;
    int loop;
    RECT  land_rect;
    float my_pos_x;
    float my_pos_y;
    RECT font_pos = { 128, 128, 640, 480, };


    if( g_InputBuff & KEY_RIGHT ) work->sprt.X += 4;
    if( g_InputBuff & KEY_LEFT  ) work->sprt.X -= 4;

    //���̂̈ړ�
    work->LandMoveCount += LAND_MOVE_SPEED;
    work->Land.X = sin( work->LandMoveCount ) * 96 + (SCREEN_WIDTH / 2);
    //���̂̑傫���ƍ��W�����v�Z
    land_rect.left  = work->Land.X;
    land_rect.right = work->Land.X + LAND_WIDTH;
    land_rect.top   = work->Land.Y;
    land_rect.bottom= work->Land.Y + LAND_HEIGHT;
    //�����̍��W���v�Z
    my_pos_x = work->sprt.X + HUMAN_WIDTH  / 2 ;
    my_pos_y = work->sprt.Y + HUMAN_HEIGHT + 1;

    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {//���n���̏���
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }else{
            //�������`�F�b�N���A�������������̂ł���Η������J�n����
            if( !(my_pos_x > land_rect.left  && my_pos_x < land_rect.right  && 
                  my_pos_y > land_rect.top   && my_pos_y < land_rect.bottom ))
            {//���������J�n
                work->Acc = 0;
                work->JumpFlag = true;
            }
        }
    }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //�K�w�ւ̒��n�����A�܂����~�������`�F�b�N����
        if( work->Acc > 0.0 )
        {//�������~���ɕ��̂ɐڐG������A���n�����Ƃ݂Ȃ�
           if( my_pos_x > land_rect.left  && my_pos_x < land_rect.right  && 
               my_pos_y > land_rect.top   && my_pos_y < land_rect.bottom )
           {//���̏�ւ̒��n����
                //���̂̍��W�𒅒n���W�ɂ���
                work->sprt.Y =  land_rect.top - HUMAN_HEIGHT;
                work->JumpFlag = false;
           }
        }

        //�n�ʂ̒��n����
        if( work->sprt.Y > GRAND)
        {//���W���A�n�ʍ��W��艺�Ȃ�΁A���n�����Ƃ݂Ȃ�
            work->sprt.Y =  GRAND;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //���̂̕\��
    SpriteDraw( &work->Land, 1);

    g_pFont->DrawText( NULL, "�y�L�[�ŃW�����v�A�����L�[�ňړ�", -1, &font_pos, DT_LEFT, 0xffffffff);
}



////////////////////////////////////////////////////
//�U�́|�Q�T �W�����v�̒��_�ʒu��m��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            JumpFlag;
    float          JumpHeight;
    float          JumpHeightMax;
} EX06_25_STRUCT;


void init06_25(TCB* thisTCB)
{
    EX06_25_STRUCT* work = (EX06_25_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //���W������
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;
}

void exec06_25(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //�W�����v��
#define GRAVITY  0.25     //�d�͒l

    EX06_25_STRUCT* work = (EX06_25_STRUCT*)thisTCB->Work;
    float jump_height;
    float chk_height;
    float tmp_Acc;

    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�W�����v���͔������Ȃ�
    if( !work->JumpFlag )
    {//���n���̏���
        //�L�[���͂ŃW�����v����
        if( g_DownInputBuff & KEY_Z    )
        {//�W�����v�J�n������
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;

            //�ō����x�L�^�p�ɏ�����
            work->JumpHeightMax = work->sprt.Y;

            tmp_Acc = work->Acc;
            jump_height = work->sprt.Y;
            //�`�F�b�N�̏����l��jump_height���傫�����OK
            chk_height = jump_height + 1.0;
            //���x�̃`�F�b�N
            //���x��������n�߂��烋�[�v�̏I��
            while(jump_height < chk_height)
            {//�旧���ăW�����v�̌v�Z���s�Ȃ�
                chk_height = jump_height;
                tmp_Acc += GRAVITY;
                jump_height += tmp_Acc;
            }
            //�\���p�̕ϐ��ɋL�^
            work->JumpHeight = chk_height;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;
        //�W�����v���̍ō����x���L�^
        if(work->JumpHeightMax > work->sprt.Y)
            work->JumpHeightMax = work->sprt.Y;

        //���n������W�����v�����I��
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {//���W���A�n�ʍ��W��艺�Ȃ�΁A���n�����Ƃ݂Ȃ�
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //�����\��
    sprintf( str,
        "�\�z����:%f\n��������:%f",
        work->JumpHeight,
        work->JumpHeightMax);

    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}



////////////////////////////////////////////////////
//�V��  ����
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�V�́|�P  �L�����̃A�j���[�V����
////////////////////////////////////////////////////
void init07_01(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\ANIM.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[1] );
}

void exec07_01(TCB* thisTCB)
{
#define ANIM_FRAME_MAX 4          //�A�j���[�V�����̃p�^�[����
#define ANIM_CHANGE_CNT 20        //�A�j���[�V������؂�ւ���t���[����

RECT  anim_table[] = 
{ {  0,  0,  64, 128,},    //�P���ڂ̃A�j���p�^�[��
  { 64,  0, 128, 128,},    //�Q���ڂ̃A�j���p�^�[��
  {128,  0, 192, 128,},    //�R���ڂ̃A�j���p�^�[��
  {192,  0, 256, 128,},    //�S���ڂ̃A�j���p�^�[��
};

    //�^�X�N�Ƀ��[�N������t����
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;

    pspr->timer += 1.0;

    if(pspr->timer >= ANIM_CHANGE_CNT)
    {
       //�^�C�}�[��߂�
       pspr->timer -= ANIM_CHANGE_CNT;
       //�\���p�^�[�����P�p�^�[���i�߂� 
       pspr->Frame++;
       if( pspr->Frame >= ANIM_FRAME_MAX )
       {//�\���p�^�[�����ŏ��Ɋ����߂�
           pspr->Frame = 0;
       } 
    }

    pspr->SrcRect = &anim_table[pspr->Frame];
    pspr->X = 320;
    pspr->Y = 240;
    SpriteDraw(pspr,0);
}


////////////////////////////////////////////////////
//�V�́|�Q �L�����N�^�[�𓮂���
////////////////////////////////////////////////////
void init07_02(TCB* thisTCB)
{
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;//���[�N�����蓖�Ă�

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //�������W
    pspr->X = 320;
    pspr->Y = 240;
}
void exec07_02(TCB* thisTCB)
{
//�ړ����x
#define MOVE_SPEED 2.0f

    SPRITE* pspr;
    float   direction;

    pspr = (SPRITE*)thisTCB->Work;//���[�N�����蓖�Ă�

    switch( g_InputBuff & 0x0f )  //�ړ��L�[�݂̂𒊏o
    {//�ړ��������Z�o����
        case 0x01: direction= atan2( -1.0,  0.0); break; //��
        case 0x02: direction= atan2(  1.0,  0.0); break; //��
        case 0x04: direction= atan2(  0.0, -1.0); break; //��
        case 0x08: direction= atan2(  0.0,  1.0); break; //�E

        case 0x05: direction= atan2( -1.0, -1.0); break; //����
        case 0x06: direction= atan2(  1.0, -1.0); break; //����
        case 0x09: direction= atan2( -1.0,  1.0); break; //�E��
        case 0x0A: direction= atan2(  1.0,  1.0); break; //�E��

        default:  SpriteDraw(pspr,0); return;
    }
    //�ړ���������ړ��l���擾�����ۂ̈ړ��������s�Ȃ�
    pspr->X += cos(direction)*MOVE_SPEED;
    pspr->Y += sin(direction)*MOVE_SPEED;

    SpriteDraw(pspr,0);

}


////////////////////////////////////////////////////
//�V�́|�R ��ʓ��ł̈ړ�
////////////////////////////////////////////////////
void init07_03(TCB* thisTCB)
{
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;//���[�N�����蓖�Ă�

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //�������W
    pspr->X = 320;
    pspr->Y = 240;
}
void exec07_03(TCB* thisTCB)
{
//�ړ����x
#define MOVE_SPEED 8.0f

    SPRITE* pspr;
    float   direction;
    RECT size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;

    pspr = (SPRITE*)thisTCB->Work;//���[�N�����蓖�Ă�

    switch( g_InputBuff & 0x0f )  //�ړ��L�[�݂̂𒊏o
    {
        case 0x01: direction= atan2( -1.0,  0.0); break; //��
        case 0x02: direction= atan2(  1.0,  0.0); break; //��
        case 0x04: direction= atan2(  0.0, -1.0); break; //��
        case 0x08: direction= atan2(  0.0,  1.0); break; //�E

        case 0x05: direction= atan2( -1.0, -1.0); break; //����
        case 0x06: direction= atan2(  1.0, -1.0); break; //����
        case 0x09: direction= atan2( -1.0,  1.0); break; //�E��
        case 0x0A: direction= atan2(  1.0,  1.0); break; //�E��

        default:  SpriteDraw(pspr,0); return;
    }
    //�ړ�����
    pspr->X += cos(direction)*MOVE_SPEED;
    pspr->Y += sin(direction)*MOVE_SPEED;

    //��ʊO����`�F�b�N�ׁ̈A�L�����̑傫���̋�`���v�Z��
    //��U�\���̂Ɋi�[����
    hit_rect.top    =   pspr->Y + size_rect.top;
    hit_rect.bottom =   pspr->Y + size_rect.bottom;
    hit_rect.left   =   pspr->X + size_rect.left;
    hit_rect.right  =   pspr->X + size_rect.right;

    //�㉺���E�A���ꂼ��̕����ɑ΂��ĉ�ʊO���ǂ������`�F�b�N��
    //����ȏ�ړ����Ȃ��悤�ɂ���
    if(hit_rect.left   < 0)
    {//��
        pspr->X = 0;
    }
    if(hit_rect.top    < 0)
    {//��
        pspr->Y = 0;
    }

    if(hit_rect.right  > SCREEN_WIDTH)
    {//�E
        pspr->X = SCREEN_WIDTH - size_rect.right;
    }
    if(hit_rect.bottom > SCREEN_HEIGHT)
    {//��
        pspr->Y = SCREEN_HEIGHT - size_rect.bottom;
    }

    SpriteDraw(pspr,0);
}




////////////////////////////////////////////////////
//�V�́|�S  ���������v���C
////////////////////////////////////////////////////
typedef struct{
    SPRITE         MyShip;
    int            PID;
} EX07_04_STRUCT;

#define MOVE_SPEED    4.0
#define PLAYER_1      0
#define PLAYER_2      1
#define PLAYER_3      2
#define PLAYER_4      3


unsigned char EX07_04_player_input( int PlayerID )
{    unsigned char input_key;

    //���͒l���v���C���[�ɂ���ĕύX����
    switch( PlayerID )
    {//�L�[�{�[�h���v���C���[�P�A�W���C�X�e�B�b�N���v���C���[�Q�Ƃ���
       case PLAYER_1: input_key = g_KeyInputBuff; break;
       case PLAYER_2: input_key = g_JoystickBuff; break;
    }

    return input_key;
}

void exec07_04_myship(TCB* thisTCB)
{
    EX07_04_STRUCT* work = (EX07_04_STRUCT*)thisTCB->Work;
    unsigned char input_buff;

    //�v���C���[ID�ɂ�����
    input_buff = EX07_04_player_input( work->PID );

    //�L�[���͂ɂ��ړ�
    if( input_buff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( input_buff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( input_buff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( input_buff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    SpriteDraw( &work->MyShip,0);
}

void init07_04(TCB* thisTCB)
{
    TCB*    myship_tcb;
    EX07_04_STRUCT* myship_work;
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //���@���Q�@�쐬����
    for( loop = 0;loop < 2; loop++ )
    {
        myship_tcb = TaskMake( exec07_04_myship, 0x2000 );
        myship_work = (EX07_04_STRUCT*)myship_tcb->Work;
        //�v���C���[�ɂ�菉���\�����W��ς���
        myship_work->MyShip.X = 
            ((SCREEN_WIDTH  / 2) - 64) + loop * 128;
        myship_work->MyShip.Y = 
            SCREEN_HEIGHT / 2;
        //�v���C���[ID��o�^
        myship_work->PID = loop;
    }
}

void exec07_04(TCB* thisTCB)
{
    //�������������̂��߁A�T���v���ł̓��C�������͉������Ȃ�
}




////////////////////////////////////////////////////
//�V�́|�T  �L�����N�^�̋@�����Ǘ�����
////////////////////////////////////////////////////

#define DEFAULT_SHIP   3
#define BONUS_SCORE    1000

typedef struct{
    int            Score;
    int            MyShipCount;
    int            NextScore;     //���Ɏc�@��������X�R�A
    int            GameOver;
} EX07_05_STRUCT;


void init07_05(TCB* thisTCB)
{
    EX07_05_STRUCT* work = (EX07_05_STRUCT*)thisTCB->Work;

    //�c�@���ƁA�{�[�i�X�X�R�A�̏�����
    work->MyShipCount = DEFAULT_SHIP;
    work->NextScore = BONUS_SCORE;
}

void exec07_05(TCB* thisTCB)
{
    EX07_05_STRUCT* work = (EX07_05_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�y�L�[�ŃX�R�A�����Z
    if( g_DownInputBuff & KEY_Z ) work->Score += 100;
    //�w�L�[�Ŏc�@���P�@���炷
    if( g_DownInputBuff & KEY_X )
    {
        work->MyShipCount--;
        if( work->MyShipCount < 0 )
        {//�Q�[���I�[�o�[����
             work->GameOver = true;
        }
    }

    //�X�R�A�ɂ��c�@��������
    if( work->Score >= work->NextScore )
    {
        work->MyShipCount++;
        work->NextScore += BONUS_SCORE;
    }

    //�X�R�A�ƁA�@���̕\��
    sprintf( str,"SCORE: %8d  �c��@��:%2d\n���̃{�[�i�X�X�R�A:%8d",work->Score,work->MyShipCount,work->NextScore);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    if( work->GameOver )
    {//�Q�[���I�[�o�[���Ƀ��b�Z�[�W�\��
        font_pos.top += 48;
        g_pFont->DrawText( NULL, "�f�`�l�d�@�n�u�d�q�I�I", -1, &font_pos, DT_LEFT, 0xffffffff);
    }
}



////////////////////////////////////////////////////
//�V�́|�U ���j���[�̕\��
////////////////////////////////////////////////////
#define MENU_INDEX  5
//���j���[���b�Z�[�W�̍���
#define MENU_HEIGHT  16

typedef struct{
    int (*MenuFunc)(int);            //���j���[���s�֐�
    char*          MenuName;         //���j���[��
    char*          HelpMess;         //���j���[�I�����ɕ\������w���v���b�Z�[�W
} EX07_06_MENU_DATA;


typedef struct{
    SPRITE         SelectMark;
    int            SelectMenu;
    char           DispMess[64];
    int            Num;
    int            MenuX;
    int            MenuY;


} EX07_06_STRUCT;


int EX07_06_add( int num )
{
    return num + 100;
}
int EX07_06_sub( int num )
{
    return num - 100;
}
int EX07_06_mult( int num )
{
    return num * 2;
}
int EX07_06_div( int num )
{
    return num / 2;
}


void init07_06(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );
}

void exec07_06(TCB* thisTCB)
{
    EX07_06_STRUCT* work = (EX07_06_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];

    EX07_06_MENU_DATA menu_data[] =
{
    {//���j���[�P
      EX07_06_add,
      "���j���[�P�F���Z",
      "�T���v���p���j���[�P�ł��B�l��100���Z���܂��B",
    },{//���j���[�Q
      EX07_06_sub,
      "���j���|�Q�F���Z",
      "�T���v���p���j���[�Q�ł��B100���Z���܂��B",
    },{//���j���[�R
      EX07_06_mult,
      "���j���[�R:��Z",
      "�R�ڂ̃��j���[�ł��B�l���Q�{���܂��B",
    },{//���j���[�S
      EX07_06_div,
      "���j���[�S:���Z",
      "�S�ڂ̑I�����j���[�B�l�𔼕��ɂ��܂��B",
    },{//���j���[�T
      NULL,
      "�I��",
      "�֐��|�C���^��NULL�ɂ��āA�I�������ɂ��܂��B�i�I�������͎������Ă��܂���j",
    },
};


    if( g_InputBuff & KEY_X  )
    {//�w�L�[�������Ȃ��炾�ƁA���j���[�̈ړ�
        if( g_InputBuff & KEY_UP    ) work->MenuY -= 4;
        if( g_InputBuff & KEY_DOWN  ) work->MenuY += 4;
        if( g_InputBuff & KEY_RIGHT ) work->MenuX += 4;
        if( g_InputBuff & KEY_LEFT  ) work->MenuX -= 4;
    } else {
      //�����Ă��Ȃ����̓��j���[�̑I��
        if( g_DownInputBuff & KEY_DOWN )
        {
            work->SelectMenu++;
            //���j���[���`�F�b�N
            if( work->SelectMenu >= MENU_INDEX )work->SelectMenu = 0;
        }
        if( g_DownInputBuff & KEY_UP  )
        {
            work->SelectMenu--;
            if( work->SelectMenu < 0 ) work->SelectMenu = MENU_INDEX-1;
        }

        if( g_DownInputBuff & KEY_Z  )
        {//�y�L�[�őI�𒆂̃��j���[�̎��s
            if( menu_data[ work->SelectMenu ].MenuFunc == NULL )
            {//�������A���s����֐��������ꍇ�͏I��
                //���j���[�I������
            }else{
                //���j���[���e�̎��s
                work->Num = menu_data[ work->SelectMenu ].MenuFunc( work->Num );
            }
        }
    }

    //�I���}�[�N�̈ʒu������
    work->SelectMark.X = work->MenuX;
    work->SelectMark.Y = work->MenuY + work->SelectMenu * MENU_HEIGHT;
    //�I���}�[�N�̕\��
    SpriteDraw(&work->SelectMark,0);

    //���j���[�̕\��
    font_pos.top  = work->MenuY;
    font_pos.left = work->MenuX + MENU_HEIGHT;
    for(loop = 0; loop < MENU_INDEX; loop++)
    {
        g_pFont->DrawText( NULL, menu_data[ loop ].MenuName, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += MENU_HEIGHT;
    }

    //���i�̌Œ�ʒu�Ƀ��j���[�ɂ�鐔�l�ω��ƃ��j���[�ڍד��e�̕\��
    font_pos.top  = 448;
    font_pos.left = 32;
    sprintf( str,"���݂̐��l�F %d\n%s",work->Num, menu_data[ work->SelectMenu ].HelpMess);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�V�́|�V �i�s�����̌�����m��
////////////////////////////////////////////////////

void init07_07(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec07_07(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;
    float beforeX;
    float beforeY;
    float posX;
    float posY;
    float direction;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�ړ��O�̍��W��ۑ�
    beforeX = work->X;
    beforeY = work->Y;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    //�ړ��O�̍��W�ƌ��݂̍��W�Ƃ̑��΍��W���v�Z
    posX = work->X - beforeX;
    posY = work->Y - beforeY;

    //���΍��W����������v�Z
    if( posX == 0 && posY == 0 )
    {//���W���ړ����Ă��Ȃ��������͕�������܂�Ȃ����߁A���ʈ���
        direction = 0.0;
    }else{
        direction = atan2( posY , posX );
        direction = -direction / M_PI * 180.0;
    } 

    //�����̕\��
    sprintf( str,"����  %f",direction);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw( work, 0);
}





////////////////////////////////////////////////////
//�V�́|�W �ڕW��_���Ēe�𔭎˂���
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
    int            Time;
    float          AddX;                 //�����l�w
    float          AddY;                 //�����l�x
} EX07_08_STRUCT ;

void exec07_08_bullet(TCB* thisTCB)
{
#define MOVE_SPEED  24.0
    EX07_08_STRUCT* work = (EX07_08_STRUCT*)thisTCB->Work;
    //���e�̍��W
    float my_X;
    float my_Y;
    //�ڕW�̍��W
    float targetX;
    float targetY;
    //�ڕW�̕���
    float direction;

    //�����������A��莞�Ԃ��Ƃɏ����������
    if( work->Time++ == 30 )
    {//��莞�Ԃ��Ƃɍ��W�Ƒ����l���v�Z
        work->sprt.X = SCREEN_WIDTH  / 2;
        work->sprt.Y = SCREEN_HEIGHT / 2;
        work->Time = 0;

        //�e�̍��W���擾
        my_X    = work->sprt.X;
        my_Y    = work->sprt.Y;
        //�ڕW�̎��@���W���擾
        targetX = work->Target->X + 16;
        targetY = work->Target->Y + 16;

        //���W���瑊��ւ̕������v�Z
        direction = atan2( targetY - my_Y, targetX - my_X );
        //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
        work->AddX = cos( direction ) * MOVE_SPEED;
        work->AddY = sin( direction ) * MOVE_SPEED;
    }

    //���W�ɑ����l�����Z
    work->sprt.X += work->AddX;
    work->sprt.Y += work->AddY;

    SpriteDraw(&work->sprt,1);
}

void init07_08(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* sprt;
    EX07_08_STRUCT* tmp_work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���@�̃X�v���C�g
    sprt = (SPRITE*)thisTCB->Work;
    sprt->X = SCREEN_WIDTH  / 2;
    sprt->Y = SCREEN_HEIGHT / 2 + 120;

    //�������ƁA�ڕW�X�v���C�g��ݒ�
    tmp_tcb = TaskMake( exec07_08_bullet, 0x2000 );
    tmp_work = (EX07_08_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = sprt;

}

void exec07_08(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}


////////////////////////////////////////////////////
//�V�́|�X �e���ʂɔ��˂���
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    int            Time;
    float          Direction;            //�e�̕���
    float          AddX;                 //�����l�w
    float          AddY;                 //�����l�x
} EX07_09_STRUCT ;

void exec07_09_bullet(TCB* thisTCB)
{
#define MOVE_SPEED  16.0
    EX07_09_STRUCT* work = (EX07_09_STRUCT*)thisTCB->Work;

    if( work->Time == 0 )
    {   //���W�̏�����
        work->sprt.X = SCREEN_WIDTH  / 2;
        work->sprt.Y = SCREEN_HEIGHT / 2;

        //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
        work->AddX = cos( work->Direction ) * MOVE_SPEED;
        work->AddY = sin( work->Direction ) * MOVE_SPEED;
    }
    //��莞�Ԃ��������������
    if( work->Time == 30 )
    {//�e������
        TaskKill( thisTCB );
    } else {
     //�e�̐i�s����

        work->Time++;
        //���W�ɑ����l�����Z
        work->sprt.X += work->AddX;
        work->sprt.Y += work->AddY;

        SpriteDraw(&work->sprt,1);
    }
}

void init07_09(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec07_09(TCB* thisTCB)
{
#define BULLET_CYCLE 15
#define BULLET_COUNT 16
    int* work_time = (int*)thisTCB->Work;
    EX07_09_STRUCT* tmp_work;
    TCB*    tmp_tcb;
    int     loop;
    float   bullet_direction = 0;

    //��莞�Ԃ��Ƃɒe�𔭎�
    if( *work_time == BULLET_CYCLE )
    {
        //�e�𔭎�
        for( loop = 0;loop < BULLET_COUNT; loop++ )
        {
            tmp_tcb = TaskMake( exec07_09_bullet, 0x2000 );
            tmp_work = (EX07_09_STRUCT*)tmp_tcb->Work;
            tmp_work->Direction = bullet_direction;

            //���˕�����e�̔��ː��ɍ��킹�ď����Â��炵�Ă���
            bullet_direction += M_PI * 2.0  / BULLET_COUNT;
        }
        *work_time = 0;
    }

    *work_time += 1;
}






////////////////////////////////////////////////////
//�V�́|�P�O ���@�̒e������
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Bullet;
    SPRITE         MyShip;
    int            BulletDraw;
} EX07_10_STRUCT ;

void init07_10(TCB* thisTCB)
{
    EX07_10_STRUCT* work = (EX07_10_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;

}

void exec07_10(TCB* thisTCB)
{
#define MOVE_SPEED    4.0
#define BULLET_SPEED  10.0
    EX07_10_STRUCT* work = (EX07_10_STRUCT*)thisTCB->Work;

    //�L�[���͂ɂ�鎩�@�̈ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    if( !work->BulletDraw )
    {//���˃L�[���͑҂�
      if( g_DownInputBuff & KEY_Z )
      {//�e�̔��ˏ���
          //���@�̍��W���R�s�[���āA��������e�𔭎˂���
          work->Bullet.X = work->MyShip.X + 16;
          work->Bullet.Y = work->MyShip.Y;

          work->BulletDraw = true;
      }
    }else{
       //�e�̈ړ������ƕ\��
        work->Bullet.Y -= BULLET_SPEED;
        SpriteDraw( &work->Bullet,1);

        //�e����ʊO�Ɉړ�������A�L�[���͑҂��ɖ߂�
        if( work->Bullet.Y < 0 ) work->BulletDraw = false;
    }

    SpriteDraw( &work->MyShip, 0);
}





////////////////////////////////////////////////////
//�V�́|�P�P  �����̒e��A�����Ĕ��˂���
////////////////////////////////////////////////////
typedef struct{
    SPRITE         MyShip;
} EX07_11_STRUCT ;

#define MOVE_SPEED    4.0
#define BULLET_SPEED  10.0

void exec07_11_bullet(TCB* thisTCB)
{
    SPRITE* work = (SPRITE*)thisTCB->Work;

     //�e����ʊO�Ɉړ�������A�����̏I��
    if( work->Y < 0 )
    {
        TaskKill(thisTCB);
        return;
    }

    //�e�̈ړ������ƕ\��
    work->Y -= BULLET_SPEED;
    SpriteDraw( work,1);
}

void init07_11(TCB* thisTCB)
{
    EX07_11_STRUCT* work = (EX07_11_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
}

void exec07_11(TCB* thisTCB)
{
    EX07_11_STRUCT* work = (EX07_11_STRUCT*)thisTCB->Work;
    TCB*    bullet_tcb;
    SPRITE* bullet_work;

    //�L�[���͂ɂ�鎩�@�̈ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    if( g_DownInputBuff & KEY_Z )
    {//�e�̔��ˏ���
        //�^�X�N���g�p���Ēe���쐬����
        bullet_tcb = TaskMake( exec07_11_bullet, 0x2000 );
        bullet_work = (SPRITE*)bullet_tcb->Work;

        //���@�̍��W���R�s�[���āA��������e���ړ�������
        bullet_work->X = work->MyShip.X + 16;
        bullet_work->Y = work->MyShip.Y;
    }

    SpriteDraw( &work->MyShip, 0);
}



////////////////////////////////////////////////////
//�V�́|�P�Q ���˂ɑ΂��Ĕ�����t����
////////////////////////////////////////////////////
typedef struct{
    SPRITE         MyShip;
    int            LockTime;
} EX07_12_STRUCT ;

#define MOVE_SPEED    4.0
#define BULLET_SPEED  10.0
#define LOCK_TIME     4

void exec07_12_bullet(TCB* thisTCB)
{
    SPRITE* work = (SPRITE*)thisTCB->Work;

     //�e����ʊO�Ɉړ�������A�����̏I��
    if( work->Y < 0 )
    {
        TaskKill(thisTCB);
        return;
    }

    //�e�̈ړ������ƕ\��
    work->X += 0;
    work->Y += -BULLET_SPEED;
    SpriteDraw( work,1);
}

void init07_12(TCB* thisTCB)
{
    EX07_12_STRUCT* work = (EX07_12_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
}

void exec07_12(TCB* thisTCB)
{
    EX07_12_STRUCT* work = (EX07_12_STRUCT*)thisTCB->Work;
    TCB*    bullet_tcb;
    SPRITE* bullet_work;

    if( !work->LockTime )
    {//����\
        //�L�[���͂ɂ�鎩�@�̈ړ�
        if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
        if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
        if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
        if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;
    }else{
     //����s�\����
        work->LockTime--;
        //�e�̔��˕����Ƃ͔��Ε����ɐi��
        work->MyShip.X += -0.0;
        work->MyShip.Y += BULLET_SPEED / 4;
    }

    if( g_DownInputBuff & KEY_Z )
    {//�e�̔��ˏ���
        //�^�X�N���g�p���Ēe���쐬����
        bullet_tcb = TaskMake( exec07_12_bullet, 0x2000 );
        bullet_work = (SPRITE*)bullet_tcb->Work;

        //���@�̍��W���R�s�[���āA��������e���ړ�������
        bullet_work->X = work->MyShip.X + 16;
        bullet_work->Y = work->MyShip.Y;
        //����s�\���Ԃ�ݒ�
        work->LockTime = LOCK_TIME;
    }

    SpriteDraw( &work->MyShip, 0);
}



////////////////////////////////////////////////////
//�V�́|�P�R ���[�U�[�̓���
////////////////////////////////////////////////////
#define LASER_LENGTH 10
#define LASER_SPEED  16
typedef struct{
    SPRITE           MyShip;                     //���@
    int              LaserCount;                 //���˂������[�U�[�̐�
    int              DeleteLaserCount;           //�����������[�U�[�̐�
    int              LaserMode;                  //���[�U�[�̔��˃��[�h
    int              LaserDraw;                  //���[�U�[����ʓ��Ɏc���Ă��邩�̃t���O
} EX07_13_STRUCT;

typedef struct{
    EX07_13_STRUCT*  MyShipWork;     //���@���ւ̃|�C���^
    float            PosY;           //���@����̑��΍��W
    SPRITE           Laser;          //���[�U�[�\���p�X�v���C�g
} EX07_13_LASER;

void exec07_13_laser(TCB* thisTCB)
{
    EX07_13_LASER* work = (EX07_13_LASER*)thisTCB->Work;

    //���[�U�[��i�߂�
    work->PosY -= LASER_SPEED;
    //���[�U�[�̍��W�����@�̍��W�ɒǏ]������
    work->Laser.X = work->MyShipWork->MyShip.X + 24;
    work->Laser.Y = work->PosY + work->MyShipWork->MyShip.Y;

    if( work->Laser.Y > -16 )
    {//���[�U�[�̕`��
        SpriteDraw( &work->Laser, 1);
    }else{
    //���[�U�[����ʊO�Ȃ�\������Ă���������ꂽ���[�U�[�̐����J�E���g������
    //�������̍ہA�쐬�������[�U�[�̐��Ə����������[�U�[�̐��������Ȃ��ʂ��烌�[�U�[�͏����Ă���B
        work->MyShipWork->DeleteLaserCount++;
        if( work->MyShipWork->LaserCount == work->MyShipWork->DeleteLaserCount) work->MyShipWork->LaserDraw = FALSE;
        TaskKill( thisTCB );
    }
}

void init07_13(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\LASER.png",&g_pTex[1] );
}

void exec07_13(TCB* thisTCB)
{
#define MOVE_SPEED  4.0
    EX07_13_STRUCT* work = (EX07_13_STRUCT*)thisTCB->Work;
    TCB*  tmp_tcb;
    EX07_13_LASER*  tmp_work;


    //�L�[���͂ɂ�鎩�@�̈ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //���˃L�[�������ꂽ�烌�[�U�[���˃��[�h�ɐ؂�ւ���
    if( g_DownInputBuff & KEY_Z )
    {//�A���A���łɔ��˂��ꂽ���[�U�[����ʓ��Ɏc���Ă����牽�����Ȃ��B
      if( !work->LaserDraw )
      {//���[�U�[���ˊ֘A�̏�����
        work->LaserMode = TRUE;
        work->LaserCount = 0;
        work->DeleteLaserCount = 0;
      }
    }

    //���[�U�[���˃��[�h�ł���΃��[�U�[�𔭎˂�������
    if( work->LaserMode )
    {//�A���A���[�U�[�̒��������ʂ𒴂��Ă��邩�A�{�^���������ꂽ�ꍇ�A���[�U�[���[�h�͏I��
        if( work->LaserCount > LASER_LENGTH || g_UpInputBuff & KEY_Z )
        {
            work->LaserMode = FALSE;
        } else {
            //���[�U�[�\�������̍쐬
            work->LaserCount++;
            tmp_tcb = TaskMake( exec07_13_laser, 0x2000 );
            tmp_work = (EX07_13_LASER*)tmp_tcb->Work;
            tmp_work->MyShipWork = work;
            //���[�U�[�\����
            work->LaserDraw = TRUE;
        }
    }

    SpriteDraw( &work->MyShip, 0);

}



////////////////////////////////////////////////////
//�V�́|�P�S �������֒e�𔭎˂���
////////////////////////////////////////////////////
typedef struct{
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
    int            Time;
} EX07_14_STRUCT;

typedef struct{
    SPRITE         Sprt;
    int            Time;
    float          AddX;                 //�����l�w
    float          AddY;                 //�����l�x
} EX07_14_BULLET;

void exec07_14_bullet(TCB* thisTCB)
{
    EX07_14_BULLET* work = (EX07_14_BULLET*)thisTCB->Work;

    //��莞�Ԃ��������������
    if( work->Time == 30 )
    {//�e������
        TaskKill( thisTCB );
    } else {
     //�e�̐i�s����

        work->Time++;
        //���W�ɑ����l�����Z
        work->Sprt.X += work->AddX;
        work->Sprt.Y += work->AddY;

        SpriteDraw(&work->Sprt,1);
    }
}

void exec07_14_shoot(TCB* thisTCB)
{
#define BULLET_CYCLE 15
#define BULLET_COUNT 5
#define BULLET_ANGLE M_PI / 16    //�e�ƒe�̊Ԃ̊p�x
#define MOVE_SPEED  16.0

    EX07_14_STRUCT* work = (EX07_14_STRUCT*)thisTCB->Work;
    EX07_14_BULLET* tmp_work;
    TCB*    tmp_tcb;
    int     loop;
    //�e�̔��ˍ��W
    float shootX;
    float shootY;
    //�ڕW�̍��W
    float targetX;
    float targetY;
    //�e�𔭎˂������
    float bullet_direction;

    //��莞�Ԃ��Ƃɒe�𔭎�
    if( work->Time == BULLET_CYCLE )
    {
        //��ʒ�������e�𔭎�
        shootX = SCREEN_WIDTH  / 2;
        shootY = SCREEN_HEIGHT / 2;
        //�ڕW�̍��W���擾
        targetX = work->Target->X + 16;
        targetY = work->Target->Y + 16;

        //���W����e�̔��˕������v�Z
        bullet_direction = atan2( targetY - shootY, targetX - shootX );
        //�ŏ��Ɉ�C�ɕ����������Ă������̏ꍇ�͂T�v�`�x�Ȃ̂łQ�����̊p�x������
        bullet_direction -= BULLET_ANGLE * (BULLET_COUNT / 2) ;
        //�e�𔭎�
        for( loop = 0;loop < BULLET_COUNT; loop++ )
        {
            tmp_tcb = TaskMake( exec07_14_bullet, 0x2000 );
            tmp_work = (EX07_14_BULLET*)tmp_tcb->Work;

            //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
            tmp_work->AddX = cos( bullet_direction ) * MOVE_SPEED;
            tmp_work->AddY = sin( bullet_direction ) * MOVE_SPEED;
            //���˕����������Â��炵�Ă���
            bullet_direction += BULLET_ANGLE;
            //�e�̏������W
            tmp_work->Sprt.X = shootX;
            tmp_work->Sprt.Y = shootY;
        }
        work->Time = 0;
    }
    work->Time += 1;
}

void init07_14(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* sprt;
    EX07_14_STRUCT* tmp_work;
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���@�̃X�v���C�g
    sprt = (SPRITE*)thisTCB->Work;
    sprt->X = SCREEN_WIDTH  / 2;
    sprt->Y = SCREEN_HEIGHT / 2 + 120;

    //�e���˂��Ǘ����鏈�����쐬
    tmp_tcb = TaskMake( exec07_14_shoot, 0x2000 );
    tmp_work = (EX07_14_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = sprt;

}

void exec07_14(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}



////////////////////////////////////////////////////
//�V�́|�P�U ���v���C�̕��@
////////////////////////////////////////////////////
#define REPLAY_COUNT 50*5
typedef struct{
    SPRITE         Sprt;
    SPRITE*        Target;               //�ڕW�̃X�v���C�g
    int            Time;
    int            Time2;
    float          AddX;                 //�����l�w
    float          AddY;                 //�����l�x
} EX07_16_BALL;

typedef struct{
    SPRITE         Sprt;
    int            Time;
} EX07_16_MYSHIP;

typedef struct{
    unsigned char  ReplayBuff[ REPLAY_COUNT ];
    int            Time;
    int            PlayMode;
} EX07_16_STRUCT;

#define MODE_WAIT    0
#define MODE_PLAY    1
#define MODE_REPLAY  2

unsigned char g_EX07_16_InputBuff;


void exec07_16_ball(TCB* thisTCB)
{
#define MOVE_SPEED  20.0
    EX07_16_BALL* work = (EX07_16_BALL*)thisTCB->Work;
    //���e�̍��W
    float my_X;
    float my_Y;
    //�ڕW�̍��W
    float targetX;
    float targetY;
    //�ڕW�̕���
    float direction;

    if( work->Time2++ > REPLAY_COUNT )
    {//��莞�ԏ������s�Ȃ�����I��
        TaskKill( thisTCB );
    }else{
        //�����������A��莞�Ԃ��Ƃɏ����������
        if( work->Time++ == 30 )
        {//��莞�Ԃ��Ƃɍ��W�Ƒ����l���v�Z
            work->Sprt.X = SCREEN_WIDTH  / 2;
            work->Sprt.Y = SCREEN_HEIGHT / 2;
            work->Time = 0;

            //�e�̍��W���擾
            my_X    = work->Sprt.X;
            my_Y    = work->Sprt.Y;
            //�ڕW�̎��@���W���擾
            targetX = work->Target->X + 16;
            targetY = work->Target->Y + 16;

            //���W���瑊��ւ̕������v�Z
            direction = atan2( targetY - my_Y, targetX - my_X );
            //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
            work->AddX = cos( direction ) * MOVE_SPEED;
            work->AddY = sin( direction ) * MOVE_SPEED;
        }

        //���W�ɑ����l�����Z
        work->Sprt.X += work->AddX;
        work->Sprt.Y += work->AddY;

        SpriteDraw(&work->Sprt,1);
    }
}

void exec07_16_MyShip(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    EX07_16_MYSHIP* work = (EX07_16_MYSHIP*)thisTCB->Work;

    if( work->Time++ > REPLAY_COUNT )
    {//��莞�ԏ������s�Ȃ�����I��
        TaskKill( thisTCB );
    }else{
        //���v���C�p�Ƀo�b�t�@�ɑΉ������L�[���͂ɂ��ړ�
        if( g_EX07_16_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
        SpriteDraw( &work->Sprt, 0);
    }
}

void init07_16(TCB* thisTCB)
{
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec07_16(TCB* thisTCB)
{
    EX07_16_STRUCT* work = (EX07_16_STRUCT*)thisTCB->Work;
    TCB*    myship_tcb;
    TCB*    ball_tcb;
    EX07_16_MYSHIP* myship_work;
    EX07_16_BALL* ball_work;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];


    if( work->PlayMode == MODE_WAIT )
    {//���͑҂����
        //�y�L�[�Ńv���C���[�h�Ɉڍs
        if( g_DownInputBuff & KEY_Z    ) work->PlayMode = MODE_PLAY;
        //�w�L�[�Ń��v���C���[�h�Ɉڍs
        if( g_DownInputBuff & KEY_X    ) work->PlayMode = MODE_REPLAY;

        //�{�^���̓��͂�����Ίe�������쐬
        if( g_DownInputBuff & (KEY_Z | KEY_X) )
        {
            //���@�̃X�v���C�g
            myship_tcb = TaskMake( exec07_16_MyShip, 0x2000 );
            myship_work = (EX07_16_MYSHIP*)myship_tcb->Work;
            myship_work->Sprt.X = SCREEN_WIDTH  / 2;
            myship_work->Sprt.Y = SCREEN_HEIGHT / 2 + 120;

            //�{�[���̏������ƁA�ڕW�X�v���C�g��ݒ�
            ball_tcb = TaskMake( exec07_16_ball, 0x3000 );
            ball_work = (EX07_16_BALL*)ball_tcb->Work;
            ball_work->Target = &myship_work->Sprt;

        }
        //�L�^�E�Đ��p�̃^�C�}�[��������
        work->Time = 0;
        g_pFont->DrawText( NULL, "�y�L�[�Œʏ�v���C�A�w�L�[�Ń��v���C", -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    if( work->PlayMode == MODE_PLAY )
    {//�ʏ�v���C�A�L�^����
        if( work->Time > REPLAY_COUNT )
        {//���Ԃ������烂�[�h��؂�ւ��L�^�I��
            work->PlayMode = MODE_WAIT;
        }else{
         //�L�[���͂��L�^
            g_EX07_16_InputBuff = g_InputBuff;
            work->ReplayBuff[ work->Time ] = g_EX07_16_InputBuff;
        }

        sprintf( str,"�ʏ�v���C�A�L�[���͋L�^���c %d",REPLAY_COUNT - work->Time);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

        //���Ԃ��X�V
        work->Time++;
    }

    if( work->PlayMode == MODE_REPLAY )
    {//���v���C�A�Đ�����
        if( work->Time > REPLAY_COUNT )
        {//���Ԃ������烂�[�h��؂�ւ��Đ��I��
            work->PlayMode = MODE_WAIT;
        }else{
         //�L�^�����L�[���͂��Đ�
            g_EX07_16_InputBuff = work->ReplayBuff[ work->Time ];
        }

        sprintf( str,"���v���C���c %d",REPLAY_COUNT - work->Time);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

        //���Ԃ��X�V
        work->Time++;
    }
}



////////////////////////////////////////////////////
//�V�́|�P�V  ���x���A�b�v������ɂ�
////////////////////////////////////////////////////
#define LV_MAX 10
#define EXP_RANGE 50
typedef struct{
    int            NowEXP;        //���݂̌o���l
    int            NowLV;         //���݂̃��x��
    int            NextEXP;       //���Ƀ��x���A�b�v����o���l
} EX07_17_STRUCT;

static int LevelUP_EXP[] = 
{//���x���A�b�v�ɕK�v�Ȍo���l�̃e�[�u��
    100,  //01
    100,  //02
    150,  //03
    200,  //04
    300,  //05
    450,  //06
    550,  //07
    600,  //08
    700,  //09
    800,  //10
    -1,   //MAX
};


void init07_17(TCB* thisTCB)
{
    EX07_17_STRUCT* work = (EX07_17_STRUCT*)thisTCB->Work;

    //���x���̏�����
    work->NowEXP = 0;
    work->NowLV  = 1;
    //�ŏ��̃��x���A�b�v�ɕK�v�Ȍo���l���擾�A�v�Z
    work->NextEXP += LevelUP_EXP[ work->NowLV ];
}

void exec07_17(TCB* thisTCB)
{
    EX07_17_STRUCT* work = (EX07_17_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //�y�L�[�Ōo���l�������_���ɉ��Z
    if( g_DownInputBuff & KEY_Z )
    {//���x�����ő�ɒB���Ă�����A�o���l���擾���Ȃ��悤�ɂ���
        if( work->NowLV < LV_MAX ) work->NowEXP += rand() % EXP_RANGE;
    }
    //���x���ɑΉ������o���l���z�����烌�x���A�b�v
    if( work->NowEXP >= work->NextEXP )
    {//���x���A�b�v����
        //���̃��x���A�b�v�ɕK�v�Ȍo���l���擾�A�v�Z
        work->NextEXP += LevelUP_EXP[ work->NowLV ];
        //���x���A�b�v����
        work->NowLV++;

        //���x�����ő厞�̏���
        if( work->NowLV >= LV_MAX )
        {
            work->NowEXP  = 0;
            work->NextEXP = 9999;
        }
    }
    //���x���ƌo���l�̕\��
    sprintf( str,"LEVEL:%2d\n���݂̌o���l�@�@�@:%8d\n��LV�܂ł̌o���l:%8d",
             work->NowLV,work->NowEXP,work->NextEXP - work->NowEXP);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}

////////////////////////////////////////////////////
//�V�́|�P�W  �X�R�A�����L���O�̊Ǘ�
////////////////////////////////////////////////////
#define RANK_COUNT 5

typedef struct{
    int            Score;
} EX07_18_STRUCT;

typedef struct{
    int            Score;
    char           Name[4];
} EX07_18_SCORE_STRUCT;

static EX07_18_SCORE_STRUCT gEX07_18_ScoreData[ RANK_COUNT ] = 
{//�����X�R�A�f�[�^
    { 10000,{'A','B','C','\0',},},
    {  5000,{'D','E','F','\0',},},
    {  3000,{'G','H','I','\0',},},
    {  1000,{'J','K','L','\0',},},
    {   500,{'M','N','O','\0',},},
};


int EX07_18_rank_check( int Score)
{
    int loop;

    //�X�R�A�o�^�A�X�R�A�̍������Ƀ`�F�b�N���Ă���
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        //���������N�����������X�R�A�ł���΁A�����N�C��
        if(Score > gEX07_18_ScoreData[ loop ].Score )break;
    }

    //�����N�C�����`�F�b�N
    if(loop != RANK_COUNT ) return loop;
    //�����N�O
    return -1;
}


void EX07_18_score_regist( int Score, char* Name )
{
    int loop;
    int set_rank;

    //���ʂɃ����N�C�����Ă��邩���擾
    set_rank = EX07_18_rank_check( Score );

    for( loop = RANK_COUNT-1; loop > set_rank; loop-- )
    {//���ʂ��烉���N���P�Â��炵�Ă���
       gEX07_18_ScoreData[ loop ] = gEX07_18_ScoreData[ loop -1 ];
    }

    //�X�R�A�Ɩ��O��o�^
    gEX07_18_ScoreData[ set_rank ].Score = Score;
    strcpy( gEX07_18_ScoreData[ set_rank ].Name, Name);
}


void exec07_18(TCB* thisTCB)
{
    EX07_18_STRUCT* work = (EX07_18_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //��L�[�ŃX�R�A�����Z
    if( g_InputBuff & KEY_UP ) work->Score += 100;
    //���L�[�ŃX�R�A�����Z
    if( g_InputBuff & KEY_DOWN )
    {
        work->Score -= 100;
        if( work->Score < 0 ) work->Score = 0;
    }
    //�y�L�[�ŃX�R�A�o�^����
    if( g_DownInputBuff & KEY_Z )
    {
        //�����N�`�F�b�N���s���A�����N�C���Ȃ�X�R�A�o�^����
        if( 0 <= EX07_18_rank_check( work->Score ) )
        {
            EX07_18_score_regist( work->Score , "NEW" );
        }
    }

    //�X�R�A�f�[�^�̕\��
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        sprintf( str,
            "RANK%d: %8d  %s",
            loop+1, 
            gEX07_18_ScoreData[ loop ].Score,
            gEX07_18_ScoreData[ loop ].Name);

        g_pFont->DrawText( NULL, 
            str, 
            -1, 
            &font_pos, 
            DT_LEFT, 
            0xffffffff);

        font_pos.top += 32;
    }

    //���݂̃X�R�A�̕\��
    font_pos.top += 32;
    sprintf( str,"�o�^����X�R�A: %8d ",work->Score);
    g_pFont->DrawText( NULL, 
        str, 
        -1, 
        &font_pos, 
        DT_LEFT, 
        0xffffffff);

    font_pos.top += 32;
    g_pFont->DrawText( NULL,
        "�y�L�[�ŃX�R�A�o�^\n�����L�[�ŃX�R�A�ύX", 
        -1, 
        &font_pos, 
        DT_LEFT, 
        0xffffffff);
}



////////////////////////////////////////////////////
//�W�� �����蔻��
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�W�́|�P ����Ƃ̋����𑪂�
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_01_STRUCT;

void init08_01(TCB* thisTCB)
{
    EX08_01_STRUCT* work = (EX08_01_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_01(TCB* thisTCB)
{
#define MOVE_SPEED  8.0

    EX08_01_STRUCT* work = (EX08_01_STRUCT*)thisTCB->Work;
    float distance;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //������W�̏ꍇ�G���[���o��̂œ��ʏ���
    if( (work->Target.X == work->MyShip.X )&&(work->Target.Y == work->MyShip.Y) )
    {
        distance = 0;
    }else{
        //����Ƃ̋������v��
        distance = sqrtf( (work->Target.X - work->MyShip.X) * (work->Target.X - work->MyShip.X) + 
                          (work->Target.Y - work->MyShip.Y) * (work->Target.Y - work->MyShip.Y) 
                        );
    }
    sprintf( str,"����  %f",distance);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}


////////////////////////////////////////////////////
//�W�́|�Q �~���m�̓����蔻��
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_02_STRUCT;

void init08_02(TCB* thisTCB)
{
    EX08_02_STRUCT* work = (EX08_02_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_02(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
#define HIT_SIZE1   32.0
#define HIT_SIZE2   64.0


    EX08_02_STRUCT* work = (EX08_02_STRUCT*)thisTCB->Work;
    float distance;
    float check_distance;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //�ڐG�̂��߂̔���l���v�Z
    distance = (work->Target.X - work->MyShip.X) * (work->Target.X - work->MyShip.X) + 
               (work->Target.Y - work->MyShip.Y) * (work->Target.Y - work->MyShip.Y) ;

    //����l�Ƃ��ċ����̓������߂邪�A�����v�Z�Ȃ̂ŏȗ�
    //check_distance = pow( sqrtf( ((HIT_SIZE1 - 0) * (HIT_SIZE1 - 0)) + ((HIT_SIZE2 - 0) * (HIT_SIZE2 - 0)) ) );
    check_distance = (HIT_SIZE1 * HIT_SIZE1) + (HIT_SIZE2 * HIT_SIZE2) ;
    //����l�ȉ��Ȃ�ڐG
    if( distance < check_distance )
    {
        sprintf( str,"�ڐG���܂���");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}




////////////////////////////////////////////////////
//�W�́|�R ��`�Ɠ_�̓����蔻��
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_03_STRUCT;

void init08_03(TCB* thisTCB)
{
    EX08_03_STRUCT* work = (EX08_03_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_03(TCB* thisTCB)
{
#define MOVE_SPEED  8.0

    EX08_03_STRUCT* work = (EX08_03_STRUCT*)thisTCB->Work;
    char str[128];
    RECT hit_size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //����`�F�b�N�̂��߂̋�`���v�Z
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //�Ώۂ���`�͈͓̔��ɂ���Γ������Ă���
    if( hit_rect.top    < work->Target.Y  && 
        hit_rect.bottom > work->Target.Y  &&
        hit_rect.left   < work->Target.X  &&
        hit_rect.right  > work->Target.X  )
    {
        sprintf( str,"�ڐG���܂���");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}


////////////////////////////////////////////////////
//�W�́|�S ��`���m�̓����蔻��
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_04_STRUCT;

void init08_04(TCB* thisTCB)
{
    EX08_04_STRUCT* work = (EX08_04_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_04(TCB* thisTCB)
{
#define MOVE_SPEED  8.0

    EX08_04_STRUCT* work = (EX08_04_STRUCT*)thisTCB->Work;
    char str[128];
    RECT hit_size_rectA = { 0, 0, 64, 64, };
    RECT hit_size_rectB = { 0, 0, 32, 32, };
    RECT hit_rectA;
    RECT hit_rectB;
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //����`�F�b�N�̂��߂̎��@�̓����蔻���`�`���v�Z
    hit_rectA.top    =   work->MyShip.Y + hit_size_rectA.top;
    hit_rectA.bottom =   work->MyShip.Y + hit_size_rectA.bottom;
    hit_rectA.left   =   work->MyShip.X + hit_size_rectA.left;
    hit_rectA.right  =   work->MyShip.X + hit_size_rectA.right;

    //����`�F�b�N�̂��߂̖ڕW�̓����蔻���`�a���v�Z
    hit_rectB.top    =   work->Target.Y + hit_size_rectB.top;
    hit_rectB.bottom =   work->Target.Y + hit_size_rectB.bottom;
    hit_rectB.left   =   work->Target.X + hit_size_rectB.left;
    hit_rectB.right  =   work->Target.X + hit_size_rectB.right;

    //�Ώۘg�S�Ă��΂ƂȂ��`�͈͓̔��ɂ���Γ������Ă���
    if( hit_rectA.top    < hit_rectB.bottom && 
        hit_rectA.bottom > hit_rectB.top    &&
        hit_rectA.left   < hit_rectB.right  &&
        hit_rectA.right  > hit_rectB.left   )
    {
        sprintf( str,"�ڐG���܂���");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}





////////////////////////////////////////////////////
//�W�́|�T ���@�ɒe�������点�ē��_
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
    int            Score;
} EX08_05_STRUCT;

void init08_05(TCB* thisTCB)
{
    EX08_05_STRUCT* work = (EX08_05_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_05(TCB* thisTCB)
{
#define MOVE_SPEED    8.0
#define TARGET_SPEED  10.0
#define HIT_SIZE      16.0       //�����蔻��̑傫��
#define GRAZE_DISTANCE  64.0    //������̋���

    EX08_05_STRUCT* work = (EX08_05_STRUCT*)thisTCB->Work;
    float distance;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //�ڕW�̈ړ�
    work->Target.Y += TARGET_SPEED;
    if( work->Target.Y > SCREEN_HEIGHT)
    {
        work->Target.Y = 0;
        work->Target.X = SCREEN_WIDTH/2;
    }
    //������W�̏ꍇ�G���[���o��̂œ��ʏ���
    if( (work->Target.X == work->MyShip.X )&&
        (work->Target.Y == work->MyShip.Y) )
    {
        distance = 0;
    }else{
        //����Ƃ̋������v��
        distance = sqrtf( (work->Target.X-8 - work->MyShip.X-16) * 
                          (work->Target.X-8 - work->MyShip.X-16) + 
                          (work->Target.Y-8 - work->MyShip.Y-16) * 
                          (work->Target.Y-8 - work->MyShip.Y-16) 
                        );
    }


    //���������ȓ��Ȃ痩���Ă���Ɣ���A�X�R�A�����Z
    if(distance <= GRAZE_DISTANCE)work->Score += 1;

    //�ڐG������X�R�A���N���A
    if(distance <= HIT_SIZE)work->Score = 0;


    sprintf( str,"�X�R�A  %d", work->Score );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}





////////////////////////////////////////////////////
//�W�́|�U ����{��
////////////////////////////////////////////////////
#define BOMB_MAX 3
#define BOMB_TIME 50

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
    SPRITE         Bomb;
    int            BombCount;
    int            BombFlag;
    int            BombTime;
} EX08_06_STRUCT;

void init08_06(TCB* thisTCB)
{
    EX08_06_STRUCT* work = (EX08_06_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\spread.png",&g_pTex[2] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = 0;
    work->BombCount = BOMB_MAX;
}

void exec08_06(TCB* thisTCB)
{
#define MOVE_SPEED    8.0
#define TARGET_SPEED  10.0

    EX08_06_STRUCT* work = (EX08_06_STRUCT*)thisTCB->Work;
    RECT hit_size_rectA = { 0, 0, 64, 64, };
    RECT hit_size_rectB = { 0, 0, 32, 32, };
    RECT hit_rectA;
    RECT hit_rectB;

    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };
    RECT bomb_data ={192,  0,256, 64, };


    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�ڕW�̈ړ�
    work->Target.Y += TARGET_SPEED;
    if( work->Target.Y > SCREEN_HEIGHT)
    {
        work->Target.Y = 0;
        work->Target.X = SCREEN_WIDTH/2;
    }


    if( g_DownInputBuff & KEY_X  )
    {//�w�L�[�Ń{������
        work->BombCount = BOMB_MAX;
    }

    if(!work->BombFlag)
    {//�{�����������Ă��Ȃ�����������
        //�y�L�[�Ŕ�������
        if( g_DownInputBuff & KEY_Z  )
        {//�����ł́A�{���̔����ƁA�{�����̌v�Z���s�Ȃ�
            if(work->BombCount > 0)
            {//�������A�{�����Ȃ��ꍇ�̂�
                work->BombFlag = true;
                if(--work->BombCount < 0) work->BombCount = 0;
                //�{���w���W���R�s�[
                work->Bomb.X = work->MyShip.X;
                work->Bomb.Y = work->MyShip.Y;
            }
        }


        //����`�F�b�N�̂��߂̎��@�̓����蔻���`�`���v�Z
        hit_rectA.top    =   work->MyShip.Y + hit_size_rectA.top;
        hit_rectA.bottom =   work->MyShip.Y + hit_size_rectA.bottom;
        hit_rectA.left   =   work->MyShip.X + hit_size_rectA.left;
        hit_rectA.right  =   work->MyShip.X + hit_size_rectA.right;

        //����`�F�b�N�̂��߂̖ڕW�̓����蔻���`�a���v�Z
        hit_rectB.top    =   work->Target.Y + hit_size_rectB.top;
        hit_rectB.bottom =   work->Target.Y + hit_size_rectB.bottom;
        hit_rectB.left   =   work->Target.X + hit_size_rectB.left;
        hit_rectB.right  =   work->Target.X + hit_size_rectB.right;

        //�e�ɓ��������ꍇ�A��������Ƃ��ă{���̔���
        if( hit_rectA.top    < hit_rectB.bottom && 
            hit_rectA.bottom > hit_rectB.top    &&
            hit_rectA.left   < hit_rectB.right  &&
            hit_rectA.right  > hit_rectB.left   )
        {//�{�^�������Ɠ��l�ɁA���������ƌv�Z���s�Ȃ�

            //�������A�{�����Ȃ��ꍇ�͖����̃��b�Z�[�W��\��
            if(work->BombCount <= 0)
            {
                sprintf( str,"�ڐG���܂���");
                g_pFont->DrawText( NULL,
                         str, -1, &font_pos,
                          DT_LEFT, 0xffffffff);
            }
            if(work->BombCount > 0)
            {
                work->BombFlag = true;
                //�{���w���W���R�s�[
                work->Bomb.X = work->MyShip.X;
                work->Bomb.Y = work->MyShip.Y;

                if(--work->BombCount < 0) work->BombCount = 0;
            }
        }
    }


    if(work->BombFlag)
    {//�{���������̏���
        //�{���̕\��
        work->Bomb.SrcRect = &bomb_data;

        SpriteDraw(&work->Bomb,2);

        //�{���̔������Ԃ�i�߂�
        work->BombTime++;
        //�w��̎��ȂɒB���Ă���ΏI��
        if(work->BombTime >= BOMB_TIME)
        {//�I������
            work->BombTime = 0;
            work->BombFlag = false;
        }
    }



    font_pos.top +=16 ;
    sprintf( str,"�c�{���� �� %d",work->BombCount);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);
}



////////////////////////////////////////////////////
//�W�́|�V �X�s�[�h�A�b�v���̓����蔻��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Ball;
    SPRITE         Target;
    RECT           OldHitRect;
} EX08_07_STRUCT;

int EX08_07_hit_check_double( RECT NowHitRect, RECT BeforeRect, float checkX, float checkY )
{
    RECT HalfHitRect;

    //����`�F�b�N�̂��߂̋�`���v�Z
    HalfHitRect.top    =   (NowHitRect.top   + BeforeRect.top   ) / 2;
    HalfHitRect.bottom =   (NowHitRect.bottom+ BeforeRect.bottom) / 2;
    HalfHitRect.left   =   (NowHitRect.left  + BeforeRect.left  ) / 2;
    HalfHitRect.right  =   (NowHitRect.right + BeforeRect.right ) / 2;

    //�ΏۂƂQ�̓����蔻��̋�`���r
      //�P�ڂ̋�`�i�P�O�̍��W�Ƃ̒��Ԉʒu�j
    if((HalfHitRect.top   < checkY  && 
        HalfHitRect.bottom> checkY  &&
        HalfHitRect.left  < checkX  &&
        HalfHitRect.right > checkX  )||
      //�Q�ڂ̋�`�i���݂̓����蔻��j
       (NowHitRect.top    < checkY  &&
        NowHitRect.bottom > checkY  &&
        NowHitRect.left   < checkX  &&
        NowHitRect.right  > checkX  ))
    {
        return true;
    }

    return false;
}

int EX08_07_hit_check( RECT NowHitRect, float checkX, float checkY )
{
    //�ΏۂƓ����蔻��̋�`���r
    if( NowHitRect.top    < checkY  &&
        NowHitRect.bottom > checkY  &&
        NowHitRect.left   < checkX  &&
        NowHitRect.right  > checkX  )
    {
        return true;
    }

    return false;
}

void init08_07(TCB* thisTCB)
{
    EX08_07_STRUCT* work = (EX08_07_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_07(TCB* thisTCB)
{
#define MOVE_SPEED  32.0
#define TARGET_CENTER  16.0

    EX08_07_STRUCT* work = (EX08_07_STRUCT*)thisTCB->Work;
    int  hit_flag = 0;
    RECT hit_size_rect = { 12,12, 20, 20, };
    RECT hit_rect;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_DownInputBuff & KEY_UP    ) work->Ball.Y -= MOVE_SPEED;
    if( g_DownInputBuff & KEY_DOWN  ) work->Ball.Y += MOVE_SPEED;
    if( g_DownInputBuff & KEY_RIGHT ) work->Ball.X += MOVE_SPEED;
    if( g_DownInputBuff & KEY_LEFT  ) work->Ball.X -= MOVE_SPEED;

    //����`�F�b�N�̂��߂̋�`���v�Z
    hit_rect.top    =   work->Ball.Y + hit_size_rect.top;
    hit_rect.bottom =   work->Ball.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->Ball.X + hit_size_rect.left;
    hit_rect.right  =   work->Ball.X + hit_size_rect.right;


    //�y�L�[�������Ȃ��炾�Ɠ����蔻���ς���
    if( g_InputBuff & KEY_Z )
    {//�Ώۂ����݂Ɖߋ��̋�`�͈͓̔��ɂ���Γ������Ă���
        hit_flag = EX08_07_hit_check_double( hit_rect,  work->OldHitRect,
                                      work->Target.X+TARGET_CENTER,  //���S�ʒu�����Z
                                      work->Target.Y+TARGET_CENTER);
    }else{
     //���݂̓������`�݂̂��`�F�b�N
        hit_flag = EX08_07_hit_check( hit_rect,
                           work->Target.X+TARGET_CENTER,             //�ڕW�̒��S�ʒu�����Z
                           work->Target.Y+TARGET_CENTER);
    }
    //�ڐG���̃`�F�b�N
    if(hit_flag)
    {
        sprintf( str,"�ڐG���܂���");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    //���݂̓������`��ۑ�
    work->OldHitRect = hit_rect;

    SpriteDraw(&work->Ball,0);
    SpriteDraw(&work->Target,0);
}


////////////////////////////////////////////////////
//�W�́|�X �L�����N�^�[���m�̏ڍׂȓ����蔻��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Sprt;
    SPRITE         Target;
} EX08_09_STRUCT;

#define END_CODE -1
int EX08_09_hit_check( RECT* HitRect, RECT* TargetRect)
{
    int hit_loop = 0;
    int target_loop;

    //�Q�̓������`���m���P�Â�����Ŕ�r����
    while( HitRect[hit_loop].top != END_CODE )
    {
        target_loop = 0;
        while( TargetRect[target_loop].top != END_CODE )
        {//�Ώۘg�S�Ăɑ΂��ē�������`�F�b�N�A�ǂꂩ�P�ł��������Ă���ΏI��
            if( HitRect[hit_loop].top    <= TargetRect[target_loop].bottom && 
                HitRect[hit_loop].bottom >= TargetRect[target_loop].top    &&
                HitRect[hit_loop].left   <= TargetRect[target_loop].right  &&
                HitRect[hit_loop].right  >= TargetRect[target_loop].left   )
            {
                return true;
            }
            target_loop++;
        }
        hit_loop++;
    }
    return false;
}

void EX08_09_calc_rect( RECT* AnsRect, RECT* BaseHitRect,float AddX,float AddY)
{
    int loop = 0;

    while( BaseHitRect[loop].top != END_CODE )
    {//�����W������A�����蔻��p�̋�`���Z�o
        AnsRect[loop].top    = BaseHitRect[loop].top     + AddY;
        AnsRect[loop].bottom = BaseHitRect[loop].bottom  + AddY;
        AnsRect[loop].left   = BaseHitRect[loop].left    + AddX;
        AnsRect[loop].right  = BaseHitRect[loop].right   + AddX;
        loop++;
    }

    //����̔z��ɏI���R�[�h����������
    AnsRect[loop].top = -1;
}

void init08_09(TCB* thisTCB)
{
    EX08_09_STRUCT* work = (EX08_09_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\EX08_09_SPRT.png",&g_pTex[0] );

    //�ڕW�̏������W
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_09(TCB* thisTCB)
{
#define MOVE_SPEED  4.0
#define RECT_COUNT  8

    EX08_09_STRUCT* work = (EX08_09_STRUCT*)thisTCB->Work;
    int  hit_flag = 0;
    RECT hit_size_rect[] =
{//�����蔻��̃f�[�^
{ 24,  0, 39, 63, },   //�P�ڂ̓������`
{  0, 16, 63, 23, },   //�Q�ڂ̓������`
{  0, 56, 63, 63, },   //�R�ڂ̓������`
{ -1, -1, -1, -1, },   //�S��(�f�[�^�͏I���R�[�h�j
};
    RECT hit_rect[ RECT_COUNT ];
    RECT target_rect[ RECT_COUNT ];
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;

    //����`�F�b�N�̂��߂̋�`���v�Z
    EX08_09_calc_rect( hit_rect, hit_size_rect, work->Sprt.X, work->Sprt.Y);
    //���l�ɖڕW�̔���`�F�b�N��`���v�Z
    EX08_09_calc_rect( target_rect, hit_size_rect, work->Target.X, work->Target.Y);

    //�o���̓������`���X�g���`�F�b�N
    if( EX08_09_hit_check( hit_rect, target_rect ) )
    {
        sprintf( str,"�ڐG���܂���");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->Sprt,0);
    SpriteDraw(&work->Target,0);
}



////////////////////////////////////////////////////
//�W�́|�P�O ��莞�Ԗ��G
////////////////////////////////////////////////////
#define MOVE_SPEED  8.0
#define BALL_CENTER 16
#define BALL_COUNT 20
#define BALL_SPEED 4
#define BALL_SPEED_MAX 8
#define BALL_PRIORITY 0x2000
#define INVINCIBLE_TIME 50

void exec08_10_ball(TCB* thisTCB);

typedef struct{
    SPRITE         Ball;
    float          AddX;
    float          AddY;
    int            Time;
} EX08_10_BALL;

typedef struct{
    SPRITE         MyShip;
    int            InvincibleTime;
} EX08_10_STRUCT ;


void exec08_10_make_ball( void )
{
    TCB*    ball_tcb;
    EX08_10_BALL* ball_work;

    //�{�[���̏������ƁA���W��ݒ�
    ball_tcb = TaskMake( exec08_10_ball , BALL_PRIORITY );
    ball_work = (EX08_10_BALL*)ball_tcb->Work;
    ball_work->Ball.X = rand() % SCREEN_WIDTH;
    ball_work->Ball.Y = 0;
    ball_work->AddY   = BALL_SPEED + rand() % BALL_SPEED_MAX;
}

void exec08_10_ball(TCB* thisTCB)
{
    EX08_10_BALL* work = (EX08_10_BALL*)thisTCB->Work;

     //��ʏ㕔����ړ�
    if( work->Ball.Y > SCREEN_HEIGHT )
    {
        //�����I�����Ƀ{�[�����쐬����
        exec08_10_make_ball( );
        TaskKill(thisTCB);
        return;
    }

    //�{�[���̈ړ������ƕ\��
    work->Ball.X += work->AddX;
    work->Ball.Y += work->AddY;
    SpriteDraw( &work->Ball,1);
}


int EX08_10_hit_check( RECT* HitRect )
{
    TCB*    check_tcb;
    EX08_10_BALL* ball_work;

    //�^�X�N���X�g��H��A�{�[���̃^�X�N��S���`�F�b�N����
    check_tcb = GetTaskTop();             //�^�X�N�̐擪�A�h���X���擾
    check_tcb = check_tcb->Next;          //�^�X�N�w�b�h�͏���������΂�
    while( check_tcb->Prio != 0x0000)     //�^�X�N�w�b�h�ɂ��ǂ蒅������P��
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//�^�X�N���{�[���ł���΁A�����蔻����`�F�b�N����
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //�ΏۂƓ����蔻��̋�`���r
            if( HitRect->top    < ball_work->Ball.Y + BALL_CENTER &&
                HitRect->bottom > ball_work->Ball.Y + BALL_CENTER &&
                HitRect->left   < ball_work->Ball.X + BALL_CENTER &&
                HitRect->right  > ball_work->Ball.X + BALL_CENTER )
            {
                return true;
            }
        }

        //���Ƀ`�F�b�N����^�X�N
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_10(TCB* thisTCB)
{
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�{�[���̍쐬
    for( loop = 0; loop < BALL_COUNT; loop++)
    {
        exec08_10_make_ball();
    }
}

void exec08_10(TCB* thisTCB)
{
    EX08_10_STRUCT* work = (EX08_10_STRUCT*)thisTCB->Work;
    RECT hit_rect = { 0, 0, 64, 64 };
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };


    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //����`�F�b�N�̂��߂̋�`���v�Z
    hit_rect.top    +=   work->MyShip.Y;
    hit_rect.bottom +=   work->MyShip.Y;
    hit_rect.left   +=   work->MyShip.X;
    hit_rect.right  +=   work->MyShip.X;

    if( work->InvincibleTime )
    {//���G���Ԓ��́A�����蔻����s�Ȃ�Ȃ�
        //���G���Ԃ�����������
        work->InvincibleTime--;
        g_pFont->DrawText( NULL,"���G���Ԓ��I�I" , -1, &font_pos, DT_LEFT, 0xffffffff);
    }else{
     //�����蔻����`�F�b�N���������Ă����疳�G���Ԃ�ݒ�
        if( EX08_10_hit_check( &hit_rect ))
        {
            work->InvincibleTime = INVINCIBLE_TIME;
        }
    }

    //�_�ŏ����ƕ\������
    if(!(work->InvincibleTime & 0x01)) SpriteDraw(&work->MyShip,0);
}



////////////////////////////////////////////////////
//�W�́|�P�P �L�͈̓~�T�C��
////////////////////////////////////////////////////
#define BOMB_CENTER 32
#define ANIM_SPEED 0.2
#define ANIM_COUNT 4
#define BALL_COUNT 20

typedef struct{
    SPRITE         Bomb;
    float          AnimCount;
    float          AnimSpeed;
} EX08_11_STRUCT ;

int EX08_11_hit_check( float PosX, float PosY,float HitRange )
{
    TCB*    check_tcb;
    EX08_10_BALL* ball_work;
    float distance;
    float ball_X;
    float ball_Y;
    float pos_X;
    float pos_Y;

    //�^�X�N���X�g��H��A�{�[���̃^�X�N��S���`�F�b�N����
    check_tcb = GetTaskTop();             //�^�X�N�̐擪�A�h���X���擾
    check_tcb = check_tcb->Next;          //�^�X�N�w�b�h�͏���������΂�
    while( check_tcb->Prio != 0x0000)     //�^�X�N�w�b�h�ɂ��ǂ蒅������P��
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//�^�X�N���{�[���ł���΁A�����蔻����`�F�b�N����
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //���W�𕨑̂̒��S�_�ɍ��킹��
            ball_X = ball_work->Ball.X + BALL_CENTER;
            ball_Y = ball_work->Ball.Y + BALL_CENTER;
            pos_X  = PosX + BOMB_CENTER;
            pos_Y  = PosY + BOMB_CENTER;

            //������W�̏ꍇ�G���[���o��̂œ��ʏ����ɂ���
            if( ball_X == pos_X && ball_Y == pos_Y )
            {//���W���������������I�ɉ�ʊO�ֈړ������A����
                ball_work->Ball.Y = SCREEN_HEIGHT;
            }else{
                //����Ƃ̋������v�����A������͈͓��Ȃ��ʊO�ֈړ������A����
                distance = sqrtf( (ball_X - pos_X) * (ball_X - pos_X) + 
                                (ball_Y - pos_Y) * (ball_Y - pos_Y) );
                if( distance <= HitRange )  ball_work->Ball.Y = SCREEN_HEIGHT;
            }
        }
        //���Ƀ`�F�b�N����^�X�N
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_11(TCB* thisTCB)
{
    EX08_11_STRUCT* work = (EX08_11_STRUCT*)thisTCB->Work;
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\spread.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�{�[���̍쐬
    for( loop = 0; loop < BALL_COUNT; loop++)
    {//�O���ڂ̊֐����ė��p
        exec08_10_make_ball();
    }
    work->Bomb.X = SCREEN_WIDTH / 2;
    work->Bomb.Y = SCREEN_HEIGHT / 2;
}

void exec08_11(TCB* thisTCB)
{
    EX08_11_STRUCT* work = (EX08_11_STRUCT*)thisTCB->Work;
    float hit_range[ ANIM_COUNT ] = { 16.0, 32.0, 48.0, 64.0,};    //�����蔻��̑傫��
    RECT anim_table[ ANIM_COUNT] = 
{
    {  0,  0, 64, 64,},   //�P�R�}��
    { 64,  0,128, 64,},   //�Q�R�}��
    {128,  0,192, 64,},   //�R�R�}��
    {192,  0,256, 64,},   //�S�R�}��
};


    //�y�L�[�Ŕ��������J�n
    if( g_DownInputBuff & KEY_Z  )
    {//�����ł́A�A�j���[�V�����̊J�n���s�Ȃ�����
        work->AnimSpeed = ANIM_SPEED;
    }

    //�A�j���[�V�������n�܂��Ă���Ε\��
    if(work->AnimSpeed != 0.0)
    {
        //���ݕ\���̃t���[������A�j���[�V�����p�̍��W���擾
        work->Bomb.SrcRect = &anim_table[(int)work->AnimCount];

        //�\��
        SpriteDraw(&work->Bomb,0);

        //�����̏����A���W�Ɣ͈͂�n��
        EX08_11_hit_check( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, hit_range[(int)work->AnimCount]);

        //�A�j���[�V������i�߂�
        work->AnimCount += work->AnimSpeed;
        //�w��̃R�}���ɒB���Ă���΃A�j���[�V�����I��
        if(work->AnimCount >= ANIM_COUNT)
        {
            work->AnimCount = 0;
            work->AnimSpeed = 0;
        }
    }
}



////////////////////////////////////////////////////
//�W�́|�P�Q �܂Ƃ߂ēG�𐁂���΂�
////////////////////////////////////////////////////
#define MOVE_SPEED  8.0
#define MYSHIP_CENTER 32
#define BALL_COUNT 20
#define BALL_BASH_SPEED 24
#define BALL_PRIORITY 0x2000
#define HIT_RANGE 150
#define BALL_BASH_TIME 5

typedef struct{
    SPRITE         MyShip;
} EX08_12_STRUCT ;

void EX08_12_bash_ball( TCB* thisTCB )
{
    EX08_10_BALL* work = (EX08_10_BALL*)thisTCB->Work;

     //��莞�Ԃ݈̂ړ�
    if( work->Time++ > BALL_BASH_TIME )
    {
        //�����I�����Ƀ{�[�����쐬����
        exec08_10_make_ball( );
        TaskKill(thisTCB);
        return;
    }
    //�{�[���̈ړ������ƕ\��
    work->Ball.X += work->AddX;
    work->Ball.Y += work->AddY;
    SpriteDraw( &work->Ball,1);
}

int EX08_12_hit_check( float PosX, float PosY )
{
    TCB*    check_tcb;
    EX08_10_BALL* ball_work;
    float distance;
    float direction;
    float ball_X;
    float ball_Y;
    float pos_X;
    float pos_Y;


    //�^�X�N���X�g��H��A�{�[���̃^�X�N��S���`�F�b�N����
    check_tcb = GetTaskTop();             //�^�X�N�̐擪�A�h���X���擾
    check_tcb = check_tcb->Next;          //�^�X�N�w�b�h�͏���������΂�
    while( check_tcb->Prio != 0x0000)     //�^�X�N�w�b�h�ɂ��ǂ蒅������P��
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//�^�X�N���{�[���ł���΁A�����蔻����`�F�b�N����
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //���W�𕨑̂̒��S�_�ɍ��킹��
            ball_X = ball_work->Ball.X + BALL_CENTER;
            ball_Y = ball_work->Ball.Y + BALL_CENTER;
            pos_X  = PosX + MYSHIP_CENTER;
            pos_Y  = PosY + MYSHIP_CENTER;

            //������W�̏ꍇ�G���[���o��̂œ��ʏ����ɂ���
            if( ball_X == pos_X && ball_Y == pos_Y )
            {
                ball_work->AddX = 0;
                ball_work->AddY = 0;
                TaskChange( check_tcb, EX08_12_bash_ball);
            }else{
                //����Ƃ̋������v��
                distance = sqrtf( (ball_X - pos_X) * (ball_X - pos_X) + 
                                  (ball_Y - pos_Y) * (ball_Y - pos_Y) );
                if( distance <= HIT_RANGE )
                {//������΂��͈͓̔��ł���΃{�[���̑��x�����@�Ƃ͔��΂̕����֐ݒ肷��
                    //���W���瑊��ւ̕������v�Z
                    direction = atan2( ball_Y - pos_Y, ball_X - pos_X );
                    //��������A�w�A�x���ꂼ��̑��x���v�Z
                    ball_work->AddX = cos( direction ) * BALL_BASH_SPEED;
                    ball_work->AddY = sin( direction ) * BALL_BASH_SPEED;
                    //�O������A�{�[�������𐁂���΂������ɐ؂�ւ���
                    TaskChange( check_tcb, EX08_12_bash_ball);
                 }
            }
        }
        //���Ƀ`�F�b�N����^�X�N
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_12(TCB* thisTCB)
{
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //�{�[���̍쐬
    for( loop = 0; loop < BALL_COUNT; loop++)
    {//�O���ڂ̊֐����ė��p
        exec08_10_make_ball();
    }
}

void exec08_12(TCB* thisTCB)
{
    EX08_12_STRUCT* work = (EX08_12_STRUCT*)thisTCB->Work;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�y�L�[�Ő�����΂�����
    if( g_DownInputBuff & KEY_Z  )
    {//�����蔻����`�F�b�N��������΂�����
        EX08_12_hit_check( work->MyShip.X, work->MyShip.Y );
    }
    //�\��
    SpriteDraw(&work->MyShip,0);
}



////////////////////////////////////////////////////
//�W�́|�P�R  �L�����N�^�[���m�̓����蔻��
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Human;
    int            PID;
    TCB*           EnemyPlayerTask;
} EX08_13_STRUCT;

#define MOVE_SPEED    4.0
#define PLAYER_1      0
#define PLAYER_2      1

#define HUMAN_WIDTH   64
#define HUMAN_HEIGHT  96


void EX08_13_pos_add_rect( RECT* ans_rect, RECT* add_rect, float x, float y )
{//��`�̈�ɍ��W�����Z
    ans_rect->top    = add_rect->top    + y;
    ans_rect->bottom = add_rect->bottom + y;
    ans_rect->left   = add_rect->left   + x;
    ans_rect->right  = add_rect->right  + x;
}

int EX08_13_hit_rect( RECT* hit_rectA, RECT* hit_rectB )
{//�Q�̋�`�̐ڐG����
    //�Ώۘg�S�Ă��΂ƂȂ��`�͈͓̔��ɂ���Γ������Ă���
    if( hit_rectA->top    < hit_rectB->bottom && 
        hit_rectA->bottom > hit_rectB->top    &&
        hit_rectA->left   < hit_rectB->right  &&
        hit_rectA->right  > hit_rectB->left   )
    {//�q�b�g�I
        return( true );
    }
    //�q�b�g���Ă��Ȃ�
    return( false );
}

unsigned char EX08_13_player_input( int PlayerID )
{    unsigned char input_key;

    //���͒l���v���C���[�ɂ���ĕύX����
    switch( PlayerID )
    {//�L�[�{�[�h���v���C���[�P�A�W���C�X�e�B�b�N���v���C���[�Q�Ƃ���
       case PLAYER_1: input_key = g_KeyInputBuff; break;
       case PLAYER_2: input_key = g_JoystickBuff; break;
    }
    return input_key;
}

void exec08_13_human(TCB* thisTCB)
{
    EX08_13_STRUCT* work = (EX08_13_STRUCT*)thisTCB->Work;
    //���葤�̃v���C���[�̃^�X�N
    EX08_13_STRUCT* enemy_work = (EX08_13_STRUCT*)work->EnemyPlayerTask->Work;
    //�P�t���[���O�̍��W
    float old_posX;
    float old_posY;
    unsigned char input_buff;
    RECT anim_rect = {0,0,64,128,};
    RECT human_hit_rect  = {0,0,64,128,};
    RECT my_hit_rect;
    RECT enemy_hit_rect;

    //�P�t���[���O�̍��W��ێ�
    old_posX = work->Human.X;
    old_posY = work->Human.Y;

    //�v���C���[�L�����̕\��
    work->Human.SrcRect = &anim_rect;

    //�v���C���[ID�ɂ�����
    input_buff = EX08_13_player_input( work->PID );

    //�L�[���͂ɂ��ړ�
    if( input_buff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( input_buff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;

    //���L�����̓����蔻����擾
    EX08_13_pos_add_rect( 
            &my_hit_rect, &human_hit_rect,
            work->Human.X, work->Human.Y);

    //����L�������̓����蔻����擾
    EX08_13_pos_add_rect( 
            &enemy_hit_rect, &human_hit_rect,
            enemy_work->Human.X, enemy_work->Human.Y);

    //�����蔻����`�F�b�N
    if( EX08_13_hit_rect( &my_hit_rect, &enemy_hit_rect ) )
    {//�����ڐG���Ă�����
     //�ړ��ł��Ȃ��̂ō��W��߂�
        work->Human.X = old_posX;
        work->Human.Y = old_posY;
    }

    //���L�����̕`��
    SpriteDraw( &work->Human,work->PID);
}

void init08_13(TCB* thisTCB)
{
    TCB*    human_tcb[2];
    EX08_13_STRUCT* human_work[2];
    int loop;
    float player_pos[][2] = 
{//�v���C���[�̏������W
{SCREEN_WIDTH / 4.0 * 1.0,SCREEN_HEIGHT / 2.0,}, //PLAYER1
{SCREEN_WIDTH / 4.0 * 3.0,SCREEN_HEIGHT / 2.0,}, //PLAYER2
};

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim_r.png",&g_pTex[1] );

    //�v���C���[���Q�̍쐬����
    for( loop = 0;loop < 2; loop++ )
    {
        human_tcb[loop] = TaskMake( exec08_13_human, 0x2000 );
        human_work[loop] = (EX08_13_STRUCT*)human_tcb[loop]->Work;
        //�v���C���[�ɂ�菉���\�����W��ς���
        human_work[loop]->Human.X = player_pos[loop][0];
        human_work[loop]->Human.Y = player_pos[loop][1];
        //�v���C���[ID��o�^
        human_work[loop]->PID = loop;
    }

    //���݂��̃^�X�N���[�N��ێ����Ă���
    human_work[PLAYER_1]->EnemyPlayerTask 
        = human_tcb[PLAYER_2];
    human_work[PLAYER_2]->EnemyPlayerTask 
        = human_tcb[PLAYER_1];
}

void exec08_13(TCB* thisTCB)
{
    //�������������̂��߁A�T���v���ł̓��C�������͉������Ȃ�
}



////////////////////////////////////////////////////
//�W�́|�P�T �i���Q�[���̍U��
////////////////////////////////////////////////////
#define MOVE_SPEED      4.0
#define PLAYER_1        0
#define PLAYER_2        1

#define HUMAN_WIDTH     64
#define HUMAN_HEIGHT    96
#define ATTACK_SPEED    0.20
#define ATTACK_ANIM_MAX 4.0
#define ATTACK_ANIM_PTN 2

typedef struct{
    SPRITE         Human;
    int            PID;
    TCB*           EnemyPlayerTask;
    float          AnimFrame;
} EX08_15_STRUCT;

void exec08_15_human_move(TCB*);

//�v���C���[�̓����蔻��̈�
static RECT EX08_15_human_hit_rect  = {0,0,64,128,};

void exec08_15_human_attack(TCB* thisTCB)
{//�U�����̏���
    EX08_15_STRUCT* work = (EX08_15_STRUCT*)thisTCB->Work;
    //�ΐ푤�̃v���C���[�̃^�X�N
    EX08_15_STRUCT* enemy_work = (EX08_15_STRUCT*)work->EnemyPlayerTask->Work;
    RECT my_attack_rect;
    RECT enemy_hit_rect;
    char str[128];

    //�U���͈͂̍��W 
    RECT human_attack_rect[]  =
{//�U��������W
 { 64, 32, 96, 164,}, //PLAYER_1
 {-32, 32,  0, 164,}, //PLAYER_2
};
    RECT attack_anim_rect[] = 
{
 {  0,  0, 64,128,},  //1�R�}��
 {128,  0,192,128,},  //2�R�}��
 {192,  0,256,128,},  //3�R�}��(�U���j
 {128,  0,192,128,},  //4�R�}��
};

    //�v���C���[�L�����̃A�j���[�V�����\��
    work->Human.SrcRect = &attack_anim_rect[(int)work->AnimFrame];

    work->AnimFrame += ATTACK_SPEED;
    if(work->AnimFrame >= ATTACK_ANIM_MAX)
    {//�U���A�j�����I�������珈���؂�ւ�
        work->AnimFrame = 0;
        TaskChange(thisTCB,exec08_15_human_move);
        //���L�����̕`��
        SpriteDraw( &work->Human,work->PID);
    }

    if((int)work->AnimFrame == ATTACK_ANIM_PTN)
    {//�U���̃p�^�[���Ȃ�U��������s�Ȃ�
        //���L�����̍U��������擾
        EX08_13_pos_add_rect( 
                &my_attack_rect, 
                &human_attack_rect[work->PID],
                work->Human.X, work->Human.Y);

        //����L�������̓����蔻����擾
        EX08_13_pos_add_rect( 
                &enemy_hit_rect, &EX08_15_human_hit_rect,
                enemy_work->Human.X, enemy_work->Human.Y);

        //�����蔻����`�F�b�N
        if( EX08_13_hit_rect( &my_attack_rect, &enemy_hit_rect ) )
        {//�����ڐG���Ă�����
            //�U�������Ƃ݂Ȃ��A������\��
            sprintf( str,"ATTACK HIT!! P%d",work->PID+1 );
            FontPrint( 128,128+16*work->PID, str);
        }
    }

    //���L�����̕`��
    SpriteDraw( &work->Human,work->PID);
}


void exec08_15_human_move(TCB* thisTCB)
{
    EX08_15_STRUCT* work = (EX08_15_STRUCT*)thisTCB->Work;
    //�ΐ푤�̃v���C���[�̃^�X�N
    EX08_15_STRUCT* enemy_work = (EX08_15_STRUCT*)work->EnemyPlayerTask->Work;
    //�P�t���[���O�̍��W
    float old_posX;
    float old_posY;
    unsigned char input_buff;
    RECT anim_rect = {0,0,64,128,};
    RECT my_hit_rect;
    RECT enemy_hit_rect;

    //�P�t���[���O�̍��W��ێ�
    old_posX = work->Human.X;
    old_posY = work->Human.Y;

    //�v���C���[�L�����̕\��
    work->Human.SrcRect = &anim_rect;

    //�v���C���[ID�ɂ�����
    input_buff = EX08_13_player_input( work->PID );

    //�L�[���͂ɂ��ړ�
    if( input_buff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( input_buff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;

    //�L�[���͂ɂ��U��
    if( input_buff & KEY_Z )
    {//�p���`�ɂ��U��
        TaskChange(thisTCB,exec08_15_human_attack);
        //���L�����̕`��
        SpriteDraw( &work->Human,work->PID);
        return;
    }


    //���L�����̓����蔻����擾
    EX08_13_pos_add_rect( 
            &my_hit_rect, &EX08_15_human_hit_rect,
            work->Human.X, work->Human.Y);

    //����L�������̓����蔻����擾
    EX08_13_pos_add_rect( 
            &enemy_hit_rect, &EX08_15_human_hit_rect,
            enemy_work->Human.X, enemy_work->Human.Y);

    //�����蔻����`�F�b�N
    if( EX08_13_hit_rect( &my_hit_rect, &enemy_hit_rect ) )
    {//�����ڐG���Ă�����
     //�ړ��ł��Ȃ��̂ō��W��߂�
        work->Human.X = old_posX;
        work->Human.Y = old_posY;
    }

    //���L�����̕`��
    SpriteDraw( &work->Human,work->PID);
}

void init08_15(TCB* thisTCB)
{
    TCB*    human_tcb[2];
    EX08_15_STRUCT* human_work[2];
    int loop;
    float player_pos[][2] = 
{//�v���C���[�̏������W
{SCREEN_WIDTH / 4.0 * 1.0,SCREEN_HEIGHT / 2.0,}, //PLAYER1
{SCREEN_WIDTH / 4.0 * 3.0,SCREEN_HEIGHT / 2.0,}, //PLAYER2
};

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim_r.png",&g_pTex[1] );

    //�v���C���[���Q�̍쐬����
    for( loop = 0;loop < 2; loop++ )
    {
        human_tcb[loop] = TaskMake( exec08_15_human_move, 0x2000 );
        human_work[loop] = (EX08_15_STRUCT*)human_tcb[loop]->Work;
        //�v���C���[�ɂ�菉���\�����W��ς���
        human_work[loop]->Human.X = player_pos[loop][0];
        human_work[loop]->Human.Y = player_pos[loop][1];
        //�v���C���[ID��o�^
        human_work[loop]->PID = loop;
    }

    //���݂��̃^�X�N���[�N��ێ����Ă���
    human_work[PLAYER_1]->EnemyPlayerTask 
        = human_tcb[PLAYER_2];
    human_work[PLAYER_2]->EnemyPlayerTask 
        = human_tcb[PLAYER_1];
}

void exec08_15(TCB* thisTCB)
{
    //�������������̂��߁A�T���v���ł̓��C�������͉������Ȃ�
}



////////////////////////////////////////////////////
//�X�� �A�C�e������
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�X�́|�P �A�C�e�������̊�{
////////////////////////////////////////////////////

#define ITEM_00    0
#define ITEM_01    1
#define ITEM_02    2
#define ITEM_03    3
#define ITEM_MAX   4
#define ITEM_SPEED 5.0
#define MOVE_SPEED 4.0

typedef struct{
    SPRITE         Sprt;
    int            ID;
    float          AddX;
    float          AddY;
    int            Time;
    DWORD          Color;
} EX09_01_ITEM;

typedef struct{
    SPRITE         MyShip;
    int            ItemFlag;
    EX09_01_ITEM   Item;
    char           DispMess[64];
} EX09_01_STRUCT;


void EX09_01_CustomDraw(SPRITE* pspr,int bitmap_id, DWORD color)
{
    D3DXVECTOR3 pos;

    pos.x = pspr->X;
    pos.y = pspr->Y;
    pos.z = 0;

    g_pSp->Draw( g_pTex[bitmap_id],pspr->SrcRect,NULL,&pos, color);
}

void init09_01(TCB* thisTCB)
{
    EX09_01_STRUCT* work = (EX09_01_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���@���W�̏�����
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
}

void exec09_01(TCB* thisTCB)
{
    EX09_01_STRUCT* work = (EX09_01_STRUCT*)thisTCB->Work;
    RECT hit_size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;
    RECT font_pos = {  0,  0,640,480,};
    DWORD item_color[] = 
{//�A�C�e���̐F
    0xff0000ff,
    0xff00ff00,
    0xffff0000,
    0xffffffff,
};


    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�y�L�[�ŃA�C�e�����쐬
    if( g_DownInputBuff & KEY_Z )
    {
        //�����ɂ���ăA�C�e���͍쐬���Ȃ�
        if( !work->ItemFlag ) 
        {
            //����ȏ�A�C�e�����o�����Ȃ��悤�Ƀt���O�����Ă�
            work->Item.Sprt.X = rand() % SCREEN_WIDTH;
            work->Item.Sprt.Y = 0;
            work->Item.AddY   = ITEM_SPEED;
            //�A�C�e���̎�ނ����肷��
            work->Item.ID     = rand() % ITEM_MAX;
            work->Item.Color  = item_color[ work->Item.ID ];
            work->ItemFlag    = true;
        }
    }

    //�A�C�e���֘A�̏���

    if( work->ItemFlag )
    {//�A�C�e���o�����̂ݏ���
        //�A�C�e���擾���̓����蔻���`���v�Z
        hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
        hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
        hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
        hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

        //�A�C�e���̎擾�`�F�b�N
        if( hit_rect.top    < work->Item.Sprt.Y  && 
            hit_rect.bottom > work->Item.Sprt.Y  &&
            hit_rect.left   < work->Item.Sprt.X  &&
            hit_rect.right  > work->Item.Sprt.X  )
        {//�A�C�e���擾���̏���
            switch(work->Item.ID)
            {//�A�C�e����ID�ɉ����ď�����ς���i�����ł̓��b�Z�[�W�̂݁j
                case ITEM_00: sprintf( work->DispMess,"�X�R�A���Z"); break;
                case ITEM_01: sprintf( work->DispMess,"�p���[�A�b�v"); break;
                case ITEM_02: sprintf( work->DispMess,"�X�s�[�h�A�b�v"); break;
                case ITEM_03: sprintf( work->DispMess,"�P�t�o"); break;
            }
            //�擾��A�C�e�������͏I��
            work->ItemFlag = false;
        }
        //�A�C�e���̈ړ������ƕ\��
        work->Item.Sprt.X += work->Item.AddX;
        work->Item.Sprt.Y += work->Item.AddY;
        //��ʊO�ւ̈ړ��ŃA�C�e��������
        if( work->Item.Sprt.Y > SCREEN_HEIGHT ) work->ItemFlag = false;
        //�w��̃A�C�e���F�ŕ`��
        EX09_01_CustomDraw( &work->Item.Sprt,1,work->Item.Color);
    }

    //���@�̕\��
    SpriteDraw( &work->MyShip,0);

    //�擾�����A�C�e�����ʂ̕\��
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�X�́|�Q �L��������A�C�e�����o��
////////////////////////////////////////////////////

#define NO_ITEM    -1
#define ITEM_00    0
#define ITEM_01    1
#define ITEM_02    2
#define ITEM_03    3
#define ITEM_MAX   4

#define ITEM_SPEED  5.0
#define MOVE_SPEED  4.0
#define ENEMY_SPEED 8.0
#define DROP_TIME  50
#define ENEMY_PRIORITY  0x2000
#define ITEM_PRIORITY   0x4000

typedef struct{
    SPRITE         MyShip;
    char           DispMess[64];
} EX09_02_STRUCT;

typedef struct{
    SPRITE         Sprt;
    int            Timer;
} EX09_02_ENEMY;

void EX09_02_item_move(TCB* thisTCB)
{
    EX09_01_ITEM* work = (EX09_01_ITEM*)thisTCB->Work;
    DWORD item_color[] = 
{//�A�C�e���̐F
    0xff0000ff,
    0xff00ff00,
    0xffff0000,
    0xffffffff,
};

     //��ʊO�ɏo�������
    if( work->Sprt.Y > SCREEN_HEIGHT )
    {//�I������
        TaskKill(thisTCB);
        return;
    }
    //�A�C�e���̐F������
    work->Color  = item_color[ work->ID ];
    //�A�C�e���̈ړ������ƕ\��
    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;
    EX09_01_CustomDraw( &work->Sprt,1,work->Color);
}

void EX09_02_enemy_move( TCB* thisTCB )
{
    EX09_02_ENEMY* work = (EX09_02_ENEMY*)thisTCB->Work;
    TCB*    item_tcb;
    EX09_01_ITEM* item_work;

    if(work->Timer++ >= DROP_TIME)
    {//��莞�Ԗ��ɁA�A�C�e���𗎂Ƃ�
        //���Ƃ��A�C�e���̍쐬
        item_tcb = TaskMake( EX09_02_item_move , ITEM_PRIORITY );
        item_work = (EX09_01_ITEM*)item_tcb->Work;
        //�o���ʒu�́A���݂̍��W���R�s�[����
        item_work->Sprt.X = work->Sprt.X;
        item_work->Sprt.Y = work->Sprt.Y;;
        item_work->AddY   = ITEM_SPEED;
        //�A�C�e���̎�ނ������_���Ɍ��肷��
        item_work->ID     = rand() % ITEM_MAX;

        //�A�C�e���𗎂Ƃ����Ԃ�������
        work->Timer = 0;
    }

     //��ʊO�ɏo������W��߂�
    if( work->Sprt.X > SCREEN_WIDTH ) work->Sprt.X = 0;

    //�ړ������ƕ\��
    work->Sprt.X += ENEMY_SPEED;
    SpriteDraw( &work->Sprt,2);
}

int EX09_02_get_item( RECT* HitRect )
{
    TCB*    check_tcb;
    EX09_01_ITEM* item_work;

    //�^�X�N��S���`�F�b�N����
    //�^�X�N�̐擪�A�h���X���擾
    check_tcb = GetTaskTop();
    //�^�X�N�w�b�h�͏���������΂�
    check_tcb = check_tcb->Next;
    //�^�X�N�w�b�h�ɂ��ǂ蒅������P��
    while( check_tcb->Prio != 0x0000)
    {
        if( check_tcb->Prio == ITEM_PRIORITY )
        {//�^�X�N���A�C�e���ł���΁A�����蔻����`�F�b�N����
            item_work = (EX09_01_ITEM*)check_tcb->Work;
            //�ΏۂƓ����蔻��̋�`���r
            if( HitRect->top    < item_work->Sprt.Y &&
                HitRect->bottom > item_work->Sprt.Y &&
                HitRect->left   < item_work->Sprt.X &&
                HitRect->right  > item_work->Sprt.X )
            {//�A�C�e���ɐڐG�������̏���
                //�A�C�e�����������A�A�C�e��ID��Ԃ�
                TaskKill( check_tcb );
                return item_work->ID;
            }
        }
        //���Ƀ`�F�b�N����^�X�N
        check_tcb = check_tcb->Next;
    }
    return NO_ITEM;
}

void init09_02(TCB* thisTCB)
{
    EX09_02_STRUCT* work = (EX09_02_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //���@���W�̏�����
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //�A�C�e���𗎂Ƃ��L�����̍쐬
    TaskMake( EX09_02_enemy_move , ENEMY_PRIORITY );
}

void exec09_02(TCB* thisTCB)
{
    EX09_02_STRUCT* work = (EX09_02_STRUCT*)thisTCB->Work;
    RECT hit_size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;
    RECT font_pos = {  0,  0,640,480,};


    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�A�C�e���֘A�̏���

    //�A�C�e���擾���̓����蔻���`���v�Z
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //�A�C�e���̎擾�`�F�b�N
    switch( EX09_02_get_item(&hit_rect) )
    {//�擾�A�C�e����ID�ɉ����ď�����ς���
        case ITEM_00: sprintf( work->DispMess,"�X�R�A���Z"); break;
        case ITEM_01: sprintf( work->DispMess,"�p���[�A�b�v"); break;
        case ITEM_02: sprintf( work->DispMess,"�X�s�[�h�A�b�v"); break;
        case ITEM_03: sprintf( work->DispMess,"�P�t�o"); break;
        //�A�C�e���̎擾�͖���
        default: break;
    }

    //���@�̕\��
    SpriteDraw( &work->MyShip,0);

    //�擾�����A�C�e�����ʂ̕\��
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}







////////////////////////////////////////////////////
//�X�́|�U �A�C�e�������F��莞�Ԍ��ʂ𔭊�
////////////////////////////////////////////////////

#define NO_ITEM    -1
#define ITEM_00    0
#define ITEM_01    1
#define ITEM_02    2
#define ITEM_03    3
#define ITEM_MAX   4

#define MOVE_SPEED  4.0
#define ENEMY_PRIORITY  0x2000
#define EFFECT_PRIORITY 0x3000
#define EFFECT_TIME 30

typedef struct{
    SPRITE         MyShip;
    char           DispMess[64];
} EX09_06_STRUCT;


void EX09_06_item_effect01( TCB* thisTCB )
{
    RECT font_pos = {  0, 32,640,480,};

    //��莞�Ԍ��ʂ𔭊����������
    if( thisTCB->Work[0]++ >= EFFECT_TIME ) TaskKill(thisTCB);

    g_pFont->DrawText( NULL, "�p���[�A�b�v��!!", -1, &font_pos, DT_LEFT, 0xffffffff);
}

void EX09_06_item_effect02( TCB* thisTCB )
{
    RECT font_pos = {  0, 32,640,480,};

    //��莞�Ԍ��ʂ𔭊����������
    if( thisTCB->Work[0]++ >= EFFECT_TIME ) TaskKill(thisTCB);

    g_pFont->DrawText( NULL, "�X�s�[�h�A�b�v��!!", -1, &font_pos, DT_LEFT, 0xffffffff);
}

void init09_06( TCB* thisTCB )
{
    EX09_06_STRUCT* work = (EX09_06_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //���@���W�̏�����
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //�A�C�e���𗎂Ƃ��L�����̍쐬
    TaskMake( EX09_02_enemy_move , ENEMY_PRIORITY );
}

void exec09_06( TCB* thisTCB )
{
    EX09_06_STRUCT* work = (EX09_06_STRUCT*)thisTCB->Work;
    RECT hit_size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;
    RECT font_pos = {  0, 32,640,480,};


    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�A�C�e���֘A�̏���

    //�A�C�e���擾���̓����蔻���`���v�Z
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //�A�C�e���̎擾�`�F�b�N
    switch( EX09_02_get_item(&hit_rect) )
    {//���Ԍ���̏����̓^�X�N���쐬���đΉ�
        case ITEM_00: sprintf( work->DispMess,"�X�R�A���Z"); break;
        case ITEM_01:
               //�A�C�e�����ʗp�̃^�X�N���쐬
                sprintf( work->DispMess,"");
                TaskMake( EX09_06_item_effect01, EFFECT_PRIORITY);
            break;
        case ITEM_02:
               //�A�C�e�����ʗp�̃^�X�N���쐬
                sprintf( work->DispMess,"");
                TaskMake( EX09_06_item_effect02, EFFECT_PRIORITY);
            break;
        case ITEM_03: sprintf( work->DispMess,"�P�t�o"); break;

        //�A�C�e���̎擾�͖���
        default: break;
    }

    //���@�̕\��
    SpriteDraw( &work->MyShip,0);

    //�擾�����A�C�e�����ʂ̕\��
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//�X�́|�V �A�C�e�����擾��Ɏg�p����
////////////////////////////////////////////////////

#define NO_ITEM    -1
#define ITEM_00    0
#define ITEM_01    1
#define ITEM_02    2
#define ITEM_03    3
#define ITEM_MAX   4

#define MOVE_SPEED  4.0
#define ENEMY_PRIORITY  0x2000
#define EFFECT_PRIORITY 0x3000
#define EFFECT_TIME 30

typedef struct{
    SPRITE         MyShip;
    int            HaveItem;
    char           DispMess[64];
} EX09_07_STRUCT;


void init09_07( TCB* thisTCB )
{
    EX09_07_STRUCT* work = (EX09_07_STRUCT*)thisTCB->Work;
    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //���@���W�̏�����
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //�A�C�e���𗎂Ƃ��L�����̍쐬
    TaskMake( EX09_02_enemy_move , ENEMY_PRIORITY );
}

void exec09_07( TCB* thisTCB )
{
    EX09_07_STRUCT* work = (EX09_07_STRUCT*)thisTCB->Work;
    int  check_item;
    RECT hit_size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;
    RECT font_pos = {  0, 32,640,480,};


    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //�A�C�e���֘A�̏���

    //�A�C�e���擾���̓����蔻���`���v�Z
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //�A�C�e���̎擾�`�F�b�N
    check_item = EX09_02_get_item(&hit_rect);
    if( check_item != NO_ITEM )
    {//�A�C�e�����擾���Ă�����A�����A�C�e����؂�ւ���
        work->HaveItem = check_item;
        switch( check_item )
        {//�擾�����A�C�e���ɉ����āA�����A�C�e���̕\��������
            case ITEM_00: sprintf(work->DispMess,"����ITEM�F�X�R�A���Z"); break;
            case ITEM_01: sprintf(work->DispMess,"����ITEM�F�p���[�A�b�v"); break;
            case ITEM_02: sprintf(work->DispMess,"����ITEM�F�X�s�[�h�A�b�v"); break;
            case ITEM_03: sprintf(work->DispMess,"����ITEM�F�P�t�o"); break;
        }
    }

    if( g_DownInputBuff & KEY_Z )
    {//�y�L�[�ŏ����A�C�e���̎g�p
        switch( work->HaveItem )
        {//�A�C�e����ID�ɉ����ď�����ς���i�����ł̓��b�Z�[�W�̂݁j
            case ITEM_00: sprintf( work->DispMess,"�X�R�A���ZITEM�g�p"); break;
            case ITEM_01: sprintf( work->DispMess,"�p���[�A�b�vITEM�g�p"); break;
            case ITEM_02: sprintf( work->DispMess,"�X�s�[�h�A�b�vITEM�g�p"); break;
            case ITEM_03: sprintf( work->DispMess,"�P�t�oITEM�g�p"); break;
            //�A�C�e�����������Ă��Ȃ�
            default: sprintf(work->DispMess,"ITEM���������Ă��܂���"); break;break;
        }
        //�����A�C�e���̔j��
        work->HaveItem = NO_ITEM;
    }

    //���@�̕\��
    SpriteDraw( &work->MyShip,0);

    //�擾�����A�C�e�����ʂ̕\��
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//�X�́|�W �����A�C�e���̊Ǘ�
////////////////////////////////////////////////////

#define ITEM_NORMAL    0
#define ITEM_EVENT     1

typedef struct _EX09_RPG_ITEM_DATA {
    //�����֐�
    int (*ItemFunc)( _EX09_RPG_ITEM_DATA*, char* );
    //�A�C�e����
    char*          ItemName;
    //�����^�C�v
    int            ItemType;
    //�A�C�e�����L�̃f�[�^
    int            Param;
} EX09_RPG_ITEM_DATA;


//���b�Z�[�W�̍���
#define FONT_SIZE 16
#define NO_ITEM    -1
#define HAVE_MAX  4
#define ITEM_X    192
#define ITEM_Y    64

typedef struct{
    int            HaveItem[HAVE_MAX];
} EX09_08_STRUCT;

static EX09_RPG_ITEM_DATA g_EX09_08_ItemData[] = 
{
  {NULL,"��"      , ITEM_NORMAL, 5,},  //ID00
  {NULL,"���Ö�"    , ITEM_NORMAL,50,},  //ID01
  {NULL,"�ŏ�����"  , ITEM_NORMAL, 0,},  //ID02
  {NULL,"�����̖�"  , ITEM_NORMAL, 0,},  //ID03
};

void init09_08(TCB* thisTCB)
{
    EX09_08_STRUCT* work = (EX09_08_STRUCT*)thisTCB->Work;

    //�����p�̔z��ɃA�C�e�����i�[
    work->HaveItem[0] = 0;
    work->HaveItem[1] = 1;
    work->HaveItem[2] = 2;
    work->HaveItem[3] = 3;
}

void exec09_08(TCB* thisTCB)
{
    EX09_08_STRUCT* work = (EX09_08_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 0, 0, 640, 480, };


    //�A�C�e�����e�̕\��
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        g_pFont->DrawText( NULL, 
            g_EX09_08_ItemData[ work->HaveItem[ loop ] ].ItemName,
            -1,
            &font_pos,
            DT_LEFT,
            0xffffffff);
        font_pos.top += FONT_SIZE;
    }
}


////////////////////////////////////////////////////
//�X�́|�X �A�C�e�������̌��ʂ𔽉f
////////////////////////////////////////////////////
#define ITEM_NORMAL    0
#define ITEM_EVENT     1

//���b�Z�[�W�̍���
#define FONT_SIZE 16
#define NO_ITEM    -1
#define HAVE_MAX  4
#define ITEM_X    192
#define ITEM_Y    64

typedef struct{
    int            HaveItem[HAVE_MAX];
    SPRITE         SelectMark;
    int            SelectMenu;
    char           DispMess[64];
} EX09_09_STRUCT;


int EX09_09_use_item00( EX09_RPG_ITEM_DATA* ItemData ,char* ResMess )
{//�򑐁E���Ö򓙁A�񕜑S��

   //�񕜏������s�Ȃ��i�����̓T���v���ł͗Ꭶ���܂���j

    //���b�Z�[�W��Ԃ�
    sprintf( ResMess,"%s���g�p���܂����B�g�o %d�񕜁I�I",ItemData->ItemName, ItemData->Param );
}

int EX09_09_use_item02( EX09_RPG_ITEM_DATA* ItemData ,char* ResMess )
{//�ŏ�����
    //���b�Z�[�W��Ԃ�
    sprintf( ResMess,"�ŏ��������g�p���܂����B�ł��̂���������I�I");
}

int EX09_09_use_item03( EX09_RPG_ITEM_DATA* ItemData ,char* ResMess )
{//�����̌�  �C�x���g�A�C�e��
    //���b�Z�[�W��Ԃ�
    sprintf( ResMess,"���̃A�C�e�����g�p���鎖�͏o���܂���I");
}

static EX09_RPG_ITEM_DATA g_EX09_09_ItemData[] = 
{
  {EX09_09_use_item00,"��"      , ITEM_NORMAL, 5,},  //ID00
  {EX09_09_use_item00,"���Ö�"    , ITEM_NORMAL,50,},  //ID01
  {EX09_09_use_item02,"�ŏ�����"  , ITEM_NORMAL, 0,},  //ID02
  {EX09_09_use_item03,"�����̌�"  , ITEM_EVENT , 0,},  //ID03
};

void init09_09(TCB* thisTCB)
{
    EX09_09_STRUCT* work = (EX09_09_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //�����p�̔z��ɃA�C�e��ID���i�[�i�A�C�e���̎擾�j
    work->HaveItem[0] = 0;
    work->HaveItem[1] = 1;
    work->HaveItem[2] = 2;
    work->HaveItem[3] = 3;
}

void exec09_09(TCB* thisTCB)
{
    EX09_09_STRUCT* work = (EX09_09_STRUCT*)thisTCB->Work;
    int select_item;
    int loop;
    RECT font_pos = { 0, 0, 640, 480, };

    //�A�C�e���I��
    if( g_DownInputBuff & KEY_DOWN )
    {
        work->SelectMenu++;
        if( work->SelectMenu >= HAVE_MAX )work->SelectMenu = 0;
    }
    if( g_DownInputBuff & KEY_UP  )
    {
        work->SelectMenu--;
        if( work->SelectMenu < 0 ) work->SelectMenu = HAVE_MAX-1;
    }

    if( g_DownInputBuff & KEY_Z  )
    {//�y�L�[�őI�𒆂̃A�C�e���̎g�p
        //�I�𒆂̈ʒu�ɃA�C�e�������邩���m�F
        select_item = work->HaveItem[ work->SelectMenu ];

        if( select_item != NO_ITEM )
        {//�A�C�e��������Ƃ��̂ݎ��s
            //�A�C�e�������֐������s
            //�f�[�^�ƃ��b�Z�[�W�̃|�C���^��n��
            g_EX09_09_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        //�ʏ�̃A�C�e���͎g�p��ɏ�������
        if( g_EX09_09_ItemData[ select_item ].ItemType == ITEM_NORMAL)
            work->HaveItem[ work->SelectMenu ] = NO_ITEM;
    }

    //�I���}�[�N�̈ʒu������
    //�A�C�e�����e�̕\��

    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //�I���}�[�N�̕\��
    SpriteDraw(&work->SelectMark,0);

    //���j���[�̕\��
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if( work->HaveItem[ loop ] != NO_ITEM )
        {//�����A�C�e�������鎞�̂ݕ\��
            g_pFont->DrawText(
                NULL, 
                g_EX09_09_ItemData[ work->HaveItem[ loop ] ].ItemName,
                -1,
                &font_pos,
                DT_LEFT,
                0xffffffff);
        }
        font_pos.top += FONT_SIZE;
    }

    //�A�C�e���g�p���̃��b�Z�[�W�̕\��
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�X�́|�P�O  �����A�C�e���̊Ǘ�
////////////////////////////////////////////////////
#define ITEM_NORMAL    0
#define ITEM_EVENT     1

//���b�Z�[�W�̍���
#define FONT_SIZE 16
#define NO_ITEM    -1
#define HAVE_MAX  4
#define ITEM_X    192
#define ITEM_Y    64
typedef struct{
    int            HaveItem[HAVE_MAX];
    SPRITE         SelectMark;
    int            SelectMenu;
    char           DispMess[64];
} EX09_10_STRUCT;


int EX09_10_item_add( int* ItemList,int ID )
{
    int loop;

    for(loop = 0; loop < HAVE_MAX; loop++)
    {//�A�C�e�����̋󂫂�T��
        if( ItemList[ loop ] == NO_ITEM )break;
    }
    //�A�C�e��������ȏ���ĂȂ�������G���[
    if( loop == HAVE_MAX ) return -1;
    //�A�C�e����ǉ�
    ItemList[ loop ] = ID;
    //����������A�C�e����}�������ʒu��Ԃ�
    return loop;
}

void EX09_10_item_del( int* ItemList,int ID )
{
    //�A�C�e�����폜
    ItemList[ ID ] = NO_ITEM;
}

int EX09_10_item_search( int* ItemList,int ID )
{
    int loop;

    for(loop = 0; loop < HAVE_MAX; loop++)
    {//�w��ID�̃A�C�e����T��
        if( ItemList[ loop ] == ID )break;
    }

    //�A�C�e���𔭌��ł��Ȃ�������I��
    if( loop == HAVE_MAX ) return -1;

    //����������A�A�C�e����ID��Ԃ�
    return loop;
}

void EX09_10_item_correct( int* ItemList )
{
    int loop;
    int collect_flag = 0;

    //���߂̃A�C�e�������󂫂��`�F�b�N
    if( ItemList[0] == NO_ITEM )collect_flag = true;
    for(loop = 0; loop < HAVE_MAX-1; loop++)
    {
        if(collect_flag)
        {//�␳�t���O����������̃A�C�e����
         //�S�ĂP�l�߂�
             ItemList[ loop ] = ItemList[ loop+1];
        }
        //��̃A�C�e������T��
        if( ItemList[ loop+1] == NO_ITEM )collect_flag = true;
    }

    //�␳�t���O�������Ă�����A�Ō�̗��̓A�C�e������
    if(collect_flag) ItemList[ loop ] = NO_ITEM;
}


void init09_10(TCB* thisTCB)
{
    EX09_10_STRUCT* work = (EX09_10_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //�A�C�e���������z��ɉ�����i�����A�C�e���̎擾�j
    work->HaveItem[0] = 0;
    work->HaveItem[1] = 1;
    work->HaveItem[2] = 2;
    work->HaveItem[3] = 3;
}

void exec09_10(TCB* thisTCB)
{
    EX09_09_STRUCT* work = (EX09_09_STRUCT*)thisTCB->Work;
    int select_item;
    int loop;
    int get_item;
    int err;
    RECT font_pos = { 0, 0, 640, 480, };


    //�A�C�e���I��
    if( g_DownInputBuff & KEY_DOWN )
    {
        work->SelectMenu++;
        if( work->SelectMenu >= HAVE_MAX )work->SelectMenu = 0;
    }
    if( g_DownInputBuff & KEY_UP  )
    {
        work->SelectMenu--;
        if( work->SelectMenu < 0 ) work->SelectMenu = HAVE_MAX-1;
    }


    if( g_DownInputBuff & KEY_X  )
    {//�w�L�[�Ń����_���ɃA�C�e�����擾
        //�A�C�e���������_���Ɍ��߂�
        //�i���͗v�f����O���j
        get_item = rand() % (HAVE_MAX - 1);

        //�A�C�e����ǉ�
        err = EX09_10_item_add( work->HaveItem, get_item);
        if(err == -1)
        {//�ǉ��ł��Ȃ������烁�b�Z�[�W���o��
            sprintf( work->DispMess,"�A�C�e����ǉ��ł��܂���ł����B");
        }
    }

    if( g_DownInputBuff & KEY_Z  )
    {//�y�L�[�őI�𒆂̃A�C�e���̎g�p
        //�I�𒆂̈ʒu�ɃA�C�e�������邩���m�F
        select_item = work->HaveItem[ work->SelectMenu ];
        if( select_item != NO_ITEM )
        {//�A�C�e��������Ƃ��̂ݎ��s
            //�A�C�e�������֐������s
            //�f�[�^�ƃ��b�Z�[�W�̃|�C���^��n��
            g_EX09_09_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        if( g_EX09_09_ItemData[ select_item ].ItemType == ITEM_NORMAL)
        {//�ʏ�̃A�C�e���͎g�p��ɏ�������
            EX09_10_item_del(work->HaveItem,work->SelectMenu );
           //������A�J���������l�߂�
            EX09_10_item_correct(work->HaveItem);
        }
    }

    //�I���}�[�N�̈ʒu������
    //�A�C�e�����e�̕\��
    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //�I���}�[�N�̕\��
    SpriteDraw(&work->SelectMark,0);

    //���j���[�̕\��
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if( work->HaveItem[ loop ] != NO_ITEM )
        {//�����A�C�e�������鎞�̂ݕ\��
            g_pFont->DrawText(
                NULL, 
                g_EX09_09_ItemData[ work->HaveItem[ loop ] ].ItemName,
                -1,
                &font_pos,
                DT_LEFT,
                0xffffffff);
        }
        font_pos.top += FONT_SIZE;
    }

    //�A�C�e���g�p���̃��b�Z�[�W�̕\��
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//�X�́|�P�P  ���������\�ȃA�C�e��
////////////////////////////////////////////////////

#define ITEM_NORMAL    0
#define ITEM_EVENT     1
#define ITEM_COUNT     2

//���b�Z�[�W�̍���
#define FONT_SIZE 16
#define HAVE_MAX  4
#define HAVE_ITEM_COUNT 5
#define NO_ITEM    -1
#define ITEM_X    192
#define ITEM_Y    64

typedef struct{
    int            HaveItem[HAVE_MAX];
    int            ItemCount[HAVE_MAX];
    SPRITE         SelectMark;
    int            SelectMenu;
    char           DispMess[64];
} EX09_11_STRUCT;


static EX09_RPG_ITEM_DATA g_EX09_11_ItemData[] = 
{
  {EX09_09_use_item00,"��"      , ITEM_COUNT , 5,},  //ID00
  {EX09_09_use_item00,"���Ö�"    , ITEM_NORMAL,50,},  //ID01
  {EX09_09_use_item02,"�ŏ�����"  , ITEM_NORMAL, 0,},  //ID02
  {EX09_09_use_item03,"�����̌�"  , ITEM_EVENT , 0,},  //ID03
};


void init09_11(TCB* thisTCB)
{
    EX09_11_STRUCT* work = (EX09_11_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //�A�C�e���������z��ɉ�����i�����A�C�e���̎擾�j
    work->HaveItem[0] = 0;
    work->HaveItem[1] = 1;
    work->HaveItem[2] = 2;
    work->HaveItem[3] = 3;
    //�򑐂̐���������
    work->ItemCount[0] = HAVE_ITEM_COUNT;
}

void exec09_11(TCB* thisTCB)
{
    EX09_11_STRUCT* work = (EX09_11_STRUCT*)thisTCB->Work;
    int select_item;
    int loop;
    int get_item;
    int err;
    RECT font_pos = { 0, 0, 640, 480, };
    char str[128];

    //�A�C�e���I��
    if( g_DownInputBuff & KEY_DOWN )
    {
        work->SelectMenu++;
        if( work->SelectMenu >= HAVE_MAX )work->SelectMenu = 0;
    }
    if( g_DownInputBuff & KEY_UP  )
    {
        work->SelectMenu--;
        if( work->SelectMenu < 0 ) work->SelectMenu = HAVE_MAX-1;
    }


    if( g_DownInputBuff & KEY_Z  )
    {//�y�L�[�őI�𒆂̃A�C�e���̎g�p
        //�I�𒆂̈ʒu�ɃA�C�e�������邩���m�F
        select_item = work->HaveItem[ work->SelectMenu ];
        if( select_item != NO_ITEM )
        {//�A�C�e��������Ƃ��̂ݎ��s
            //�A�C�e�������֐������s
            //�f�[�^�ƃ��b�Z�[�W�̃|�C���^��n��
            g_EX09_11_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        switch(g_EX09_11_ItemData[ select_item ].ItemType)
        {//�A�C�e���̃^�C�v�ɂ���ď����𕪂���
            case ITEM_NORMAL:
                   //�ʏ�̃A�C�e���͎g�p��ɏ�������
                   EX09_10_item_del(work->HaveItem,work->SelectMenu );
                   //������A�J���������l�߂�
                   EX09_10_item_correct(work->HaveItem);
                   break;
            case ITEM_COUNT:
                   //�J�E���g�A�C�e���͐������炵�ă`�F�b�N���������
                   if( --work->ItemCount[ work->SelectMenu ] <= 0)
                   {
                       EX09_10_item_del(work->HaveItem,work->SelectMenu );
                       //������A�J���������l�߂�
                       EX09_10_item_correct(work->HaveItem);
                   }
                   break;
            default: break;
        }
    }

    //�I���}�[�N�̈ʒu������
    //�A�C�e�����e�̕\��
    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //�I���}�[�N�̕\��
    SpriteDraw(&work->SelectMark,0);

    //���j���[�̕\��
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if(work->HaveItem[ loop ] != NO_ITEM)
        {//�������Ă���A�C�e����\��
            switch(g_EX09_11_ItemData[work->HaveItem[ loop ]].ItemType)
            {//�A�C�e���̃^�C�v�ɂ���ď����𕪂���
                case ITEM_NORMAL:
                case ITEM_EVENT:
                       //�J�E���g�A�C�e���ȊO�͕��ʂɃA�C�e�����\��
                       strcpy(str,g_EX09_11_ItemData[work->HaveItem[ loop ]].ItemName);
                       break;
                case ITEM_COUNT:
                       //�J�E���g�A�C�e���͖����Ɏg�p�񐔂�\��
                       sprintf(str,"%s    %d",
                               g_EX09_11_ItemData[work->HaveItem[ loop ]].ItemName,
                               work->ItemCount[ loop ]);
                       break;
                default: break;
            }
            g_pFont->DrawText(NULL, str, -1,&font_pos,DT_LEFT,0xffffffff);
            font_pos.top += FONT_SIZE;
        }
    }

    //�A�C�e���g�p���̃��b�Z�[�W�̕\��
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}









////////////////////////////////////////////////////
//�P�O�� �Q�[���o�����X
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//�P�O�́|�Q �Q�[���o�����X���� �����N�@
////////////////////////////////////////////////////
#define RANK_MAX 3
#define BULLET_TIME 15
typedef struct {
    SPRITE         Sprt;
    int            Time;
    int            Rank;
} EX10_02_STRUCT;

typedef struct{
    SPRITE         Sprt;
    float          AddX;  //�����l�w
    float          AddY;  //�����l�x
} EX10_02_BULLET;

void exec10_02_bullet(TCB* thisTCB)
{
    EX10_02_BULLET* work = (EX10_02_BULLET*)thisTCB->Work;

    if( work->Sprt.X < 0 || work->Sprt.X >= SCREEN_WIDTH ||
        work->Sprt.Y < 0 || work->Sprt.Y >= SCREEN_HEIGHT )
    {//��ʊO�ւ̈ړ��Œe������
        TaskKill( thisTCB );
    }

    //���W�ɑ����l�����Z
    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    SpriteDraw(&work->Sprt,1);
}

void init10_02(TCB* thisTCB)
{
    EX10_02_STRUCT* work = (EX10_02_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���W�ݒ�
    work->Sprt.X = SCREEN_WIDTH  / 2;
    work->Sprt.Y = SCREEN_HEIGHT / 3 * 2;
}

void exec10_02(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    EX10_02_STRUCT* work = (EX10_02_STRUCT*)thisTCB->Work;
    TCB* tmp_tcb;
    EX10_02_BULLET* tmp_work;
    char str[128];
    //�ڕW�̕���
    float direction;
    //�����N�ɂ�鑬�x
    float rank_speed;
float speed_data[] = 
{//�����N�ʂ̑��x�f�[�^
    8.0,   //RANK 0(EASY)
    12.0,  //RANK 1(NORMAL)
    16.0,  //RANK 2(HARD)
};

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
    //�����N�̐؂�ւ�
    if( g_DownInputBuff & KEY_Z ) work->Rank--;
    if( g_DownInputBuff & KEY_X ) work->Rank++;
    if( work->Rank <  0 ) work->Rank = 0;
    if( work->Rank >= RANK_MAX ) work->Rank = RANK_MAX-1;

    if(work->Time++ >= BULLET_TIME)
    {//��莞�Ԃ��Ƃɒe���쐬
        tmp_tcb = TaskMake( exec10_02_bullet, 0x2000 );
        tmp_work = (EX10_02_BULLET*)tmp_tcb->Work;

        //��ɒ��S���W���甭��
        tmp_work->Sprt.X = SCREEN_WIDTH  / 2;
        tmp_work->Sprt.Y = SCREEN_HEIGHT / 2;

        //���S���W���玩�@�ւ̕������v�Z
        direction = 
            atan2( work->Sprt.Y - tmp_work->Sprt.Y,
                   work->Sprt.X - tmp_work->Sprt.X );

        //���x�̓����N�ɂ���Č���
        rank_speed = speed_data[ work->Rank ];

        //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
        tmp_work->AddX = cos( direction ) * rank_speed;
        tmp_work->AddY = sin( direction ) * rank_speed;

        work->Time = 0;
    }

    SpriteDraw( &work->Sprt, 0);

    sprintf(str,"RANK = %d",work->Rank);
    FontPrint( 16,16, str);
}





////////////////////////////////////////////////////
//�P�O�́|�R �Q�[���o�����X���� ���x���@
////////////////////////////////////////////////////
#define LEVEL_MAX 256
#define BULLET_SPEED_MIN 4.0
#define BULLET_SPEED_MAX 15.0
#define BULLET_TIME 15

typedef struct {
    SPRITE         Sprt;
    int            Time;
    int            Level;
} EX10_03_STRUCT;

float EX10_03_BulletSpeed(float Level)
{//���x���l����e�̑��x���v�Z
    return  (BULLET_SPEED_MAX - BULLET_SPEED_MIN) * Level / LEVEL_MAX + BULLET_SPEED_MIN;
}

void init10_03(TCB* thisTCB)
{
    EX10_03_STRUCT* work = (EX10_03_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //���W�ݒ�
    work->Sprt.X = SCREEN_WIDTH  / 2;
    work->Sprt.Y = SCREEN_HEIGHT / 3 * 2;
}

void exec10_03(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    EX10_03_STRUCT* work = (EX10_03_STRUCT*)thisTCB->Work;
    TCB* tmp_tcb;
    EX10_02_BULLET* tmp_work;
    char str[128];
    //�ڕW�̕���
    float direction;
    //���x���ɂ�鑬�x
    float level_speed;


    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
    //���x���l�̑���
    if( g_InputBuff & KEY_Z ) work->Level--;
    if( g_InputBuff & KEY_X ) work->Level++;
    if( work->Level <  0 ) work->Level = 0;
    if( work->Level >= LEVEL_MAX ) work->Level = LEVEL_MAX-1;

    if(work->Time++ >= BULLET_TIME)
    {//��莞�Ԃ��Ƃɒe���쐬
        tmp_tcb = TaskMake( exec10_02_bullet, 0x2000 );
        tmp_work = (EX10_02_BULLET*)tmp_tcb->Work;

        //��ɒ��S���W���甭��
        tmp_work->Sprt.X = SCREEN_WIDTH  / 2;
        tmp_work->Sprt.Y = SCREEN_HEIGHT / 2;

        //���S���W���玩�@�ւ̕������v�Z
        direction = 
            atan2( work->Sprt.Y - tmp_work->Sprt.Y,
                   work->Sprt.X - tmp_work->Sprt.X );

        //���x���l���瑬�x���Z�o
        level_speed = EX10_03_BulletSpeed(work->Level);

        //��������A�w�A�x���ꂼ��̍��W�����l���v�Z
        tmp_work->AddX = cos( direction ) * level_speed;
        tmp_work->AddY = sin( direction ) * level_speed;

        work->Time = 0;
    }

    SpriteDraw( &work->Sprt, 0);

    sprintf(str,"LEVEL = %3d",work->Level);
    FontPrint( 16,16, str);
}





////////////////////////////////////////////////////
//�P�O�́|�U ���x���A�b�v�����Ƃ��̐��l�̊Ǘ�
////////////////////////////////////////////////////

typedef struct{
    int            Level;
    int            Str;
    int            Agi;
    int            HP;
    int            MP;
} EX10_06_STRUCT;

void exec10_06(TCB* thisTCB)
{
#define LEVEL_MAX  10

typedef struct{
    char           Str;       //��
    char           Agi;       //�f����
    char           HP;        //�g�o
    char           MP;        //�l�o
} EX10_06_TABLE;

    EX10_06_STRUCT* work = (EX10_06_STRUCT*)thisTCB->Work;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };
    EX10_06_TABLE LevelTable[ LEVEL_MAX ] =
{
    { 1, 5,15, 5,},  //Level 1-2
    { 2, 4, 5, 5,},  //Level 2-3
    { 3, 3, 3, 5,},  //Level 3-4
    { 4, 2,25, 5,},  //Level 4-5
    { 1, 5, 0, 5,},  //Level 5-6
    { 2, 4, 2, 5,},  //Level 6-7
    { 3, 3, 3, 5,},  //Level 7-8
    { 4, 2, 5, 5,},  //Level 8-9
    { 1, 5,12, 5,},  //Level 9-10
    { 0, 0, 0, 0,},  //Level10-11
};

    //�L�[���͂ɂ�郌�x���A�b�v
    if( g_DownInputBuff & KEY_Z )
    {
         //���x�����ő�ɂȂ��Ă��邩�`�F�b�N
         if( work->Level < LEVEL_MAX - 1 )
         {//���x���A�b�v�����A�e��p�����[�^���l���A�b�v
             work->Str += LevelTable[ work->Level ].Str;
             work->Agi += LevelTable[ work->Level ].Agi;
             work->HP  += LevelTable[ work->Level ].HP;
             work->MP  += LevelTable[ work->Level ].MP;
             work->Level++;
         }
    }

    //���x���̏�����
    if( g_DownInputBuff & KEY_X )
    {
         work->Str = 0;
         work->Agi = 0;
         work->HP  = 0;
         work->MP  = 0;
         work->Level = 0;
    }
    //�e��p�����[�^�\��
    sprintf( str,"���x��  %d", work->Level + 1 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;

    sprintf( str,"����    %2d  �g�o %3d", work->Str,work->HP );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;

    sprintf( str,"�f����  %2d  �l�o %3d", work->Agi,work->MP );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);


}



////////////////////////////////////////////////////
//�P�P�� �T�E���h
////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//ReadWaveFile: WAV�t�@�C���̏��擾�Ɠǂݍ��݂��s��
//              ���j�APCM�̂ݑΉ��Atag���P���data�̂ݑΉ�
//
//char *pFileName:           �t�@�C����
//WAVEFORMATEX* pWaveFormat: �����i�[����\���̂ւ̃|�C���^
//char* pWaveData:           �ǂݍ��܂��WAV�f�[�^�ւ̃|�C���^
//////////////////////////////////////////////////////////////////////
int ReadWaveFile( char *pFileName, WAVEFORMATEX* pWaveFormat, char* pWaveData )
{
    FILE *fp;
    int wave_size;
    char dummy[16];

    fp = fopen( pFileName, "rb" );
    if ( !fp ) return( false );

    //�`�����N����"RIFF"�ƁARIFF�f�[�^�T�C�Y��ǂݔ�΂�
    fread( dummy, 1, 4 + 4, fp );

    //�`�����N����"WAVE"�ƁA"fmt "��ǂݔ�΂�
    fread( dummy, 1, 4 + 4, fp );

    //fmt�f�[�^�T�C�Y�G���A��ǂݔ�΂�
    fread( dummy, sizeof( int ), 1, fp );

    //�t�H�[�}�b�g���擾 ���j�APCM�f�[�^�݂̂Ŋg�����͑��݂��Ȃ�
    fread( pWaveFormat, sizeof( WAVEFORMATEX ) - 2, 1, fp );

    //�`�����N����"data"��ǂݔ�΂�
    ZeroMemory(dummy, sizeof( dummy ));
    while( strcmp( "data", dummy ) )
    {
        fread( dummy, 1, 1, fp );
        if( dummy[0] == 'd' ) fread( &dummy[1], 1, 3, fp );
    }

    //Wave�f�[�^�T�C�Y�擾
    fread( &wave_size, sizeof( int ), 1, fp );

    //wave�f�[�^�擾
    fread( pWaveData, 1, wave_size, fp );

    fclose( fp );

    return( wave_size );
}


//////////////////////////////////////////////////////////////////////
//ReadWaveFileOffset: WAV�t�@�C���̏��擾�Ɠǂݍ��݂��s�� offset�w��
//                    ���j�APCM�̂ݑΉ��Atag���P���data�̂ݑΉ�
//
//char *pFileName:           �t�@�C����
//WAVEFORMATEX* pWaveFormat: �����i�[����\���̂ւ̃|�C���^
//char* pWaveData:           �ǂݍ��܂��WAV�f�[�^�ւ̃|�C���^
//int   ReadOffset:          �ǂݍ��܂��WAV�f�[�^�ւ̃I�t�Z�b�g�l
//int   ReadSize:            �ǂݍ��܂��WAV�f�[�^�̃T�C�Y
//////////////////////////////////////////////////////////////////////
int ReadWaveFileOffset( char *pFileName, WAVEFORMATEX* pWaveFormat, char* pWaveData, int ReadOffset ,int ReadSize)
{
    FILE *fp;
    int wave_size;
    int data_top;             //�f�[�^�擪�ʒu
    int read_size2;
    char dummy[16];

    fp = fopen( pFileName, "rb" );
    if ( !fp ) return( false );

    //�`�����N����"RIFF"�ƁARIFF�f�[�^�T�C�Y��ǂݔ�΂�
    fread( dummy, 1, 4 + 4, fp );

    //�`�����N����"WAVE"�ƁA"fmt "��ǂݔ�΂�
    fread( dummy, 1, 4 + 4, fp );

    //fmt�f�[�^�T�C�Y�G���A��ǂݔ�΂�
    fread( dummy, sizeof( int ), 1, fp );

    //�t�H�[�}�b�g��� �擾���j�APCM�f�[�^�݂̂ŁA�g�����͑��݂��Ȃ�
    fread( pWaveFormat, sizeof( WAVEFORMATEX ) - 2, 1, fp );


    //�`�����N����"data"��ǂݔ�΂�
    ZeroMemory(dummy, sizeof( dummy ));
    while( strcmp( "data", dummy ) )
    {
        fread( dummy, 1, 1, fp );
        if( dummy[0] == 'd' ) fread( &dummy[1], 1, 3, fp );
    }

    //Wave�f�[�^�T�C�Y�擾
    fread( &wave_size, sizeof( int ), 1, fp );

    //�f�[�^�擪�ʒu���L�^
    data_top = ftell( fp );

    //�ǂݍ��݊J�n�ʒu�փV�[�N
    //�f�[�^�ǂݎ��ʒu���f�[�^�ŏI�ʒu���z���Ȃ��悤�ɒ���
    ReadOffset %= wave_size;
    fseek( fp, ReadOffset, SEEK_CUR );

    //wave�f�[�^�擾
      //�ǂݎ��f�[�^���f�[�^�ŏI�ʒu���z���Ă��邩�H

    if( ReadOffset + ReadSize > wave_size )
    {   //�����Ȃ璲�����A�������ēǂݍ���
        read_size2 = wave_size - ReadOffset;
        fread( pWaveData, 1, read_size2, fp );

        //�ǂݎ��ʒu���A�f�[�^�̐擪�֖߂��A�c��f�[�^��ǂݍ���
        fseek( fp, data_top, SEEK_SET );
        fread( &pWaveData[read_size2], 1, ReadSize - read_size2, fp );
    }else
    {   //�����łȂ���Έꊇ�ǂݍ���
        fread( pWaveData, 1, ReadSize, fp );
    }

    fclose( fp );

    return( wave_size );
}


////////////////////////////////////////////////////
//�P�P�́|�Q �P���Đ�
////////////////////////////////////////////////////

void init11_02(TCB* thisTCB)
{
    int wave_size;
    //�A�N�Z�X�\�ȃo�b�t�@�̃T�C�Y
    DWORD  buff_size1, buff_size2;
    //WAV�o�b�t�@�A�N�Z�X�|�C���g���i�[����ׂ̃|�C���^
    LPVOID pvAudioPtr1, pvAudioPtr2; 

    //Wave�t�@�C���̏��ƃf�[�^�擾
    wave_size = ReadWaveFile( "..\\..\\data\\bgm00.wav", &g_WaveControllBGM->WaveFormat, g_WaveControllBGM->WaveData);

    //�擾��������ݒ�
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
    g_WaveControllBGM->DSBDesc.dwBufferBytes = wave_size;
    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //�T�E���h�o�b�t�@�쐬
    g_pDSound->CreateSoundBuffer(&g_WaveControllBGM->DSBDesc,
                                 &g_WaveControllBGM->pDSBuffer, NULL );

    //�o�b�t�@���b�N
    g_WaveControllBGM->pDSBuffer->Lock( 0, wave_size, //WAVE�f�[�^�T�C�Y���������b�N����
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //�S�u���b�N�����b�N
                                        DSBLOCK_ENTIREBUFFER
                                       );

    //�T�E���h�f�[�^���o�b�t�@�֏�������
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, wave_size );

    //���b�N����
    g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1, pvAudioPtr2, buff_size2 );

}

void exec11_02(TCB* thisTCB)
{
    //�Đ�
    if( g_DownInputBuff & KEY_Z ) 
        g_WaveControllBGM->pDSBuffer->Play( 0, 0, 0 );
    //��~
    if( g_DownInputBuff & KEY_X ) 
        g_WaveControllBGM->pDSBuffer->Stop();

}



////////////////////////////////////////////////////
//�P�P�́|�R �t�F�[�h�C��/�A�E�g
////////////////////////////////////////////////////
void init11_03(TCB* thisTCB)
{
    int wave_size;
    //�A�N�Z�X�\�ȃo�b�t�@�̃T�C�Y
    DWORD  buff_size1, buff_size2;
    //WAV�o�b�t�@�A�N�Z�X�|�C���g���i�[����ׂ̃|�C���^
    LPVOID pvAudioPtr1, pvAudioPtr2;

    //Wave�t�@�C���̏��ƃf�[�^�擾
    wave_size = ReadWaveFile( "..\\..\\data\\bgm00.wav", &g_WaveControllBGM->WaveFormat, g_WaveControllBGM->WaveData);

    //�擾��������ݒ�
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE|DSBCAPS_CTRLVOLUME;  //�{�����[�����ς�����悤�Ɏw��
    g_WaveControllBGM->DSBDesc.dwBufferBytes = wave_size;
    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //�T�E���h�o�b�t�@�쐬
    g_pDSound->CreateSoundBuffer( &g_WaveControllBGM->DSBDesc,
                                  &g_WaveControllBGM->pDSBuffer, NULL );

    //�o�b�t�@���b�N
    g_WaveControllBGM->pDSBuffer->Lock( 0, wave_size, //WAVE�f�[�^�T�C�Y���������b�N����
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //�S�u���b�N�����b�N
                                        DSBLOCK_ENTIREBUFFER
                                       );

    //�T�E���h�f�[�^���o�b�t�@�֏�������
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, wave_size );

    //���b�N����
    g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1, 
                                          pvAudioPtr2, buff_size2 );

}

void exec11_03(TCB* thisTCB)
{
#define FADE_IN  1
#define FADE_OUT 2
#define FADE_STEP (DSBVOLUME_MIN / 100)

    int* fade_count = thisTCB->Work;
    int* fade_mode  = &thisTCB->Work[1];

    if( *fade_mode == 0)
    {
        //�Đ�
        if( g_DownInputBuff & KEY_Z ) 
            g_WaveControllBGM->pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
        //��~
        if( g_DownInputBuff & KEY_X ) 
            g_WaveControllBGM->pDSBuffer->Stop();

        //�t�F�[�h�J�n�̃`�F�b�N
        if( g_DownInputBuff & KEY_UP   )
        {   //�t�F�[�h�C���J�n
            *fade_count = DSBVOLUME_MIN;
            *fade_mode = FADE_IN;
        }
        if( g_DownInputBuff & KEY_DOWN )
        {   //�t�F�[�h�A�E�g�J�n
            *fade_count = DSBVOLUME_MAX;
            *fade_mode = FADE_OUT;
        }

    }else if( *fade_mode == FADE_IN)
    {//�t�F�[�h�C������
        *fade_count -= FADE_STEP;
        if( *fade_count >= DSBVOLUME_MAX)
        {
            *fade_count = DSBVOLUME_MAX;
            *fade_mode = 0;
        }

        g_WaveControllBGM->pDSBuffer->SetVolume( *fade_count );
    }else if( *fade_mode == FADE_OUT)
    {//�t�F�[�h�A�E�g����
        *fade_count += FADE_STEP;
        if( *fade_count <= DSBVOLUME_MIN)
        {
            *fade_count = DSBVOLUME_MIN;
            *fade_mode = 0;
        }

        g_WaveControllBGM->pDSBuffer->SetVolume( *fade_count );
    }

}






////////////////////////////////////////////////////
//�P�P�́|�T �r�d�Đ�
////////////////////////////////////////////////////

void init11_05(TCB* thisTCB)
{
    int loop;
    int wave_size;
    //�A�N�Z�X�\�ȃo�b�t�@�̃T�C�Y
    DWORD  buff_size1, buff_size2;
    //WAV�o�b�t�@�A�N�Z�X�|�C���g���i�[����ׂ̃|�C���^
    LPVOID pvAudioPtr1, pvAudioPtr2; 

char* se_file[] = 
{
    "..\\..\\data\\se00.wav",
    "..\\..\\data\\se01.wav",
    "..\\..\\data\\se02.wav",
    "..\\..\\data\\se03.wav",
};

    for( loop = 0; loop < 4 ; loop++ )
    {
        //Wave�t�@�C���̏��ƃf�[�^�擾
        wave_size = 
        ReadWaveFile( se_file[ loop ], &g_WaveControllSE[ loop ]->WaveFormat, g_WaveControllSE[ loop ]->WaveData);

        //�擾��������ݒ�
        g_WaveControllSE[ loop ]->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
        g_WaveControllSE[ loop ]->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
        g_WaveControllSE[ loop ]->DSBDesc.dwBufferBytes = wave_size;
        g_WaveControllSE[ loop ]->DSBDesc.lpwfxFormat   = &g_WaveControllSE[ loop ]->WaveFormat;

        //�T�E���h�o�b�t�@�쐬
        g_pDSound->CreateSoundBuffer( &g_WaveControllSE[ loop ]->DSBDesc, 
                                      &g_WaveControllSE[ loop ]->pDSBuffer,
                                      NULL );

        //�o�b�t�@���b�N
        //WAVE�f�[�^�T�C�Y���������b�N����
        g_WaveControllSE[ loop ]->pDSBuffer->Lock( 0, wave_size,
                                                   &pvAudioPtr1, &buff_size1,
                                                   &pvAudioPtr2, &buff_size2,
                                                   //�S�u���b�N�����b�N
                                                   DSBLOCK_ENTIREBUFFER
                                                  );
        //�T�E���h�f�[�^���o�b�t�@�֏�������
        memcpy( pvAudioPtr1, g_WaveControllSE[ loop ]->WaveData, wave_size );

        //���b�N����
        g_WaveControllSE[ loop ]->pDSBuffer->Unlock( pvAudioPtr1, buff_size1,
                                                     pvAudioPtr2, buff_size2 );
    }

}

void exec11_05(TCB* thisTCB)
{
    int loop;
    int key_data[] = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

    for( loop = 0; loop < 4 ; loop++ )
    {
        //���̓L�[�ɍ��킹�Ăr�d���Đ�
        if( g_DownInputBuff & key_data[ loop ] )
        {
             //SE�Ȃ̂ŏ�ɍŏ�����Đ�
             g_WaveControllSE[ loop ]->pDSBuffer->SetCurrentPosition( 0 );
             g_WaveControllSE[ loop ]->pDSBuffer->Play( 0, 0, 0 );
        }
    }
}


////////////////////////////////////////////////////
//�P�P�́|�U �a�f�l�̃X�g���[�~���O�Đ�
////////////////////////////////////////////////////
void init11_06(TCB* thisTCB)
{
    int loop;
    int wave_size;
    //�A�N�Z�X�\�ȃo�b�t�@�̃T�C�Y
    DWORD  buff_size1, buff_size2;
    //WAV�o�b�t�@�A�N�Z�X�|�C���g���i�[����ׂ̃|�C���^
    LPVOID pvAudioPtr1, pvAudioPtr2; 

    //Wave�t�@�C���̏��ƃf�[�^�擾
    wave_size = ReadWaveFileOffset(
                "..\\..\\data\\bgm00.wav",
                &g_WaveControllBGM->WaveFormat,
                g_WaveControllBGM->WaveData,
                0, BGM_READ_SIZE );

    //�擾��������ݒ�
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
    //�o�b�t�@�͗]�T�����ēǂݍ��ݗʂ�3�{��ݒ�
    g_WaveControllBGM->DSBDesc.dwBufferBytes = BGM_READ_SIZE * 3;

    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //�T�E���h�o�b�t�@�쐬
    g_pDSound->CreateSoundBuffer( &g_WaveControllBGM->DSBDesc,
                                  &g_WaveControllBGM->pDSBuffer,
                                  NULL );

    //�ŏ���BGM�Đ�
      //�������ރf�[�^�T�C�Y���������b�N����
    g_WaveControllBGM->pDSBuffer->Lock( 0, BGM_READ_SIZE,
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //�������݈ʒu����o�b�t�@�����b�N
                                        DSBLOCK_FROMWRITECURSOR
                                       );

    //�T�E���h�f�[�^���o�b�t�@�֏�������
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, BGM_READ_SIZE );

    //���b�N����
    g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1,
                                          pvAudioPtr2, buff_size2 );

    //���݂̃J�[�\���ʒu���L�^
    g_WaveControllBGM->pDSBuffer->GetCurrentPosition( NULL, &g_WaveControllBGM->BeforePlayCursor );
    g_WaveControllBGM->WaveFilePtr     = BGM_READ_SIZE;

}

void exec11_06(TCB* thisTCB)
{
    //�A�N�Z�X�\�ȃo�b�t�@�̃T�C�Y
    DWORD  buff_size1, buff_size2;
    //WAV�o�b�t�@�A�N�Z�X�|�C���g���i�[����ׂ̃|�C���^
    LPVOID pvAudioPtr1, pvAudioPtr2; 
    DWORD CurrentWriteCursor;
    RECT font_pos = {224, 192,640,480,};
    int play_size;


    //���݂̃J�[�\�����擾
    g_WaveControllBGM->pDSBuffer->GetCurrentPosition( NULL, &CurrentWriteCursor );

    //�Đ��ς݂̗e�ʂ��擾
    play_size = CurrentWriteCursor - g_WaveControllBGM->BeforePlayCursor;

    if( play_size < 0 )
    {
        play_size = 
          (CurrentWriteCursor + BGM_READ_SIZE * 3) - g_WaveControllBGM->BeforePlayCursor;
    }
    //�Đ����ꂽ�o�b�t�@���w��T�C�Y�ȏ�Ȃ�A�o�b�t�@���X�V
    if( play_size > BGM_READ_SIZE )
    {
        ReadWaveFileOffset(
        "..\\..\\data\\bgm00.wav", 
        &g_WaveControllBGM->WaveFormat,
        g_WaveControllBGM->WaveData,
        g_WaveControllBGM->WaveFilePtr,
        play_size );

        g_WaveControllBGM->WaveFilePtr += play_size;


        //�������ރf�[�^�T�C�Y���������b�N����
        g_WaveControllBGM->pDSBuffer->Lock( 0, play_size,
                                            &pvAudioPtr1, &buff_size1,
                                            &pvAudioPtr2, &buff_size2,
                                           //�������݈ʒu����o�b�t�@�����b�N
                                            DSBLOCK_FROMWRITECURSOR
                                           );

        //WAVE�f�[�^�̏�������
        memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, buff_size1 );

        //�������݈ʒu����������Ă��邩�H
        if( pvAudioPtr2 != NULL )
        {   //�����Ȃ�c��f�[�^����������
            memcpy( pvAudioPtr2, 
                    &g_WaveControllBGM->WaveData[ buff_size1 ],
                    buff_size2 );
        }

        //���b�N����
        g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1,
                                              pvAudioPtr2, buff_size2 );

        //���݂̃J�[�\���ʒu���L�^
        g_WaveControllBGM->BeforePlayCursor = CurrentWriteCursor;
    }

    if( g_DownInputBuff & KEY_Z )
        g_WaveControllBGM->pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    //��~
    if( g_DownInputBuff & KEY_X )
        g_WaveControllBGM->pDSBuffer->Stop();


    g_pFont->DrawText( NULL, "'�y'�L�[�ŋȍĐ�\n'�w'�L�[�ōĐ���~", -1, &font_pos, DT_LEFT, 0xffffffff);

}





////////////////////////////////////////////////////
//�P�P�́|�V �b�c�c�`�Đ�
////////////////////////////////////////////////////
void init11_07(TCB* thisTCB)
{
    //CD�I�[�v��(�J�n)
    mciSendString( "open cdaudio", NULL, 0, NULL );

    //�g���b�N�P�ʂŐ���
    mciSendString("set cdaudio time format tmsf",NULL,0,NULL);

}


void exec11_07(TCB* thisTCB)
{
    RECT font_pos = {160, 192,640,480,};
    char current_track[16];

    g_pFont->DrawText( NULL, "'�y'�L�[  �ȍĐ�\n'�w'�L�[  �Đ���~", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 48;
    g_pFont->DrawText( NULL, "�J�[�\���L�[", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    g_pFont->DrawText( NULL, "��  �b�c�g���C�I�[�v��", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    g_pFont->DrawText( NULL, "�E  �b�c�g���C�N���[�Y", -1, &font_pos, DT_LEFT, 0xffffffff);

    //���݂̃g���b�N���擾
    mciSendString("status cdaudio current track", current_track , sizeof(current_track) , NULL);

    font_pos.top += 16;
    g_pFont->DrawText( NULL, "���݂̃g���b�N", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.left += 160;
    g_pFont->DrawText( NULL, current_track, -1, &font_pos, DT_LEFT, 0xffffffff);


    //�擪����Đ�
    if( g_DownInputBuff & KEY_UP    )mciSendString( "play cdaudio notify", NULL, 0, NULL );

    //CD�g���C�I�[�v��
    if( g_DownInputBuff & KEY_LEFT )mciSendString( "set cdaudio door open", NULL, 0, NULL );

    //CD�g���C�N���[�Y
    if( g_DownInputBuff & KEY_RIGHT)mciSendString( "set cdaudio door closed", NULL, 0, NULL );

    //�Đ�
    if( g_DownInputBuff & KEY_Z    )mciSendString( "play cdaudio", NULL, 0, NULL );

    //��~
    if( g_DownInputBuff & KEY_X    )mciSendString( "stop cdaudio", NULL, 0, NULL );

}





////////////////////////////////////////////////////
//�P�Q�� ���̑�
////////////////////////////////////////////////////

typedef struct CARD_STRUCT{
    CARD_STRUCT*   Prev;
    CARD_STRUCT*   Next;
    int            CardType;
    int            CardNo;
    RECT           CardChipData;
} EX12_CARD_STRUCT;

#define CARD_WIDTH  32         //�J�[�h�̉���
#define CARD_HEIGHT 64         //�J�[�h�̍���
#define CARD_COUNT  13         //�P��ނ�����̃J�[�h����
#define CARD_TYPE   4          //�J�[�h�̎��
#define CARD_TOTAL  52         //�J�[�h�̍��v����
#define CARD_SPADE  0          //�X�y�[�h
#define CARD_CLUB   1          //�N���u
#define CARD_HEART  2          //�n�[�g
#define CARD_DIA    3          //�_�C��

static EX12_CARD_STRUCT CardData[CARD_TOTAL];

////////////////////////////////////////////////////
//�J�[�h�̒ǉ�
////////////////////////////////////////////////////
void CardAdd( EX12_CARD_STRUCT* pCardList, EX12_CARD_STRUCT* pCard, int Index)
{
    int loop;
    EX12_CARD_STRUCT* pCardListOld = pCardList;

    for( loop = 0; loop < Index; loop++ )
    {//�J�[�h�}���ʒu�̌���
        pCardListOld = pCardList;          //�P�O�̃J�[�h���L�^���Ă���
        pCardList = pCardList->Next;
    }

    if( pCardList != NULL )
    {//���X�g�ւ̑}��
        pCard->Prev = pCardList;
        pCard->Next = pCardList->Next;
        pCardList->Next->Prev = pCard;
        pCardList->Next = pCard;
    } else {
     //���X�g�I�[�ʒu�ւ̑}��
        pCardList = pCardListOld;          //�P�O�̃J�[�h�̌���ɑ}������
        pCard->Prev = pCardList;
        pCard->Next = NULL;
        pCardList->Next = pCard;
    }

}
////////////////////////////////////////////////////
//�J�[�h�̍폜
////////////////////////////////////////////////////
EX12_CARD_STRUCT* CardDel( EX12_CARD_STRUCT* pCardList, int Index)
{
    int loop;
    EX12_CARD_STRUCT* pCardListOld = pCardList;
    EX12_CARD_STRUCT* pCardNext;

    for( loop = 0; loop < Index; loop++)
    {//�폜�J�[�h�̌���
        pCardList = pCardList->Next;
    }

    if( Index == 0 )
    {//���X�g�擪����̍폜
        pCardNext = pCardList->Next;
        pCardNext->Prev = NULL;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;

        //�폜�������̃J�[�h�����X�g�̐擪
        return pCardNext;
    }

    if( pCardList->Next != NULL )
    {//���X�g����̍폜
        pCardList->Prev->Next = pCardList->Next;
        pCardList->Next->Prev = pCardList->Prev;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;
    } else {
     //���X�g�I�[�ʒu����̍폜
        pCardList->Prev->Next = NULL;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;
    }

    //���X�g�̐擪�A�h���X
    return pCardListOld ;
}
////////////////////////////////////////////////////
//�J�[�h�̎擾
////////////////////////////////////////////////////
EX12_CARD_STRUCT* CardGet( EX12_CARD_STRUCT* pCardList, int Index)
{
    int loop;

    for( loop = 0; loop < Index; loop++)
    {//�w��ԍ��̃J�[�h�̌���
        pCardList = pCardList->Next;
    }

    return pCardList;
}
////////////////////////////////////////////////////
//�J�[�h�̌���
////////////////////////////////////////////////////
int CardSearch( EX12_CARD_STRUCT* pCardList, int CardType, int CardNo)
{
    int count = 0;

    while(pCardList != NULL)
    {
        if( (pCardList->CardType == CardType) && (pCardList->CardNo == CardNo) )
        {//�J�[�h�𔭌�������Index��Ԃ�
            return count;
        }
        count++;
        pCardList = pCardList->Next;
    };
    //�J�[�h���Ȃ���΁|�P��Ԃ�
    return -1;
}
////////////////////////////////////////////////////
//�J�[�h�̏�����
////////////////////////////////////////////////////
void CardInit( void )
{
    int loopX;
    int loopY;
    //�J�[�h�f�[�^�̏�����
    for (loopY = 0; loopY < CARD_TYPE; loopY++)
    {
        for (loopX = 0; loopX < CARD_COUNT; loopX++)
        {
              //�J�[�h�f�[�^�̏�����
              CardData[ loopY * CARD_COUNT + loopX ].Prev     = NULL;
              CardData[ loopY * CARD_COUNT + loopX ].Next     = NULL;
              CardData[ loopY * CARD_COUNT + loopX ].CardType = loopY;
              CardData[ loopY * CARD_COUNT + loopX ].CardNo   = loopX;
              //�J�[�h�\���f�[�^�̏�����
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.top     = CARD_HEIGHT * loopY;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.bottom  = CARD_HEIGHT * loopY + CARD_HEIGHT;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.left    = CARD_WIDTH  * loopX;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.right   = CARD_WIDTH  * loopX + CARD_WIDTH;
        }
    }

}

////////////////////////////////////////////////////
//�P�Q�́|�P �J�[�h�̊Ǘ�
////////////////////////////////////////////////////

typedef struct{
    EX12_CARD_STRUCT*    MainCard;
} EX12_01_STRUCT;

void init12_01(TCB* thisTCB)
{
    EX12_01_STRUCT* work = (EX12_01_STRUCT*)thisTCB->Work;
    int loop;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //�J�[�h�̏�����
    CardInit();

    //�ŏ��̃J�[�h��o�^
    work->MainCard = &CardData[0];
    //�c��̃J�[�h���J�[�h���X�g�ɓo�^
    for( loop = 1; loop < CARD_TOTAL; loop++)
    {
         CardAdd( work->MainCard , &CardData[ loop ],loop );
    }

}

void exec12_01(TCB* thisTCB)
{

    EX12_01_STRUCT* work = (EX12_01_STRUCT*)thisTCB->Work;
    int index = 0;
    EX12_CARD_STRUCT* pchk_card;
    SPRITE  card;

    pchk_card = work->MainCard;
    while( pchk_card != NULL )
    {
        //�J�[�h�̕\�����W���v�Z
        card.X = (index % CARD_COUNT) * CARD_WIDTH;
        card.Y = (index / CARD_COUNT) * CARD_HEIGHT;
 
        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        index++;
        pchk_card = pchk_card->Next;
    };

}




////////////////////////////////////////////////////
//�P�Q�́|�R �J�[�h���V���b�t������
////////////////////////////////////////////////////

typedef struct{
    EX12_CARD_STRUCT*    MainCard;
} EX12_03_STRUCT;

void init12_03(TCB* thisTCB)
{
    EX12_03_STRUCT* work = (EX12_03_STRUCT*)thisTCB->Work;
    EX12_CARD_STRUCT*    tmpCardList;
    EX12_CARD_STRUCT*    padd_card;
    int card_id;
    int loop;


    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //�J�[�h�̏�����
    CardInit();

    //�V���b�t���p�ɃJ�[�h�����X�g�ɓo�^
    tmpCardList = &CardData[0];
    for( loop = 1; loop < CARD_TOTAL; loop++)
    {
         CardAdd( tmpCardList , &CardData[ loop ],loop );
    }

    //�ŏ��̂P��  ���X�g���痐���ŃJ�[�h���P�������o��
    card_id = rand() % CARD_TOTAL;
    padd_card = CardGet( tmpCardList,card_id );
    tmpCardList = CardDel( tmpCardList    , card_id );
    //�����o�����J�[�h�𐳎��ȃ��X�g�ɓo�^
    work->MainCard = padd_card;

    //�c��̃J�[�h�����l�ɁA�����Ŕ����o���Đ����ȃ��X�g�ɓo�^
    for( loop = 1; loop < CARD_TOTAL - 1; loop++ )
    {
         card_id = rand() % (CARD_TOTAL - loop - 1);         //�w��͈̗͂����𓾂�
         padd_card = CardGet( tmpCardList,card_id );
         tmpCardList = CardDel( tmpCardList    , card_id );
         CardAdd( work->MainCard , padd_card,loop );
    }

    //�Ō�̃J�[�h��o�^
    CardAdd( work->MainCard , tmpCardList, 1 );
}


void exec12_03(TCB* thisTCB)
{
    EX12_03_STRUCT* work = (EX12_03_STRUCT*)thisTCB->Work;
    int index = 0;
    EX12_CARD_STRUCT* pchk_card;
    SPRITE  card;

    pchk_card = work->MainCard;

    //���X�g��H���ăJ�[�h��\��
    while( pchk_card != NULL )
    {
        //�J�[�h�̕\�����W���v�Z
        card.X = (index % CARD_COUNT) * CARD_WIDTH;
        card.Y = (index / CARD_COUNT) * CARD_HEIGHT;
 
        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        index++;
        pchk_card = pchk_card->Next;
    }

}




////////////////////////////////////////////////////
//�P�Q�́|�S ���𔻒肷��ɂ�
////////////////////////////////////////////////////

typedef struct{
    EX12_CARD_STRUCT*    MainCard;
} EX12_04_STRUCT;
//��D�̏�������
#define HAND_CARD_COUNT 5

void EX12_04_CardShuffle( EX12_04_STRUCT* CardWork )
{
    EX12_CARD_STRUCT*    padd_card;
    EX12_CARD_STRUCT*    tmpCardList;
    int card_id;
    int loop;

    //�J�[�h�f�[�^�̏�����
    for (loop = 0; loop < CARD_TOTAL; loop++ )
    {   //�J�[�h�f�[�^�̏�����
        CardData[ loop ].Prev     = NULL;
        CardData[ loop ].Next     = NULL;
    }

    //�V���b�t���p�ɃJ�[�h�����X�g�ɓo�^
    tmpCardList = &CardData[0];
    for( loop = 1; loop < CARD_TOTAL; loop++)
    {
         CardAdd( tmpCardList , &CardData[ loop ],loop );
    }

    //�ŏ��̂P��  ���X�g���痐���ŃJ�[�h���P�������o��
    card_id = rand() % CARD_TOTAL;
    padd_card = CardGet( tmpCardList,card_id );
    tmpCardList = CardDel( tmpCardList    , card_id );
    //�����o�����J�[�h����D�̃��X�g�ɓo�^
    CardWork->MainCard = padd_card;

    //�c��̃J�[�h�����l�ɁA�����Ŕ����o���Ď�D�̃��X�g�ɓo�^
    for( loop = 1; loop < HAND_CARD_COUNT; loop++ )
    {
         card_id = rand() % (CARD_TOTAL - loop - 1);
         padd_card = CardGet( tmpCardList,card_id );
         tmpCardList = CardDel( tmpCardList    , card_id );
         CardAdd( CardWork->MainCard , padd_card,loop );
    }
}

void init12_04(TCB* thisTCB)
{
    EX12_04_STRUCT* work = (EX12_04_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //�J�[�h�̏�����
    CardInit();

    EX12_04_CardShuffle( work );

}


void exec12_04(TCB* thisTCB)
{
    EX12_04_STRUCT* work = (EX12_04_STRUCT*)thisTCB->Work;
    int loop;
    EX12_CARD_STRUCT* pchk_card;
    SPRITE  card;
    RECT font_pos = {160, 192,640,480,};

    char card_NoCount[ CARD_COUNT ];
    char card_TypeCount[ CARD_TYPE ];
    char pair_count = 0;
    char max_count = 0;
    char straight = 0;
    char flush = 0;


    //�L�[���͂ōēx�V���b�t�����Ď�D��z��
    if( g_DownInputBuff & KEY_Z    )
    {
        EX12_04_CardShuffle( work );
    }

    //��D�̕\��
    pchk_card = work->MainCard;
    for( loop = 0; loop < HAND_CARD_COUNT ; loop++ )
    {
        //�J�[�h�̕\�����W���v�Z
        card.X = loop * CARD_WIDTH + 128;
        card.Y = 128;

        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        pchk_card = pchk_card->Next;
    }

    //�`�F�b�N�p�̃J�E���^������������
    for( loop = 0;loop < CARD_TYPE ;loop++ ) card_TypeCount[ loop ] = 0;
    for( loop = 0;loop < CARD_COUNT;loop++ ) card_NoCount[ loop ] = 0;

    //��D�̎�ނ��J�E���g����
    pchk_card = work->MainCard;
    for( loop = 0;loop < HAND_CARD_COUNT ;loop++ )
    {
        card_NoCount[ pchk_card->CardNo ]++;
        card_TypeCount[ pchk_card->CardType ]++;
        pchk_card = pchk_card->Next;
    }

    //���̏����W�߂�
    for( loop = 0;loop < CARD_COUNT ;loop++ )
    {
         //�y�A�̐����J�E���g����
         if( card_NoCount[ loop ] == 2 )pair_count++;

         //�ő吔�̃J�[�hNo���J�E���g����
         if( card_NoCount[ loop ] > max_count )max_count = card_NoCount[ loop ];

         //�J�[�h������A�����Ă��邩���J�E���g����
         if( loop != 0)     //�ŏ��̂P���ڂ̓J�E���g�o���Ȃ�
         {
             if( card_NoCount[ loop - 1 ] != 0 && card_NoCount[ loop ] != 0 )
             {
                 straight++;
             }
         }
    }
    for( loop = 0;loop < CARD_TYPE ;loop++ )
    {
         //����̃J�[�h���ő剽�����邩���J�E���g����
         if( card_TypeCount[ loop ] > flush )flush = card_TypeCount[ loop ];
    }


    //�W�߂��������Ɏ��ۂ̖��𔻒�

    //�����y�A
    if( pair_count == 1 )
    {
        g_pFont->DrawText( NULL, "�����y�A", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�c�[�y�A
    if( pair_count == 2 )
    {
        g_pFont->DrawText( NULL, "�c�[�y�A", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�X���[�J�[�h
    if( max_count == 3 )
    {
        g_pFont->DrawText( NULL, "�X���[�J�[�h", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�X�g���[�g
    if( straight == 4 )
    {
        g_pFont->DrawText( NULL, "�X�g���[�g", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�t���b�V��
    if( flush == 5 )
    {
        g_pFont->DrawText( NULL, "�t���b�V��", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�t���n�E�X
    if( pair_count == 1 && max_count == 3 )
    {
        g_pFont->DrawText( NULL, "�t���n�E�X", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�t�H�[�J�[�h
    if( max_count == 4 )
    {
        g_pFont->DrawText( NULL, "�t�H�[�J�[�h", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //�X�g���[�g�t���b�V��
    if( straight ==  4 && flush == 5 )
    {
        g_pFont->DrawText( NULL, "�X�g���[�g�t���b�V��", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }



}





////////////////////////////////////////////////////
//�P�Q�́|�T �����_��
////////////////////////////////////////////////////

void exec12_05(TCB* thisTCB)
{
    int*   int_work = thisTCB->Work;
    float* float_work1 = (float*)&thisTCB->Work[1];
    float* float_work2 = (float*)&thisTCB->Work[2];

    RECT font_pos = {160, 192,640,480,};
    char str[256];

//�D���Ȕ͈̗͂���
#define rand_range( a, b ) ( (rand() % ( b - a + 1 )) + a )

//�����_�� �O�`�P�D�O�̊Ԃ̐��l��Ԃ�
#define rand_float() ((float)rand() / (float)RAND_MAX)

//�D���Ȕ͈̗͂���  �����_��
#define rand_range_float( a, b ) ( ( rand_float() * ( b - a )) + a )

    //�L�[���͂ŗ����l���Čv�Z
    if( g_DownInputBuff & KEY_Z    )
    {
         *int_work    = rand_range( 25, 50 );
         *float_work1 = rand_float();
         *float_work2 = rand_range_float( 3, 17 );
    }

    g_pFont->DrawText( NULL, "'�y'�L�[  �ŗ����l���v�Z", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 48;

    g_pFont->DrawText( NULL, "�͈͂Q�T�|�T�O�̊Ԃ̗���", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%d", *int_work );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;


    g_pFont->DrawText( NULL, "�����_�̗���", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%f",*float_work1 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;


    g_pFont->DrawText( NULL, "�͈͂R�|�Q�V�̊Ԃ̗��� �����_", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%f",*float_work2 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;

}





////////////////////////////////////////////////////
//�P�Q�́|�U �G�Ԃ̓���
////////////////////////////////////////////////////

//�ڕW�ƂȂ�|�C���g�̐�
#define POINT_MAX 4
#define START_X   320
#define START_Y   96
#define MOVE_TIME 60.0

typedef struct{
    SPRITE      Sprt;
    BACK_GROUND Bg;
    int         Time;
    int         Point;
    //�ړ����x
    float     AddX;
    float     AddY;
} EX12_06_STRUCT;

void init12_06(TCB* thisTCB)
{
    EX12_06_STRUCT* work = (EX12_06_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\race_bg.png",&g_pTex[1] );

    work->Sprt.X = START_X;
    work->Sprt.Y = START_Y;
}

void exec12_06(TCB* thisTCB)
{
    EX12_06_STRUCT* work = (EX12_06_STRUCT*)thisTCB->Work;
    float direction;
    float distance;
    float posX;
    float posY;
//�`�F�b�N�|�C���g�̍��W�f�[�^
float point_data[][2] = 
{//  X     Y
    {512.0,128.0,},  //1�ӏ���
    {512.0,352.0,},  //2�ӏ���
    { 96.0,352.0,},  //3�ӏ���
    { 96.0,128.0,},  //4�ӏ���
};

    //�`�F�b�N�|�C���g�̃`�F�b�N
    if(work->Time-- <= 0)
    {//��莞�ԂŃ`�F�b�N�|�C���g��ʉ�
        work->Time = MOVE_TIME;

        //�ڕW�|�C���g����̑��΍��W���v�Z
        posX = point_data[work->Point][0] - work->Sprt.X;
        posY = point_data[work->Point][1] - work->Sprt.Y;

        //�ڕW�|�C���g�ւ̕������v�Z
        direction = atan2( posY, posX);
 
        //�ڕW�|�C���g�ւ̋������v�����A���v���Ԃ��瑬�x������o��
        distance = sqrtf( (posX * posX) + (posY * posY) );

        work->AddX = cos( direction ) * distance / MOVE_TIME;
        work->AddY = sin( direction ) * distance / MOVE_TIME;

        //���̃|�C���g
        work->Point++;
        if( work->Point >= POINT_MAX ) work->Point = 0;
    }

    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    //�w�i�ƃ{�[���̕\��
    BGDraw( &work->Bg, 1);
    SpriteDraw( &work->Sprt, 0);
}



////////////////////////////////////////////////////
//�P�Q�́|�V �t����m��
////////////////////////////////////////////////////
//�f�[�^�l�̍ő�l
#define DATA_MAX 6
#define MOVE_SPEED 4
#define START_X   320
#define START_Y   96
//�}�b�v�f�[�^�̑傫���i�s�N�Z���T�C�Y�j
#define DATA_SIZE 32
#define DATA_WIDTH 20
#define DATA_HEIGHT 15
#define BALL_CENTER 16

typedef struct{
    SPRITE      Sprt;
    BACK_GROUND Bg;
    char        ForwardData;
    int         ReverseFlag;
} EX12_07_STRUCT;

void init12_07(TCB* thisTCB)
{
    EX12_07_STRUCT* work = (EX12_07_STRUCT*)thisTCB->Work;

    //�g�p����e�N�X�`���̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\race_bg.png",&g_pTex[1] );

    work->Sprt.X = START_X;
    work->Sprt.Y = START_Y;
}

void exec12_07(TCB* thisTCB)
{
    EX12_07_STRUCT* work = (EX12_07_STRUCT*)thisTCB->Work;
    float old_posX;
    float old_posY;
    char chk_data;
    char reverse_data;
    RECT font_pos = {0,0, 640, 480, };

char map_data[ DATA_HEIGHT ][ DATA_WIDTH ] =
{//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //0
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //1
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //2
{ -1,-1, 4, 4, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 1, 1,-1,-1,-1, },  //3
{ -1,-1, 4, 4, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 1, 1,-1,-1,-1, },  //4
{ -1,-1, 4, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1, },  //5
{ -1,-1, 4, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1, },  //6
{ -1,-1, 4, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1, },  //7
{ -1,-1, 4, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1, },  //8
{ -1,-1, 4, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1, },  //9
{ -1,-1, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1,-1,-1,-1, },  //10
{ -1,-1, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1,-1,-1,-1, },  //11
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //12
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //13
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },  //14
};                                 

    //�ߋ��̍��W��ێ�
    old_posX = work->Sprt.X;
    old_posY = work->Sprt.Y;

    //�L�[���͂ɂ��ړ�
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;


    //���ݍ��W�̃f�[�^���擾
    chk_data = 
        map_data [ (int)(work->Sprt.Y+BALL_CENTER)/DATA_SIZE ]
                 [ (int)(work->Sprt.X+BALL_CENTER)/DATA_SIZE ];

    //�i���֎~��悩�`�F�b�N����
    if(chk_data == -1)
    {//�����i���ł��Ȃ���΁A���W��߂�
        work->Sprt.X = old_posX;
        work->Sprt.Y = old_posY;
        //�f�[�^���ēx�擾
        chk_data = 
            map_data [(int)(work->Sprt.Y+BALL_CENTER)/DATA_SIZE ]
                     [(int)(work->Sprt.X+BALL_CENTER)/DATA_SIZE ];
    }

    //�f�[�^�^�C�v���r���O�i���Ă��邩���`�F�b�N
    if(work->ForwardData == chk_data)
    {//�O�i���̏���
         //���ɑO�i�����Ƃ݂Ȃ��f�[�^�^�C�v��ݒ�
         work->ForwardData++;
         //�f�[�^�l���ő�Ȃ玟�̃`�F�b�N��0
         if(work->ForwardData >= DATA_MAX)
             work->ForwardData = 0;
    }

    //�t���̃`�F�b�N
    reverse_data = work->ForwardData-1;
    if(reverse_data < 0) reverse_data = DATA_MAX-1;

    if(chk_data != reverse_data)
    {//�����Y���G���A�ȊO�ɐi�񂾂�A�t���Ƃ݂Ȃ�
         work->ReverseFlag = true;
    }else{
     //�����łȂ���΋t������
         work->ReverseFlag = false;
    }

    if(work->ReverseFlag)
    {//�t�����̓��b�Z�[�W��\��
        g_pFont->DrawText(
            NULL,"�t�����Ă��܂�", -1, &font_pos, DT_CENTER, 0xffffffff);
    }
    //�w�i�ƃ{�[���̕\��
    BGDraw( &work->Bg, 1);
    SpriteDraw( &work->Sprt, 0);
}




////////////////////////////////////////////////////
//�P�Q�́|�W  ���Ԃ��v��ɂ�
////////////////////////////////////////////////////

typedef struct{
    int   StartTime;
} EX12_08_STRUCT;

void init12_08(TCB* thisTCB)
{
    EX12_08_STRUCT* work = (EX12_08_STRUCT*)thisTCB->Work;

    //�v���J�n�̎��Ԃ��L�^���Ă���
    work->StartTime = timeGetTime();
}

void exec12_08(TCB* thisTCB)
{
#define SECOND 1000
    EX12_08_STRUCT* work = (EX12_08_STRUCT*)thisTCB->Work;
    int time;
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    //�J�n���Ԃ���o�߂������Ԃ��v�Z���Ď擾
    time = timeGetTime() - work->StartTime;

    //�o�ߎ��Ԃ̕\��
    sprintf( str,"�o�ߎ��� %2d.%03d",time / SECOND,time % SECOND);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//�P�Q�́|�X 10�i��16�i�ϊ�
////////////////////////////////////////////////////
void exec12_09(TCB* thisTCB)
{
//�P�O�i������̕ϊ�
#define BASE_RADIX 10
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    int ans = 0;
    int digit = 1;
    int digit_count = 0;
    //�ϊ��p��10�i���f�[�^
    char* in_str = "32579";


    //�P���ځi�E�[�j�����o
    while( in_str[ ++digit_count ] != NULL );

    for(digit_count--; digit_count >= 0; digit_count--)
    {//�e�����ƂɊ�̌����|���Ă��
        ans += (in_str[ digit_count ] - '0') * digit;
        digit *= BASE_RADIX;
    }

    //�ϊ����l�̕\��
    sprintf( str,"�ϊ����l %d",ans);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}


////////////////////////////////////////////////////
//�P�Q�́|�P�O 16�i��10�i�ϊ�
////////////////////////////////////////////////////
void exec12_10(TCB* thisTCB)
{
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    int loop;
    int in_num = 1024;
    char str_buff[11];
    char cnv_tbl[10] = "0123456789";
    int  div_tbl[] = 
{ 1000000000,
   100000000,
    10000000,
     1000000,
      100000,
       10000,
        1000,
         100,
          10,
};

    //�P�U�i���P�O�i�ϊ�
    for( loop=0; loop < 9; loop++ )
    {
        //�P�����ɕ�����֕ϊ�
        str_buff[ loop ] = cnv_tbl[ in_num / div_tbl[ loop ] ];
        if( str_buff[ loop ] != '0' )
        {//�e�����Ƃɏ�������(�]������߂�)
            in_num %= div_tbl[ loop ];
        }
    }
    str_buff[ loop ] = cnv_tbl[ in_num ];
    //�I�[�������������
    str_buff[ loop+1 ] = NULL;

    //�ϊ����l�̕\��
    sprintf( str,"�ϊ����l %s",str_buff);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}


////////////////////////////////////////////////////
//�P�Q�́|�P�P ���W�A���̓x���ϊ�
////////////////////////////////////////////////////

float degree2rad( float degree )
{
    return degree / 180.0 * M_PI;
}

float rad2degree( float rad )
{
    return  rad / M_PI * 180.0;
}







typedef struct{
    void (*InitFunc)(TCB*);
    void (*ExecFunc)(TCB*);
    char* ChapterName;
} chapter_prog;

static chapter_prog chapter_table[]  = 
{
  {init03_SCENE_CTRL, exec03_SCENE_CTRL,"�R�́|SAMPLE", },
  {init03_01, exec03_01, "  �R�́|�O�P",},
  {init03_11, exec03_11, "  �R�́|�P�P",},
  {init03_13, exec03_13, "  �R�́|�P�R",},
  {init03_14, exec03_14, "  �R�́|�P�S",},
  {init03_16, exec03_16, "  �R�́|�P�U",},
  {     NULL, exec03_18, "  �R�́|�P�W",},
  {init03_19, exec03_19, "  �R�́|�P�X",},
  {init03_20, exec03_20, "  �R�́|�Q�O",},
  {     NULL, exec03_22, "  �R�́|�Q�Q",},
  {init03_script, exec03_script, "�R�́|SCRIPT SHOOTING",},
  {init03_27, exec03_27, "  �R�́|�Q�V",},
  {init03_28, exec03_28, "  �R�́|�Q�W",},
  {init03_29, exec03_29, "  �R�́|�Q�X",},
  {     NULL, exec03_30, "  �R�́|�R�O",},
  {init03_script_rpg, exec03_script_rpg, "�R�́|SCRIPT RPG", },

  {     NULL, exec04_01, "  �S�́|�O�P",},
  {     NULL, exec04_05, "  �S�́|�O�T",},
  {     NULL, exec04_07, "  �S�́|�O�V",},
  {     NULL, exec04_09, "  �S�́|�O�X",},
  {     NULL, exec04_11, "  �S�́|�P�P",},
  {init04_12, exec04_12, "  �S�́|�P�Q",},

  {init05_01, exec05_01, "  �T�́|�O�P",},
  {init05_02, exec05_02, "  �T�́|�O�Q",},
  {     NULL, exec05_03, "  �T�́|�O�R",},
  {init05_04, exec05_04, "  �T�́|�O�S",},
  {init05_05, exec05_05, "  �T�́|�O�T",},
  {init05_06, exec05_06, "  �T�́|�O�U",},
  {init05_07, exec05_07, "  �T�́|�O�V",},
  {init05_08, exec05_08, "  �T�́|�O�W",},
  {init05_09, exec05_09, "  �T�́|�O�X",},
  {init05_10, exec05_10, "  �T�́|�P�O",},
  {init05_12, exec05_12, "  �T�́|�P�Q",},
  {init05_13, exec05_13, "  �T�́|�P�R",},
  {init05_14, exec05_14, "  �T�́|�P�S",},
  {init05_15, exec05_15, "  �T�́|�P�T",},
  {init05_16, exec05_16, "  �T�́|�P�U",},
  {init05_17, exec05_17, "  �T�́|�P�V",},
  {init05_18, exec05_18, "  �T�́|�P�W",},
  {init05_19, exec05_19, "  �T�́|�P�X",},
  {init05_20, exec05_20, "  �T�́|�Q�O",},
  {init05_21, exec05_21, "  �T�́|�Q�P",},

  {init06_02, exec06_02, "  �U�́|�O�Q",},
  {init06_03, exec06_03, "  �U�́|�O�R",},
  {init06_04, exec06_04, "  �U�́|�O�S",},
  {init06_05, exec06_05, "  �U�́|�O�T",},
  {init06_06, exec06_06, "  �U�́|�O�U",},
  {init06_07, exec06_07, "  �U�́|�O�V",},
  {init06_08, exec06_08, "  �U�́|�O�W",},
  {init06_09, exec06_09, "  �U�́|�O�X",},
  {init06_10, exec06_10, "  �U�́|�P�O",},
  {init06_11, exec06_11, "  �U�́|�P�P",},
  {init06_12, exec06_12, "  �U�́|�P�Q",},
  {init06_13, exec06_13, "  �U�́|�P�R",},
  {init06_14, exec06_14, "  �U�́|�P�S",},
  {init06_15, exec06_15, "  �U�́|�P�T",},
  {init06_16, exec06_16, "  �U�́|�P�U",},
  {init06_17, exec06_17, "  �U�́|�P�V",},
  {init06_18, exec06_18, "  �U�́|�P�W",},
  {init06_19, exec06_19, "  �U�́|�P�X",},
  {init06_20, exec06_20, "  �U�́|�Q�O",},
  {init06_21, exec06_21, "  �U�́|�Q�P",},
  {init06_22, exec06_22, "  �U�́|�Q�Q",},
  {init06_23, exec06_23, "  �U�́|�Q�R",},
  {init06_24, exec06_24, "  �U�́|�Q�S",},
  {init06_25, exec06_25, "  �U�́|�Q�T",},

  {init07_01, exec07_01, "  �V�́|�O�P",},
  {init07_02, exec07_02, "  �V�́|�O�Q",},
  {init07_03, exec07_03, "  �V�́|�O�R",},
  {init07_04, exec07_04, "  �V�́|�O�S",},
  {init07_05, exec07_05, "  �V�́|�O�T",},
  {init07_06, exec07_06, "  �V�́|�O�U",},
  {init07_07, exec07_07, "  �V�́|�O�V",},
  {init07_08, exec07_08, "  �V�́|�O�W",},
  {init07_09, exec07_09, "  �V�́|�O�X",},
  {init07_10, exec07_10, "  �V�́|�P�O",},
  {init07_11, exec07_11, "  �V�́|�P�P",},
  {init07_12, exec07_12, "  �V�́|�P�Q",},
  {init07_13, exec07_13, "  �V�́|�P�R",},
  {init07_14, exec07_14, "  �V�́|�P�S",},
  {init07_16, exec07_16, "  �V�́|�P�U",},
  {init07_17, exec07_17, "  �V�́|�P�V",},
  {     NULL, exec07_18, "  �V�́|�P�W",},

  {init08_01, exec08_01, "  �W�́|�O�P",},
  {init08_02, exec08_02, "  �W�́|�O�Q",},
  {init08_03, exec08_03, "  �W�́|�O�R",},
  {init08_04, exec08_04, "  �W�́|�O�S",},
  {init08_05, exec08_05, "  �W�́|�O�T",},
  {init08_06, exec08_06, "  �W�́|�O�U",},
  {init08_07, exec08_07, "  �W�́|�O�V",},
  {init08_09, exec08_09, "  �W�́|�O�X",},
  {init08_10, exec08_10, "  �W�́|�P�O",},
  {init08_11, exec08_11, "  �W�́|�P�P",},
  {init08_12, exec08_12, "  �W�́|�P�Q",},
  {init08_13, exec08_13, "  �W�́|�P�R",},
  {init08_15, exec08_15, "  �W�́|�P�T",},

  {init09_01, exec09_01, "  �X�́|�O�P",},
  {init09_02, exec09_02, "  �X�́|�O�Q",},
  {init09_06, exec09_06, "  �X�́|�O�U",},
  {init09_07, exec09_07, "  �X�́|�O�V",},
  {init09_08, exec09_08, "  �X�́|�O�W",},
  {init09_09, exec09_09, "  �X�́|�O�X",},
  {init09_10, exec09_10, "  �X�́|�P�O",},
  {init09_11, exec09_11, "  �X�́|�P�P",},

  {init10_02, exec10_02, "�P�O�́|�O�Q",},
  {init10_03, exec10_03, "�P�O�́|�O�R",},
  {     NULL, exec10_06, "�P�O�́|�O�U" },

  {init11_02, exec11_02, "�P�P�́|�O�Q" },
  {init11_03, exec11_03, "�P�P�́|�O�R" },
  {init11_05, exec11_05, "�P�P�́|�O�T" },
  {init11_06, exec11_06, "�P�P�́|�O�U" },
  {init11_07, exec11_07, "�P�P�́|�O�V" },

  {init12_01, exec12_01, "�P�Q�́|�O�P" },
  {init12_03, exec12_03, "�P�Q�́|�O�R" },
  {init12_04, exec12_04, "�P�Q�́|�O�S" },
  {     NULL, exec12_05, "�P�Q�́|�O�T" },
  {init12_06, exec12_06, "�P�Q�́|�O�U" },
  {init12_07, exec12_07, "�P�Q�́|�O�V" },
  {init12_08, exec12_08, "�P�Q�́|�O�W" },
  {     NULL, exec12_09, "�P�Q�́|�O�X" },
  {     NULL, exec12_10, "�P�Q�́|�P�O" },
};

#define FUNC_COUNT (sizeof(chapter_table) / sizeof(chapter_prog ) )

void GameMain(TCB* thisTCB)
{
static int select_chapter = 0;
    char str[128];
    RECT font_pos = {0,0, 640, 480, };

    //���j���[�I��
    if( g_DownInputBuff & KEY_RIGHT )
    {
        select_chapter++;
        if(select_chapter >= FUNC_COUNT) select_chapter = 0;

    }
    if( g_DownInputBuff & KEY_LEFT )
    {
        select_chapter--;
        if(select_chapter < 0) select_chapter = FUNC_COUNT-1;
    }

    if( g_DownInputBuff & KEY_Z     )
    {
        if(chapter_table[select_chapter].InitFunc != NULL)
            //�J�n���ɂP�x�������s����֐�
            chapter_table[select_chapter].InitFunc(thisTCB);
        //���t���[�����s����֐�
        TaskChange(thisTCB,chapter_table[select_chapter].ExecFunc);
    }

    font_pos.top  = SCREEN_HEIGHT/2;
    sprintf( str,"%s",chapter_table[ select_chapter ].ChapterName);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_CENTER, 0xffffffff);

    font_pos.top  = SCREEN_HEIGHT/3 * 2;
    g_pFont->DrawText( NULL, "�����L�[�Ń��j���[�I��\n�y�L�[�Ŏ��s", -1, &font_pos, DT_CENTER, 0xffffffff);

}

