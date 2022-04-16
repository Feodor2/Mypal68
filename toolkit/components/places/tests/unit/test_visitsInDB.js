add_task(async function test_execute() {
  const TEST_URI = uri("http://mozilla.com");

  Assert.equal(0, await PlacesTestUtils.visitsInDB(TEST_URI));
  await PlacesTestUtils.addVisits({ uri: TEST_URI });
  Assert.equal(1, await PlacesTestUtils.visitsInDB(TEST_URI));
  await PlacesTestUtils.addVisits({ uri: TEST_URI });
  Assert.equal(2, await PlacesTestUtils.visitsInDB(TEST_URI));
});
