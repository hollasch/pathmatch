//==================================================================================================
// fileSystemProxyMock.cpp
//
//     This file contains the definitions for the file system proxy classes for the file system
//     proxy mock object.
//
// _________________________________________________________________________________________________
// Copyright 2015 Steve Hollasch
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#include "fileSystemProxyMock.h"
#include <stdlib.h>
#include <string>

using namespace std;
using namespace FSProxy;


DirectoryIteratorMock::DirectoryIteratorMock (const std::wstring /* path */)
{
    // Not yet implemented.
}

DirectoryIteratorMock::~DirectoryIteratorMock()
{
    // Not yet implemented.
}

bool DirectoryIteratorMock::next()
{
    // Not yet implemented.
    return false;
}

bool DirectoryIteratorMock::isDirectory() const
{
    // Not yet implemented.
    return false;
}

const wchar_t* DirectoryIteratorMock::name() const
{
    // Not yet implemented.
    return L"xxx";
}



FileSysProxyMock::FileSysProxyMock (const wstring mockDirFileName)
{
    if (S_OK == _wfopen_s(&m_mockDirFile, mockDirFileName.c_str(), L"r, ccs=UNICODE"))
    {
        // Do nothing with the file for now.
        fclose (m_mockDirFile);
    }
}


DirectoryIterator* FileSysProxyMock::newDirectoryIterator (const std::wstring path) const
{
    // Not yet implemented.
    return new DirectoryIteratorMock(path);
}
