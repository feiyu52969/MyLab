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




//���C�����[�v�����̈�ԍŌ�Ɏ��s�����
void TaskTail(TCB* thisTCB)
{
    g_pSp->End();
    g_pD3Ddevice->EndScene();
    // �\��
    g_pD3Ddevice->Present( NULL, NULL, NULL, NULL );

}

//���͊֌W�̏���
void KeyControll(TCB* thisTCB)
{
#define RAPID_COUNT 5        //�A�˂̊Ԋu
#define HALF_RAPID_COUNT 30   //�������A�˂̘A�ˎ���
#define KEY_INDEX_X 5
#define KEY_INDEX_Z 6

typedef struct
{
    unsigned char     RapidCount[8];         //�A�ˊԊu�J�E���g
    unsigned char     HalfRapidCount[8];     //�������A�ˁA�A�ː��J�E���g

} TASK_HEAD_STRUCT;

    TASK_HEAD_STRUCT* pTASK_work = (TASK_HEAD_STRUCT*)thisTCB->Work;

    unsigned char BeforeInputBuff;
    POINT mouse_pos;
    POINT client_mouse_pos;
    JOYINFO joyinfo;

    //�L�[�����擾
    BeforeInputBuff = g_InputBuff;        //�P�t���[���O�̃L�[��Ԃ�ۑ�
    GetKeyboardState(g_KeyboardBuff);     //�L�[�{�[�h���̎擾
    g_KeyInputBuff = 0;
    if( g_KeyboardBuff[VK_UP]   & 0x80 ) g_KeyInputBuff |= KEY_UP;
    if( g_KeyboardBuff[VK_DOWN] & 0x80 ) g_KeyInputBuff |= KEY_DOWN;
    if( g_KeyboardBuff[VK_LEFT] & 0x80 ) g_KeyInputBuff |= KEY_LEFT;
    if( g_KeyboardBuff[VK_RIGHT]& 0x80 ) g_KeyInputBuff |= KEY_RIGHT;
    if( g_KeyboardBuff[ 'Z' ]   & 0x80 ) g_KeyInputBuff |= KEY_Z;
    if( g_KeyboardBuff[ 'X' ]   & 0x80 ) g_KeyInputBuff |= KEY_X;


    //�W���C�X�e�B�b�N�̏����擾
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

    //�W���C�X�e�B�b�N�ƃL�[�{�[�h�̓��͂�����
    g_InputBuff = g_JoystickBuff | g_KeyInputBuff;

    //���͂�ON/OFF���Z�o
    g_DownInputBuff  =  g_InputBuff & ~BeforeInputBuff;
    g_UpInputBuff    = ~g_InputBuff &  BeforeInputBuff;


    //�}�E�X�����擾
    GetCursorPos( &mouse_pos );
    ScreenToClient( g_hWnd, &mouse_pos );         //�N���C�A���g���W�֕ϊ�
    g_MousePos.x = mouse_pos.x;
    g_MousePos.y = mouse_pos.y;

    g_MouseButton = 0;
    if( g_KeyboardBuff[VK_LBUTTON] & 0x80) g_MouseButton |= MOUSE_L;
    if( g_KeyboardBuff[VK_RBUTTON] & 0x80) g_MouseButton |= MOUSE_R;


    //�A�˓����ꏈ��
    g_RapidBuff = 0;
    g_HalfRapidFlag  = 0;
    if( g_HalfRapidFlag ){
        if( g_InputBuff & KEY_X )
        {//�w�{�^����������Ă��鎞�̂ݘA�ˏ���
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
    //�������A�� 
        //�{�^����������Ă����甼�����A�˃��[�h�ɐ؂�ւ�
        if( g_DownInputBuff & KEY_X )
            pTASK_work->HalfRapidCount[ KEY_INDEX_X ] = HALF_RAPID_COUNT;

        //�������A�˃��[�h���͘A��
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


        //���l�ɂy�L�[����������
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


//���C�����[�v�����̈�ԏ��߂Ɏ��s�����
void TaskHead00(TCB* thisTCB)
{
#define FRAME_RATE (1000.0/30.0)
static int BeforeTime = 0;
static double StoreErr = 0.0;
    int sleep_time;
    int now_time;

    //�܂��͓��������
    //�O�񂩂�̌o�ߎ��Ԃ��擾
    now_time = timeGetTime();

    //��{�҂����Ԃ𓾂�
    StoreErr += FRAME_RATE;
    sleep_time = StoreErr;
    StoreErr -= sleep_time;
    //�o�ߎ��Ԃ���A�҂����Ԃ����肷��
    sleep_time -= now_time - BeforeTime;

    //���������̃`�F�b�N
    if( sleep_time < 0.0)
    {//�������������Ƃ��͌덷�����Z�b�g
        sleep_time =  0.0;
        StoreErr = 0.0;
    }

    //���ԑ҂�
    Sleep(sleep_time);
    BeforeTime = now_time;


//�e�^�X�N���s�O����

    g_pD3Ddevice->Clear( 0, NULL, D3DCLEAR_TARGET , //�摜�̃N���A
    D3DCOLOR_XRGB(0,0,64), 1.0f, 0 );
    //�J�n�錾
    g_pD3Ddevice->BeginScene();
    g_pSp->Begin( NULL );
    KeyControll( thisTCB );

    g_Count += 1;                      //�t���[�����J�E���g


}



//���C�������֐��̊O���Q��
extern void GameMain(TCB*);

void TaskHead(TCB* thisTCB)
{
    TaskMake(TaskTail,0xffffffff);      //�����̈�ԍŌ�Ɏ��s�����^�X�N���쐬
    TaskMake(GameMain,0x1000);          //�Q�[���Ǘ��p�̃^�X�N���쐬
    TaskChange(thisTCB,TaskHead00);     //�����̐؂�ւ��A�A�����ۏ������؂�ւ��̂͂P�t���[����
    TaskHead00(thisTCB);                //���̃t���[���ł���������������

    g_Count = 0;                        //�t���[�����J�E���g
}


BOOL InitializeDirectX( HINSTANCE hInst)
{
    int loop;


    //////////////////////////////
    //Direct3D�I�u�W�F�N�g�𐶐�
    //////////////////////////////
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
    {
       MessageBox(g_hWnd,"Err1:DirectXDevice�̏������Ɏ��s���܂���", "�m�F",NULL);
       return(false);
    }

    //�f�t�H���g3D�f�o�C�X���擾
    D3DDISPLAYMODE d3ddm;
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
    {
       MessageBox(g_hWnd,"Err2:DirectX3DDevice�̏������Ɏ��s���܂���", "�m�F",NULL);
       return(false);
    }

   // �ׂ�����ʃ��[�h�̐ݒ�
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    d3dpp.PresentationInterval  = D3DPRESENT_INTERVAL_IMMEDIATE;

    //D3D�f�o�C�X�I�u�W�F�N�g�̍쐬
    if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,g_hWnd,
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                    &d3dpp,&g_pD3Ddevice)))
    {
       MessageBox(g_hWnd,"Err3:3DDeviceObject�̏������Ɏ��s���܂���", "�m�F",NULL);
       return(false);
    }


    //////////////////////////////
    //�X�v���C�g�I�u�W�F�N�g�̍쐬
    //////////////////////////////
    if(FAILED(D3DXCreateSprite(g_pD3Ddevice,&g_pSp)))
    {
       MessageBox(g_hWnd,"Err4:SpriteObject�̍쐬�Ɏ��s���܂���", "�m�F",NULL);
       return(false);
    }


    //////////////////////////////
    //�t�H���g�I�u�W�F�N�g�̍쐬
    //////////////////////////////
    D3DXCreateFont(g_pD3Ddevice,
     16,                                //��
    NULL,                               //����
    FW_DONTCARE,                        //����
    NULL,                               //�~�b�v�}�b�v���x��
    FALSE,                              //�Α�
    SHIFTJIS_CHARSET,                   //�����Z�b�g
    OUT_DEFAULT_PRECIS,                 //�o�͐��x
    DEFAULT_QUALITY,                    //�o�͕i��
    DEFAULT_PITCH | FF_DONTCARE,        //�t�H���g�s�b�`&�t�H���g�t�@�~��
    "�l�r �S�V�b�N",                    //�t�H���g��
    &g_pFont);


    //�t�H���g�e�N�X�`��(�X�v���C�g�\���p�j�̓ǂݍ���
    D3DXCreateTextureFromFile( g_pD3Ddevice, "..\\..\\data\\font.png",
                               &g_pTex[FONT_BITMAP_ID] );



    //////////////////////////////
    // �A���t�@�u�����h�̐ݒ�
    //////////////////////////////
    g_pD3Ddevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pD3Ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pD3Ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );


    //////////////////////////////
    //DirectSound�̐ݒ�
    //////////////////////////////
    g_WaveControllBGM = (WAVE_CONTROLL_STRUCT*)g_WaveDataBGM;                     //�T���h�o�b�t�@�Ǘ��p�\���̂Ƀ��[�N�����蓖�Ă� new��malloc�ł��n�j
    for( loop = 0; loop < SE_MAX; loop++ )
    {
        g_WaveControllSE[ loop ] = (WAVE_CONTROLL_STRUCT*)g_WaveDataSE[ loop ];   //SE�̃T���h�o�b�t�@�Ǘ��p�\����
    }

    //DirectSound�I�u�W�F�N�g�쐬
    if(FAILED(DirectSoundCreate8( NULL, &g_pDSound ,NULL ) ))
    {
       MessageBox(g_hWnd,"Err5:SoundObject�̍쐬�Ɏ��s���܂���", "�m�F",NULL);
       return(false);
    }

    //�������x���̐ݒ�
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

    //�N���C�A���g�̈�̑傫������E�B���h�E�̑傫�����擾
    //AdjustWindowrect�ł͂��܂��s���Ȃ��H�H�H
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

    //DirectX�̏�����
    if( InitializeDirectX(hInstance) == false)return(false);

    //�g�p���郏�[�N�̏�����
    InitTask();

    //���C�����[�v
    while(TRUE){
        //���b�Z�[�W�����Ă��邩�H
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        //���Ă���
            if(msg.message == WM_QUIT) break;   //�I��
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }else{
        //���Ă��Ȃ��@���̊ԂɎ又��
            TaskExec();
        }
    }


    CloseWindowProcess();

    return (msg.wParam);
}

