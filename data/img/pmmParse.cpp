#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shlwapi.h>

void Search(TCHAR *SearchFolder, TCHAR *SearchFile, int *Flag, TCHAR *FindPath)
{

	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	TCHAR SearchPath[MAX_PATH], NewSearchFolder[MAX_PATH];

	wsprintf(SearchPath, L"%s\\*", SearchFolder);


	hFind = FindFirstFile(SearchPath, &FindData);
	do
	{      
		wcslwr(FindData.cFileName);
		if(wcscmp(FindData.cFileName, L"..") == 0 || wcscmp(FindData.cFileName, L".") == 0)
		{
		}
		else if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{            
			wsprintf(NewSearchFolder, L"%s\\%s", SearchFolder, FindData.cFileName);
			Search(NewSearchFolder, SearchFile, Flag, FindPath);
		}
		else if(wcscmp(FindData.cFileName, SearchFile) == 0)
		{
			wsprintf(FindPath, L"%s\\%s", SearchFolder, SearchFile);
			*Flag = 1;
		}

	} while(FindNextFile(hFind, &FindData) && *Flag == 0);
	FindClose(hFind);
}

int pmmParse(HWND Edit, TCHAR *ReadFile, TCHAR *OutFolder, TCHAR *SearchFolder1, TCHAR *SearchFolder2, TCHAR *PathList)
{
	TCHAR EditBuffer[65536];
	if(Edit != NULL)
		GetWindowText(Edit, EditBuffer, 65536);

	TCHAR Buffer[1024], Str[MAX_PATH], Temp[MAX_PATH], *Token, OutFile[MAX_PATH];
	if(PathList != NULL)
	{
		PathList[0] = '\0'; //初期化
	}

	//ファイル名抽出
    wcscpy(Str, ReadFile);
    Token = wcstok(Str, L"\\");
    if(Token == NULL)
		return -1;
	wcscpy(Temp, Token);
	while(Token != NULL)
	{
		wcscpy(OutFile, Token);
	    Token = wcstok(NULL, L"\\");
	}
	
	wcscpy(Temp, OutFile);
	Token = wcstok(Temp, L".");
	Token = wcstok(NULL, L"\0");
	if(Token == NULL)
	{
		if(Edit != NULL)
		{
			wsprintf(Buffer, L"フォルダ%sをパスします。\r\n", ReadFile);
			wcscat(EditBuffer, Buffer);
			SetWindowText(Edit, EditBuffer);
		}
		return -1;
	}
	else if(0 != wcsicmp(wcslwr(Token), L"pmm"))
	{
		if(Edit != NULL)
		{
			wsprintf(Buffer, L"ファイル%sをパスします。\r\n", ReadFile);
			wcscat(EditBuffer, Buffer);
			SetWindowText(Edit, EditBuffer);
		}
		return -1;
	}

	if (OutFolder != NULL) //出力あり
	{
		
		if (!PathIsDirectory(OutFolder))
		{
			if(0 == CreateDirectory(OutFolder, NULL))
			{
				if(Edit != NULL)
				{
					wsprintf(Buffer, L"出力フォルダが生成できません。\r\n");
					wcscat(EditBuffer, Buffer);
					SetWindowText(Edit, EditBuffer);
				}
				return -1;
			}
		}


		wcscpy(Temp, OutFile);
		wsprintf(OutFile, L"%s\\%s", OutFolder, Temp);
	}



//	if(wcscmp(wcslwr(ReadFile), wcslwr(OutFile)) == 0)
//	{
//		if(Edit != NULL)
//		{
//			wsprintf(Buffer, L"ファイル%sは入力と出力が一緒です。\r\n", ReadFile);
//			wcscat(EditBuffer, Buffer);
//			SetWindowText(Edit, EditBuffer);
//		}
//		return -1;
//
//	}

	if(Edit != NULL)
	{
		wsprintf(Buffer, L"ファイル%sを解析開始しました。\r\n", ReadFile);
		wcscat(EditBuffer, Buffer);
	}


	//重複対策追加
	TCHAR TempReadFile[MAX_PATH];
	if (OutFolder != NULL) //出力あり
	{
		//重複対策追加
		wsprintf(TempReadFile, L"%s\\working.pmm", OutFolder);
		CopyFile(ReadFile, TempReadFile, FALSE);
		wcscpy(ReadFile, TempReadFile);
	}
///////////////////////////////////////////////////////////////////////////
	typedef struct
	{
		void *Next;
		void *Previous;
		char Path[MAX_PATH];
		char Data[500];
		int Count;
	} TCreateData;

	TCreateData *CreateData, *CreateDataNow;
	CreateData = new TCreateData;
	CreateDataNow = CreateData;
	CreateDataNow->Next = NULL;
	CreateDataNow->Previous = NULL;
	FILE *rfp, *wfp;
	int Count, NowCount;
    char Data[1500];
	char *Before = &Data[0];
	char *Main = &Data[500];
	char *After = &Data[1000];
	memset(Data, 0, 1500 * sizeof(char));


    rfp = _wfopen(ReadFile, L"rb");
	if (OutFolder != NULL) //出力あり
	{
		wfp = _wfopen(OutFile, L"wb");
	}

	NowCount = fread(Main, sizeof(char), 500, rfp);
	Count = fread(After, sizeof(char), 500, rfp);

	do
	{
		//routine
		int i = 0;
		int j;
		int Start = 0;
		char TargetFile[MAX_PATH], TargetPath[MAX_PATH];
		int Flag, ml;
		char Inc[6];
		TCHAR TargetFileUnicode[MAX_PATH], TargetPathUnicode[MAX_PATH], SearchPathUnicode[MAX_PATH], FindPath[MAX_PATH];


		while(i < NowCount)
		{
			if(Main[i] == '.')
			{
				j = i + 1;

				memcpy(Inc,Main + j - 2, 6);
				strlwr(Inc);

				ml = 0;
				if(strncmp(Inc + 2, "pmd", 3) == 0)
				{
					int p = j;
					while(Main[p] != ':' && Main[p] != 0) --p;

					if(Main[p] == ':') //フルパスがあるのがアクセパス
					{
						i += 4;
						ml = 1;
					}
				}
				else if(strncmp(Inc + 2, "avi", 3) == 0)
				{
					int p = j;
					while(Main[p] != ':' && Main[p] != 0) --p;

					if(Main[p] == ':') //フルパスがあるのがアクセパス
					{
						i += 4;
						ml = 1;
					}
				}
				else if(strncmp(Inc + 2, "bmp", 3) == 0)
				{
					int p = j;
					while(Main[p] != ':' && Main[p] != 0) --p;

					if(Main[p] == ':') //フルパスがあるのがアクセパス
					{
						i += 4;
						ml = 1;
					}
				}
				else if(strncmp(Inc + 2, "wav", 3) == 0)
				{
					int p = j;
					while(Main[p] != ':' && Main[p] != 0) --p;

					if(Main[p] == ':') //フルパスがあるのがアクセパス
					{
						i += 4;
						ml = 1;
					}
				}
				else if(Inc[0] != 0 && Inc[1] == '.' && Inc[2] == 'x' && Inc[3] == 0)
				{
					int p = j;
					while(Main[p] != ':' && Main[p] != 0) --p;

					if(Main[p] == ':') //フルパスがあるのがアクセパス
					{
						i += 2;
						ml = 1;
					}
				}

				if(ml == 1)
				{
					while(Main[j] != '\\') --j;
					j++;
					strncpy(TargetFile, Main + j, i - j);
					TargetFile[i - j] = '\0';
					while(Main[j] != ':') --j;
					--j;
					if(Main[j] == 0) ++j; //バグ対策
					strncpy(TargetPath, Main + j, i - j);
					TargetPath[i - j] = '\0';

					MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, TargetPath, i - j + 1, TargetPathUnicode, MAX_PATH);
					if(PathList != NULL)
					{
						wcscat(PathList, TargetPathUnicode);
						wcscat(PathList, L",");
					}
					if(Edit != NULL)
					{
						wsprintf(Buffer, L"----------------------------------------\r\nパス%sを書き換えます。\r\n", TargetPathUnicode);
						wcscat(EditBuffer, Buffer);
						SetWindowText(Edit, EditBuffer);
					}

					if(j <= 0)
					{
//						wsprintf(Buffer, L"debug:j=%d\r\n", j);
//						wcscat(EditBuffer, Buffer);
//						SetWindowText(Edit, EditBuffer);

						CreateDataNow->Count = 0;
						((TCreateData *)CreateDataNow->Previous)->Count += j; //オーバー分を引く
					}
					else
					{
						CreateDataNow->Count = j - Start;
						memcpy(CreateDataNow->Data, Main + Start, j);
					}

					MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, TargetFile, strlen(TargetFile) + 1, TargetFileUnicode, MAX_PATH);
					wcslwr(TargetFileUnicode);

					if(OutFolder != NULL) //出力がない場合は書き換えルーチンを飛ばす
					{

						Flag = 0;
						FindPath[0] = '\0';

						if(SearchFolder1 != NULL)
						{
							wcscpy(SearchPathUnicode, SearchFolder1);
							wcslwr(SearchPathUnicode);
							Search(SearchPathUnicode, TargetFileUnicode, &Flag, FindPath);
						}
						if(SearchFolder2 != NULL && (SearchFolder1 == NULL || FindPath[0] == '\0')) //みつからない
						{
							wcscpy(SearchPathUnicode, SearchFolder2);
							wcslwr(SearchPathUnicode);
							Search(SearchPathUnicode, TargetFileUnicode, &Flag, FindPath);
						}

						if(FindPath[0] != '\0')
						{
							if(Edit != NULL)
							{
								wsprintf(Buffer, L"パス%sに変更しました。\r\n----------------------------------------\r\n", FindPath);
								wcscat(EditBuffer, Buffer);
								SetWindowText(Edit, EditBuffer);
							}

							WideCharToMultiByte(CP_ACP, 0, FindPath, wcslen(FindPath) + 1, CreateDataNow->Path, MAX_PATH, NULL, NULL);

							int x = strlen(TargetPath) - strlen(CreateDataNow->Path);
							if(x > 0)
							{
								CreateDataNow->Next = (void *)new TCreateData;
								((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
								CreateDataNow = (TCreateData *)CreateDataNow->Next;

								CreateDataNow->Count = x;
								memset(CreateDataNow->Data, 0x00, x * sizeof(char));
								strcpy(CreateDataNow->Path, "");
								CreateDataNow->Next = (void *)new TCreateData;
								((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
								CreateDataNow = (TCreateData *)CreateDataNow->Next;
							}
							else if(x < 0)
							{
								i += -x;
								Main[i] = 0; //終端を追加
								CreateDataNow->Next = (void *)new TCreateData;
								((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
								CreateDataNow = (TCreateData *)CreateDataNow->Next;
							}
							else
							{
								CreateDataNow->Next = (void *)new TCreateData;
								((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
								CreateDataNow = (TCreateData *)CreateDataNow->Next;
							}
						}
						else
						{
							if(strncmp(Inc + 2, "avi", 3) == 0 || strncmp(Inc + 2, "wav", 3) == 0)
							{
								if(Edit != NULL)
								{
									wsprintf(Buffer, L"ファイル%sが見つかりませんでした。続行します。\r\n----------------------------------------\r\n", TargetFileUnicode);
									wcscat(EditBuffer, Buffer);
									SetWindowText(Edit, EditBuffer);
								}

								strcpy(CreateDataNow->Path, TargetPath);

								CreateDataNow->Next = (void *)new TCreateData;
								((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
								CreateDataNow = (TCreateData *)CreateDataNow->Next;
							}
							else
							{
								if(Edit != NULL)
								{
									wsprintf(Buffer, L"ファイル%sが見つかりませんでした。失敗しました。\r\n----------------------------------------\r\n", TargetFileUnicode);
									wcscat(EditBuffer, Buffer);
									SetWindowText(Edit, EditBuffer);
								}
								return -1;
							}
						}
					}

					Start = i;
					--i; //相殺//iの場所が先頭だから。
				}

			}
			++i;
		}

		CreateDataNow->Count = i - Start;
		memcpy(CreateDataNow->Data, Main + Start, i - Start);
		strcpy(CreateDataNow->Path, "");
		CreateDataNow->Next = (void *)new TCreateData;
		((TCreateData *)CreateDataNow->Next)->Previous = (void *)CreateDataNow;
		CreateDataNow = (TCreateData *)CreateDataNow->Next;

		//
		memcpy(Before, Before + i, 500);
		int y = i - NowCount;
		if(After == NULL) 
			Main = NULL;
		else
		{
			memcpy(Main, Main + i, 500 - y);
			NowCount = Count;
		}

		if(feof(rfp) != 0) 
		{
			After = NULL;
		}
		else
		{
			Count = fread(Main + 500 - y, sizeof(char), i, rfp);
			if(Count - y >= 0)
			{
				Count -= y;
			}
			else
			{
				NowCount -= y - Count;
				Count = 0;
				After = NULL;

//				wsprintf(Buffer, L"debug:NowCount=%d\r\n", NowCount);
//				wcscat(EditBuffer, Buffer);
//				SetWindowText(Edit, EditBuffer);
			}
		}	


	}while(Main != NULL);

	CreateDataNow = (TCreateData *)CreateDataNow->Previous;
	delete CreateDataNow->Next;
	CreateDataNow->Next = NULL;

	CreateDataNow = CreateData;

	do
	{
		if(OutFolder != NULL)
		{
			fwrite(CreateDataNow->Data, sizeof(char), CreateDataNow->Count, wfp);
			if(CreateDataNow->Path[0] != '\0')
				fwrite(CreateDataNow->Path, sizeof(char), strlen(CreateDataNow->Path), wfp);
		}
		CreateDataNow = (TCreateData *)CreateDataNow->Next;
	}
	while(CreateDataNow != NULL);


	fclose(rfp);
	if(OutFolder != NULL)
	{
		fclose(wfp);
	}
///////////////////////////////////////////////////////////////////////////
	if(Edit != NULL && OutFolder != NULL)
	{
		wsprintf(Buffer, L"ファイル%sを出力完了しました。\r\n", OutFile);
		wcscat(EditBuffer, Buffer);

		SetWindowText(Edit, EditBuffer);
	}

	if (OutFolder != NULL) //出力あり
	{
		//重複対策追加
		DeleteFile(ReadFile);
	}


	return 0;
}
