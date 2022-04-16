/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsMai.h"
#include "DocAccessibleWrap.h"
#include "mozilla/PresShell.h"

using namespace mozilla;
using namespace mozilla::a11y;

////////////////////////////////////////////////////////////////////////////////
// DocAccessibleWrap
////////////////////////////////////////////////////////////////////////////////

DocAccessibleWrap::DocAccessibleWrap(dom::Document* aDocument,
                                     PresShell* aPresShell)
    : DocAccessible(aDocument, aPresShell), mActivated(false) {}

DocAccessibleWrap::~DocAccessibleWrap() {}
