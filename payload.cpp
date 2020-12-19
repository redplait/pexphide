#include "pch.h"
#include "exp.h"

void secret_func1()
{
  printf("hello from secret_func1\n");
}

void secret_func2()
{
  printf("hello from secret_func2\n");
}

void secret_func3()
{
  printf("hello from secret_func3\n");
}

// fill vector with really exported functions - in this case s1, s2 and s3
void make_payload(std::vector<exp_pair> &out_res)
{
  exp_pair tmp;
  char func_name[10];
  func_name[0] = 's';
  func_name[1] = '2';
  tmp.name = func_name;
  tmp.func = (PVOID)&secret_func2;
  out_res.push_back(tmp);
  func_name[1] = '1';
  tmp.name = func_name;
  tmp.func = (PVOID)&secret_func1;
  out_res.push_back(tmp);
  func_name[1] = '3';
  tmp.name = func_name;
  tmp.func = (PVOID)&secret_func3;
  out_res.push_back(tmp);
}