/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ExtendedValidation_h
#define ExtendedValidation_h

#include "ScopedNSSTypes.h"
#include "certt.h"

namespace mozilla {
namespace pkix {
struct CertPolicyId;
}
}  // namespace mozilla

namespace mozilla {
namespace psm {

nsresult LoadExtendedValidationInfo();

/**
 * Finds the first policy OID in the given cert that is known to be an EV policy
 * OID.
 *
 * @param cert
 *        The cert to find the first EV policy of.
 * @param policy
 *        The found policy.
 * @param policyOidTag
 *        The OID tag of the found policy.
 * @return true if a suitable policy was found, false otherwise.
 */
bool GetFirstEVPolicy(CERTCertificate& cert,
                      /*out*/ mozilla::pkix::CertPolicyId& policy,
                      /*out*/ SECOidTag& policyOidTag);

// CertIsAuthoritativeForEVPolicy does NOT evaluate whether the cert is trusted
// or distrusted.
bool CertIsAuthoritativeForEVPolicy(const UniqueCERTCertificate& cert,
                                    const mozilla::pkix::CertPolicyId& policy);

}  // namespace psm
}  // namespace mozilla

#endif  // ExtendedValidation_h
