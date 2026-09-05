//

#include <windows.h>
#include <iostream>
#include <AclAPI.h>
#include <string>
#include <fcntl.h>
#include <io.h>

void SetDirectoryWritePermission(const std::wstring& dirPath, bool readOnly)
{
    DWORD attributes = GetFileAttributesW(dirPath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        std::wcerr << L"Folder was not found or path is not a directory." << std::endl;
        return;
    }

    HANDLE tokenHandle = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
    {
        std::wcerr << L"Failed to obtain the process token.: " << GetLastError() << std::endl;
        return;
    }

    DWORD tokenInfoLength = 0;
    GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &tokenInfoLength);

    PTOKEN_USER tokenUser = (PTOKEN_USER)malloc(tokenInfoLength);
    if (!GetTokenInformation(tokenHandle, TokenUser, tokenUser, tokenInfoLength, &tokenInfoLength))
    {
        std::wcerr << L"Error while retrieving token information: " << GetLastError() << std::endl;
        CloseHandle(tokenHandle);
        free(tokenUser);
        return;
    }

    PACL pOldDACL = nullptr, pNewDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    DWORD dwRes = GetNamedSecurityInfoW(
        dirPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pOldDACL,
        nullptr,
        &pSD);

    if (dwRes != ERROR_SUCCESS)
    {
        std::wcerr << L"Error while getting DACL: " << dwRes << std::endl;
        CloseHandle(tokenHandle);
        free(tokenUser);
        return;
    }

    if (readOnly)
    {
        EXPLICIT_ACCESSW ea = {};
        ea.grfAccessPermissions = FILE_GENERIC_WRITE | FILE_WRITE_DATA | FILE_APPEND_DATA | DELETE;
        ea.grfAccessMode = DENY_ACCESS;
        ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
        ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea.Trustee.ptstrName = (LPWSTR)tokenUser->User.Sid;

        dwRes = SetEntriesInAclW(1, &ea, pOldDACL, &pNewDACL);
        if (dwRes != ERROR_SUCCESS)
        {
            std::wcerr << L"Error while making new DACL: " << dwRes << std::endl;
            LocalFree(pSD);
            CloseHandle(tokenHandle);
            free(tokenUser);
            return;
        }
    }
    else
    {
        dwRes = SetEntriesInAclW(0, nullptr, pOldDACL, &pNewDACL); // Сброс DACL
        if (dwRes != ERROR_SUCCESS)
        {
            std::wcerr << L"Error while resetting DACL: " << dwRes << std::endl;
            LocalFree(pSD);
            CloseHandle(tokenHandle);
            free(tokenUser);
            return;
        }
    }

    dwRes = SetNamedSecurityInfoW(
        (LPWSTR)dirPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        pNewDACL,
        nullptr);

    if (dwRes != ERROR_SUCCESS)
    {
        std::wcerr << L"Error while installing DACL: " << dwRes << std::endl;
    }
    else
    {
        std::wcout << (readOnly ? L"Folder was made for reading only." : L"Rights for wring returned.") << std::endl;
    }

    LocalFree(pSD);
    LocalFree(pNewDACL);
    CloseHandle(tokenHandle);
    free(tokenUser);
}

int wmain()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    std::wstring path;
    int choice;

    std::wcout << L"Enter number of chosen action:\n";
    std::wcout << L"1 - Make folder only for reading\n";
    std::wcout << L"2 - Remove the write restriction\n> ";
    std::wcin >> choice;
    std::wcin.ignore();

    std::wcout << L"Enter full way to the folder:\n> ";
    std::getline(std::wcin, path);

    if (choice == 1)
    {
        SetDirectoryWritePermission(path, true);
    }
    else if (choice == 2)
    {
        SetDirectoryWritePermission(path, false);
    }
    else
    {
        std::wcout << L"w r o n g    c h o i c e" << std::endl;
    }

    return 0;
}

//
