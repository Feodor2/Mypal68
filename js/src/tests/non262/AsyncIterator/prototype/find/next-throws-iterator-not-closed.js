// |reftest| skip-if(!this.hasOwnProperty('AsyncIterator'))

class TestIterator extends AsyncIterator {
  next() {
    throw new Error();
  }

  closed = false;
  return() {
    this.closed = true;
  }
}

const fn = x => x;
const iter = new TestIterator();

assertEq(iter.closed, false);
iter.find(fn).then(() => assertEq(true, false, 'expected error'), err => {
  assertEq(err instanceof Error, true);
  assertEq(iter.closed, false);
});

if (typeof reportCompare === 'function')
  reportCompare(0, 0);
