/*
Copyright (c) 2017-2018 Adubbz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <functional>
#include <vector>

#include "install/hfs0.hpp"
#include "install/nsp_or_xci.hpp"

#include "nx/ncm.hpp"
#include <memory>
#include <switch/types.h>

namespace tin::install::xci {
class XCI : public NSPorXCI {
protected:
  u64 m_secureHeaderOffset;
  std::vector<u8> m_secureHeaderBytes;
  FILE *m_file;

public:
  XCI(std::string path);
  ~XCI();

  void RetrieveHeader() override;
  const HFS0BaseHeader *GetSecureHeader();
  u64 GetDataOffset();

  void BufferData(void *buf, off_t offset, size_t size) override;

  const u32 GetFileEntryNum() override;
  const void *GetFileEntry(unsigned int index) override;
  const char *GetFileEntryName(const void *fileEntry) override;
  const u64 GetFileEntrySize(const void *fileEntry) override;
  const u64 GetFileEntryOffset(const void *fileEntry) override;
};
} // namespace tin::install::xci
