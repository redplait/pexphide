// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "../exp.h"

extern "C" NTSYSAPI PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(HMODULE);

void make_fake_exp(HMODULE mz)
{
  DWORD exp_size = 0;
  std::vector<exp_pair> real_exp;
  make_payload(real_exp);
  PBYTE new_exp_tab = NULL; // pointer to IMAGE_EXPORT_DIRECTORY somewhere in place
  if ( make_new_export_table(mz, "pexphide.dll", &real_exp, exp_size, &new_exp_tab) )
  {
    PIMAGE_NT_HEADERS hdr = RtlImageNtHeader(mz);
    DWORD old_prot = 0;
    if ( ::VirtualProtect(hdr, sizeof(IMAGE_NT_HEADERS), PAGE_READWRITE, &old_prot) )
    {
      hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = exp_size;
      hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = new_exp_tab - (PBYTE)mz;
      ::VirtualProtect(hdr, sizeof(IMAGE_NT_HEADERS), old_prot, &old_prot);
    }
  }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
      // Microsoft recommends to use DisableThreadLibraryCalls: https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-disablethreadlibrarycalls#remarks
      // but you can sure omit it if you need
      DisableThreadLibraryCalls(hModule);
      // make export table in memory and patch IMAGE_NT_HEADERS of this module
      make_fake_exp(hModule);
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

