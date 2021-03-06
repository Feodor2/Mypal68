/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPERRORHOLDER_H_
#define _SDPERRORHOLDER_H_

#include <vector>
#include <string>

namespace mozilla {

class SdpErrorHolder {
 public:
  SdpErrorHolder() {}
  virtual ~SdpErrorHolder() {}

  void AddParseError(size_t line, const std::string& message) {
    mErrors.push_back(std::make_pair(line, message));
  }

  void AddParseWarnings(size_t line, const std::string& message) {
    mWarnings.push_back(std::make_pair(line, message));
  }

  void ClearParseErrors() { mErrors.clear(); }

  void ClearParseWarnings() { mWarnings.clear(); }

  /**
   * Returns a reference to the list of parse errors.
   * This gets cleared out when you call Parse.
   */
  const std::vector<std::pair<size_t, std::string> >& GetParseErrors() const {
    return mErrors;
  }

  const std::vector<std::pair<size_t, std::string> >& GetParseWarnings() const {
    return mWarnings;
  }

 private:
  std::vector<std::pair<size_t, std::string> > mErrors;
  std::vector<std::pair<size_t, std::string> > mWarnings;
};

}  // namespace mozilla

#endif
