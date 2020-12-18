#pragma once

typedef struct _exp_pair {
  std::string name;
  PVOID func;
} exp_pair;

size_t calc_exp_size(const char *dll_name, std::vector<exp_pair> *funcs);
int make_new_export_table(HMODULE mz, const char *dll_name, std::vector<exp_pair> *funcs, DWORD &exp_size, PBYTE* out_exp = NULL);
void dump_place(DWORD exp_size);
void make_payload(std::vector<exp_pair> &out_res);