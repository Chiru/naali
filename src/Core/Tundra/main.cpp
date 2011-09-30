// For conditions of distribution and use, see copyright notice in license.txt

#include "DebugOperatorNew.h"

#include "Application.h"
#include "Framework.h"
#include "LoggingFunctions.h"

#include <QDir>

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#endif
#ifdef WINDOWS_APP
#include <tchar.h>
#include <io.h>
#include <iostream>
#include <fcntl.h>
#include <conio.h>
#endif

#if defined(_MSC_VER) && defined(MEMORY_LEAK_CHECK)
// for reporting memory leaks upon debug exit
#include <crtdbg.h>
#endif

#if defined(_MSC_VER) && defined(_DMEMDUMP)
// For generating minidump
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <strsafe.h>
#pragma warning(pop)
#endif

#include "MemoryLeakCheck.h"

int run(int argc, char **argv);

#if defined(_MSC_VER) && defined(_DMEMDUMP)
int generate_dump(EXCEPTION_POINTERS* pExceptionPointers);
#endif

int main(int argc, char **argv)
{
    int return_value = EXIT_SUCCESS;

    // set up a debug flag for memory leaks. Output the results to file when the app exits.
    // Note that this file is written to the same directory where the executable resides,
    // so you can only use this in a development version where you have write access to
    // that directory.
#if defined(_MSC_VER) && defined(MEMORY_LEAK_CHECK) && defined(_DEBUG)
    int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpDbgFlag);

    HANDLE hLogFile = CreateFileW(L"fullmemoryleaklog.txt", GENERIC_WRITE, 
      FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

#if defined(_MSC_VER) && defined(_DMEMDUMP)
    __try
    {
#endif
        return_value = run(argc, argv);

#if defined(_MSC_VER) && defined(_DMEMDUMP)
    }
    __except(generate_dump(GetExceptionInformation()))
    {
    }
#endif

#if defined(_MSC_VER) && defined(MEMORY_LEAK_CHECK) && defined(_DEBUG)
    if (hLogFile != INVALID_HANDLE_VALUE)
    {
       _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
       _CrtSetReportFile(_CRT_WARN, hLogFile);
       _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
       _CrtSetReportFile(_CRT_ERROR, hLogFile);
       _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
       _CrtSetReportFile(_CRT_ASSERT, hLogFile);
    }
#endif

    // Note: We cannot close the file handle manually here. Have to let the OS close it
    // after it has printed out the list of leaks to the file.
    //CloseHandle(hLogFile);

    return return_value;
}

int run(int argc, char **argv)
{
    int return_value = EXIT_SUCCESS;

    // Initilizing prints
    LogInfo("Starting up Tundra.");
    LogInfo("* Working directory: " + QDir::currentPath());

    // Create application object
#if !defined(_DEBUG) || !defined (_MSC_VER)
    try
#endif
    {
        Framework* fw = new Framework(argc, argv);
        fw->Go();
        delete fw;
    }
#if !defined(_DEBUG) || !defined (_MSC_VER)
    catch(std::exception& e)
    {
        Application::Message("An exception has occurred!", e.what());
#if defined(_DEBUG)
        throw;
#else
        return_value = EXIT_FAILURE;
#endif
    }
#endif

    return return_value;
}

#if defined(_MSC_VER) && defined(WINDOWS_APP)
int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int /*nShowCmd*/)
{
    lpCmdLine = GetCommandLine();
    std::wstring cmdLine(lpCmdLine);
    // If trying to run Windows GUI application in headless mode, we must set up the console in order to be able to proceed.
    if (cmdLine.find(__T("--headless")) != std::wstring::npos)
    {
        // Code below adapted from http://dslweb.nwnexus.com/~ast/dload/guicon.htm
        BOOL ret = AllocConsole();
        if (!ret)
            return EXIT_FAILURE;

        // Prepare stdin, stdout and stderr.
        long hStd =(long)GetStdHandle(STD_INPUT_HANDLE);
        int hCrt = _open_osfhandle(hStd, _O_TEXT);
        FILE *hf = _fdopen(hCrt, "r+");
        setvbuf(hf,0,_IONBF,0);
        *stdin = *hf;

        hStd =(long)GetStdHandle(STD_OUTPUT_HANDLE);
        hCrt = _open_osfhandle(hStd, _O_TEXT);
        hf = _fdopen(hCrt, "w+");
        setvbuf(hf, 0, _IONBF, 0);
        *stdout = *hf;

        hStd =(long)GetStdHandle(STD_ERROR_HANDLE);
        hCrt = _open_osfhandle(hStd, _O_TEXT);
        hf = _fdopen(hCrt, "w+");
        setvbuf(hf, 0, _IONBF, 0);
        *stderr = *hf;

        // Make C++ IO streams cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well.
        std::ios::sync_with_stdio();

        printf("Tried to start Tundra as a Windows GUI application in headless mode: console window created.\n");
    }

    // Parse the Windows command line.
    std::vector<std::wstring> arguments;
    unsigned i;
    unsigned cmdStart = 0;
    unsigned cmdEnd = 0;
    bool cmd = false;
    bool quote = false;
    for(i = 0; i < cmdLine.length(); ++i)
    {
        if (cmdLine[i] == '\"')
            quote = !quote;
        if ((cmdLine[i] == ' ') && (!quote))
        {
            if (cmd)
            {
                cmd = false;
                cmdEnd = i;
                arguments.push_back(cmdLine.substr(cmdStart, cmdEnd-cmdStart));
            }
        }
        else
        {
            if (!cmd)
            {
               cmd = true;
               cmdStart = i;
            }
        }
    }
    if (cmd)
        arguments.push_back(cmdLine.substr(cmdStart, i-cmdStart));

    std::vector<const wchar_t*> argv;
    for(size_t i = 0; i < arguments.size(); ++i)
        argv.push_back(arguments[i].c_str());
    
    if (argv.size())
        return main(argv.size(), (char**)&argv[0]);
    else
        return main(0, 0);
}
#endif

#if defined(_MSC_VER) && defined(_DMEMDUMP)
int generate_dump(EXCEPTION_POINTERS* pExceptionPointers)
{
    // Add a hardcoded check to guarantee we only write a dump file of the first crash exception that is received.
    // Sometimes a crash is so bad that writing the dump below causes another exception to occur, in which case
    // this function would be recursively called, spawning tons of error dialogs to the user.
    static bool dumpGenerated = false;
    if (dumpGenerated)
    {
        printf("WARNING: Not generating another dump, one has been generated already!\n");
        return 0;
    }
    dumpGenerated = true;

    BOOL bMiniDumpSuccessful;
    WCHAR szPath[MAX_PATH]; 
    WCHAR szFileName[MAX_PATH];

    // Can't use Application for application name and version,
    // since it might have not been initialized yet, or it might have caused 
    // the exception in the first place
    WCHAR* szAppName = L"realXtend";
    WCHAR* szVersion = L"Tundra_v2.0";
    DWORD dwBufferSize = MAX_PATH;
    HANDLE hDumpFile;
    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

    GetLocalTime( &stLocalTime );
    GetTempPathW( dwBufferSize, szPath );

    StringCchPrintf( szFileName, MAX_PATH, L"%s%s", szPath, szAppName );
    CreateDirectoryW( szFileName, 0 );
    StringCchPrintf( szFileName, MAX_PATH, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
               szPath, szAppName, szVersion, 
               stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
               GetCurrentProcessId(), GetCurrentThreadId());

    hDumpFile = CreateFileW(szFileName, GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionPointers;
    ExpParam.ClientPointers = TRUE;

    bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
                    hDumpFile, MiniDumpWithDataSegs, &ExpParam, 0, 0);
   
    std::wstring message(L"Program ");
    message += szAppName;
    message += L" encountered an unexpected error.\n\nCrashdump was saved to location:\n";
    message += szFileName;

    if (bMiniDumpSuccessful)
        Application::Message(L"Minidump generated!", message);
    else
        Application::Message(szAppName, L"Unexpected error was encountered while generating minidump!");

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif
