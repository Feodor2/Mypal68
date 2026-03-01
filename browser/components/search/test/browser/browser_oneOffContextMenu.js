"use strict";

const TEST_ENGINE_NAME = "Foo";
const TEST_ENGINE_BASENAME = "testEngine.xml";

const searchPopup = document.getElementById("PopupSearchAutoComplete");
const oneOffInstance = searchPopup.oneOffButtons;
const contextMenu = oneOffInstance.querySelector(
  ".search-one-offs-context-menu"
);
const oneOffButtons = oneOffInstance.buttons;
const searchInNewTabMenuItem = oneOffInstance.querySelector(
  ".search-one-offs-context-open-in-new-tab"
);

let searchbar;
let searchIcon;

add_task(async function init() {
  searchbar = await gCUITestUtils.addSearchBar();
  registerCleanupFunction(() => {
    gCUITestUtils.removeSearchBar();
  });
  searchIcon = searchbar.querySelector(".searchbar-search-button");

  await promiseNewEngine(TEST_ENGINE_BASENAME, {
    setAsCurrent: false,
  });
});
