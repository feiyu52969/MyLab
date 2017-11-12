#ifndef _MAIN_H_
#define _MAIN_H_
//�O���[�o���ϐ���`�G���A
#ifdef EXTERN
#undef EXTERN
#endif 

#ifdef _GLOBAL_DEFINE_    //main.cpp�Œ�`        
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN HWND                    g_hWnd;

//��ʕ\������
#define SCREEN_WIDTH   640    //��ʕ�
#define SCREEN_HEIGHT  480    //��ʍ���

#define BITMAP_MAX  8         //�����r�b�g�}�b�v�̍ő吔
#define FONT_BITMAP_ID 7      //�t�H���g�p�r�b�g�}�b�v�̂h�c

EXTERN LPDIRECT3D9             g_pD3D;
EXTERN LPDIRECT3DDEVICE9       g_pD3Ddevice;
EXTERN LPDIRECT3DTEXTURE9      g_pTex[BITMAP_MAX];
EXTERN LPD3DXSPRITE            g_pSp;
EXTERN LPD3DXFONT              g_pFont;


//�T�E���h���� 
#define SE_MAX  8             //SE�̍ő吔
#define BGM_BUFFER_SIZE   (1024 * 1024)
#define SE_BUFFER_SIZE    (1024 * 128)
#define BGM_READ_SIZE     (1024 * 256)    //BGM��256K���ǂݍ���ōĐ�

typedef struct {
    WAVEFORMATEX               WaveFormat;      //�ǂݍ���Wave�t�@�C���̏��
    DSBUFFERDESC               DSBDesc;         //�T�E���h�o�b�t�@�ւ̐ݒ���
    LPDIRECTSOUNDBUFFER        pDSBuffer;       //�쐬�����T�E���h�o�b�t�@�i�[�p�̃|�C���^
    DWORD                      BeforePlayCursor;//�Ō�Ƀf�[�^���������񂾎��̓ǂݍ��݃J�[�\���ʒu
    unsigned int               WaveFilePtr;     //Wave�f�[�^�t�@�C���̍Đ��ʒu
    unsigned char              WaveData[];
} WAVE_CONTROLL_STRUCT;

EXTERN LPDIRECTSOUND8          g_pDSound;       //DirectSound
EXTERN unsigned char           g_WaveDataBGM[ BGM_BUFFER_SIZE + sizeof( WAVE_CONTROLL_STRUCT ) ];         //BGM�p�f�[�^�o�b�t�@
EXTERN unsigned char           g_WaveDataSE[ SE_MAX ][ SE_BUFFER_SIZE + sizeof( WAVE_CONTROLL_STRUCT ) ]; //SE�p�f�[�^�o�b�t�@
EXTERN WAVE_CONTROLL_STRUCT*   g_WaveControllBGM;                                                         //BGM�Ǘ��p
EXTERN WAVE_CONTROLL_STRUCT*   g_WaveControllSE[ SE_MAX ];                                                //SE�Ǘ��p



//���o�́i�L�[�{�[�h�A�W���C�X�e�B�b�N�j����
#define KEY_UP      1<<0       //��
#define KEY_DOWN    1<<1       //��
#define KEY_LEFT    1<<2       //��
#define KEY_RIGHT   1<<3       //�E
#define KEY_Z       1<<4       //�y
#define KEY_X       1<<5       //�w

#define MOUSE_L     1<<0       //�}�E�X���{�^��
#define MOUSE_R     1<<1       //�}�E�X�E�{�^��


EXTERN unsigned char g_KeyboardBuff[256];    //���݂̃L�[�{�[�h���
EXTERN unsigned char g_JoystickBuff;         //���݂̃W���C�X�e�B�b�N�̏��
EXTERN unsigned char g_KeyInputBuff;         //���݂̃L�[�{�[�h���͂��܂Ƃ߂�����
EXTERN unsigned char g_InputBuff;            //�g�p�p�x�̍����L�[���܂Ƃ߂�����
EXTERN unsigned char g_DownInputBuff;        //�L�[�n�m���Ƀt���O������
EXTERN unsigned char g_UpInputBuff;          //�L�[�n�e�e���Ƀt���O������
EXTERN unsigned char g_RapidBuff;            //�A��
EXTERN unsigned char g_HalfRapidBuff;        //�������A��
EXTERN BOOL          g_HalfRapidFlag;        //����/�������A�ː؂�ւ��t���O


EXTERN POINT         g_MousePos;             //�}�E�X���W
EXTERN unsigned char g_MouseButton;          //�}�E�X�{�^��



//���̑�
EXTERN int            g_Count;                //�`��t���[�����J�E���g


#endif
