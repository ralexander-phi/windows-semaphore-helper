// SemaphoreHelper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <atlbase.h>
#include <iostream>
#include <Windows.h>

void handleLastError() {
	USES_CONVERSION;
	LPTSTR lpMsgBuf;
	DWORD lastError = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	std::wcout << T2W(lpMsgBuf) << L"\n";
	LocalFree(lpMsgBuf);
}

HANDLE create(LPCSTR name, int initCount, int maxCount) {
	std::cout << "Creating semaphore " << name << " with counts " << initCount
		<< " of " << maxCount << "...\n";
	HANDLE h = CreateSemaphoreA(NULL, initCount, maxCount, name);
	if (h == NULL) {
		std::cout << "CreateSemaphoreA Error: ";
		handleLastError();
		exit(1);
	}
	std::cout << "Created\n";
	return h;
}

HANDLE open(LPCSTR name) {
	std::cout << "Opening semaphore " << name << "...\n";
	HANDLE h = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, name);
	if (h == NULL) {
		std::cout << "OpenSemaphoreA Error: ";
		handleLastError();
		exit(1);
	}
	std::cout << "Opened\n";
	return h;
}

void acquire(HANDLE h) {
	std::cout << "Acquiring...\n";
	DWORD res = WaitForSingleObject(h, 0);
	if (res == WAIT_OBJECT_0) {
		std::cout << "Acquired\n";
	}
	else if (res == WAIT_TIMEOUT) {
		std::cout << "WaitForSingleObject Timeout\n";
		exit(2);
	}
	else if (res == WAIT_FAILED) {
		std::cout << "WaitForSingleObject Error: ";
		handleLastError();
		exit(1);
	}
	else {
		std::cout << "Wait error:" << res << "\n";
		exit(1);
	}
}

void release(HANDLE h) {
	std::cout << "Releasing...\n";
	BOOL res = ReleaseSemaphore(h, 1, NULL);
	if (! res) {
		std::cout << "ReleaseSemaphore Error: ";
		handleLastError();
		exit(1);
	}
}

int close(HANDLE h) {
	std::cout << "Closing...\n";
	BOOL closed = CloseHandle(h);
	if (!closed) {
		std::cout << "CloseHandle Error: ";
		handleLastError();
		exit(1);
	}
	std::cout << "Closed\n";
	return 0;
}

void usage() {
	std::cout << "Usage:\n";
	std::cout << "    SemaphoreHelper.exe <action> <name> <sleep_ms>\n";
	std::cout << "    SemaphoreHelper.exe <action> <name> <sleep_ms> <inital_count> <max_count>\n";
	std::cout << "Action is one of create, open, create-and-acquire, open-and-acquire\n";
}

int main(int argc, char* argv[])
{
	int init_count;
	int max_count;
	if (argc == 4) {
		init_count = 1;
		max_count = 1;
	}
	else if (argc == 6) {
		init_count = atoi(argv[4]);
		max_count = atoi(argv[5]);
	}
	else {
		usage();
		return 1;
	}

	LPCSTR action = argv[1];
	LPCSTR name = argv[2];
	int sleep_ms = atoi(argv[3]);

	bool bCreate = false;
	bool bOpen = false;
	bool bAcquire = false;

	if (strcmp(action, "create") == 0) {
		bCreate = true;
	}
	else if (strcmp(action, "open") == 0) {
		bOpen = true;
	}
	else if (strcmp(action, "create-and-acquire") == 0) {
		bCreate = true;
		bAcquire = true;
	}
	else if (strcmp(action, "open-and-acquire") == 0) {
		bOpen = true;
		bAcquire = true;
	}
	else {
		usage();
		return 1;
	}

	HANDLE h;
	if (bCreate) {
		h = create(name, init_count, max_count);
	}
	else if (bOpen) {
		h = open(name);
	}
	else {
		exit(1);
	}

	if (bAcquire) {
		acquire(h);
	}

	std::cout << "Sleeping for " << sleep_ms << "ms...\n";
	Sleep(sleep_ms);

	if (bAcquire) {
		release(h);
	}

	close(h);
	return 0;
}
