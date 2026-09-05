Folder Blocker for Windows
==

Praeambulum
--

This project was developed as a quick practical solution to a system issue in Windows 10, where the default explorer gui and other system gui freezes or fails to restrict folder access permissions properly. The tool directly interfaces with the Win32 API to hard-lock write access with recursive inheritance across all subdirectories and contained files for the current user's security identifier - or seamlessly revert access rights to default.

Anatome
--

#### Stack & Tools

* **Language:** C++ 17

* **API:** Win32 API (AclAPI.h, windows.h)

* **IDE:** Visual Studio 2022

#### Core Features

* **Direct DACL Manipulation:** Uses SetEntriesInAclW and SetNamedSecurityInfoW to insert DENY_ACCESS entries for file creation, modification, and deletion.

* **Recursive Inheritance:** Applies SUB_CONTAINERS_AND_OBJECTS_INHERIT flags to enforce write-protection across all subfolders and contained files.

* **SID Resolution:** Dynamically fetches current process token user SID (OpenProcessToken) to protect system services while targeting the specific user.

*Unicode Ready: Configured with wmain and _O_U16TEXT streams to ensure full support for UTF-16 paths with spaces and special characters.

Portatio
--

#### Requirements

* **OS:** Windows 7+ (x86 / x64)
* **Language:** C++ (11+)


