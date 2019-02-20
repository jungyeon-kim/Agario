#include <Windows.h>
#include<time.h>
#include<math.h>
#include"resource.h"

#define WndX 800
#define WndY 750
#define MAX 5000

struct Agario {
	int x, y;
	int radius;
	int R, G, B;
};

Agario user[MAX];
Agario com[MAX];
Agario food[MAX];
Agario spitFood[MAX];
Agario trap[MAX];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = "ApiBase";

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB(150, 255, 150));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(
		lpszClass, // 윈도우 클래스 이름
		"정연쓰 Agario", // 윈도우 타이틀 이름
		WS_BORDER | WS_POPUP, // 윈도우 스타일
		0, // 윈도우 위치, x좌표 
		0, // 윈도우 위치, y좌표
		WndX, // 윈도우 가로(폭) 크기 
		WndY, // 윈도우 세로(높이) 크기
		NULL, // 부모 윈도우 핶들
		NULL, // 메뉴 핶들
		hInstance, // 응용 프로그램 인스턴스
		NULL);	// 생성된 윈도우 정보
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

char print_size[MAX] = { 0 }, print_time[MAX] = { 0 };
int WndXx = WndX / 16, WndYy = WndY / 16;
int mx, my, Rmx[MAX] = { 0 }, Rmy[MAX] = { 0 };
int foodCount = 0, comCount = 1, trapCount = 3, Rcount = 0, cellCount1 = 1, cellCount2 = 3;
int trapDir[MAX] = { 0 }, trapMove[MAX] = { 0 };
int cut = 1000; // 삭제
int Eat[MAX] = { 0 }; // 먹기 스위치
int comMove[MAX] = { 0 }; // 컴퓨터 이동 스위치
int cell1 = 0; // 분열생성 스위치
int cell2 = 0; // 분열삭제 스위치
int playTime = 0; // 플레이시간
int gameOver = 0; // 종료 스위치

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM
	wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HFONT hFont, saveFont;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	HDC memDC;
	HBITMAP hBitmap;
	COLORREF bk_color = RGB(200, 255, 200);
	COLORREF gameOver_bk_color = RGB(0, 0, 0), gameOver_text_color = RGB(255, 255, 255);


	srand(time(NULL));

	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hwnd, 1, 100, NULL);
		SetTimer(hwnd, 2, 3000, NULL);
		SetTimer(hwnd, 3, 30000, NULL);
		SetTimer(hwnd, 4, 1000, NULL);

		// 주인공 원 좌표 초기값
		user[0].x = 400;
		user[0].y = 400;
		user[0].radius = 10;

		// 컴퓨터 원 좌표 초기값
		for (int i = 0; i < MAX; i++)
		{
			com[i].x = rand() % WndX;
			com[i].y = rand() % WndY;
			com[i].radius = 20;
		}

		// 트랩 원 좌표 초기값
		for (int i = 0; i < MAX; i++)
		{
			trap[i].x = rand() % (WndX - 100);
			trap[i].y = rand() % (WndY - 300);
			trap[i].radius = 30;
		}

		// 먹이원 좌표, 컬러
		for (int i = 0; i < MAX; i++)
		{
			food[i].x = rand() % WndX;
			food[i].y = rand() % WndY;
			food[i].R = rand() % 256;
			food[i].G = rand() % 256;
			food[i].B = rand() % 256;
			com[i].R = rand() % 256;
			com[i].G = rand() % 256;
			com[i].B = rand() % 256;
		}
		break;

	case WM_CHAR:
		break;

	case WM_KEYDOWN:
		if (wParam == 'Q')
			PostQuitMessage(0);
		break;

	case WM_RBUTTONDOWN:
		// 뱉은 먹이가 이동할 좌표 저장
		for (int i = Rcount - 1; i < Rcount; i++)
		{
			spitFood[i + 1].x = user[0].x;
			spitFood[i + 1].y = user[0].y;
			if (user[0].radius > 10)
			{
				Rmx[i] = mx;
				Rmy[i] = my;
			}
		}

		//먹이 뱉기
		if (user[0].radius > 10)
		{
			user[0].radius--;
			Rcount++;
		}

		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
		if (user[0].radius >= 20)
		{
			cell1++;
			SetTimer(hwnd, 5, 2000, NULL);
		}
		if (cell1 == 2) cell1--;
		break;

	case WM_MOUSEMOVE:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MENU_START:
			SetTimer(hwnd, 1, 100, NULL);
			SetTimer(hwnd, 2, 3000, NULL);
			SetTimer(hwnd, 3, 30000, NULL);
			SetTimer(hwnd, 4, 1000, NULL);
			break;
		case ID_MENU_PAUSE:
			for (int i = 1; i <= 4; i++)
				KillTimer(hwnd, i);
			break;
		case ID_MENU_EXIT:
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			// 주인공원 이동
			user[0].x += (mx - user[0].x) / user[0].radius;
			user[0].y += (my - user[0].y) / user[0].radius;

			// 컴퓨터원 이동
			for (int i = 0; i < comCount; i++)
			{
				if (abs(com[i].x - user[0].x) < com[i].radius * 5
					&& abs(com[i].y - user[0].y) < com[i].radius * 5
					&& com[i].radius > user[0].radius)
					comMove[i] = 1;
				else
					comMove[i] = 0;
				// 먹이를 따라간다.
				if (com[i].x != cut && com[i].y != cut && comMove[i] == 0)
				{
					com[i].x += (food[i].x - com[i].x) / com[i].radius;
					com[i].y += (food[i].y - com[i].y) / com[i].radius;
				}
				// 주인공 원을 따라간다.
				if (com[i].x != cut && com[i].y != cut && comMove[i] == 1)
				{
					com[i].x += (user[0].x - com[i].x) / com[i].radius;
					com[i].y += (user[0].y - com[i].y) / com[i].radius;
				}
			}

			// 뱉은 먹이 이동
			for (int i = 0; i < Rcount; i++)
			{
				if (spitFood[i].x != NULL && spitFood[i].y != NULL)
				{
					spitFood[i].x += (Rmx[i - 1] - spitFood[i].x) / 5;
					spitFood[i].y += (Rmy[i - 1] - spitFood[i].y) / 5;
					if (spitFood[i].x < Rmx[i - 1] + 5
						&& spitFood[i].y < Rmy[i - 1] + 5
						&& spitFood[i].x > Rmx[i - 1] - 5
						&& spitFood[i].y > Rmy[i - 1] - 5) Eat[i] = 1;
				}
			}

			// 트랩원 이동
			for (int i = 0; i < trapCount; i++)
			{
				if (trap[i].x != cut && trap[i].y != cut)
				{
					trapMove[i] = rand() % 3 - 1;
					if (trapDir[i] == 0)
					{
						if (trapMove[i] != 0 && i % 2 == 0)
						{
							trap[i].x += trapMove[i] * 2;
							trap[i].y += trapMove[i] * 2;
						}
						else if (trapMove[i] != 0 && i % 2 == 1)
						{
							trap[i].x += trapMove[i] * 2;
							trap[i].y -= trapMove[i] * 2;
						}
						else
						{
							trap[i].x -= 4;
							trap[i].y -= 4;
						}
					}
					if (trapDir[i] == 1)
					{
						trap[i].x -= 5;
						trap[i].y += trapMove[i] * 2;
					}
					if (trapDir[i] == 2)
					{
						trap[i].x -= trapMove[i] * 2;
						trap[i].y -= 5;
					}
					if (trapDir[i] == 3)
					{
						trap[i].x += 5;
						trap[i].y -= trapMove[i] * 2;
					}
					if (trapDir[i] == 4)
					{
						trap[i].x += trapMove[i] * 2;
						trap[i].y += 5;
					}

					if (trap[i].x + 30 > WndX) trapDir[i] = 1;
					if (trap[i].y + 50 > WndY) trapDir[i] = 2;
					if (trap[i].x - 30 < 0) trapDir[i] = 3;
					if (trap[i].y - 30 < 0) trapDir[i] = 4;
				}
			}

			for (int i = 0; i < MAX; i++)
			{
				// 먹으면 주인공 원이 커지고 뱉은먹이 원이 사라진다.
				if (abs(user[0].x - spitFood[i].x) < user[0].radius
					&& abs(user[0].y - spitFood[i].y) < user[0].radius
					&& Eat[i] == 1)
				{
					user[0].radius++;
					spitFood[i].x = NULL;
					spitFood[i].y = NULL;
				}
			}
			for (int i = 0; i < MAX; i++)
				for (int j = 0; j < comCount; j++)
				{
					// 먹으면 컴퓨터 원이 커지고 뱉은먹이 원이 사라진다.
					if (abs(com[j].x - spitFood[i].x) < com[j].radius
						&& abs(com[j].y - spitFood[i].y) < com[j].radius)
					{
						if (com[j].radius < 100)
							com[j].radius += 2;
						spitFood[i].x = NULL;
						spitFood[i].y = NULL;
					}
				}
			for (int i = 0; i < 30; i++)
			{
				// 먹으면 주인공 원이 커지고 먹이 원이 사라진다.
				if (abs(user[0].x - food[i].x) < user[0].radius
					&& abs(user[0].y - food[i].y) < user[0].radius)
				{
					user[0].radius++;
					food[i].x = NULL;
					food[i].y = NULL;
				}
				// 먹으면 주인공 원이 커지고 컴퓨터 원이 사라진다.
				if (abs(user[0].x - com[i].x) < user[0].radius
					&& abs(user[0].y - com[i].y) < user[0].radius
					&& user[0].radius > com[i].radius)
				{
					user[0].radius++;
					com[i].x = cut;
					com[i].y = cut;
				}
			}
			for (int i = 0; i < 30; i++)
				for (int j = 0; j < comCount; j++)
				{
					// 먹으면 컴퓨터 원이 커지고 먹이 원이 사라진다.
					if (abs(com[j].x - food[i].x) < com[j].radius
						&& abs(com[j].y - food[i].y) < com[j].radius)
					{
						if (com[j].radius < 100)
							com[j].radius += 3;
						food[i].x = NULL;
						food[i].y = NULL;
					}
				}
			for (int i = 0; i < trapCount; i++)
			{
				// 주인공원과 닿으면 트랩원이 사라지고 분열한다.
				if (abs(user[0].x - trap[i].x) < user[0].radius
					&& abs(user[0].y - trap[i].y) < user[0].radius
					&& user[0].radius > trap[i].radius)
				{
					user[0].radius++;
					trap[i].x = cut;
					trap[i].y = cut;
					cell1++;
					SetTimer(hwnd, 5, 2000, NULL);
					if (cell1 == 2) cell1--;
				}
			}
			for (int i = 0; i < trapCount; i++)
				for (int j = 0; j < comCount; j++)
				{
					// 더 큰 컴퓨터원과 닿으면 트랩원이 사라진다.
					if (abs(com[j].x - trap[i].x) < com[j].radius
						&& abs(com[j].y - trap[i].y) < com[j].radius
						&& trap[i].radius < com[j].radius)
					{
						trap[i].x = cut;
						trap[i].y = cut;
					}
				}
			for (int i = 0; i < trapCount; i++)
				for (int j = 0; j < comCount; j++)
				{
					// 더 큰 트랩원과 닿으면 컴퓨터원이 사라진다.
					if (abs(trap[i].x - com[j].x) < trap[i].radius
						&& abs(trap[i].y - com[j].y) < trap[i].radius
						&& trap[i].radius > com[j].radius)
					{
						com[j].x = cut;
						com[j].y = cut;
					}
				}

			for (int i = 0; i < 30; i++)
				// 먹이원이 사라졌으면 다시 그린다.
				if (food[i].x == NULL && food[i].y == NULL)
				{
					food[i].x = rand() % WndX;
					food[i].y = rand() % WndY;
				}

			// 게임 오버
			for (int i = 0; i < comCount; i++)
			{
				if (abs(com[i].x - user[0].x) < com[i].radius
					&& abs(com[i].y - user[0].y) < com[i].radius
					&& user[0].radius <= com[i].radius)
				{
					gameOver = 1;
					KillTimer(hwnd, 1);
					KillTimer(hwnd, 4);
				}
			}
			break;
		case 2:
			// 일정 시간마다 먹이원을 삭제한다.
			for (int i = foodCount - 1; i < foodCount; i++)
			{
				food[i].x = NULL;
				food[i].y = NULL;
				if (foodCount == 30)
					foodCount = 0;
			}
			foodCount++;

			for (int i = 0; i < 30; i++)
				// 컴퓨터원이 사라졌으면 다시 그린다.
				if (com[i].x == cut && com[i].y == cut)
				{
					com[i].x = rand() % WndX;
					com[i].y = rand() % WndY;
					com[i].radius = 20;
				}
			break;
		case 3:
			comCount++;
			trapCount++;
			break;
		case 4:
			playTime++;
			break;
		case 5:
			cell2++;
			if (cell2 == 1)
			{
				cell1 = 0;
				cell2 = 0;
				KillTimer(hwnd, 5);
			}
			break;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		memDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, WndX, WndY);
		SelectObject(memDC, (HBITMAP)hBitmap);
		PatBlt(memDC, 0, 0, WndX, WndY, WHITENESS);

		// 배경색
		hBrush = CreateSolidBrush(RGB(200, 255, 200));
		oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
		Rectangle(memDC, -10, -10, WndX, WndY);
		SelectObject(memDC, oldBrush);
		DeleteObject(hBrush);

		// 바둑판
		for (int i = 0; i < 16; i++)
		{
			hPen = CreatePen(PS_SOLID, 2, RGB(255, 200, 200));
			oldPen = (HPEN)SelectObject(memDC, hPen);
			MoveToEx(memDC, 0, WndYy*i, NULL);
			LineTo(memDC, WndXx * 16, WndYy*i);
			MoveToEx(memDC, WndXx*i, 0, NULL);
			LineTo(memDC, WndXx * i, WndYy * 16);
			SelectObject(memDC, oldPen);
			DeleteObject(hPen);
		}

		//먹이 원
		for (int i = 0; i < 30; i++)
		{
			hBrush = CreateSolidBrush(RGB(food[i].R, food[i].G, food[i].B));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, food[i].x - 5, food[i].y - 5, food[i].x + 5, food[i].y + 5);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);
		}
		for (int i = 0; i < Rcount; i++)
		{
			if (user[0].radius >= 10)
			{
				hBrush = CreateSolidBrush(RGB(food[i].R, food[i].G, food[i].B));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, spitFood[i].x - 5, spitFood[i].y - 5, spitFood[i].x + 5, spitFood[i].y + 5);
				SelectObject(memDC, oldBrush);
				DeleteObject(hBrush);
			}
		}

		// 컴퓨터 원
		for (int i = 0; i < comCount; i++)
		{
			hPen = CreatePen(PS_SOLID, 1, RGB(com[i].R, com[i].G, com[i].B));
			oldPen = (HPEN)SelectObject(memDC, hPen);
			hBrush = CreateSolidBrush(RGB(com[i].R, com[i].G, com[i].B));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, com[i].x - com[i].radius, com[i].y - com[i].radius, com[i].x + com[i].radius, com[i].y + com[i].radius);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);
			SelectObject(memDC, oldPen);
			DeleteObject(hPen);
		}

		// 주인공 원
		if (cell1 == 0)
		{
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 150, 150));
			oldPen = (HPEN)SelectObject(memDC, hPen);
			hBrush = CreateSolidBrush(RGB(255, 150, 150));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, user[0].x - user[0].radius, user[0].y - user[0].radius, user[0].x + user[0].radius, user[0].y + user[0].radius);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);
			SelectObject(memDC, oldPen);
			DeleteObject(hPen);
		}
		if (cell1 == 1)
		{
			for (int i = cellCount1; i < cellCount2; i++)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(255, 150, 150));
				oldPen = (HPEN)SelectObject(memDC, hPen);
				hBrush = CreateSolidBrush(RGB(255, 150, 150));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, (user[0].x+i*user[0].radius) - user[0].radius/2, (user[0].y+i*user[0].radius) - user[0].radius/2, (user[0].x+i*user[0].radius) + user[0].radius/2, (user[0].y+i*user[0].radius) + user[0].radius/2);
				SelectObject(memDC, oldBrush);
				DeleteObject(hBrush);
				SelectObject(memDC, oldPen);
				DeleteObject(hPen);
			}
		}

		// 트랩 원
		for (int i = 0; i < trapCount; i++)
		{
			hBrush = CreateSolidBrush(RGB(150, 150, 255));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, trap[i].x - trap[i].radius, trap[i].y - trap[i].radius, trap[i].x + trap[i].radius, trap[i].y + trap[i].radius);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);
			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, trap[i].x - trap[i].radius / 2, trap[i].y - trap[i].radius / 2, trap[i].x + trap[i].radius / 2, trap[i].y + trap[i].radius / 2);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);
		}

		// 주인공원 사이즈 출력
		wsprintf(print_size, ("Size: %d"), user[0].radius);
		SetBkColor(memDC, bk_color);
		TextOut(memDC, 0, 1, print_size, strlen(print_size));

		// 플레이시간 출력
		wsprintf(print_time, ("Play Time: %d sec"), playTime);
		SetBkColor(memDC, bk_color);
		TextOut(memDC, 0, 17, print_time, strlen(print_time));

		// 게임 오버
		if (gameOver == 1)
		{
			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Rectangle(memDC, -10, -10, WndX, WndY);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);

			SetBkColor(memDC, gameOver_bk_color);
			SetTextColor(memDC, gameOver_text_color);
			wsprintf(print_size, ("Size: %d"), user[0].radius);
			TextOut(memDC, 370, 377, print_size, strlen(print_size));
			wsprintf(print_time, ("Play Time: %d sec"), playTime);
			TextOut(memDC, 342, 394, print_time, strlen(print_time));
			saveFont = CreateFont(100, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("궁서"));
			saveFont = (HFONT)SelectObject(memDC, saveFont);
			TextOut(memDC, 80, 250, "GAME OVER!", 10);
			DeleteObject(saveFont);
		}

		BitBlt(hdc, 0, 0, WndX, WndY, memDC, 0, 0, SRCCOPY);
		DeleteDC(memDC);
		DeleteObject(hBitmap);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMessage, wParam, lParam));
}

// 추가구현)
// 컴퓨터원 초기 크기가 주인공원과 같으면 난이도가 낮아서 더크게 설정함.
// 컴퓨터원이 일정크기가 되면 먹이를 먹지 못하도록 함.
// 분열은 크기 20이상 부터 가능함.
