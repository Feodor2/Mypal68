/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _RUSTSDPMEDIASECTION_H_
#define _RUSTSDPMEDIASECTION_H_

#include "mozilla/Attributes.h"
#include "mozilla/UniquePtr.h"
#include "signaling/src/sdp/RsdparsaSdpInc.h"
#include "signaling/src/sdp/RsdparsaSdpGlue.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/RsdparsaSdpAttributeList.h"

#include <map>

namespace mozilla {

class RsdparsaSdp;
class SdpParser;

class RsdparsaSdpMediaSection final : public SdpMediaSection {
  friend class RsdparsaSdp;

 public:
  ~RsdparsaSdpMediaSection() {}

  MediaType GetMediaType() const override { return mMediaType; }

  unsigned int GetPort() const override;
  void SetPort(unsigned int port) override;
  unsigned int GetPortCount() const override;
  Protocol GetProtocol() const override;
  const SdpConnection& GetConnection() const override;
  SdpConnection& GetConnection() override;
  uint32_t GetBandwidth(const std::string& type) const override;
  const std::vector<std::string>& GetFormats() const override;

  const SdpAttributeList& GetAttributeList() const override;
  SdpAttributeList& GetAttributeList() override;
  SdpDirectionAttribute GetDirectionAttribute() const override;

  void AddCodec(const std::string& pt, const std::string& name,
                uint32_t clockrate, uint16_t channels) override;
  void ClearCodecs() override;

  void AddDataChannel(const std::string& name, uint16_t port, uint16_t streams,
                      uint32_t message_size) override;

  void Serialize(std::ostream&) const override;

 private:
  RsdparsaSdpMediaSection(size_t level, RsdparsaSessionHandle session,
                          const RustMediaSection* const section,
                          const RsdparsaSdpAttributeList* sessionLevel);

  void LoadFormats();
  void LoadConnection();

  RsdparsaSessionHandle mSession;
  const RustMediaSection* mSection;

  MediaType mMediaType;
  std::vector<std::string> mFormats;

  UniquePtr<SdpConnection> mConnection;

  UniquePtr<RsdparsaSdpAttributeList> mAttributeList;
};
}  // namespace mozilla

#endif
