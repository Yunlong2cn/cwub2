
typedef struct WINTRUST_FILE_INFO_
{
	DWORD           cbStruct;                   // = sizeof(WINTRUST_FILE_INFO)

	LPCWSTR         pcwszFilePath;              // required, file name to be verified
	HANDLE          hFile;                      // optional, open handle to pcwszFilePath

	GUID            *pgKnownSubject;            // optional: fill if the subject type is known.

} WINTRUST_FILE_INFO, *PWINTRUST_FILE_INFO;

#define WINTRUST_ACTION_GENERIC_VERIFY_V2                       \
            { 0xaac56b,                                         \
              0xcd44,                                           \
              0x11d0,                                           \
              { 0x8c, 0xc2, 0x0, 0xc0, 0x4f, 0xc2, 0x95, 0xee } \
            }
typedef struct _WINTRUST_DATA
{
	DWORD           cbStruct;                   // = sizeof(WINTRUST_DATA)

	LPVOID          pPolicyCallbackData;        // optional: used to pass data between the app and policy
	LPVOID          pSIPClientData;             // optional: used to pass data between the app and SIP.

	DWORD           dwUIChoice;                 // required: UI choice.  One of the following.
#                       define      WTD_UI_ALL              1
#                       define      WTD_UI_NONE             2
#                       define      WTD_UI_NOBAD            3
#                       define      WTD_UI_NOGOOD           4

	DWORD           fdwRevocationChecks;        // required: certificate revocation check options
#                       define      WTD_REVOKE_NONE         0x00000000
#                       define      WTD_REVOKE_WHOLECHAIN   0x00000001

	DWORD           dwUnionChoice;              // required: which structure is being passed in?
#                       define      WTD_CHOICE_FILE         1
#                       define      WTD_CHOICE_CATALOG      2
#                       define      WTD_CHOICE_BLOB         3
#                       define      WTD_CHOICE_SIGNER       4
#                       define      WTD_CHOICE_CERT         5
	union
	{
		struct WINTRUST_FILE_INFO_      *pFile;         // individual file
		struct WINTRUST_CATALOG_INFO_   *pCatalog;      // member of a Catalog File
		struct WINTRUST_BLOB_INFO_      *pBlob;         // memory blob
		struct WINTRUST_SGNR_INFO_      *pSgnr;         // signer structure only
		struct WINTRUST_CERT_INFO_      *pCert;
	};

	DWORD           dwStateAction;                      // optional (Catalog File Processing)
#                       define      WTD_STATEACTION_IGNORE           0x00000000
#                       define      WTD_STATEACTION_VERIFY           0x00000001
#                       define      WTD_STATEACTION_CLOSE            0x00000002
#                       define      WTD_STATEACTION_AUTO_CACHE       0x00000003
#                       define      WTD_STATEACTION_AUTO_CACHE_FLUSH 0x00000004

	HANDLE          hWVTStateData;                      // optional (Catalog File Processing)

	WCHAR           *pwszURLReference;          // optional: (future) used to determine zone.

	DWORD           dwProvFlags;
#       define WTD_PROV_FLAGS_MASK                      0x0000FFFF
#       define WTD_USE_IE4_TRUST_FLAG                   0x00000001
#       define WTD_NO_IE4_CHAIN_FLAG                    0x00000002
#       define WTD_NO_POLICY_USAGE_FLAG                 0x00000004
#       define WTD_REVOCATION_CHECK_NONE                0x00000010
#       define WTD_REVOCATION_CHECK_END_CERT            0x00000020
#       define WTD_REVOCATION_CHECK_CHAIN               0x00000040
#       define WTD_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT  0x00000080
#       define WTD_SAFER_FLAG                           0x00000100
#       define WTD_HASH_ONLY_FLAG                       0x00000200
#       define WTD_USE_DEFAULT_OSVER_CHECK              0x00000400
#       define WTD_LIFETIME_SIGNING_FLAG                0x00000800
#       define WTD_CACHE_ONLY_URL_RETRIEVAL             0x00001000 // affects CRL retrieval and AIA retrieval

	DWORD           dwUIContext;
#       define WTD_UICONTEXT_EXECUTE                    0
#       define WTD_UICONTEXT_INSTALL                    1

} WINTRUST_DATA, *PWINTRUST_DATA;

//extern LONG WINAPI WinVerifyTrust(HWND hwnd, GUID *pgActionID,LPVOID pWVTData);

BOOL VerifySignature(LPCWSTR pwszSourceFile)
{
	LONG lStatus;
    
    static wchar_t temp[1024];
    wcscpy(temp,tzDicPath);
    wcscat(temp,L"\\");
    wcscat(temp,pwszSourceFile);
    //OutputDebugString(temp);
    
	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(WINTRUST_FILE_INFO));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = temp;
	FileData.hFile = NULL;
	//FileData.pgKnownSubject = NULL;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;

	memset(&WinTrustData, 0, sizeof(WINTRUST_DATA));
	WinTrustData.cbStruct = sizeof(WINTRUST_DATA);
	WinTrustData.pPolicyCallbackData = NULL;
	WinTrustData.pSIPClientData = NULL;
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	WinTrustData.dwStateAction = WTD_STATEACTION_IGNORE;
	//WinTrustData.hWVTStateData = NULL;
	//WinTrustData.pwszURLReference = NULL;
	WinTrustData.dwProvFlags = 0;
	WinTrustData.pFile = &FileData;

	typedef LONG (WINAPI* pWinVerifyTrust)(HWND hWnd, GUID* pgActionID, WINTRUST_DATA* pWinTrustData);
	pWinVerifyTrust WinVerifyTrust = NULL;

	HINSTANCE hWinTrust = LoadLibrary(L"wintrust.dll");
	if ( !hWinTrust )
		return TRUE;

	WinVerifyTrust = (pWinVerifyTrust)GetProcAddress( hWinTrust, "WinVerifyTrust");
	if (WinVerifyTrust == NULL )
	{
		FreeLibrary(hWinTrust);
		return TRUE;
	}

	lStatus = WinVerifyTrust(
				NULL,
				&WVTPolicyGUID,
				&WinTrustData);

	FreeLibrary(hWinTrust);
	return lStatus == ERROR_SUCCESS;
}