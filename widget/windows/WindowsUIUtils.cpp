/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <windows.h>
#include <winsdkver.h>
#include "mozwrlbase.h"

#include "nsServiceManagerUtils.h"

#include "WindowsUIUtils.h"

#include "nsIObserverService.h"
#include "nsIAppShellService.h"
#include "nsAppShellCID.h"
#include "mozilla/ResultVariant.h"
#include "mozilla/Services.h"
#include "mozilla/WidgetUtils.h"
#include "mozilla/WindowsVersion.h"
#include "nsString.h"
#include "nsIWidget.h"
#include "nsIWindowMediator.h"
#include "nsPIDOMWindow.h"

/* mingw currently doesn't support windows.ui.viewmanagement.h, so we disable it
 * until it's fixed. */
#ifndef __MINGW32__

#  include <windows.ui.viewmanagement.h>

#  pragma comment(lib, "runtimeobject.lib")

using namespace mozilla;
using namespace ABI::Windows::UI;
using namespace ABI::Windows::UI::ViewManagement;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;

/* All of this is win10 stuff and we're compiling against win81 headers
 * for now, so we may need to do some legwork: */
#  if WINVER_MAXVER < 0x0A00
namespace ABI {
namespace Windows {
namespace UI {
namespace ViewManagement {
enum UserInteractionMode {
  UserInteractionMode_Mouse = 0,
  UserInteractionMode_Touch = 1
};
}
}  // namespace UI
}  // namespace Windows
}  // namespace ABI

#  endif

#  ifndef RuntimeClass_Windows_UI_ViewManagement_UIViewSettings
#    define RuntimeClass_Windows_UI_ViewManagement_UIViewSettings \
      L"Windows.UI.ViewManagement.UIViewSettings"
#  endif

#  if WINVER_MAXVER < 0x0A00
namespace ABI {
namespace Windows {
namespace UI {
namespace ViewManagement {
interface IUIViewSettings;
MIDL_INTERFACE("C63657F6-8850-470D-88F8-455E16EA2C26")
IUIViewSettings : public IInspectable {
 public:
  virtual HRESULT STDMETHODCALLTYPE get_UserInteractionMode(
      UserInteractionMode * value) = 0;
};

extern const __declspec(selectany) IID& IID_IUIViewSettings =
    __uuidof(IUIViewSettings);
}  // namespace ViewManagement
}  // namespace UI
}  // namespace Windows
}  // namespace ABI
#  endif

#  ifndef IUIViewSettingsInterop

typedef interface IUIViewSettingsInterop IUIViewSettingsInterop;

MIDL_INTERFACE("3694dbf9-8f68-44be-8ff5-195c98ede8a6")
IUIViewSettingsInterop : public IInspectable {
 public:
  virtual HRESULT STDMETHODCALLTYPE GetForWindow(HWND hwnd, REFIID riid,
                                                 void** ppv) = 0;
};
#  endif

#endif

WindowsUIUtils::WindowsUIUtils() : mInTabletMode(eTabletModeUnknown) {}

WindowsUIUtils::~WindowsUIUtils() {}

/*
 * Implement the nsISupports methods...
 */
NS_IMPL_ISUPPORTS(WindowsUIUtils, nsIWindowsUIUtils)

NS_IMETHODIMP
WindowsUIUtils::GetInTabletMode(bool* aResult) {
  if (mInTabletMode == eTabletModeUnknown) {
    UpdateTabletModeState();
  }
  *aResult = mInTabletMode == eTabletModeOn;
  return NS_OK;
}

NS_IMETHODIMP
WindowsUIUtils::UpdateTabletModeState() {
#ifndef __MINGW32__
  if (!IsWin10OrLater()) {
    return NS_OK;
  }

  nsresult rv;
  nsCOMPtr<nsIWindowMediator> winMediator(
      do_GetService(NS_WINDOWMEDIATOR_CONTRACTID, &rv));
  if (NS_FAILED(rv)) {
    return rv;
  }

  nsCOMPtr<nsIWidget> widget;
  nsCOMPtr<mozIDOMWindowProxy> navWin;

  rv = winMediator->GetMostRecentWindow(u"navigator:browser",
                                        getter_AddRefs(navWin));
  if (NS_FAILED(rv) || !navWin) {
    // Fall back to the hidden window
    nsCOMPtr<nsIAppShellService> appShell(
        do_GetService(NS_APPSHELLSERVICE_CONTRACTID));

    rv = appShell->GetHiddenDOMWindow(getter_AddRefs(navWin));
    if (NS_FAILED(rv) || !navWin) {
      return rv;
    }
  }

  nsPIDOMWindowOuter* win = nsPIDOMWindowOuter::From(navWin);
  widget = widget::WidgetUtils::DOMWindowToWidget(win);

  if (!widget) return NS_ERROR_FAILURE;

  HWND winPtr = (HWND)widget->GetNativeData(NS_NATIVE_WINDOW);
  ComPtr<IUIViewSettingsInterop> uiViewSettingsInterop;

  HRESULT hr = GetActivationFactory(
      HStringReference(RuntimeClass_Windows_UI_ViewManagement_UIViewSettings)
          .Get(),
      &uiViewSettingsInterop);
  if (SUCCEEDED(hr)) {
    ComPtr<IUIViewSettings> uiViewSettings;
    hr = uiViewSettingsInterop->GetForWindow(winPtr,
                                             IID_PPV_ARGS(&uiViewSettings));
    if (SUCCEEDED(hr)) {
      UserInteractionMode mode;
      hr = uiViewSettings->get_UserInteractionMode(&mode);
      if (SUCCEEDED(hr)) {
        TabletModeState oldTabletModeState = mInTabletMode;
        mInTabletMode = (mode == UserInteractionMode_Touch) ? eTabletModeOn
                                                            : eTabletModeOff;
        if (mInTabletMode != oldTabletModeState) {
          nsCOMPtr<nsIObserverService> observerService =
              mozilla::services::GetObserverService();
          observerService->NotifyObservers(
              nullptr, "tablet-mode-change",
              ((mInTabletMode == eTabletModeOn) ? u"tablet-mode"
                                                : u"normal-mode"));
        }
      }
    }
  }
#endif

  return NS_OK;
}
