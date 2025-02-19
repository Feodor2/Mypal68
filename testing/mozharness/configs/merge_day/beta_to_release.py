import os

ABS_WORK_DIR = os.path.join(os.getcwd(), "build")

config = {
    "log_name": "beta_to_release",
    "copy_files": [
        {
            "src": "browser/config/version.txt",
            "dst": "browser/config/version_display.txt",
        },
    ],
    "replacements": [
        # File, from, to
        ("{}{}".format(d, f),
        "ac_add_options --with-branding=mobile/android/branding/beta",
        "ac_add_options --with-branding=mobile/android/branding/official")
        for d in ["mobile/android/config/mozconfigs/android-aarch64/",
                  "mobile/android/config/mozconfigs/android-api-16/",
                  "mobile/android/config/mozconfigs/android-x86/",
                  "mobile/android/config/mozconfigs/android-x86_64/"]
        for f in ["debug", "nightly"]
    ] + [
        # File, from, to
    ],

    "vcs_share_base": os.path.join(ABS_WORK_DIR, 'hg-shared'),
    # "hg_share_base": None,
    "from_repo_url": "https://hg.mozilla.org/releases/mozilla-beta",
    "to_repo_url": "https://hg.mozilla.org/releases/mozilla-release",

    "base_tag": "FIREFOX_RELEASE_%(major_version)s_BASE",
    "end_tag": "FIREFOX_RELEASE_%(major_version)s_END",

    "migration_behavior": "beta_to_release",
    "require_remove_locales": False,
    "pull_all_branches": True,

    "virtualenv_modules": [
        "requests==2.8.1",
    ],
}
