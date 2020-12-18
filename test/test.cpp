// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>
#include "../exp.h"

typedef void (*secret_fp)(void);

int main(int argc, char **argv)
{
  if ( argc == 1 )
  {
    DWORD exp_size = 0;
    std::vector<exp_pair> real_exp;
    make_payload(real_exp);
    if ( make_new_export_table(::GetModuleHandle(NULL), "pexphide.dll", &real_exp, exp_size) )
    {
      dump_place(exp_size);
      return 0;  
    }
    return 1;
  }
  HMODULE hMod = ::LoadLibraryA("pexphide.dll");
  if ( NULL == hMod )
  {
    fprintf(stderr, "cannot load pexphide.dll, error %d\n", ::GetLastError());
    return 1;
  }
  secret_fp fp = (secret_fp)::GetProcAddress(hMod, argv[1]);
  if ( fp == NULL )
  {
    fprintf(stderr, "cannot get %s, error %d\n", argv[1], ::GetLastError());
    return 1;
  }
  fp();
  ::FreeLibrary(hMod);
  return 0;
}
