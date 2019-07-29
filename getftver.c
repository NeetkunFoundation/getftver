#include <Windows.h>

#include <stdio.h>
#include <stdbool.h>

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


static bool create_libraries(void);

int main(void)
{
    FT_Library library;
    FT_Int major, minor, patch;
    FT_Error error;

    if (!init_libraries()) {
        return EXIT_FAILURE;
    }

    if (FT_FAILURE(error = FT_Init_FreeType(&library))) {
        _tprintf(_T("Error '0x%x' has been occurred while INITIALIZING FreeType library.\n"), error);
        return EXIT_FAILURE;
    }

    FT_Library_Version(library, &major, &minor, &patch);

    _tprintf(_T("FreeType version %d.%d.%d"), major, minor, patch);

    if (FT_FAILURE(error = FT_Done_FreeType(library))) {
        _tprintf(_T("Error '0x%x' has been occurred while DESTROYING FreeType library.\n"), error);
        return EXIT_FAILURE;
    }

    FreeLibrary(hLibrary);
    return EXIT_SUCCESS;
}

static bool init_libraries(void)
{
    DWORD numCharsWritten;
    TCHAR lpBuffer[512];

    hLibrary = LoadLibrary(LIBRARY_NAME);
    if (!hLibrary)
    {
        numCharsWritten = FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            GetLastError(),
            0,
            lpBuffer,
            sizeof(lpBuffer) / sizeof(TCHAR),
            NULL);
        if (!numCharsWritten)
        {
            _tprintf(_T("FormatMessage is exited with error code 0x%x\n"), GetLastError());
            return false;
        }
        _tprintf_s(_T("Could not load library '") LIBRARY_NAME _T("': %s\n"), lpBuffer, numCharsWritten);
    }

    FT_Init_FreeType = (PROC_FT_Init_FreeType)GetProcAddress(hLibrary, "FT_Init_FreeType");
    FT_Done_FreeType = (PROC_FT_Done_FreeType)GetProcAddress(hLibrary, "FT_Done_FreeType");
    FT_Library_Version = (PROC_FT_Library_Version)GetProcAddress(hLibrary, "FT_Library_Version");

    return true;
}
