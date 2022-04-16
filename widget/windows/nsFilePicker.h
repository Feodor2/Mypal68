/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsFilePicker_h__
#define nsFilePicker_h__

#include <windows.h>

typedef /* [v1_enum] */
/*enum FDE_OVERWRITE_RESPONSE
    {
        FDEOR_DEFAULT	= 0,
        FDEOR_ACCEPT	= 1,
        FDEOR_REFUSE	= 2
    } 	FDE_OVERWRITE_RESPONSE;

typedef /* [v1_enum] */
/*enum FDE_SHAREVIOLATION_RESPONSE
    {
        FDESVR_DEFAULT	= 0,
        FDESVR_ACCEPT	= 1,
        FDESVR_REFUSE	= 2
    } 	FDE_SHAREVIOLATION_RESPONSE;*/

// For Vista IFileDialog interfaces which aren't exposed
// unless _WIN32_WINNT >= _WIN32_WINNT_LONGHORN.
#if _WIN32_WINNT < _WIN32_WINNT_LONGHORN
#define _WIN32_WINNT_bak _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#define _WIN32_IE_bak _WIN32_IE
#undef _WIN32_IE
#define _WIN32_IE _WIN32_IE_IE70
#endif

#include "nsIFile.h"
#include "nsITimer.h"
#include "nsISimpleEnumerator.h"
#include "nsCOMArray.h"
#include "nsBaseFilePicker.h"
#include "nsString.h"
#include "nsdefs.h"
#include <commdlg.h>
//#include <shobjidl.h>
//#include <shobjidl_core.h>
#undef LogSeverity  // SetupAPI.h #defines this as DWORD

#ifndef _COMDLG_FILTERSPEC
typedef struct _COMDLG_FILTERSPEC
    {
    /* [string] */ LPCWSTR pszName;
    /* [string] */ LPCWSTR pszSpec;
    } 	COMDLG_FILTERSPEC;
#endif

class nsILoadContext;

class nsBaseWinFilePicker : public nsBaseFilePicker {
 public:
  NS_IMETHOD GetDefaultString(nsAString& aDefaultString) override;
  NS_IMETHOD SetDefaultString(const nsAString& aDefaultString) override;
  NS_IMETHOD GetDefaultExtension(nsAString& aDefaultExtension) override;
  NS_IMETHOD SetDefaultExtension(const nsAString& aDefaultExtension) override;

 protected:
  nsString mDefaultFilePath;
  nsString mDefaultFilename;
  nsString mDefaultExtension;
};

/**
 * Native Windows FileSelector wrapper
 */

//class nsFilePicker : public IFileDialogEvents, public nsBaseWinFilePicker {
class nsFilePicker : public nsBaseWinFilePicker {
  virtual ~nsFilePicker();

 public:
  nsFilePicker();

  NS_IMETHOD Init(mozIDOMWindowProxy* aParent, const nsAString& aTitle,
                  int16_t aMode) override;

  NS_DECL_ISUPPORTS

  // IUnknown's QueryInterface
  STDMETHODIMP QueryInterface(REFIID refiid, void** ppvResult);

  // nsIFilePicker (less what's in nsBaseFilePicker and nsBaseWinFilePicker)
  NS_IMETHOD GetFilterIndex(int32_t* aFilterIndex) override;
  NS_IMETHOD SetFilterIndex(int32_t aFilterIndex) override;
  NS_IMETHOD GetFile(nsIFile** aFile) override;
  NS_IMETHOD GetFileURL(nsIURI** aFileURL) override;
  NS_IMETHOD GetFiles(nsISimpleEnumerator** aFiles) override;
  NS_IMETHOD AppendFilter(const nsAString& aTitle,
                          const nsAString& aFilter) override;

  // IFileDialogEvents
  /*HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog* pfd,
                                             IShellItem* psiFolder);
  HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE
  OnShareViolation(IFileDialog* pfd, IShellItem* psi,
                   FDE_SHAREVIOLATION_RESPONSE* pResponse);
  HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog* pfd, IShellItem* psi,
                                        FDE_OVERWRITE_RESPONSE* pResponse);*/

 protected:
  enum PickerType {
    PICKER_TYPE_OPEN,
    PICKER_TYPE_SAVE,
  };

  /* method from nsBaseFilePicker */
  virtual void InitNative(nsIWidget* aParent, const nsAString& aTitle) override;
  static void GetQualifiedPath(const wchar_t *aInPath, nsString &aOutPath);
  nsresult Show(int16_t* aReturnVal) override;
  nsresult ShowW(int16_t* aReturnVal);
  void GetFilterListArray(nsString& aFilterList);
  static bool GetFileNameWrapper(OPENFILENAMEW* ofn, PickerType aType);
  bool FilePickerWrapper(OPENFILENAMEW* ofn, PickerType aType);
  bool ShowXPFolderPicker(const nsString& aInitialDir);
  bool ShowXPFilePicker(const nsString& aInitialDir);
  bool ShowFolderPicker(const nsString& aInitialDir, bool &aWasInitError);
  bool ShowFilePicker(const nsString& aInitialDir, bool &aWasInitError);
  void AppendXPFilter(const nsAString& aTitle, const nsAString& aFilter);
  void RememberLastUsedDirectory();
  bool IsPrivacyModeEnabled();
  bool IsDefaultPathLink();
  bool IsDefaultPathHtml();
  void SetDialogHandle(HWND aWnd);
  bool ClosePickerIfNeeded(bool aIsXPDialog);
  static void PickerCallbackTimerFunc(nsITimer* aTimer, void* aPicker);
  static UINT_PTR CALLBACK MultiFilePickerHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  static UINT_PTR CALLBACK FilePickerHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  nsCOMPtr<nsILoadContext> mLoadContext;
  nsCOMPtr<nsIWidget> mParentWidget;
  nsString mTitle;
  nsCString mFile;
  nsString mFilterList;
  int16_t mSelectedType;
  nsCOMArray<nsIFile> mFiles;
  static char mLastUsedDirectory[];
  nsString mUnicodeFile;
  static char16_t* mLastUsedUnicodeDirectory;
  HWND mDlgWnd;

  class ComDlgFilterSpec {
   public:
    ComDlgFilterSpec() {}
    ~ComDlgFilterSpec() {}

    const uint32_t Length() { return mSpecList.Length(); }

    const bool IsEmpty() { return (mSpecList.Length() == 0); }

    const COMDLG_FILTERSPEC* get() { return mSpecList.Elements(); }

    void Append(const nsAString& aTitle, const nsAString& aFilter);

   private:
    AutoTArray<COMDLG_FILTERSPEC, 1> mSpecList;
    AutoTArray<nsString, 2> mStrings;
  };

  ComDlgFilterSpec mComFilterList;
  DWORD mFDECookie;
};

#if defined(_WIN32_WINNT_bak)
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_bak
#undef _WIN32_IE
#define _WIN32_IE _WIN32_IE_bak
#endif

#endif  // nsFilePicker_h__
