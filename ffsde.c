/*
*
* Author: Avi Saranga <avi@openbsd.org.il>, (C) 2009-2010
* Copyright: See COPYING file that comes with this distribution.
*
* This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
* WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h>
#include <pk11pub.h>
#include <nssb64.h>
#include <pk11sdr.h>
#include "iniparser.h"

#define KEY3DB "key3.db"
#define TRUE PR_TRUE
#define MAX_PATH 300
typedef enum
{
	eNoOp = 0,
	eDecrypt = 1,
	eEncrypt = 2
} enumSecOp;

char *pathSeparator = "/";


void PK11Decrypt(char *cipheredBuffer, char **plaintext) {
        SECItem *request;
        SECItem *reply;
        unsigned int len = strlen(cipheredBuffer);

        reply = SECITEM_AllocItem(NULL, NULL, 0);

        request = NSSBase64_DecodeBuffer(NULL, NULL, cipheredBuffer, len);
        PK11SDR_Decrypt(request, reply, NULL);

        if(reply->len > 1) {
        	*plaintext = malloc(reply->len + 1);
        	strncpy(*plaintext, reply->data, reply->len);
        	(*plaintext)[reply->len] = '\0';
        }
        SECITEM_FreeItem(request, TRUE);
        SECITEM_FreeItem(reply, TRUE);
        return;
}

void PK11Encrypt(char *plaintext, char **cipheredBuffer , SECItem *key) {

		SECStatus s;
		SECItem *request;
        SECItem *reply;
        PK11SlotInfo *slot = NULL;

        request = SECITEM_AllocItem(NULL, NULL, 0);
        reply = SECITEM_AllocItem(NULL, NULL, 0);
        request->data = plaintext;
        request->len = strlen(plaintext);
        request->type = 0;
        reply->data = 0;
        reply->len = 0;
        reply->type = 0;

        s = PK11SDR_Encrypt(key, request, reply, NULL);
        if(s == SECSuccess) {
        	*cipheredBuffer = (char *) NSSBase64_EncodeItem(NULL, NULL, 0, reply);
        }

        SECITEM_FreeItem(request, TRUE);
        SECITEM_FreeItem(reply, TRUE);

        return;
}



int get_profile(char* pathProfilesIni, char* profile)
{
       dictionary * ini;
       ini = iniparser_load(pathProfilesIni);
       if (ini==NULL) {
               fprintf(stderr, "cannot parse file: %s\n", pathProfilesIni);
               return -1;
       }
       char *s = iniparser_getstring(ini, "Profile0:Path", NULL);
       int length = strlen(s);
       strncpy(profile, s, length + 1);

       iniparser_freedict(ini);

//        printf("Found profile2 path: %s\n", profile);
        return 0;
}

_Bool initialize() {




	return true;
}

void help()
{
	fprintf(stderr, "\nFirefox keys encryption/decryption using key database.\n");
	fprintf(stderr, "This program performs encryption or decryption on the string it receives and outputs the result to standart output.\n\n");
	fprintf(stderr, "-h path to firefox homedir\n");
	fprintf(stderr, "-d (decrypt)\n");
	fprintf(stderr, "-e (encrypt)\n");
	fprintf(stderr, "-s (string to be encrypted/decrypted)\n");

	return;
}


int main(int argc, char **argv)
{
	int strLength;
	enumSecOp eSecOp = eNoOp;
	char *strkeyDB = NULL;
	char *strKey = NULL;
	char *strResult = NULL;
	char *homedir = getenv("HOME");
	char pathFirefoxData[MAX_PATH];
	char pathProfilesIni[MAX_PATH];
	char profile[MAX_PATH];
	char pathProfile[MAX_PATH];
	char pathSignons[MAX_PATH];
	void *keySlot;
	int c;
	opterr = 0;



	while ((c = getopt(argc, argv, "deh:s:")) != -1) {
		switch(c) {
			case 'd':
				if(eSecOp == eEncrypt) { help(); exit(1); }
				eSecOp = eDecrypt;
				break;
			case 'e':
				if(eSecOp == eDecrypt) { help(); exit(1); }
				eSecOp = eEncrypt;
				break;
			case 'h':
				strLength = strlen(optarg);
				if (!(strkeyDB = (char *) malloc(strLength))) { fprintf(stderr, "out of memory while allocating memory for strkeyDB, peace out!\n"); exit (1); }
				strncpy(strkeyDB,optarg,strLength);
				break;
			case 's':
				strLength = strlen(optarg);
				if (!(strKey = (char *) malloc(strLength))) { fprintf(stderr, "out of memory while allocating memory for strKey, peace out!\n"); exit (1); }
				strncpy(strKey,optarg,strLength);
				break;
			default:
				//help();
				break;
		}

	}
	if(strkeyDB != NULL) {
		sprintf(pathFirefoxData, "%s/.mozilla/firefox", strkeyDB);
	}
	else
	{
		sprintf(pathFirefoxData, "%s/.mozilla/firefox", homedir);
	}

	if(strKey != NULL && eSecOp != eNoOp) {


		sprintf(pathProfilesIni, "%s/profiles.ini", pathFirefoxData);
		get_profile(pathProfilesIni, profile);
		sprintf(pathProfile, "%s%s%s", pathFirefoxData, pathSeparator, profile);

		if(NSS_Init(pathProfile) != SECSuccess) {
			fprintf(stderr, "NSS_Init fails\r\n");
			fflush(stderr);
			return 1;
		}

		if((keySlot = PK11_GetInternalKeySlot()) == NULL) {
		    fprintf(stderr, "PK11_GetInternalKeySlot fails\r\n");
		    fflush(stderr);
		    return 1;
		}

		if(initialize() == true) {

			if(eSecOp == eDecrypt) {

				 PK11Decrypt(strKey, &strResult);

			}
			if(eSecOp == eEncrypt) {
				 //printf("%s", strKey);
				 PK11Encrypt(strKey, &strResult, (SECItem *) keySlot);

			}
		}
	}
	else
	{
		help();
	}

	if(strResult != NULL) {
		/*char *ptr;
		if((ptr = strchr(strResult, '\x0a')) != NULL) {
			*ptr = '\0';
		}*/
		fprintf(stdout, "%s\n", strResult);
	}
	else if(strKey != NULL && strResult == NULL) {
		fprintf(stdout, "Decoder/Encoder failed.\n");
	}
	free(strResult);
	free(strkeyDB);
	free(strKey);

	return 0;
}
