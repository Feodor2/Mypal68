/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nsXPLookAndFeel
#define __nsXPLookAndFeel

#include "mozilla/LookAndFeel.h"
#include "nsTArray.h"

class nsLookAndFeel;

class nsXPLookAndFeel : public mozilla::LookAndFeel {
 public:
  virtual ~nsXPLookAndFeel();

  static nsXPLookAndFeel* GetInstance();
  static void Shutdown();

  void Init();

  // These functions will return a value specified by an override pref, if it
  // exists, and otherwise will call into the NativeGetXxx function to get the
  // platform-specific value.
  //
  // NS_ERROR_NOT_AVAILABLE is returned if there is neither an override pref or
  // a platform-specific value.
  nsresult GetColorValue(ColorID aID, bool aUseStandinsForNativeColors,
                         nscolor& aResult);
  nsresult GetIntValue(IntID aID, int32_t& aResult);
  nsresult GetFloatValue(FloatID aID, float& aResult);
  // Same, but returns false if there is no platform-specific value.
  // (There are no override prefs for font values.)
  bool GetFontValue(FontID aID, nsString& aName, gfxFontStyle& aStyle) {
    return NativeGetFont(aID, aName, aStyle);
  }

  virtual nsresult NativeGetInt(IntID aID, int32_t& aResult) = 0;
  virtual nsresult NativeGetFloat(FloatID aID, float& aResult) = 0;
  virtual nsresult NativeGetColor(ColorID aID, nscolor& aResult) = 0;
  virtual bool NativeGetFont(FontID aID, nsString& aName,
                             gfxFontStyle& aStyle) = 0;

  virtual void RefreshImpl();

  virtual char16_t GetPasswordCharacterImpl() { return char16_t('*'); }

  virtual bool GetEchoPasswordImpl() { return false; }

  virtual uint32_t GetPasswordMaskDelayImpl() { return 600; }

  virtual nsTArray<LookAndFeelInt> GetIntCacheImpl();
  virtual void SetIntCacheImpl(
      const nsTArray<LookAndFeelInt>& aLookAndFeelIntCache) {}

  virtual void NativeInit() = 0;

 protected:
  nsXPLookAndFeel() = default;

  static nscolor GetStandinForNativeColor(ColorID);

  static void OnPrefChanged(const char* aPref, void* aClosure);

  static bool sInitialized;

  static nsXPLookAndFeel* sInstance;
  static bool sShutdown;
};

#endif
