
#include "regdef.h"
#include "regdata.h"

RegData<RegSZ> regdata("test RegSZ");

std::string data = regdata;

regshot::regshot()
{
    doshot();
}

regshot::regshot(std::string const &file)
{
    
}

void regshot::doshot()
{
    
}

#if 0

LPKEYCONTENT GetRegistrySnap(HKEY hRegKey, LPTSTR lpszRegKeyName, LPKEYCONTENT lpFatherKC, LPKEYCONTENT *lplpCaller)
{
    LPKEYCONTENT lpKC;
    DWORD cSubKeys;
    DWORD cchMaxSubKeyName;

    // Process registry key itself, then key values with data, then sub keys (see msdn.microsoft.com/en-us/library/windows/desktop/ms724256.aspx)

    // Extra local block to reduce stack usage due to recursive calls
    {
        LPTSTR lpszFullName;
        DWORD cValues;
        DWORD cchMaxValueName;
        DWORD cbMaxValueData;

        // Create new key content
        // put in a separate var for later use
        lpKC = MYALLOC0(sizeof(KEYCONTENT));

        // Set father of current key
        lpKC->lpFatherKC = lpFatherKC;

        // Set key name
        lpKC->lpszKeyName = lpszRegKeyName;
        lpKC->cchKeyName = _tcslen(lpKC->lpszKeyName);

        // Check if key is to be excluded
        if (NULL != lprgszRegSkipStrings[0]) {  // only if there is something to exclude
            if ((NULL != lpKC->lpszKeyName) && (IsInSkipList(lpKC->lpszKeyName, lprgszRegSkipStrings))) {
                FreeAllKeyContents(lpKC);
                return NULL;
            }

            lpszFullName = GetWholeKeyName(lpKC, FALSE);
            if (IsInSkipList(lpszFullName, lprgszRegSkipStrings)) {
                MYFREE(lpszFullName);
                FreeAllKeyContents(lpKC);
                return NULL;
            }
            MYFREE(lpszFullName);
        }

        // Examine key for values and sub keys, get counts and also maximum lengths of names plus value data
        nErrNo = RegQueryInfoKey(
                     hRegKey,
                     NULL,
                     NULL,
                     NULL,
                     &cSubKeys,
                     &cchMaxSubKeyName,  // in TCHARs *not* incl. NULL char
                     NULL,
                     &cValues,
                     &cchMaxValueName,   // in TCHARs *not* incl. NULL char
                     &cbMaxValueData,
                     NULL,
                     NULL
                 );
        if (ERROR_SUCCESS != nErrNo) {
            // TODO: process/protocol issue in some way, do not silently ignore it (at least in Debug builds)
            FreeAllKeyContents(lpKC);
            return NULL;
        }

        // Increase key count
        nGettingKey++;

        // Write pointer to current key into caller's pointer
        if (NULL != lplpCaller) {
            *lplpCaller = lpKC;
        }

        // Copy the registry values of the current key
        if (0 < cValues) {
            LPVALUECONTENT lpVC;
            LPVALUECONTENT *lplpVCPrev;
            DWORD i;
            DWORD cchValueName;
            DWORD nValueType;
            DWORD cbValueData;
#ifdef DEBUGLOG
            LPTSTR lpszDebugMsg;
#endif

            // Account for NULL char
            if (0 < cchMaxValueName) {
                cchMaxValueName++;
            }

            // Get buffer for maximum value name length
            nSourceSize = cchMaxValueName * sizeof(TCHAR);
            nStringBufferSize = AdjustBuffer(&lpStringBuffer, nStringBufferSize, nSourceSize, REGSHOT_BUFFER_BLOCK_BYTES);

            // Get buffer for maximum value data length
            nSourceSize = cbMaxValueData;
            nDataBufferSize = AdjustBuffer(&lpDataBuffer, nDataBufferSize, nSourceSize, REGSHOT_BUFFER_BLOCK_BYTES);

            // Get registry key values
            lplpVCPrev = &lpKC->lpFirstVC;
            for (i = 0; ; i++) {
                // Enumerate value
                cchValueName = (DWORD)nStringBufferSize;
                cbValueData = (DWORD)nDataBufferSize;
                nErrNo = RegEnumValue(hRegKey, i,
                                      lpStringBuffer,
                                      &cchValueName,   // in TCHARs; in *including* and out *excluding* NULL char
                                      NULL,
                                      &nValueType,
                                      lpDataBuffer,
                                      &cbValueData);
                if (ERROR_NO_MORE_ITEMS == nErrNo) {
                    break;
                }
                if (ERROR_SUCCESS != nErrNo) {
                    // TODO: process/protocol issue in some way, do not silently ignore it (at least in Debug builds)
                    continue;
                }
                lpStringBuffer[cchValueName] = (TCHAR)'\0';  // safety NULL char

#ifdef DEBUGLOG
                DebugLog(lpszDebugTryToGetValueLog, TEXT("trying: "), FALSE);
                DebugLog(lpszDebugTryToGetValueLog, lpStringBuffer, TRUE);
#endif

                // Create new value content
                // put in a separate var for later use
                lpVC = MYALLOC0(sizeof(VALUECONTENT));

                // Set father key of current value to current key
                lpVC->lpFatherKC = lpKC;

                // Copy value name
                if (0 < cchValueName) {
                    lpVC->lpszValueName = MYALLOC((cchValueName + 1) * sizeof(TCHAR));
                    _tcscpy(lpVC->lpszValueName, lpStringBuffer);
                    lpVC->cchValueName = _tcslen(lpVC->lpszValueName);
                }

                // Check if value is to be excluded
                if (NULL != lprgszRegSkipStrings[0]) {  // only if there is something to exclude
                    if ((NULL != lpVC->lpszValueName) && (IsInSkipList(lpVC->lpszValueName, lprgszRegSkipStrings))) {
                        FreeAllValueContents(lpVC);
                        continue;  // ignore this entry and continue with next brother value
                    }

                    lpszFullName = GetWholeValueName(lpVC, FALSE);
                    if (IsInSkipList(lpszFullName, lprgszRegSkipStrings)) {
                        MYFREE(lpszFullName);
                        FreeAllValueContents(lpVC);
                        continue;  // ignore this entry and continue with next brother value
                    }
                    MYFREE(lpszFullName);
                }

                // Increase value count
                nGettingValue++;

                // Write pointer to current value into previous value's next value pointer
                if (NULL != lplpVCPrev) {
                    *lplpVCPrev = lpVC;
                }
                lplpVCPrev = &lpVC->lpBrotherVC;

                // Copy value meta data
                lpVC->nTypeCode = nValueType;
                lpVC->cbData = cbValueData;

                // Copy value data
                if (0 < cbValueData) {  // otherwise leave it NULL
                    lpVC->lpValueData = MYALLOC(cbValueData);
                    CopyMemory(lpVC->lpValueData, lpDataBuffer, cbValueData);
                }

#ifdef DEBUGLOG
                lpszDebugMsg = MYALLOC0(REGSHOT_DEBUG_MESSAGE_LENGTH * sizeof(TCHAR));
                _sntprintf(lpszDebugMsg, REGSHOT_DEBUG_MESSAGE_LENGTH, TEXT("LGVN:%08d LGVD:%08d VN:%08d VD:%08d\0"), cchMaxValueName, cbMaxValueData, cchValueName, cbValueData);
                lpszDebugMsg[REGSHOT_DEBUG_MESSAGE_LENGTH - 1] = (TCHAR)'\0';  // safety NULL char
                DebugLog(lpszDebugValueNameDataLog, lpszDebugMsg, TRUE);
                MYFREE(lpszDebugMsg);

                lpszDebugMsg = GetWholeValueName(lpVC, FALSE);
                DebugLog(lpszDebugValueNameDataLog, lpszDebugMsg, FALSE);
                MYFREE(lpszDebugMsg);

                lpszDebugMsg = GetWholeValueData(lpVC);
                DebugLog(lpszDebugValueNameDataLog, lpszDebugMsg, TRUE);
                MYFREE(lpszDebugMsg);
#endif
            }
        }
    }  // End of extra local block

    // Update counters display
    nGettingTime = GetTickCount();
    if (REFRESHINTERVAL < (nGettingTime - nBASETIME1)) {
        UpdateCounters(asLangTexts[iszTextKey].lpszText, asLangTexts[iszTextValue].lpszText, nGettingKey, nGettingValue);
    }

    // Process sub keys
    if (0 < cSubKeys) {
        LPKEYCONTENT lpKCSub;
        LPKEYCONTENT *lplpKCPrev;
        DWORD i;
        LPTSTR lpszRegSubKeyName;
        HKEY hRegSubKey;

        // Account for NULL char
        if (0 < cchMaxSubKeyName) {
            cchMaxSubKeyName++;
        }

        // Get buffer for maximum sub key name length
        nSourceSize = cchMaxSubKeyName * sizeof(TCHAR);
        nStringBufferSize = AdjustBuffer(&lpStringBuffer, nStringBufferSize, nSourceSize, REGSHOT_BUFFER_BLOCK_BYTES);

        // Get registry sub keys
        lplpKCPrev = &lpKC->lpFirstSubKC;
        for (i = 0; ; i++) {
            // Extra local block to reduce stack usage due to recursive calls
            {
                DWORD cchSubKeyName;
#ifdef DEBUGLOG
                LPTSTR lpszDebugMsg;
#endif

                // Enumerate sub key
                cchSubKeyName = (DWORD)nStringBufferSize;
                nErrNo = RegEnumKeyEx(hRegKey, i,
                                      lpStringBuffer,
                                      &cchSubKeyName,  // in TCHARs; in *including* and out *excluding* NULL char
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL);
                if (ERROR_NO_MORE_ITEMS == nErrNo) {
                    break;
                }
                if (ERROR_SUCCESS != nErrNo) {
                    // TODO: process/protocol issue in some way, do not silently ignore it (at least in Debug builds)
                    continue;
                }
                lpStringBuffer[cchSubKeyName] = (TCHAR)'\0';  // safety NULL char

                // Copy sub key name
                lpszRegSubKeyName = NULL;
                if (0 < cchSubKeyName) {
                    lpszRegSubKeyName = MYALLOC((cchSubKeyName + 1) * sizeof(TCHAR));
                    _tcscpy(lpszRegSubKeyName, lpStringBuffer);
                }

#ifdef DEBUGLOG
                lpszDebugMsg = MYALLOC0(REGSHOT_DEBUG_MESSAGE_LENGTH * sizeof(TCHAR));
                _sntprintf(lpszDebugMsg, REGSHOT_DEBUG_MESSAGE_LENGTH, TEXT("LGKN:%08d KN:%08d\0"), cchMaxSubKeyName, cchSubKeyName);
                lpszDebugMsg[REGSHOT_DEBUG_MESSAGE_LENGTH - 1] = (TCHAR)'\0';  // safety NULL char
                DebugLog(lpszDebugKeyLog, lpszDebugMsg, TRUE);
                MYFREE(lpszDebugMsg);

                lpszDebugMsg = GetWholeKeyName(lpKC, FALSE);
                DebugLog(lpszDebugKeyLog, lpszDebugMsg, FALSE);
                MYFREE(lpszDebugMsg);

                DebugLog(lpszDebugKeyLog, TEXT("\\"), FALSE);
                DebugLog(lpszDebugKeyLog, lpszRegSubKeyName, TRUE);
#endif
            }  // End of extra local block

            nErrNo = RegOpenKeyEx(hRegKey, lpszRegSubKeyName, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hRegSubKey);
            if (ERROR_SUCCESS != nErrNo) {
                // TODO: process/protocol issue in some way, do not silently ignore it (at least in Debug builds)
                //       e.g. when ERROR_ACCESS_DENIED then at least add key itself to the list
                MYFREE(lpszRegSubKeyName);
                continue;
            }

            lpKCSub = GetRegistrySnap(hRegSubKey, lpszRegSubKeyName, lpKC, lplpKCPrev);
            RegCloseKey(hRegSubKey);

            if (NULL != lpKCSub) {
                lplpKCPrev = &lpKCSub->lpBrotherKC;
            }
        }
    }

    return lpKC;
}

#endif