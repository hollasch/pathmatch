//==================================================================================================
// FileSystemProxyWindows
//
//     Windows file system proxy, using the FileSystemProxy base.
//
// _________________________________________________________________________________________________
// Copyright 2015 Steve Hollasch
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under
// the License.
//==================================================================================================

#ifndef _fileSystemProxyWindows_h
#define _fileSystemProxyWindows_h

    // Includes

#include <FileSystemProxy.h>
#include <windows.h>
#include <string>


namespace FileSystemProxy {


class DirectoryIteratorWindows : public DirectoryIterator {

  public:
    DirectoryIteratorWindows (const std::wstring path);
    ~DirectoryIteratorWindows();

    // Advance to first/next entry.
    bool next() override;

    // True => current entry is a directory.
    bool isDirectory() const override;

    // Return name of the current entry.
    const wchar_t* name() const override;

  private:
    bool            m_started;      // True => directory iteration started
    HANDLE          m_findHandle;   // Directory Find Context
    WIN32_FIND_DATA m_findData;     // Directory Find Entry
};



class FileSysProxyWindows : public FileSysProxy {

  public:
    virtual ~FileSysProxyWindows() {}

    size_t maxPathLength() const override { return _MAX_PATH; }

    // Return a directory iterator object.
    // NOTE: User must delete this object!
    DirectoryIterator* newDirectoryIterator (const std::wstring path) const;
};


};  // namespace FileSystemProxy


#endif   // _fileSystemProxyWindows_h