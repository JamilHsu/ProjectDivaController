// Build this using the following command in x64 Native Tools Command Prompt for VS
// cl mod.c /LD /O2 /GS- /Gy /Zl /link /OUT:"Run ProjectDivaController_exe.dll" /NODEFAULTLIB /NOENTRY /NOIMPLIB /OPT:REF /OPT:ICF /MERGE:.rdata=.text /MERGE:.pdata=.text /SUBSYSTEM:WINDOWS /RELEASE kernel32.lib && del mod.obj && del mod.exp

// A mini mod to automatically execute the .exe when the game starts.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma function(memset)
void* memset(void* dest, int c, size_t count)
{
    char* bytes = (char*)dest;
    while (count--)
    {
        *bytes++ = (char)c;
    }
    return dest;
}
__declspec(dllexport) void __stdcall Init(void)
{
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION ji = { 0 };

    HANDLE hJob = CreateJobObjectW(NULL, NULL);
    if (!hJob) return;

    ji.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    SetInformationJobObject(
        hJob,
        JobObjectExtendedLimitInformation,
        &ji,
        sizeof(ji));

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    if (!CreateProcessW(
        L"ProjectDivaController.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED | CREATE_NEW_CONSOLE ,
        NULL,
        NULL,
        &si,
        &pi))
        return;

    AssignProcessToJobObject(hJob, pi.hProcess);
    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}