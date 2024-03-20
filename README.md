# Malw_Dev
## C
There are only process injection codes currently. Process injection is done in two ways: By using Win32API and Native API
Customer's security solutions may detect the suspicious Win32 functions like **VirtualAllocEx**, **WriteProcessMemory**, **CreateRemoteThread**. Therefore, there is another program which uses Native API versions of **OpenProcess**, **VirtualAllocEx**, **WriteProcessMemory**, **CreateRemoteThread**, **CloseHandle**.
### Soon to be added (programmed but not tested):
* Socket Programming for both Client (Victim) and Server (Attacker). Connection provides reverse shell
* Executing Commands from gathered shell
* Keylogger
* Saving the malware into the registry keys for persistence
## Python
There are simple malware codes like;
* Crashing computer
* Keylogger
* Port Scanner
* Reverse shell provider (socket programming)
* Cracking the passwords hashed with SHA256
* Ransomware and its decryptor
