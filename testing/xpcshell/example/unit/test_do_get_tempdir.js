/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

/* This tests that do_get_tempdir returns a directory that we can write to. */

function run_test() {
  let tmpd = do_get_tempdir();
  Assert.ok(tmpd.exists());
  tmpd.append("testfile");
  tmpd.create(Ci.nsIFile.NORMAL_FILE_TYPE, 600);
  Assert.ok(tmpd.exists());
}
