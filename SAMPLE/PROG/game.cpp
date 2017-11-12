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
//共通
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

    pos.x = pspr->X;    //座標の設定
    pos.y = pspr->Y;
    pos.z = 0;

    //スプライトの描画
    g_pSp->Draw( g_pTex[bitmap_id],pspr->SrcRect,NULL,&pos,0xffffffff);
}

typedef struct{
    int             X;
    int             Y;
} BACK_GROUND;

void BGDraw(BACK_GROUND* bg,int bitmap_id)
{
    D3DXVECTOR3 pos;

    pos.x = bg->X;    //座標の設定
    pos.y = bg->Y;
    pos.z = 0;

    //背景のの描画
    g_pSp->Draw( g_pTex[bitmap_id],NULL,NULL,&pos,0xffffffff);
}


#define FONT_SIZE_X 16
#define FONT_SIZE_Y 16

void FontPrint(int x, int y,unsigned char* str)
{
    RECT rect;
    unsigned char alphabet;
    int strx,stry;
    int dispx,dispy;            //表示座標
    D3DXVECTOR3 pos;

    //表示座標を保存
    dispx = x;
    dispy = y;

    while(*str != NULL)
    {
        alphabet = *str++;

        if(alphabet == 0x0a)    //改行処理
        {
            dispx = x;
            dispy += FONT_SIZE_Y;
            continue;
        }

        //文字の表示処理
        alphabet -= ' ';
        strx = (alphabet % 0x10) * FONT_SIZE_X;   //元のビットマップ座標を計算
        stry = (alphabet / 0x10) * FONT_SIZE_Y;
        rect.top    = stry;
        rect.bottom = stry + FONT_SIZE_Y;
        rect.left   = strx;
        rect.right  = strx + FONT_SIZE_X;

        pos.x = dispx;
        pos.y = dispy;
        pos.z = 0;
        g_pSp->Draw( g_pTex[FONT_BITMAP_ID],&rect,NULL,&pos,0xffffffff); //スプライトで文字表示

        dispx += FONT_SIZE_X;
    }
}





////////////////////////////////////////////////////
//３章
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//３章－４～５
////////////////////////////////////////////////////

#define SCENE_INIT          -1    //初期化
#define SCENE_CHANGE_WAIT   0     //切り替え時の待ち状態
#define SCENE_TITLE         1     //タイトルシーン
#define SCENE_GAME          2     //ゲーム中
#define SCENE_GAME_OVER     3     //ゲームオーバー
#define SCENE_OPTION        4     //オプション画面
#define SCENE_END           5     //シーンが終了した時


typedef struct{
    int         SceneID;
    int         Time;
} EX3_SCENE_STRUCT;
static int   g_SCENE_CHANGE_COMM;

void exec03_SCENE_GAME(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

     FontPrint( 208, 160,"NOW GAME SCENE");
    //一定時間ごとに点滅を繰り返す
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 208, 240,"PUSH 'Z' GAME OVER");
    }

    //シーンの切り替えをシーン管理タスクに伝える
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

    //一定時間ごとに点滅を繰り返す
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 256,"PUSH 'Z' CONTINUE");
      FontPrint( 176, 272,"PUSH 'X' TITLE");
    }

    //シーンの切り替えをシーン管理タスクに伝える
    if( g_DownInputBuff & KEY_Z )
    {
        g_SCENE_CHANGE_COMM = SCENE_GAME;    //ゲームを続行する
        TaskKill(thisTCB);
        return;
    }
    if( g_DownInputBuff & KEY_X )
    {
        g_SCENE_CHANGE_COMM = SCENE_TITLE;  //タイトルへ戻る
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

//ゲーム中のランク
//本来は外部から参照できるようグローバル変数にするか
//関数による処理にする
static int RANK = 2;


    //ランクの増減処理
    if( g_DownInputBuff & KEY_RIGHT ) RANK++;
    if( g_DownInputBuff & KEY_LEFT  ) RANK--;
    //ランク上限値、下限値チェック
    if( RANK >= RANK_MAX ) RANK = RANK_MAX-1;
    if( RANK <  0        ) RANK = 0;



     //表示処理
     FontPrint( 208, 160,"GAME OPTION");
     //ランクの表示
     sprintf( str, "SELECT RANK %d",RANK+1);
     FontPrint( 208, 208,str);

    //一定時間ごとに点滅を繰り返す
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 240,"PUSH 'X' OPTION EXIT");
    }

    //シーンの切り替えをシーン管理タスクに伝える
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

    //一定時間ごとに点滅を繰り返す
    pEX3_SCENEwork->Time++;
    if( pEX3_SCENEwork->Time & 0x10)
    {
      FontPrint( 176, 256,"PUSH 'Z' GAME START");
      FontPrint( 176, 272,"PUSH 'X' OPTION");
    }

    //シーンの切り替えをシーン管理タスクに伝える
    if( g_DownInputBuff & KEY_Z )
    {
        //ゲームを選択
        g_SCENE_CHANGE_COMM = SCENE_GAME;
        TaskKill(thisTCB);
        return;
    }
    if( g_DownInputBuff & KEY_X )
    {
        //オプションを選択
        g_SCENE_CHANGE_COMM = SCENE_OPTION;
        TaskKill(thisTCB);
        return;
    }
}


void init03_SCENE_CTRL(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

    //初期化中の状態
     pEX3_SCENEwork->SceneID = SCENE_INIT;

}

void exec03_SCENE_CTRL(TCB* thisTCB)
{
    EX3_SCENE_STRUCT* pEX3_SCENEwork;
    pEX3_SCENEwork = (EX3_SCENE_STRUCT*)thisTCB->Work;

    switch( pEX3_SCENEwork->SceneID )
    {
        case SCENE_INIT: //初期化中
                pEX3_SCENEwork->SceneID = SCENE_TITLE;
                TaskMake( exec03_SCENE_TITLE, 0x2000 );
                g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                break;
        case SCENE_TITLE: //タイトル画面
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
        case SCENE_GAME: //ゲーム処理
                if( g_SCENE_CHANGE_COMM != SCENE_CHANGE_WAIT )
                {
                    pEX3_SCENEwork->SceneID = SCENE_GAME_OVER;
                    TaskMake( exec03_SCENE_GAME_OVER, 0x2000 );
                    g_SCENE_CHANGE_COMM = SCENE_CHANGE_WAIT;
                }
                break;
        case SCENE_GAME_OVER:  //ゲームオーバー処理
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
        case SCENE_OPTION:  //オプション画面
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
//３章－ＬＩＳＴ １
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
        //ワークに格納されているのは表示する相手のデータポインタ
        pEX3_1pos_data =  (EX3_1_STRUCT*)thisTCB->Work[ loop ];

        //表示座標を取得
        sprt.X = pEX3_1pos_data->sprt.X;
        sprt.Y = pEX3_1pos_data->sprt.Y;
        //座標の変換を行う
        sprt.X = sprt.X / 10 + RADER_POS_X;
        sprt.Y = sprt.Y / 10 + RADER_POS_Y;
        //レーダーキャラをスプライトで表示
        SpriteDraw(&sprt,1);

    }

}



void exec03_01_move(TCB* thisTCB)
{
#define WALL_TOP    0       //壁上座標
#define WALL_BOTTOM 480     //壁下座標
#define WALL_LEFT   0       //壁右座標
#define WALL_RIGHT  640     //壁左座標

    EX3_1_STRUCT* pEX3_1work;
    pEX3_1work = (EX3_1_STRUCT*)thisTCB->Work;


    //もし、進行した先が壁であれば方向を反転させる
    if( (pEX3_1work->sprt.X+32 + pEX3_1work->AddX >= WALL_RIGHT) ||       //＋３２は表示物の大きさを考慮
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
    //使用するテクスチャの読み込み
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
    //レーダー処理の作成
    prader_task = TaskMake(exec03_01_rader, 0x8000);

    //複数表示処理の初期化と起動
    for(loop = 0;loop < EX3_1_MOVE_COUNT; loop++)
    {
        pnew_task = TaskMake(exec03_01_move, 0x2000);
        pnew_work = (EX3_1_STRUCT*)pnew_task->Work;

        //レーダー処理の為に登録
        prader_task->Work[ loop ] = (int)pnew_work;


        pnew_work->AddX  = (((rand() % 100) / 100.0) - 0.5) * ADD_SPEED;
        pnew_work->AddY  = (((rand() % 100) / 100.0) - 0.5) * ADD_SPEED;

        pnew_work->sprt.X = SCREEN_WIDTH / 2 - 32;           //初期座標
        pnew_work->sprt.Y = SCREEN_HEIGHT / 2 - 32;
    }

    TaskKill(thisTCB);
}




////////////////////////////////////////////////////
//３章－１１  パッケージ化
////////////////////////////////////////////////////
typedef struct{
    //先頭からのファイルの位置
    unsigned int   FilePoint;
    //ファイルサイズ
    unsigned int   FileSize;
    //ファイル名
    TCHAR          FileName[ MAX_PATH ];  
} EX03_11_DATA;
#define FILE_MAX    32
#define HEADER_SIZE (sizeof(EX03_11_DATA) * FILE_MAX + sizeof(int))

int EX03_11_archive_data( char* DirName, char* SaveName)
{
    int file_count = 0;    //ファイル数
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    //ファイル先頭からの位置
    unsigned int file_point = HEADER_SIZE;

    FILE* archive_file;
    FILE* read_file;
    //検索するファイル名
    char search_name[MAX_PATH];
    char new_name[MAX_PATH];
    unsigned int copy_byte;
    int loop;
    unsigned int copy_loop;

    //アーカイブファイルのヘッダ部分
static EX03_11_DATA file_header[FILE_MAX];


    //検索するファイル名を作成
    //フォルダ内の全ファイルを指定する
    strcpy(search_name,DirName);
    strcat(search_name,"*.*");

    //最初の１度だけ実行
    hFind = FindFirstFile( search_name, &FindFileData);
    //最初のファイルはフォルダなのではずす
    FindNextFile( hFind, &FindFileData);
    while(true)
    {
        //ファイルを検索
        if( FindNextFile( hFind, &FindFileData) == 0)
        {//エラーチェック
            if( GetLastError () == ERROR_NO_MORE_FILES )
            {//ファイルがもうなければ検索ループを終了
                break;
            }else{
              //そうでなければエラーで戻る
                FindClose(hFind);
                return true ;
            }
        }

        //ヘッダに情報を書き込む
        file_header[ file_count ].FilePoint = file_point;
        file_header[ file_count ].FileSize  = FindFileData.nFileSizeLow;
        //ファイル名のコピー
        strcpy( file_header[ file_count ].FileName, FindFileData.cFileName);
        //格納するファイルの先頭からの位置計算
        file_point += FindFileData.nFileSizeLow;

        //ファイル数をカウントする
        file_count++;
    }
    //検索終了
    FindClose(hFind);

    //バイナリ書き込みモードでまとめるファイルを開く
    archive_file = fopen( SaveName, "w+b");

    //最初にヘッダ部分を記録する
    //ファイル数
    fwrite( &file_count, sizeof(int) , 1, archive_file );
    //ヘッダ本体（ファイル数を記録した分減らす）
    fwrite( file_header, HEADER_SIZE-sizeof(int) , 1, archive_file );

    for(loop=0; loop < file_count; loop++ )
    {//個別のファイルを１つのファイルにコピーする
        strcpy(new_name,DirName);
        strcat(new_name, file_header[loop].FileName);
        read_file = fopen(new_name, "r+b");

        for(copy_loop = 0; copy_loop < file_header[loop].FileSize; copy_loop++)
        {//ファイルサイズ分だけ繰り返す
            fread(  &copy_byte, 1, 1, read_file );
            fwrite( &copy_byte, 1, 1, archive_file );
        }

        fclose( read_file );
    }

    //ファイルを閉じる
    fclose( archive_file );

    //正常終了
    return false;
}

void init03_11(TCB* thisTCB)
{
    //まとめるフォルダ
    char* archive_file  = "..\\..\\data\\EX03_11\\";
    //まとめたデータのファイル名
    char* archive_name = "..\\..\\data\\EX03_11.DAT";

    EX03_11_archive_data( archive_file,archive_name );

}


int EX03_11_getFileCount( char* Buff )
{
    //先頭にファイル数が格納されている
    return  *((int*)Buff);
}

unsigned int EX03_11_getFileSize( char* Buff ,int ID)
{
    //データ配列の先頭をポイントする
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //ファイルサイズを返す
    return data[ID].FileSize;
}

char* EX03_11_getFileName( char* Buff ,int ID)
{
    //データ配列の先頭をポイントする
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //ファイル名へのポインタを返す
    return data[ID].FileName;
}

char* EX03_11_getFilePoint( char* Buff ,int ID)
{
    //データ配列の先頭をポイントする
    EX03_11_DATA* data = (EX03_11_DATA*)(Buff + sizeof(int) );
    //バッファポインタからファイル格納位置へのポインタを算出
    return (char*)(data[ID].FilePoint+(int)Buff);
}



void exec03_11(TCB* thisTCB)
{
    //ファイルの読み込みバッファ（32K)
    char read_buff[1024*32];
    int file_count;
    char* file_name;
    int   file_size;
    char* file_point;
    int loop;
    int archive_size;
    FILE* archive_file;
    FILE* write_file;

    //読み込み処理
    archive_file = fopen( "..\\..\\data\\EX03_11.DAT","r+b");
    fseek( archive_file, 0, SEEK_END );
    //ファイルサイズを取得
    archive_size = ftell( archive_file);
    fseek( archive_file, 0, SEEK_SET );
    //ファイルを全部読み込む
    fread( &read_buff, 1, archive_size, archive_file );
    //読み込み終了後ファイルを閉じる
    fclose( archive_file );


    //ファイル数の取得
    file_count = EX03_11_getFileCount(read_buff);
    for(loop = 0; loop< file_count; loop++)
    {//まとたファイルをバラバラに書き込む
        //書き込みに必要な各種情報を取得
        //ファイル名
        file_name  = EX03_11_getFileName (read_buff,loop);
        //ファイルサイズ
        file_size  = EX03_11_getFileSize (read_buff,loop);
        //格納されているファイルへのポインタ
        file_point = EX03_11_getFilePoint(read_buff,loop);

        write_file = fopen(file_name ,"w+b");
        fwrite( file_point, file_size, 1, write_file);
        fclose( write_file );
    }

    //処理の終了
    TaskKill(thisTCB);

}







////////////////////////////////////////////////////
//３章－１３  ポーズをかける
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //目標のスプライト
} EX03_13_STRUCT;

//ポーズフラグ
int g_EX03_13_PAUSE_FLAG;

void exec03_13_Ball(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //ボールの移動処理部分 ポーズ中には処理しない
    if( !g_EX03_13_PAUSE_FLAG )
    {
        work->Y += MOVE_SPEED;
        if( work->Y  > SCREEN_HEIGHT ) work->Y = 0;    //画面外に到達したら座標を戻す
    }

    //描画部分はポーズ中でも常に描画し続ける
    SpriteDraw(work,0);
}

void init03_13(TCB* thisTCB)
{
#define BALL_COUNT 32
    TCB*    tmp_tcb;
    SPRITE* ball_sprt;
    int loop;


    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //ボールの作成と座標の初期化
    for( loop = 0; loop < BALL_COUNT; loop++)
    {
        tmp_tcb = TaskMake( exec03_13_Ball, 0x1000 );
        ball_sprt = (SPRITE*)tmp_tcb->Work;

        ball_sprt->X = rand() / (RAND_MAX / SCREEN_WIDTH);
        ball_sprt->Y = rand() / (RAND_MAX / SCREEN_HEIGHT);
    }
    //ポーズフラグの初期化
    g_EX03_13_PAUSE_FLAG = 0;
}

void exec03_13(TCB* thisTCB)
{
    //ポーズフラグの処理
    if( g_DownInputBuff & KEY_Z )
    {//ボタンを押すたびにフラグを反転させ、ポーズとポーズ解除を行う
        g_EX03_13_PAUSE_FLAG = !g_EX03_13_PAUSE_FLAG;
    }
}






////////////////////////////////////////////////////
//３章－１４  グループ別にポーズをかける
////////////////////////////////////////////////////

//ポーズフラグ
#define PAUSE_GROUP_A  (1 << 0)
#define PAUSE_GROUP_B  (1 << 1)
#define PAUSE_GROUP_C  (1 << 2)
#define PAUSE_GROUP_D  (1 << 3)
int g_EX03_14_PAUSE_FLAG;

//ボール種類Ａ
void exec03_14_BallA(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //ボールの移動処理部分 ポーズ中には処理しない
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_A) )
    {
        work->Y += MOVE_SPEED;
        if( work->Y  > SCREEN_HEIGHT ) work->Y = 0;    //画面外に到達したら座標を戻す
    }
    //描画部分はポーズ中でも常に描画し続ける
    SpriteDraw(work,0);
}

//ボール種類Ｂ
void exec03_14_BallB(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //ボールの移動処理部分 ポーズ中には処理しない
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_B) )
    {
        work->Y -= MOVE_SPEED;
        if( work->Y  < 0 ) work->Y = SCREEN_HEIGHT;    //画面外に到達したら座標を戻す
    }
    //描画部分はポーズ中でも常に描画し続ける
    SpriteDraw(work,0);
}

//ボール種類Ｃ
void exec03_14_BallC(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //ボールの移動処理部分 ポーズ中には処理しない
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_C) )
    {
        work->X += MOVE_SPEED;
        if( work->X  > SCREEN_WIDTH ) work->X = 0;    //画面外に到達したら座標を戻す
    }
    //描画部分はポーズ中でも常に描画し続ける
    SpriteDraw(work,0);
}

//ボール種類Ｄ
void exec03_14_BallD(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //ボールの移動処理部分 ポーズ中には処理しない
    if( !(g_EX03_14_PAUSE_FLAG & PAUSE_GROUP_D) )
    {
        work->X -= MOVE_SPEED;
        if( work->X  < 0 ) work->X = SCREEN_WIDTH;    //画面外に到達したら座標を戻す
    }
    //描画部分はポーズ中でも常に描画し続ける
    SpriteDraw(work,0);
}

void init03_14(TCB* thisTCB)
{
#define BALL_COUNT 32
    TCB*    tmp_tcb;
    SPRITE* ball_sprt;
    int loop;


    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //ボールの作成と座標の初期化
    for( loop = 0; loop < BALL_COUNT; loop++)
    {
        switch( loop & 0x03 )    //ボールの種類を４つに分ける
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
    //ポーズフラグの初期化
    g_EX03_14_PAUSE_FLAG = 0;
}

void exec03_14(TCB* thisTCB)
{
    //ポーズフラグの処理、方向キーに合わせてフラグ反転処理
    //各キーを押すたびに、ポーズとポーズ解除を行う

    if( g_DownInputBuff & KEY_UP )
    {//上
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_A;
    }

    if( g_DownInputBuff & KEY_DOWN )
    {//下
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_B;
    }

    if( g_DownInputBuff & KEY_LEFT )
    {//左
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_C;
    }

    if( g_DownInputBuff & KEY_RIGHT )
    {//右
        g_EX03_14_PAUSE_FLAG ^= PAUSE_GROUP_D;
    }
}





////////////////////////////////////////////////////
//３章－１６  現在のＦＰＳを知る
////////////////////////////////////////////////////
#define SECOND 1000.0
static int g_EX03_16_FramePerSecond;

void CALLBACK EX03_16_time_callback( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
static float record_time;
    //１秒間に更新されたフレーム数を記録
    g_EX03_16_FramePerSecond = g_Count - record_time;
    record_time = g_Count;
}

void init03_16(TCB* thisTCB)
{
    //１秒毎に呼び出す関数を設定する
    SetTimer( NULL, NULL, SECOND, EX03_16_time_callback );

    g_EX03_16_FramePerSecond = 0;
}

void exec03_16(TCB* thisTCB)
{
    float fps;
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    //ＦＰＳの表示
    sprintf( str,"現在のＦＰＳ %d",g_EX03_16_FramePerSecond);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//３章－１８  得点の管理
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
    {//負数なら特殊スコアとしてスコア計算
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

    //Ｚキーでスコアを加算
    if( g_DownInputBuff & KEY_Z ) work->Score += EX03_18_get_score( work->ScoreID );
    //ＸキーでIDを切り返る
    if( g_DownInputBuff & KEY_X )
    {
        work->ScoreID++;
        work->ScoreID &= 0x07;
    }

    //スコアの表示
    sprintf( str,"SCORE: %8d  ID%d\n加算スコア：%8d",work->Score,work->ScoreID,EX03_18_get_score( work->ScoreID ));
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//３章－１９  単純なセーブ・ロード
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

    //バイナリ書き込みモードでファイルをオープン
    save_file = fopen( FILE_NAME, "w+b");

    //スコアデータをセーブする
    fwrite( SaveData, sizeof( EX03_19_STRUCT ) , RANK_COUNT, save_file );

    //ファイルを閉じる
    fclose( save_file );
}

int EX03_19_score_load( char* FileName, EX03_19_STRUCT* LoadData )
{
    FILE* load_file;

    //バイナリ読み込みモードでファイルをオープン
    load_file = fopen( FileName, "r+b");

    //もしファイルがなければエラーを返す
    if( load_file == NULL )return true;

    //スコアデータをロードする
    fread( LoadData, sizeof( EX03_19_STRUCT ) ,RANK_COUNT, load_file );

    //ファイルを閉じる
    fclose( load_file );

    //正常終了
    return false;
}

void init03_19(TCB* thisTCB)
{
    //初期化時にスコアをロードを試みて、ファイルがなければデフォルトのデータを読み込む
    if( EX03_19_score_load( DATA_FILE_NAME, gEX03_19_SAVE_DATA ) )
    {//デフォルトのスコアをロードする
        EX03_19_score_load( "EX03_19_DEFAULT_DATA.DAT", gEX03_19_SAVE_DATA );
    }
}

void exec03_19(TCB* thisTCB)
{
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //Ｚキーでスコアをロード
    if( g_DownInputBuff & KEY_Z ) EX03_19_score_load( DATA_FILE_NAME, gEX03_19_SAVE_DATA );
    //Ｘキーでスコアをセーブ
    if( g_DownInputBuff & KEY_X ) EX03_19_score_save( DATA_FILE_NAME, gEX03_19_SAVE_DATA );


    //方向キーでスコアの増減を行なう
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

    //スコアデータの表示
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        sprintf( str,"RANK%d: %8d  %s",loop+1, gEX03_19_SAVE_DATA[ loop ].Score,gEX03_19_SAVE_DATA[ loop ].Name);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 32;
    }

        font_pos.top += 32;
        g_pFont->DrawText( NULL,"Ｚキーでロード、Ｘキーでセーブ\n方向キーでスコア変更", -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//３章－２０  少し複雑なセーブ・ロード
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

    //バイナリ書き込みモードでファイルをオープン
    save_file = fopen( FILE_NAME, "w+b");

    //データをセーブする
    fwrite( SaveData, sizeof( EX03_20_STRUCT ) , 1, save_file );

    //ファイルを閉じる
    fclose( save_file );
}

void EX03_20_load( char* FILE_NAME, EX03_20_STRUCT* LoadData )
{
    FILE* load_file;

    //バイナリ読み込みモードでファイルをオープン
    load_file = fopen( FILE_NAME, "r+b");

    //データをロードする
    fread( LoadData, sizeof( EX03_20_STRUCT ) , 1, load_file );

    //ファイルを閉じる
    fclose( load_file );
}

int EX03_20_load_update( void )
{
    int loop;
    //ロード後にデータをプログラムに反映させる処理の例
//  SetMyShipCount( gEX03_20_SAVE_DATA.MyShip );
//  SetDifficulty ( gEX03_20_SAVE_DATA.Difficulty );

    //スコアの整列を調べる（簡単なチェック）
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
    //セーブ前にデータを更新しておく処理
}

void init03_20(TCB* thisTCB)
{
    //デフォルトのデータをロードする
    EX03_20_load( "EX03_20_DEFAULT_DATA.DAT", &gEX03_20_SAVE_DATA );

}

void exec03_20(TCB* thisTCB)
{
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //Ｚキーでロード
    if( g_DownInputBuff & KEY_Z )
    {//ロード
        EX03_20_load( DATA_FILE_NAME, &gEX03_20_SAVE_DATA );
     //ロード後に、処理を行ない、エラーならデフォルトデータを読み込む
        if( EX03_20_load_update() )
          EX03_20_load( "EX03_20_DEFAULT_DATA.DAT", &gEX03_20_SAVE_DATA );
    }

    //Ｘキーでセーブ
    if( g_DownInputBuff & KEY_X )
    {//セーブ前に、データの更新処理
        EX03_20_save_update();
     //セーブ
        EX03_20_save( DATA_FILE_NAME, &gEX03_20_SAVE_DATA );
    }


    //スコアデータの表示
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        sprintf( str,"RANK%d: %8d  %s",loop+1, gEX03_20_SAVE_DATA.ScoreData[ loop ].Score,gEX03_20_SAVE_DATA.ScoreData[ loop ].Name);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 32;
    }

        font_pos.top += 32;
        g_pFont->DrawText( NULL,"Ｚキーでロード、Ｘキーでセーブ", -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//３章－２２  データを暗号化する
////////////////////////////////////////////////////
void exec03_22(TCB* thisTCB)
{
#define NUM_DATA_COUNT 4
#define CODE_KEY 0xAA
#define RAND_CODE_KEY 0x10
    //元データ
    unsigned char  in_num_data[]  = {0x00,0x01,0x02,0x03,};
    //暗号化データ格納用変数
    unsigned char  encode_num_data1[ NUM_DATA_COUNT ];
    unsigned char  encode_num_data2[ NUM_DATA_COUNT ];
    //復号化データ格納用変数
    unsigned char  decode_num_data1[ NUM_DATA_COUNT ];
    unsigned char  decode_num_data2[ NUM_DATA_COUNT ];

    char  str[128];
    unsigned char  code_key;
    int  loop;
    RECT font_pos = {  0,  0,640,480,};


    //暗号化処理１
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//暗号化の演算を行なう
        encode_num_data1[ loop ] = in_num_data[ loop ] + CODE_KEY;
    }
    //復号化処理１
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//暗号化とは逆の演算を行なう
        decode_num_data1[ loop ] = encode_num_data1[ loop ] - CODE_KEY;
    }

    //暗号化処理２
    srand( RAND_CODE_KEY );
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//暗号化の演算を行なう
        encode_num_data2[ loop ] = in_num_data[ loop ] ^ rand() % 255;
    }
    //復号化処理２
    srand( RAND_CODE_KEY );
    for( loop = 0; loop < NUM_DATA_COUNT; loop++)
    {//暗号化とは逆の演算を行なう
        decode_num_data2[ loop ] = encode_num_data2[ loop ] ^ rand() % 255;
    }


    //元データ表示
    sprintf( str,"暗号前データ　：0x%02x 0x%02x 0x%02x 0x%02x",
    in_num_data[0], in_num_data[1], in_num_data[2], in_num_data[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 64;

    //暗号データ１表示
    sprintf( str,"暗号化データ１：0x%02x 0x%02x 0x%02x 0x%02x",
    encode_num_data1[0], encode_num_data1[1], encode_num_data1[2], encode_num_data1[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 32;

    //複合データ１表示
    sprintf( str,"復号化データ１：0x%02x 0x%02x 0x%02x 0x%02x",
    decode_num_data1[0], decode_num_data1[1], decode_num_data1[2], decode_num_data1[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);font_pos.top += 64;


    //暗号データ２表示
    sprintf( str,"暗号化データ２：0x%02x 0x%02x 0x%02x 0x%02x",
    encode_num_data2[0], encode_num_data2[1], encode_num_data2[2], encode_num_data2[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff); font_pos.top += 32;

    //複合データ２表示
    sprintf( str,"復号化データ２：0x%02x 0x%02x 0x%02x 0x%02x",
    decode_num_data2[0], decode_num_data2[1], decode_num_data2[2], decode_num_data2[3]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);


}





////////////////////////////////////////////////////
//３章－２５  簡易スクリプトを作る（シューティング編）
////////////////////////////////////////////////////
typedef struct{
    SPRITE            Sprt;
    int               PC;        //コードの実行位置を示すカウンタ
    unsigned char*    pCode;     //コードデータを格納してあるポインタ
    unsigned char     Code;      //現在実行中の命令
    unsigned char     Data;      //現在実行中の命令の引数
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

    //画面下へ向かって弾の移動
    work->Y += 16;

    if( work->Y > SCREEN_HEIGHT )
    {//画面外に移動したら終了
        TaskKill( thisTCB );
    }else{
     //表示処理
        SpriteDraw( work, 1 );
    }
}


void EX03_script_exec( EX03_SCRIPT_SHOOT* work )
{
    TCB*      shot_tcb;
    SPRITE*   shot_sprt;


    switch ( work->Code )
    {
        case CODE_END:  //処理が終了していた時はコードと引数データの読み込みを行なう
                work->Code = work->pCode[ work->PC++ ];
                work->Data = work->pCode[ work->PC++ ];
                break;

        case CODE_STOP: //指定の時間停止を行なう
                //停止後、データの読み込み処理へ
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_LEFT: //一定時間左へ移動する

                work->Sprt.X -= MOVE_SPEED;
                //移動終了後、データの読み込み処理へ
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_RIGHT://一定時間右へ移動する

                work->Sprt.X += MOVE_SPEED;
                //移動終了後、データの読み込み処理へ
                if( --work->Data == 0 ) work->Code = CODE_END;
                break;

        case CODE_SHOOT://弾を発射する
                //弾の作成
                shot_tcb = TaskMake( EX03_script_shot ,0x2000);
                shot_sprt= (SPRITE*)shot_tcb->Work;
                //弾の初期化
                shot_sprt->X = work->Sprt.X;
                shot_sprt->Y = work->Sprt.Y;

                //発射後、データの読み込み処理へ
                work->Code = CODE_END;
                break;

        case CODE_RESET://リセット処理を行なう
                work->Code = CODE_END;
                work->PC   = 0;
                break;
    }

}


void init03_script(TCB* thisTCB)
{
    EX03_SCRIPT_SHOOT* work = (EX03_SCRIPT_SHOOT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //座標と、実行コードの初期化
    work->Sprt.X = SCREEN_WIDTH  / 2;
    work->Sprt.Y = SCREEN_HEIGHT / 3;

    work->Code = CODE_END;

}

void exec03_script(TCB* thisTCB)
{
    EX03_SCRIPT_SHOOT* work = (EX03_SCRIPT_SHOOT*)thisTCB->Work;

unsigned char  script_data[] = 
{//行動イベントデータ
    CODE_LEFT  , 0x10,  //左移動
    CODE_RIGHT , 0x20,  //右移動
    CODE_STOP  , 0x10,  //停止
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_LEFT  , 0x10,  //左移動
    CODE_RIGHT , 0x18,  //右移動
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_LEFT  , 0x18,  //左移動
    CODE_RESET , 0x00,  //行動をリセットし、最初に戻る
};


    //実行するスクリプトデータ
    work->pCode  = script_data;

    //スクリプトの実行
    EX03_script_exec( work );

    //表示
    SpriteDraw( &work->Sprt , 0);
}




////////////////////////////////////////////////////
//３章－２７ 状態と処理の切り替え
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //座標と、実行コードの初期化
    work->ScriptWork.Sprt.X = SCREEN_WIDTH  / 2;
    work->ScriptWork.Sprt.Y = SCREEN_HEIGHT / 3;

    work->ScriptWork.Code = CODE_END;
}

void exec03_27(TCB* thisTCB)
{
    EX03_27_STRUCT* work = (EX03_27_STRUCT*)thisTCB->Work;

unsigned char  script_data_A[] = 
{//行動データA
    CODE_LEFT  , 0x18,  //左移動
    CODE_RIGHT , 0x20,  //右移動
    CODE_STOP  , 0x10,  //停止
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_LEFT  , 0x10,  //左移動
    CODE_RIGHT , 0x18,  //右移動
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_LEFT  , 0x18,  //左移動
    CODE_RESET , 0x00,  //行動をリセットし、最初に戻る
};
unsigned char  script_data_B[] = 
{//行動データB
    CODE_LEFT  , 0x08,  //左移動
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_RIGHT , 0x08,  //右移動
    CODE_SHOOT , 0x00,  //弾の発射
    CODE_RESET , 0x00,  //行動をリセットし、最初に戻る
};

    //管理用のタイマーを加算
    work->Time++;

    if( work->ModeID == MODE_A )
    {//行動モードA
        //実行するスクリプトデータ
        work->ScriptWork.pCode  = script_data_A;

        if(work->Time >= CHANGE_TIME_A)
        {//条件がそろったら処理を切り替える
            work->ModeID = MODE_B;
            work->ScriptWork.Code = CODE_RESET;
            work->ScriptWork.pCode  = script_data_B;
            work->Time = 0;
        }
    }

    if( work->ModeID == MODE_B )
    {//行動モードB
        //実行するスクリプトデータ
        work->ScriptWork.pCode  = script_data_B;

        if(work->Time >= CHANGE_TIME_B)
        {//条件がそろったら処理を切り替える
            work->ModeID = MODE_A;
            work->ScriptWork.Code = CODE_RESET;
            work->ScriptWork.pCode  = script_data_A;
            work->Time = 0;
        }
    }

    //スクリプトの実行
    EX03_script_exec( &work->ScriptWork );

    //表示
    SpriteDraw( &work->ScriptWork.Sprt , 0);
}

////////////////////////////////////////////////////
//３章－２８ キャラ選択画面
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
    //使用するテクスチャの読み込み
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

    //キー入力によるキャラクター選択
    if( g_DownInputBuff & KEY_RIGHT )
    {
        work->SelectID++;
        //正規IDチェック
        if( work->SelectID >= CHR_MAX )work->SelectID = 0;
    }
    if( g_DownInputBuff & KEY_LEFT  )
    {
        work->SelectID--;
        //正規IDチェック
        if( work->SelectID < 0 ) work->SelectID = CHR_MAX-1;
    }

    if( g_DownInputBuff & KEY_Z  )
    {//キャラクターの決定、ここではメッセージの表示のみ
         switch( work->SelectID )
         {
            case 0: strcpy(work->DispMess,"決定したキャラクターはＡです。"); break;
            case 1: strcpy(work->DispMess,"決定したキャラクターはＢです。"); break;
            case 2: strcpy(work->DispMess,"決定したキャラクターはＣです。"); break;
            case 3: strcpy(work->DispMess,"決定したキャラクターはＤです。"); break;
         }
    }

    //キャラクター選択グラフィックの表示
    for( loop = 0; loop < CHR_MAX; loop++ )
    {
        work->ChrGraph.X = chr_pos[loop][0];
        work->ChrGraph.Y = chr_pos[loop][1];
        SpriteDraw(&work->ChrGraph,loop);
    }

    //選択マークの表示
    work->MarkPoint.X = chr_pos[work->SelectID][0];
    work->MarkPoint.Y = chr_pos[work->SelectID][1];

    SpriteDraw(&work->MarkPoint,4);

    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);

}




////////////////////////////////////////////////////
//３章－２９ 隠しプレイヤーキャラを出現させる
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

    //使用するテクスチャの読み込み
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

    //キー入力によるキャラクター選択
    if( g_DownInputBuff & KEY_RIGHT )
    {
        work->SelectID++;
        if( work->SelectID >= HIDE_CHR_E )
        {//隠しキャラクターが表示されているかチェックし
         //表示されているなら、IDチェックの際に考慮する
            //隠しキャラ非表示なら、そのIDは表示しない
            if( (work->SelectID == HIDE_CHR_E) && !work->HideChr_E ) work->SelectID++;
            //次のIDも同様
            if( (work->SelectID == HIDE_CHR_F) && !work->HideChr_F ) work->SelectID++;
        }

        if( work->SelectID >= CHR_MAX )work->SelectID = 0;
    }
    if( g_DownInputBuff & KEY_LEFT  )
    {
        work->SelectID--;
        if( work->SelectID < 0 ) work->SelectID = CHR_MAX-1;
        if( work->SelectID >= HIDE_CHR_E )
        {//減算時も同様、但し逆順にチェックする
            //隠しキャラ非表示なら、そのIDは表示しない
            if( (work->SelectID == HIDE_CHR_F) && !work->HideChr_F ) work->SelectID--;
            //次のIDも同様
            if( (work->SelectID == HIDE_CHR_E) && !work->HideChr_E ) work->SelectID--;
        }
    }

    if( g_DownInputBuff & KEY_X  )
    {//隠しキャラクターＥの表示、非表示を切り替える
        work->HideChr_E = !work->HideChr_E;
        work->SelectID = 0;
    }

    if( g_DownInputBuff & KEY_Z  )
    {//隠しキャラクターＦの表示、非表示を切り替える
        work->HideChr_F = !work->HideChr_F;
        work->SelectID = 0;
    }

    //キャラクター選択グラフィックの表示
    for( loop = 0; loop < CHR_MAX; loop++ )
    {
        work->ChrGraph.X = chr_pos[loop][0];
        work->ChrGraph.Y = chr_pos[loop][1];

        if( loop >= HIDE_CHR_E)
        {//フラグが立っていた場合のみ、隠しキャラヲ表示する
            if(((loop == HIDE_CHR_E) && work->HideChr_E) |
               ((loop == HIDE_CHR_F) && work->HideChr_F) )
            {
                SpriteDraw(&work->ChrGraph,loop);
            }
        }else{
            SpriteDraw(&work->ChrGraph,loop);
        }
    }

    //選択マークの表示
    work->MarkPoint.X = chr_pos[work->SelectID][0];
    work->MarkPoint.Y = chr_pos[work->SelectID][1];

    SpriteDraw(&work->MarkPoint,6);

}




////////////////////////////////////////////////////
//３章－３０ RPGでの敵の出現
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
    char* map_name[] = { "道","草原","砂漠","山岳",};

    EX03_30_MAP_DATA map_data[] = 
{
    {  8, 10, 0,},    //道
    {  6,  8, 1,},    //草原
    {  4,  5, 2,},    //砂漠
    {  4,  2, 3,},    //山
};


    //キー入力によるエンカウントチェック
    if( g_DownInputBuff & KEY_Z )
    {
        //チェック時に表示メッセージをクリア
        work->MessData = NULL;

        //一定数移動するまでは、敵は出現しない
        if( work->EncountStep == 0 )
        {

            //地形のデータに合わせて乱数を取得し出現確立を計算する
            encount_check = rand() % map_data[work->MapDataIndex].RandRate;

            if( encount_check == 0)
            {//敵の出現処理
                //地形に合わせた敵の出現
                switch( map_data[work->MapDataIndex].EnemyLevel )
                {
                    case MAP_ROAD:    work->MessData = "道の敵が出現しました"; break;
                    case MAP_GRASS:   work->MessData = "草原の敵が出現しました"; break;
                    case MAP_DESERT:  work->MessData = "砂漠の敵が出現しました"; break;
                    case MAP_MOUNTAIN:work->MessData = "山岳の敵が出現しました"; break;
                }
                //次に出現するまでの最低移動数を設定
                work->EncountStep = map_data[work->MapDataIndex].EncountStep;
            }
        } else {
            //エンカウント可能な歩数を一つ減らす
            work->EncountStep--;
        }
    }

    //チェックする地形データの切り替え
    if( g_DownInputBuff & KEY_X  )
    {
        //マップデータは4種類
        if(++work->MapDataIndex == 4) work->MapDataIndex = 0;
    }

    //出現時メッセージの表示
    if(work->MessData != NULL) g_pFont->DrawText( NULL, work->MessData, -1, &font_pos, DT_LEFT, 0xffffffff);

    font_pos.top += 16;
    sprintf( str,"現在の地形データ:  %s",map_name[work->MapDataIndex]);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    font_pos.top += 32;
    g_pFont->DrawText( NULL, "Ｚキーで敵の出現チェック、Ｘキーでデータ切り替え", -1, &font_pos, DT_LEFT, 0xffffffff);

}




////////////////////////////////////////////////////
//３章－３２  簡易スクリプトを作る（アドベンチャー・RPG編）
////////////////////////////////////////////////////
typedef struct{
    unsigned char     FlagArea[16];
    char              MessBuff[128]; //文字列格納用のバッファ
} EX03_SCRIPT_RPG;

typedef struct{
    unsigned char     Code;          //実行する命令
    unsigned char     Data1;         //実行する命令の１番目の引数
    unsigned char     Data2;         //実行する命令の２番目の引数
    char*             Message;       //表示用の文字列
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
    int               flag_pos;      //フラグの位置
    unsigned char     data1;         //現在実行中の命令の引数
    unsigned char     data2;         //現在実行中の命令の引数
    unsigned char*    pStr;          //表示用文字列へのポインタ

    while( true ){
        //データを読み出す
        data1 = pCode->Data1;
        data2 = pCode->Data2;
        pStr  = pCode->Message;

        switch ( pCode->Code )
        {//スクリプト命令を実行
            case CODE_END:  //スクリプト処理の終了
                    return;
                    break;

            case CODE_FLAG_SET: //フラグを立てる
                    //バイト中のフラグ位置を計算
                    flag_pos = data1 & 0x03;
                    //フラグセット
                    work->FlagArea[ data1 / 8 ]  |= 1 << flag_pos;
                    break;

            case CODE_FLAG_CLR: //フラグを消去
                    //バイト中のフラグ位置を計算
                    flag_pos = data1 & 0x03;
                    //フラグクリア
                    work->FlagArea[ data1 / 8 ]  &= ~(1 << flag_pos);
                    break;

            case CODE_JUMP_ON: //フラグをチェックし、立っていたら処理を分岐させる
                    //バイト中のフラグ位置を計算
                    flag_pos = data1 & 0x03;
                    //フラグのチェックと分岐
                    if( work->FlagArea[ data1 / 8 ]  & (1 << flag_pos) ) pCode += data2-1;
                    break;

            case CODE_JUMP_OFF: //フラグをチェックし、立っていなければ処理を分岐させる
                    //バイト中のフラグ位置を計算
                    flag_pos = data1 & 0x03;
                    //フラグのチェックと分岐
                    if( !(work->FlagArea[ data1 / 8 ]  & (1 << flag_pos)) ) pCode += data2-1;
                    break;

            case CODE_PRINT://メッセージを表示する（外部への情報出力）
                    //実際はバッファにコピーするだけで、表示は呼び出し側で行なう
                    strcpy( work->MessBuff, pStr );
                    break;
        }

        //処理するスクリプトデータを１つ進める
        pCode++;
    }

}


void init03_script_rpg(TCB* thisTCB)
{
    EX03_SCRIPT_RPG* work = (EX03_SCRIPT_RPG*)thisTCB->Work;

EX03_SCRIPT_RPG_DATA script_data_init[] = 
{//初期化データ
    {CODE_PRINT   ,0,0,"これは初期化時に表示されるメッセージです。\nまずはＺキーを押してください",},
    {CODE_END     ,},
};

    //初期化用のスクリプトで初期化
    EX03_script_rpg_exec( work ,script_data_init );

}

void exec03_script_rpg(TCB* thisTCB)
{
    EX03_SCRIPT_RPG* work = (EX03_SCRIPT_RPG*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
//フラグ種類決定
#define FLAG_Z    0
#define FLAG_UP   1
#define FLAG_X    2

EX03_SCRIPT_RPG_DATA script_data_Z[] = 
{//Ｚキーイベントデータ
    {CODE_JUMP_ON ,FLAG_Z, 4},
    {CODE_PRINT   ,0,0,"フラグを立てました。次に方向キーの上を押してください。",},
    {CODE_FLAG_SET,FLAG_Z,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"すでにフラグは立っています。",},
    {CODE_END     ,},
};

EX03_SCRIPT_RPG_DATA script_data_UP[] = 
{//方向キー上イベントデータ
    {CODE_JUMP_ON,FLAG_Z, 3},
    {CODE_PRINT   ,0,0,"Ｚキーがまだ押されていません。\n先にＺキーを押してください。",},
    {CODE_END     ,},

    {CODE_JUMP_ON ,FLAG_UP, 4},
    {CODE_PRINT   ,0,0,"フラグを立てました。\n最後にＸキーを押してください。",},
    {CODE_FLAG_SET,FLAG_UP,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"すでにフラグは立ちましたので、このキーを押す必要はありません。",},
    {CODE_END     ,},
};

EX03_SCRIPT_RPG_DATA script_data_X[] = 
{//Ｘキーイベントデータ
    {CODE_JUMP_ON,FLAG_UP, 3},
    {CODE_PRINT   ,0,0,"このキーを押す条件が満たされていません。\n上キーを押してみてください。",},
    {CODE_END     ,},

    {CODE_JUMP_ON ,FLAG_X, 4},
    {CODE_PRINT   ,0,0,"最後のフラグを立てました。\n以上で処理は終了です。",},
    {CODE_FLAG_SET,FLAG_X,},
    {CODE_END     ,},

    {CODE_PRINT   ,0,0,"すでに処理は終了しました。",},
    {CODE_END     ,},
};



    //各キー入力をイベント代わりにして、スクリプトを実行する
    //Ｚキーを押した時の処理
    if( g_DownInputBuff & KEY_Z ) EX03_script_rpg_exec( work ,script_data_Z );
    //Ｘキーを押した時の処理
    if( g_DownInputBuff & KEY_X ) EX03_script_rpg_exec( work ,script_data_X );
    //方向キー上を押した時の処理
    if( g_DownInputBuff & KEY_UP) EX03_script_rpg_exec( work ,script_data_UP );

    //メッセージ文字の表示
    g_pFont->DrawText( NULL, work->MessBuff, -1, &font_pos, DT_LEFT, 0xffffffff);

}





////////////////////////////////////////////////////
//４章　インターフェース
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//４章－１ Windowsでキーボードを使う
////////////////////////////////////////////////////

void exec04_01(TCB* thisTCB)
{
    unsigned char KeyboardBuff[256];
    unsigned char InputBuff;

    GetKeyboardState(KeyboardBuff);    //キーボード情報の取得
    InputBuff = 0;
    if( KeyboardBuff[VK_UP]   & 0x80 ) InputBuff |= 1 << 0;
    if( KeyboardBuff[VK_DOWN] & 0x80 ) InputBuff |= 1 << 1;
    if( KeyboardBuff[VK_LEFT] & 0x80 ) InputBuff |= 1 << 2;
    if( KeyboardBuff[VK_RIGHT]& 0x80 ) InputBuff |= 1 << 3;
    if( KeyboardBuff[ 'Z' ]   & 0x80 ) InputBuff |= 1 << 4;
    if( KeyboardBuff[ 'X' ]   & 0x80 ) InputBuff |= 1 << 5;
}


////////////////////////////////////////////////////
//４章－５ タメ撃ちをするには
////////////////////////////////////////////////////
typedef struct{
        int    accmulate_power_on;        //ボタンＯＮの蓄積量
        int    accmulate_power_off;       //ボタンＯＦＦの蓄積量
} EX04_05_STRUCT ;

void exec04_05_Success(TCB* thisTCB)
{
    FontPrint(128,224,"'X' ACCMULATE SUCCESS!");
    //一定時間後に消去
    thisTCB->Work[0]++;
    if( thisTCB->Work[0] == 50 )
    {
        TaskKill( thisTCB );
    }
}

void exec04_06_Success(TCB* thisTCB)
{
    FontPrint(128,256,"'Z' ACCMULATE SUCCESS!");

    //一定時間後に消去
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

    //Ｘキーを押している間は溜め
    if(g_InputBuff & KEY_X )
    {
        work->accmulate_power_on++;
        if( work->accmulate_power_on > MAX_POWER )
        {
            work->accmulate_power_on = MAX_POWER;
        }
    }

    //離した瞬間のみ処理する
    if(g_UpInputBuff & KEY_X )
    {
        if( work->accmulate_power_on >= MAX_POWER)
        {
            TaskMake(exec04_05_Success,0x2000);
        }
        work->accmulate_power_on = 0;
    }

    //Ｚキーを離している間は溜め
    if(!(g_InputBuff & KEY_Z))
    {
         work->accmulate_power_off++;
         if( work->accmulate_power_off > MAX_POWER )
         {
             work->accmulate_power_off = MAX_POWER;
         }
    }

    //押した瞬間のみ処理する
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
//４章－７ ボタンの同時押しの判定
////////////////////////////////////////////////////

void exec04_07_Success(TCB* thisTCB)
{
    FontPrint(128,224,"BUTTONS SAME PRESS SUCESS!!");
    //一定時間後に消去
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

    //キー状態の履歴を更新
    for( loop = INPUT_HISTORY_COUNT-1; loop>0; loop--)
    {
        work->HistoryInputBuff[loop] = work->HistoryInputBuff[loop-1];
    }
    //最新のキー押下状態を保存
    work->HistoryInputBuff[0] = g_InputBuff;        //１フレーム前のキー状態を保存

    for(loop=0; loop<4; loop++) //過去４フレーム分の履歴をチェック
    {
        same_button_check |= work->HistoryInputBuff[loop];
    }

    //同時押し成功
    if( same_button_check & ((1 << KEY_X)|(1 << KEY_Z)) )
    {
        TaskMake(exec04_07_Success,0x2000);
    }
}



////////////////////////////////////////////////////
//４章－９ 同方向２回押しによる入力判定
////////////////////////////////////////////////////
void exec04_09_Success(TCB* thisTCB)
{
    FontPrint(64,224,"SAME DIRECTION DOUBLE PUSH SUCESS!!");
    //一定時間後に消去
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
        //一定時間内にキーがもう一度入力されていたら入力成功
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
    //連続入力時間のカウント
    if( work->KeyTime[ KEY_INDEX_LEFT  ] != 0 ) work->KeyTime[ KEY_INDEX_LEFT  ]--;
    if( work->KeyTime[ KEY_INDEX_RIGHT ] != 0 ) work->KeyTime[ KEY_INDEX_RIGHT ]--;
}


////////////////////////////////////////////////////
//４章－１１ コマンド入力
////////////////////////////////////////////////////

void exec04_11_Success(TCB* thisTCB)
{
    switch(thisTCB->Work[0]){
         case 0:    FontPrint(144,224,"COMMAND 1 SUCCESSS!!");break;
         case 1:    FontPrint(144,240,"COMMAND 2 SUCCESSS!!");break;
         case 2:    FontPrint(144,256,"COMMAND 3 SUCCESSS!!");break;
    }
    //一定時間後に消去
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
    { {30,},{ KEY_DOWN,  KEY_DOWN|KEY_RIGHT, KEY_RIGHT, 0,}, },   //コマンド１ 下 右下 右
    { {30,},{ KEY_RIGHT, KEY_DOWN,  KEY_DOWN|KEY_RIGHT, 0,}, },   //コマンド２ 右、下、左下
    { {30,},{ KEY_DOWN,  KEY_DOWN|KEY_LEFT,  KEY_LEFT,  0,}, },   //コマンド３ 下、左下、左
    { { 0,},{0,},},   //コマンド終了
};

    EX04_11_STRUCT* work = (EX04_11_STRUCT*)thisTCB->Work;
    TCB* newTCB;
    int check_command = 0;
    int loop;
    unsigned char* key_data;

    FontPrint(192,160,"INPUT COMMAND");


    //キー状態の履歴を更新
    for( loop = HISTORY_COUNT-1; loop>0; loop--)
    {
        work->HistoryBuff[loop] = work->HistoryBuff[loop-1];
    }

    //最新のキー状態を保存
    work->HistoryBuff[0] = g_InputBuff & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT); //方向キーのみ保存

    //Ｚキーが押された時に、コマンドの解析を開始
    if( g_DownInputBuff & KEY_Z )
    {
        //チェックするコマンドが終了するまでループ
        while( command_data[ check_command ].Time )
        {
            //履歴上のキー入力パターンをチェック
            key_data = command_data[ check_command ].Key;
            for(loop = 0;loop < command_data[ check_command ].Time; loop++)
            {
                if( *key_data == work->HistoryBuff[ command_data[ check_command ].Time - 1 - loop] )
                {   //全部のキーパターンが入力されていたらコマンド入力成功
                    key_data++;
                    if(*key_data == 0)break;
                }
            }
            //コマンド入力成功なら、処理を行いチェック終了
            if(*key_data == 0)
            {
                //コマンドを表示するタスクを生成
                newTCB = TaskMake(exec04_11_Success,0x2000);
                newTCB->Work[0] = check_command;
                break;
            }
            check_command++;

        }
    }
}





////////////////////////////////////////////////////
//４章－１２ プレイヤー名前登録
////////////////////////////////////////////////////

#define ALPHABET_NUM    40
//入力可能な名前の文字数
#define NAME_MAX        5
//表示する文字数(奇数にする事）
#define DISP_ALPHABET   11
//文字の表示座標等
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
    //入力した名前、終端文字用に+1
    char    Name[NAME_MAX + 1];
} EX04_12_STRUCT;

void EX04_12_name_display(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;

    //決定した名前の表示
    FontPrint(NAME_X,NAME_Y,work->Name);
}

void init04_12(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\mark32x32.png",&g_pTex[0] );

    //選択マークの表示座標の初期化
    work->Mark.X = SELECT_MARK_X;
    work->Mark.Y = SELECT_MARK_Y;
}

void exec04_12(TCB* thisTCB)
{
    EX04_12_STRUCT* work = (EX04_12_STRUCT*)thisTCB->Work;
    int loop;
    int display_alphabet;
char* alphabet_data[ ALPHABET_NUM ] = 
{//使用する文字と、その並びのデータ
    "A", "B", "C", "D", "E", "F", "G", "H",
    "I", "J", "K", "L", "M", "N", "O", "P",
    "Q", "R", "S", "T", "U", "V", "W", "X",
    "Y", "Z", " ", "0", "1", "2", "3", "4",
    "5", "6", "7", "8", "9", "0", "!", "?",
};

    //文字の選択
    if( g_DownInputBuff & KEY_RIGHT  )
    {//選択文字を範囲内に収める
        work->SelectAlphabetID++;
        if(work->SelectAlphabetID >= ALPHABET_NUM)
            work->SelectAlphabetID -= ALPHABET_NUM;
    }
    if( g_DownInputBuff & KEY_LEFT )
    {//選択文字を範囲内に収める
        work->SelectAlphabetID--;
        if(work->SelectAlphabetID < 0)
            work->SelectAlphabetID += ALPHABET_NUM;
    }

    //選択用文字列の表示
    //初めに表示開始文字を決定
    display_alphabet = work->SelectAlphabetID - (DISP_ALPHABET-1) / 2;
    //同様に範囲内に収める
    if(display_alphabet < 0)
        display_alphabet += ALPHABET_NUM;

    for( loop = 0; loop < DISP_ALPHABET; loop++ )
    {
        FontPrint(
            //表示位置の計算
            SELECT_NAME_X - ((DISP_ALPHABET-1) / 2 * DISPLAY_WIDTH) + loop * DISPLAY_WIDTH,
            SELECT_NAME_Y,
            alphabet_data[ display_alphabet ]
        );

        //表示する文字を更新する
        display_alphabet++;
        if(display_alphabet >= ALPHABET_NUM)
            display_alphabet-= ALPHABET_NUM;
    }

    if( g_DownInputBuff & KEY_Z )
    {//文字の決定
        //決定した文字を名前文字列にコピー
        strcpy( &work->Name[ work->NameCount ],alphabet_data[ work->SelectAlphabetID ] );

        work->NameCount++;
        if(work->NameCount >= NAME_MAX )
        {//規定の入力数を越えたら終了
             //名前入力の終了処理
             //ここでは、名前だけを表示する処理に切り替えています
             TaskChange( thisTCB,EX04_12_name_display );
        }
    }

    if( g_DownInputBuff & KEY_X )
    {//文字のキャンセル
        //決定した文字を１文字もどす
        work->NameCount--;
        //ただし０以下にはしない
        if(work->NameCount < 0 ) work->NameCount = 0;
        //文字の消去
        work->Name[ work->NameCount ] = NULL;
    }

    //決定した名前の表示
    FontPrint(NAME_X,NAME_Y,work->Name);

    //選択マークの表示（位置は固定
    SpriteDraw( &work->Mark, 0);
}



////////////////////////////////////////////////////
//５章  グラフィックの表示
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//５章－１ キャラを表示する
////////////////////////////////////////////////////
void init05_01(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_01(TCB* thisTCB)
{
    D3DXVECTOR3 pos;

    pos.x = 320;    //座標の設定
    pos.y = 240;
    pos.z = 0;

    //スプライトの描画
    g_pSp->Draw( g_pTex[0],NULL,NULL,&pos,0xffffffff);
}

////////////////////////////////////////////////////
//５章－２ たくさんのキャラを表示する
////////////////////////////////////////////////////
void init05_02(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0011.png",&g_pTex[0] );
}

void exec05_02(TCB* thisTCB)
{
    D3DXVECTOR3 pos;
    int i;
static float posdata[3][2] = {
        { 320, 240, },            //０番目のキャラクターの表示座標
        { 160, 360, },            //１番目のキャラクターの表示座標
        { 448, 112, },            //２番目のキャラクターの表示座標
     };

    for( i=0; i<3; i++){
        pos.x = posdata[i][0];
        pos.y = posdata[i][1];
        g_pSp->Draw( g_pTex[0],NULL,NULL,&pos,0xffffffff);
    }
}



////////////////////////////////////////////////////
//５章－３ メッセージの表示を管理する
////////////////////////////////////////////////////

void exec05_03(TCB* thisTCB)
{
#define DISP_SPEED 5
typedef struct{
    int           StrPoint;
    int           Time;
} EX05_03_STRUCT ;

char sample_mess[] = 
    "これはメッセージの表示を管理するプログラムの、\n"
    "テスト用文字列です。\n"
    "このプログラムを基本に色々変更してみて下さい。";

    EX05_03_STRUCT* work = (EX05_03_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    int mess_count;

    switch( sample_mess[work->StrPoint] )
    {
        case  '\n':  //改行時の処理 Ｚキーで次行へ
            if( g_DownInputBuff & KEY_Z  )
            {
                work->StrPoint += 1;
                work->Time = 0;
            }
            mess_count = work->StrPoint;
            break;
        case  NULL:   //文字列終了
            mess_count = -1;
            break;

        default: //通常文字の処理
            //一定時間経過後、次の文字へ
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

    //Ｘキーで最初から表示
    if( g_DownInputBuff & KEY_X  )
    {
        work->StrPoint = 0;
        work->Time     = 0;
    }

}






////////////////////////////////////////////////////
//５章－４ 背景の表示
////////////////////////////////////////////////////

void init05_04(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_04(TCB* thisTCB)
{
    BACK_GROUND* bg;
    bg = (BACK_GROUND*)thisTCB->Work;

    BGDraw(bg,0);      //背景の描画
}



////////////////////////////////////////////////////
//５章－５ 背景との当たり判定
////////////////////////////////////////////////////

typedef struct{
    BACK_GROUND    Bg;
    SPRITE        Cursor;
} EX05_05_STRUCT ;

void init05_05(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\HIT_CHECK.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\CURSOR.png",&g_pTex[1] );
}

void exec05_05(TCB* thisTCB)
{
#define HIT_ACC 32          //当たり判定の精度

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
    //背景の表示 
    BGDraw( &bg, 0 );

    //カーソルの移動
    if( g_InputBuff & KEY_UP    ) work->Cursor.Y -= 8;
    if( g_InputBuff & KEY_DOWN  ) work->Cursor.Y += 8;
    if( g_InputBuff & KEY_RIGHT ) work->Cursor.X += 8;
    if( g_InputBuff & KEY_LEFT  ) work->Cursor.X -= 8;

    SpriteDraw( &work->Cursor, 1 );

    //背景接触データのチェック
    check_x =  work->Cursor.X / HIT_ACC;
    check_y =  work->Cursor.Y / HIT_ACC;

    //データ範囲外の場合エラー
    if( check_x >= SCREEN_WIDTH  / HIT_ACC) err = TRUE;
    if( check_y >= SCREEN_HEIGHT / HIT_ACC) err = TRUE;

    //接触判定
    if( !err ) //エラーの場合はチェックしない
    {
        if( check_data[ check_y ][ check_x ] )
        {

            FontPrint(5,15,"HIT BACK GROUND");
        }
    }

}


////////////////////////////////////////////////////
//５章－６ 背景のスクロール
////////////////////////////////////////////////////
void init05_06(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_06(TCB* thisTCB)
{
#define SCROLL_SPEED 2
    BACK_GROUND* bg;
    bg = (BACK_GROUND*)thisTCB->Work;

    //スクロールは逆方向に進む
    bg->X -= SCROLL_SPEED;
    bg->X %= SCREEN_WIDTH;
    bg->Y = 0;

    BGDraw(bg,0);    //背景の描画
}

////////////////////////////////////////////////////
//５章－７ 多重スクロール
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\SPACE_BG.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\pix.png", &g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\GRAND_BG.png",&g_pTex[2] );

    //スター座標の初期化
    for( loop = 0; loop < STAR_MAX; loop++)
    {
        work->StarPosX[ loop ]  = rand() / (RAND_MAX / SCREEN_WIDTH);
        work->StarPosY[ loop ]  = rand() / (RAND_MAX / SCREEN_HEIGHT);
        work->StarSpeed[ loop ] = rand() / (RAND_MAX / 2.0) + 1.0;
    }
    //手前表示背景のＹ座標
    work->Bg1.Y  = 416;
}

void exec05_07(TCB* thisTCB)
{
    EX05_07_STRUCT* work = (EX05_07_STRUCT*)thisTCB->Work;
    SPRITE sprt;
    RECT star_rect = { 0, 0, 2, 2 };
    int loop;

    //一番奥の背景
    BGDraw( &work->Bg0, 0 );

    //スター描画
    sprt.SrcRect = &star_rect;

    for( loop = 0; loop < STAR_MAX; loop++)
    {
        work->StarPosX[ loop ]  -= work->StarSpeed[ loop ];
        //左端に到達したら右端に戻す
        if( work->StarPosX[ loop ]  < 0 )
            work->StarPosX[ loop ]  = SCREEN_WIDTH;

        sprt.X = work->StarPosX[ loop ];
        sprt.Y = work->StarPosY[ loop ];

        SpriteDraw( &sprt, 1 );
    }
    //一番手前の背景
    work->Bg1.X  -= 4.0;
    //左端に到達したら右端に戻す
    if( work->Bg1.X < -(SCREEN_WIDTH / 2) ) work->Bg1.X = 0;

    BGDraw( &work->Bg1, 2 );
}



////////////////////////////////////////////////////
//５章－８ 拡大、縮小
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
    //使用するテクスチャの読み込み
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
//５章－９ 回転
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

    D3DXMatrixRotationZ( &matrix_a, work->Rotate );             //回転行列を取得

    D3DXMatrixTranslation( &matrix_b, pos.x, pos.y, 0 );        //平行移動行列を取得

    D3DXMatrixMultiply( &matrix, &matrix_a, &matrix_b );        //行列を合成

    g_pSp->SetTransform( &matrix );


    g_pSp->Draw( g_pTex[bitmap_id], NULL, &work->Center, &pos, 0xffffffff);

    //設定した回転行列を元に戻す
    D3DXMatrixTranslation( &matrix, 0, 0, 0 );
    g_pSp->SetTransform( &matrix );

}

void init05_09(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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
//５章－１０ 反転処理
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

    if( work->ReversX )  //Ｘ反転処理
    {
        work->Scale.x = -1.0;
        work->Center.x = 128;
    }

    if( work->ReversY ) //Ｙ反転処理
    {
        work->Scale.y = -1.0;
        work->Center.y = 128;
    }
    //座標設定
    pos.x = work->X / work->Scale.x;
    pos.y = work->Y / work->Scale.y;
    pos.z = 0;

    //変換行列の設定
    D3DXMatrixScaling( &matrix, work->Scale.x, work->Scale.y, 1.0 );
    g_pSp->SetTransform( &matrix );
    //描画
    g_pSp->Draw( g_pTex[0], NULL, &work->Center, &pos, 0xffffffff);
}

void init05_10(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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
//５章－１２ ウィンドウを表示する
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


    //ウィンドウ幅と高さ設定
    scaleWidth  = pRect->right  - pRect->left;
    scaleHeight = pRect->bottom - pRect->top;

    //表示座標設定
    pos.x = pRect->left / scaleWidth;
    pos.y = pRect->top  / scaleHeight;
    pos.z = 0;

    D3DXMatrixScaling( &matrix, scaleWidth, scaleHeight, 1.0 );
    g_pSp->SetTransform( &matrix );
    g_pSp->Draw( g_pTex[0], &win_rect[ ID ], NULL, &pos, 0xffffffff);

    //設定した行列を元に戻す
    D3DXMatrixScaling( &matrix, 1.0, 1.0, 1.0 );
    g_pSp->SetTransform( &matrix );

}

void exec05_12_window3(TCB* thisTCB)
{
   RECT win_rect = { 128, 256, 512, 320 };
   char str[128];

    //ウィンドウ枠表示
    exec5_12_WinDraw( &win_rect, 1 );

    //ウィンドウ内部の文字表示
    FontPrint( win_rect.left , win_rect.top, "SAMPLE\nWINDOW2\nCOLOR GRADATION\nTEST!!");
}

void exec05_12_window2(TCB* thisTCB)
{
   RECT win_rect = { 336, 264, 496, 416 };
   char str[128];
    //ウィンドウ枠表示
    exec5_12_WinDraw( &win_rect, 0 );

    //ウィンドウ内部の文字表示
    FontPrint( win_rect.left , win_rect.top, "\n 1.MENU\n 2.STATUS\n 3.ITEM\n 4.HELP\n 5.OPTION\n 6.ETC\n 7.");
}

void init05_12(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\pix.png", &g_pTex[0] );
    //2つ目のウィンドウ
      TaskMake(exec05_12_window2,0x1000);
    //3つ目のウィンドウ
      TaskMake(exec05_12_window3,0x1000);
}

void exec05_12(TCB* thisTCB)
{
   RECT win_rect = { 64, 384, 576, 448 };
   char str[128];

    //ウィンドウ枠表示
    exec5_12_WinDraw( &win_rect, 2 );

    //ウィンドウ内部の文字表示
    FontPrint( win_rect.left , win_rect.top, "\n SAMPLE WINDOW MAIN\n MAKE FIRST TASK");
}



////////////////////////////////////////////////////
//５章－１３ 広いマップを表示する
////////////////////////////////////////////////////

typedef struct{
    int            ScrollX;
    int            ScrollY;
} EX05_13_STRUCT ;

void init05_13(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP.png",&g_pTex[0] );
}

void exec05_13(TCB* thisTCB)
{
#define CHIP_SIZE 32          //マップチップの大きさ
#define MAP_SIZE_X 40         //マップのサイズX
#define MAP_SIZE_Y 15         //マップのサイズY

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
    int map_dataX;        //取得するマップデータのＸ座標
    int map_dataY;        //取得するマップデータのＹ座標

    SPRITE  chip;
    RECT chip_ptn[4] = {
    {  0,   0,  32, 32},
    { 32,   0,  64, 32},
    { 64,   0,  96, 32},
    { 96,   0, 128, 32},
    };

    //スクロール座標の計算処理
    if( g_InputBuff & KEY_UP    ) work->ScrollY -= 8;
    if( g_InputBuff & KEY_DOWN  ) work->ScrollY += 8;
    if( g_InputBuff & KEY_RIGHT ) work->ScrollX += 8;
    if( g_InputBuff & KEY_LEFT  ) work->ScrollX -= 8;

    for( loopY = 0; loopY < SCREEN_HEIGHT / CHIP_SIZE + 2; loopY++ )
    {
        for( loopX = 0; loopX < SCREEN_WIDTH / CHIP_SIZE + 2; loopX++ )
        {
            //チップデータのＸ座標を計算
            map_dataX =  work->ScrollX / CHIP_SIZE + loopX;
            if( map_dataX < 0) map_dataX += MAP_SIZE_X * ( -map_dataX / MAP_SIZE_X + 1 );
            map_dataX %= MAP_SIZE_X;

            //チップデータのＹ座標を計算
            map_dataY =  work->ScrollY / CHIP_SIZE + loopY;
            map_dataY %= MAP_SIZE_Y;
            if( map_dataY < 0) map_dataY += MAP_SIZE_Y * ( -map_dataY / MAP_SIZE_Y + 1 );

            //チップの表示座標を計算
            chip.X = loopX * CHIP_SIZE - work->ScrollX % CHIP_SIZE - CHIP_SIZE;
            chip.Y = loopY * CHIP_SIZE - work->ScrollY % CHIP_SIZE - CHIP_SIZE;

            //描画するチップのデータを取得
            chip.SrcRect = &chip_ptn[  map_data[ map_dataY ][ map_dataX ]  ];

            SpriteDraw( &chip, 0);
        }
    }

}



////////////////////////////////////////////////////
//５章－１４ 背景のアニメーション
////////////////////////////////////////////////////

typedef struct{
    int            ScrollX;
    int            ScrollY;
    float          AnimCount;
} EX05_14_STRUCT ;

void init05_14(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_B.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_C.png",&g_pTex[2] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\MAP_CHIP_D.png",&g_pTex[3] );
}

void exec05_14(TCB* thisTCB)
{
#define ANIM_SPEED 1.0/8.0    //アニメーションスピード
#define ANIM_COUNT 4          //アニメーション数
#define CHIP_SIZE 32          //マップチップの大きさ
#define MAP_SIZE_X 20         //マップのサイズX
#define MAP_SIZE_Y 15         //マップのサイズY

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

    //アニメーションで切り替えるマップチップのパターンを取得する
    work->AnimCount += ANIM_SPEED;
    //アニメーションカウンタを補正
    if( work->AnimCount >= ANIM_COUNT )work->AnimCount -= ANIM_COUNT;
    map_chip_pattern = work->AnimCount;

    for( loopY = 0; loopY < SCREEN_HEIGHT / CHIP_SIZE ; loopY++ )
    {
        for( loopX = 0; loopX < SCREEN_WIDTH / CHIP_SIZE ; loopX++ )
        {
            //チップの表示座標を計算
            chip.X = loopX * CHIP_SIZE;
            chip.Y = loopY * CHIP_SIZE;

            //描画するチップのデータを取得
            chip.SrcRect = &chip_ptn[  map_data[ loopY ][ loopX ]  ];

            SpriteDraw( &chip, map_chip_pattern);
        }
    }

}






////////////////////////////////////////////////////
//５章－１５ラスタスクロール
////////////////////////////////////////////////////

void ex5_RasterDraw( short*  raster_pos, int bitmap_id)
{
    int   loop;
    RECT  slice_work = { 0, 0, 1024, 0,};  //分割用のワーク
    D3DXVECTOR3 pos;

    for(loop = 0;loop < SCREEN_HEIGHT; loop++ )
    {   //スクロール座標設定
        pos.x = raster_pos[ loop ];
        pos.y = loop;
        pos.z = 0;
        //分割
        slice_work.top   = loop;
        slice_work.bottom= loop + 1;
        //描画
        g_pSp->Draw( g_pTex[bitmap_id], &slice_work, NULL, &pos, 0xffffffff);
    }

}


void init05_15(TCB* thisTCB)
{

    //使用するテクスチャの読み込み
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
//５章－１６ 奥行きのある地面を表示
////////////////////////////////////////////////////

void init05_16(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\PERS_BG.png",&g_pTex[0] );
}

void exec05_16(TCB* thisTCB)
{
#define RASTER_START  256        //ラスタースクロールを開始する位置
#define RASTER_END    512        //ラスタースクロールを終了する位置
#define BACK_POS      180
#define SCROLL_SPEED  4
typedef struct{
    int            ScrollPos;
    short          RasterPos[SCREEN_HEIGHT];
} EX5_18_STRUCT ;

    EX5_18_STRUCT* work = (EX5_18_STRUCT*)thisTCB->Work;
    int   loop;

    //左右ヘスクロール
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

    //奥行き部分を分割
    for( loop = 0; loop < SCREEN_HEIGHT; loop++)
    {   //地面以外の部分は固定位置で、スクロールしない
        work->RasterPos[ loop ] = -BACK_POS;

        //地面部分をスクロール座標にあわせて歪ませる
        if( loop >= RASTER_START )
        {
            work->RasterPos[ loop ] += work->ScrollPos * (loop / (float)(RASTER_END - RASTER_START) );
        }
    }

    ex5_RasterDraw( work->RasterPos, 0);
}






////////////////////////////////////////////////////
//５章－１７ フェードイン・アウトを行う
////////////////////////////////////////////////////
typedef struct{
    BACK_GROUND    BaseBg;
    BACK_GROUND    FadeBg;
    int            FadeMode;         //0.フェード無し、1.フェードアウト -1.フェードイン
    int            FadeCount;
} EX05_17_STRUCT ;

void exec05_17_FadeDraw(BACK_GROUND* bg,int bitmap_id,unsigned int fade_level)
{
    D3DXVECTOR3 pos;
    DWORD       color;

    pos.x = bg->X;
    pos.y = bg->Y;
    pos.z = 0;
    //フェードレベルによってα値と色を設定
    color = D3DCOLOR_ARGB( fade_level ,0xff,0xff,0xff );

    g_pSp->Draw( g_pTex[bitmap_id],NULL,NULL,&pos, color);
}


void init05_17(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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

    //フェード中かどうかで処理を分ける
    if( work->FadeMode == FADE_NONE )
    {//キー入力でフェードの開始
        if( g_DownInputBuff & KEY_Z )
        {//フェードイン処理開始
            work->FadeMode  = FADE_IN;
            work->FadeCount = 0xff;
        }
        if( g_DownInputBuff & KEY_X )
        {//フェードアウト処理開始
            work->FadeMode  = FADE_OUT;
            work->FadeCount = 0;
        }
    }else{
    //フェードイン・アウト中
        if( work->FadeMode == FADE_IN )
        {//フェードイン処理
            work->FadeCount -= FADE_SPEED;
            if( work->FadeCount <= 0 )
            {
                work->FadeMode  = FADE_NONE;
                work->FadeCount = 0;
            }
        }
        if( work->FadeMode == FADE_OUT )
        {//フェードアウト処理
            work->FadeCount += FADE_SPEED;
            if( work->FadeCount >= 0xff )
            {
                work->FadeMode  = FADE_NONE;
                work->FadeCount = 0xff;
            }
        }
   }

    //対象の背景を表示
    BGDraw( &work->BaseBg, 0);

    //フェード中はフェード用の画像を表示
    if( work->FadeMode != FADE_NONE ) exec05_17_FadeDraw( &work->FadeBg, 1,work->FadeCount );
}





////////////////////////////////////////////////////
//５章－１８ キャラクターに残像を付ける
////////////////////////////////////////////////////


#define IMAGE_COUNT 8
#define MOVE_SPEED  8.0

typedef struct{
    SPRITE         Sprt;
    SPRITE         Image;
    D3DXVECTOR2    PosBuff[ IMAGE_COUNT ];  //残像の座標
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
}

void exec05_18(TCB* thisTCB)
{
    EX05_18_STRUCT* work = (EX05_18_STRUCT*)thisTCB->Work;
    int loop;
    float add_color_depth;
    float color_depth = 0;

    //座標履歴（残像の表示座標）をずらす
    for( loop = IMAGE_COUNT-1; loop > 0; loop-- )
    {//一番古い座標から消していく
        work->PosBuff[ loop ].x = work->PosBuff[ loop-1 ].x;
        work->PosBuff[ loop ].y = work->PosBuff[ loop-1 ].y;
    }

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;

    //自機の移動座標を履歴に記録する
    work->PosBuff[ 0 ].x = work->Sprt.X;
    work->PosBuff[ 0 ].y = work->Sprt.Y;

    //残像の表示数を変更
    if( g_DownInputBuff & KEY_Z )
    {//最大表示数を越えないように補正
       if( ++work->ImageCount >= IMAGE_COUNT ) work->ImageCount = 0;
    }

    //残像の色の濃さを計算する加算値を取得
    if(work->ImageCount != 0)
    {
       add_color_depth = 0xff / work->ImageCount;
    }

    //指定した数だけ残像を表示する
    for( loop = work->ImageCount; loop > 0; loop-- )
    {
        //座標履歴から表示座標を取得
        work->Image.X = work->PosBuff[ loop ].x;
        work->Image.Y = work->PosBuff[ loop ].y;

        //残像の色の濃さを計算
        color_depth += add_color_depth;

        //色の濃さを反映する専用の描画関数で描画
        EX05_18_CustomDraw( &work->Image,0,(int)color_depth );
    }

    SpriteDraw( &work->Sprt, 0);

}



////////////////////////////////////////////////////
//５章－１９ キャラクターに影を付ける
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Human;
    SPRITE         Shadow;
    float          Acc;
    int            JumpFlag;
} EX05_19_STRUCT;
#define MOVE_SPEED 4.0             //移動速度
#define JUMP_POWER 10.0            //ジャンプ力
#define GRAVITY    0.35            //重力値
#define GRAND_LINE SCREEN_HEIGHT/2 //地面位置
#define CHR_HEIGHT 104.0           //キャラの高さ

void init05_19(TCB* thisTCB)
{
    EX05_19_STRUCT* work = (EX05_19_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\shadow.png",&g_pTex[1] );

    //座標の初期化
    work->Human.X = SCREEN_WIDTH / 2;
    work->Human.Y = GRAND_LINE - CHR_HEIGHT;
}

void exec05_19(TCB* thisTCB)
{
    EX05_19_STRUCT* work = (EX05_19_STRUCT*)thisTCB->Work;

    //キャラクターの移動処理
    if( g_InputBuff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;
    //ジャンプ処理
    if( !work->JumpFlag )
    {//着地時の処理
        if( g_DownInputBuff & KEY_Z    )
        {//キー入力でジャンプ
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
     }else{
      //ジャンプ中処理
        work->Acc += GRAVITY;
        work->Human.Y += work->Acc;
        if( work->Human.Y > GRAND_LINE - CHR_HEIGHT)
        {//ジャンプ処理終了
            work->Human.Y =  GRAND_LINE - CHR_HEIGHT;
            work->JumpFlag = false;
        }
    }

    //影の処理
    //キャラクターのX座標のみ追従する
    work->Shadow.X = work->Human.X;
    //Y座標は地面に合わせる（ここでは固定値）
    work->Shadow.Y = GRAND_LINE;

    //影の描画を先に行なう
    SpriteDraw( &work->Shadow, 1);
    SpriteDraw( &work->Human, 0);
}



////////////////////////////////////////////////////
//５章－２０ マウスなどで自由にスクロールさせる
////////////////////////////////////////////////////

typedef struct{
    BACK_GROUND    BG;
} EX05_20_STRUCT;

typedef struct{
    int            AddX;           //マウスに反応した時加算するＸの値
    int            AddY;           //マウスに反応した時加算するＹの値
    RECT           MoveRect;       //マウスが反応する領域
} EX05_20_DATA;

void init05_20(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_20(TCB* thisTCB)
{
//マウスを移動させるポイントの数
#define POINT_COUNT 4

    EX05_20_STRUCT* work = (EX05_20_STRUCT*)thisTCB->Work;
    EX05_20_DATA  MovePoint[] = 
{//    X    Y    RECT
    {  0,   4, {   0,   0, 640,  16,},},      //１つ目 上
    {  0,  -4, {   0, 464, 640, 480,},},      //２つ目 下
    { -4,   0, { 624,   0, 640, 480,},},      //３つ目 右
    {  4,   0, {   0,   0,  16, 480,},},      //４つ目 左
};
    int loop;

    //マウスカーソルに反応するポイントの数だけループ
    for( loop = 0; loop < POINT_COUNT ;loop++ ) 
    {//マウスカーソルが反応する領域をチェック
        if( g_MousePos.y > MovePoint[ loop ].MoveRect.top    && 
            g_MousePos.y < MovePoint[ loop ].MoveRect.bottom &&
            g_MousePos.x > MovePoint[ loop ].MoveRect.left   &&
            g_MousePos.x < MovePoint[ loop ].MoveRect.right  )
        {//領域内にあれば、座標を加算
            work->BG.X += MovePoint[ loop ].AddX;
            work->BG.Y += MovePoint[ loop ].AddY;
        }
    }

    //スクロールを画面の範囲内に収める
    if( work->BG.X > 0    ) work->BG.X = 0;
    if( work->BG.X < -640 ) work->BG.X = -640;
    if( work->BG.Y > 0    ) work->BG.Y = 0;
    if( work->BG.Y < -32  ) work->BG.Y = -32;

    BGDraw(&work->BG,0);
}





////////////////////////////////////////////////////
//５章－２１ マウスなどで自由にスクロールさせる２
////////////////////////////////////////////////////
typedef struct{
    BACK_GROUND    BG;
    POINT          MouseOldPos;
} EX05_21_STRUCT;

void init05_21(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[0] );
}

void exec05_21(TCB* thisTCB)
{
    EX05_21_STRUCT* work = (EX05_21_STRUCT*)thisTCB->Work;


    //マウス左ボタンをチェック
    if( g_MouseButton & MOUSE_L )
    {//ドラッグ中の処理
        //マウスの移動量を算出、スクロール値として加算する
        work->BG.X += g_MousePos.x - work->MouseOldPos.x;
        work->BG.Y += g_MousePos.y - work->MouseOldPos.y;
    }
    //1フレーム前のマウス位置を保存
    work->MouseOldPos = g_MousePos;

    //スクロールを画面の範囲内に収める
    if( work->BG.X > 0    ) work->BG.X = 0;
    if( work->BG.X < -640 ) work->BG.X = -640;
    if( work->BG.Y > 0    ) work->BG.Y = 0;
    if( work->BG.Y < -32  ) work->BG.Y = -32;

    BGDraw(&work->BG,0);
}







////////////////////////////////////////////////////
//６章 移動
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//６章－２  目標に向かって移動
////////////////////////////////////////////////////

typedef struct{
    SPRITE         Ball;
    SPRITE         Target;               //目標のスプライト
    int            Time;
} EX06_02_STRUCT ;

void init06_02(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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
    float direction;    //進行方向

    //初期化処理、一定時間ごとに初期化される
    if( work->Time-- == 0 )
    {
        work->Ball.X   = START_X;
        work->Ball.Y   = START_Y;
        work->Target.X = TARGET_X;
        work->Target.Y = TARGET_Y;

        work->Time = 30;
    }
    //座標から相手への方向を計算
    direction = atan2( TARGET_Y - START_Y, TARGET_X - START_X );
    //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
    work->Ball.X += cos( direction ) * MOVE_SPEED;
    work->Ball.Y += sin( direction ) * MOVE_SPEED;

    SpriteDraw( &work->Ball, 1);
    SpriteDraw( &work->Target, 0);
}




////////////////////////////////////////////////////
//６章－３  キャラを曲線的に移動
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //座標の初期化
    work->Ball.X   = SCREEN_WIDTH  / 3;
    //移動速度の初期化
    work->AddX     = -MOVE_SPEED;
}

void exec06_03(TCB* thisTCB)
{
#define CHANGE_TIME 100      //左右移動切り替え時間
#define SIN_WIDTH   50.0     //カーブの揺れ幅
#define SIN_SPEED   0.1      //振幅の速度

    EX06_03_STRUCT* work = (EX06_03_STRUCT*)thisTCB->Work;


    //一定時間ごとに左右移動を切り替える
    if( work->Time-- <= 0 )
    {
        //移動方向を反転させる
        work->AddX *= -1;

        //次の切り替え時間
        work->Time = CHANGE_TIME;
    }

    //Ｚキーを押している間は曲線用の値を加算
    if( g_InputBuff & KEY_Z ) work->SinNum += SIN_SPEED;

    //左右移動は直線的な運動のみ
    work->Ball.X += work->AddX;

    //上下移動はSin関数を用いて曲線状にする
    work->Ball.Y = SCREEN_HEIGHT / 2; //基本座標
    work->Ball.Y += sin( work->SinNum ) * SIN_WIDTH;

    SpriteDraw( &work->Ball, 0);
}





////////////////////////////////////////////////////
//６章－４  スクロールに合わせてキャラを動かす
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
    //使用するテクスチャの読み込み
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

    //背景をループさせる
    work->BG.X = work->ScrollPosX % 640;
    BGDraw(&work->BG,0);


    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShipPosY -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShipPosY += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShipPosX += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShipPosX -= MOVE_SPEED;

    //スクロールに合わせる
    work->MyShip.X = work->ScrollPosX + work->MyShipPosX;
    work->MyShip.Y = work->MyShipPosY;

    SpriteDraw(&work->MyShip,1);

}






////////////////////////////////////////////////////
//６章－５  円運動
////////////////////////////////////////////////////

void init06_05(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_05(TCB* thisTCB)
{
#define CIRCLE_CENTER_X 320-16   //回転中心点Ｘ
#define CIRCLE_CENTER_Y 240-16   //回転中心点Ｙ
#define CIRCLE_RADIUS   80       //回転半径
#define CIRCLE_SPEED    0.05     //回転速度

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;
    //回転速度
    pspr->Count += CIRCLE_SPEED;     
    //円運動計算
    pspr->X = CIRCLE_CENTER_X + sin(pspr->Count) * CIRCLE_RADIUS;
    pspr->Y = CIRCLE_CENTER_Y + cos(pspr->Count) * CIRCLE_RADIUS;

    SpriteDraw(pspr,0);
}





////////////////////////////////////////////////////
//６章－６ 楕円運動
////////////////////////////////////////////////////
typedef struct{
  int        BallCount;
  int        Time;
} EX06_06_STRUCT;

static float EX06_06_gDist;
void exec06_06_ball(TCB* thisTCB)
{
#define CIRCLE_CENTER_X 320-16   //回転中心点Ｘ
#define CIRCLE_CENTER_Y 240-16   //回転中心点Ｙ
#define CIRCLE_RADIUS   80       //回転半径
#define CIRCLE_SPEED    0.05     //回転速度

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;

    //回転速度
    pspr->Count += CIRCLE_SPEED;

    //楕円運動計算
    pspr->X = CIRCLE_CENTER_X + sin(pspr->Count-EX06_06_gDist) * CIRCLE_RADIUS;
    pspr->Y = CIRCLE_CENTER_Y + cos(pspr->Count+EX06_06_gDist) * CIRCLE_RADIUS;

    SpriteDraw(pspr,0);
}

void init06_06(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    EX06_06_gDist = 0.0;
}

void exec06_06(TCB* thisTCB)
{
#define  BALL_COUNT 16 
    EX06_06_STRUCT* work = (EX06_06_STRUCT*)thisTCB->Work;

    //８フレーム毎にボールを作成
    if(work->Time++ == 0x08)
    {
        work->Time = 0;
        if(work->BallCount++ < BALL_COUNT)
        {//一定数作成するまで繰り返す
            TaskMake(exec06_06_ball,0x1000);
        }
    }
    //歪み値
    EX06_06_gDist += 0.01;
}



////////////////////////////////////////////////////
//６章－７ 加減速移動
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc_Speed;
    float          Speed;
} EX06_07_STRUCT ;

void exec06_07_acc(TCB* thisTCB)
{
#define STOP_LINE       380     //停止ライン

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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_07(TCB* thisTCB)
{
#define START_LINE      64       //開始ライン
#define ACC_SPEED       0.1      //速度
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
    work->Speed     = 8.0;        //初速値


    TaskKill( thisTCB );
}





////////////////////////////////////////////////////
//６章－８ 誘導弾１  常に相手を狙う
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //目標のスプライト
} EX06_08_STRUCT ;


void exec06_08_horming(TCB* thisTCB)
{
#define MOVE_SPEED  2.0
    EX06_08_STRUCT* work = (EX06_08_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //目標の座標を取得
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float direction;

    //目標の方向を取得
    direction= atan2(  targetY - my_Y , targetX - my_X);

    //方向への移動値を取得
    work->sprt.X += cos(direction)*MOVE_SPEED;
    work->sprt.Y += sin(direction)*MOVE_SPEED;

    SpriteDraw(&work->sprt,1);
}

void exec06_08_target(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_08(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_08(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_08_STRUCT* tmp_work;

    //目標のスプライト
    tmp_tcb = TaskMake( exec06_08_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;

    //誘導弾の初期化と、目標スプライトを設定
    tmp_tcb = TaskMake( exec06_08_horming, 0x2000 );
    tmp_work = (EX06_08_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;


    //生成後、処理終了
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//６章－９ 誘導弾２  加速をつけて相手を狙う
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //目標のスプライト
    float          SpeedX;               //速度
    float          SpeedY;
} EX06_09_STRUCT ;


void exec06_09_horming(TCB* thisTCB)
{
#define ACC_SPEED  1.0
    EX06_09_STRUCT* work = (EX06_09_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //目標の座標を取得
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float acc_speedX;
    float acc_speedY;
    float direction;

    //目標との相対位置を元に、加速方向を取得
    if( targetX - my_X >= 0) acc_speedX =  ACC_SPEED;
    if( targetX - my_X <  0) acc_speedX = -ACC_SPEED;
    if( targetY - my_Y >= 0) acc_speedY =  ACC_SPEED;
    if( targetY - my_Y <  0) acc_speedY = -ACC_SPEED;


    //加速値を元に速度を算出
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_09(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_09(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_09_STRUCT* tmp_work;

    //目標のスプライト
    tmp_tcb = TaskMake( exec06_09_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;


    //誘導弾の初期化と、目標スプライトを設定
    tmp_tcb = TaskMake( exec06_09_horming, 0x2000 );
    tmp_work = (EX06_09_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;

    //生成後、処理終了
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//６章－１０ 誘導弾３  回り込むように狙う
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //目標のスプライト
    float          Direction;            //方向
} EX06_10_STRUCT ;

void exec06_10_horming(TCB* thisTCB)
{
#define ROT_SPEED  0.05    //旋回速度
#define MOVE_SPEED  4.0
    EX06_10_STRUCT* work = (EX06_10_STRUCT*)thisTCB->Work;
    float my_X    = work->sprt.X;
    float my_Y    = work->sprt.Y;
    //目標の座標を取得
    float targetX = work->Target->X + 32;
    float targetY = work->Target->Y + 32;

    float direction;


    //目標の方向を取得
    direction= atan2( targetY - my_Y , targetX - my_X );

    //方向を±ＰＩの範囲内に丸める
    if( work->Direction - direction >  M_PI) work->Direction -= M_PI * 2;
    if( work->Direction - direction < -M_PI) work->Direction += M_PI * 2;

    //左回り
    if( work->Direction - direction < 0 ) work->Direction += ROT_SPEED;
    //右回り
    if( work->Direction - direction > 0 ) work->Direction -= ROT_SPEED;


    //進行方向への移動値を取得
    work->sprt.X += cos(work->Direction)*MOVE_SPEED;
    work->sprt.Y += sin(work->Direction)*MOVE_SPEED;

    SpriteDraw(&work->sprt,1);
}

void exec06_10_target(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_10(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_10(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* target_sprt;
    EX06_10_STRUCT* tmp_work;

    //目標のスプライト
    tmp_tcb = TaskMake( exec06_10_target, 0x1000 );
    target_sprt = (SPRITE*)tmp_tcb->Work;
    target_sprt->X = SCREEN_WIDTH/2;
    target_sprt->Y = SCREEN_HEIGHT/2;

    //誘導弾の初期化と、目標スプライトを設定
    tmp_tcb = TaskMake( exec06_10_horming, 0x2000 );
    tmp_work = (EX06_10_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = target_sprt;

    //生成後、処理終了
    TaskKill( thisTCB );
}



////////////////////////////////////////////////////
//６章－１１  データに沿ってキャラを動かす
////////////////////////////////////////////////////
#define MOVE_SPEED 8.0
#define END_DATA   -999.0
typedef struct{
    SPRITE      Sprt;
    int         Time;
    int         Point;
    //目標への移動速度
    float     AddX;
    float     AddY;
} EX06_11_STRUCT;

void init06_11(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_11(TCB* thisTCB)
{
    EX06_11_STRUCT* work = (EX06_11_STRUCT*)thisTCB->Work;
    float direction;
    float distance;
    float posX;
    float posY;
//移動先データ
float point_data[][2] = 
{//  X     Y
    { 320.0,   0.0,},  //0初期座標
    { 320.0,  96.0,},  //1
    { 480.0,  96.0,},  //2
    { 448.0, 256.0,},  //3
    {  96.0, 256.0,},  //4
    {  96.0, 320.0,},  //5
    { 560.0, 400.0,},  //6
    { 560.0, 480.0,},  //7
    {END_DATA,END_DATA,},  //終了
};

    //初期化時、または一定時間で目標を更新
    if(work->Time-- <= 0)
    {
        //次の目標をチェック
        if( point_data[work->Point+1][0] == END_DATA)
        {//データが終了していたら、データを初めに戻す
         //(サンプルの場合 通常はそのまま終了処理）
            work->Point = 0;
        }

        //移動開始座標を指定
        //（移動終了時の誤差修正の意もある）
        work->Sprt.X = point_data[work->Point][0];
        work->Sprt.Y = point_data[work->Point][1];

        //目標からの相対座標を計算
        posX = point_data[work->Point+1][0] - work->Sprt.X;
        posY = point_data[work->Point+1][1] - work->Sprt.Y;


        //距離を計測し、速度から目標への所要時間を割り出す
        distance = sqrtf( (posX * posX) + (posY * posY) );
        work->Time = distance / MOVE_SPEED;

        //目標の方向計算
        direction = atan2( posY, posX);

        //方向から移動速度を設定
        work->AddX = cos( direction ) * MOVE_SPEED;
        work->AddY = sin( direction ) * MOVE_SPEED;

        //次のデータ
        work->Point++;
    }

    //移動処理
    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    //ボール表示
    SpriteDraw( &work->Sprt, 0);
}






////////////////////////////////////////////////////
//６章－１２  振り子のような動き
////////////////////////////////////////////////////

void init06_12(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
}

void exec06_12(TCB* thisTCB)
{
#define PENDULUM_CENTER_X 320-16   //中心点Ｘ
#define PENDULUM_CENTER_Y 240-16   //中心点Ｙ
#define PENDULUM_RADIUS   80       //振り子の半径
#define PENDULUM_SPEED    0.15     //往復速度
#define PENDULUM_RADIUS2   1.0     //振り子の往復の幅

    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;
    //振り子の速度
    pspr->Count += PENDULUM_SPEED;

    //運動計算
    pspr->X = PENDULUM_CENTER_X + sin( sin( pspr->Count ) * PENDULUM_RADIUS2 ) * PENDULUM_RADIUS;
    pspr->Y = PENDULUM_CENTER_Y + cos( sin( pspr->Count ) * PENDULUM_RADIUS2 ) * PENDULUM_RADIUS;

    SpriteDraw(pspr,0);
}





////////////////////////////////////////////////////
//６章－１３ 移動に慣性をつける
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Sprt;
    float          SpeedX;          //Ｘ方向への加速値
    float          SpeedY;          //Ｙ方向への加速値
} EX06_13_STRUCT ;


void init06_13(TCB* thisTCB)
{
    EX06_13_STRUCT* work = (EX06_13_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //自機のスプライト
    work->Sprt.X = SCREEN_WIDTH/2;
    work->Sprt.Y = SCREEN_HEIGHT/2;
}

void exec06_13(TCB* thisTCB)
{
#define ACC_SPEED  1.0
    EX06_13_STRUCT* work = (EX06_13_STRUCT*)thisTCB->Work;

    float acc_speedX = 0;
    float acc_speedY = 0;

    //もし、移動していたら減速を行う
    if(work->SpeedX > 0 ) acc_speedX = -ACC_SPEED;
    if(work->SpeedX < 0 ) acc_speedX =  ACC_SPEED;

    if(work->SpeedY > 0 ) acc_speedY = -ACC_SPEED;
    if(work->SpeedY < 0 ) acc_speedY =  ACC_SPEED;

    //キー入力により加速を行う
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
//６章－１４ オプション１  キャラの周囲を旋回する
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Myship;               //自機情報
    int            OptionID;
    float          Rot;                  //回転
} EX06_14_STRUCT ;

void exec06_14_option(TCB* thisTCB)
{
#define ROT_SPEED    0.05        //回転速度
#define ROT_RADIUS   96          //回転半径
//各オプションの回転間隔
float option_table[] = { 0.0, M_PI * 0.5, M_PI, M_PI * 1.5,};

    EX06_14_STRUCT* work = (EX06_14_STRUCT*)thisTCB->Work;
    //自機の座標を取得
    float my_shipX = work->Myship->X + 16;
    float my_shipY = work->Myship->Y + 16;

    work->Rot += ROT_SPEED;     
    //精度低下を防ぐために丸める
    if( work->Rot >  M_PI) work->Rot -= M_PI * 2;

    work->sprt.X = my_shipX + cos(work->Rot + option_table[ work->OptionID ] ) * ROT_RADIUS;
    work->sprt.Y = my_shipY + sin(work->Rot + option_table[ work->OptionID ] ) * ROT_RADIUS;

    SpriteDraw(&work->sprt,1);

}

void exec06_14_my_ship(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}

void init06_14(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_14(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* my_ship_sprt;
    EX06_14_STRUCT* tmp_work;
    int loop;


    //自機のスプライト
    tmp_tcb = TaskMake( exec06_14_my_ship, 0x1000 );
    my_ship_sprt = (SPRITE*)tmp_tcb->Work;
    my_ship_sprt->X = SCREEN_WIDTH/2;
    my_ship_sprt->Y = SCREEN_HEIGHT/2;

    //オプションを４つ生成、自機ワーク情報を設定
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_14_option, 0x2000 );
        tmp_work = (EX06_14_STRUCT*)tmp_tcb->Work;
        tmp_work->Myship = my_ship_sprt;
        tmp_work->OptionID = loop;
    }
    //生成後、処理終了
    TaskKill( thisTCB );
}


////////////////////////////////////////////////////
//６章－１５ オプション２  常にキャラを追いかける
////////////////////////////////////////////////////

typedef struct{
    SPRITE         sprt;
    int            PosPointer;
    D3DXVECTOR2    PosBuff[64];       //座標履歴
} EX06_15_MY_SHIP;

typedef struct{
    SPRITE            sprt;
    EX06_15_MY_SHIP*  Myship;               //自機情報
    int               OptionID;
} EX06_15_OPTION;


void exec06_15_option(TCB* thisTCB)
{
    EX06_15_OPTION* work = (EX06_15_OPTION*)thisTCB->Work;
    int option_pos_pointer;

    //オプションのＩＤから表示する座標の履歴を取得
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->sprt.X -= MOVE_SPEED;

    work->PosPointer++;
    work->PosPointer %= 64;
    //移動座標を履歴に記録
    work->PosBuff[work->PosPointer].x = work->sprt.X;
    work->PosBuff[work->PosPointer].y = work->sprt.Y;

    SpriteDraw( &work->sprt, 0);
}

void init06_15(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_15(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    EX06_15_MY_SHIP* my_ship;
    EX06_15_OPTION*  tmp_work;
    int loop;


    //自機のスプライト
    tmp_tcb = TaskMake( exec06_15_my_ship, 0x1000 );
    my_ship = (EX06_15_MY_SHIP*)tmp_tcb->Work;
    my_ship->sprt.X = SCREEN_WIDTH/2;
    my_ship->sprt.Y = SCREEN_HEIGHT/2;

    //オプションを４つ生成、自機ワーク情報を設定
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_15_option, 0x2000 );
        tmp_work = (EX06_15_OPTION*)tmp_tcb->Work;
        tmp_work->Myship = my_ship;
        tmp_work->OptionID = loop;
    }
    //生成後、処理終了
    TaskKill( thisTCB );
}



////////////////////////////////////////////////////
//６章－１６ オプション３
////////////////////////////////////////////////////

typedef struct{
    SPRITE         sprt;
    int            PosPointer;
    D3DXVECTOR2    PosBuff[64];             //座標履歴
} EX06_16_MY_SHIP;

typedef struct{
    SPRITE            sprt;
    EX06_16_MY_SHIP*  Myship;               //自機情報
    int               OptionID;
} EX06_16_OPTION;


void exec06_16_option(TCB* thisTCB)
{
    EX06_16_OPTION* work = (EX06_16_OPTION*)thisTCB->Work;
    int option_pos_pointer;

    //オプションのＩＤから表示する座標の履歴を取得
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
    //キー入力による移動
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

    //移動が行われた時のみ履歴を更新
    if( move_flag )
    {
        work->PosPointer++;
        work->PosPointer %= 64;
        //移動座標を履歴に記録
        work->PosBuff[work->PosPointer].x = work->sprt.X;
        work->PosBuff[work->PosPointer].y = work->sprt.Y;
    }
    SpriteDraw( &work->sprt, 0);
}

void init06_16(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
}

void exec06_16(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    EX06_16_MY_SHIP* my_ship;
    EX06_16_OPTION*  tmp_work;
    int loop;

    //自機のスプライト
    tmp_tcb = TaskMake( exec06_16_my_ship, 0x1000 );
    my_ship = (EX06_16_MY_SHIP*)tmp_tcb->Work;
    my_ship->sprt.X = SCREEN_WIDTH/2;
    my_ship->sprt.Y = SCREEN_HEIGHT/2;

    //オプションを４つ生成、自機ワーク情報を設定
    for( loop = 0; loop < 4; loop++)
    {
        tmp_tcb = TaskMake( exec06_16_option, 0x2000 );
        tmp_work = (EX06_16_OPTION*)tmp_tcb->Work;
        tmp_work->Myship = my_ship;
        tmp_work->OptionID = loop;
    }
    //生成後、処理終了
    TaskKill( thisTCB );
}





////////////////////////////////////////////////////
//６章－１７ ジャンプをするには
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            JumpFlag;
} EX06_17_STRUCT;


void init06_17(TCB* thisTCB)
{
    EX06_17_STRUCT* work = (EX06_17_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_17(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //ジャンプ力
#define GRAVITY  0.25     //重力値

    EX06_17_STRUCT* work = (EX06_17_STRUCT*)thisTCB->Work;

    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {//着地時の処理
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //着地したらジャンプ処理終了
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {//座標が、地面座標より下ならば、着地したとみなす
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}


////////////////////////////////////////////////////
//６章－１８ バウンドをさせる
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    float          Acc;
    int            BoundFlag;
} EX06_18_STRUCT;

void init06_18(TCB* thisTCB)
{
    EX06_18_STRUCT* work = (EX06_18_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_18(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //ジャンプ力
#define GRAVITY  0.25     //重力値
#define BOUND_DECR     1.0      //バウンド時の減衰

    EX06_18_STRUCT* work = (EX06_18_STRUCT*)thisTCB->Work;

    //バウンド中は反応しない
    if( !work->BoundFlag )
    {
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//バウンド開始時処理
            work->Acc = -JUMP_POWER;
            work->BoundFlag = true;
        }
     }

    if( work->BoundFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //地面に再接触したら、バウンドする
        if( work->sprt.Y > SCREEN_HEIGHT / 2 )
        {
            work->sprt.Y -= work->Acc;
            work->Acc = -( work->Acc - BOUND_DECR );    //バウンド時に減衰する

            if( work->Acc > -1.0 )
            { //殆どバウンドしなくなったら終了
                work->sprt.Y =  SCREEN_HEIGHT / 2;
                work->BoundFlag = false;
            }
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//６章－１９ ボタンを押す長さでジャンプの高さを変える
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;

}

void exec06_19(TCB* thisTCB)
{
#define JUMP_POWER  10.0        //ジャンプ力
#define PUSH_GRAVITY  0.25      //押下時の重力値
#define FALL_RATE     8.0       //落下速度の計算値

    EX06_19_STRUCT* work = (EX06_19_STRUCT*)thisTCB->Work;


    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {//着地時の処理
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
            work->Gravity = PUSH_GRAVITY;
        }
     }

    if( work->JumpFlag )
    {
        //ジャンプボタンが離されたか？
        if( g_UpInputBuff & KEY_Z  )
        {
            //上昇中だけ処理
            if( work->Acc < 0 )
            {//ボタンが離された時の上昇速度に応じて落下速度を変える
                work->Gravity = -work->Acc / FALL_RATE;
            }
        }

        work->Acc += work->Gravity;
        work->sprt.Y += work->Acc;
        //着地したらジャンプ処理終了
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//６章－２０ 好きな高さでジャンプする
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;
}

float GetJumpPower( float Time, float Height )
{
    return Height / (Time * (Time + 1.0) * (1.0 / 2.0) );                //高さ ÷ 時間×時間＋１ × １／２
}


void exec06_20(TCB* thisTCB)
{
#define JUMP_POS_A  256.0  //ジャンプ高さ
#define JUMP_POS_B  128.0  //ジャンプ高さ
#define JUMP_TIME_A 30.0   //頂点までの時間
#define JUMP_TIME_B 60.0   //頂点までの時間

    EX06_20_STRUCT* work = (EX06_20_STRUCT*)thisTCB->Work;

    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理A
            work->Acc = -(GetJumpPower(JUMP_TIME_A , JUMP_POS_A) * JUMP_TIME_A);
            work->Gravity = GetJumpPower(JUMP_TIME_A , JUMP_POS_A);
            work->JumpFlag = true;

        }
        if( g_DownInputBuff & KEY_X    )
        {//ジャンプ開始時処理B
            work->Acc = -(GetJumpPower(JUMP_TIME_B , JUMP_POS_B) * JUMP_TIME_B);
            work->Gravity = GetJumpPower(JUMP_TIME_B , JUMP_POS_B);
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += work->Gravity;
        work->sprt.Y += work->Acc;

        //着地したらジャンプ処理終了
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);
}



////////////////////////////////////////////////////
//６章－２１ スクロールすると敵が出てくる様にする
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
    {//初期化
        //座標の設定
        work->Sprt.X = rand() % SCREEN_WIDTH;
        //初期化終了のフラグ
        work->InitFlag = TRUE;
    }
    //移動処理、上から落下する
    work->Sprt.Y += MOVE_SPEED;

    SpriteDraw( &work->Sprt ,0);
}


void init06_21(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\BG.png",&g_pTex[1] );
}

void exec06_21(TCB* thisTCB)
{
#define MOVE_SPEED 8
    EX06_21_STRUCT* work = (EX06_21_STRUCT*)thisTCB->Work;
    char str[256];
int scroll_table[] = 
{//出現データ
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


    //出現データとスクロールポイントを比較
    if( work->ScrollPoint == scroll_table[ work->DataIndex ] )
    {//一致したら出現
        TaskMake( exec06_21_ball, 0x2000 );

        //次の比較データ
        work->DataIndex++;
    }

    //スクロール処理

    //もしキー入力があれば初期化する
    if( g_DownInputBuff & KEY_Z    )
    {
        work->ScrollPoint = 0;
        work->DataIndex = 0;
    }

    //スクロールポイントを座標に変換
    work->BG.X = -work->ScrollPoint;
    //画面端に到達していなければスクロールさせる
    if(work->BG.X >  -384 ) work->ScrollPoint++;

    BGDraw(&work->BG,1);

    sprintf( str,"'Z'KEY Scroll Reset!!\n SCROLL POINT %d",work->ScrollPoint );
    FontPrint( 128,128, str);

}




////////////////////////////////////////////////////
//６章－２２ 高低差のある地形を移動するには
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\bar.png",&g_pTex[1] );

    //座標の設定
    work->X = SCREEN_WIDTH / 2;

    for( loop=0;loop < HEIGHT_DATA; loop++)
    { //sin関数を利用して上下のある地形を生成
        work->HeightData[loop] = 
            sin( sin_wave ) * UP_DOWN + HEIGHT_BASE_POS;
        sin_wave += 0.125;
    }

}

void exec06_22(TCB* thisTCB)
{
    EX06_22_STRUCT* work = (EX06_22_STRUCT*)thisTCB->Work;
    int loop;

    //キー入力による移動
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;
    //移動範囲を画面内にする
    if(work->X >= SCREEN_WIDTH - DATA_SIZE)
        work->X = SCREEN_WIDTH - DATA_SIZE;
    if(work->X <  0)work->X = 0;

    //Ｘ座標から、地面の高さデータを取得
    work->Y = work->HeightData[(int)work->X/DATA_SIZE];

    //表示座標を設定
    work->Human.X = work->X - HUMAN_CENTER_X;
    work->Human.Y = work->Y - HUMAN_CENTER_Y;

    //キャラクターの表示
    SpriteDraw( &work->Human,0);

    for( loop=0;loop < HEIGHT_DATA; loop++)
    { //地形データの表示
        work->GrandBar.X = loop * DATA_SIZE;
        work->GrandBar.Y = work->HeightData[loop];
        SpriteDraw( &work->GrandBar,1);
    }
}



////////////////////////////////////////////////////
//６章－２３ 階層のある地面に着地するには
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE         Land;
    float          Acc;
    int            JumpFlag;
} EX06_23_STRUCT;

#define GRAND            (SCREEN_HEIGHT / 2)  //地面座標
#define LAND_AREA_TOP    GRAND - 144          //着地する階層領域
#define LAND_AREA_BOTTOM GRAND - 128
#define JUMP_POWER  10.0  //ジャンプ力
#define GRAVITY  0.25     //重力値

void init06_23(TCB* thisTCB)
{
    EX06_23_STRUCT* work = (EX06_23_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[1] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = GRAND;
    //着地する階層
    work->Land.X = SCREEN_WIDTH  / 2;
    work->Land.Y = 216;
}

void exec06_23(TCB* thisTCB)
{
    EX06_23_STRUCT* work = (EX06_23_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = { 128, 128, 640, 480, };

    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {//着地時の処理
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }
        //もし上階層で、下キーを入力していたら、落下して階層から降りる
        if( g_DownInputBuff & KEY_DOWN  )
        {//落下処理開始
            work->Acc = 0;
            work->JumpFlag = true;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //階層への着地処理、まず下降中かをチェックする
        if( work->Acc > 0.0 )
        {//もし下降中に階層のエリアに達したら、着地したとみなす
            if( work->sprt.Y > LAND_AREA_TOP && work->sprt.Y < LAND_AREA_BOTTOM )
            {//ただし、下キーを入力しているならば、階層から降りる処理の為、何もしない
                if( !(g_InputBuff & KEY_DOWN))
                {//階層上への着地処理
                    work->sprt.Y =  LAND_AREA_TOP;
                    work->JumpFlag = false;
                }
            }
        }

        //地面の着地処理
        if( work->sprt.Y > GRAND)
        {//座標が、地面座標より下ならば、着地したとみなす
            work->sprt.Y =  GRAND;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //階層表示
    SpriteDraw( &work->Land, 1);

    g_pFont->DrawText( NULL, "Ｚキーでジャンプ、下キーで階層から降りる", -1, &font_pos, DT_LEFT, 0xffffffff);
}



////////////////////////////////////////////////////
//６章－２４ 移動する物体に着地するには
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE         Land;
    float          LandMoveCount;
    float          Acc;
    int            JumpFlag;
} EX06_24_STRUCT;

#define GRAND            (SCREEN_HEIGHT / 2)  //地面座標
#define HUMAN_WIDTH      64
#define HUMAN_HEIGHT     128
#define LAND_MOVE_SPEED  0.05;
#define LAND_WIDTH       64
#define LAND_HEIGHT      16
#define JUMP_POWER  10.0  //ジャンプ力
#define GRAVITY  0.25     //重力値

void init06_24(TCB* thisTCB)
{
    EX06_24_STRUCT* work = (EX06_24_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[1] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = GRAND;
    //着地する物体
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

    //物体の移動
    work->LandMoveCount += LAND_MOVE_SPEED;
    work->Land.X = sin( work->LandMoveCount ) * 96 + (SCREEN_WIDTH / 2);
    //物体の大きさと座標情報を計算
    land_rect.left  = work->Land.X;
    land_rect.right = work->Land.X + LAND_WIDTH;
    land_rect.top   = work->Land.Y;
    land_rect.bottom= work->Land.Y + LAND_HEIGHT;
    //足元の座標を計算
    my_pos_x = work->sprt.X + HUMAN_WIDTH  / 2 ;
    my_pos_y = work->sprt.Y + HUMAN_HEIGHT + 1;

    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {//着地時の処理
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;
        }else{
            //足元をチェックし、もし何も無いのであれば落下を開始する
            if( !(my_pos_x > land_rect.left  && my_pos_x < land_rect.right  && 
                  my_pos_y > land_rect.top   && my_pos_y < land_rect.bottom ))
            {//落下処理開始
                work->Acc = 0;
                work->JumpFlag = true;
            }
        }
    }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;

        //階層への着地処理、まず下降中かをチェックする
        if( work->Acc > 0.0 )
        {//もし下降中に物体に接触したら、着地したとみなす
           if( my_pos_x > land_rect.left  && my_pos_x < land_rect.right  && 
               my_pos_y > land_rect.top   && my_pos_y < land_rect.bottom )
           {//物体上への着地処理
                //物体の座標を着地座標にする
                work->sprt.Y =  land_rect.top - HUMAN_HEIGHT;
                work->JumpFlag = false;
           }
        }

        //地面の着地処理
        if( work->sprt.Y > GRAND)
        {//座標が、地面座標より下ならば、着地したとみなす
            work->sprt.Y =  GRAND;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //物体の表示
    SpriteDraw( &work->Land, 1);

    g_pFont->DrawText( NULL, "Ｚキーでジャンプ、方向キーで移動", -1, &font_pos, DT_LEFT, 0xffffffff);
}



////////////////////////////////////////////////////
//６章－２５ ジャンプの頂点位置を知る
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human.png",&g_pTex[0] );

    //座標初期化
    work->sprt.X = SCREEN_WIDTH  / 2;
    work->sprt.Y = SCREEN_HEIGHT / 2;
}

void exec06_25(TCB* thisTCB)
{
#define JUMP_POWER  10.0  //ジャンプ力
#define GRAVITY  0.25     //重力値

    EX06_25_STRUCT* work = (EX06_25_STRUCT*)thisTCB->Work;
    float jump_height;
    float chk_height;
    float tmp_Acc;

    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //ジャンプ中は反応しない
    if( !work->JumpFlag )
    {//着地時の処理
        //キー入力でジャンプ処理
        if( g_DownInputBuff & KEY_Z    )
        {//ジャンプ開始時処理
            work->Acc = -JUMP_POWER;
            work->JumpFlag = true;

            //最高高度記録用に初期化
            work->JumpHeightMax = work->sprt.Y;

            tmp_Acc = work->Acc;
            jump_height = work->sprt.Y;
            //チェックの初期値はjump_heightより大きければOK
            chk_height = jump_height + 1.0;
            //高度のチェック
            //高度が下がり始めたらループの終了
            while(jump_height < chk_height)
            {//先立ってジャンプの計算を行なう
                chk_height = jump_height;
                tmp_Acc += GRAVITY;
                jump_height += tmp_Acc;
            }
            //表示用の変数に記録
            work->JumpHeight = chk_height;
        }
     }

    if( work->JumpFlag )
    {
        work->Acc += GRAVITY;
        work->sprt.Y += work->Acc;
        //ジャンプ中の最高高度を記録
        if(work->JumpHeightMax > work->sprt.Y)
            work->JumpHeightMax = work->sprt.Y;

        //着地したらジャンプ処理終了
        if( work->sprt.Y > SCREEN_HEIGHT / 2)
        {//座標が、地面座標より下ならば、着地したとみなす
            work->sprt.Y =  SCREEN_HEIGHT / 2;
            work->JumpFlag = false;
        }
    }

    SpriteDraw( &work->sprt, 0);

    //高さ表示
    sprintf( str,
        "予想高さ:%f\n実測高さ:%f",
        work->JumpHeight,
        work->JumpHeightMax);

    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}



////////////////////////////////////////////////////
//７章  処理
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//７章－１  キャラのアニメーション
////////////////////////////////////////////////////
void init07_01(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\ANIM.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0022.png",&g_pTex[1] );
}

void exec07_01(TCB* thisTCB)
{
#define ANIM_FRAME_MAX 4          //アニメーションのパターン数
#define ANIM_CHANGE_CNT 20        //アニメーションを切り替えるフレーム数

RECT  anim_table[] = 
{ {  0,  0,  64, 128,},    //１枚目のアニメパターン
  { 64,  0, 128, 128,},    //２枚目のアニメパターン
  {128,  0, 192, 128,},    //３枚目のアニメパターン
  {192,  0, 256, 128,},    //４枚目のアニメパターン
};

    //タスクにワークを割り付ける
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;

    pspr->timer += 1.0;

    if(pspr->timer >= ANIM_CHANGE_CNT)
    {
       //タイマーを戻す
       pspr->timer -= ANIM_CHANGE_CNT;
       //表示パターンを１パターン進める 
       pspr->Frame++;
       if( pspr->Frame >= ANIM_FRAME_MAX )
       {//表示パターンを最初に巻き戻す
           pspr->Frame = 0;
       } 
    }

    pspr->SrcRect = &anim_table[pspr->Frame];
    pspr->X = 320;
    pspr->Y = 240;
    SpriteDraw(pspr,0);
}


////////////////////////////////////////////////////
//７章－２ キャラクターを動かす
////////////////////////////////////////////////////
void init07_02(TCB* thisTCB)
{
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;//ワークを割り当てる

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //初期座標
    pspr->X = 320;
    pspr->Y = 240;
}
void exec07_02(TCB* thisTCB)
{
//移動速度
#define MOVE_SPEED 2.0f

    SPRITE* pspr;
    float   direction;

    pspr = (SPRITE*)thisTCB->Work;//ワークを割り当てる

    switch( g_InputBuff & 0x0f )  //移動キーのみを抽出
    {//移動方向を算出する
        case 0x01: direction= atan2( -1.0,  0.0); break; //上
        case 0x02: direction= atan2(  1.0,  0.0); break; //下
        case 0x04: direction= atan2(  0.0, -1.0); break; //左
        case 0x08: direction= atan2(  0.0,  1.0); break; //右

        case 0x05: direction= atan2( -1.0, -1.0); break; //左上
        case 0x06: direction= atan2(  1.0, -1.0); break; //左下
        case 0x09: direction= atan2( -1.0,  1.0); break; //右上
        case 0x0A: direction= atan2(  1.0,  1.0); break; //右下

        default:  SpriteDraw(pspr,0); return;
    }
    //移動方向から移動値を取得し実際の移動処理を行なう
    pspr->X += cos(direction)*MOVE_SPEED;
    pspr->Y += sin(direction)*MOVE_SPEED;

    SpriteDraw(pspr,0);

}


////////////////////////////////////////////////////
//７章－３ 画面内での移動
////////////////////////////////////////////////////
void init07_03(TCB* thisTCB)
{
    SPRITE* pspr;
    pspr = (SPRITE*)thisTCB->Work;//ワークを割り当てる

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //初期座標
    pspr->X = 320;
    pspr->Y = 240;
}
void exec07_03(TCB* thisTCB)
{
//移動速度
#define MOVE_SPEED 8.0f

    SPRITE* pspr;
    float   direction;
    RECT size_rect = { 0, 0, 64, 64, };
    RECT hit_rect;

    pspr = (SPRITE*)thisTCB->Work;//ワークを割り当てる

    switch( g_InputBuff & 0x0f )  //移動キーのみを抽出
    {
        case 0x01: direction= atan2( -1.0,  0.0); break; //上
        case 0x02: direction= atan2(  1.0,  0.0); break; //下
        case 0x04: direction= atan2(  0.0, -1.0); break; //左
        case 0x08: direction= atan2(  0.0,  1.0); break; //右

        case 0x05: direction= atan2( -1.0, -1.0); break; //左上
        case 0x06: direction= atan2(  1.0, -1.0); break; //左下
        case 0x09: direction= atan2( -1.0,  1.0); break; //右上
        case 0x0A: direction= atan2(  1.0,  1.0); break; //右下

        default:  SpriteDraw(pspr,0); return;
    }
    //移動処理
    pspr->X += cos(direction)*MOVE_SPEED;
    pspr->Y += sin(direction)*MOVE_SPEED;

    //画面外判定チェックの為、キャラの大きさの矩形を計算し
    //一旦構造体に格納する
    hit_rect.top    =   pspr->Y + size_rect.top;
    hit_rect.bottom =   pspr->Y + size_rect.bottom;
    hit_rect.left   =   pspr->X + size_rect.left;
    hit_rect.right  =   pspr->X + size_rect.right;

    //上下左右、それぞれの方向に対して画面外かどうかをチェックし
    //それ以上移動しないようにする
    if(hit_rect.left   < 0)
    {//左
        pspr->X = 0;
    }
    if(hit_rect.top    < 0)
    {//上
        pspr->Y = 0;
    }

    if(hit_rect.right  > SCREEN_WIDTH)
    {//右
        pspr->X = SCREEN_WIDTH - size_rect.right;
    }
    if(hit_rect.bottom > SCREEN_HEIGHT)
    {//下
        pspr->Y = SCREEN_HEIGHT - size_rect.bottom;
    }

    SpriteDraw(pspr,0);
}




////////////////////////////////////////////////////
//７章－４  複数同時プレイ
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

    //入力値をプレイヤーによって変更する
    switch( PlayerID )
    {//キーボードをプレイヤー１、ジョイスティックをプレイヤー２とする
       case PLAYER_1: input_key = g_KeyInputBuff; break;
       case PLAYER_2: input_key = g_JoystickBuff; break;
    }

    return input_key;
}

void exec07_04_myship(TCB* thisTCB)
{
    EX07_04_STRUCT* work = (EX07_04_STRUCT*)thisTCB->Work;
    unsigned char input_buff;

    //プレイヤーIDによる入力
    input_buff = EX07_04_player_input( work->PID );

    //キー入力による移動
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );

    //自機を２機作成する
    for( loop = 0;loop < 2; loop++ )
    {
        myship_tcb = TaskMake( exec07_04_myship, 0x2000 );
        myship_work = (EX07_04_STRUCT*)myship_tcb->Work;
        //プレイヤーにより初期表示座標を変える
        myship_work->MyShip.X = 
            ((SCREEN_WIDTH  / 2) - 64) + loop * 128;
        myship_work->MyShip.Y = 
            SCREEN_HEIGHT / 2;
        //プレイヤーIDを登録
        myship_work->PID = loop;
    }
}

void exec07_04(TCB* thisTCB)
{
    //複数同時処理のため、サンプルではメイン処理は何もしない
}




////////////////////////////////////////////////////
//７章－５  キャラクタの機数を管理する
////////////////////////////////////////////////////

#define DEFAULT_SHIP   3
#define BONUS_SCORE    1000

typedef struct{
    int            Score;
    int            MyShipCount;
    int            NextScore;     //次に残機が増えるスコア
    int            GameOver;
} EX07_05_STRUCT;


void init07_05(TCB* thisTCB)
{
    EX07_05_STRUCT* work = (EX07_05_STRUCT*)thisTCB->Work;

    //残機数と、ボーナススコアの初期化
    work->MyShipCount = DEFAULT_SHIP;
    work->NextScore = BONUS_SCORE;
}

void exec07_05(TCB* thisTCB)
{
    EX07_05_STRUCT* work = (EX07_05_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //Ｚキーでスコアを加算
    if( g_DownInputBuff & KEY_Z ) work->Score += 100;
    //Ｘキーで残機を１機減らす
    if( g_DownInputBuff & KEY_X )
    {
        work->MyShipCount--;
        if( work->MyShipCount < 0 )
        {//ゲームオーバー処理
             work->GameOver = true;
        }
    }

    //スコアによる残機増加処理
    if( work->Score >= work->NextScore )
    {
        work->MyShipCount++;
        work->NextScore += BONUS_SCORE;
    }

    //スコアと、機数の表示
    sprintf( str,"SCORE: %8d  残り機数:%2d\n次のボーナススコア:%8d",work->Score,work->MyShipCount,work->NextScore);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    if( work->GameOver )
    {//ゲームオーバー時にメッセージ表示
        font_pos.top += 48;
        g_pFont->DrawText( NULL, "ＧＡＭＥ　ＯＶＥＲ！！", -1, &font_pos, DT_LEFT, 0xffffffff);
    }
}



////////////////////////////////////////////////////
//７章－６ メニューの表示
////////////////////////////////////////////////////
#define MENU_INDEX  5
//メニューメッセージの高さ
#define MENU_HEIGHT  16

typedef struct{
    int (*MenuFunc)(int);            //メニュー実行関数
    char*          MenuName;         //メニュー名
    char*          HelpMess;         //メニュー選択時に表示するヘルプメッセージ
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
    //使用するテクスチャの読み込み
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
    {//メニュー１
      EX07_06_add,
      "メニュー１：加算",
      "サンプル用メニュー１です。値を100加算します。",
    },{//メニュー２
      EX07_06_sub,
      "メニュ－２：減算",
      "サンプル用メニュー２です。100減算します。",
    },{//メニュー３
      EX07_06_mult,
      "メニュー３:乗算",
      "３つ目のメニューです。値を２倍します。",
    },{//メニュー４
      EX07_06_div,
      "メニュー４:除算",
      "４つ目の選択メニュー。値を半分にします。",
    },{//メニュー５
      NULL,
      "終了",
      "関数ポインタをNULLにして、終了扱いにします。（終了処理は実装していません）",
    },
};


    if( g_InputBuff & KEY_X  )
    {//Ｘキーを押しながらだと、メニューの移動
        if( g_InputBuff & KEY_UP    ) work->MenuY -= 4;
        if( g_InputBuff & KEY_DOWN  ) work->MenuY += 4;
        if( g_InputBuff & KEY_RIGHT ) work->MenuX += 4;
        if( g_InputBuff & KEY_LEFT  ) work->MenuX -= 4;
    } else {
      //押していない時はメニューの選択
        if( g_DownInputBuff & KEY_DOWN )
        {
            work->SelectMenu++;
            //メニュー数チェック
            if( work->SelectMenu >= MENU_INDEX )work->SelectMenu = 0;
        }
        if( g_DownInputBuff & KEY_UP  )
        {
            work->SelectMenu--;
            if( work->SelectMenu < 0 ) work->SelectMenu = MENU_INDEX-1;
        }

        if( g_DownInputBuff & KEY_Z  )
        {//Ｚキーで選択中のメニューの実行
            if( menu_data[ work->SelectMenu ].MenuFunc == NULL )
            {//ただし、実行する関数が無い場合は終了
                //メニュー終了処理
            }else{
                //メニュー内容の実行
                work->Num = menu_data[ work->SelectMenu ].MenuFunc( work->Num );
            }
        }
    }

    //選択マークの位置を決定
    work->SelectMark.X = work->MenuX;
    work->SelectMark.Y = work->MenuY + work->SelectMenu * MENU_HEIGHT;
    //選択マークの表示
    SpriteDraw(&work->SelectMark,0);

    //メニューの表示
    font_pos.top  = work->MenuY;
    font_pos.left = work->MenuX + MENU_HEIGHT;
    for(loop = 0; loop < MENU_INDEX; loop++)
    {
        g_pFont->DrawText( NULL, menu_data[ loop ].MenuName, -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += MENU_HEIGHT;
    }

    //下段の固定位置にメニューによる数値変化とメニュー詳細内容の表示
    font_pos.top  = 448;
    font_pos.left = 32;
    sprintf( str,"現在の数値： %d\n%s",work->Num, menu_data[ work->SelectMenu ].HelpMess);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//７章－７ 進行方向の向きを知る
////////////////////////////////////////////////////

void init07_07(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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

    //移動前の座標を保存
    beforeX = work->X;
    beforeY = work->Y;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    //移動前の座標と現在の座標との相対座標を計算
    posX = work->X - beforeX;
    posY = work->Y - beforeY;

    //相対座標から方向を計算
    if( posX == 0 && posY == 0 )
    {//座標が移動していなかった時は方向が定まらないため、特別扱い
        direction = 0.0;
    }else{
        direction = atan2( posY , posX );
        direction = -direction / M_PI * 180.0;
    } 

    //方向の表示
    sprintf( str,"方向  %f",direction);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw( work, 0);
}





////////////////////////////////////////////////////
//７章－８ 目標を狙って弾を発射する
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    SPRITE*        Target;               //目標のスプライト
    int            Time;
    float          AddX;                 //増分値Ｘ
    float          AddY;                 //増分値Ｙ
} EX07_08_STRUCT ;

void exec07_08_bullet(TCB* thisTCB)
{
#define MOVE_SPEED  24.0
    EX07_08_STRUCT* work = (EX07_08_STRUCT*)thisTCB->Work;
    //自弾の座標
    float my_X;
    float my_Y;
    //目標の座標
    float targetX;
    float targetY;
    //目標の方向
    float direction;

    //初期化処理、一定時間ごとに初期化される
    if( work->Time++ == 30 )
    {//一定時間ごとに座標と増分値を計算
        work->sprt.X = SCREEN_WIDTH  / 2;
        work->sprt.Y = SCREEN_HEIGHT / 2;
        work->Time = 0;

        //弾の座標を取得
        my_X    = work->sprt.X;
        my_Y    = work->sprt.Y;
        //目標の自機座標を取得
        targetX = work->Target->X + 16;
        targetY = work->Target->Y + 16;

        //座標から相手への方向を計算
        direction = atan2( targetY - my_Y, targetX - my_X );
        //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
        work->AddX = cos( direction ) * MOVE_SPEED;
        work->AddY = sin( direction ) * MOVE_SPEED;
    }

    //座標に増分値を加算
    work->sprt.X += work->AddX;
    work->sprt.Y += work->AddY;

    SpriteDraw(&work->sprt,1);
}

void init07_08(TCB* thisTCB)
{
    TCB*    tmp_tcb;
    SPRITE* sprt;
    EX07_08_STRUCT* tmp_work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //自機のスプライト
    sprt = (SPRITE*)thisTCB->Work;
    sprt->X = SCREEN_WIDTH  / 2;
    sprt->Y = SCREEN_HEIGHT / 2 + 120;

    //初期化と、目標スプライトを設定
    tmp_tcb = TaskMake( exec07_08_bullet, 0x2000 );
    tmp_work = (EX07_08_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = sprt;

}

void exec07_08(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}


////////////////////////////////////////////////////
//７章－９ 弾を大量に発射する
////////////////////////////////////////////////////
typedef struct{
    SPRITE         sprt;
    int            Time;
    float          Direction;            //弾の方向
    float          AddX;                 //増分値Ｘ
    float          AddY;                 //増分値Ｙ
} EX07_09_STRUCT ;

void exec07_09_bullet(TCB* thisTCB)
{
#define MOVE_SPEED  16.0
    EX07_09_STRUCT* work = (EX07_09_STRUCT*)thisTCB->Work;

    if( work->Time == 0 )
    {   //座標の初期化
        work->sprt.X = SCREEN_WIDTH  / 2;
        work->sprt.Y = SCREEN_HEIGHT / 2;

        //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
        work->AddX = cos( work->Direction ) * MOVE_SPEED;
        work->AddY = sin( work->Direction ) * MOVE_SPEED;
    }
    //一定時間たったら消去する
    if( work->Time == 30 )
    {//弾を消去
        TaskKill( thisTCB );
    } else {
     //弾の進行処理

        work->Time++;
        //座標に増分値を加算
        work->sprt.X += work->AddX;
        work->sprt.Y += work->AddY;

        SpriteDraw(&work->sprt,1);
    }
}

void init07_09(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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

    //一定時間ごとに弾を発射
    if( *work_time == BULLET_CYCLE )
    {
        //弾を発射
        for( loop = 0;loop < BULLET_COUNT; loop++ )
        {
            tmp_tcb = TaskMake( exec07_09_bullet, 0x2000 );
            tmp_work = (EX07_09_STRUCT*)tmp_tcb->Work;
            tmp_work->Direction = bullet_direction;

            //発射方向を弾の発射数に合わせて少しづつずらしていく
            bullet_direction += M_PI * 2.0  / BULLET_COUNT;
        }
        *work_time = 0;
    }

    *work_time += 1;
}






////////////////////////////////////////////////////
//７章－１０ 自機の弾を撃つ
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Bullet;
    SPRITE         MyShip;
    int            BulletDraw;
} EX07_10_STRUCT ;

void init07_10(TCB* thisTCB)
{
    EX07_10_STRUCT* work = (EX07_10_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
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

    //キー入力による自機の移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    if( !work->BulletDraw )
    {//発射キー入力待ち
      if( g_DownInputBuff & KEY_Z )
      {//弾の発射処理
          //自機の座標をコピーして、そこから弾を発射する
          work->Bullet.X = work->MyShip.X + 16;
          work->Bullet.Y = work->MyShip.Y;

          work->BulletDraw = true;
      }
    }else{
       //弾の移動処理と表示
        work->Bullet.Y -= BULLET_SPEED;
        SpriteDraw( &work->Bullet,1);

        //弾が画面外に移動したら、キー入力待ちに戻る
        if( work->Bullet.Y < 0 ) work->BulletDraw = false;
    }

    SpriteDraw( &work->MyShip, 0);
}





////////////////////////////////////////////////////
//７章－１１  複数の弾を連続して発射する
////////////////////////////////////////////////////
typedef struct{
    SPRITE         MyShip;
} EX07_11_STRUCT ;

#define MOVE_SPEED    4.0
#define BULLET_SPEED  10.0

void exec07_11_bullet(TCB* thisTCB)
{
    SPRITE* work = (SPRITE*)thisTCB->Work;

     //弾が画面外に移動したら、処理の終了
    if( work->Y < 0 )
    {
        TaskKill(thisTCB);
        return;
    }

    //弾の移動処理と表示
    work->Y -= BULLET_SPEED;
    SpriteDraw( work,1);
}

void init07_11(TCB* thisTCB)
{
    EX07_11_STRUCT* work = (EX07_11_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
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

    //キー入力による自機の移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    if( g_DownInputBuff & KEY_Z )
    {//弾の発射処理
        //タスクを使用して弾を作成する
        bullet_tcb = TaskMake( exec07_11_bullet, 0x2000 );
        bullet_work = (SPRITE*)bullet_tcb->Work;

        //自機の座標をコピーして、そこから弾を移動させる
        bullet_work->X = work->MyShip.X + 16;
        bullet_work->Y = work->MyShip.Y;
    }

    SpriteDraw( &work->MyShip, 0);
}



////////////////////////////////////////////////////
//７章－１２ 発射に対して反動を付ける
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

     //弾が画面外に移動したら、処理の終了
    if( work->Y < 0 )
    {
        TaskKill(thisTCB);
        return;
    }

    //弾の移動処理と表示
    work->X += 0;
    work->Y += -BULLET_SPEED;
    SpriteDraw( work,1);
}

void init07_12(TCB* thisTCB)
{
    EX07_12_STRUCT* work = (EX07_12_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
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
    {//操作可能
        //キー入力による自機の移動
        if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
        if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
        if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
        if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;
    }else{
     //操作不可能時間
        work->LockTime--;
        //弾の発射方向とは反対方向に進む
        work->MyShip.X += -0.0;
        work->MyShip.Y += BULLET_SPEED / 4;
    }

    if( g_DownInputBuff & KEY_Z )
    {//弾の発射処理
        //タスクを使用して弾を作成する
        bullet_tcb = TaskMake( exec07_12_bullet, 0x2000 );
        bullet_work = (SPRITE*)bullet_tcb->Work;

        //自機の座標をコピーして、そこから弾を移動させる
        bullet_work->X = work->MyShip.X + 16;
        bullet_work->Y = work->MyShip.Y;
        //操作不可能時間を設定
        work->LockTime = LOCK_TIME;
    }

    SpriteDraw( &work->MyShip, 0);
}



////////////////////////////////////////////////////
//７章－１３ レーザーの動き
////////////////////////////////////////////////////
#define LASER_LENGTH 10
#define LASER_SPEED  16
typedef struct{
    SPRITE           MyShip;                     //自機
    int              LaserCount;                 //発射したレーザーの数
    int              DeleteLaserCount;           //消去したレーザーの数
    int              LaserMode;                  //レーザーの発射モード
    int              LaserDraw;                  //レーザーが画面内に残っているかのフラグ
} EX07_13_STRUCT;

typedef struct{
    EX07_13_STRUCT*  MyShipWork;     //自機情報へのポインタ
    float            PosY;           //自機からの相対座標
    SPRITE           Laser;          //レーザー表示用スプライト
} EX07_13_LASER;

void exec07_13_laser(TCB* thisTCB)
{
    EX07_13_LASER* work = (EX07_13_LASER*)thisTCB->Work;

    //レーザーを進める
    work->PosY -= LASER_SPEED;
    //レーザーの座標を自機の座標に追従させる
    work->Laser.X = work->MyShipWork->MyShip.X + 24;
    work->Laser.Y = work->PosY + work->MyShipWork->MyShip.Y;

    if( work->Laser.Y > -16 )
    {//レーザーの描画
        SpriteDraw( &work->Laser, 1);
    }else{
    //レーザーが画面外なら表示されている消去されたレーザーの数をカウントし消去
    //もしその際、作成したレーザーの数と消去したレーザーの数が同じなら画面からレーザーは消えている。
        work->MyShipWork->DeleteLaserCount++;
        if( work->MyShipWork->LaserCount == work->MyShipWork->DeleteLaserCount) work->MyShipWork->LaserDraw = FALSE;
        TaskKill( thisTCB );
    }
}

void init07_13(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\LASER.png",&g_pTex[1] );
}

void exec07_13(TCB* thisTCB)
{
#define MOVE_SPEED  4.0
    EX07_13_STRUCT* work = (EX07_13_STRUCT*)thisTCB->Work;
    TCB*  tmp_tcb;
    EX07_13_LASER*  tmp_work;


    //キー入力による自機の移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //発射キーが押されたらレーザー発射モードに切り替える
    if( g_DownInputBuff & KEY_Z )
    {//但し、すでに発射されたレーザーが画面内に残っていたら何もしない。
      if( !work->LaserDraw )
      {//レーザー発射関連の初期化
        work->LaserMode = TRUE;
        work->LaserCount = 0;
        work->DeleteLaserCount = 0;
      }
    }

    //レーザー発射モードであればレーザーを発射し続ける
    if( work->LaserMode )
    {//但し、レーザーの長さが一定量を超えているか、ボタンが離された場合、レーザーモードは終了
        if( work->LaserCount > LASER_LENGTH || g_UpInputBuff & KEY_Z )
        {
            work->LaserMode = FALSE;
        } else {
            //レーザー表示処理の作成
            work->LaserCount++;
            tmp_tcb = TaskMake( exec07_13_laser, 0x2000 );
            tmp_work = (EX07_13_LASER*)tmp_tcb->Work;
            tmp_work->MyShipWork = work;
            //レーザー表示中
            work->LaserDraw = TRUE;
        }
    }

    SpriteDraw( &work->MyShip, 0);

}



////////////////////////////////////////////////////
//７章－１４ 多方向へ弾を発射する
////////////////////////////////////////////////////
typedef struct{
    SPRITE*        Target;               //目標のスプライト
    int            Time;
} EX07_14_STRUCT;

typedef struct{
    SPRITE         Sprt;
    int            Time;
    float          AddX;                 //増分値Ｘ
    float          AddY;                 //増分値Ｙ
} EX07_14_BULLET;

void exec07_14_bullet(TCB* thisTCB)
{
    EX07_14_BULLET* work = (EX07_14_BULLET*)thisTCB->Work;

    //一定時間たったら消去する
    if( work->Time == 30 )
    {//弾を消去
        TaskKill( thisTCB );
    } else {
     //弾の進行処理

        work->Time++;
        //座標に増分値を加算
        work->Sprt.X += work->AddX;
        work->Sprt.Y += work->AddY;

        SpriteDraw(&work->Sprt,1);
    }
}

void exec07_14_shoot(TCB* thisTCB)
{
#define BULLET_CYCLE 15
#define BULLET_COUNT 5
#define BULLET_ANGLE M_PI / 16    //弾と弾の間の角度
#define MOVE_SPEED  16.0

    EX07_14_STRUCT* work = (EX07_14_STRUCT*)thisTCB->Work;
    EX07_14_BULLET* tmp_work;
    TCB*    tmp_tcb;
    int     loop;
    //弾の発射座標
    float shootX;
    float shootY;
    //目標の座標
    float targetX;
    float targetY;
    //弾を発射する方向
    float bullet_direction;

    //一定時間ごとに弾を発射
    if( work->Time == BULLET_CYCLE )
    {
        //画面中央から弾を発射
        shootX = SCREEN_WIDTH  / 2;
        shootY = SCREEN_HEIGHT / 2;
        //目標の座標を取得
        targetX = work->Target->X + 16;
        targetY = work->Target->Y + 16;

        //座標から弾の発射方向を計算
        bullet_direction = atan2( targetY - shootY, targetX - shootX );
        //最初に一気に方向を引いておくこの場合は５ＷＡＹなので２発分の角度を引く
        bullet_direction -= BULLET_ANGLE * (BULLET_COUNT / 2) ;
        //弾を発射
        for( loop = 0;loop < BULLET_COUNT; loop++ )
        {
            tmp_tcb = TaskMake( exec07_14_bullet, 0x2000 );
            tmp_work = (EX07_14_BULLET*)tmp_tcb->Work;

            //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
            tmp_work->AddX = cos( bullet_direction ) * MOVE_SPEED;
            tmp_work->AddY = sin( bullet_direction ) * MOVE_SPEED;
            //発射方向を少しづつずらしていく
            bullet_direction += BULLET_ANGLE;
            //弾の初期座標
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //自機のスプライト
    sprt = (SPRITE*)thisTCB->Work;
    sprt->X = SCREEN_WIDTH  / 2;
    sprt->Y = SCREEN_HEIGHT / 2 + 120;

    //弾発射を管理する処理を作成
    tmp_tcb = TaskMake( exec07_14_shoot, 0x2000 );
    tmp_work = (EX07_14_STRUCT*)tmp_tcb->Work;
    tmp_work->Target = sprt;

}

void exec07_14(TCB* thisTCB)
{
#define MOVE_SPEED  8.0
    SPRITE* work = (SPRITE*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->X -= MOVE_SPEED;

    SpriteDraw( work, 0);
}



////////////////////////////////////////////////////
//７章－１６ リプレイの方法
////////////////////////////////////////////////////
#define REPLAY_COUNT 50*5
typedef struct{
    SPRITE         Sprt;
    SPRITE*        Target;               //目標のスプライト
    int            Time;
    int            Time2;
    float          AddX;                 //増分値Ｘ
    float          AddY;                 //増分値Ｙ
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
    //自弾の座標
    float my_X;
    float my_Y;
    //目標の座標
    float targetX;
    float targetY;
    //目標の方向
    float direction;

    if( work->Time2++ > REPLAY_COUNT )
    {//一定時間処理を行なったら終了
        TaskKill( thisTCB );
    }else{
        //初期化処理、一定時間ごとに初期化される
        if( work->Time++ == 30 )
        {//一定時間ごとに座標と増分値を計算
            work->Sprt.X = SCREEN_WIDTH  / 2;
            work->Sprt.Y = SCREEN_HEIGHT / 2;
            work->Time = 0;

            //弾の座標を取得
            my_X    = work->Sprt.X;
            my_Y    = work->Sprt.Y;
            //目標の自機座標を取得
            targetX = work->Target->X + 16;
            targetY = work->Target->Y + 16;

            //座標から相手への方向を計算
            direction = atan2( targetY - my_Y, targetX - my_X );
            //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
            work->AddX = cos( direction ) * MOVE_SPEED;
            work->AddY = sin( direction ) * MOVE_SPEED;
        }

        //座標に増分値を加算
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
    {//一定時間処理を行なったら終了
        TaskKill( thisTCB );
    }else{
        //リプレイ用にバッファに対応したキー入力による移動
        if( g_EX07_16_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
        if( g_EX07_16_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
        SpriteDraw( &work->Sprt, 0);
    }
}

void init07_16(TCB* thisTCB)
{
    //使用するテクスチャの読み込み
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
    {//入力待ち状態
        //Ｚキーでプレイモードに移行
        if( g_DownInputBuff & KEY_Z    ) work->PlayMode = MODE_PLAY;
        //Ｘキーでリプレイモードに移行
        if( g_DownInputBuff & KEY_X    ) work->PlayMode = MODE_REPLAY;

        //ボタンの入力があれば各処理を作成
        if( g_DownInputBuff & (KEY_Z | KEY_X) )
        {
            //自機のスプライト
            myship_tcb = TaskMake( exec07_16_MyShip, 0x2000 );
            myship_work = (EX07_16_MYSHIP*)myship_tcb->Work;
            myship_work->Sprt.X = SCREEN_WIDTH  / 2;
            myship_work->Sprt.Y = SCREEN_HEIGHT / 2 + 120;

            //ボールの初期化と、目標スプライトを設定
            ball_tcb = TaskMake( exec07_16_ball, 0x3000 );
            ball_work = (EX07_16_BALL*)ball_tcb->Work;
            ball_work->Target = &myship_work->Sprt;

        }
        //記録・再生用のタイマーを初期化
        work->Time = 0;
        g_pFont->DrawText( NULL, "Ｚキーで通常プレイ、Ｘキーでリプレイ", -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    if( work->PlayMode == MODE_PLAY )
    {//通常プレイ、記録処理
        if( work->Time > REPLAY_COUNT )
        {//時間がきたらモードを切り替え記録終了
            work->PlayMode = MODE_WAIT;
        }else{
         //キー入力を記録
            g_EX07_16_InputBuff = g_InputBuff;
            work->ReplayBuff[ work->Time ] = g_EX07_16_InputBuff;
        }

        sprintf( str,"通常プレイ、キー入力記録中… %d",REPLAY_COUNT - work->Time);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

        //時間を更新
        work->Time++;
    }

    if( work->PlayMode == MODE_REPLAY )
    {//リプレイ、再生処理
        if( work->Time > REPLAY_COUNT )
        {//時間がきたらモードを切り替え再生終了
            work->PlayMode = MODE_WAIT;
        }else{
         //記録したキー入力を再生
            g_EX07_16_InputBuff = work->ReplayBuff[ work->Time ];
        }

        sprintf( str,"リプレイ中… %d",REPLAY_COUNT - work->Time);
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

        //時間を更新
        work->Time++;
    }
}



////////////////////////////////////////////////////
//７章－１７  レベルアップをするには
////////////////////////////////////////////////////
#define LV_MAX 10
#define EXP_RANGE 50
typedef struct{
    int            NowEXP;        //現在の経験値
    int            NowLV;         //現在のレベル
    int            NextEXP;       //次にレベルアップする経験値
} EX07_17_STRUCT;

static int LevelUP_EXP[] = 
{//レベルアップに必要な経験値のテーブル
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

    //レベルの初期化
    work->NowEXP = 0;
    work->NowLV  = 1;
    //最初のレベルアップに必要な経験値を取得、計算
    work->NextEXP += LevelUP_EXP[ work->NowLV ];
}

void exec07_17(TCB* thisTCB)
{
    EX07_17_STRUCT* work = (EX07_17_STRUCT*)thisTCB->Work;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //Ｚキーで経験値をランダムに加算
    if( g_DownInputBuff & KEY_Z )
    {//レベルが最大に達していたら、経験値を取得しないようにする
        if( work->NowLV < LV_MAX ) work->NowEXP += rand() % EXP_RANGE;
    }
    //レベルに対応した経験値を越えたらレベルアップ
    if( work->NowEXP >= work->NextEXP )
    {//レベルアップ処理
        //次のレベルアップに必要な経験値を取得、計算
        work->NextEXP += LevelUP_EXP[ work->NowLV ];
        //レベルアップ処理
        work->NowLV++;

        //レベルが最大時の処理
        if( work->NowLV >= LV_MAX )
        {
            work->NowEXP  = 0;
            work->NextEXP = 9999;
        }
    }
    //レベルと経験値の表示
    sprintf( str,"LEVEL:%2d\n現在の経験値　　　:%8d\n次LVまでの経験値:%8d",
             work->NowLV,work->NowEXP,work->NextEXP - work->NowEXP);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}

////////////////////////////////////////////////////
//７章－１８  スコアランキングの管理
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
{//初期スコアデータ
    { 10000,{'A','B','C','\0',},},
    {  5000,{'D','E','F','\0',},},
    {  3000,{'G','H','I','\0',},},
    {  1000,{'J','K','L','\0',},},
    {   500,{'M','N','O','\0',},},
};


int EX07_18_rank_check( int Score)
{
    int loop;

    //スコア登録、スコアの高い順にチェックしていく
    for( loop = 0; loop < RANK_COUNT; loop++ )
    {
        //もしランク内よりも高いスコアであれば、ランクイン
        if(Score > gEX07_18_ScoreData[ loop ].Score )break;
    }

    //ランクインかチェック
    if(loop != RANK_COUNT ) return loop;
    //ランク外
    return -1;
}


void EX07_18_score_regist( int Score, char* Name )
{
    int loop;
    int set_rank;

    //何位にランクインしているかを取得
    set_rank = EX07_18_rank_check( Score );

    for( loop = RANK_COUNT-1; loop > set_rank; loop-- )
    {//下位からランクを１つづつずらしていく
       gEX07_18_ScoreData[ loop ] = gEX07_18_ScoreData[ loop -1 ];
    }

    //スコアと名前を登録
    gEX07_18_ScoreData[ set_rank ].Score = Score;
    strcpy( gEX07_18_ScoreData[ set_rank ].Name, Name);
}


void exec07_18(TCB* thisTCB)
{
    EX07_18_STRUCT* work = (EX07_18_STRUCT*)thisTCB->Work;
    int loop;
    RECT font_pos = {  0,  0,640,480,};
    char str[128];

    //上キーでスコアを加算
    if( g_InputBuff & KEY_UP ) work->Score += 100;
    //下キーでスコアを減算
    if( g_InputBuff & KEY_DOWN )
    {
        work->Score -= 100;
        if( work->Score < 0 ) work->Score = 0;
    }
    //Ｚキーでスコア登録処理
    if( g_DownInputBuff & KEY_Z )
    {
        //ランクチェックを行い、ランクインならスコア登録処理
        if( 0 <= EX07_18_rank_check( work->Score ) )
        {
            EX07_18_score_regist( work->Score , "NEW" );
        }
    }

    //スコアデータの表示
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

    //現在のスコアの表示
    font_pos.top += 32;
    sprintf( str,"登録するスコア: %8d ",work->Score);
    g_pFont->DrawText( NULL, 
        str, 
        -1, 
        &font_pos, 
        DT_LEFT, 
        0xffffffff);

    font_pos.top += 32;
    g_pFont->DrawText( NULL,
        "Ｚキーでスコア登録\n方向キーでスコア変更", 
        -1, 
        &font_pos, 
        DT_LEFT, 
        0xffffffff);
}



////////////////////////////////////////////////////
//８章 当たり判定
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//８章－１ 相手との距離を測る
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_01_STRUCT;

void init08_01(TCB* thisTCB)
{
    EX08_01_STRUCT* work = (EX08_01_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //目標の初期座標
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //同一座標の場合エラーが出るので特別処理
    if( (work->Target.X == work->MyShip.X )&&(work->Target.Y == work->MyShip.Y) )
    {
        distance = 0;
    }else{
        //相手との距離を計測
        distance = sqrtf( (work->Target.X - work->MyShip.X) * (work->Target.X - work->MyShip.X) + 
                          (work->Target.Y - work->MyShip.Y) * (work->Target.Y - work->MyShip.Y) 
                        );
    }
    sprintf( str,"距離  %f",distance);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}


////////////////////////////////////////////////////
//８章－２ 円同士の当たり判定
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_02_STRUCT;

void init08_02(TCB* thisTCB)
{
    EX08_02_STRUCT* work = (EX08_02_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //目標の初期座標
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //接触のための判定値を計算
    distance = (work->Target.X - work->MyShip.X) * (work->Target.X - work->MyShip.X) + 
               (work->Target.Y - work->MyShip.Y) * (work->Target.Y - work->MyShip.Y) ;

    //判定値として距離の二乗を求めるが、同じ計算なので省略
    //check_distance = pow( sqrtf( ((HIT_SIZE1 - 0) * (HIT_SIZE1 - 0)) + ((HIT_SIZE2 - 0) * (HIT_SIZE2 - 0)) ) );
    check_distance = (HIT_SIZE1 * HIT_SIZE1) + (HIT_SIZE2 * HIT_SIZE2) ;
    //判定値以下なら接触
    if( distance < check_distance )
    {
        sprintf( str,"接触しました");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}




////////////////////////////////////////////////////
//８章－３ 矩形と点の当たり判定
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_03_STRUCT;

void init08_03(TCB* thisTCB)
{
    EX08_03_STRUCT* work = (EX08_03_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //目標の初期座標
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //判定チェックのための矩形を計算
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //対象が矩形の範囲内にあれば当たっている
    if( hit_rect.top    < work->Target.Y  && 
        hit_rect.bottom > work->Target.Y  &&
        hit_rect.left   < work->Target.X  &&
        hit_rect.right  > work->Target.X  )
    {
        sprintf( str,"接触しました");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}


////////////////////////////////////////////////////
//８章－４ 矩形同士の当たり判定
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
} EX08_04_STRUCT;

void init08_04(TCB* thisTCB)
{
    EX08_04_STRUCT* work = (EX08_04_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //目標の初期座標
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

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //判定チェックのための自機の当たり判定矩形Ａを計算
    hit_rectA.top    =   work->MyShip.Y + hit_size_rectA.top;
    hit_rectA.bottom =   work->MyShip.Y + hit_size_rectA.bottom;
    hit_rectA.left   =   work->MyShip.X + hit_size_rectA.left;
    hit_rectA.right  =   work->MyShip.X + hit_size_rectA.right;

    //判定チェックのための目標の当たり判定矩形Ｂを計算
    hit_rectB.top    =   work->Target.Y + hit_size_rectB.top;
    hit_rectB.bottom =   work->Target.Y + hit_size_rectB.bottom;
    hit_rectB.left   =   work->Target.X + hit_size_rectB.left;
    hit_rectB.right  =   work->Target.X + hit_size_rectB.right;

    //対象枠全てが対となる矩形の範囲内にあれば当たっている
    if( hit_rectA.top    < hit_rectB.bottom && 
        hit_rectA.bottom > hit_rectB.top    &&
        hit_rectA.left   < hit_rectB.right  &&
        hit_rectA.right  > hit_rectB.left   )
    {
        sprintf( str,"接触しました");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}





////////////////////////////////////////////////////
//８章－５ 自機に弾をかすらせて得点
////////////////////////////////////////////////////

typedef struct{
    SPRITE         MyShip;
    SPRITE         Target;
    int            Score;
} EX08_05_STRUCT;

void init08_05(TCB* thisTCB)
{
    EX08_05_STRUCT* work = (EX08_05_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //目標の初期座標
    work->Target.X = SCREEN_WIDTH  / 2;
    work->Target.Y = SCREEN_HEIGHT / 2;
}

void exec08_05(TCB* thisTCB)
{
#define MOVE_SPEED    8.0
#define TARGET_SPEED  10.0
#define HIT_SIZE      16.0       //当たり判定の大きさ
#define GRAZE_DISTANCE  64.0    //かすりの距離

    EX08_05_STRUCT* work = (EX08_05_STRUCT*)thisTCB->Work;
    float distance;
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;


    //目標の移動
    work->Target.Y += TARGET_SPEED;
    if( work->Target.Y > SCREEN_HEIGHT)
    {
        work->Target.Y = 0;
        work->Target.X = SCREEN_WIDTH/2;
    }
    //同一座標の場合エラーが出るので特別処理
    if( (work->Target.X == work->MyShip.X )&&
        (work->Target.Y == work->MyShip.Y) )
    {
        distance = 0;
    }else{
        //相手との距離を計測
        distance = sqrtf( (work->Target.X-8 - work->MyShip.X-16) * 
                          (work->Target.X-8 - work->MyShip.X-16) + 
                          (work->Target.Y-8 - work->MyShip.Y-16) * 
                          (work->Target.Y-8 - work->MyShip.Y-16) 
                        );
    }


    //距離が一定以内なら掠っていると判定、スコアを加算
    if(distance <= GRAZE_DISTANCE)work->Score += 1;

    //接触したらスコアをクリア
    if(distance <= HIT_SIZE)work->Score = 0;


    sprintf( str,"スコア  %d", work->Score );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);

}





////////////////////////////////////////////////////
//８章－６ 回避ボム
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\spread.png",&g_pTex[2] );

    //目標の初期座標
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


    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //目標の移動
    work->Target.Y += TARGET_SPEED;
    if( work->Target.Y > SCREEN_HEIGHT)
    {
        work->Target.Y = 0;
        work->Target.X = SCREEN_WIDTH/2;
    }


    if( g_DownInputBuff & KEY_X  )
    {//Ｘキーでボム数回復
        work->BombCount = BOMB_MAX;
    }

    if(!work->BombFlag)
    {//ボムが発動していない時だけ処理
        //Ｚキーで爆発処理
        if( g_DownInputBuff & KEY_Z  )
        {//ここでは、ボムの発動と、ボム数の計算を行なう
            if(work->BombCount > 0)
            {//ただし、ボムがない場合のみ
                work->BombFlag = true;
                if(--work->BombCount < 0) work->BombCount = 0;
                //ボムヘ座標をコピー
                work->Bomb.X = work->MyShip.X;
                work->Bomb.Y = work->MyShip.Y;
            }
        }


        //判定チェックのための自機の当たり判定矩形Ａを計算
        hit_rectA.top    =   work->MyShip.Y + hit_size_rectA.top;
        hit_rectA.bottom =   work->MyShip.Y + hit_size_rectA.bottom;
        hit_rectA.left   =   work->MyShip.X + hit_size_rectA.left;
        hit_rectA.right  =   work->MyShip.X + hit_size_rectA.right;

        //判定チェックのための目標の当たり判定矩形Ｂを計算
        hit_rectB.top    =   work->Target.Y + hit_size_rectB.top;
        hit_rectB.bottom =   work->Target.Y + hit_size_rectB.bottom;
        hit_rectB.left   =   work->Target.X + hit_size_rectB.left;
        hit_rectB.right  =   work->Target.X + hit_size_rectB.right;

        //弾に当たった場合、回避処理としてボムの発動
        if( hit_rectA.top    < hit_rectB.bottom && 
            hit_rectA.bottom > hit_rectB.top    &&
            hit_rectA.left   < hit_rectB.right  &&
            hit_rectA.right  > hit_rectB.left   )
        {//ボタン押しと同様に、発動処理と計算を行なう

            //ただし、ボムがない場合は命中のメッセージを表示
            if(work->BombCount <= 0)
            {
                sprintf( str,"接触しました");
                g_pFont->DrawText( NULL,
                         str, -1, &font_pos,
                          DT_LEFT, 0xffffffff);
            }
            if(work->BombCount > 0)
            {
                work->BombFlag = true;
                //ボムヘ座標をコピー
                work->Bomb.X = work->MyShip.X;
                work->Bomb.Y = work->MyShip.Y;

                if(--work->BombCount < 0) work->BombCount = 0;
            }
        }
    }


    if(work->BombFlag)
    {//ボム発動時の処理
        //ボムの表示
        work->Bomb.SrcRect = &bomb_data;

        SpriteDraw(&work->Bomb,2);

        //ボムの発動時間を進める
        work->BombTime++;
        //指定の時簡に達していれば終了
        if(work->BombTime >= BOMB_TIME)
        {//終了処理
            work->BombTime = 0;
            work->BombFlag = false;
        }
    }



    font_pos.top +=16 ;
    sprintf( str,"残ボム数 ＝ %d",work->BombCount);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

    SpriteDraw(&work->MyShip,0);
    SpriteDraw(&work->Target,1);
}



////////////////////////////////////////////////////
//８章－７ スピードアップ時の当たり判定
////////////////////////////////////////////////////
typedef struct{
    SPRITE         Ball;
    SPRITE         Target;
    RECT           OldHitRect;
} EX08_07_STRUCT;

int EX08_07_hit_check_double( RECT NowHitRect, RECT BeforeRect, float checkX, float checkY )
{
    RECT HalfHitRect;

    //判定チェックのための矩形を計算
    HalfHitRect.top    =   (NowHitRect.top   + BeforeRect.top   ) / 2;
    HalfHitRect.bottom =   (NowHitRect.bottom+ BeforeRect.bottom) / 2;
    HalfHitRect.left   =   (NowHitRect.left  + BeforeRect.left  ) / 2;
    HalfHitRect.right  =   (NowHitRect.right + BeforeRect.right ) / 2;

    //対象と２つの当たり判定の矩形を比較
      //１つ目の矩形（１つ前の座標との中間位置）
    if((HalfHitRect.top   < checkY  && 
        HalfHitRect.bottom> checkY  &&
        HalfHitRect.left  < checkX  &&
        HalfHitRect.right > checkX  )||
      //２つ目の矩形（現在の当たり判定）
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
    //対象と当たり判定の矩形を比較
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[0] );

    //目標の初期座標
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

    //キー入力による移動
    if( g_DownInputBuff & KEY_UP    ) work->Ball.Y -= MOVE_SPEED;
    if( g_DownInputBuff & KEY_DOWN  ) work->Ball.Y += MOVE_SPEED;
    if( g_DownInputBuff & KEY_RIGHT ) work->Ball.X += MOVE_SPEED;
    if( g_DownInputBuff & KEY_LEFT  ) work->Ball.X -= MOVE_SPEED;

    //判定チェックのための矩形を計算
    hit_rect.top    =   work->Ball.Y + hit_size_rect.top;
    hit_rect.bottom =   work->Ball.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->Ball.X + hit_size_rect.left;
    hit_rect.right  =   work->Ball.X + hit_size_rect.right;


    //Ｚキーを押しながらだと当たり判定を変える
    if( g_InputBuff & KEY_Z )
    {//対象が現在と過去の矩形の範囲内にあれば当たっている
        hit_flag = EX08_07_hit_check_double( hit_rect,  work->OldHitRect,
                                      work->Target.X+TARGET_CENTER,  //中心位置も加算
                                      work->Target.Y+TARGET_CENTER);
    }else{
     //現在の当たり矩形のみをチェック
        hit_flag = EX08_07_hit_check( hit_rect,
                           work->Target.X+TARGET_CENTER,             //目標の中心位置も加算
                           work->Target.Y+TARGET_CENTER);
    }
    //接触時のチェック
    if(hit_flag)
    {
        sprintf( str,"接触しました");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    //現在の当たり矩形を保存
    work->OldHitRect = hit_rect;

    SpriteDraw(&work->Ball,0);
    SpriteDraw(&work->Target,0);
}


////////////////////////////////////////////////////
//８章－９ キャラクター同士の詳細な当たり判定
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

    //２つの当たり矩形同士を１つづつ総当りで比較する
    while( HitRect[hit_loop].top != END_CODE )
    {
        target_loop = 0;
        while( TargetRect[target_loop].top != END_CODE )
        {//対象枠全てに対して当たりをチェック、どれか１つでも当たっていれば終了
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
    {//元座標をから、当たり判定用の矩形を算出
        AnsRect[loop].top    = BaseHitRect[loop].top     + AddY;
        AnsRect[loop].bottom = BaseHitRect[loop].bottom  + AddY;
        AnsRect[loop].left   = BaseHitRect[loop].left    + AddX;
        AnsRect[loop].right  = BaseHitRect[loop].right   + AddX;
        loop++;
    }

    //判定の配列に終了コードを書き込む
    AnsRect[loop].top = -1;
}

void init08_09(TCB* thisTCB)
{
    EX08_09_STRUCT* work = (EX08_09_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\EX08_09_SPRT.png",&g_pTex[0] );

    //目標の初期座標
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
{//当たり判定のデータ
{ 24,  0, 39, 63, },   //１つ目の当たり矩形
{  0, 16, 63, 23, },   //２つ目の当たり矩形
{  0, 56, 63, 63, },   //３つ目の当たり矩形
{ -1, -1, -1, -1, },   //４つ目(データは終了コード）
};
    RECT hit_rect[ RECT_COUNT ];
    RECT target_rect[ RECT_COUNT ];
    char str[128];
    RECT font_pos = { 0, 0, 640, 480, };

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;

    //判定チェックのための矩形を計算
    EX08_09_calc_rect( hit_rect, hit_size_rect, work->Sprt.X, work->Sprt.Y);
    //同様に目標の判定チェック矩形を計算
    EX08_09_calc_rect( target_rect, hit_size_rect, work->Target.X, work->Target.Y);

    //双方の当たり矩形リストをチェック
    if( EX08_09_hit_check( hit_rect, target_rect ) )
    {
        sprintf( str,"接触しました");
        g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    }

    SpriteDraw(&work->Sprt,0);
    SpriteDraw(&work->Target,0);
}



////////////////////////////////////////////////////
//８章－１０ 一定時間無敵
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

    //ボールの初期化と、座標を設定
    ball_tcb = TaskMake( exec08_10_ball , BALL_PRIORITY );
    ball_work = (EX08_10_BALL*)ball_tcb->Work;
    ball_work->Ball.X = rand() % SCREEN_WIDTH;
    ball_work->Ball.Y = 0;
    ball_work->AddY   = BALL_SPEED + rand() % BALL_SPEED_MAX;
}

void exec08_10_ball(TCB* thisTCB)
{
    EX08_10_BALL* work = (EX08_10_BALL*)thisTCB->Work;

     //画面上部から移動
    if( work->Ball.Y > SCREEN_HEIGHT )
    {
        //処理終了時にボールを作成する
        exec08_10_make_ball( );
        TaskKill(thisTCB);
        return;
    }

    //ボールの移動処理と表示
    work->Ball.X += work->AddX;
    work->Ball.Y += work->AddY;
    SpriteDraw( &work->Ball,1);
}


int EX08_10_hit_check( RECT* HitRect )
{
    TCB*    check_tcb;
    EX08_10_BALL* ball_work;

    //タスクリストを辿り、ボールのタスクを全部チェックする
    check_tcb = GetTaskTop();             //タスクの先頭アドレスを取得
    check_tcb = check_tcb->Next;          //タスクヘッドは処理せず飛ばす
    while( check_tcb->Prio != 0x0000)     //タスクヘッドにたどり着いたら１順
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//タスクがボールであれば、当たり判定をチェックする
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //対象と当たり判定の矩形を比較
            if( HitRect->top    < ball_work->Ball.Y + BALL_CENTER &&
                HitRect->bottom > ball_work->Ball.Y + BALL_CENTER &&
                HitRect->left   < ball_work->Ball.X + BALL_CENTER &&
                HitRect->right  > ball_work->Ball.X + BALL_CENTER )
            {
                return true;
            }
        }

        //次にチェックするタスク
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_10(TCB* thisTCB)
{
    int loop;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //ボールの作成
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


    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //判定チェックのための矩形を計算
    hit_rect.top    +=   work->MyShip.Y;
    hit_rect.bottom +=   work->MyShip.Y;
    hit_rect.left   +=   work->MyShip.X;
    hit_rect.right  +=   work->MyShip.X;

    if( work->InvincibleTime )
    {//無敵時間中は、当たり判定を行なわない
        //無敵時間を減少させる
        work->InvincibleTime--;
        g_pFont->DrawText( NULL,"無敵時間中！！" , -1, &font_pos, DT_LEFT, 0xffffffff);
    }else{
     //当たり判定をチェックし当たっていたら無敵時間を設定
        if( EX08_10_hit_check( &hit_rect ))
        {
            work->InvincibleTime = INVINCIBLE_TIME;
        }
    }

    //点滅処理と表示処理
    if(!(work->InvincibleTime & 0x01)) SpriteDraw(&work->MyShip,0);
}



////////////////////////////////////////////////////
//８章－１１ 広範囲ミサイル
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

    //タスクリストを辿り、ボールのタスクを全部チェックする
    check_tcb = GetTaskTop();             //タスクの先頭アドレスを取得
    check_tcb = check_tcb->Next;          //タスクヘッドは処理せず飛ばす
    while( check_tcb->Prio != 0x0000)     //タスクヘッドにたどり着いたら１順
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//タスクがボールであれば、当たり判定をチェックする
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //座標を物体の中心点に合わせる
            ball_X = ball_work->Ball.X + BALL_CENTER;
            ball_Y = ball_work->Ball.Y + BALL_CENTER;
            pos_X  = PosX + BOMB_CENTER;
            pos_Y  = PosY + BOMB_CENTER;

            //同一座標の場合エラーが出るので特別処理にする
            if( ball_X == pos_X && ball_Y == pos_Y )
            {//座標を書き換え強制的に画面外へ移動させ、消去
                ball_work->Ball.Y = SCREEN_HEIGHT;
            }else{
                //相手との距離を計測し、当たり範囲内なら画面外へ移動させ、消去
                distance = sqrtf( (ball_X - pos_X) * (ball_X - pos_X) + 
                                (ball_Y - pos_Y) * (ball_Y - pos_Y) );
                if( distance <= HitRange )  ball_work->Ball.Y = SCREEN_HEIGHT;
            }
        }
        //次にチェックするタスク
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_11(TCB* thisTCB)
{
    EX08_11_STRUCT* work = (EX08_11_STRUCT*)thisTCB->Work;
    int loop;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\spread.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //ボールの作成
    for( loop = 0; loop < BALL_COUNT; loop++)
    {//前項目の関数を再利用
        exec08_10_make_ball();
    }
    work->Bomb.X = SCREEN_WIDTH / 2;
    work->Bomb.Y = SCREEN_HEIGHT / 2;
}

void exec08_11(TCB* thisTCB)
{
    EX08_11_STRUCT* work = (EX08_11_STRUCT*)thisTCB->Work;
    float hit_range[ ANIM_COUNT ] = { 16.0, 32.0, 48.0, 64.0,};    //当たり判定の大きさ
    RECT anim_table[ ANIM_COUNT] = 
{
    {  0,  0, 64, 64,},   //１コマ目
    { 64,  0,128, 64,},   //２コマ目
    {128,  0,192, 64,},   //３コマ目
    {192,  0,256, 64,},   //４コマ目
};


    //Ｚキーで爆発処理開始
    if( g_DownInputBuff & KEY_Z  )
    {//ここでは、アニメーションの開始を行なうだけ
        work->AnimSpeed = ANIM_SPEED;
    }

    //アニメーションが始まっていれば表示
    if(work->AnimSpeed != 0.0)
    {
        //現在表示のフレームからアニメーション用の座標を取得
        work->Bomb.SrcRect = &anim_table[(int)work->AnimCount];

        //表示
        SpriteDraw(&work->Bomb,0);

        //爆発の処理、座標と範囲を渡す
        EX08_11_hit_check( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, hit_range[(int)work->AnimCount]);

        //アニメーションを進める
        work->AnimCount += work->AnimSpeed;
        //指定のコマ数に達していればアニメーション終了
        if(work->AnimCount >= ANIM_COUNT)
        {
            work->AnimCount = 0;
            work->AnimSpeed = 0;
        }
    }
}



////////////////////////////////////////////////////
//８章－１２ まとめて敵を吹き飛ばす
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

     //一定時間のみ移動
    if( work->Time++ > BALL_BASH_TIME )
    {
        //処理終了時にボールを作成する
        exec08_10_make_ball( );
        TaskKill(thisTCB);
        return;
    }
    //ボールの移動処理と表示
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


    //タスクリストを辿り、ボールのタスクを全部チェックする
    check_tcb = GetTaskTop();             //タスクの先頭アドレスを取得
    check_tcb = check_tcb->Next;          //タスクヘッドは処理せず飛ばす
    while( check_tcb->Prio != 0x0000)     //タスクヘッドにたどり着いたら１順
    {
        if( check_tcb->Prio == BALL_PRIORITY )
        {//タスクがボールであれば、当たり判定をチェックする
            ball_work = (EX08_10_BALL*)check_tcb->Work;
            //座標を物体の中心点に合わせる
            ball_X = ball_work->Ball.X + BALL_CENTER;
            ball_Y = ball_work->Ball.Y + BALL_CENTER;
            pos_X  = PosX + MYSHIP_CENTER;
            pos_Y  = PosY + MYSHIP_CENTER;

            //同一座標の場合エラーが出るので特別処理にする
            if( ball_X == pos_X && ball_Y == pos_Y )
            {
                ball_work->AddX = 0;
                ball_work->AddY = 0;
                TaskChange( check_tcb, EX08_12_bash_ball);
            }else{
                //相手との距離を計測
                distance = sqrtf( (ball_X - pos_X) * (ball_X - pos_X) + 
                                  (ball_Y - pos_Y) * (ball_Y - pos_Y) );
                if( distance <= HIT_RANGE )
                {//吹き飛ばしの範囲内であればボールの速度を自機とは反対の方向へ設定する
                    //座標から相手への方向を計算
                    direction = atan2( ball_Y - pos_Y, ball_X - pos_X );
                    //方向から、Ｘ、Ｙそれぞれの速度を計算
                    ball_work->AddX = cos( direction ) * BALL_BASH_SPEED;
                    ball_work->AddY = sin( direction ) * BALL_BASH_SPEED;
                    //外部から、ボール処理を吹き飛ばし処理に切り替える
                    TaskChange( check_tcb, EX08_12_bash_ball);
                 }
            }
        }
        //次にチェックするタスク
        check_tcb = check_tcb->Next;
    }

    return false;
}

void init08_12(TCB* thisTCB)
{
    int loop;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //ボールの作成
    for( loop = 0; loop < BALL_COUNT; loop++)
    {//前項目の関数を再利用
        exec08_10_make_ball();
    }
}

void exec08_12(TCB* thisTCB)
{
    EX08_12_STRUCT* work = (EX08_12_STRUCT*)thisTCB->Work;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->MyShip.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->MyShip.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //Ｚキーで吹き飛ばし処理
    if( g_DownInputBuff & KEY_Z  )
    {//当たり判定をチェックし吹き飛ばす処理
        EX08_12_hit_check( work->MyShip.X, work->MyShip.Y );
    }
    //表示
    SpriteDraw(&work->MyShip,0);
}



////////////////////////////////////////////////////
//８章－１３  キャラクター同士の当たり判定
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
{//矩形領域に座標を加算
    ans_rect->top    = add_rect->top    + y;
    ans_rect->bottom = add_rect->bottom + y;
    ans_rect->left   = add_rect->left   + x;
    ans_rect->right  = add_rect->right  + x;
}

int EX08_13_hit_rect( RECT* hit_rectA, RECT* hit_rectB )
{//２つの矩形の接触判定
    //対象枠全てが対となる矩形の範囲内にあれば当たっている
    if( hit_rectA->top    < hit_rectB->bottom && 
        hit_rectA->bottom > hit_rectB->top    &&
        hit_rectA->left   < hit_rectB->right  &&
        hit_rectA->right  > hit_rectB->left   )
    {//ヒット！
        return( true );
    }
    //ヒットしていない
    return( false );
}

unsigned char EX08_13_player_input( int PlayerID )
{    unsigned char input_key;

    //入力値をプレイヤーによって変更する
    switch( PlayerID )
    {//キーボードをプレイヤー１、ジョイスティックをプレイヤー２とする
       case PLAYER_1: input_key = g_KeyInputBuff; break;
       case PLAYER_2: input_key = g_JoystickBuff; break;
    }
    return input_key;
}

void exec08_13_human(TCB* thisTCB)
{
    EX08_13_STRUCT* work = (EX08_13_STRUCT*)thisTCB->Work;
    //相手側のプレイヤーのタスク
    EX08_13_STRUCT* enemy_work = (EX08_13_STRUCT*)work->EnemyPlayerTask->Work;
    //１フレーム前の座標
    float old_posX;
    float old_posY;
    unsigned char input_buff;
    RECT anim_rect = {0,0,64,128,};
    RECT human_hit_rect  = {0,0,64,128,};
    RECT my_hit_rect;
    RECT enemy_hit_rect;

    //１フレーム前の座標を保持
    old_posX = work->Human.X;
    old_posY = work->Human.Y;

    //プレイヤーキャラの表示
    work->Human.SrcRect = &anim_rect;

    //プレイヤーIDによる入力
    input_buff = EX08_13_player_input( work->PID );

    //キー入力による移動
    if( input_buff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( input_buff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;

    //自キャラの当たり判定を取得
    EX08_13_pos_add_rect( 
            &my_hit_rect, &human_hit_rect,
            work->Human.X, work->Human.Y);

    //相手キャラ自の当たり判定を取得
    EX08_13_pos_add_rect( 
            &enemy_hit_rect, &human_hit_rect,
            enemy_work->Human.X, enemy_work->Human.Y);

    //当たり判定をチェック
    if( EX08_13_hit_rect( &my_hit_rect, &enemy_hit_rect ) )
    {//もし接触していたら
     //移動できないので座標を戻す
        work->Human.X = old_posX;
        work->Human.Y = old_posY;
    }

    //自キャラの描画
    SpriteDraw( &work->Human,work->PID);
}

void init08_13(TCB* thisTCB)
{
    TCB*    human_tcb[2];
    EX08_13_STRUCT* human_work[2];
    int loop;
    float player_pos[][2] = 
{//プレイヤーの初期座標
{SCREEN_WIDTH / 4.0 * 1.0,SCREEN_HEIGHT / 2.0,}, //PLAYER1
{SCREEN_WIDTH / 4.0 * 3.0,SCREEN_HEIGHT / 2.0,}, //PLAYER2
};

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim_r.png",&g_pTex[1] );

    //プレイヤーを２体作成する
    for( loop = 0;loop < 2; loop++ )
    {
        human_tcb[loop] = TaskMake( exec08_13_human, 0x2000 );
        human_work[loop] = (EX08_13_STRUCT*)human_tcb[loop]->Work;
        //プレイヤーにより初期表示座標を変える
        human_work[loop]->Human.X = player_pos[loop][0];
        human_work[loop]->Human.Y = player_pos[loop][1];
        //プレイヤーIDを登録
        human_work[loop]->PID = loop;
    }

    //お互いのタスクワークを保持しておく
    human_work[PLAYER_1]->EnemyPlayerTask 
        = human_tcb[PLAYER_2];
    human_work[PLAYER_2]->EnemyPlayerTask 
        = human_tcb[PLAYER_1];
}

void exec08_13(TCB* thisTCB)
{
    //複数同時処理のため、サンプルではメイン処理は何もしない
}



////////////////////////////////////////////////////
//８章－１５ 格闘ゲームの攻撃
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

//プレイヤーの当たり判定領域
static RECT EX08_15_human_hit_rect  = {0,0,64,128,};

void exec08_15_human_attack(TCB* thisTCB)
{//攻撃時の処理
    EX08_15_STRUCT* work = (EX08_15_STRUCT*)thisTCB->Work;
    //対戦側のプレイヤーのタスク
    EX08_15_STRUCT* enemy_work = (EX08_15_STRUCT*)work->EnemyPlayerTask->Work;
    RECT my_attack_rect;
    RECT enemy_hit_rect;
    char str[128];

    //攻撃範囲の座標 
    RECT human_attack_rect[]  =
{//攻撃判定座標
 { 64, 32, 96, 164,}, //PLAYER_1
 {-32, 32,  0, 164,}, //PLAYER_2
};
    RECT attack_anim_rect[] = 
{
 {  0,  0, 64,128,},  //1コマ目
 {128,  0,192,128,},  //2コマ目
 {192,  0,256,128,},  //3コマ目(攻撃）
 {128,  0,192,128,},  //4コマ目
};

    //プレイヤーキャラのアニメーション表示
    work->Human.SrcRect = &attack_anim_rect[(int)work->AnimFrame];

    work->AnimFrame += ATTACK_SPEED;
    if(work->AnimFrame >= ATTACK_ANIM_MAX)
    {//攻撃アニメが終了したら処理切り替え
        work->AnimFrame = 0;
        TaskChange(thisTCB,exec08_15_human_move);
        //自キャラの描画
        SpriteDraw( &work->Human,work->PID);
    }

    if((int)work->AnimFrame == ATTACK_ANIM_PTN)
    {//攻撃のパターンなら攻撃判定を行なう
        //自キャラの攻撃判定を取得
        EX08_13_pos_add_rect( 
                &my_attack_rect, 
                &human_attack_rect[work->PID],
                work->Human.X, work->Human.Y);

        //相手キャラ自の当たり判定を取得
        EX08_13_pos_add_rect( 
                &enemy_hit_rect, &EX08_15_human_hit_rect,
                enemy_work->Human.X, enemy_work->Human.Y);

        //当たり判定をチェック
        if( EX08_13_hit_rect( &my_attack_rect, &enemy_hit_rect ) )
        {//もし接触していたら
            //攻撃命中とみなし、文字列表示
            sprintf( str,"ATTACK HIT!! P%d",work->PID+1 );
            FontPrint( 128,128+16*work->PID, str);
        }
    }

    //自キャラの描画
    SpriteDraw( &work->Human,work->PID);
}


void exec08_15_human_move(TCB* thisTCB)
{
    EX08_15_STRUCT* work = (EX08_15_STRUCT*)thisTCB->Work;
    //対戦側のプレイヤーのタスク
    EX08_15_STRUCT* enemy_work = (EX08_15_STRUCT*)work->EnemyPlayerTask->Work;
    //１フレーム前の座標
    float old_posX;
    float old_posY;
    unsigned char input_buff;
    RECT anim_rect = {0,0,64,128,};
    RECT my_hit_rect;
    RECT enemy_hit_rect;

    //１フレーム前の座標を保持
    old_posX = work->Human.X;
    old_posY = work->Human.Y;

    //プレイヤーキャラの表示
    work->Human.SrcRect = &anim_rect;

    //プレイヤーIDによる入力
    input_buff = EX08_13_player_input( work->PID );

    //キー入力による移動
    if( input_buff & KEY_RIGHT ) work->Human.X += MOVE_SPEED;
    if( input_buff & KEY_LEFT  ) work->Human.X -= MOVE_SPEED;

    //キー入力による攻撃
    if( input_buff & KEY_Z )
    {//パンチによる攻撃
        TaskChange(thisTCB,exec08_15_human_attack);
        //自キャラの描画
        SpriteDraw( &work->Human,work->PID);
        return;
    }


    //自キャラの当たり判定を取得
    EX08_13_pos_add_rect( 
            &my_hit_rect, &EX08_15_human_hit_rect,
            work->Human.X, work->Human.Y);

    //相手キャラ自の当たり判定を取得
    EX08_13_pos_add_rect( 
            &enemy_hit_rect, &EX08_15_human_hit_rect,
            enemy_work->Human.X, enemy_work->Human.Y);

    //当たり判定をチェック
    if( EX08_13_hit_rect( &my_hit_rect, &enemy_hit_rect ) )
    {//もし接触していたら
     //移動できないので座標を戻す
        work->Human.X = old_posX;
        work->Human.Y = old_posY;
    }

    //自キャラの描画
    SpriteDraw( &work->Human,work->PID);
}

void init08_15(TCB* thisTCB)
{
    TCB*    human_tcb[2];
    EX08_15_STRUCT* human_work[2];
    int loop;
    float player_pos[][2] = 
{//プレイヤーの初期座標
{SCREEN_WIDTH / 4.0 * 1.0,SCREEN_HEIGHT / 2.0,}, //PLAYER1
{SCREEN_WIDTH / 4.0 * 3.0,SCREEN_HEIGHT / 2.0,}, //PLAYER2
};

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\human_anim_r.png",&g_pTex[1] );

    //プレイヤーを２体作成する
    for( loop = 0;loop < 2; loop++ )
    {
        human_tcb[loop] = TaskMake( exec08_15_human_move, 0x2000 );
        human_work[loop] = (EX08_15_STRUCT*)human_tcb[loop]->Work;
        //プレイヤーにより初期表示座標を変える
        human_work[loop]->Human.X = player_pos[loop][0];
        human_work[loop]->Human.Y = player_pos[loop][1];
        //プレイヤーIDを登録
        human_work[loop]->PID = loop;
    }

    //お互いのタスクワークを保持しておく
    human_work[PLAYER_1]->EnemyPlayerTask 
        = human_tcb[PLAYER_2];
    human_work[PLAYER_2]->EnemyPlayerTask 
        = human_tcb[PLAYER_1];
}

void exec08_15(TCB* thisTCB)
{
    //複数同時処理のため、サンプルではメイン処理は何もしない
}



////////////////////////////////////////////////////
//９章 アイテム処理
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//９章－１ アイテム処理の基本
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //自機座標の初期化
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
{//アイテムの色
    0xff0000ff,
    0xff00ff00,
    0xffff0000,
    0xffffffff,
};


    if( g_InputBuff & KEY_RIGHT ) work->MyShip.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->MyShip.X -= MOVE_SPEED;

    //Ｚキーでアイテムを作成
    if( g_DownInputBuff & KEY_Z )
    {
        //条件によってアイテムは作成しない
        if( !work->ItemFlag ) 
        {
            //これ以上アイテムが出現しないようにフラグを建てる
            work->Item.Sprt.X = rand() % SCREEN_WIDTH;
            work->Item.Sprt.Y = 0;
            work->Item.AddY   = ITEM_SPEED;
            //アイテムの種類を決定する
            work->Item.ID     = rand() % ITEM_MAX;
            work->Item.Color  = item_color[ work->Item.ID ];
            work->ItemFlag    = true;
        }
    }

    //アイテム関連の処理

    if( work->ItemFlag )
    {//アイテム出現時のみ処理
        //アイテム取得時の当たり判定矩形を計算
        hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
        hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
        hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
        hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

        //アイテムの取得チェック
        if( hit_rect.top    < work->Item.Sprt.Y  && 
            hit_rect.bottom > work->Item.Sprt.Y  &&
            hit_rect.left   < work->Item.Sprt.X  &&
            hit_rect.right  > work->Item.Sprt.X  )
        {//アイテム取得時の処理
            switch(work->Item.ID)
            {//アイテムのIDに応じて処理を変える（ここではメッセージのみ）
                case ITEM_00: sprintf( work->DispMess,"スコア加算"); break;
                case ITEM_01: sprintf( work->DispMess,"パワーアップ"); break;
                case ITEM_02: sprintf( work->DispMess,"スピードアップ"); break;
                case ITEM_03: sprintf( work->DispMess,"１ＵＰ"); break;
            }
            //取得後アイテム処理は終了
            work->ItemFlag = false;
        }
        //アイテムの移動処理と表示
        work->Item.Sprt.X += work->Item.AddX;
        work->Item.Sprt.Y += work->Item.AddY;
        //画面外への移動でアイテムを消去
        if( work->Item.Sprt.Y > SCREEN_HEIGHT ) work->ItemFlag = false;
        //指定のアイテム色で描画
        EX09_01_CustomDraw( &work->Item.Sprt,1,work->Item.Color);
    }

    //自機の表示
    SpriteDraw( &work->MyShip,0);

    //取得したアイテム効果の表示
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//９章－２ キャラからアイテムを出す
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
{//アイテムの色
    0xff0000ff,
    0xff00ff00,
    0xffff0000,
    0xffffffff,
};

     //画面外に出たら消去
    if( work->Sprt.Y > SCREEN_HEIGHT )
    {//終了処理
        TaskKill(thisTCB);
        return;
    }
    //アイテムの色を決定
    work->Color  = item_color[ work->ID ];
    //アイテムの移動処理と表示
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
    {//一定時間毎に、アイテムを落とす
        //落とすアイテムの作成
        item_tcb = TaskMake( EX09_02_item_move , ITEM_PRIORITY );
        item_work = (EX09_01_ITEM*)item_tcb->Work;
        //出現位置は、現在の座標をコピーする
        item_work->Sprt.X = work->Sprt.X;
        item_work->Sprt.Y = work->Sprt.Y;;
        item_work->AddY   = ITEM_SPEED;
        //アイテムの種類をランダムに決定する
        item_work->ID     = rand() % ITEM_MAX;

        //アイテムを落とす時間を初期化
        work->Timer = 0;
    }

     //画面外に出たら座標を戻す
    if( work->Sprt.X > SCREEN_WIDTH ) work->Sprt.X = 0;

    //移動処理と表示
    work->Sprt.X += ENEMY_SPEED;
    SpriteDraw( &work->Sprt,2);
}

int EX09_02_get_item( RECT* HitRect )
{
    TCB*    check_tcb;
    EX09_01_ITEM* item_work;

    //タスクを全部チェックする
    //タスクの先頭アドレスを取得
    check_tcb = GetTaskTop();
    //タスクヘッドは処理せず飛ばす
    check_tcb = check_tcb->Next;
    //タスクヘッドにたどり着いたら１順
    while( check_tcb->Prio != 0x0000)
    {
        if( check_tcb->Prio == ITEM_PRIORITY )
        {//タスクがアイテムであれば、当たり判定をチェックする
            item_work = (EX09_01_ITEM*)check_tcb->Work;
            //対象と当たり判定の矩形を比較
            if( HitRect->top    < item_work->Sprt.Y &&
                HitRect->bottom > item_work->Sprt.Y &&
                HitRect->left   < item_work->Sprt.X &&
                HitRect->right  > item_work->Sprt.X )
            {//アイテムに接触した時の処理
                //アイテムを消去し、アイテムIDを返す
                TaskKill( check_tcb );
                return item_work->ID;
            }
        }
        //次にチェックするタスク
        check_tcb = check_tcb->Next;
    }
    return NO_ITEM;
}

void init09_02(TCB* thisTCB)
{
    EX09_02_STRUCT* work = (EX09_02_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //自機座標の初期化
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //アイテムを落とすキャラの作成
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

    //アイテム関連の処理

    //アイテム取得時の当たり判定矩形を計算
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //アイテムの取得チェック
    switch( EX09_02_get_item(&hit_rect) )
    {//取得アイテムのIDに応じて処理を変える
        case ITEM_00: sprintf( work->DispMess,"スコア加算"); break;
        case ITEM_01: sprintf( work->DispMess,"パワーアップ"); break;
        case ITEM_02: sprintf( work->DispMess,"スピードアップ"); break;
        case ITEM_03: sprintf( work->DispMess,"１ＵＰ"); break;
        //アイテムの取得は無し
        default: break;
    }

    //自機の表示
    SpriteDraw( &work->MyShip,0);

    //取得したアイテム効果の表示
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}







////////////////////////////////////////////////////
//９章－６ アイテム処理：一定時間効果を発揮
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

    //一定時間効果を発揮した後消去
    if( thisTCB->Work[0]++ >= EFFECT_TIME ) TaskKill(thisTCB);

    g_pFont->DrawText( NULL, "パワーアップ中!!", -1, &font_pos, DT_LEFT, 0xffffffff);
}

void EX09_06_item_effect02( TCB* thisTCB )
{
    RECT font_pos = {  0, 32,640,480,};

    //一定時間効果を発揮した後消去
    if( thisTCB->Work[0]++ >= EFFECT_TIME ) TaskKill(thisTCB);

    g_pFont->DrawText( NULL, "スピードアップ中!!", -1, &font_pos, DT_LEFT, 0xffffffff);
}

void init09_06( TCB* thisTCB )
{
    EX09_06_STRUCT* work = (EX09_06_STRUCT*)thisTCB->Work;
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //自機座標の初期化
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //アイテムを落とすキャラの作成
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

    //アイテム関連の処理

    //アイテム取得時の当たり判定矩形を計算
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //アイテムの取得チェック
    switch( EX09_02_get_item(&hit_rect) )
    {//時間限定の処理はタスクを作成して対応
        case ITEM_00: sprintf( work->DispMess,"スコア加算"); break;
        case ITEM_01:
               //アイテム効果用のタスクを作成
                sprintf( work->DispMess,"");
                TaskMake( EX09_06_item_effect01, EFFECT_PRIORITY);
            break;
        case ITEM_02:
               //アイテム効果用のタスクを作成
                sprintf( work->DispMess,"");
                TaskMake( EX09_06_item_effect02, EFFECT_PRIORITY);
            break;
        case ITEM_03: sprintf( work->DispMess,"１ＵＰ"); break;

        //アイテムの取得は無し
        default: break;
    }

    //自機の表示
    SpriteDraw( &work->MyShip,0);

    //取得したアイテム効果の表示
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//９章－７ アイテムを取得後に使用する
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
    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\block1.png",&g_pTex[2] );

    //自機座標の初期化
    work->MyShip.X = SCREEN_WIDTH  / 2;
    work->MyShip.Y = SCREEN_HEIGHT / 2;
    //アイテムを落とすキャラの作成
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

    //アイテム関連の処理

    //アイテム取得時の当たり判定矩形を計算
    hit_rect.top    =   work->MyShip.Y + hit_size_rect.top;
    hit_rect.bottom =   work->MyShip.Y + hit_size_rect.bottom;
    hit_rect.left   =   work->MyShip.X + hit_size_rect.left;
    hit_rect.right  =   work->MyShip.X + hit_size_rect.right;

    //アイテムの取得チェック
    check_item = EX09_02_get_item(&hit_rect);
    if( check_item != NO_ITEM )
    {//アイテムを取得していたら、所持アイテムを切り替える
        work->HaveItem = check_item;
        switch( check_item )
        {//取得したアイテムに応じて、所持アイテムの表示をする
            case ITEM_00: sprintf(work->DispMess,"所持ITEM：スコア加算"); break;
            case ITEM_01: sprintf(work->DispMess,"所持ITEM：パワーアップ"); break;
            case ITEM_02: sprintf(work->DispMess,"所持ITEM：スピードアップ"); break;
            case ITEM_03: sprintf(work->DispMess,"所持ITEM：１ＵＰ"); break;
        }
    }

    if( g_DownInputBuff & KEY_Z )
    {//Ｚキーで所持アイテムの使用
        switch( work->HaveItem )
        {//アイテムのIDに応じて処理を変える（ここではメッセージのみ）
            case ITEM_00: sprintf( work->DispMess,"スコア加算ITEM使用"); break;
            case ITEM_01: sprintf( work->DispMess,"パワーアップITEM使用"); break;
            case ITEM_02: sprintf( work->DispMess,"スピードアップITEM使用"); break;
            case ITEM_03: sprintf( work->DispMess,"１ＵＰITEM使用"); break;
            //アイテムを所持していない
            default: sprintf(work->DispMess,"ITEMを所持していません"); break;break;
        }
        //所持アイテムの破棄
        work->HaveItem = NO_ITEM;
    }

    //自機の表示
    SpriteDraw( &work->MyShip,0);

    //取得したアイテム効果の表示
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//９章－８ 多数アイテムの管理
////////////////////////////////////////////////////

#define ITEM_NORMAL    0
#define ITEM_EVENT     1

typedef struct _EX09_RPG_ITEM_DATA {
    //処理関数
    int (*ItemFunc)( _EX09_RPG_ITEM_DATA*, char* );
    //アイテム名
    char*          ItemName;
    //処理タイプ
    int            ItemType;
    //アイテム特有のデータ
    int            Param;
} EX09_RPG_ITEM_DATA;


//メッセージの高さ
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
  {NULL,"薬草"      , ITEM_NORMAL, 5,},  //ID00
  {NULL,"治療薬"    , ITEM_NORMAL,50,},  //ID01
  {NULL,"毒消し草"  , ITEM_NORMAL, 0,},  //ID02
  {NULL,"復活の薬"  , ITEM_NORMAL, 0,},  //ID03
};

void init09_08(TCB* thisTCB)
{
    EX09_08_STRUCT* work = (EX09_08_STRUCT*)thisTCB->Work;

    //所持用の配列にアイテムを格納
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


    //アイテム内容の表示
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
//９章－９ アイテム処理の効果を反映
////////////////////////////////////////////////////
#define ITEM_NORMAL    0
#define ITEM_EVENT     1

//メッセージの高さ
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
{//薬草・治療薬等、回復全般

   //回復処理を行なう（処理はサンプルでは例示しません）

    //メッセージを返す
    sprintf( ResMess,"%sを使用しました。ＨＰ %d回復！！",ItemData->ItemName, ItemData->Param );
}

int EX09_09_use_item02( EX09_RPG_ITEM_DATA* ItemData ,char* ResMess )
{//毒消し草
    //メッセージを返す
    sprintf( ResMess,"毒消し草を使用しました。毒が体から消えた！！");
}

int EX09_09_use_item03( EX09_RPG_ITEM_DATA* ItemData ,char* ResMess )
{//黄金の鍵  イベントアイテム
    //メッセージを返す
    sprintf( ResMess,"このアイテムを使用する事は出来ません！");
}

static EX09_RPG_ITEM_DATA g_EX09_09_ItemData[] = 
{
  {EX09_09_use_item00,"薬草"      , ITEM_NORMAL, 5,},  //ID00
  {EX09_09_use_item00,"治療薬"    , ITEM_NORMAL,50,},  //ID01
  {EX09_09_use_item02,"毒消し草"  , ITEM_NORMAL, 0,},  //ID02
  {EX09_09_use_item03,"黄金の鍵"  , ITEM_EVENT , 0,},  //ID03
};

void init09_09(TCB* thisTCB)
{
    EX09_09_STRUCT* work = (EX09_09_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //所持用の配列にアイテムIDを格納（アイテムの取得）
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

    //アイテム選択
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
    {//Ｚキーで選択中のアイテムの使用
        //選択中の位置にアイテムがあるかを確認
        select_item = work->HaveItem[ work->SelectMenu ];

        if( select_item != NO_ITEM )
        {//アイテムがあるときのみ実行
            //アイテム処理関数を実行
            //データとメッセージのポインタを渡す
            g_EX09_09_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        //通常のアイテムは使用後に消去する
        if( g_EX09_09_ItemData[ select_item ].ItemType == ITEM_NORMAL)
            work->HaveItem[ work->SelectMenu ] = NO_ITEM;
    }

    //選択マークの位置を決定
    //アイテム内容の表示

    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //選択マークの表示
    SpriteDraw(&work->SelectMark,0);

    //メニューの表示
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if( work->HaveItem[ loop ] != NO_ITEM )
        {//所持アイテムがある時のみ表示
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

    //アイテム使用時のメッセージの表示
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//９章－１０  所持アイテムの管理
////////////////////////////////////////////////////
#define ITEM_NORMAL    0
#define ITEM_EVENT     1

//メッセージの高さ
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
    {//アイテム欄の空きを探す
        if( ItemList[ loop ] == NO_ITEM )break;
    }
    //アイテムをこれ以上もてなかったらエラー
    if( loop == HAVE_MAX ) return -1;
    //アイテムを追加
    ItemList[ loop ] = ID;
    //成功したらアイテムを挿入した位置を返す
    return loop;
}

void EX09_10_item_del( int* ItemList,int ID )
{
    //アイテムを削除
    ItemList[ ID ] = NO_ITEM;
}

int EX09_10_item_search( int* ItemList,int ID )
{
    int loop;

    for(loop = 0; loop < HAVE_MAX; loop++)
    {//指定IDのアイテムを探す
        if( ItemList[ loop ] == ID )break;
    }

    //アイテムを発見できなかったら終了
    if( loop == HAVE_MAX ) return -1;

    //発見したら、アイテムのIDを返す
    return loop;
}

void EX09_10_item_correct( int* ItemList )
{
    int loop;
    int collect_flag = 0;

    //初めのアイテム欄が空きかチェック
    if( ItemList[0] == NO_ITEM )collect_flag = true;
    for(loop = 0; loop < HAVE_MAX-1; loop++)
    {
        if(collect_flag)
        {//補正フラグが立った後のアイテムは
         //全て１個詰める
             ItemList[ loop ] = ItemList[ loop+1];
        }
        //空のアイテム欄を探す
        if( ItemList[ loop+1] == NO_ITEM )collect_flag = true;
    }

    //補正フラグが立っていたら、最後の欄はアイテム無し
    if(collect_flag) ItemList[ loop ] = NO_ITEM;
}


void init09_10(TCB* thisTCB)
{
    EX09_10_STRUCT* work = (EX09_10_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //アイテムを所持配列に加える（初期アイテムの取得）
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


    //アイテム選択
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
    {//Ｘキーでランダムにアイテムを取得
        //アイテムをランダムに決める
        //（鍵は要素から外す）
        get_item = rand() % (HAVE_MAX - 1);

        //アイテムを追加
        err = EX09_10_item_add( work->HaveItem, get_item);
        if(err == -1)
        {//追加できなかったらメッセージを出す
            sprintf( work->DispMess,"アイテムを追加できませんでした。");
        }
    }

    if( g_DownInputBuff & KEY_Z  )
    {//Ｚキーで選択中のアイテムの使用
        //選択中の位置にアイテムがあるかを確認
        select_item = work->HaveItem[ work->SelectMenu ];
        if( select_item != NO_ITEM )
        {//アイテムがあるときのみ実行
            //アイテム処理関数を実行
            //データとメッセージのポインタを渡す
            g_EX09_09_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        if( g_EX09_09_ItemData[ select_item ].ItemType == ITEM_NORMAL)
        {//通常のアイテムは使用後に消去する
            EX09_10_item_del(work->HaveItem,work->SelectMenu );
           //消去後、開いた欄を詰める
            EX09_10_item_correct(work->HaveItem);
        }
    }

    //選択マークの位置を決定
    //アイテム内容の表示
    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //選択マークの表示
    SpriteDraw(&work->SelectMark,0);

    //メニューの表示
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if( work->HaveItem[ loop ] != NO_ITEM )
        {//所持アイテムがある時のみ表示
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

    //アイテム使用時のメッセージの表示
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}






////////////////////////////////////////////////////
//９章－１１  複数所持可能なアイテム
////////////////////////////////////////////////////

#define ITEM_NORMAL    0
#define ITEM_EVENT     1
#define ITEM_COUNT     2

//メッセージの高さ
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
  {EX09_09_use_item00,"薬草"      , ITEM_COUNT , 5,},  //ID00
  {EX09_09_use_item00,"治療薬"    , ITEM_NORMAL,50,},  //ID01
  {EX09_09_use_item02,"毒消し草"  , ITEM_NORMAL, 0,},  //ID02
  {EX09_09_use_item03,"黄金の鍵"  , ITEM_EVENT , 0,},  //ID03
};


void init09_11(TCB* thisTCB)
{
    EX09_11_STRUCT* work = (EX09_11_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\select_mark.png",&g_pTex[0] );

    //アイテムを所持配列に加える（初期アイテムの取得）
    work->HaveItem[0] = 0;
    work->HaveItem[1] = 1;
    work->HaveItem[2] = 2;
    work->HaveItem[3] = 3;
    //薬草の数を初期化
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

    //アイテム選択
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
    {//Ｚキーで選択中のアイテムの使用
        //選択中の位置にアイテムがあるかを確認
        select_item = work->HaveItem[ work->SelectMenu ];
        if( select_item != NO_ITEM )
        {//アイテムがあるときのみ実行
            //アイテム処理関数を実行
            //データとメッセージのポインタを渡す
            g_EX09_11_ItemData[ select_item ].ItemFunc(
                &g_EX09_09_ItemData[ select_item ],
                work->DispMess);
        }

        switch(g_EX09_11_ItemData[ select_item ].ItemType)
        {//アイテムのタイプによって処理を分ける
            case ITEM_NORMAL:
                   //通常のアイテムは使用後に消去する
                   EX09_10_item_del(work->HaveItem,work->SelectMenu );
                   //消去後、開いた欄を詰める
                   EX09_10_item_correct(work->HaveItem);
                   break;
            case ITEM_COUNT:
                   //カウントアイテムは数を減らしてチェック後消去する
                   if( --work->ItemCount[ work->SelectMenu ] <= 0)
                   {
                       EX09_10_item_del(work->HaveItem,work->SelectMenu );
                       //消去後、開いた欄を詰める
                       EX09_10_item_correct(work->HaveItem);
                   }
                   break;
            default: break;
        }
    }

    //選択マークの位置を決定
    //アイテム内容の表示
    work->SelectMark.X = ITEM_X - FONT_SIZE;
    work->SelectMark.Y = ITEM_Y + work->SelectMenu * FONT_SIZE;
    //選択マークの表示
    SpriteDraw(&work->SelectMark,0);

    //メニューの表示
    font_pos.left = ITEM_X;
    font_pos.top  = ITEM_Y;
    for(loop = 0; loop < HAVE_MAX; loop++)
    {
        if(work->HaveItem[ loop ] != NO_ITEM)
        {//所持しているアイテムを表示
            switch(g_EX09_11_ItemData[work->HaveItem[ loop ]].ItemType)
            {//アイテムのタイプによって処理を分ける
                case ITEM_NORMAL:
                case ITEM_EVENT:
                       //カウントアイテム以外は普通にアイテム名表示
                       strcpy(str,g_EX09_11_ItemData[work->HaveItem[ loop ]].ItemName);
                       break;
                case ITEM_COUNT:
                       //カウントアイテムは末尾に使用回数を表示
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

    //アイテム使用時のメッセージの表示
    font_pos.top  = 448;
    font_pos.left = 32;
    g_pFont->DrawText( NULL, work->DispMess, -1, &font_pos, DT_LEFT, 0xffffffff);
}









////////////////////////////////////////////////////
//１０章 ゲームバランス
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//１０章－２ ゲームバランス調整 ランク法
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
    float          AddX;  //増分値Ｘ
    float          AddY;  //増分値Ｙ
} EX10_02_BULLET;

void exec10_02_bullet(TCB* thisTCB)
{
    EX10_02_BULLET* work = (EX10_02_BULLET*)thisTCB->Work;

    if( work->Sprt.X < 0 || work->Sprt.X >= SCREEN_WIDTH ||
        work->Sprt.Y < 0 || work->Sprt.Y >= SCREEN_HEIGHT )
    {//画面外への移動で弾を消去
        TaskKill( thisTCB );
    }

    //座標に増分値を加算
    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    SpriteDraw(&work->Sprt,1);
}

void init10_02(TCB* thisTCB)
{
    EX10_02_STRUCT* work = (EX10_02_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //座標設定
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
    //目標の方向
    float direction;
    //ランクによる速度
    float rank_speed;
float speed_data[] = 
{//ランク別の速度データ
    8.0,   //RANK 0(EASY)
    12.0,  //RANK 1(NORMAL)
    16.0,  //RANK 2(HARD)
};

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
    //ランクの切り替え
    if( g_DownInputBuff & KEY_Z ) work->Rank--;
    if( g_DownInputBuff & KEY_X ) work->Rank++;
    if( work->Rank <  0 ) work->Rank = 0;
    if( work->Rank >= RANK_MAX ) work->Rank = RANK_MAX-1;

    if(work->Time++ >= BULLET_TIME)
    {//一定時間ごとに弾を作成
        tmp_tcb = TaskMake( exec10_02_bullet, 0x2000 );
        tmp_work = (EX10_02_BULLET*)tmp_tcb->Work;

        //常に中心座標から発射
        tmp_work->Sprt.X = SCREEN_WIDTH  / 2;
        tmp_work->Sprt.Y = SCREEN_HEIGHT / 2;

        //中心座標から自機への方向を計算
        direction = 
            atan2( work->Sprt.Y - tmp_work->Sprt.Y,
                   work->Sprt.X - tmp_work->Sprt.X );

        //速度はランクによって決定
        rank_speed = speed_data[ work->Rank ];

        //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
        tmp_work->AddX = cos( direction ) * rank_speed;
        tmp_work->AddY = sin( direction ) * rank_speed;

        work->Time = 0;
    }

    SpriteDraw( &work->Sprt, 0);

    sprintf(str,"RANK = %d",work->Rank);
    FontPrint( 16,16, str);
}





////////////////////////////////////////////////////
//１０章－３ ゲームバランス調整 レベル法
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
{//レベル値から弾の速度を計算
    return  (BULLET_SPEED_MAX - BULLET_SPEED_MIN) * Level / LEVEL_MAX + BULLET_SPEED_MIN;
}

void init10_03(TCB* thisTCB)
{
    EX10_03_STRUCT* work = (EX10_03_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0044.png",&g_pTex[0] );
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\0055.png",&g_pTex[1] );

    //座標設定
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
    //目標の方向
    float direction;
    //レベルによる速度
    float level_speed;


    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;
    //レベル値の増減
    if( g_InputBuff & KEY_Z ) work->Level--;
    if( g_InputBuff & KEY_X ) work->Level++;
    if( work->Level <  0 ) work->Level = 0;
    if( work->Level >= LEVEL_MAX ) work->Level = LEVEL_MAX-1;

    if(work->Time++ >= BULLET_TIME)
    {//一定時間ごとに弾を作成
        tmp_tcb = TaskMake( exec10_02_bullet, 0x2000 );
        tmp_work = (EX10_02_BULLET*)tmp_tcb->Work;

        //常に中心座標から発射
        tmp_work->Sprt.X = SCREEN_WIDTH  / 2;
        tmp_work->Sprt.Y = SCREEN_HEIGHT / 2;

        //中心座標から自機への方向を計算
        direction = 
            atan2( work->Sprt.Y - tmp_work->Sprt.Y,
                   work->Sprt.X - tmp_work->Sprt.X );

        //レベル値から速度を算出
        level_speed = EX10_03_BulletSpeed(work->Level);

        //方向から、Ｘ、Ｙそれぞれの座標増分値を計算
        tmp_work->AddX = cos( direction ) * level_speed;
        tmp_work->AddY = sin( direction ) * level_speed;

        work->Time = 0;
    }

    SpriteDraw( &work->Sprt, 0);

    sprintf(str,"LEVEL = %3d",work->Level);
    FontPrint( 16,16, str);
}





////////////////////////////////////////////////////
//１０章－６ レベルアップしたときの数値の管理
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
    char           Str;       //力
    char           Agi;       //素早さ
    char           HP;        //ＨＰ
    char           MP;        //ＭＰ
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

    //キー入力によるレベルアップ
    if( g_DownInputBuff & KEY_Z )
    {
         //レベルが最大になっているかチェック
         if( work->Level < LEVEL_MAX - 1 )
         {//レベルアップ処理、各種パラメータ数値をアップ
             work->Str += LevelTable[ work->Level ].Str;
             work->Agi += LevelTable[ work->Level ].Agi;
             work->HP  += LevelTable[ work->Level ].HP;
             work->MP  += LevelTable[ work->Level ].MP;
             work->Level++;
         }
    }

    //レベルの初期化
    if( g_DownInputBuff & KEY_X )
    {
         work->Str = 0;
         work->Agi = 0;
         work->HP  = 0;
         work->MP  = 0;
         work->Level = 0;
    }
    //各種パラメータ表示
    sprintf( str,"レベル  %d", work->Level + 1 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;

    sprintf( str,"強さ    %2d  ＨＰ %3d", work->Str,work->HP );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;

    sprintf( str,"素早さ  %2d  ＭＰ %3d", work->Agi,work->MP );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);


}



////////////////////////////////////////////////////
//１１章 サウンド
////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//ReadWaveFile: WAVファイルの情報取得と読み込みを行う
//              リニアPCMのみ対応、tagも単一のdataのみ対応
//
//char *pFileName:           ファイル名
//WAVEFORMATEX* pWaveFormat: 情報を格納する構造体へのポインタ
//char* pWaveData:           読み込まれるWAVデータへのポインタ
//////////////////////////////////////////////////////////////////////
int ReadWaveFile( char *pFileName, WAVEFORMATEX* pWaveFormat, char* pWaveData )
{
    FILE *fp;
    int wave_size;
    char dummy[16];

    fp = fopen( pFileName, "rb" );
    if ( !fp ) return( false );

    //チャンク文字"RIFF"と、RIFFデータサイズを読み飛ばす
    fread( dummy, 1, 4 + 4, fp );

    //チャンク文字"WAVE"と、"fmt "を読み飛ばす
    fread( dummy, 1, 4 + 4, fp );

    //fmtデータサイズエリアを読み飛ばす
    fread( dummy, sizeof( int ), 1, fp );

    //フォーマット情報取得 リニアPCMデータのみで拡張情報は存在しない
    fread( pWaveFormat, sizeof( WAVEFORMATEX ) - 2, 1, fp );

    //チャンク文字"data"を読み飛ばす
    ZeroMemory(dummy, sizeof( dummy ));
    while( strcmp( "data", dummy ) )
    {
        fread( dummy, 1, 1, fp );
        if( dummy[0] == 'd' ) fread( &dummy[1], 1, 3, fp );
    }

    //Waveデータサイズ取得
    fread( &wave_size, sizeof( int ), 1, fp );

    //waveデータ取得
    fread( pWaveData, 1, wave_size, fp );

    fclose( fp );

    return( wave_size );
}


//////////////////////////////////////////////////////////////////////
//ReadWaveFileOffset: WAVファイルの情報取得と読み込みを行う offset指定
//                    リニアPCMのみ対応、tagも単一のdataのみ対応
//
//char *pFileName:           ファイル名
//WAVEFORMATEX* pWaveFormat: 情報を格納する構造体へのポインタ
//char* pWaveData:           読み込まれるWAVデータへのポインタ
//int   ReadOffset:          読み込まれるWAVデータへのオフセット値
//int   ReadSize:            読み込まれるWAVデータのサイズ
//////////////////////////////////////////////////////////////////////
int ReadWaveFileOffset( char *pFileName, WAVEFORMATEX* pWaveFormat, char* pWaveData, int ReadOffset ,int ReadSize)
{
    FILE *fp;
    int wave_size;
    int data_top;             //データ先頭位置
    int read_size2;
    char dummy[16];

    fp = fopen( pFileName, "rb" );
    if ( !fp ) return( false );

    //チャンク文字"RIFF"と、RIFFデータサイズを読み飛ばす
    fread( dummy, 1, 4 + 4, fp );

    //チャンク文字"WAVE"と、"fmt "を読み飛ばす
    fread( dummy, 1, 4 + 4, fp );

    //fmtデータサイズエリアを読み飛ばす
    fread( dummy, sizeof( int ), 1, fp );

    //フォーマット情報 取得リニアPCMデータのみで、拡張情報は存在しない
    fread( pWaveFormat, sizeof( WAVEFORMATEX ) - 2, 1, fp );


    //チャンク文字"data"を読み飛ばす
    ZeroMemory(dummy, sizeof( dummy ));
    while( strcmp( "data", dummy ) )
    {
        fread( dummy, 1, 1, fp );
        if( dummy[0] == 'd' ) fread( &dummy[1], 1, 3, fp );
    }

    //Waveデータサイズ取得
    fread( &wave_size, sizeof( int ), 1, fp );

    //データ先頭位置を記録
    data_top = ftell( fp );

    //読み込み開始位置へシーク
    //データ読み取り位置がデータ最終位置を越えないように調整
    ReadOffset %= wave_size;
    fseek( fp, ReadOffset, SEEK_CUR );

    //waveデータ取得
      //読み取るデータがデータ最終位置を越えているか？

    if( ReadOffset + ReadSize > wave_size )
    {   //そうなら調整し、分割して読み込み
        read_size2 = wave_size - ReadOffset;
        fread( pWaveData, 1, read_size2, fp );

        //読み取り位置を、データの先頭へ戻し、残りデータを読み込む
        fseek( fp, data_top, SEEK_SET );
        fread( &pWaveData[read_size2], 1, ReadSize - read_size2, fp );
    }else
    {   //そうでなければ一括読み込み
        fread( pWaveData, 1, ReadSize, fp );
    }

    fclose( fp );

    return( wave_size );
}


////////////////////////////////////////////////////
//１１章－２ 単純再生
////////////////////////////////////////////////////

void init11_02(TCB* thisTCB)
{
    int wave_size;
    //アクセス可能なバッファのサイズ
    DWORD  buff_size1, buff_size2;
    //WAVバッファアクセスポイントを格納する為のポインタ
    LPVOID pvAudioPtr1, pvAudioPtr2; 

    //Waveファイルの情報とデータ取得
    wave_size = ReadWaveFile( "..\\..\\data\\bgm00.wav", &g_WaveControllBGM->WaveFormat, g_WaveControllBGM->WaveData);

    //取得した情報を設定
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
    g_WaveControllBGM->DSBDesc.dwBufferBytes = wave_size;
    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //サウンドバッファ作成
    g_pDSound->CreateSoundBuffer(&g_WaveControllBGM->DSBDesc,
                                 &g_WaveControllBGM->pDSBuffer, NULL );

    //バッファロック
    g_WaveControllBGM->pDSBuffer->Lock( 0, wave_size, //WAVEデータサイズ分だけロックする
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //全ブロックをロック
                                        DSBLOCK_ENTIREBUFFER
                                       );

    //サウンドデータをバッファへ書き込む
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, wave_size );

    //ロック解除
    g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1, pvAudioPtr2, buff_size2 );

}

void exec11_02(TCB* thisTCB)
{
    //再生
    if( g_DownInputBuff & KEY_Z ) 
        g_WaveControllBGM->pDSBuffer->Play( 0, 0, 0 );
    //停止
    if( g_DownInputBuff & KEY_X ) 
        g_WaveControllBGM->pDSBuffer->Stop();

}



////////////////////////////////////////////////////
//１１章－３ フェードイン/アウト
////////////////////////////////////////////////////
void init11_03(TCB* thisTCB)
{
    int wave_size;
    //アクセス可能なバッファのサイズ
    DWORD  buff_size1, buff_size2;
    //WAVバッファアクセスポイントを格納する為のポインタ
    LPVOID pvAudioPtr1, pvAudioPtr2;

    //Waveファイルの情報とデータ取得
    wave_size = ReadWaveFile( "..\\..\\data\\bgm00.wav", &g_WaveControllBGM->WaveFormat, g_WaveControllBGM->WaveData);

    //取得した情報を設定
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE|DSBCAPS_CTRLVOLUME;  //ボリュームが変えられるように指定
    g_WaveControllBGM->DSBDesc.dwBufferBytes = wave_size;
    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //サウンドバッファ作成
    g_pDSound->CreateSoundBuffer( &g_WaveControllBGM->DSBDesc,
                                  &g_WaveControllBGM->pDSBuffer, NULL );

    //バッファロック
    g_WaveControllBGM->pDSBuffer->Lock( 0, wave_size, //WAVEデータサイズ分だけロックする
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //全ブロックをロック
                                        DSBLOCK_ENTIREBUFFER
                                       );

    //サウンドデータをバッファへ書き込む
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, wave_size );

    //ロック解除
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
        //再生
        if( g_DownInputBuff & KEY_Z ) 
            g_WaveControllBGM->pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
        //停止
        if( g_DownInputBuff & KEY_X ) 
            g_WaveControllBGM->pDSBuffer->Stop();

        //フェード開始のチェック
        if( g_DownInputBuff & KEY_UP   )
        {   //フェードイン開始
            *fade_count = DSBVOLUME_MIN;
            *fade_mode = FADE_IN;
        }
        if( g_DownInputBuff & KEY_DOWN )
        {   //フェードアウト開始
            *fade_count = DSBVOLUME_MAX;
            *fade_mode = FADE_OUT;
        }

    }else if( *fade_mode == FADE_IN)
    {//フェードイン処理
        *fade_count -= FADE_STEP;
        if( *fade_count >= DSBVOLUME_MAX)
        {
            *fade_count = DSBVOLUME_MAX;
            *fade_mode = 0;
        }

        g_WaveControllBGM->pDSBuffer->SetVolume( *fade_count );
    }else if( *fade_mode == FADE_OUT)
    {//フェードアウト処理
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
//１１章－５ ＳＥ再生
////////////////////////////////////////////////////

void init11_05(TCB* thisTCB)
{
    int loop;
    int wave_size;
    //アクセス可能なバッファのサイズ
    DWORD  buff_size1, buff_size2;
    //WAVバッファアクセスポイントを格納する為のポインタ
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
        //Waveファイルの情報とデータ取得
        wave_size = 
        ReadWaveFile( se_file[ loop ], &g_WaveControllSE[ loop ]->WaveFormat, g_WaveControllSE[ loop ]->WaveData);

        //取得した情報を設定
        g_WaveControllSE[ loop ]->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
        g_WaveControllSE[ loop ]->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
        g_WaveControllSE[ loop ]->DSBDesc.dwBufferBytes = wave_size;
        g_WaveControllSE[ loop ]->DSBDesc.lpwfxFormat   = &g_WaveControllSE[ loop ]->WaveFormat;

        //サウンドバッファ作成
        g_pDSound->CreateSoundBuffer( &g_WaveControllSE[ loop ]->DSBDesc, 
                                      &g_WaveControllSE[ loop ]->pDSBuffer,
                                      NULL );

        //バッファロック
        //WAVEデータサイズ分だけロックする
        g_WaveControllSE[ loop ]->pDSBuffer->Lock( 0, wave_size,
                                                   &pvAudioPtr1, &buff_size1,
                                                   &pvAudioPtr2, &buff_size2,
                                                   //全ブロックをロック
                                                   DSBLOCK_ENTIREBUFFER
                                                  );
        //サウンドデータをバッファへ書き込む
        memcpy( pvAudioPtr1, g_WaveControllSE[ loop ]->WaveData, wave_size );

        //ロック解除
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
        //入力キーに合わせてＳＥを再生
        if( g_DownInputBuff & key_data[ loop ] )
        {
             //SEなので常に最初から再生
             g_WaveControllSE[ loop ]->pDSBuffer->SetCurrentPosition( 0 );
             g_WaveControllSE[ loop ]->pDSBuffer->Play( 0, 0, 0 );
        }
    }
}


////////////////////////////////////////////////////
//１１章－６ ＢＧＭのストリーミング再生
////////////////////////////////////////////////////
void init11_06(TCB* thisTCB)
{
    int loop;
    int wave_size;
    //アクセス可能なバッファのサイズ
    DWORD  buff_size1, buff_size2;
    //WAVバッファアクセスポイントを格納する為のポインタ
    LPVOID pvAudioPtr1, pvAudioPtr2; 

    //Waveファイルの情報とデータ取得
    wave_size = ReadWaveFileOffset(
                "..\\..\\data\\bgm00.wav",
                &g_WaveControllBGM->WaveFormat,
                g_WaveControllBGM->WaveData,
                0, BGM_READ_SIZE );

    //取得した情報を設定
    g_WaveControllBGM->DSBDesc.dwSize        = sizeof( DSBUFFERDESC ); 
    g_WaveControllBGM->DSBDesc.dwFlags       = DSBCAPS_LOCSOFTWARE;
    //バッファは余裕を見て読み込み量の3倍を設定
    g_WaveControllBGM->DSBDesc.dwBufferBytes = BGM_READ_SIZE * 3;

    g_WaveControllBGM->DSBDesc.lpwfxFormat   = &g_WaveControllBGM->WaveFormat;

    //サウンドバッファ作成
    g_pDSound->CreateSoundBuffer( &g_WaveControllBGM->DSBDesc,
                                  &g_WaveControllBGM->pDSBuffer,
                                  NULL );

    //最初のBGM再生
      //書き込むデータサイズ分だけロックする
    g_WaveControllBGM->pDSBuffer->Lock( 0, BGM_READ_SIZE,
                                        &pvAudioPtr1, &buff_size1,
                                        &pvAudioPtr2, &buff_size2,
                                        //書き込み位置からバッファをロック
                                        DSBLOCK_FROMWRITECURSOR
                                       );

    //サウンドデータをバッファへ書き込む
    memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, BGM_READ_SIZE );

    //ロック解除
    g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1,
                                          pvAudioPtr2, buff_size2 );

    //現在のカーソル位置を記録
    g_WaveControllBGM->pDSBuffer->GetCurrentPosition( NULL, &g_WaveControllBGM->BeforePlayCursor );
    g_WaveControllBGM->WaveFilePtr     = BGM_READ_SIZE;

}

void exec11_06(TCB* thisTCB)
{
    //アクセス可能なバッファのサイズ
    DWORD  buff_size1, buff_size2;
    //WAVバッファアクセスポイントを格納する為のポインタ
    LPVOID pvAudioPtr1, pvAudioPtr2; 
    DWORD CurrentWriteCursor;
    RECT font_pos = {224, 192,640,480,};
    int play_size;


    //現在のカーソルを取得
    g_WaveControllBGM->pDSBuffer->GetCurrentPosition( NULL, &CurrentWriteCursor );

    //再生済みの容量を取得
    play_size = CurrentWriteCursor - g_WaveControllBGM->BeforePlayCursor;

    if( play_size < 0 )
    {
        play_size = 
          (CurrentWriteCursor + BGM_READ_SIZE * 3) - g_WaveControllBGM->BeforePlayCursor;
    }
    //再生されたバッファが指定サイズ以上なら、バッファを更新
    if( play_size > BGM_READ_SIZE )
    {
        ReadWaveFileOffset(
        "..\\..\\data\\bgm00.wav", 
        &g_WaveControllBGM->WaveFormat,
        g_WaveControllBGM->WaveData,
        g_WaveControllBGM->WaveFilePtr,
        play_size );

        g_WaveControllBGM->WaveFilePtr += play_size;


        //書き込むデータサイズ分だけロックする
        g_WaveControllBGM->pDSBuffer->Lock( 0, play_size,
                                            &pvAudioPtr1, &buff_size1,
                                            &pvAudioPtr2, &buff_size2,
                                           //書き込み位置からバッファをロック
                                            DSBLOCK_FROMWRITECURSOR
                                           );

        //WAVEデータの書き込み
        memcpy( pvAudioPtr1, g_WaveControllBGM->WaveData, buff_size1 );

        //書き込み位置が分割されているか？
        if( pvAudioPtr2 != NULL )
        {   //そうなら残りデータを書き込む
            memcpy( pvAudioPtr2, 
                    &g_WaveControllBGM->WaveData[ buff_size1 ],
                    buff_size2 );
        }

        //ロック解除
        g_WaveControllBGM->pDSBuffer->Unlock( pvAudioPtr1, buff_size1,
                                              pvAudioPtr2, buff_size2 );

        //現在のカーソル位置を記録
        g_WaveControllBGM->BeforePlayCursor = CurrentWriteCursor;
    }

    if( g_DownInputBuff & KEY_Z )
        g_WaveControllBGM->pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    //停止
    if( g_DownInputBuff & KEY_X )
        g_WaveControllBGM->pDSBuffer->Stop();


    g_pFont->DrawText( NULL, "'Ｚ'キーで曲再生\n'Ｘ'キーで再生停止", -1, &font_pos, DT_LEFT, 0xffffffff);

}





////////////////////////////////////////////////////
//１１章－７ ＣＤＤＡ再生
////////////////////////////////////////////////////
void init11_07(TCB* thisTCB)
{
    //CDオープン(開始)
    mciSendString( "open cdaudio", NULL, 0, NULL );

    //トラック単位で制御
    mciSendString("set cdaudio time format tmsf",NULL,0,NULL);

}


void exec11_07(TCB* thisTCB)
{
    RECT font_pos = {160, 192,640,480,};
    char current_track[16];

    g_pFont->DrawText( NULL, "'Ｚ'キー  曲再生\n'Ｘ'キー  再生停止", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 48;
    g_pFont->DrawText( NULL, "カーソルキー", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    g_pFont->DrawText( NULL, "左  ＣＤトレイオープン", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    g_pFont->DrawText( NULL, "右  ＣＤトレイクローズ", -1, &font_pos, DT_LEFT, 0xffffffff);

    //現在のトラックを取得
    mciSendString("status cdaudio current track", current_track , sizeof(current_track) , NULL);

    font_pos.top += 16;
    g_pFont->DrawText( NULL, "現在のトラック", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.left += 160;
    g_pFont->DrawText( NULL, current_track, -1, &font_pos, DT_LEFT, 0xffffffff);


    //先頭から再生
    if( g_DownInputBuff & KEY_UP    )mciSendString( "play cdaudio notify", NULL, 0, NULL );

    //CDトレイオープン
    if( g_DownInputBuff & KEY_LEFT )mciSendString( "set cdaudio door open", NULL, 0, NULL );

    //CDトレイクローズ
    if( g_DownInputBuff & KEY_RIGHT)mciSendString( "set cdaudio door closed", NULL, 0, NULL );

    //再生
    if( g_DownInputBuff & KEY_Z    )mciSendString( "play cdaudio", NULL, 0, NULL );

    //停止
    if( g_DownInputBuff & KEY_X    )mciSendString( "stop cdaudio", NULL, 0, NULL );

}





////////////////////////////////////////////////////
//１２章 その他
////////////////////////////////////////////////////

typedef struct CARD_STRUCT{
    CARD_STRUCT*   Prev;
    CARD_STRUCT*   Next;
    int            CardType;
    int            CardNo;
    RECT           CardChipData;
} EX12_CARD_STRUCT;

#define CARD_WIDTH  32         //カードの横幅
#define CARD_HEIGHT 64         //カードの高さ
#define CARD_COUNT  13         //１種類あたりのカード枚数
#define CARD_TYPE   4          //カードの種類
#define CARD_TOTAL  52         //カードの合計枚数
#define CARD_SPADE  0          //スペード
#define CARD_CLUB   1          //クラブ
#define CARD_HEART  2          //ハート
#define CARD_DIA    3          //ダイヤ

static EX12_CARD_STRUCT CardData[CARD_TOTAL];

////////////////////////////////////////////////////
//カードの追加
////////////////////////////////////////////////////
void CardAdd( EX12_CARD_STRUCT* pCardList, EX12_CARD_STRUCT* pCard, int Index)
{
    int loop;
    EX12_CARD_STRUCT* pCardListOld = pCardList;

    for( loop = 0; loop < Index; loop++ )
    {//カード挿入位置の検索
        pCardListOld = pCardList;          //１つ前のカードを記録しておく
        pCardList = pCardList->Next;
    }

    if( pCardList != NULL )
    {//リストへの挿入
        pCard->Prev = pCardList;
        pCard->Next = pCardList->Next;
        pCardList->Next->Prev = pCard;
        pCardList->Next = pCard;
    } else {
     //リスト終端位置への挿入
        pCardList = pCardListOld;          //１つ前のカードの後尾に挿入する
        pCard->Prev = pCardList;
        pCard->Next = NULL;
        pCardList->Next = pCard;
    }

}
////////////////////////////////////////////////////
//カードの削除
////////////////////////////////////////////////////
EX12_CARD_STRUCT* CardDel( EX12_CARD_STRUCT* pCardList, int Index)
{
    int loop;
    EX12_CARD_STRUCT* pCardListOld = pCardList;
    EX12_CARD_STRUCT* pCardNext;

    for( loop = 0; loop < Index; loop++)
    {//削除カードの検索
        pCardList = pCardList->Next;
    }

    if( Index == 0 )
    {//リスト先頭からの削除
        pCardNext = pCardList->Next;
        pCardNext->Prev = NULL;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;

        //削除した次のカードがリストの先頭
        return pCardNext;
    }

    if( pCardList->Next != NULL )
    {//リストからの削除
        pCardList->Prev->Next = pCardList->Next;
        pCardList->Next->Prev = pCardList->Prev;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;
    } else {
     //リスト終端位置からの削除
        pCardList->Prev->Next = NULL;
        pCardList->Prev = NULL;
        pCardList->Next = NULL;
    }

    //リストの先頭アドレス
    return pCardListOld ;
}
////////////////////////////////////////////////////
//カードの取得
////////////////////////////////////////////////////
EX12_CARD_STRUCT* CardGet( EX12_CARD_STRUCT* pCardList, int Index)
{
    int loop;

    for( loop = 0; loop < Index; loop++)
    {//指定番号のカードの検索
        pCardList = pCardList->Next;
    }

    return pCardList;
}
////////////////////////////////////////////////////
//カードの検索
////////////////////////////////////////////////////
int CardSearch( EX12_CARD_STRUCT* pCardList, int CardType, int CardNo)
{
    int count = 0;

    while(pCardList != NULL)
    {
        if( (pCardList->CardType == CardType) && (pCardList->CardNo == CardNo) )
        {//カードを発見したらIndexを返す
            return count;
        }
        count++;
        pCardList = pCardList->Next;
    };
    //カードがなければ－１を返す
    return -1;
}
////////////////////////////////////////////////////
//カードの初期化
////////////////////////////////////////////////////
void CardInit( void )
{
    int loopX;
    int loopY;
    //カードデータの初期化
    for (loopY = 0; loopY < CARD_TYPE; loopY++)
    {
        for (loopX = 0; loopX < CARD_COUNT; loopX++)
        {
              //カードデータの初期化
              CardData[ loopY * CARD_COUNT + loopX ].Prev     = NULL;
              CardData[ loopY * CARD_COUNT + loopX ].Next     = NULL;
              CardData[ loopY * CARD_COUNT + loopX ].CardType = loopY;
              CardData[ loopY * CARD_COUNT + loopX ].CardNo   = loopX;
              //カード表示データの初期化
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.top     = CARD_HEIGHT * loopY;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.bottom  = CARD_HEIGHT * loopY + CARD_HEIGHT;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.left    = CARD_WIDTH  * loopX;
              CardData[ loopY * CARD_COUNT + loopX ].CardChipData.right   = CARD_WIDTH  * loopX + CARD_WIDTH;
        }
    }

}

////////////////////////////////////////////////////
//１２章－１ カードの管理
////////////////////////////////////////////////////

typedef struct{
    EX12_CARD_STRUCT*    MainCard;
} EX12_01_STRUCT;

void init12_01(TCB* thisTCB)
{
    EX12_01_STRUCT* work = (EX12_01_STRUCT*)thisTCB->Work;
    int loop;

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //カードの初期化
    CardInit();

    //最初のカードを登録
    work->MainCard = &CardData[0];
    //残りのカードをカードリストに登録
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
        //カードの表示座標を計算
        card.X = (index % CARD_COUNT) * CARD_WIDTH;
        card.Y = (index / CARD_COUNT) * CARD_HEIGHT;
 
        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        index++;
        pchk_card = pchk_card->Next;
    };

}




////////////////////////////////////////////////////
//１２章－３ カードをシャッフルする
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


    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //カードの初期化
    CardInit();

    //シャッフル用にカードをリストに登録
    tmpCardList = &CardData[0];
    for( loop = 1; loop < CARD_TOTAL; loop++)
    {
         CardAdd( tmpCardList , &CardData[ loop ],loop );
    }

    //最初の１枚  リストから乱数でカードを１枚抜き出す
    card_id = rand() % CARD_TOTAL;
    padd_card = CardGet( tmpCardList,card_id );
    tmpCardList = CardDel( tmpCardList    , card_id );
    //抜き出したカードを正式なリストに登録
    work->MainCard = padd_card;

    //残りのカードも同様に、乱数で抜き出して正式なリストに登録
    for( loop = 1; loop < CARD_TOTAL - 1; loop++ )
    {
         card_id = rand() % (CARD_TOTAL - loop - 1);         //指定範囲の乱数を得る
         padd_card = CardGet( tmpCardList,card_id );
         tmpCardList = CardDel( tmpCardList    , card_id );
         CardAdd( work->MainCard , padd_card,loop );
    }

    //最後のカードを登録
    CardAdd( work->MainCard , tmpCardList, 1 );
}


void exec12_03(TCB* thisTCB)
{
    EX12_03_STRUCT* work = (EX12_03_STRUCT*)thisTCB->Work;
    int index = 0;
    EX12_CARD_STRUCT* pchk_card;
    SPRITE  card;

    pchk_card = work->MainCard;

    //リストを辿ってカードを表示
    while( pchk_card != NULL )
    {
        //カードの表示座標を計算
        card.X = (index % CARD_COUNT) * CARD_WIDTH;
        card.Y = (index / CARD_COUNT) * CARD_HEIGHT;
 
        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        index++;
        pchk_card = pchk_card->Next;
    }

}




////////////////////////////////////////////////////
//１２章－４ 役を判定するには
////////////////////////////////////////////////////

typedef struct{
    EX12_CARD_STRUCT*    MainCard;
} EX12_04_STRUCT;
//手札の所持枚数
#define HAND_CARD_COUNT 5

void EX12_04_CardShuffle( EX12_04_STRUCT* CardWork )
{
    EX12_CARD_STRUCT*    padd_card;
    EX12_CARD_STRUCT*    tmpCardList;
    int card_id;
    int loop;

    //カードデータの初期化
    for (loop = 0; loop < CARD_TOTAL; loop++ )
    {   //カードデータの初期化
        CardData[ loop ].Prev     = NULL;
        CardData[ loop ].Next     = NULL;
    }

    //シャッフル用にカードをリストに登録
    tmpCardList = &CardData[0];
    for( loop = 1; loop < CARD_TOTAL; loop++)
    {
         CardAdd( tmpCardList , &CardData[ loop ],loop );
    }

    //最初の１枚  リストから乱数でカードを１枚抜き出す
    card_id = rand() % CARD_TOTAL;
    padd_card = CardGet( tmpCardList,card_id );
    tmpCardList = CardDel( tmpCardList    , card_id );
    //抜き出したカードを手札のリストに登録
    CardWork->MainCard = padd_card;

    //残りのカードも同様に、乱数で抜き出して手札のリストに登録
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

    //使用するテクスチャの読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\card.png",&g_pTex[0] );

    //カードの初期化
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


    //キー入力で再度シャッフルして手札を配る
    if( g_DownInputBuff & KEY_Z    )
    {
        EX12_04_CardShuffle( work );
    }

    //手札の表示
    pchk_card = work->MainCard;
    for( loop = 0; loop < HAND_CARD_COUNT ; loop++ )
    {
        //カードの表示座標を計算
        card.X = loop * CARD_WIDTH + 128;
        card.Y = 128;

        card.SrcRect = &pchk_card->CardChipData;
        SpriteDraw( &card, 0);
        pchk_card = pchk_card->Next;
    }

    //チェック用のカウンタを初期化する
    for( loop = 0;loop < CARD_TYPE ;loop++ ) card_TypeCount[ loop ] = 0;
    for( loop = 0;loop < CARD_COUNT;loop++ ) card_NoCount[ loop ] = 0;

    //手札の種類をカウントする
    pchk_card = work->MainCard;
    for( loop = 0;loop < HAND_CARD_COUNT ;loop++ )
    {
        card_NoCount[ pchk_card->CardNo ]++;
        card_TypeCount[ pchk_card->CardType ]++;
        pchk_card = pchk_card->Next;
    }

    //役の情報を集める
    for( loop = 0;loop < CARD_COUNT ;loop++ )
    {
         //ペアの数をカウントする
         if( card_NoCount[ loop ] == 2 )pair_count++;

         //最大数のカードNoをカウントする
         if( card_NoCount[ loop ] > max_count )max_count = card_NoCount[ loop ];

         //カードが何回連続しているかをカウントする
         if( loop != 0)     //最初の１枚目はカウント出来ない
         {
             if( card_NoCount[ loop - 1 ] != 0 && card_NoCount[ loop ] != 0 )
             {
                 straight++;
             }
         }
    }
    for( loop = 0;loop < CARD_TYPE ;loop++ )
    {
         //同種のカードが最大何枚あるかをカウントする
         if( card_TypeCount[ loop ] > flush )flush = card_TypeCount[ loop ];
    }


    //集めた情報を元に実際の役を判定

    //ワンペア
    if( pair_count == 1 )
    {
        g_pFont->DrawText( NULL, "ワンペア", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //ツーペア
    if( pair_count == 2 )
    {
        g_pFont->DrawText( NULL, "ツーペア", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //スリーカード
    if( max_count == 3 )
    {
        g_pFont->DrawText( NULL, "スリーカード", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //ストレート
    if( straight == 4 )
    {
        g_pFont->DrawText( NULL, "ストレート", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //フラッシュ
    if( flush == 5 )
    {
        g_pFont->DrawText( NULL, "フラッシュ", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //フルハウス
    if( pair_count == 1 && max_count == 3 )
    {
        g_pFont->DrawText( NULL, "フルハウス", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //フォーカード
    if( max_count == 4 )
    {
        g_pFont->DrawText( NULL, "フォーカード", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }

    //ストレートフラッシュ
    if( straight ==  4 && flush == 5 )
    {
        g_pFont->DrawText( NULL, "ストレートフラッシュ", -1, &font_pos, DT_LEFT, 0xffffffff);
        font_pos.top += 24;
    }



}





////////////////////////////////////////////////////
//１２章－５ ランダム
////////////////////////////////////////////////////

void exec12_05(TCB* thisTCB)
{
    int*   int_work = thisTCB->Work;
    float* float_work1 = (float*)&thisTCB->Work[1];
    float* float_work2 = (float*)&thisTCB->Work[2];

    RECT font_pos = {160, 192,640,480,};
    char str[256];

//好きな範囲の乱数
#define rand_range( a, b ) ( (rand() % ( b - a + 1 )) + a )

//小数点化 ０～１．０の間の数値を返す
#define rand_float() ((float)rand() / (float)RAND_MAX)

//好きな範囲の乱数  小数点版
#define rand_range_float( a, b ) ( ( rand_float() * ( b - a )) + a )

    //キー入力で乱数値を再計算
    if( g_DownInputBuff & KEY_Z    )
    {
         *int_work    = rand_range( 25, 50 );
         *float_work1 = rand_float();
         *float_work2 = rand_range_float( 3, 17 );
    }

    g_pFont->DrawText( NULL, "'Ｚ'キー  で乱数値を計算", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 48;

    g_pFont->DrawText( NULL, "範囲２５－５０の間の乱数", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%d", *int_work );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;


    g_pFont->DrawText( NULL, "小数点の乱数", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%f",*float_work1 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;


    g_pFont->DrawText( NULL, "範囲３－２７の間の乱数 小数点", -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 16;
    sprintf( str,"%f",*float_work2 );
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
    font_pos.top += 32;

}





////////////////////////////////////////////////////
//１２章－６ 敵車の動き
////////////////////////////////////////////////////

//目標となるポイントの数
#define POINT_MAX 4
#define START_X   320
#define START_Y   96
#define MOVE_TIME 60.0

typedef struct{
    SPRITE      Sprt;
    BACK_GROUND Bg;
    int         Time;
    int         Point;
    //移動速度
    float     AddX;
    float     AddY;
} EX12_06_STRUCT;

void init12_06(TCB* thisTCB)
{
    EX12_06_STRUCT* work = (EX12_06_STRUCT*)thisTCB->Work;

    //使用するテクスチャの読み込み
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
//チェックポイントの座標データ
float point_data[][2] = 
{//  X     Y
    {512.0,128.0,},  //1箇所目
    {512.0,352.0,},  //2箇所目
    { 96.0,352.0,},  //3箇所目
    { 96.0,128.0,},  //4箇所目
};

    //チェックポイントのチェック
    if(work->Time-- <= 0)
    {//一定時間でチェックポイントを通過
        work->Time = MOVE_TIME;

        //目標ポイントからの相対座標を計算
        posX = point_data[work->Point][0] - work->Sprt.X;
        posY = point_data[work->Point][1] - work->Sprt.Y;

        //目標ポイントへの方向を計算
        direction = atan2( posY, posX);
 
        //目標ポイントへの距離を計測し、所要時間から速度を割り出す
        distance = sqrtf( (posX * posX) + (posY * posY) );

        work->AddX = cos( direction ) * distance / MOVE_TIME;
        work->AddY = sin( direction ) * distance / MOVE_TIME;

        //次のポイント
        work->Point++;
        if( work->Point >= POINT_MAX ) work->Point = 0;
    }

    work->Sprt.X += work->AddX;
    work->Sprt.Y += work->AddY;

    //背景とボールの表示
    BGDraw( &work->Bg, 1);
    SpriteDraw( &work->Sprt, 0);
}



////////////////////////////////////////////////////
//１２章－７ 逆走を知る
////////////////////////////////////////////////////
//データ値の最大値
#define DATA_MAX 6
#define MOVE_SPEED 4
#define START_X   320
#define START_Y   96
//マップデータの大きさ（ピクセルサイズ）
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

    //使用するテクスチャの読み込み
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

    //過去の座標を保持
    old_posX = work->Sprt.X;
    old_posY = work->Sprt.Y;

    //キー入力による移動
    if( g_InputBuff & KEY_UP    ) work->Sprt.Y -= MOVE_SPEED;
    if( g_InputBuff & KEY_DOWN  ) work->Sprt.Y += MOVE_SPEED;
    if( g_InputBuff & KEY_RIGHT ) work->Sprt.X += MOVE_SPEED;
    if( g_InputBuff & KEY_LEFT  ) work->Sprt.X -= MOVE_SPEED;


    //現在座標のデータを取得
    chk_data = 
        map_data [ (int)(work->Sprt.Y+BALL_CENTER)/DATA_SIZE ]
                 [ (int)(work->Sprt.X+BALL_CENTER)/DATA_SIZE ];

    //進入禁止区域かチェックする
    if(chk_data == -1)
    {//もし進入できなければ、座標を戻す
        work->Sprt.X = old_posX;
        work->Sprt.Y = old_posY;
        //データも再度取得
        chk_data = 
            map_data [(int)(work->Sprt.Y+BALL_CENTER)/DATA_SIZE ]
                     [(int)(work->Sprt.X+BALL_CENTER)/DATA_SIZE ];
    }

    //データタイプを比較し前進しているかをチェック
    if(work->ForwardData == chk_data)
    {//前進時の処理
         //次に前進したとみなすデータタイプを設定
         work->ForwardData++;
         //データ値が最大なら次のチェックは0
         if(work->ForwardData >= DATA_MAX)
             work->ForwardData = 0;
    }

    //逆走のチェック
    reverse_data = work->ForwardData-1;
    if(reverse_data < 0) reverse_data = DATA_MAX-1;

    if(chk_data != reverse_data)
    {//もし該当エリア以外に進んだら、逆送とみなす
         work->ReverseFlag = true;
    }else{
     //そうでなければ逆走解除
         work->ReverseFlag = false;
    }

    if(work->ReverseFlag)
    {//逆走中はメッセージを表示
        g_pFont->DrawText(
            NULL,"逆送しています", -1, &font_pos, DT_CENTER, 0xffffffff);
    }
    //背景とボールの表示
    BGDraw( &work->Bg, 1);
    SpriteDraw( &work->Sprt, 0);
}




////////////////////////////////////////////////////
//１２章－８  時間を計るには
////////////////////////////////////////////////////

typedef struct{
    int   StartTime;
} EX12_08_STRUCT;

void init12_08(TCB* thisTCB)
{
    EX12_08_STRUCT* work = (EX12_08_STRUCT*)thisTCB->Work;

    //計測開始の時間を記録しておく
    work->StartTime = timeGetTime();
}

void exec12_08(TCB* thisTCB)
{
#define SECOND 1000
    EX12_08_STRUCT* work = (EX12_08_STRUCT*)thisTCB->Work;
    int time;
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    //開始時間から経過した時間を計算して取得
    time = timeGetTime() - work->StartTime;

    //経過時間の表示
    sprintf( str,"経過時間 %2d.%03d",time / SECOND,time % SECOND);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}





////////////////////////////////////////////////////
//１２章－９ 10進→16進変換
////////////////////////////////////////////////////
void exec12_09(TCB* thisTCB)
{
//１０進数からの変換
#define BASE_RADIX 10
    char str[128];
    RECT font_pos = {  0,  0,640,480,};

    int ans = 0;
    int digit = 1;
    int digit_count = 0;
    //変換用の10進数データ
    char* in_str = "32579";


    //１桁目（右端）を検出
    while( in_str[ ++digit_count ] != NULL );

    for(digit_count--; digit_count >= 0; digit_count--)
    {//各桁ごとに基数の桁を掛けてやる
        ans += (in_str[ digit_count ] - '0') * digit;
        digit *= BASE_RADIX;
    }

    //変換数値の表示
    sprintf( str,"変換数値 %d",ans);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);
}


////////////////////////////////////////////////////
//１２章－１０ 16進→10進変換
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

    //１６進→１０進変換
    for( loop=0; loop < 9; loop++ )
    {
        //１桁毎に文字列へ変換
        str_buff[ loop ] = cnv_tbl[ in_num / div_tbl[ loop ] ];
        if( str_buff[ loop ] != '0' )
        {//各桁ごとに商を引く(余りを求める)
            in_num %= div_tbl[ loop ];
        }
    }
    str_buff[ loop ] = cnv_tbl[ in_num ];
    //終端文字列を加える
    str_buff[ loop+1 ] = NULL;

    //変換数値の表示
    sprintf( str,"変換数値 %s",str_buff);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_LEFT, 0xffffffff);

}


////////////////////////////////////////////////////
//１２章－１１ ラジアン⇔度数変換
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
  {init03_SCENE_CTRL, exec03_SCENE_CTRL,"３章－SAMPLE", },
  {init03_01, exec03_01, "  ３章－０１",},
  {init03_11, exec03_11, "  ３章－１１",},
  {init03_13, exec03_13, "  ３章－１３",},
  {init03_14, exec03_14, "  ３章－１４",},
  {init03_16, exec03_16, "  ３章－１６",},
  {     NULL, exec03_18, "  ３章－１８",},
  {init03_19, exec03_19, "  ３章－１９",},
  {init03_20, exec03_20, "  ３章－２０",},
  {     NULL, exec03_22, "  ３章－２２",},
  {init03_script, exec03_script, "３章－SCRIPT SHOOTING",},
  {init03_27, exec03_27, "  ３章－２７",},
  {init03_28, exec03_28, "  ３章－２８",},
  {init03_29, exec03_29, "  ３章－２９",},
  {     NULL, exec03_30, "  ３章－３０",},
  {init03_script_rpg, exec03_script_rpg, "３章－SCRIPT RPG", },

  {     NULL, exec04_01, "  ４章－０１",},
  {     NULL, exec04_05, "  ４章－０５",},
  {     NULL, exec04_07, "  ４章－０７",},
  {     NULL, exec04_09, "  ４章－０９",},
  {     NULL, exec04_11, "  ４章－１１",},
  {init04_12, exec04_12, "  ４章－１２",},

  {init05_01, exec05_01, "  ５章－０１",},
  {init05_02, exec05_02, "  ５章－０２",},
  {     NULL, exec05_03, "  ５章－０３",},
  {init05_04, exec05_04, "  ５章－０４",},
  {init05_05, exec05_05, "  ５章－０５",},
  {init05_06, exec05_06, "  ５章－０６",},
  {init05_07, exec05_07, "  ５章－０７",},
  {init05_08, exec05_08, "  ５章－０８",},
  {init05_09, exec05_09, "  ５章－０９",},
  {init05_10, exec05_10, "  ５章－１０",},
  {init05_12, exec05_12, "  ５章－１２",},
  {init05_13, exec05_13, "  ５章－１３",},
  {init05_14, exec05_14, "  ５章－１４",},
  {init05_15, exec05_15, "  ５章－１５",},
  {init05_16, exec05_16, "  ５章－１６",},
  {init05_17, exec05_17, "  ５章－１７",},
  {init05_18, exec05_18, "  ５章－１８",},
  {init05_19, exec05_19, "  ５章－１９",},
  {init05_20, exec05_20, "  ５章－２０",},
  {init05_21, exec05_21, "  ５章－２１",},

  {init06_02, exec06_02, "  ６章－０２",},
  {init06_03, exec06_03, "  ６章－０３",},
  {init06_04, exec06_04, "  ６章－０４",},
  {init06_05, exec06_05, "  ６章－０５",},
  {init06_06, exec06_06, "  ６章－０６",},
  {init06_07, exec06_07, "  ６章－０７",},
  {init06_08, exec06_08, "  ６章－０８",},
  {init06_09, exec06_09, "  ６章－０９",},
  {init06_10, exec06_10, "  ６章－１０",},
  {init06_11, exec06_11, "  ６章－１１",},
  {init06_12, exec06_12, "  ６章－１２",},
  {init06_13, exec06_13, "  ６章－１３",},
  {init06_14, exec06_14, "  ６章－１４",},
  {init06_15, exec06_15, "  ６章－１５",},
  {init06_16, exec06_16, "  ６章－１６",},
  {init06_17, exec06_17, "  ６章－１７",},
  {init06_18, exec06_18, "  ６章－１８",},
  {init06_19, exec06_19, "  ６章－１９",},
  {init06_20, exec06_20, "  ６章－２０",},
  {init06_21, exec06_21, "  ６章－２１",},
  {init06_22, exec06_22, "  ６章－２２",},
  {init06_23, exec06_23, "  ６章－２３",},
  {init06_24, exec06_24, "  ６章－２４",},
  {init06_25, exec06_25, "  ６章－２５",},

  {init07_01, exec07_01, "  ７章－０１",},
  {init07_02, exec07_02, "  ７章－０２",},
  {init07_03, exec07_03, "  ７章－０３",},
  {init07_04, exec07_04, "  ７章－０４",},
  {init07_05, exec07_05, "  ７章－０５",},
  {init07_06, exec07_06, "  ７章－０６",},
  {init07_07, exec07_07, "  ７章－０７",},
  {init07_08, exec07_08, "  ７章－０８",},
  {init07_09, exec07_09, "  ７章－０９",},
  {init07_10, exec07_10, "  ７章－１０",},
  {init07_11, exec07_11, "  ７章－１１",},
  {init07_12, exec07_12, "  ７章－１２",},
  {init07_13, exec07_13, "  ７章－１３",},
  {init07_14, exec07_14, "  ７章－１４",},
  {init07_16, exec07_16, "  ７章－１６",},
  {init07_17, exec07_17, "  ７章－１７",},
  {     NULL, exec07_18, "  ７章－１８",},

  {init08_01, exec08_01, "  ８章－０１",},
  {init08_02, exec08_02, "  ８章－０２",},
  {init08_03, exec08_03, "  ８章－０３",},
  {init08_04, exec08_04, "  ８章－０４",},
  {init08_05, exec08_05, "  ８章－０５",},
  {init08_06, exec08_06, "  ８章－０６",},
  {init08_07, exec08_07, "  ８章－０７",},
  {init08_09, exec08_09, "  ８章－０９",},
  {init08_10, exec08_10, "  ８章－１０",},
  {init08_11, exec08_11, "  ８章－１１",},
  {init08_12, exec08_12, "  ８章－１２",},
  {init08_13, exec08_13, "  ８章－１３",},
  {init08_15, exec08_15, "  ８章－１５",},

  {init09_01, exec09_01, "  ９章－０１",},
  {init09_02, exec09_02, "  ９章－０２",},
  {init09_06, exec09_06, "  ９章－０６",},
  {init09_07, exec09_07, "  ９章－０７",},
  {init09_08, exec09_08, "  ９章－０８",},
  {init09_09, exec09_09, "  ９章－０９",},
  {init09_10, exec09_10, "  ９章－１０",},
  {init09_11, exec09_11, "  ９章－１１",},

  {init10_02, exec10_02, "１０章－０２",},
  {init10_03, exec10_03, "１０章－０３",},
  {     NULL, exec10_06, "１０章－０６" },

  {init11_02, exec11_02, "１１章－０２" },
  {init11_03, exec11_03, "１１章－０３" },
  {init11_05, exec11_05, "１１章－０５" },
  {init11_06, exec11_06, "１１章－０６" },
  {init11_07, exec11_07, "１１章－０７" },

  {init12_01, exec12_01, "１２章－０１" },
  {init12_03, exec12_03, "１２章－０３" },
  {init12_04, exec12_04, "１２章－０４" },
  {     NULL, exec12_05, "１２章－０５" },
  {init12_06, exec12_06, "１２章－０６" },
  {init12_07, exec12_07, "１２章－０７" },
  {init12_08, exec12_08, "１２章－０８" },
  {     NULL, exec12_09, "１２章－０９" },
  {     NULL, exec12_10, "１２章－１０" },
};

#define FUNC_COUNT (sizeof(chapter_table) / sizeof(chapter_prog ) )

void GameMain(TCB* thisTCB)
{
static int select_chapter = 0;
    char str[128];
    RECT font_pos = {0,0, 640, 480, };

    //メニュー選択
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
            //開始時に１度だけ実行する関数
            chapter_table[select_chapter].InitFunc(thisTCB);
        //毎フレーム実行する関数
        TaskChange(thisTCB,chapter_table[select_chapter].ExecFunc);
    }

    font_pos.top  = SCREEN_HEIGHT/2;
    sprintf( str,"%s",chapter_table[ select_chapter ].ChapterName);
    g_pFont->DrawText( NULL, str, -1, &font_pos, DT_CENTER, 0xffffffff);

    font_pos.top  = SCREEN_HEIGHT/3 * 2;
    g_pFont->DrawText( NULL, "方向キーでメニュー選択\nＺキーで実行", -1, &font_pos, DT_CENTER, 0xffffffff);

}

