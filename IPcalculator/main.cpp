#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <CommCtrl.h>
#include <cstdio>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
CHAR* FormatAddress(CHAR szBuffer[], CONST CHAR szMessage[], DWORD dwIPaddress);
CHAR* FormatCount(CHAR szBuffer[], CONST CHAR szMessage[], DWORD dwCount);

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
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));
		SendMessage(GetDlgItem(hwnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKEWORD(30, 0));
		SendMessage(GetDlgItem(hwnd, IDC_EDIT_PREFIX), EM_SETLIMITTEXT, 2, 0);
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
		CHAR szPrefix[3] = {};
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				DWORD dwFirst = FIRST_IPADDRESS(dwIPaddress);
				LPARAM lpMask = 0;
				if (dwFirst < 128) lpMask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (dwFirst < 192) lpMask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (dwFirst < 224) lpMask = MAKEIPADDRESS(255, 255, 255, 0);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, lpMask);
			}
			break;
			//Вынесли в WM_NOTIFY, иначе происходит зацикливание сообщений между IDC_IPMASK и IDC_EDIT_PREFIX
			/*case IDC_IPMASK:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				//dwPrefix = 0xFFFFFFFF; // 4 байта забиваем единицами, равнозначно dwPrefix = UINT_MAX;
				for (dwPrefix = 0; dwIPmask; dwPrefix++) dwIPmask <<= 1;
				sprintf(szPrefix, "%i", dwPrefix);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szPrefix);
			}
			break;*/
		case IDC_EDIT_PREFIX:
			//if (HIWORD(wParam) == EN_CHANGE)
		{
			SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)&szPrefix);
			dwPrefix = atoi(szPrefix);
			if (dwPrefix > 30)
			{
				dwPrefix = 30;
				sprintf(szPrefix, "%i", dwPrefix);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szPrefix);
			}
			dwIPmask = UINT_MAX;
			for (INT i = 0; i < 32 - dwPrefix; i++) dwIPmask <<= 1;
			SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
		}
		//break;
		case IDOK:
		{
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwNetworkAddress = dwIPaddress & dwIPmask;				//Адрес сети = побитовое умножение IP-адреса и маски
			DWORD dwBroadcastAddress = dwIPaddress | ~dwIPmask;				//Широковещательный адрес = побитовое сложение IP-alhtccf с инверсией маски
			DWORD dwIPsCount = dwBroadcastAddress - dwNetworkAddress + 1;	//Количество IP-адресов в сети с учетом адреса сети и широковещательного адреса
			DWORD dwHostsCount = dwBroadcastAddress - dwNetworkAddress - 1; //Количество узлов

			CHAR szNetworkAddress[64] = {};
			CHAR szBroadcastAddress[64] = {};
			CHAR szIPsCount[64] = {};
			CHAR szHostsCount[64] = {};
			CHAR szInfo[256] = {};

			sprintf
			(
				szInfo, "%s;\n%s;\n%s;\n%s;\n",
				FormatAddress(szNetworkAddress, "Адрес сети:\t\t\t", dwNetworkAddress),
				FormatAddress(szBroadcastAddress, "Широковещательный адрес:\t", dwBroadcastAddress),
				FormatCount(szIPsCount, "Количество IP-адресов:\t", dwIPsCount),
				FormatCount(szHostsCount, "Количество узлов:\t", dwHostsCount)
			);
			SendMessage(GetDlgItem(hwnd, IDC_STATIC_INFO), WM_SETTEXT, 0, (LPARAM)szInfo);
		}
		break;
		case IDCANCEL:EndDialog(hwnd, 0);
		case IDC_BUTTON_RESET:
			SendMessage(hIPaddress, IPM_CLEARADDRESS, 0, 0);
			SendMessage(hIPmask, IPM_CLEARADDRESS, 0, 0);
			break;
		}
	}
	break;
	case WM_NOTIFY:
	{
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPmask = 0;
		DWORD dwPrefix = 0;
		CHAR szPrefix[3] = {};
		SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
		for (dwPrefix = 0; dwIPmask; dwPrefix++) dwIPmask <<= 1;
		sprintf(szPrefix, "%i", dwPrefix);
		SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szPrefix);
	}
	break;
	case WM_CLOSE: EndDialog(hwnd, 0);
	}
	return FALSE;
}

CHAR* FormatAddress(CHAR szBuffer[], CONST CHAR szMessage[], DWORD dwIPaddress)
{
	sprintf
	(
		szBuffer, "%s%i.%i.%i.%i",
		szMessage,
		FIRST_IPADDRESS(dwIPaddress),
		SECOND_IPADDRESS(dwIPaddress),
		THIRD_IPADDRESS(dwIPaddress),
		FOURTH_IPADDRESS(dwIPaddress)
	);
	return szBuffer;
}
CHAR* FormatCount(CHAR szBuffer[], CONST CHAR szMessage[], DWORD dwCount)
{
	sprintf(szBuffer, "%s\t%i", szMessage, dwCount);
	return szBuffer;
}