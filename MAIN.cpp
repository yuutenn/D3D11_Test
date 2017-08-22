#include "MAIN.h"

//グローバル変数
MAIN* g_pMain = NULL;

//関数プロトタイプの宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//アプリケーションのエントリー関数
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT) {
	g_pMain = new MAIN;
	if (g_pMain != NULL) {
		if (SUCCEEDED(g_pMain->InitWindow(hInstance, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME))) {
			if (SUCCEEDED(g_pMain->InitD3D())) {
				g_pMain->Loop();
			}
		}
		//アプリ終了
		delete g_pMain;
	}
	return 0;
}


//OSから見たウィンドウプロシージャー
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return g_pMain->MsgProc(hWnd, uMsg, wParam, lParam);
}


//コンストラクタ
MAIN::MAIN() {
	ZeroMemory(this, sizeof(MAIN));
}
//デストラクタ
MAIN::~MAIN() {
	DestroyD3D();
}


///////////////ウィンドウ作成////////////////////

HRESULT MAIN::InitWindow(HINSTANCE hInstance, INT iX, INT iY, INT iWidth, INT iHeight, LPCWSTR WindowName) {
	
	//ウィンドウの定義
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);

	//ウィンドウの作成
	m_hWnd = CreateWindow(
		WindowName,
		WindowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		iWidth,
		iHeight,
		0,
		0,
		hInstance,
		0
	);
	if (!m_hWnd) {
		return E_FAIL;
	}

	//ウィンドウの表示
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++//


////////////ウィンドウプロシージャー/////////////

LRESULT MAIN::MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	
	switch (iMsg) {

	case WM_KEYDOWN:
		switch ((char)wParam){

		case VK_ESCAPE://ESCキーで終了
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

//++++++++++++++++++++++++++++++++++++++++++//


/////////////メッセージループ//////////////////

void MAIN::Loop() {
	//メッセージループ
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//アプリケーションの処理はここから飛ぶ
			App();
		}
	}
	//アプリケーションの終了
}

//++++++++++++++++++++++++++++++++++++++++++//


////////////アプリケーションの処理/////////////

void MAIN::App() {
	Render();
}

//+++++++++++++++++++++++++++++++++++++++++//


/////////////Direct3Dの初期化////////////////

HRESULT MAIN::InitD3D() {
	//デバイスとスワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		0, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice))) {
		return FALSE;
	}

	//レンダーターゲットビューの作成
	ID3D10Texture2D *pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	//深度ステンシルビューの作成
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
	m_pDevice->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDepthStencilView);

	//レンダーターゲットビューと深度ステンシルビューをパイプラインにバインド
	m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//ビューポートの設定
	D3D10_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDevice->RSSetViewports(1, &vp);

	//ラスタライズ設定
	D3D10_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D10_CULL_NONE;
	rdc.FillMode = D3D10_FILL_SOLID;
	rdc.FrontCounterClockwise = TRUE;

	m_pDevice->CreateRasterizerState(&rdc, &m_pRasterrizerState);
	m_pDevice->RSSetState(m_pRasterrizerState);

	//シェーダー初期化
	if (FAILED(InitShader())) {
		return E_FAIL;
	}

	//ポリゴン作成
	if (FAILED(InitShader())) {
		return E_FAIL;
	}

	return S_OK;
}

//+++++++++++++++++++++++++++++++++++++++++//



/////シェーダーを作成　頂点レイアウトを定義/////

HRESULT MAIN::InitShader() {
	//hlslファイルで読み込み　ブロブ作成
	ID3D10Blob *pCompiledShader = NULL;
	ID3D10Blob *pErrors = NULL;

	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX10CompileFromFile(L"Simple.hlsl", NULL, NULL, "VS", "vs_4_0", 0, 0, NULL,
		&pCompiledShader, &pErrors, NULL))) {
		MessageBox(0, L"hlsl 読み込み失敗", NULL, MB_OK);
		//MessageBoxA(0, (char*)pErrors->GetBufferPointer(), NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		&m_pVertexShaer))) {
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"バーテックスシェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義
	D3D10_INPUT_ELEMENT_DESC layout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D10_INPUT_PER_VERTEX_DATA,
			0
		},
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(m_pDevice->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(), &m_pVertexLayout))) {
		return FALSE;
	}

	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX10CompileFromFile(L"Simple.hlsl", NULL, NULL, "PS", "ps_4_0",
		0, 0, NULL, &pCompiledShader, &pErrors, NULL))) {
		MessageBox(0, L"hlsl 読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(), &m_pPixelShader))) {
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"ピクセルシェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//コンスタントバッファー作成　ここでは変換行列渡し用
	D3D10_BUFFER_DESC cb;
	cb.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SIMPLESHADER_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D10_USAGE_DYNAMIC;

	if (FAILED(m_pDevice->CreateBuffer(&cb, NULL, &m_pConstantBuffer))) {
		return E_FAIL;
	}
	return S_OK;
}

//+++++++++++++++++++++++++++++++++++++++++//



//ポリゴン定義しそのバーテックスバッファーを作成//

HRESULT MAIN::InitPolygon() {
	//バーテックスバッファー作成
	SIMPLE_VERTEX vertices[] = {
		D3DXVECTOR3(0.0f,0.5f,0.0f),
		D3DXVECTOR3(0.5f,-0.5f,0.0f),
		D3DXVECTOR3(-0.5f,-0.5f,0.0f),
	};

	D3D10_BUFFER_DESC bd;
	bd.Usage - D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SIMPLE_VERTEX);
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer))) {
		return E_FAIL;
	}
	return S_OK;
}

//++++++++++++++++++++++++++++++++++++++++++//



///////////シーンを画面にレンダリング//////////

void MAIN::Render() {
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	//画面クリア
	float ClearColor[4] = { 0,0,1,1 }; //クリア色作成 RGBAの順
	m_pDevice->ClearRenderTargetView(m_pRenderTargetView, ClearColor);//画面クリア
	//深度バッファクリア
	m_pDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	//使用するシェーダーのセット
	m_pDevice->VSSetShader(m_pVertexShaer);
	m_pDevice->PSSetShader(m_pPixelShader);

	//シェーダーのコンスタントバッファーに各種データをセット
	SIMPLESHADER_CONSTANT_BUFFER* pcb;
	if (SUCCEEDED(m_pConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL,
		(void**)&pcb))) {
		//ワールド、カメラ、射影行列をセット
		D3DXMATRIX m = mWorld*mView*mProj;
		D3DXMatrixTranspose(&m, &m);
		pcb->mWVP = m;

		//ワールドトランスフォーム(絶対座標変換)
		D3DXMatrixRotationY(&mWorld, timeGetTime() / 100.0f);//Y軸回転

		//ビュートランスフォーム(視点座標変換)
		D3DXVECTOR3 vEyePt(0.0f, 1.0f, -2.0f);//カメラ位置
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);//注視位置
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);//上方位置
		D3DXMatrixLookAtLH(&mView, &vEyePt, &vLookatPt, &vUpVec);

		//プロジェクショントランスフォーム(射影変換)
		D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4,
			(FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 100.0f);

		m_pConstantBuffer->Unmap();
	}

	//このコンスタントバッファーを使うシェーダーにバッファーをセット
	m_pDevice->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDevice->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//プリミティブ・トポロジーをセット
	m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点インプットレイアウトをセット
	m_pDevice->IASetInputLayout(m_pVertexLayout);

	//バーテックスバッファーをセット
	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
	m_pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//プリミティブをレンダリング
	m_pDevice->Draw(3, 0);

	//画面更新(バックバッファをフロントバッファに)
	m_pSwapChain->Present(0, 0);
}

//+++++++++++++++++++++++++++++++++++++++++//


///////////Direct3Dオブジェクトの破棄/////////

void MAIN::DestroyD3D() {
	SAFE_RELEASE(m_pRasterrizerState);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShaer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencil);
}

//+++++++++++++++++++++++++++++++++++++++++//
