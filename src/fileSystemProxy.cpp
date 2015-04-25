//==================================================================================================
// fileSystemProxy.cpp
//
//     This file contains the definitions for the file system proxy classes.
//
// _________________________________________________________________________________________________
// Copyright 2013 Steve Hollasch
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

#include "fileSystemProxy.h"
#include <stdlib.h>
#include <string>

using namespace std;
using namespace FileSystemProxy;



// Directory Iterator Methods

DirectoryIterator::DirectoryIterator (const wstring path)
  : m_started(false)
{
    m_findHandle = FindFirstFile(path.c_str(), &m_findData);
}

DirectoryIterator::~DirectoryIterator()
{
    FindClose (m_findHandle);
}



bool DirectoryIterator::next()
{
    // Advances the iterator to the first/next entry.

    if (m_started)
        return 0 != FindNextFile(m_findHandle, &m_findData);
    
    m_started = true;
    return m_findHandle != INVALID_HANDLE_VALUE;
}



bool DirectoryIterator::isDirectory() const
{
    // Returns true if the current entry is a directory.
    return 0 != (m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}



const wchar_t* DirectoryIterator::name() const
{
    // Returns the name of the current entry.
    return m_findData.cFileName;
}