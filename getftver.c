#include <Windows.h>

#include <stdio.h>
#include <varargs.h>

#include <tchar.h>

/* standard type definitions */
typedef void *FT_Library;
typedef signed int FT_Int;
typedef int FT_Error;

/* library procedures */
typedef FT_Error (*PROC_FT_Init_FreeType)(FT_Library *);
typedef FT_Error (*PROC_FT_Done_FreeType)(FT_Library);
typedef void (*PROC_FT_Library_Version)(FT_Library, FT_Int *, FT_Int *, FT_Int *);

/* macros for FT_Error */
#define FT_FAILURE(E) ((E) != 0)
#define FT_SUCCESS(E) ((E) == 0)

#define LIBRARY_NAME TEXT("freetype.dll")

static PROC_FT_Init_FreeType FT_Init_FreeType;
static PROC_FT_Done_FreeType FT_Done_FreeType;
static PROC_FT_Library_Version FT_Library_Version;

static HMODULE hLibrary;

static BOOL PrintError(DWORD, LPCTSTR);
static BOOL InitLibrary(VOID);

int main(void)
{
    FT_Library library;
    FT_Int major, minor, patch;
    FT_Error error;

    if (!InitLibrary())
    {
        return EXIT_FAILURE;
    }

    if (FT_FAILURE(error = FT_Init_FreeType(&library)))
    {
        _tprintf(_T("Error '0x%x' has been occurred while INITIALIZING FreeType library.\n"), error);
        return EXIT_FAILURE;
    }

    FT_Library_Version(library, &major, &minor, &patch);

    _tprintf(_T("FreeType version %d.%d.%d"), major, minor, patch);

    if (FT_FAILURE(error = FT_Done_FreeType(library)))
    {
        _tprintf(_T("Error '0x%x' has been occurred while DESTROYING FreeType library.\n"), error);
        return EXIT_FAILURE;
    }

    FreeLibrary(hLibrary);
    return EXIT_SUCCESS;
}

static BOOL PrintError(DWORD dwError, LPCTSTR lpDetails, size_t szDetailsLength)
{
    DWORD dwWritten;
    TCHAR lpBuffer[512];
    dwWritten = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwError,
        0,
        lpBuffer,
        sizeof(lpBuffer) / sizeof(TCHAR),
        NULL);

    if (!dwWritten)
    {
        _tprintf(_T("FormatMessage is exited with error code 0x%x\n"), GetLastError());
        return FALSE;
    }

    _tprintf_s(_T("%s%s"), lpDetails, szDetailsLength, lpBuffer, dwWritten);
    return FALSE;
}

static BOOL init_libraries(void)
{
    static LPCSTR lpProcNames[3] = {
        "FT_Init_FreeType",
        "FT_Done_FreeType",
        "FT_Library_Version",
    };
    LPCSTR lpName;
    FARPROC lpProc;
    SIZE_T i;

    hLibrary = LoadLibrary(LIBRARY_NAME);
    if (!hLibrary)
    {
        return PrintError(GetLastError(), _T("Could not load library '") LIBRARY_NAME _T("': "));
    }

    for (i = 0; i < 3; ++i)
    {
        FARPROC lpProc = GetProcAddress(hLibrary, lpName);
        if (!lpProc)
        {
            TCHAR lpDetails[256];
            _stprintf_s(lpDetails, 255, _T("Function '%s' is not available: "), lpName, strlen(lpName));
            return PrintError(GetLastError(), lpDetails, _tcslen(lpDetails));
        }
        switch (i)
        {
        case 0:
            FT_Init_FreeType = lpProc;
            break;
        case 1:
            FT_Done_FreeType = lpProc;
            break;
        case 2:
            FT_Library_Version = lpProc;
            break;
        }
    }

    return TRUE;
}
