/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

add_task(async function test() {
  let engine = await promiseNewEngine("testEngine.xml");
  let histogramKey = "other-" + engine.name + ".contextmenu";
  let numSearchesBefore = 0;
  let tabs = [];
  let tabsLoadedDeferred = new Deferred();

  function tabAdded(event) {
    let tab = event.target;
    tabs.push(tab);

    // We wait for the blank tab and the two context searches tabs to open.
    if (tabs.length == 3) {
      tabsLoadedDeferred.resolve();
    }
  }

  let container = gBrowser.tabContainer;
  container.addEventListener("TabOpen", tabAdded);

  BrowserTestUtils.addTab(gBrowser, "about:blank");
  BrowserSearch.loadSearchFromContext(
    "mozilla",
    Services.scriptSecurityManager.getSystemPrincipal()
  );
  BrowserSearch.loadSearchFromContext(
    "firefox",
    Services.scriptSecurityManager.getSystemPrincipal()
  );

  // Wait for all the tabs to open.
  await tabsLoadedDeferred.promise;

  is(tabs[0], gBrowser.tabs[3], "blank tab has been pushed to the end");
  is(
    tabs[1],
    gBrowser.tabs[1],
    "first search tab opens next to the current tab"
  );
  is(
    tabs[2],
    gBrowser.tabs[2],
    "second search tab opens next to the first search tab"
  );

  container.removeEventListener("TabOpen", tabAdded);
  tabs.forEach(gBrowser.removeTab, gBrowser);
});

function Deferred() {
  this.promise = new Promise((resolve, reject) => {
    this.resolve = resolve;
    this.reject = reject;
  });
}
