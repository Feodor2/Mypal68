/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef widget_windows_nsWidgetFactory_h
#define widget_windows_nsWidgetFactory_h

#include "nscore.h"
#include "nsID.h"

class nsISupports;

nsresult nsAppShellConstructor(nsISupports* outer, const nsIID& iid,
                               void** result);

nsresult nsWidgetWindowsModuleCtor();
void nsWidgetWindowsModuleDtor();

#endif  // defined widget_windows_nsWidgetFactory_h
