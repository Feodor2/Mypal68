/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsFilePicker_h__
#define nsFilePicker_h__

#include <windows.h>

#include "nsIFile.h"
#include "nsITimer.h"
#include "nsISimpleEnumerator.h"
#include "nsCOMArray.h"
#include "nsBaseFilePicker.h"
#include "nsString.h"
#include "nsdefs.h"
#include <commdlg.h>
#include <shobjidl.h>
#undef LogSeverity  // SetupAPI.h #defines this as DWORD

typedef /* [v1_enum] */
    enum FDE_OVERWRITE_RESPONSE {
      FDEOR_DEFAULT = 0,
      FDEOR_ACCEPT = 1,
      FDEOR_REFUSE = 2
    } FDE_OVERWRITE_RESPONSE;

typedef /* [v1_enum] */
    enum FDE_SHAREVIOLATION_RESPONSE {
      FDESVR_DEFAULT = 0,
      FDESVR_ACCEPT = 1,
      FDESVR_REFUSE = 2
    } FDE_SHAREVIOLATION_RESPONSE;
typedef /* [v1_enum] */
    enum FDAP {
      FDAP_BOTTOM = 0,
      FDAP_TOP = 1
    } FDAP;
typedef struct _COMDLG_FILTERSPEC {
  /* [string] */ LPCWSTR pszName;
  /* [string] */ LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;
/* [v1_enum] */
enum _FILEOPENDIALOGOPTIONS {
  FOS_OVERWRITEPROMPT = 0x2,
  FOS_STRICTFILETYPES = 0x4,
  FOS_NOCHANGEDIR = 0x8,
  FOS_PICKFOLDERS = 0x20,
  FOS_FORCEFILESYSTEM = 0x40,
  FOS_ALLNONSTORAGEITEMS = 0x80,
  FOS_NOVALIDATE = 0x100,
  FOS_ALLOWMULTISELECT = 0x200,
  FOS_PATHMUSTEXIST = 0x800,
  FOS_FILEMUSTEXIST = 0x1000,
  FOS_CREATEPROMPT = 0x2000,
  FOS_SHAREAWARE = 0x4000,
  FOS_NOREADONLYRETURN = 0x8000,
  FOS_NOTESTFILECREATE = 0x10000,
  FOS_HIDEMRUPLACES = 0x20000,
  FOS_HIDEPINNEDPLACES = 0x40000,
  FOS_NODEREFERENCELINKS = 0x100000,
  FOS_DONTADDTORECENT = 0x2000000,
  FOS_FORCESHOWHIDDEN = 0x10000000,
  FOS_DEFAULTNOMINIMODE = 0x20000000,
  FOS_FORCEPREVIEWPANEON = 0x40000000,
  FOS_SUPPORTSTREAMABLEITEMS = 0x80000000
};
typedef DWORD FILEOPENDIALOGOPTIONS;

EXTERN_C const IID IID_IFileDialog;

MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
IFileDialog : public IModalWindow {
 public:
  virtual HRESULT STDMETHODCALLTYPE SetFileTypes(
      /* [in] */ UINT cFileTypes,
      /* [size_is][in] */ __RPC__in_ecount_full(cFileTypes)
          const COMDLG_FILTERSPEC* rgFilterSpec) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(
      /* [in] */ UINT iFileType) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(
      /* [out] */ __RPC__out UINT * piFileType) = 0;

  virtual HRESULT STDMETHODCALLTYPE Advise(
      /* [in] */ __RPC__in_opt IFileDialogEvents * pfde,
      /* [out] */ __RPC__out DWORD * pdwCookie) = 0;

  virtual HRESULT STDMETHODCALLTYPE Unadvise(
      /* [in] */ DWORD dwCookie) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetOptions(
      /* [in] */ FILEOPENDIALOGOPTIONS fos) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetOptions(
      /* [out] */ __RPC__out FILEOPENDIALOGOPTIONS * pfos) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(
      /* [in] */ __RPC__in_opt IShellItem * psi) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetFolder(
      /* [in] */ __RPC__in_opt IShellItem * psi) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetFolder(
      /* [out] */ __RPC__deref_out_opt IShellItem * *ppsi) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(
      /* [out] */ __RPC__deref_out_opt IShellItem * *ppsi) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetFileName(
      /* [string][in] */ __RPC__in_string LPCWSTR pszName) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetFileName(
      /* [string][out] */ __RPC__deref_out_opt_string LPWSTR * pszName) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetTitle(
      /* [string][in] */ __RPC__in_string LPCWSTR pszTitle) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(
      /* [string][in] */ __RPC__in_string LPCWSTR pszText) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(
      /* [string][in] */ __RPC__in_string LPCWSTR pszLabel) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetResult(
      /* [out] */ __RPC__deref_out_opt IShellItem * *ppsi) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddPlace(
      /* [in] */ __RPC__in_opt IShellItem * psi,
      /* [in] */ FDAP fdap) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(
      /* [string][in] */ __RPC__in_string LPCWSTR pszDefaultExtension) = 0;

  virtual HRESULT STDMETHODCALLTYPE Close(
      /* [in] */ HRESULT hr) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetClientGuid(
      /* [in] */ __RPC__in REFGUID guid) = 0;

  virtual HRESULT STDMETHODCALLTYPE ClearClientData(void) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetFilter(
      /* [in] */ __RPC__in_opt IShellItemFilter * pFilter) = 0;
};

EXTERN_C const IID IID_IFileOpenDialog;

MIDL_INTERFACE("d57c7288-d4ad-4768-be02-9d969532d960")
IFileOpenDialog : public IFileDialog {
 public:
  virtual HRESULT STDMETHODCALLTYPE GetResults(
      /* [out] */ __RPC__deref_out_opt IShellItemArray * *ppenum) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(
      /* [out] */ __RPC__deref_out_opt IShellItemArray * *ppsai) = 0;
};

EXTERN_C const IID IID_IFileDialogEvents;

MIDL_INTERFACE("973510db-7d7f-452b-8975-74a85828d354")
IFileDialogEvents : public IUnknown {
 public:
  virtual HRESULT STDMETHODCALLTYPE OnFileOk(
      /* [in] */ __RPC__in_opt IFileDialog * pfd) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(
      /* [in] */ __RPC__in_opt IFileDialog * pfd,
      /* [in] */ __RPC__in_opt IShellItem * psiFolder) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnFolderChange(
      /* [in] */ __RPC__in_opt IFileDialog * pfd) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(
      /* [in] */ __RPC__in_opt IFileDialog * pfd) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnShareViolation(
      /* [in] */ __RPC__in_opt IFileDialog * pfd,
      /* [in] */ __RPC__in_opt IShellItem * psi,
      /* [out] */ __RPC__out FDE_SHAREVIOLATION_RESPONSE * pResponse) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnTypeChange(
      /* [in] */ __RPC__in_opt IFileDialog * pfd) = 0;

  virtual HRESULT STDMETHODCALLTYPE OnOverwrite(
      /* [in] */ __RPC__in_opt IFileDialog * pfd,
      /* [in] */ __RPC__in_opt IShellItem * psi,
      /* [out] */ __RPC__out FDE_OVERWRITE_RESPONSE * pResponse) = 0;
};

EXTERN_C const IID IID_IFileSaveDialog;

class nsILoadContext;

class nsBaseWinFilePicker : public nsBaseFilePicker {
 public:
  NS_IMETHOD GetDefaultString(nsAString& aDefaultString);
  NS_IMETHOD SetDefaultString(const nsAString& aDefaultString);
  NS_IMETHOD GetDefaultExtension(nsAString& aDefaultExtension);
  NS_IMETHOD SetDefaultExtension(const nsAString& aDefaultExtension);

 protected:
  nsString mDefaultFilePath;
  nsString mDefaultFilename;
  nsString mDefaultExtension;
};

/**
 * Native Windows FileSelector wrapper
 */

class nsFilePicker : public IFileDialogEvents, public nsBaseWinFilePicker {
  virtual ~nsFilePicker();

 public:
  nsFilePicker();

  NS_IMETHOD Init(mozIDOMWindowProxy* aParent, const nsAString& aTitle,
                  int16_t aMode) override;

  NS_DECL_ISUPPORTS

  // IUnknown's QueryInterface
  STDMETHODIMP QueryInterface(REFIID refiid, void** ppvResult);

  // nsIFilePicker (less what's in nsBaseFilePicker and nsBaseWinFilePicker)
  NS_IMETHOD GetFilterIndex(int32_t* aFilterIndex);
  NS_IMETHOD SetFilterIndex(int32_t aFilterIndex);
  NS_IMETHOD GetFile(nsIFile** aFile);
  NS_IMETHOD GetFileURL(nsIURI** aFileURL);
  NS_IMETHOD GetFiles(nsISimpleEnumerator** aFiles);
  nsresult Show(int16_t* aReturnVal);
  nsresult ShowW(int16_t* aReturnVal);
  NS_IMETHOD AppendFilter(const nsAString& aTitle, const nsAString& aFilter);

  // IFileDialogEvents
  HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog* pfd,
                                             IShellItem* psiFolder);
  HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE
  OnShareViolation(IFileDialog* pfd, IShellItem* psi,
                   FDE_SHAREVIOLATION_RESPONSE* pResponse);
  HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog* pfd);
  HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog* pfd, IShellItem* psi,
                                        FDE_OVERWRITE_RESPONSE* pResponse);

 protected:
  enum PickerType {
    PICKER_TYPE_OPEN,
    PICKER_TYPE_SAVE,
  };

  /* method from nsBaseFilePicker */
  virtual void InitNative(nsIWidget* aParent, const nsAString& aTitle);
  static void GetQualifiedPath(const wchar_t* aInPath, nsString& aOutPath);
  void GetFilterListArray(nsString& aFilterList);
  static bool GetFileNameWrapper(OPENFILENAMEW* ofn, PickerType aType);
  bool FilePickerWrapper(OPENFILENAMEW* ofn, PickerType aType);
  bool ShowXPFolderPicker(const nsString& aInitialDir);
  bool ShowXPFilePicker(const nsString& aInitialDir);
  bool ShowFolderPicker(const nsString& aInitialDir, bool& aWasInitError);
  bool ShowFilePicker(const nsString& aInitialDir, bool& aWasInitError);
  void AppendXPFilter(const nsAString& aTitle, const nsAString& aFilter);
  void RememberLastUsedDirectory();
  bool IsPrivacyModeEnabled();
  bool IsDefaultPathLink();
  bool IsDefaultPathHtml();
  void SetDialogHandle(HWND aWnd);
  bool ClosePickerIfNeeded(bool aIsXPDialog);
  static void PickerCallbackTimerFunc(nsITimer* aTimer, void* aPicker);
  static UINT_PTR CALLBACK MultiFilePickerHook(HWND hwnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam);
  static UINT_PTR CALLBACK FilePickerHook(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam);

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
#  undef _WIN32_WINNT
#  define _WIN32_WINNT _WIN32_WINNT_bak
#  undef _WIN32_IE
#  define _WIN32_IE _WIN32_IE_bak
#endif

#endif  // nsFilePicker_h__
