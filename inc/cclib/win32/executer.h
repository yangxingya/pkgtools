//!
/// brief: executer.h define execute a cmd line.
///

#if !defined(cclib_win32_executer_h_)
#define cclib_win32_executer_h_

#include <windows.h>
#include <string.h>
#include <string>
#include "../types.h"
#include "autores.h"

CCLIB_NAMESPACE_BEGIN
WIN32_NAMESPACE_BEGIN

bool execute(
    uint32_t *retcode,  /// you must wait for process exit, then ret code will used.
    std::string const& app, std::string const& cmd, 
    bool inherit_handle = false, bool hide_wnd = true, bool waited = true)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);
    if (hide_wnd) {
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }

    char const* capp = app.empty() ? 0 : app.c_str();

    char *cmdline = (char *)malloc(cmd.length() + 1);
    if (cmdline == 0)
        return false;

#pragma warning(push)
#pragma warning(disable:4996)  /// disable strcpy warning.
    strcpy(cmdline, cmd.c_str());
#pragma warning(pop)

    if (!CreateProcessA(capp, cmdline, 0, 0, inherit_handle, 0, 0, 0, &si, &pi)) 
        return false;
	
    
    ScopeHandle(pi.hProcess);
    ScopeHandle(pi.hThread);

    if (waited) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        if (retcode) 
            GetExitCodeProcess(pi.hProcess, (LPDWORD)retcode);
    }

    return true;
}

bool execute(
    uint32_t *retcode,  /// you must wait for process exit, then ret code will used.
    std::string const& app, std::string const& cmd,
    std::string const& optfile, bool hide_wnd = true, bool waited = true)
{
    HANDLE rfile = 0;
    HANDLE wpipe = 0;
    SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, true};   

    if (!CreatePipe(&rfile, &wpipe, &sa, 0))
        return false;

    ScopeHandle(rfile);
    ScopeHandle(wpipe);

    HANDLE hstdout = CreateFileA(
                        optfile.c_str(), 
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        &sa,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if (hstdout == INVALID_HANDLE_VALUE) 
        return false;

    ScopeHandle(hstdout);

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};

    GetStartupInfoA(&si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput  = hstdout;
    si.hStdError   = hstdout;

    if (hide_wnd) {
        si.dwFlags |= STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }

    char const* capp = app.empty() ? 0 : app.c_str();
    char *cmdline = (char *)malloc(cmd.length() + 1);
    if (cmdline == 0)
        return false;

#pragma warning(push)
#pragma warning(disable:4996)  /// disable strcpy warning.
    strcpy(cmdline, cmd.c_str());
#pragma warning(pop)

    if (!CreateProcessA(capp, cmdline, 0, 0, true, 0, 0, 0, &si, &pi)) 
        return false;
    
    ScopeHandle(pi.hProcess);
    ScopeHandle(pi.hThread);

    if (waited) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        if (retcode) 
            GetExitCodeProcess(pi.hProcess, (LPDWORD)retcode);
    }

    return true;
}

#if 0

BOOL WaitAppExecuteWithRedirectOutput(LPCTSTR lpAppName, LPTSTR lpCmdLine, LPCTSTR lpStdOutFile)
{
    SECURITY_ATTRIBUTES  sa = {sizeof(sa), NULL, TRUE};   
    HANDLE stdout = NULL;

    BOOL bResult = FALSE;
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    // pipe.
    HANDLE rfile  = NULL;
    HANDLE wpipe = NULL;

    CreatePipe(&rfile, &wpipe, &sa, 0);

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    // create file.
    stdout = CreateFile(
        lpStdOutFile, 
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (stdout == INVALID_HANDLE_VALUE) { 
        printf("Could not create file (error %d)\n", GetLastError());
        goto exit;
    }

    ZeroMemory(&si, sizeof(si));
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.stdoutput  = stdout;
    si.hStdError   = stdout;

    ZeroMemory(&pi, sizeof(pi));

    bResult = CreateProcess(	
        lpAppName,	        // No module name (use command line)
        lpCmdLine,		    // Command line
        NULL,				// Process handle not inheritable
        NULL,				// Thread handle not inheritable
        TRUE,				// Set handle inheritance to TRUE
        0,					// No creation flags
        NULL,				// Use parent's environment block
        NULL,				// Use parent's starting directory 
        &si,				// Pointer to STARTUPINFO structure
        &pi );				// Pointer to PROCESS_INFORMATION structure

    if (!bResult) {
        goto exit;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

exit:

    // Close process and thread handles. 
    if (pi.hProcess)
        CloseHandle(pi.hProcess);
    if (pi.hThread)
        CloseHandle(pi.hThread);

    // close file handle.
    if (stdout)
        CloseHandle(stdout);

    // close pipe handle.
    if (rfile)
        CloseHandle(rfile);
    if (wpipe)
        CloseHandle(wpipe);

    return bResult;
}
#endif

WIN32_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_win32_executer_h_