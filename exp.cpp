#include "pch.h"
#include "exp.h"

// memory for IMAGE_EXPORT_DIRECTORY and all guts like module name, function names, ordinals etc
// will be placed in .data section
BYTE place[0x2000] = { 0 };

size_t calc_exp_size(const char *dll_name, std::vector<exp_pair> *funcs)
{
  if ( funcs->empty() )
     return 0;
  size_t dll_len = strlen(dll_name) + 1;
  size_t count = funcs->size();
  // calc strings length
  for ( auto s = funcs->cbegin(); s != funcs->cend(); ++s )
  {
    dll_len += s->name.length() + 1;
  }
  // rva of name + rva of function itself + ordinal
  return dll_len + sizeof(IMAGE_EXPORT_DIRECTORY) + count * (sizeof(DWORD) + sizeof(WORD) + sizeof(DWORD));
}

int make_new_export_table(HMODULE mz, const char *dll_name, std::vector<exp_pair> *funcs, DWORD &exp_size, PBYTE *out_exp)
{
  exp_size = (DWORD)calc_exp_size(dll_name, funcs);
  if ( !exp_size )
    return 0;
  if ( exp_size >= sizeof(place) )
  {
    printf("expand place to %X bytes\n", exp_size);
    return 0;
  }
  size_t dll_len = strlen(dll_name) + 1;
  PBYTE ptr = place;
  memcpy(ptr, dll_name, dll_len + 1);
  ptr += dll_len;
  // make IMAGE_EXPORT_DIRECTORY
  PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)ptr;
  if ( out_exp != NULL )
    *out_exp = (PBYTE)exp;
  exp->Characteristics = 0;
  exp->MajorVersion = exp->MinorVersion = NULL;
  exp->Name = place - (PBYTE)mz;
  exp->Base = 0;
  exp->NumberOfFunctions = exp->NumberOfNames = funcs->size();
  ptr += sizeof(IMAGE_EXPORT_DIRECTORY);
  exp->AddressOfFunctions = ptr - (PBYTE)mz;
  // sort funcs
  std::sort(funcs->begin(), funcs->end(), [](const exp_pair &l, exp_pair &r) -> bool { return l.name < r.name; });
  // fill
  PDWORD funcz = (PDWORD)ptr;
  ptr += exp->NumberOfFunctions * sizeof(DWORD);
  exp->AddressOfNameOrdinals = ptr - (PBYTE)mz;
  PWORD ords = (PWORD)ptr;
  ptr += exp->NumberOfFunctions * sizeof(WORD);
  exp->AddressOfNames = ptr - (PBYTE)mz;
  WORD ord = 0;
  PDWORD names = (PDWORD)ptr;
  ptr += exp->NumberOfFunctions * sizeof(DWORD);
  for ( auto s = funcs->cbegin(); s != funcs->cend(); ++s, ord++ )
  {
    *ords = ord;
     ords++;
     *funcz = (PBYTE)s->func - (PBYTE)mz;
     funcz++;
     // rva to name
     *names = ptr - (PBYTE)mz;
     names++;
     // copy function name
     size_t curr_len = s->name.length();
     memcpy(ptr, s->name.c_str(), curr_len);
     ptr += curr_len;
     *ptr = 0;
     ptr++;
  }
  return 1;
}

void dump_place(DWORD exp_size)
{
  FILE *fp = NULL;
  if ( !fopen_s(&fp, "exp.bin", "wb") )
  {
    fwrite(place, exp_size, 1, fp);
    fclose(fp);
  }
}