#include <Windows.h>
#include <iostream>
#include "resource.h"

HBITMAP hBmpAir, hBmpBack, hBmpBullet, hBmpEnemy, hBmpExplosion;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lparam);
	// Global Variable
void game_init(HWND hWnd, bool all_init);
static int nX = 0, nY = 0; //airplane
static int nX_back = 0; //background
static int nXb[10], nYb[10]; //bullet
static int nXe, nYe; //enemy 포지션
static bool bullet[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static bool hit[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static bool enemy = false;
static int max_no_of_bullet = 9, bn = 0;
int n;
static int number_of_fire = 0, number_of_hit = 0;
char szText[100];
static bool game_start = false;
static bool game_multi = false;


	// Win main
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPreinstance, LPSTR IpCmdLine, int nShowCmd)
{
	HWND hWnd;
	MSG mSg;
	char szTitle[] = "Win32 Airplane Game 201901263 Dahe Yang";
	char szClass[] = "Class";

	//Declare window class
	WNDCLASSEX WndEx;

	WndEx.cbSize = sizeof(WndEx);
	WndEx.style = NULL;
	WndEx.lpfnWndProc = WndProc; //함수명자체=함수의 메모리주소 (포인터), WndProc호출하고자함
	WndEx.cbClsExtra = 0;
	WndEx.cbWndExtra = 0;
	WndEx.hInstance = hinstance; //Winmain매개변수에 instance있던걸 그대로셋업
	WndEx.hIcon = LoadIcon(NULL, "");
	WndEx.hCursor = LoadCursorA(NULL, IDC_ARROW);
	WndEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndEx.lpszMenuName = "IDR_MENU"; //lp~ 포인트변수
	WndEx.lpszClassName = szClass; //나중에 레지스터클래스함수를 이용해 해당되는 구조체를 os에 등록, 그이름이szclass
	WndEx.hIconSm = LoadIcon(hinstance, "");

	//Register window Class 위에 셋업된내용을 os에 등록
	RegisterClassExA(&WndEx); //&:포인트변수
	//구조체를 그냥 등록(매개변수하나씩 넘기지않음)

	// Create window 생성문
	// CreateWindowExA에서 구조체에정의된걸로 윈도우 생성, 그 아이디를 리턴해줌 핸들
	hWnd = CreateWindowExA(NULL, szClass, szTitle,
		WS_OVERLAPPEDWINDOW, // 윈도우 스타일
		CW_USEDEFAULT, CW_USEDEFAULT, //운영체제가 지정한 위치대로 출력해라
		320 * 2, 240 * 2, NULL, NULL, hinstance, NULL);

	//Show window 메모리에 생성된 윈도우를 출력
	ShowWindow(hWnd, nShowCmd); // 해당 윈도우를 화면에 출력 //createWindow에서 얻은 핸들로 보여주고 갱신

	hBmpAir = LoadBitmap(hinstance, "IDB_AIR");
	hBmpBack = LoadBitmap(hinstance, "IDB_BACK");
	hBmpBullet = LoadBitmap(hinstance, "IDB_BULLET");
	hBmpEnemy = LoadBitmap(hinstance, "IDB_ENEMY");
	hBmpExplosion = LoadBitmap(hinstance, "IDB_EXPLOSION");

	InvalidateRect(hWnd, NULL, TRUE); // 클라이언트 영역에서 원하는 사각형 영역을 갱신하도록 지정함 WM_PAINT 메시지 발생시키는 함수
	// hWnd: 클라이언트 영역을 갱신할 윈도우의 핸들 값 
	// NULL: 갱신영역을 지정 안하면 클라이언트 전체 영역이 적용됨
	// TRUE:해당 영역의 배경까지 갱신할건지(윈도우 배경색 다시그려짐)
	UpdateWindow(hWnd); // 갱신이 지연없이 WM_PAINT 수행되게 하고싶다면 (갱신 영역이 있으면 시스템상태 신경안쓰고 즉각갱신하게함)


	//Message Loop
	while (TRUE) 
	{
		if (PeekMessage(&mSg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&mSg, NULL, 0, 0)) //겟메시지:메시지 큐를 메시지를가져오는 역할
				break; //겟메시지아니면=프로그램 종료되면? break
			TranslateMessage(&mSg); //문자키 키입력에 대한 메시지를 변환
			DispatchMessageA(&mSg); //WndProc()을 호출, 메시지 발생햇을때 그걸 윈프록에전달
		}
	}

	return mSg.wParam;
}
	// Game_init procdure
void game_init(HWND hWnd, bool all_init) // window생성후 불러와짐 게임에셋 불러오는곳 // 초기화
	{
		for (n = 0; n <= max_no_of_bullet; n++) {
			KillTimer(hWnd, n + 1); //타이머 삭제
			bullet[n] = false;
			hit[n] = 0;
		}
		bn = 0;
		number_of_fire = 0; number_of_hit = 0;
		if (all_init) {
			KillTimer(hWnd, max_no_of_bullet + 2);
			game_start = false;
			enemy = false;
		}
	}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // Call-back procedure
{
	HDC hDC, hMemDC;
	PAINTSTRUCT pS;

	switch (uMsg) //
	{				// Menu command, Message handling
	case WM_COMMAND:
		switch (LOWORD(wParam)) { //아래쪽 4바이트는 wparam으로 동작
		case ID_CONTROL_START: //메세지루프에서 wparam아랮쪽2바이트에 4004를저장해서 WndProc을부름
			game_init(hWnd, true);
			game_start = TRUE;
			SetTimer(hWnd, max_no_of_bullet + 2, 2000, NULL); //핸들, 타이머 번호, 주기ms, 콜주기마다 실행할함수 = NULL이면 WM_TIMER에서 설정한 함수호출
			enemy = false;
			break;
		case ID_CONTROL_STOP:
			game_init(hWnd, true);
			break;
		case ID_BULLET_SINGLE: //메뉴
			game_init(hWnd, false);
			game_multi = false;
			break;
		case ID_BULLET_MULTI:
			game_init(hWnd, false);
			game_multi = true;
			break;
		case ID_CONTROL_EXIT:
			PostQuitMessage(0);
			break;
		case ID_HELP_GUIDE:
			MessageBox(hWnd, "<Enter> Start Enemy; <Space> Bullet Fire", "User's Guide", NULL);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE;

	case WM_KEYDOWN: //좌우상하키 Keydown message handling
		switch (LOWORD(wParam)) {
		case VK_UP:
			nY -= 5;
			break;
		case VK_DOWN:
			nY += 5;
			break;
		case VK_LEFT:
			nX -= 5;
			nX_back += 5; //배경은 오른쪽으로 이동
			break;
		case VK_RIGHT:
			nX += 5;
			nX_back -= 5; //배경은 왼쪽으로 이동
			break;
		case VK_RETURN: //start the game
			game_start = TRUE;
			SetTimer(hWnd, max_no_of_bullet + 2, 2000, NULL);
			enemy = true;
			break;

		case VK_SPACE: //bullet

			if (game_start) {
				number_of_fire++;

				nXb[bn] = nX + 127; //end of the airplane
				nYb[bn] = nY;
				if (!bullet[bn]) {
					SetTimer(hWnd, bn + 1, 5, NULL); //5밀리초
					bullet[bn] = true;
				}
				if (game_multi) bn++;
				if (bn > max_no_of_bullet) bn = 0;
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE;
	
		// Timer message handling
	case WM_TIMER:
		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);

		if (wParam == max_no_of_bullet + 2) //timer for enemy +2인이유:
			//enemy디스플레이> enemy는 return키에서 나타남, 타이머id 맞추려고
		{
			nXe = (rand() % 308) + 300;
			nYe = (rand() % 230);
		}
		for (n = 0; n <= max_no_of_bullet; n++) { //불렛 수만큼 타이머체크 10개불렛 쏨 = 타이머 10개
			//타이머가 만들어질때(스페이스누름) bn+1 타이머계속생성 누르고누를때 타이머번호 처음엔 1, 2, 3.... 계속생성 bn+1타이머생성하는변수
			if (wParam == n + 1) { //timer for bullet 타이머번호가 1부터시작하니까 n+1해야 0부터시작말고 1부터시작인거됨
				nXb[n] += 5; //bullet 위치 오른쪽으로 +5
				if (nXb[n] > 640) { //화면밖으로 넘어가면
					KillTimer(hWnd, n + 1);
					bullet[n] = false;
				}
			}

			if (bullet[n]) {
				if (nXb[n] >= nXe - 15 && nXb[n] <= nXe + 15 && nYb[n] > nYe - 15 && nYb[n] <= nYe + 15) //bullet위치가 enemy주변
				{
					bullet[n] = false;
					//enemy = false;
					KillTimer(hWnd, n + 1);
					//KillTimer(hWnd, max_no_of_bullet + 2);
					hit[n] = TRUE;
					number_of_hit++;
				}
			}
		}
		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hMemDC);

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE;

		// Paint message handling

	case WM_PAINT: //display the image
		hDC = BeginPaint(hWnd, &pS); //화면에 관련된 핸들 리턴 
		hMemDC = CreateCompatibleDC(hDC);

		if (nX > 640) nX = 0;
		else if (nX < 0) nX = 640;
		if (nY > 480) nY = 0;
		else if (nY < 0) nY = 480;
		if (nX_back > 640) nX_back = 0;
		else if (nX_back < 0) nX_back = 640;



		if (enemy) { //= if enemy = true, 위에서 return key 눌러서 enemy가 true되었을 때
			SelectObject(hMemDC, hBmpEnemy);
			BitBlt(hDC, nXe, nYe, 32, 32, hMemDC, 0, 0, SRCCOPY);
		}

		for (n = 0; n <= max_no_of_bullet; n++) {
			if (bullet[n]) { 
				SelectObject(hMemDC, hBmpBullet);
				BitBlt(hDC, nXb[n], nYb[n], 32, 30, hMemDC, 0, 0, SRCCOPY);
			}

			if (hit[n]) { 
				SelectObject(hMemDC, hBmpExplosion);
				BitBlt(hDC, nXe, nYe + 2, 32, 28, hMemDC, 0, 0, SRCCOPY);
				hit[n] = 0;
				Sleep(100);
			}
		}

		SelectObject(hMemDC, hBmpAir); //show window에 hBmpAir
		BitBlt(hDC, nX, nY, 127, 37, hMemDC, 0, 0, SRCCOPY); 

		SelectObject(hMemDC, hBmpBack); //show window에 hBmpBack
		BitBlt(hDC, nX_back, 281, 640, 159, hMemDC, 0, 0, SRCCOPY);
		BitBlt(hDC, nX_back - 640, 281, 640, 159, hMemDC, 0, 0, SRCCOPY); // x, y, size of , ,

		if (game_start) {
			sprintf_s(szText,  "No of Fire: %d, No of hit: %d", number_of_fire, number_of_hit);
			TextOut(hDC, 300, 10, szText, lstrlen(szText));
		}

		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hMemDC);
		EndPaint(hWnd, &pS);
	
		return FALSE; 

	case WM_DESTROY: //Exit program 윈도우가 화면에서 사라진후에 보내지는 메시지
		PostQuitMessage(0);
		return FALSE;
	}
	return DefWindowProcA(hWnd, uMsg, wParam, lParam);

}
