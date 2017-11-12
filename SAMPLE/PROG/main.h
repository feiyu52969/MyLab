#ifndef _MAIN_H_
#define _MAIN_H_
//グローバル変数定義エリア
#ifdef EXTERN
#undef EXTERN
#endif 

#ifdef _GLOBAL_DEFINE_    //main.cppで定義        
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN HWND                    g_hWnd;

//画面表示周り
#define SCREEN_WIDTH   640    //画面幅
#define SCREEN_HEIGHT  480    //画面高さ

#define BITMAP_MAX  8         //扱うビットマップの最大数
#define FONT_BITMAP_ID 7      //フォント用ビットマップのＩＤ

EXTERN LPDIRECT3D9             g_pD3D;
EXTERN LPDIRECT3DDEVICE9       g_pD3Ddevice;
EXTERN LPDIRECT3DTEXTURE9      g_pTex[BITMAP_MAX];
EXTERN LPD3DXSPRITE            g_pSp;
EXTERN LPD3DXFONT              g_pFont;


//サウンド周り 
#define SE_MAX  8             //SEの最大数
#define BGM_BUFFER_SIZE   (1024 * 1024)
#define SE_BUFFER_SIZE    (1024 * 128)
#define BGM_READ_SIZE     (1024 * 256)    //BGMは256Kずつ読み込んで再生

typedef struct {
    WAVEFORMATEX               WaveFormat;      //読み込んだWaveファイルの情報
    DSBUFFERDESC               DSBDesc;         //サウンドバッファへの設定情報
    LPDIRECTSOUNDBUFFER        pDSBuffer;       //作成したサウンドバッファ格納用のポインタ
    DWORD                      BeforePlayCursor;//最後にデータを書き込んだ時の読み込みカーソル位置
    unsigned int               WaveFilePtr;     //Waveデータファイルの再生位置
    unsigned char              WaveData[];
} WAVE_CONTROLL_STRUCT;

EXTERN LPDIRECTSOUND8          g_pDSound;       //DirectSound
EXTERN unsigned char           g_WaveDataBGM[ BGM_BUFFER_SIZE + sizeof( WAVE_CONTROLL_STRUCT ) ];         //BGM用データバッファ
EXTERN unsigned char           g_WaveDataSE[ SE_MAX ][ SE_BUFFER_SIZE + sizeof( WAVE_CONTROLL_STRUCT ) ]; //SE用データバッファ
EXTERN WAVE_CONTROLL_STRUCT*   g_WaveControllBGM;                                                         //BGM管理用
EXTERN WAVE_CONTROLL_STRUCT*   g_WaveControllSE[ SE_MAX ];                                                //SE管理用



//入出力（キーボード、ジョイスティック）周り
#define KEY_UP      1<<0       //上
#define KEY_DOWN    1<<1       //下
#define KEY_LEFT    1<<2       //左
#define KEY_RIGHT   1<<3       //右
#define KEY_Z       1<<4       //Ｚ
#define KEY_X       1<<5       //Ｘ

#define MOUSE_L     1<<0       //マウス左ボタン
#define MOUSE_R     1<<1       //マウス右ボタン


EXTERN unsigned char g_KeyboardBuff[256];    //現在のキーボード状態
EXTERN unsigned char g_JoystickBuff;         //現在のジョイスティックの状態
EXTERN unsigned char g_KeyInputBuff;         //現在のキーボード入力をまとめたもの
EXTERN unsigned char g_InputBuff;            //使用頻度の高いキーをまとめたもの
EXTERN unsigned char g_DownInputBuff;        //キーＯＮ時にフラグが立つ
EXTERN unsigned char g_UpInputBuff;          //キーＯＦＦ時にフラグが立つ
EXTERN unsigned char g_RapidBuff;            //連射
EXTERN unsigned char g_HalfRapidBuff;        //半自動連射
EXTERN BOOL          g_HalfRapidFlag;        //自動/半自動連射切り替えフラグ


EXTERN POINT         g_MousePos;             //マウス座標
EXTERN unsigned char g_MouseButton;          //マウスボタン



//その他
EXTERN int            g_Count;                //描画フレーム数カウント


#endif
