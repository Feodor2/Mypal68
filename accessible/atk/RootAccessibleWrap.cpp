/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "RootAccessibleWrap.h"

#include "nsMai.h"

using namespace mozilla::a11y;

GtkWindowAccessible::GtkWindowAccessible(AtkObject* aAccessible)
    : DummyAccessible() {
  g_object_ref(aAccessible);
  mAtkObject = aAccessible;
}

GtkWindowAccessible::~GtkWindowAccessible() {
  g_object_unref(mAtkObject);
  mAtkObject = nullptr;
}
