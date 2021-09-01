#include "contacts.h"
#include "resource.h"
#include <windows.h>
#include <CommCtrl.h>


//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



/************************Definitions************************/


#define ID_MENU_FILE	100
#define ID_FILE_NEW		101
#define ID_FILE_OPEN	102
#define ID_FILE_CLOSE	103
#define ID_FILE_EXIT	104

#define ID_BTN_ADD		201
#define ID_BTN_DELETE	202
#define ID_BTN_EDIT		203

#define ID_SEARCHBOX	301

#define ID_LISTVIEW		401

/***********************************************************/





/************************Variables************************/
int windowWidth = 800;
int windowHeight = 500;
int listWidth = 760;
int listHeight = 300;

char filePath[MAX_PATH];

char replaceContactName[NAME_SIZE];
char replaceContactNumber[MAX_NUMBERS*NUMBER_SIZE];
char replaceContactEmail[MAX_EMAILS*EMAIL_SIZE];
char replaceContactAddress[ADDRESS_SIZE];

HWND contactListView, searchTextBox, btnAdd, btnRemove, btnEdit, labelSearchContact;

bool isFileOpen;

const char windowClassName[] = "WindowClass";

/*********************************************************/





/************************Functions************************/

void addContactsToList(char fileName[], char startsWithString[])
{
	FILE* fileptr = fopen(fileName, "r");
	int i = 0, j;

	SendMessage(contactListView, LVM_DELETEALLITEMS, 0, 0L);

	LVITEM listItem = { 0 };
	listItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;

	char contactBuffer[CONTACT_SIZE];
	char buffer[CONTACT_SIZE];
	char nameBuffer[NAME_SIZE];
	char numberBuffer[MAX_NUMBERS][NUMBER_SIZE];
	char emailBuffer[MAX_EMAILS][EMAIL_SIZE];
	char addressBuffer[ADDRESS_SIZE];

	while (fgets(contactBuffer, CONTACT_SIZE, fileptr) != NULL)
	{
		memset(nameBuffer, '\0', NAME_SIZE);
		memset(addressBuffer, '\0', ADDRESS_SIZE);
		memset(buffer, '\0', CONTACT_SIZE);
		for (j = 0; j < MAX_NUMBERS; j++)
		{
			memset(numberBuffer[j], '\0', NUMBER_SIZE);
		}
		for (j = 0; j < MAX_EMAILS; j++)
		{
			memset(emailBuffer[j], '\0', EMAIL_SIZE);
		}

		getContactName(contactBuffer, nameBuffer);
		if (startsWith(startsWithString, nameBuffer))
		{
			listItem.iItem = i;
			listItem.pszText = nameBuffer;

			ListView_InsertItem(contactListView, &listItem);

			int numberArraySize;
			getContactNumbers(contactBuffer, numberBuffer, &numberArraySize);
			strcpy(buffer, numberBuffer[0]);
			for (j = 1; j < numberArraySize; j++)
			{
				strcat(buffer, ",");
				strcat(buffer, numberBuffer[j]);
			}
			ListView_SetItemText(contactListView, i, 1, buffer);

			memset(buffer, '\0', CONTACT_SIZE);

			int emailArraySize;
			getContactEmails(contactBuffer, emailBuffer,&emailArraySize);
			strcpy(buffer, emailBuffer[0]);
			for (j = 1; j < emailArraySize; j++)
			{
				strcat(buffer, ",");
				strcat(buffer, emailBuffer[j]);
			}
			ListView_SetItemText(contactListView, i, 2, buffer);

			getContactAddress(contactBuffer, addressBuffer);
			ListView_SetItemText(contactListView, i, 3, addressBuffer);
			i++;
		}
	}
	fclose(fileptr);
}

void createFile(HWND hwndParent)
{
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndParent;
	ofn.lpstrFile = filePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Create Contact File";
	int result = GetSaveFileName(&ofn);
	if (result != 0)
	{
		isFileOpen = true;
		FILE* fileptr = fopen(filePath, "a");
		fclose(fileptr);
		EnableWindow(searchTextBox, TRUE);
		EnableWindow(contactListView, TRUE);
	}
}

void openFile(HWND hwndParent)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME)); //Set all parameters to zero
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndParent;
	ofn.lpstrFile = filePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Open Contact File";
	int result = GetOpenFileName(&ofn);
	if (result != 0)
	{
		isFileOpen = true;
		EnableWindow(searchTextBox, TRUE);
		EnableWindow(contactListView, TRUE);
		addContactsToList(filePath, "");
	}
}

void closeFile()
{
	ListView_DeleteAllItems(contactListView);
	memset(filePath, '\0', MAX_PATH);
	isFileOpen = false;
	SetWindowText(searchTextBox, "");
	EnableWindow(searchTextBox, FALSE);
	EnableWindow(contactListView, FALSE);
}



void createListViewWindow(HWND hwndParent)
{
	contactListView = CreateWindow(WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | WS_DISABLED, 10, 94, listWidth, listHeight, hwndParent, (HMENU)ID_LISTVIEW, NULL, NULL);

	LPSTR columnHeads[] = { "Name", "Number", "Email", "Address" };
	int numberOfColumns = 4;
	LVCOLUMN columns;      // List View Column structure
	columns.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columns.fmt = LVCFMT_LEFT;		// Left align the column
	columns.cx = (int)(listWidth/4);				// Width of the column in pixels

	//Adding Columns
	for (int i = 0; i < numberOfColumns; i++)
	{
		columns.iSubItem = i;
		columns.pszText = columnHeads[i];    //Column Header Text
		ListView_InsertColumn(contactListView, i, &columns);
	}
}

void createMenu(HWND hwndParent)
{
	HMENU menuFile = CreateMenu();

	HMENU fileSubMenu = CreatePopupMenu();
	AppendMenu(fileSubMenu, MF_STRING, ID_FILE_NEW, "&New");
	AppendMenu(fileSubMenu, MF_STRING, ID_FILE_OPEN, "&Open");
	AppendMenu(fileSubMenu, MF_STRING, ID_FILE_CLOSE, "&Close");
	AppendMenu(fileSubMenu, MF_SEPARATOR, -1, NULL);
	AppendMenu(fileSubMenu, MF_STRING, ID_FILE_EXIT, "&Exit");

	AppendMenu(menuFile, MF_STRING | MF_POPUP, (UINT_PTR)fileSubMenu, "&File");

	SetMenu(hwndParent, menuFile);
}

void addControls(HWND hwnd)
{
	HFONT defaultGUIFont = GetStockObject(DEFAULT_GUI_FONT);
	HFONT fontSegoeUI = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Segoe UI");


	btnAdd = CreateWindow(WC_BUTTON, "Add", WS_VISIBLE | WS_CHILD, 10, 400, 50, 25, hwnd, (HMENU)ID_BTN_ADD, NULL, NULL);
	btnRemove = CreateWindow("Button", "Delete", WS_VISIBLE | WS_CHILD, 60, 400, 60, 25, hwnd, (HMENU)ID_BTN_DELETE, NULL, NULL);
	btnEdit = CreateWindow("Button", "Edit", WS_VISIBLE | WS_CHILD, 120, 400, 50, 25, hwnd, (HMENU)ID_BTN_EDIT, NULL, NULL);


	labelSearchContact = CreateWindow("Static", "Search Contact:", WS_CHILD | WS_VISIBLE, 10, 40, 100, 30, hwnd, NULL, NULL, NULL);
	searchTextBox = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED, 10, 60, 760, 24, hwnd, (HMENU)ID_SEARCHBOX, NULL, NULL);


	SendMessage(btnAdd, WM_SETFONT, (LPARAM)defaultGUIFont, TRUE);
	SendMessage(btnRemove, WM_SETFONT, (LPARAM)defaultGUIFont, TRUE);
	SendMessage(btnEdit, WM_SETFONT, (LPARAM)defaultGUIFont, TRUE);
	SendMessage(labelSearchContact, WM_SETFONT, (LPARAM)defaultGUIFont, TRUE);
	SendMessage(searchTextBox, WM_SETFONT, (LPARAM)fontSegoeUI, TRUE);
}


/*********************************************************/


BOOL CALLBACK InputDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, IDB_CANCEL);
		return TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{

		case IDB_ADD:
		{
			struct Contact c = { 0 };

			char nameBuffer[NAME_SIZE], numberBuffer[NUMBER_SIZE], emailBuffer[EMAIL_SIZE], addressBuffer[ADDRESS_SIZE];

			GetDlgItemText(hwnd, IDE_NAME, nameBuffer, NAME_SIZE);
			if (nameBuffer[0] == '\0')
			{
				MessageBox(NULL, "Name not provided!", "No Name", MB_OK);
				break;
			}
			if (findContactName(filePath, nameBuffer))
			{
				char buffer[MAX_PATH];
				strcpy(buffer, "Contact with name ");
				strcat(buffer, nameBuffer);
				strcat(buffer, " already exist. Do you want to merge this contact?");
				int result = MessageBox(NULL, buffer, "Contact Already Exist", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
				if (result == IDYES)
				{
					c = getContactByName(filePath, nameBuffer);
					GetDlgItemText(hwnd, IDE_NUMBER, numberBuffer, NUMBER_SIZE);
					if(numberBuffer[0]!='\0')strcpy(c.number[c.numberArraySize++], numberBuffer);

					GetDlgItemText(hwnd, IDE_EMAIL, emailBuffer, EMAIL_SIZE);
					if (emailBuffer[0] != '\0')strcpy(c.email[c.emailArraySize++], emailBuffer);

					GetDlgItemText(hwnd, IDE_ADDRESS, addressBuffer, ADDRESS_SIZE);
					if (addressBuffer[0] != '\0')strcpy(c.address, addressBuffer);
					replaceContact(filePath, nameBuffer, c);
				}
			}
			else
			{
				strcpy(c.name, nameBuffer);

				GetDlgItemText(hwnd, IDE_NUMBER, numberBuffer, NUMBER_SIZE);
				if (numberBuffer[0] != '\0')strcpy(c.number[c.numberArraySize++], numberBuffer);

				GetDlgItemText(hwnd, IDE_EMAIL, emailBuffer, EMAIL_SIZE);
				if (emailBuffer[0] != '\0')strcpy(c.email[c.emailArraySize++], emailBuffer);

				GetDlgItemText(hwnd, IDE_ADDRESS, addressBuffer, ADDRESS_SIZE);
				if (addressBuffer[0] != '\0')strcpy(c.address, addressBuffer);

				addContact(filePath, c);
			}

			EndDialog(hwnd, IDB_ADD);
			addContactsToList(filePath, "");

			return TRUE;
		}

		case IDB_CANCEL:
			EndDialog(hwnd, IDB_CANCEL);
			return TRUE;

		default:
			return FALSE;
		}
		return TRUE;
	}

	default:
		return FALSE;
	}
	return TRUE;
}



BOOL CALLBACK EditDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwnd, IDE_NAME, replaceContactName);
		SetDlgItemText(hwnd, IDE_NUMBER, replaceContactNumber);
		SetDlgItemText(hwnd, IDE_EMAIL, replaceContactEmail);
		SetDlgItemText(hwnd, IDE_ADDRESS, replaceContactAddress);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, IDB_CANCEL);
		return TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{

		case IDB_OK:
		{
			struct Contact c = { 0 };

			char nameBuffer[NAME_SIZE], numberBuffer[MAX_NUMBERS*NUMBER_SIZE], emailBuffer[MAX_EMAILS*EMAIL_SIZE], addressBuffer[ADDRESS_SIZE], buffer[CONTACT_SIZE];

			GetDlgItemText(hwnd, IDE_NAME, nameBuffer, NAME_SIZE);

			GetDlgItemText(hwnd, IDE_NUMBER, numberBuffer, MAX_NUMBERS*NUMBER_SIZE);
			strcat(numberBuffer, ",");

			GetDlgItemText(hwnd, IDE_EMAIL, emailBuffer, MAX_EMAILS*EMAIL_SIZE);
			strcat(emailBuffer, ",");

			GetDlgItemText(hwnd, IDE_ADDRESS, addressBuffer, ADDRESS_SIZE);

			strcpy(buffer, nameBuffer);
			strcat(buffer, ";");
			strcat(buffer, numberBuffer);
			strcat(buffer, ";");
			strcat(buffer, emailBuffer);
			strcat(buffer, ";");
			strcat(buffer, addressBuffer);
			strcat(buffer, ";");

			c = getContactInfo(buffer);

			replaceContact(filePath, replaceContactName, c);

			EndDialog(hwnd, IDB_OK);

			addContactsToList(filePath, "");
			return TRUE;
		}

		case IDB_CANCEL:
			EndDialog(hwnd, IDB_CANCEL);
			return TRUE;

		default:
			return FALSE;
		}
		return TRUE;
	}

	default:
		return FALSE;
	}
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_CREATE:
	{
		isFileOpen = false;
		memset(filePath, '\0', MAX_PATH);

		createMenu(hwnd);

		addControls(hwnd);

		createListViewWindow(hwnd);

		break;
	}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_BTN_ADD:
		{
			if (isFileOpen)
			{
				INT_PTR ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INPUT_DIALOG), hwnd, (DLGPROC)InputDlgProc);
				if (ret == -1)MessageBox(hwnd, "Add Contact Dialog Creation Failed!", "Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		case ID_BTN_DELETE:
		{
			if (isFileOpen)
			{
				int selectedIndex = ListView_GetNextItem(contactListView, -1, LVNI_SELECTED);
				if (selectedIndex != -1)
				{
					char nameBuffer[NAME_SIZE];
					char buffer[MAX_PATH];
					ListView_GetItemText(contactListView, selectedIndex, 0, nameBuffer, NAME_SIZE);
					strcpy(buffer, "Are you sure you want to delete the contact ");
					strcat(buffer, nameBuffer);
					strcat(buffer, "?");
					int result = MessageBox(NULL, buffer, "Delete Contact", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
					if (result == IDYES)
					{
						deleteContact(filePath, nameBuffer);
						addContactsToList(filePath, "");
					}
				}
			}
			break;
		}
		case ID_BTN_EDIT:
		{
			int selectedIndex = ListView_GetNextItem(contactListView, -1, LVNI_SELECTED);
			if (selectedIndex != -1)
			{
				char buffer[CONTACT_SIZE];
				ListView_GetItemText(contactListView, selectedIndex, 0, buffer, CONTACT_SIZE);
				strcpy(replaceContactName, buffer);
				ListView_GetItemText(contactListView, selectedIndex, 1, buffer, CONTACT_SIZE);
				strcpy(replaceContactNumber, buffer);
				ListView_GetItemText(contactListView, selectedIndex, 2, buffer, CONTACT_SIZE);
				strcpy(replaceContactEmail, buffer);
				ListView_GetItemText(contactListView, selectedIndex, 3, buffer, CONTACT_SIZE);
				strcpy(replaceContactAddress, buffer);

				INT_PTR ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_DIALOG), hwnd, (DLGPROC)EditDlgProc);
				if (ret == -1)MessageBox(hwnd, "Edit Contact Dialog Creation Failed!", "Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		case ID_FILE_NEW:
		{
			if (isFileOpen)
			{
				int result = MessageBox(NULL, "This file is currently in use. Do you want to close this file?", "File In Use", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
				if (result == IDYES)
				{
					closeFile();
					createFile(hwnd);
				}
			}
			else
			{
				createFile(hwnd);
			}
			break;
		}
		case ID_FILE_OPEN:
		{
			if (isFileOpen)
			{
				int result = MessageBox(NULL, "This file is currently in use. Do you want to close this file?", "File In Use", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
				if (result == IDYES)
				{
					closeFile();
					openFile(hwnd);
				}
			}
			else
			{
				openFile(hwnd);
			}
			break;
		}
		case ID_FILE_CLOSE:
		{
			if (isFileOpen)
			{
				int result = MessageBox(NULL, "Do you want to close this file?", "Close File", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
				if (result == IDYES)closeFile();
			}
			break;
		}
		case ID_FILE_EXIT:
		{
			int result = MessageBox(NULL, "Are you sure you want to exit?", "Exit", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
			if (result == IDYES)DestroyWindow(hwnd);
			break;
		}
		case ID_SEARCHBOX:
		{
			if (HIWORD(wParam) == EN_CHANGE && isFileOpen)
			{
				char buffer[NAME_SIZE];
				GetWindowText(searchTextBox, buffer, NAME_SIZE);
				addContactsToList(filePath, buffer);
			}
			break;
		}
		}
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND mainWindow;

	memset(&wc, 0, sizeof(wc));

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = windowClassName;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	mainWindow = CreateWindowEx(WS_EX_WINDOWEDGE, windowClassName, "Contacts Manager", (WS_VISIBLE | WS_OVERLAPPEDWINDOW) & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (mainWindow == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	MSG Msg;

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}

