# Malw_Dev
## C
There are process injection, reverse shell socket connection, persistency and stealer codes separately.
### Process Injection
Customer's security solutions may detect the suspicious Win32 functions like **VirtualAllocEx**, **WriteProcessMemory**, **CreateRemoteThread**. Therefore, there is another program which uses Native API versions of **OpenProcess**, **VirtualAllocEx**, **WriteProcessMemory**, **CreateRemoteThread**, **CloseHandle**.
* OpenProcess -> NtOpenProcess
* VirtualAllocEx -> NtAllocateVirtualMemory
* WriteProcessMemory -> NtWriteVirtualMemory
* CreateRemoteThread -> NtCreateThreadEx
* CloseHandle -> NtClose
### Reverse Shell Socket Connection
This part of malware gives the attacker connection to the victim and provides executing commands on the command prompt. Connection is done with reverse shell method. When both of the **rat.c** and **server.c** programs are run, connection is done automatically.
### Persistency
Malware saves itself on the registry key: **HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run**
This will assure that the malware will run each time the victim's computer is booted.
### Stealer
Malware steals data from **Chrome** and **Edge** browsers. It looks for **History** (search history and downloads history) and **Login Data** (user credentials saved for automatical login) databases. The variety of browsers and databases can be increased but this stealer is done for **PoC** yet.
### Soon to be added:
* Keylogger
## Python
There are simple malware codes like;
* Crashing computer
* Keylogger
* Port Scanner
* Reverse shell provider (socket programming)
* Cracking the passwords hashed with SHA256
* Ransomware and its decryptor
