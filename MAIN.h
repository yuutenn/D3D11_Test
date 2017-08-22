//ヘッダファイルのインクルード
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>

//ライブラリファイルのロード
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")

//警告非表示
#pragma warning(disable:4305)

//定数定義
#define WINDOW_WIDTH 640//ウィンドウ幅
#define WINDOW_HEIGHT 480//ウィンドウ高さ
#define APP_NAME L"最小ポリゴンシェーダーd3d11"

//解放マクロ
#define SAFE_RELEASE(x) if(x){x->Release(); x=0;}

//頂点の構造体(頂点レイアウト)
struct  SIMPLE_VERTEX{
	D3DXVECTOR3 Pos;
};

//コンスタントバッファーのアプリ側構造体(シェーダー内のコンスタントバッファーと一致している必要あり)
struct SIMPLESHADER_CONSTANT_BUFFER {
	D3DXMATRIX mWVP;
};


//MAINクラス定義
class  MAIN
{
public:
	//メソッド
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

	//データ
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
