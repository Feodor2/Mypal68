/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
** Base class implmenation for I/O (ref: prio.h)
*/

#include "rcio.h"

RCIO::~RCIO() { }

RCIO::RCIO(RCIO::RCIOType): RCBase() { }
