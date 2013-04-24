// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_GALLERIES_FILEAPI_PICASA_PMP_TABLE_READER_H_
#define CHROME_BROWSER_MEDIA_GALLERIES_FILEAPI_PICASA_PMP_TABLE_READER_H_

#include <string>

#include "base/basictypes.h"
#include "base/memory/scoped_vector.h"

namespace base {
class FilePath;
}

namespace picasaimport {

class PmpColumnReader;

class PmpTableReader {
 public:
  PmpTableReader();

  virtual ~PmpTableReader();

  // |columns| parameter will define, in-order, the columns returned by
  // subsequent columns to GetColumns() if Init() succeeds.
  bool Init(const std::string& table_name,
            const base::FilePath& directory_path,
            const std::vector<std::string>& columns);

  // Returns a const "view" of the current contents of |column_readers_|.
  std::vector<const PmpColumnReader*> GetColumns() const;

  uint32 RowCount() const;

 private:
  ScopedVector<PmpColumnReader> column_readers_;
  uint32 max_row_count_;

  DISALLOW_COPY_AND_ASSIGN(PmpTableReader);
};

}  // namespace picasaimport

#endif  // CHROME_BROWSER_MEDIA_GALLERIES_FILEAPI_PICASA_PMP_TABLE_READER_H_
