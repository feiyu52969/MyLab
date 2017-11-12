#define _GLOBAL_DEFINE_


#include <windows.h>
#include <stdio.h>
#include <math.h>

#define sqrtf sqrt
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>

#include "main.h"
#include "task.h"




//メインループ処理の一番最後に実行される
void TaskTail(TCB* thisTCB)
{
    g_pSp->End();
    g_pD3Ddevice->EndScene();
    // 表示
    g_pD3Ddevice->Present( NULL, NULL, NULL, NULL );

}

//入力関係の処理
void KeyControll(TCB* thisTCB)
{
#define RAPID_COUNT 5        //連射の間隔
#define HALF_RAPID_COUNT 30   //半自動連射の連射時間
#define KEY_INDEX_X 5
#define KEY_INDEX_Z 6

typedef struct
{
    unsigned char     RapidCount[8];         //連射間隔カウント
    unsigned char     HalfRapidCount[8];     //半自動連射、連射数カウント

} TASK_HEAD_STRUCT;

    TASK_HEAD_STRUCT* pTASK_work = (TASK_HEAD_STRUCT*)thisTCB->Work;

    unsigned char BeforeInputBuff;
    POINT mouse_pos;
    POINT client_mouse_pos;
    JOYINFO joyinfo;

    //キー情報を取得
    BeforeInputBuff = g_InputBuff;        //１フレーム前のキー状態を保存
    GetKeyboardState(g_KeyboardBuff);     //キーボード情報の取得
    g_KeyInputBuff = 0;
    if( g_KeyboardBuff[VK_UP]   & 0x80 ) g_KeyInputBuff |= KEY_UP;
    if( g_KeyboardBuff[VK_DOWN] & 0x80 ) g_KeyInputBuff |= KEY_DOWN;
    if( g_KeyboardBuff[VK_LEFT] & 0x80 ) g_KeyInputBuff |= KEY_LEFT;
    if( g_KeyboardBuff[VK_RIGHT]& 0x80 ) g_KeyInputBuff |= KEY_RIGHT;
    if( g_KeyboardBuff[ 'Z' ]   & 0x80 ) g_KeyInputBuff |= KEY_Z;
    if( g_KeyboardBuff[ 'X' ]   & 0x80 ) g_KeyInputBuff |= KEY_X;


    //ジョイスティックの情報を取得
    g_JoystickBuff = 0;
    if( !joyGetPos( JOYSTICKID1, &joyinfo) )
    {
        if( joyinfo.wYpos   == 0     ) g_JoystickBuff |= KEY_UP;
        if( joyinfo.wYpos   == 65535 ) g_JoystickBuff |= KEY_DOWN;
        if( joyinfo.wXpos   == 0     ) g_JoystickBuff |= KEY_LEFT;
        if( joyinfo.wXpos   == 65535 ) g_JoystickBuff |= KEY_RIGHT;
        if( joyinfo.wButtons & 0x01  ) g_JoystickBuff |= KEY_Z;
        if( joyinfo.wButtons & 0x02  ) g_JoystickBuff |= KEY_X;
    }

    //ジョイスティックとキーボードの入力を合成
    g_InputBuff = g_JoystickBuff | g_KeyInputBuff;

    //入力のON/OFFを算出
    g_DownInputBuff  =  g_InputBuff & ~BeforeInputBuff;
    g_UpInputBuff    = ~g_InputBuff &  BeforeInputBuff;


    //マウス情報を取得
    GetCursorPos( &mouse_pos );
    ScreenToClient( g_hWnd, &mouse_pos );         //クライアント座標へ変換
    g_MousePos.x = mouse_pos.x;
    g_MousePos.y = mouse_pos.y;

    g_MouseButton = 0;
    if( g_KeyboardBuff[VK_LBUTTON] & 0x80) g_MouseButton |= MOUSE_L;
    if( g_KeyboardBuff[VK_RBUTTON] & 0x80) g_MouseButton |= MOUSE_R;


    //連射等特殊処理
    g_RapidBuff = 0;
    g_HalfRapidFlag  = 0;
    if( g_HalfRapidFlag ){
        if( g_InputBuff & KEY_X )
        {//Ｘボタンが押されている時のみ連射処理
            ++pTASK_work->RapidCount[ KEY_INDEX_X ] %= RAPID_COUNT;
            if( !pTASK_work->RapidCount[ KEY_INDEX_X ] )
            {
                g_RapidBuff |= KEY_X;
            }
        } else {
            pTASK_work->RapidCount[ KEY_INDEX_X ] = 0;
        }

        if( g_InputBuff & KEY_Z )
        {
            ++pTASK_work->RapidCount[ KEY_INDEX_Z ] %= RAPID_COUNT;
            if( !pTASK_work->RapidCount[ KEY_INDEX_Z ] )
            {
                g_RapidBuff |= KEY_Z;
            }
        } else {
            pTASK_work->RapidCount[ KEY_INDEX_Z ] = 0;
        }

    } else {
    //半自動連射 
        //ボタンが押されていたら半自動連射モードに切り替え
        if( g_DownInputBuff & KEY_X )
            pTASK_work->HalfRapidCount[ KEY_INDEX_X ] = HALF_RAPID_COUNT;

        //半自動連射モード中は連射
        if( pTASK_work->HalfRapidCount[ KEY_INDEX_X ] != 0)
        {
            ++pTASK_work->RapidCount[ KEY_INDEX_X ] %= RAPID_COUNT;
            if( !pTASK_work->RapidCount[ KEY_INDEX_X ] )
            {
                g_RapidBuff |= KEY_X;
            }
        } else {
            pTASK_work->RapidCount[ KEY_INDEX_X ] = 0;
        }

        if( pTASK_work->HalfRapidCount[ KEY_INDEX_X ] )
                pTASK_work->HalfRapidCount[ KEY_INDEX_X ]--;


        //同様にＺキーも処理する
        if( g_DownInputBuff & KEY_Z )
            pTASK_work->HalfRapidCount[ KEY_INDEX_Z ] = HALF_RAPID_COUNT;

        if( pTASK_work->HalfRapidCount[KEY_INDEX_Z] != 0)
        {
            ++pTASK_work->RapidCount[ KEY_INDEX_Z ] %= RAPID_COUNT;
            if( !pTASK_work->RapidCount[ KEY_INDEX_Z ] )
            {
                g_RapidBuff |= KEY_Z;
            }
        } else {
            pTASK_work->RapidCount[ KEY_INDEX_Z ] = 0;
        }

        if( pTASK_work->HalfRapidCount[ KEY_INDEX_Z ] )
                pTASK_work->HalfRapidCount[ KEY_INDEX_Z ]--;

    }

}


//メインループ処理の一番初めに実行される
void TaskHead00(TCB* thisTCB)
{
#define FRAME_RATE (1000.0/30.0)
static int BeforeTime = 0;
static double StoreErr = 0.0;
    int sleep_time;
    int now_time;

    //まずは同期を取る
    //前回からの経過時間を取得
    now_time = timeGetTime();

    //基本待ち時間を得る
    StoreErr += FRAME_RATE;
    sleep_time = StoreErr;
    StoreErr -= sleep_time;
    //経過時間から、待ち時間を決定する
    sleep_time -= now_time - BeforeTime;

    //処理落ちのチェック
    if( sleep_time < 0.0)
    {//処理落ちしたときは誤差もリセット
        sleep_time =  0.0;
        StoreErr = 0.0;
    }

    //時間待ち
    Sleep(sleep_time);
    BeforeTime = now_time;


//各タスク実行前処理

    g_pD3Ddevice->Clear( 0, NULL, D3DCLEAR_TARGET , //画像のクリア
    D3DCOLOR_XRGB(0,0,64), 1.0f, 0 );
    //開始宣言
    g_pD3Ddevice->BeginScene();
    g_pSp->Begin( NULL );
    KeyControll( thisTCB );

    g_Count += 1;                      //フレーム数カウント


}



//メイン処理関数の外部参照
extern void GameMain(TCB*);

void TaskHead(TCB* thisTCB)
{
    TaskMake(TaskTail,0xffffffff);      //処理の一番最後に実行されるタスクを作成
    TaskMake(GameMain,0x1000);          //ゲーム管理用のタスクを作成
    TaskChange(thisTCB,TaskHead00);     //処理の切り替え、但し実際処理が切り替わるのは１フレーム後
    TaskHead00(thisTCB);                //このフレームでも同じ処理をする

    g_Count = 0;                        //フレーム数カウント
}


BOOL InitializeDirectX( HINSTANCE hInst)
{
    int loop;


    //////////////////////////////
    //Direct3Dオブジェクトを生成
    //////////////////////////////
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
    {
       MessageBox(g_hWnd,"Err1:DirectXDeviceの初期化に失敗しました", "確認",NULL);
       return(false);
    }

    //デフォルト3Dデバイスを取得
    D3DDISPLAYMODE d3ddm;
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
    {
       MessageBox(g_hWnd,"Err2:DirectX3DDeviceの初期化に失敗しました", "確認",NULL);
       return(false);
    }

   // 細かい画面モードの設定
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    d3dpp.PresentationInterval  = D3DPRESENT_INTERVAL_IMMEDIATE;

    //D3Dデバイスオブジェクトの作成
    if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,g_hWnd,
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                    &d3dpp,&g_pD3Ddevice)))
    {
       MessageBox(g_hWnd,"Err3:3DDeviceObjectの初期化に失敗しました", "確認",NULL);
       return(false);
    }


    //////////////////////////////
    //スプライトオブジェクトの作成
    //////////////////////////////
    if(FAILED(D3DXCreateSprite(g_pD3Ddevice,&g_pSp)))
    {
       MessageBox(g_hWnd,"Err4:SpriteObjectの作成に失敗しました", "確認",NULL);
       return(false);
    }


    //////////////////////////////
    //フォントオブジェクトの作成
    //////////////////////////////
    D3DXCreateFont(g_pD3Ddevice,
     16,                                //幅
    NULL,                               //高さ
    FW_DONTCARE,                        //太さ
    NULL,                               //ミップマップレベル
    FALSE,                              //斜体
    SHIFTJIS_CHARSET,                   //文字セット
    OUT_DEFAULT_PRECIS,                 //出力精度
    DEFAULT_QUALITY,                    //出力品質
    DEFAULT_PITCH | FF_DONTCARE,        //フォントピッチ&フォントファミリ
    "ＭＳ ゴシック",                    //フォント名
    &g_pFont);


    //フォントテクスチャ(スプライト表示用）の読み込み
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\font.png",
                               &g_pTex[FONT_BITMAP_ID] );



    //////////////////////////////
    // アルファブレンドの設定
    //////////////////////////////
    g_pD3Ddevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pD3Ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pD3Ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );


    //////////////////////////////
    //DirectSoundの設定
    //////////////////////////////
    g_WaveControllBGM = (WAVE_CONTROLL_STRUCT*)g_WaveDataBGM;                     //サンドバッファ管理用構造体にワークを割り当てる newやmallocでもＯＫ
    for( loop = 0; loop < SE_MAX; loop++ )
    {
        g_WaveControllSE[ loop ] = (WAVE_CONTROLL_STRUCT*)g_WaveDataSE[ loop ];   //SEのサンドバッファ管理用構造体
    }

    //DirectSoundオブジェクト作成
    if(FAILED(DirectSoundCreate8( NULL, &g_pDSound ,NULL ) ))
    {
       MessageBox(g_hWnd,"Err5:SoundObjectの作成に失敗しました", "確認",NULL);
       return(false);
    }

    //協調レベルの設定
    g_pDSound->SetCooperativeLevel( g_hWnd, DSSCL_PRIORITY );


    return( true );
}






void CloseWindowProcess( void ) 
{
    mciSendString( "stop cdaudio", NULL, 0, NULL );
    mciSendString( "set cdaudio door closed", NULL, 0, NULL );
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd, msg, wParam, lParam));
    }
    return (0);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASS wnd;
    int win_width;
    int win_height;

    //クライアント領域の大きさからウィンドウの大きさを取得
    //AdjustWindowrectではうまく行かない？？？
    win_width  = GetSystemMetrics( SM_CXFRAME ) * 2 + SCREEN_WIDTH;
    win_height = GetSystemMetrics( SM_CYFRAME ) * 2 + 
                 GetSystemMetrics( SM_CYCAPTION )   + SCREEN_HEIGHT; 

    if (!hPreInst) {
        wnd.style            = 0;
        wnd.lpfnWndProc      = WndProc;
        wnd.cbClsExtra       = 0;
        wnd.cbWndExtra       = 0;
        wnd.hInstance        = hInstance;
        wnd.hIcon            = NULL;
        wnd.hCursor          = LoadCursor(NULL, IDC_ARROW);
        wnd.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wnd.lpszMenuName     = NULL;
        wnd.lpszClassName    = "Window";

        if (!RegisterClass(&wnd)) return FALSE;
    }

    g_hWnd = CreateWindow("Window",
        NULL,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        win_width,
        win_height,
        NULL,
        NULL,
        hInstance,
        NULL);
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    //DirectXの初期化
    if( InitializeDirectX(hInstance) == false)return(false);

    //使用するワークの初期化
    InitTask();

    //メインループ
    while(TRUE){
        //メッセージが来ているか？
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        //来ている
            if(msg.message == WM_QUIT) break;   //終了
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }else{
        //来ていない　この間に主処理
            TaskExec();
        }
    }


    CloseWindowProcess();

    return (msg.wParam);
}

