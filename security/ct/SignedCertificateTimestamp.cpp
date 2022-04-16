/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SignedCertificateTimestamp.h"

namespace mozilla {
namespace ct {

void LogEntry::Reset() {
  type = LogEntry::Type::X509;
  leafCertificate.clear();
  issuerKeyHash.clear();
  tbsCertificate.clear();
}

bool DigitallySigned::SignatureParametersMatch(
    HashAlgorithm aHashAlgorithm,
    SignatureAlgorithm aSignatureAlgorithm) const {
  return (hashAlgorithm == aHashAlgorithm) &&
         (signatureAlgorithm == aSignatureAlgorithm);
}

}  // namespace ct
}  // namespace mozilla
