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
		lpszClass, // ������ Ŭ���� �̸�
		"������ Agario", // ������ Ÿ��Ʋ �̸�
		WS_BORDER | WS_POPUP, // ������ ��Ÿ��
		0, // ������ ��ġ, x��ǥ 
		0, // ������ ��ġ, y��ǥ
		WndX, // ������ ����(��) ũ�� 
		WndY, // ������ ����(����) ũ��
		NULL, // �θ� ������ �L��
		NULL, // �޴� �L��
		hInstance, // ���� ���α׷� �ν��Ͻ�
		NULL);	// ������ ������ ����
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
int cut = 1000; // ����
int Eat[MAX] = { 0 }; // �Ա� ����ġ
int comMove[MAX] = { 0 }; // ��ǻ�� �̵� ����ġ
int cell1 = 0; // �п����� ����ġ
int cell2 = 0; // �п����� ����ġ
int playTime = 0; // �÷��̽ð�
int gameOver = 0; // ���� ����ġ

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

		// ���ΰ� �� ��ǥ �ʱⰪ
		user[0].x = 400;
		user[0].y = 400;
		user[0].radius = 10;

		// ��ǻ�� �� ��ǥ �ʱⰪ
		for (int i = 0; i < MAX; i++)
		{
			com[i].x = rand() % WndX;
			com[i].y = rand() % WndY;
			com[i].radius = 20;
		}

		// Ʈ�� �� ��ǥ �ʱⰪ
		for (int i = 0; i < MAX; i++)
		{
			trap[i].x = rand() % (WndX - 100);
			trap[i].y = rand() % (WndY - 300);
			trap[i].radius = 30;
		}

		// ���̿� ��ǥ, �÷�
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
		// ���� ���̰� �̵��� ��ǥ ����
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

		//���� ���
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
			// ���ΰ��� �̵�
			user[0].x += (mx - user[0].x) / user[0].radius;
			user[0].y += (my - user[0].y) / user[0].radius;

			// ��ǻ�Ϳ� �̵�
			for (int i = 0; i < comCount; i++)
			{
				if (abs(com[i].x - user[0].x) < com[i].radius * 5
					&& abs(com[i].y - user[0].y) < com[i].radius * 5
					&& com[i].radius > user[0].radius)
					comMove[i] = 1;
				else
					comMove[i] = 0;
				// ���̸� ���󰣴�.
				if (com[i].x != cut && com[i].y != cut && comMove[i] == 0)
				{
					com[i].x += (food[i].x - com[i].x) / com[i].radius;
					com[i].y += (food[i].y - com[i].y) / com[i].radius;
				}
				// ���ΰ� ���� ���󰣴�.
				if (com[i].x != cut && com[i].y != cut && comMove[i] == 1)
				{
					com[i].x += (user[0].x - com[i].x) / com[i].radius;
					com[i].y += (user[0].y - com[i].y) / com[i].radius;
				}
			}

			// ���� ���� �̵�
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

			// Ʈ���� �̵�
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
				// ������ ���ΰ� ���� Ŀ���� �������� ���� �������.
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
					// ������ ��ǻ�� ���� Ŀ���� �������� ���� �������.
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
				// ������ ���ΰ� ���� Ŀ���� ���� ���� �������.
				if (abs(user[0].x - food[i].x) < user[0].radius
					&& abs(user[0].y - food[i].y) < user[0].radius)
				{
					user[0].radius++;
					food[i].x = NULL;
					food[i].y = NULL;
				}
				// ������ ���ΰ� ���� Ŀ���� ��ǻ�� ���� �������.
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
					// ������ ��ǻ�� ���� Ŀ���� ���� ���� �������.
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
				// ���ΰ����� ������ Ʈ������ ������� �п��Ѵ�.
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
					// �� ū ��ǻ�Ϳ��� ������ Ʈ������ �������.
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
					// �� ū Ʈ������ ������ ��ǻ�Ϳ��� �������.
					if (abs(trap[i].x - com[j].x) < trap[i].radius
						&& abs(trap[i].y - com[j].y) < trap[i].radius
						&& trap[i].radius > com[j].radius)
					{
						com[j].x = cut;
						com[j].y = cut;
					}
				}

			for (int i = 0; i < 30; i++)
				// ���̿��� ��������� �ٽ� �׸���.
				if (food[i].x == NULL && food[i].y == NULL)
				{
					food[i].x = rand() % WndX;
					food[i].y = rand() % WndY;
				}

			// ���� ����
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
			// ���� �ð����� ���̿��� �����Ѵ�.
			for (int i = foodCount - 1; i < foodCount; i++)
			{
				food[i].x = NULL;
				food[i].y = NULL;
				if (foodCount == 30)
					foodCount = 0;
			}
			foodCount++;

			for (int i = 0; i < 30; i++)
				// ��ǻ�Ϳ��� ��������� �ٽ� �׸���.
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

		// ����
		hBrush = CreateSolidBrush(RGB(200, 255, 200));
		oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
		Rectangle(memDC, -10, -10, WndX, WndY);
		SelectObject(memDC, oldBrush);
		DeleteObject(hBrush);

		// �ٵ���
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

		//���� ��
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

		// ��ǻ�� ��
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

		// ���ΰ� ��
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

		// Ʈ�� ��
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

		// ���ΰ��� ������ ���
		wsprintf(print_size, ("Size: %d"), user[0].radius);
		SetBkColor(memDC, bk_color);
		TextOut(memDC, 0, 1, print_size, strlen(print_size));

		// �÷��̽ð� ���
		wsprintf(print_time, ("Play Time: %d sec"), playTime);
		SetBkColor(memDC, bk_color);
		TextOut(memDC, 0, 17, print_time, strlen(print_time));

		// ���� ����
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
			saveFont = CreateFont(100, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("�ü�"));
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

// �߰�����)
// ��ǻ�Ϳ� �ʱ� ũ�Ⱑ ���ΰ����� ������ ���̵��� ���Ƽ� ��ũ�� ������.
// ��ǻ�Ϳ��� ����ũ�Ⱑ �Ǹ� ���̸� ���� ���ϵ��� ��.
// �п��� ũ�� 20�̻� ���� ������.
