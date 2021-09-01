#include <stdio.h>
#include "functions.h"

#define NAME_SIZE     100
#define NUMBER_SIZE   20
#define MAX_NUMBERS   10
#define EMAIL_SIZE    50
#define MAX_EMAILS    10
#define ADDRESS_SIZE  100
#define CONTACT_SIZE	NAME_SIZE + MAX_NUMBERS*NUMBER_SIZE + MAX_EMAILS*EMAIL_SIZE + ADDRESS_SIZE



struct Contact
{
	char name[NAME_SIZE];
	char number[MAX_NUMBERS][NUMBER_SIZE];
	int numberArraySize;
	char email[MAX_EMAILS][EMAIL_SIZE];
	int emailArraySize;
	char address[ADDRESS_SIZE];
};

void initializeContact(struct Contact *c)
{
	memset(c->name, '\0', NAME_SIZE);
	memset(c->address, '\0', ADDRESS_SIZE);
	c->emailArraySize = 0;
	c->numberArraySize = 0;
}

int getNumberOfContacts(char filePath[])
{
	FILE* filePointer = fopen(filePath, "r");
	if (filePointer == NULL)return -1;
	int noc = 0;
	char ch;
	for (ch = getc(filePointer); ch != EOF; ch = getc(filePointer))if (ch == '\n')noc++;
	fclose(filePointer);
	return noc;
}



void getContactName(char str[], char nameBuffer[])
{
	substring(str, nameBuffer, 0, indexOf(';', str, 1));
}



void getContactAddress(char str[], char addressBuffer[])
{
	substring(str, addressBuffer, indexOf(';', str, 3) + 1, indexOf(';', str, 4));
}



void getContactNumbers(char str[], char numberBuffer[][NUMBER_SIZE], int *numberArraySize)
{
	int i, n;

	char numbers[MAX_NUMBERS*NUMBER_SIZE];

	substring(str, numbers, indexOf(';', str, 1) + 1, indexOf(';', str, 2));
	n = frequency(',', numbers);

	for (i = 0; i < n; i++)
	{
		substring(numbers, numberBuffer[i], 0, indexOf(',', numbers, 1));
		substring(numbers, numbers, indexOf(',', numbers, 1) + 1, strlen(numbers));
	}
	if (numberArraySize != NULL)*numberArraySize = n;
}



void getContactEmails(char str[], char emailBuffer[][EMAIL_SIZE], int *emailArraySize)
{
	int i, n;
	char emails[MAX_EMAILS*EMAIL_SIZE];

	substring(str, emails, indexOf(';', str, 2) + 1, indexOf(';', str, 3));
	n = frequency(',', emails);

	for (i = 0; i < n; i++)
	{
		substring(emails, emailBuffer[i], 0, indexOf(',', emails, 1));
		substring(emails, emails, indexOf(',', emails, 1) + 1, strlen(emails));
	}
	if (emailArraySize != NULL)*emailArraySize = n;
}

bool findContactName(char filePath[], char contactName[])
{
	FILE* filePointer = fopen(filePath, "r");
	bool found = false;
	char lineBuffer[CONTACT_SIZE], nameBuffer[NAME_SIZE];

	while (fgets(lineBuffer, CONTACT_SIZE, filePointer) != NULL)
	{
		getContactName(lineBuffer, nameBuffer);
		if (_strcmpi(contactName, nameBuffer) == 0)
		{
			found = true;
			break;
		}
	}
	fclose(filePointer);
	return found;
}


struct Contact getContactInfo(char str[])
{
	struct Contact c;

	getContactName(str, c.name);
	getContactNumbers(str, c.number, &c.numberArraySize);
	getContactEmails(str, c.email, &c.emailArraySize);
	getContactAddress(str, c.address);

	return c;
}

struct Contact getContactByName(char filePath[], char contactName[])
{
	FILE* filePointer = fopen(filePath, "r");
	struct Contact c = { 0 };

	char lineBuffer[CONTACT_SIZE], nameBuffer[NAME_SIZE];

	while (fgets(lineBuffer, CONTACT_SIZE, filePointer) != NULL)
	{
		getContactName(lineBuffer, nameBuffer);
		if (_stricmp(nameBuffer, contactName) == 0)
		{
			getContactName(lineBuffer, c.name);
			getContactNumbers(lineBuffer, c.number, &c.numberArraySize);
			getContactEmails(lineBuffer, c.email, &c.emailArraySize);
			getContactAddress(lineBuffer, c.address);
			break;
		}
	}
	fclose(filePointer);
	return c;
}






void addContact(char filePath[], struct Contact c)
{
	int i;
	FILE* filePointer = fopen(filePath, "a");
	fprintf(filePointer, "%s;", c.name);
	for (i = 0; i < c.numberArraySize; i++)
	{
		fprintf(filePointer, "%s,", c.number[i]);
	}
	fprintf(filePointer, ";");
	for (i = 0; i < c.emailArraySize; i++)
	{
		fprintf(filePointer, "%s,", c.email[i]);
	}
	fprintf(filePointer, ";%s;\n", c.address);
	fclose(filePointer);
}


int deleteContact(char filePath[], char contactName[])
{
	char temp[] = "temp.txt";

	FILE* readFilePointer = fopen(filePath, "r");
	FILE* writeFilePointer = fopen(temp, "w");

	char lineBuffer[CONTACT_SIZE];
	char nameBuffer[NAME_SIZE];

	int lineNumber = -1;

	for (int i = 0; fgets(lineBuffer, CONTACT_SIZE, readFilePointer) != NULL; i++)
	{
		getContactName(lineBuffer, nameBuffer);
		if (_stricmp(nameBuffer, contactName) == 0)lineNumber = i;
		else fprintf(writeFilePointer, "%s", lineBuffer);
	}

	fclose(readFilePointer);
	fclose(writeFilePointer);
	remove(filePath);
	rename(temp, filePath);
	return lineNumber;
}





/*
	Returns line number at which it is found
	Returns -1 if contact name is not found
*/
int searchContact(char filePath[], char contactName[])
{
	FILE* filePointer = fopen(filePath, "r");

	char lineBuffer[CONTACT_SIZE];
	char nameBuffer[NAME_SIZE];

	int lineNumber = -1;
	for (int i = 0; fgets(lineBuffer, CONTACT_SIZE, filePointer); i++)
	{
		getContactName(lineBuffer, nameBuffer);
		if (_stricmp(nameBuffer, contactName) == 0)
		{
			lineNumber = i;
			break;
		}
	}

	fclose(filePointer);
	return lineNumber;
}

void insertContact(char filePath[], struct Contact c, int lineNumber)
{
	if (getNumberOfContacts(filePath) <= lineNumber)
	{
		addContact(filePath, c);
		return;
	}
	char temp[] = "temp.txt";

	FILE* readFilePointer = fopen(filePath, "r");
	FILE* writeFilePointer = fopen(temp, "w");

	char lineBuffer[CONTACT_SIZE];

	for (int i = 0; fgets(lineBuffer, CONTACT_SIZE, readFilePointer) != NULL; i++)
	{
		if (i == lineNumber)
		{
			//fprintf(writeFilePointer, "%s;%s;%s;\n", c.name, c.number, c.email);

			fprintf(writeFilePointer, "%s;", c.name);
			for (i = 0; i < c.numberArraySize; i++)
			{
				fprintf(writeFilePointer, "%s,", c.number[i]);
			}
			fprintf(writeFilePointer, ";");
			for (i = 0; i < c.emailArraySize; i++)
			{
				fprintf(writeFilePointer, "%s,", c.email[i]);
			}
			fprintf(writeFilePointer, ";%s;\n", c.address);
		}
		fprintf(writeFilePointer, "%s", lineBuffer);
	}

	fclose(readFilePointer);
	fclose(writeFilePointer);
	remove(filePath);
	rename(temp, filePath);
}



/**
	Replace Contact Function:
	Search for Contact Name and Replace that Contact with New Contact
	If Contact Name is not found, nothing is replaced
 */
void replaceContact(char filePath[], char oldContactName[], struct Contact newContact)
{
	int lineNumber = deleteContact(filePath, oldContactName);
	if (lineNumber != -1)insertContact(filePath, newContact, lineNumber);
}
