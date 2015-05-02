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


namespace FSProxy {


const size_t c_MaxPath = 256;     // Maximum path length in mock file system.



class DirectoryIteratorMock : public DirectoryIterator {
    // This class implements a directory iterator for the mock file system.

  public:
    DirectoryIteratorMock (const std::wstring path);
    ~DirectoryIteratorMock();

    // Advance to first/next entry.
    bool next() override;

    // True => current entry is a directory.
    bool isDirectory() const override;

    // Return name of the current entry.
    const wchar_t* name() const override;
};



class FileSysProxyMock : public FileSysProxy {
    // This class provides an mock file system to test out the generic FileSysProxy class.

  public:
    virtual ~FileSysProxyMock() {}

    size_t maxPathLength() const override { return c_MaxPath; }

    // Return a directory iterator object.
    // NOTE: User must delete this object!
    DirectoryIterator* newDirectoryIterator (const std::wstring path) const;

  private: 
};


};   // namespace FileSystemProxy


#endif   // _fileSystemProxyMock_h