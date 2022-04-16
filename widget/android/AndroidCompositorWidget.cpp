/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AndroidCompositorWidget.h"
#include "nsWindow.h"

namespace mozilla {
namespace widget {

EGLNativeWindowType AndroidCompositorWidget::GetEGLNativeWindow() {
  return (EGLNativeWindowType)mWidget->GetNativeData(NS_JAVA_SURFACE);
}

EGLNativeWindowType AndroidCompositorWidget::GetPresentationEGLSurface() {
  return (EGLNativeWindowType)mWidget->GetNativeData(NS_PRESENTATION_SURFACE);
}

void AndroidCompositorWidget::SetPresentationEGLSurface(EGLSurface aVal) {
  mWidget->SetNativeData(NS_PRESENTATION_SURFACE, (uintptr_t)aVal);
}

ANativeWindow* AndroidCompositorWidget::GetPresentationANativeWindow() {
  return (ANativeWindow*)mWidget->GetNativeData(NS_PRESENTATION_WINDOW);
}

}  // namespace widget
}  // namespace mozilla
