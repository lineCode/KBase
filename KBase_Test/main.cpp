// KBase_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cassert>
#include <cstdio>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "kbase\error_exception_util.h"
#include "kbase\registry.h"

template<typename T>
void print_out(T beg, T end, const char* dem = " ")
{
    for (auto it = beg; it != end; ++it) {
        std::wcout << *it << dem;
    }

    std::wcout << std::endl;
}

int main(int /*argc*/, char* /*argv[]*/)
{
    try {
        kbase::RegKey regkey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", KEY_READ);
        std::wstring value;
        bool ret = regkey.ReadStringValue(L"ProgramFilesPath", &value);
        kbase::LastError error;
        if (!ret) {
            std::wcout << error.GetVerboseMessage() << std::endl;
        } else {
            std::wcout << value;
        }

    } catch (const kbase::Win32Exception& ex) {
        std::cout << ex.what();
    }

    _getch();
    return 0;
}
