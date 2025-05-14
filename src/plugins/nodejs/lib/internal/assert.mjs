///////////////////////////////////////////////////////
// code moved to errors module to avoid cyclical import statements
/* 'use strict';
import {codes} from 'internal/errors'
let error;
function lazyError() {
  return error ??= codes.ERR_INTERNAL_ASSERTION;
}

function assert(value, message) {
  if (!value) {
    const ERR_INTERNAL_ASSERTION = lazyError();
    throw new ERR_INTERNAL_ASSERTION(message);
  }
}

function fail(message) {
  const ERR_INTERNAL_ASSERTION = lazyError();
  throw new ERR_INTERNAL_ASSERTION(message);
}

assert.fail = fail;

export default assert;
 */