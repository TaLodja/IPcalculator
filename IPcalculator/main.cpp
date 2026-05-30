#include <Windows.h>
#include <CommCtrl.h>
#include <cmath>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT FindZeroPosition(CHAR sz_buffer[], INT ZEROposition);
INT BitsToByte(INT n);
//INT FindByte();

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR IpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, NULL);

	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hwnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKEWORD(32, 0));
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		HWND hSpinPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		DWORD dwPrefix = 0;
		CHAR sz_buffer_mask[33] = {};
		CHAR sz_position[3] = {};
		INT minPrefix = 8;
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				DWORD dwFirst = FIRST_IPADDRESS(dwIPaddress);
				LPARAM lpMask = 0;
				if (dwFirst < 128) { lpMask = MAKEIPADDRESS(255, 0, 0, 0); minPrefix = 8; }
				else if (dwFirst < 192) { lpMask = MAKEIPADDRESS(255, 255, 0, 0); minPrefix = 16; }
				else if (dwFirst < 224) { lpMask = MAKEIPADDRESS(255, 255, 255, 0); minPrefix = 24; }
				SendMessage(hIPmask, IPM_SETADDRESS, 0, lpMask);
			}
			break;
		case IDC_IPMASK:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				_itoa_s(int(dwIPmask), sz_buffer_mask, 2);
				INT ZEROposition = FindZeroPosition(sz_buffer_mask, 0);
				_itoa_s(ZEROposition, sz_position, 10);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_position);
			}
			break;
		case IDC_EDIT_PREFIX:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				LPARAM lpMask = 0;
				SendMessage(hEditPrefix, WM_GETTEXT, 0, (LPARAM)&dwPrefix);
				INT n = dwPrefix - 8;
				if (dwPrefix > 8) 
				{
					lpMask = MAKEIPADDRESS(255, BitsToByte(n), 0, 0);
					//LPARAM lpMask = 0;
					//if (dwPrefix > 8 && dwPrefix < 16)
					//{
					//	INT byte = 0;
					//	for (INT i = 0; i < n; i++)
					//	{
					//		byte += (1 << (7 - n));
					//	}
					CHAR sz_buffer[4] = {};
					_itoa_s(BitsToByte(n), sz_buffer, 10);
					SendMessage(GetDlgItem(hwnd, IDC_GROUP_INFO), WM_SETTEXT, 0, (LPARAM)sz_buffer);
					SendMessage(hIPmask, IPM_SETADDRESS, 0, lpMask);
				}
				//}
			}
			break;
		case IDOK:
			break;
		case IDCANCEL:EndDialog(hwnd, 0);
		}
	}
	break;
	case WM_CLOSE: EndDialog(hwnd, 0);
	}
	return FALSE;
}

INT FindZeroPosition(CHAR sz_buffer[], INT ZEROposition)
{
	for (INT i = 0; i < 32; i++)
		if (sz_buffer[i] == '0')
		{
			ZEROposition = i;
			break;
		}
		else ZEROposition = 32;
	return ZEROposition;
}
INT BitsToByte(INT n)
{
	INT byte = 0;
	for (INT i = 0; i < n; i++)
		byte = (1 << (7 - n));
	return byte;
}