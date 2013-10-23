#pragma once

#ifndef PACMANCLIENT_PRIVATE_HEADER
#include "pacmanclient.h"
#endif

#include "regext.h"
#include <list>

typedef enum
{
	LARGE = 0xE1,
	MEDIUM = 0xD1,
	SMALL = 0xC8
}TILE_TYPE;

typedef struct
{
	APPID appId;
	DWORD dwPosition;
	wchar_t tokenID[200];
}MOVABLE_TILE;

typedef enum
{
	TPT_NO_TRANSFORM = 0,
	TPT_NORMALIZE = 1
}TILE_POSITION_TRANSFORMATION;

typedef struct
{
	bool sectors[4];
}TILE_ROW;

typedef std::list<MOVABLE_TILE*> mlist;

namespace Shell
{
	// class used to manipulate tile positions.
	class CTilePosition
	{
	public:
		void SetDirect(DWORD value)
		{
			dwPosition = value;
		}

		DWORD GetDirect()
		{
			return dwPosition;
		}


		TILE_TYPE GetTileType()
		{
			return (TILE_TYPE)((dwPosition >> 24) & 0xFF);
		}

		DWORD GetColumn(TILE_POSITION_TRANSFORMATION transformation)
		{
			DWORD res = (dwPosition >> 16) & 0xFF;
			if (transformation == TPT_NORMALIZE)
			{
				TILE_TYPE type = GetTileType();
				if (type == MEDIUM)
				{
					switch (res)
					{
					case 0x80:
						return 0;
					case 0x88:
						return 1;
					case 0x90:
						return 2;
					case 0x98:
						return 3;
					default:
						return 0;
					}
				}
				else if (type == SMALL)
				{
					switch (res)
					{
					case 0x10:
						return 2;
					default:
						return 0;
					}
				}
			}
			return res;
		}


		DWORD GetRow()
		{
			return dwPosition & 0xFFFF;
		}

		void SetTileType(TILE_TYPE type)
		{
			dwPosition = (dwPosition & 0x00FFFFFF) | (type & 0xFF) << 24;
		}

		void SetColumn(DWORD value, TILE_POSITION_TRANSFORMATION transformation)
		{
			DWORD r = value;
			if (transformation == TPT_NORMALIZE)
			{
				TILE_TYPE type = GetTileType();
				if (type == MEDIUM)
				{
					switch (value)
					{
					case 0:
						r = 0x80;
						break;
					case 1:
						r = 0x88;
						break;
					case 2:
						r = 0x90;
						break;
					case 3:
						r = 0x98;
						break;
					}
				}
				else if (type == SMALL)
				{
					switch (value)
					{
					case 0:
						r = 0x00;
						break;
					case 1:
						r = 0x10;
						break;
					}
				}
			}
			dwPosition = (dwPosition & 0xFF00FFFF) | ((r & 0xFF) << 16);
		}

		void SetRow(DWORD row)
		{
			dwPosition = (dwPosition & 0xFFFF0000) | (row & 0xFFFF);
		}

		DWORD GetCompareValue()
		{
			TILE_TYPE type = GetTileType();
			DWORD column = GetColumn(TPT_NORMALIZE);
			DWORD row = GetRow();
			return (row << 16) | column;
		}
	private:
		DWORD dwPosition;
	};


	class Tiles
	{
	public:

		// Moves token to appreciated position.
		static void MoveTokenSimple(APPID appId, LPCWSTR tokenID, DWORD position)
		{
			// we play with thread quantum to prevent scheduler from going into StartMenu.dll.
			// if it is there, it immediately processes incoming token notifications, eliminating the possibility of
			// setting token position an easy way.
			// before it could be like that:
			// PinTokenToStart (msg PROCESSED) -> SetTokenPosition (msg NOT PROCESSED)
			// after this fix it is like that:
			// PinTokenToStart (msg NOT PROCESSED) -> SetTokenPosition -> whatever -> MSG PROCESSED.

			HANDLE thread = GetCurrentThread();
			DWORD oldQuantum = CeGetThreadQuantum(thread);
			CeSetThreadQuantum(thread, 0);

			SetTokenPromoted(appId, tokenID, FALSE);
			PMPinTokenToStart(appId, tokenID);
			SetTokenPosition(appId, tokenID, position);

			CeSetThreadQuantum(thread, oldQuantum);

			// let Start Menu process notification
			Sleep(60);
		}

		
		// If there are no promoted tokens, system (only 7.8?)
		// may freeze after startup.
		// So we promote Phone token to fix an issue.
		static void FixTokenAbsence()
		{
			__try
			{
				int count = 0;

				ITokenInfoEnumerator *di = NULL;
				GetAllPromotedTokens(&di);
				ITokenInfo *info = NULL;
				while (di->get_Next(&info) == S_OK)
				{
					count++;
				}
				if (count == 0)
				{
					GUID guid;
					CLSIDFromString(L"{5B04B775-356B-4AA0-AAF8-6491FFEA5611}", &guid);
					IApplicationInfo *info = NULL;
					GetApplicationInfoByProductID(guid, &info);
					if (info)
					{
						APPID appId = info->get_AppID();
						ITokenInfo *tokenInfo = NULL;
						GetDefaultTokenInfo(appId, &tokenInfo);
						if (tokenInfo)
						{
							MoveTokenSimple(appId, tokenInfo->get_TokenID(), (MEDIUM << 24));
							delete (tokenInfo);
						}
						delete(info);
					}
				}
			}	
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		
		static BOOL FillMovableTile(ITokenManager *tokenManager, wchar_t *productID, wchar_t *tokenID, MOVABLE_TILE *mt)
		{
			GUID guid;
			Applications::String2GUID(productID, &guid);
			APPID appId = 0;
			if (tokenManager->GetAppIDFromProductID(guid, &appId) == S_OK)
			{	
				ITokenInfo *tokenInfo = NULL;
				if(tokenID)
					GetTokenInfo(appId, tokenID, &tokenInfo);
				else
					GetDefaultTokenInfo(appId, &tokenInfo);
				if (tokenInfo)
				{
					mt->appId = appId;
					wcscpy(mt->tokenID, tokenInfo->get_TokenID());
					delete(tokenInfo);
					return TRUE;
				}
			}
			return FALSE;
		}

		static void PinTokenAtPosition(ITokenManager *tokenManager, wchar_t *productID, wchar_t *tokenID, DWORD dwPosition)
		{
			MOVABLE_TILE mt;
			FillMovableTile(tokenManager, productID, tokenID, &mt);
			mt.dwPosition = dwPosition;
			Shell::CTilePosition tp;
			tp.SetDirect(dwPosition);
			int shift = tp.GetRow() + 1;
			TILE_TYPE tileType = tp.GetTileType();
			if (tileType == LARGE || tileType == MEDIUM)
				shift++;

			mlist list;
			CollectSortedTokenList(&list);

			for (mlist::reverse_iterator iterator = list.rbegin(); iterator != list.rend(); ++iterator)
			{
				MOVABLE_TILE *m = *iterator;
				MoveTokenSimple(m->appId, m->tokenID, m->dwPosition + shift);
				delete *iterator;
			}

			list.erase(list.begin(), list.end());
			MoveTokenSimple(mt.appId, mt.tokenID, mt.dwPosition);


			Sleep(500);
			FixSpaceBetweenRows(tokenManager);
		}

		static void UnpinToken(ITokenManager *tokenManager, wchar_t *productID, wchar_t *tokenID)
		{
			GUID guid;
			Applications::String2GUID(productID, &guid);
			APPID appId = 0;
			if (tokenManager->GetAppIDFromProductID(guid, &appId) == S_OK)
			{	
				SetTokenPromoted(appId, tokenID, FALSE);
			}
		}
		static void CollectSortedTokenListFromRegistry(mlist *list, ITokenManager *tokenManager)
		{
			HKEY hKey;
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				L"Software\\OEM\\FirstBoot\\Predefined",
				0,
				KEY_READ,
				&hKey) == ERROR_SUCCESS)
			{
				wchar_t keyName[500];
				DWORD keyNameSize = 500;

				DWORD dwIndex = 0;

				DWORD dwType = REG_NONE;
				while (RegEnumValue(hKey, 
					dwIndex++, 
					keyName, 
					&keyNameSize, 
					NULL, 
					&dwType, 
					NULL, 
					NULL) == ERROR_SUCCESS)
				{
					if (dwType == REG_DWORD)
					{

						DWORD dwValue = 0;
						RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"Software\\OEM\\FirstBoot\\Predefined", keyName, &dwValue);
						wchar_t *tokenID = NULL;
						if (wcschr(keyName, L'|'))
						{
							tokenID = wcschr(keyName, L'|');
							*tokenID = NULL;
							tokenID++;
						}
						GUID guid;
						CLSIDFromString(keyName, &guid);
						APPID appId = 0;
						if (tokenManager->GetAppIDFromProductID(guid, &appId) == S_OK)
						{	
							ITokenInfo *tokenInfo = NULL;
							if(tokenID)
								GetTokenInfo(appId, tokenID, &tokenInfo);
							else
								GetDefaultTokenInfo(appId, &tokenInfo);
							if (tokenInfo)
							{
								MOVABLE_TILE *mt = new MOVABLE_TILE;
								mt->appId = appId;
								mt->dwPosition = dwValue;
								wcscpy(mt->tokenID, tokenInfo->get_TokenID());

								bool foundPosition = false;
								int i = 0;
								for (mlist::iterator iterator = list->begin(); iterator != list->end(); ++iterator)
								{
									MOVABLE_TILE *curTile = *iterator;
									Shell::CTilePosition tp1;
									Shell::CTilePosition tp2;
									tp1.SetDirect(curTile->dwPosition);
									tp2.SetDirect(mt->dwPosition);
									int v2 = tp1.GetCompareValue();
									int v1 = tp2.GetCompareValue();
									if (v2 > v1)
									{
										foundPosition = true;
										list->insert(iterator, mt);
										break;
									}
									i++;
								}

								if (!foundPosition)
									list->push_back(mt);
								delete(tokenInfo);
							}
						}
					}
					keyNameSize = 500;
				}
				RegCloseKey(hKey);
			}
		}

		static void CollectSortedTokenList(mlist *list)
		{
			ITokenInfoEnumerator *di = NULL;
			if (GetAllPromotedTokens(&di) == S_OK)
			{
				ITokenInfo *info = NULL;
				while (di->get_Next(&info) == S_OK)
				{
					if (info == NULL)
						break;

					IApplicationInfo *appInfo = NULL;
					if (GetApplicationInfo(info->get_AppID(), &appInfo) == S_OK && appInfo)
					{
						wchar_t prodID[50] = {0};
						StringFromGUID2(*appInfo->get_ProductID(), prodID, 50);
						delete(appInfo);
					}

					MOVABLE_TILE *mt = new MOVABLE_TILE;
					mt->appId = info->get_AppID();
					mt->dwPosition = info->get_Position();
					wcscpy(mt->tokenID, info->get_TokenID());
					bool foundPosition = false;
					int i = 0;
					for (mlist::iterator iterator = list->begin(); iterator != list->end(); ++iterator)
					{
						MOVABLE_TILE *curTile = *iterator;
						Shell::CTilePosition tp1;
						Shell::CTilePosition tp2;
						tp1.SetDirect(curTile->dwPosition);
						tp2.SetDirect(mt->dwPosition);
						int v2 = tp1.GetCompareValue();
						int v1 = tp2.GetCompareValue();
						if (v2 > v1)
						{
							foundPosition = true;
							list->insert(iterator, mt);
							break;
						}
						i++;
					}

					if (!foundPosition)
						list->push_back(mt);
				}
				delete di;
			}
		}

		
		static void FixSpaceBetweenRows(ITokenManager *tokenManager)
		{
			mlist list;
			CollectSortedTokenList(&list);
			if (list.size() > 0)
			{
				// checking last row:
				Shell::CTilePosition tp1;
				tp1.SetDirect((*list.rbegin())->dwPosition);

				DWORD lastRow = tp1.GetRow();
				DWORD lastRowSize = 1;
				for (mlist::reverse_iterator iterator = list.rbegin(); iterator != list.rend(); ++iterator)
				{
					MOVABLE_TILE *mt = *iterator;
					Shell::CTilePosition tp;
					tp.SetDirect(mt->dwPosition);
					DWORD currentRow = tp.GetRow();
					if (currentRow != lastRow)
						break;
					DWORD tileType = tp.GetTileType();
					if ((tileType == LARGE || tileType == MEDIUM) && lastRowSize == 1)
						lastRowSize = 2;
				}

				DWORD absolutelyLastRow = lastRow + lastRowSize;

				TILE_ROW *rows = new TILE_ROW[absolutelyLastRow];
				memset(rows, 0, sizeof(TILE_ROW) * absolutelyLastRow);
				for (mlist::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
				{
					MOVABLE_TILE *mt = *iterator;
					Shell::CTilePosition tp;
					tp.SetDirect(mt->dwPosition);
					DWORD currentRow = tp.GetRow();
					DWORD currentCol = tp.GetColumn(TPT_NORMALIZE);
					TILE_TYPE tileType = tp.GetTileType();

					rows[currentRow].sectors[currentCol] = true;
					if (tileType == MEDIUM || tileType == LARGE)
					{
						rows[currentRow].sectors[currentCol + 1] = true;

						rows[currentRow + 1].sectors[currentCol] = true;
						rows[currentRow + 1].sectors[currentCol + 1] = true;
					}
					if (tileType == LARGE)
					{
						rows[currentRow].sectors[currentCol + 2] = true;
						rows[currentRow].sectors[currentCol + 3] = true;
						rows[currentRow + 1].sectors[currentCol + 2] = true;
						rows[currentRow + 1].sectors[currentCol + 3] = true;
					}
				}
				int shift = 0;
				int firstEmpty = -1;
				// let's check for pure empty rows
				for (DWORD i = 0; i < absolutelyLastRow; ++i)
				{
					if (rows[i].sectors[0] == false &&
						rows[i].sectors[1] == false &&
						rows[i].sectors[2] == false &&
						rows[i].sectors[3] == false)
					{
						if (firstEmpty == -1)
							firstEmpty = i;
					}
					else
					{
						if (firstEmpty != -1)
						{
							int moveFirst = firstEmpty;
							int moveLast = i;

							int tempShift = moveLast - moveFirst;
							// we found it. Let's shift next tiles by -1.
							for (mlist::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
							{
								MOVABLE_TILE *mt = *iterator;
								if ((mt->dwPosition & 0xFF) >= (i - shift))
								{
									MoveTokenSimple(mt->appId, mt->tokenID, mt->dwPosition - tempShift);
									mt->dwPosition -= tempShift;
								}
							}

							shift += tempShift;
							firstEmpty = -1;
						}
					}
				}	
			}
		}

	};
}