//==================================================================================================
// FileSystemProxy
//
// Declarations for the file system proxy classes. These objects proxy the file system of the
// underlying operating system, or test harness.
//
// _________________________________________________________________________________________________
// Copyright 2013 Steve Hollasch
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#ifndef _fileSystemProxy_h
#define _fileSystemProxy_h

    // Includes

#include <stdlib.h>
#include <windows.h>
#include <string>


namespace FileSystemProxy {


class DirectoryIterator {

  public:
    DirectoryIterator(const std::wstring path);
    ~DirectoryIterator();

    // Advance to first/next entry.
    bool next();

    // True => current entry is a directory.
    bool isDirectory() const;

    // Return name of the current entry.
    const wchar_t* name() const;

  private:
    bool            m_started;      // True => directory iteration started
    HANDLE          m_findHandle;   // Directory Find Context
    WIN32_FIND_DATA m_findData;     // Directory Find Entry
};



class FileSysProxy {

  public:

    FileSysProxy() {}
    ~FileSysProxy() {}

    inline size_t MaxPath() const { return _MAX_PATH; }
};



};  // namespace FileSystemProxy


#endif   // ifndef _fileSystemProxy_h