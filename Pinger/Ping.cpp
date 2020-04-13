#include <winsock.h>
#include <iostream>

#include "Ping.h"

using namespace std;

bool doit(string host)
{
    WSADATA wsaData;
    WORD version;
    version = MAKEWORD(2, 2);
    int err = WSAStartup(version, &wsaData);

    cout << "WSA init: " << err << endl;
    const char* port = "icmp.dll";
    size_t size = strlen(port) + 1;
    wchar_t* portName = new wchar_t[size];

    size_t outSize;
    mbstowcs_s(&outSize, portName, size, port, size - 1);

    LPWSTR ptr = portName;
    HINSTANCE hIcmp = LoadLibrary(ptr);
    if (hIcmp == 0) {
        cout << "Unable to locate ICMP.DLL!" << endl;
        return 2;
    }

    // Look up an IP address for the given host name
    struct hostent* phe = gethostbyname(host.c_str());
    cout << phe << endl;
    if (!phe)
    {
        cout << "Could not find IP address for: " << host << endl;
        return 3;
    }

    // Get handles to the functions inside ICMP.DLL that we'll need
    typedef HANDLE(WINAPI* pfnHV)(VOID);
    typedef BOOL(WINAPI* pfnBH)(HANDLE);
    typedef DWORD(WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
        PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);
    pfnHV pIcmpCreateFile;
    pfnBH pIcmpCloseHandle;
    pfnDHDPWPipPDD pIcmpSendEcho;
    pIcmpCreateFile = (pfnHV)GetProcAddress(hIcmp, "IcmpCreateFile");
    pIcmpCloseHandle = (pfnBH)GetProcAddress(hIcmp, "IcmpCloseHandle");
    pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(hIcmp, "IcmpSendEcho");
    if ((pIcmpCreateFile == 0) || (pIcmpCloseHandle == 0) || (pIcmpSendEcho == 0))
    {
        cout << "Failed to get proc addr for function." << endl;
        return false;
    }

    // Open the ping service
    HANDLE hIP = pIcmpCreateFile();
    if (hIP == INVALID_HANDLE_VALUE)
    {
        cout << "Unable to open ping service." << endl;
        return false;
    }

    // Build ping packet
    char acPingBuffer[64];
    memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
    PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));
    if (pIpe == 0)
    {
        cout << "Failed to allocate global ping packet buffer." << endl;
        return false;
    }
    pIpe->Data = acPingBuffer;
    pIpe->DataSize = sizeof(acPingBuffer);

    // Send the ping packet
    DWORD dwStatus = pIcmpSendEcho(hIP, *((DWORD*)phe->h_addr_list[0]),
        acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
        sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), 5000);
    if (dwStatus != 0)
    {
        cout << "Addr: " <<
            int(LOBYTE(LOWORD(pIpe->Address))) << "." <<
            int(HIBYTE(LOWORD(pIpe->Address))) << "." <<
            int(LOBYTE(HIWORD(pIpe->Address))) << "." <<
            int(HIBYTE(HIWORD(pIpe->Address))) << ", " <<
            "RTT: " << int(pIpe->RoundTripTime) << "ms, " <<
            "TTL: " << int(pIpe->Options.Ttl) << endl;
    }
    else
    {
        cout << "Error obtaining info from ping packet." << endl;
    }

    // Shut down...
    GlobalFree(pIpe);
    FreeLibrary(hIcmp);
    WSACleanup();
    return 0;
}