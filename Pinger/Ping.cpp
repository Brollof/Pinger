#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include "Ping.h"

#pragma comment (lib, "Ws2_32.lib")

#define ICMP_DLL_NAME     L"icmp.dll"
#define PING_TIMEOUT      1000 // ms

typedef struct
{
  unsigned char Ttl;                         // Time To Live
  unsigned char Tos;                         // Type Of Service
  unsigned char Flags;                       // IP header flags
  unsigned char OptionsSize;                 // Size in bytes of options data
  unsigned char* OptionsData;                // Pointer to options data
} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

typedef struct
{
  DWORD Address;                             // Replying address
  unsigned long  Status;                     // Reply status
  unsigned long  RoundTripTime;              // RTT in milliseconds
  unsigned short DataSize;                   // Echo data size
  unsigned short Reserved;                   // Reserved for system use
  void* Data;                                // Pointer to the echo data
  IP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, * PIP_ECHO_REPLY;

PingResult_t ping(std::string target)
{
  WSADATA wsaData;
  WORD version;
  version = MAKEWORD(2, 2);
  PingResult_t result = { false, 0, "" };

  if (WSAStartup(version, &wsaData) != 0)
  {
    std::cout << "WSAStartup failed!" << std::endl;
    return result;
  }

  HINSTANCE hIcmp = LoadLibrary(ICMP_DLL_NAME);
  if (hIcmp == 0)
  {
    std::cout << "Unable to locate ICMP.DLL!" << std::endl;
    WSACleanup();
    return result;
  }

  // Look up an IP address for the given host name
  struct addrinfo* addrResult = nullptr;
  int error = getaddrinfo(target.c_str(), nullptr, nullptr, &addrResult);
  if (error != 0)
  {
    std::cout << "Could not find IP address for: " << target << ", error: " << error << std::endl;
    WSACleanup();
    return result;
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
    std::cout << "Failed to get proc addr for function." << std::endl;
    WSACleanup();
    freeaddrinfo(addrResult);
    return result;
  }

  // Open the ping service
  HANDLE hIP = pIcmpCreateFile();
  if (hIP == INVALID_HANDLE_VALUE)
  {
    std::cout << "Unable to open ping service." << std::endl;
    WSACleanup();
    freeaddrinfo(addrResult);
    return result;
  }

  // Build ping packet
  unsigned char acPingBuffer[64];
  memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
  PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));
  if (pIpe == 0)
  {
    std::cout << "Failed to allocate global ping packet buffer." << std::endl;
    WSACleanup();
    freeaddrinfo(addrResult);
    return result;
  }
  pIpe->Data = acPingBuffer;
  pIpe->DataSize = sizeof(acPingBuffer);

  // Send the ping packet
  struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)addrResult->ai_addr;
  DWORD dwStatus = pIcmpSendEcho(hIP, sockaddr_ipv4->sin_addr.S_un.S_addr,
    acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
    sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), PING_TIMEOUT);

  std::stringstream ss;
  if (dwStatus != 0)
  {
    ss << "Addr: " <<
      int(LOBYTE(LOWORD(pIpe->Address))) << "." <<
      int(HIBYTE(LOWORD(pIpe->Address))) << "." <<
      int(LOBYTE(HIWORD(pIpe->Address))) << "." <<
      int(HIBYTE(HIWORD(pIpe->Address))) << ", " <<
      "RTT: " << int(pIpe->RoundTripTime) << "ms, " <<
      "TTL: " << int(pIpe->Options.Ttl) << std::endl;
    std::cout << ss.rdbuf();
  }
  else
  {
    ss << "Timeout." << std::endl;
    std::cout << ss.rdbuf();
    WSACleanup();
    freeaddrinfo(addrResult);
    return result;
  }
  result.data = ss.str();
  result.latency = int(pIpe->RoundTripTime);
  result.status = true;

  // Shut down...
  GlobalFree(pIpe);
  FreeLibrary(hIcmp);
  WSACleanup();
  freeaddrinfo(addrResult);

  return result;
}