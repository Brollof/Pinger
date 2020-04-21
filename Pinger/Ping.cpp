#include <winsock.h>
#include <iostream>
#include "Ping.h"

#define RET_SUCCESS(lat) {true, lat}
#define RET_FAILURE() {false, 0}

typedef struct
{
  unsigned char Ttl;                         // Time To Live
  unsigned char Tos;                         // Type Of Service
  unsigned char Flags;                       // IP header flags
  unsigned char OptionsSize;                 // Size in bytes of options data
  unsigned char* OptionsData;                // Pointer to options data
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct
{
  DWORD Address;                             // Replying address
  unsigned long  Status;                     // Reply status
  unsigned long  RoundTripTime;              // RTT in milliseconds
  unsigned short DataSize;                   // Echo data size
  unsigned short Reserved;                   // Reserved for system use
  void* Data;                                // Pointer to the echo data
  IP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, *PIP_ECHO_REPLY;


PingResult_t ping(std::string target)
{
  WSADATA wsaData;
  WORD version;
  version = MAKEWORD(2, 2);
  if (WSAStartup(version, &wsaData) != 0)
  {
    std::cout << "WSAStartup failed!" << std::endl;
  }
  const char* dll = "icmp.dll";
  size_t size = strlen(dll) + 1;
  wchar_t* dllName = new wchar_t[size];

  size_t outSize;
  mbstowcs_s(&outSize, dllName, size, dll, size - 1);

  HINSTANCE hIcmp = LoadLibrary((LPWSTR)dllName);
  if (hIcmp == 0)
  {
    std::cout << "Unable to locate ICMP.DLL!" << std::endl;
    return RET_FAILURE();
  }

  // Look up an IP address for the given host name
  struct hostent* phe = gethostbyname(target.c_str());
  if (!phe)
  {
    std::cout << "Could not find IP address for: " << target << std::endl;
    return RET_FAILURE();
  }

  // Get handles to the functions inside ICMP.DLL that we'll need
  typedef HANDLE(WINAPI * pfnHV)(VOID);
  typedef BOOL(WINAPI * pfnBH)(HANDLE);
  typedef DWORD(WINAPI * pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
                                         PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);
  pfnHV pIcmpCreateFile;
  pfnBH pIcmpCloseHandle;
  pfnDHDPWPipPDD pIcmpSendEcho;
  pIcmpCreateFile = (pfnHV)GetProcAddress(hIcmp, "IcmpCreateFile");
  pIcmpCloseHandle = (pfnBH)GetProcAddress(hIcmp, "IcmpCloseHandle");
  pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(hIcmp, "IcmpSendEcho");
  if ((pIcmpCreateFile == 0) || (pIcmpCloseHandle == 0) || (pIcmpSendEcho == 0))
  {
    std::cout << "Failed to get proc addr for function." << std::endl;
    return RET_FAILURE();
  }

  // Open the ping service
  HANDLE hIP = pIcmpCreateFile();
  if (hIP == INVALID_HANDLE_VALUE)
  {
    std::cout << "Unable to open ping service." << std::endl;
    return RET_FAILURE();
  }

  // Build ping packet
  unsigned char acPingBuffer[64];
  memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
  PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));
  if (pIpe == 0)
  {
    std::cout << "Failed to allocate global ping packet buffer." << std::endl;
    return RET_FAILURE();
  }
  pIpe->Data = acPingBuffer;
  pIpe->DataSize = sizeof(acPingBuffer);

  // Send the ping packet
  
  DWORD dwStatus = pIcmpSendEcho(hIP, *((DWORD*)phe->h_addr_list[0]),
                                 acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
                                 sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), 1000);
  if (dwStatus != 0)
  {
    std::cout << "Addr: " <<
              int(LOBYTE(LOWORD(pIpe->Address))) << "." <<
              int(HIBYTE(LOWORD(pIpe->Address))) << "." <<
              int(LOBYTE(HIWORD(pIpe->Address))) << "." <<
              int(HIBYTE(HIWORD(pIpe->Address))) << ", " <<
              "RTT: " << int(pIpe->RoundTripTime) << "ms, " <<
              "TTL: " << int(pIpe->Options.Ttl) << std::endl;
  }
  else
  {
    std::cout << "Error obtaining info from ping packet." << std::endl;
    return RET_FAILURE();
  }

  // Shut down...
  GlobalFree(pIpe);
  FreeLibrary(hIcmp);
  WSACleanup();
  return RET_SUCCESS(int(pIpe->RoundTripTime));
}