//�w�b�_�t�@�C���̃C���N���[�h
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>

//���C�u�����t�@�C���̃��[�h
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")

//�x����\��
#pragma warning(disable:4305)

//�萔��`
#define WINDOW_WIDTH 640//�E�B���h�E��
#define WINDOW_HEIGHT 480//�E�B���h�E����
#define APP_NAME L"�ŏ��|���S���V�F�[�_�[d3d11"

//����}�N��
#define SAFE_RELEASE(x) if(x){x->Release(); x=0;}

//���_�̍\����(���_���C�A�E�g)
struct  SIMPLE_VERTEX{
	D3DXVECTOR3 Pos;
};

//�R���X�^���g�o�b�t�@�[�̃A�v�����\����(�V�F�[�_�[���̃R���X�^���g�o�b�t�@�[�ƈ�v���Ă���K�v����)
struct SIMPLESHADER_CONSTANT_BUFFER {
	D3DXMATRIX mWVP;
};


//MAIN�N���X��`
class  MAIN
{
public:
	//���\�b�h
	 MAIN();
	~ MAIN();
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPCWSTR);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT InitD3D();
	HRESULT InitPolygon();
	HRESULT InitShader();
	void Loop();
	void App();
	void Render();
	void DestroyD3D();

	//�f�[�^
	HWND m_hWnd;
	ID3D10Device* m_pDevice;
	IDXGISwapChain* m_pSwapChain;
	ID3D10RenderTargetView* m_pRenderTargetView;
	ID3D10DepthStencilView* m_pDepthStencilView;
	ID3D10Texture2D* m_pDepthStencil;
	ID3D10RasterizerState* m_pRasterrizerState;
	ID3D10InputLayout* m_pVertexLayout;
	ID3D10VertexShader* m_pVertexShaer;
	ID3D10PixelShader* m_pPixelShader;
	ID3D10Buffer* m_pConstantBuffer;
	ID3D10Buffer* m_pVertexBuffer;
};
