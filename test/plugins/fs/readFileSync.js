"use strict";
require("console")
const fs = require("fs")
const binding = require("fs")


const Error = {};
Error.stackTraceLimit = 0
const ReflectApply = Reflect.apply
const S_IFREG=32768
const S_IFMT=61440

function StringPrototypeIncludes(value) {

}

function TypedArrayPrototypeGetSymbolToStringTag(value) {

}
function TypedArrayPrototypeGetSymbolToStringTag(value) {

}




function isUint8Array(value) {
	return TypedArrayPrototypeGetSymbolToStringTag(value) === 'Uint8Array';
  }
  function parseFileMode(value, name, def) {
	value ??= def;
	if (typeof value === 'string') {
	  if (RegExpPrototypeExec(octalReg, value) === null) {
		throw new ERR_INVALID_ARG_VALUE(name, value, modeDesc);
	  }
	  value = NumberParseInt(value, 8);
	}
  
	validateUint32(value, name);
	return value;
  }
const O_RDONLY = 0;

  function stringToFlags(flags, name = 'flags') {
	if (typeof flags === 'number') {
	  validateInt32(flags, name);
	  return flags;
	}
  
	if (flags == null) {
	  return O_RDONLY;
	}
  
	switch (flags) {
	  case 'r' : return O_RDONLY;
	  case 'rs' : // Fall through.
	  case 'sr' : return O_RDONLY | O_SYNC;
	  case 'r+' : return O_RDWR;
	  case 'rs+' : // Fall through.
	  case 'sr+' : return O_RDWR | O_SYNC;
  
	  case 'w' : return O_TRUNC | O_CREAT | O_WRONLY;
	  case 'wx' : // Fall through.
	  case 'xw' : return O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;
  
	  case 'w+' : return O_TRUNC | O_CREAT | O_RDWR;
	  case 'wx+': // Fall through.
	  case 'xw+': return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;
  
	  case 'a' : return O_APPEND | O_CREAT | O_WRONLY;
	  case 'ax' : // Fall through.
	  case 'xa' : return O_APPEND | O_CREAT | O_WRONLY | O_EXCL;
	  case 'as' : // Fall through.
	  case 'sa' : return O_APPEND | O_CREAT | O_WRONLY | O_SYNC;
  
	  case 'a+' : return O_APPEND | O_CREAT | O_RDWR;
	  case 'ax+': // Fall through.
	  case 'xa+': return O_APPEND | O_CREAT | O_RDWR | O_EXCL;
	  case 'as+': // Fall through.
	  case 'sa+': return O_APPEND | O_CREAT | O_RDWR | O_SYNC;
	}
  
	throw new ERR_INVALID_ARG_VALUE('flags', flags);
  }
  function NumberIsInteger(value) {
	return Number.isInteger(value)
  }
  const validateUint32 = hideStackFrames((value, name, positive = false) => {
	if (typeof value !== 'number') {
	  throw new ERR_INVALID_ARG_TYPE(name, 'number', value);
	}
	if (!NumberIsInteger(value)) {
	  throw new ERR_OUT_OF_RANGE(name, 'an integer', value);
	}
	const min = positive ? 1 : 0;
	// 2 ** 32 === 4294967296
	const max = 4_294_967_295;
	if (value < min || value > max) {
	  throw new ERR_OUT_OF_RANGE(name, `>= ${min} && <= ${max}`, value);
	}
  });
const validatePath = hideStackFrames((path, propName = 'path') => {
	if (typeof path !== 'string' && !isUint8Array(path)) {
	  throw new ERR_INVALID_ARG_TYPE.HideStackFramesError(propName, ['string', 'Buffer', 'URL'], path);
	}
  
	const pathIsString = typeof path === 'string';
	const pathIsUint8Array = isUint8Array(path);
  
	// We can only perform meaningful checks on strings and Uint8Arrays.
	if ((!pathIsString && !pathIsUint8Array) ||
		(pathIsString && !StringPrototypeIncludes(path, '\u0000')) ||
		(pathIsUint8Array && !TypedArrayPrototypeIncludes(path, 0))) {
	  return;
	}
  
	throw new ERR_INVALID_ARG_VALUE.HideStackFramesError(
	  propName,
	  path,
	  'must be a string, Uint8Array, or URL without null bytes',
	);
  });

function isURL(self) {
	return Boolean(self?.href && self.protocol && self.auth === undefined && self.path === undefined);
  }

function toPathIfFileURL(fileURLOrPath) {
	if (!isURL(fileURLOrPath))
	  return fileURLOrPath;
	return fileURLToPath(fileURLOrPath);
  }

//console.debug(buffer)
const getValidatedPath = hideStackFrames((fileURLOrPath, propName = 'path') => {
	const path = toPathIfFileURL(fileURLOrPath);
	validatePath(path, propName);
	return path;
  });
  
  const getValidatedFd = hideStackFrames((fd, propName = 'fd') => {
	if (ObjectIs(fd, -0)) {
	  return 0;
	}
  
	validateInt32(fd, propName, 0);
  
	return fd;
  });

function hideStackFrames(fn) {
	function wrappedFn(...args) {
	  try {
		return ReflectApply(fn, this, args);
	  } catch (error) {
		Error.stackTraceLimit && ErrorCaptureStackTrace(error, wrappedFn);
		throw error;
	  }
	}
	wrappedFn.withoutStackTrace = fn;
	return wrappedFn;
  }

function isInt32(value) {
  return value === (value | 0);
}

function isFd(value) {
	return value === (value | 0);
  }

function getOptions(options, defaultOptions = kEmptyObject) {
	if (options == null || typeof options === 'function') {
	  return defaultOptions;
	}
  
	if (typeof options === 'string') {
	  defaultOptions = { ...defaultOptions };
	  defaultOptions.encoding = options;
	  options = defaultOptions;
	} else if (typeof options !== 'object') {
	  throw new ERR_INVALID_ARG_TYPE('options', ['string', 'Object'], options);
	}
  
	if (options.encoding !== 'buffer')
	  assertEncoding(options.encoding);
  
	if (options.signal !== undefined) {
	  validateAbortSignal(options.signal, 'options.signal');
	}
  
	return options;
}
function openSync(path, flags, mode) {
	console.log(stringToFlags(flags))
	return binding.open(
	  getValidatedPath(path),
	  stringToFlags(flags),
	  parseFileMode(mode, 'mode', 0o666),
	);
  }
  function tryStatSync(fd, isUserFd) {
	const stats = binding.fstat(fd, false, undefined, true /* shouldNotThrow */);
	if (stats === undefined && !isUserFd) {
	  fs.closeSync(fd);
	}
	return stats;
  }
  function isFileType(stats, fileType) {
	// Use stats array directly to avoid creating an fs.Stats instance just for
	// our internal use.
	let mode = stats[1];
	if (typeof mode === 'bigint')
	  mode = Number(mode);
	return (mode & S_IFMT) === fileType;
  }
function readFileSync(path, options) {
	options = getOptions(options, { flag: 'r' });
  
	if (options.encoding === 'utf8' || options.encoding === 'utf-8') {
	  if (!isInt32(path)) {
		path = getValidatedPath(path);
	  }
	  return binding.readFileUtf8(path, stringToFlags(options.flag));
	}
	console.debug("readFileSync", path)
	const isUserFd = isFd(path); // File descriptor ownership
	const fd = isUserFd ? path : openSync(path, options.flag, 0o666);
  
	const stats = tryStatSync(fd, isUserFd);
	const size = isFileType(stats, S_IFREG) ? stats[8] : 0;
	let pos = 0;
	let buffer; // Single buffer with file data
	let buffers; // List for when size is unknown
  
	if (size === 0) {
	  buffers = [];
	} else {
	  buffer = tryCreateBuffer(size, fd, isUserFd);
	}
  
	let bytesRead;
  
	if (size !== 0) {
	  do {
		bytesRead = tryReadSync(fd, isUserFd, buffer, pos, size - pos);
		pos += bytesRead;
	  } while (bytesRead !== 0 && pos < size);
	} else {
	  do {
		// The kernel lies about many files.
		// Go ahead and try to read some bytes.
		buffer = Buffer.allocUnsafe(8192);
		bytesRead = tryReadSync(fd, isUserFd, buffer, 0, 8192);
		if (bytesRead !== 0) {
		  ArrayPrototypePush(buffers, buffer.slice(0, bytesRead));
		}
		pos += bytesRead;
	  } while (bytesRead !== 0);
	}
  
	if (!isUserFd)
	  fs.closeSync(fd);
  
	if (size === 0) {
	  // Data was collected into the buffers list.
	  buffer = Buffer.concat(buffers, pos);
	} else if (pos < size) {
	  buffer = buffer.slice(0, pos);
	}
  
	if (options.encoding) buffer = buffer.toString(options.encoding);
	return buffer;
  }
// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.



/* const {
  Array,
  ArrayBufferIsView,
  ArrayIsArray,
  ArrayPrototypeForEach,
  MathFloor,
  MathMin,
  MathTrunc,
  NumberIsInteger,
  NumberIsNaN,
  NumberMAX_SAFE_INTEGER,
  NumberMIN_SAFE_INTEGER,
  ObjectDefineProperties,
  ObjectDefineProperty,
  ObjectPrototypeHasOwnProperty,
  ObjectSetPrototypeOf,
  RegExpPrototypeSymbolReplace,
  StringPrototypeCharCodeAt,
  StringPrototypeSlice,
  StringPrototypeToLowerCase,
  StringPrototypeTrim,
  SymbolSpecies,
  SymbolToPrimitive,
  TypedArrayPrototypeFill,
  TypedArrayPrototypeGetBuffer,
  TypedArrayPrototypeGetByteLength,
  TypedArrayPrototypeGetByteOffset,
  TypedArrayPrototypeGetLength,
  TypedArrayPrototypeSet,
  TypedArrayPrototypeSlice,
  Uint8Array,
  Uint8ArrayPrototype,
} = primordials; */

/* const {
  byteLengthUtf8,
  compare: _compare,
  compareOffset,
  copy: _copy,
  fill: bindingFill,
  isAscii: bindingIsAscii,
  isUtf8: bindingIsUtf8,
  indexOfBuffer,
  indexOfNumber,
  indexOfString,
  swap16: _swap16,
  swap32: _swap32,
  swap64: _swap64,
  kMaxLength,
  kStringMaxLength,
  atob: _atob,
  btoa: _btoa,
} = internalBinding('buffer');
const {
  constants: {
    ALL_PROPERTIES,
    ONLY_ENUMERABLE,
  },
  getOwnNonIndexProperties,
  isInsideNodeModules,
} = internalBinding('util');
const {
  customInspectSymbol,
  lazyDOMException,
  normalizeEncoding,
  kIsEncodingSymbol,
  defineLazyProperties,
  encodingsMap,
  deprecate,
} = require('internal/util');
const {
  isAnyArrayBuffer,
  isArrayBufferView,

  isTypedArray,
} = require('internal/util/types');
const {
  inspect: utilInspect,
} = require('internal/util/inspect');

const {
  codes: {
    ERR_BUFFER_OUT_OF_BOUNDS,
    ERR_INVALID_ARG_TYPE,
    ERR_INVALID_ARG_VALUE,
    ERR_INVALID_BUFFER_SIZE,
    ERR_MISSING_ARGS,
    ERR_OUT_OF_RANGE,
    ERR_UNKNOWN_ENCODING,
  },
  genericNodeError,
} = require('internal/errors');
const {
  validateArray,
  validateBuffer,
  validateInteger,
  validateNumber,
  validateString,
} = require('internal/validators');
// Provide validateInteger() but with kMaxLength as the default maximum value.
const validateOffset = (value, name, min = 0, max = kMaxLength) =>
  validateInteger(value, name, min, max);

const {
    markAsUntransferable,
  addBufferPrototypeMethods,
  createUnsafeBuffer,
} = require('internal/buffer'); */


/* 

const {
  BigInt,
  Float32Array,
  Float64Array,
  MathFloor,
  Number,
  Uint8Array,
} = primordials;

const {
  ERR_BUFFER_OUT_OF_BOUNDS,
  ERR_INVALID_ARG_TYPE,
  ERR_OUT_OF_RANGE,
} = require('internal/errors').codes;
const { validateNumber } = require('internal/validators');
const {
  asciiSlice,
  base64Slice,
  base64urlSlice,
  latin1Slice,
  hexSlice,
  ucs2Slice,
  utf8Slice,
  asciiWriteStatic,
  base64Write,
  base64urlWrite,
  latin1WriteStatic,
  hexWrite,
  ucs2Write,
  utf8WriteStatic,
  getZeroFillToggle,
} = internalBinding('buffer');

const {
  privateSymbols: {
    untransferable_object_private_symbol,
  },
} = internalBinding('util');

const {
  namespace: {
    isBuildingSnapshot,
  },
  addAfterUserSerializeCallback,
} = require('internal/v8/startup_snapshot'); */


/*
* Needs
*	asciiSlice
*
*
*
*
*
*/





// Temporary buffers to convert numbers.
const float32Array = new Float32Array(1);
const uInt8Float32Array = new Uint8Array(float32Array.buffer);
const float64Array = new Float64Array(1);
const uInt8Float64Array = new Uint8Array(float64Array.buffer);

// Check endianness.
float32Array[0] = -1; // 0xBF800000
// Either it is [0, 0, 128, 191] or [191, 128, 0, 0]. It is not possible to
// check this with `os.endianness()` because that is determined at compile time.
const bigEndian = uInt8Float32Array[3] === 0;

function checkBounds(buf, offset, byteLength) {
  validateNumber(offset, 'offset');
  if (buf[offset] === undefined || buf[offset + byteLength] === undefined)
    boundsError(offset, buf.length - (byteLength + 1));
}

function checkInt(value, min, max, buf, offset, byteLength) {
  if (value > max || value < min) {
    const n = typeof min === 'bigint' ? 'n' : '';
    let range;
    if (byteLength > 3) {
      if (min === 0 || min === 0n) {
        range = `>= 0${n} and < 2${n} ** ${(byteLength + 1) * 8}${n}`;
      } else {
        range = `>= -(2${n} ** ${(byteLength + 1) * 8 - 1}${n}) and ` +
                `< 2${n} ** ${(byteLength + 1) * 8 - 1}${n}`;
      }
    } else {
      range = `>= ${min}${n} and <= ${max}${n}`;
    }
    throw new ERR_OUT_OF_RANGE('value', range, value);
  }
  checkBounds(buf, offset, byteLength);
}

function boundsError(value, length, type) {
  if (MathFloor(value) !== value) {
    validateNumber(value, type);
    throw new ERR_OUT_OF_RANGE(type || 'offset', 'an integer', value);
  }

  if (length < 0)
    throw new ERR_BUFFER_OUT_OF_BOUNDS();

  throw new ERR_OUT_OF_RANGE(type || 'offset',
                             `>= ${type ? 1 : 0} and <= ${length}`,
                             value);
}

// Read integers.
function readBigUInt64LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  const lo = first +
    this[++offset] * 2 ** 8 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 24;

  const hi = this[++offset] +
    this[++offset] * 2 ** 8 +
    this[++offset] * 2 ** 16 +
    last * 2 ** 24;

  return BigInt(lo) + (BigInt(hi) << 32n);
}

function readBigUInt64BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  const hi = first * 2 ** 24 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    this[++offset];

  const lo = this[++offset] * 2 ** 24 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    last;

  return (BigInt(hi) << 32n) + BigInt(lo);
}

function readBigInt64LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  const val = this[offset + 4] +
    this[offset + 5] * 2 ** 8 +
    this[offset + 6] * 2 ** 16 +
    (last << 24); // Overflow
  return (BigInt(val) << 32n) +
    BigInt(first +
    this[++offset] * 2 ** 8 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 24);
}

function readBigInt64BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  const val = (first << 24) + // Overflow
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    this[++offset];
  return (BigInt(val) << 32n) +
    BigInt(this[++offset] * 2 ** 24 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    last);
}

function readUIntLE(offset, byteLength) {
  if (offset === undefined)
    throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
  if (byteLength === 6)
    return readUInt48LE(this, offset);
  if (byteLength === 5)
    return readUInt40LE(this, offset);
  if (byteLength === 3)
    return readUInt24LE(this, offset);
  if (byteLength === 4)
    return this.readUInt32LE(offset);
  if (byteLength === 2)
    return this.readUInt16LE(offset);
  if (byteLength === 1)
    return this.readUInt8(offset);

  boundsError(byteLength, 6, 'byteLength');
}

function readUInt48LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 5];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 6);

  return first +
    buf[++offset] * 2 ** 8 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 24 +
    (buf[++offset] + last * 2 ** 8) * 2 ** 32;
}

function readUInt40LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 4];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 5);

  return first +
    buf[++offset] * 2 ** 8 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 24 +
    last * 2 ** 32;
}

function readUInt32LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  return first +
    this[++offset] * 2 ** 8 +
    this[++offset] * 2 ** 16 +
    last * 2 ** 24;
}

function readUInt24LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 2];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 3);

  return first + buf[++offset] * 2 ** 8 + last * 2 ** 16;
}

function readUInt16LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 1];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 2);

  return first + last * 2 ** 8;
}

function readUInt8(offset = 0) {
  validateNumber(offset, 'offset');
  const val = this[offset];
  if (val === undefined)
    boundsError(offset, this.length - 1);

  return val;
}

function readUIntBE(offset, byteLength) {
  if (offset === undefined)
    throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
  if (byteLength === 6)
    return readUInt48BE(this, offset);
  if (byteLength === 5)
    return readUInt40BE(this, offset);
  if (byteLength === 3)
    return readUInt24BE(this, offset);
  if (byteLength === 4)
    return this.readUInt32BE(offset);
  if (byteLength === 2)
    return this.readUInt16BE(offset);
  if (byteLength === 1)
    return this.readUInt8(offset);

  boundsError(byteLength, 6, 'byteLength');
}

function readUInt48BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 5];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 6);

  return (first * 2 ** 8 + buf[++offset]) * 2 ** 32 +
    buf[++offset] * 2 ** 24 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 8 +
    last;
}

function readUInt40BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 4];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 5);

  return first * 2 ** 32 +
    buf[++offset] * 2 ** 24 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 8 +
    last;
}

function readUInt32BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  return first * 2 ** 24 +
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    last;
}

function readUInt24BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 2];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 3);

  return first * 2 ** 16 + buf[++offset] * 2 ** 8 + last;
}

function readUInt16BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 1];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 2);

  return first * 2 ** 8 + last;
}

function readIntLE(offset, byteLength) {
  if (offset === undefined)
    throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
  if (byteLength === 6)
    return readInt48LE(this, offset);
  if (byteLength === 5)
    return readInt40LE(this, offset);
  if (byteLength === 3)
    return readInt24LE(this, offset);
  if (byteLength === 4)
    return this.readInt32LE(offset);
  if (byteLength === 2)
    return this.readInt16LE(offset);
  if (byteLength === 1)
    return this.readInt8(offset);

  boundsError(byteLength, 6, 'byteLength');
}

function readInt48LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 5];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 6);

  const val = buf[offset + 4] + last * 2 ** 8;
  return (val | (val & 2 ** 15) * 0x1fffe) * 2 ** 32 +
    first +
    buf[++offset] * 2 ** 8 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 24;
}

function readInt40LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 4];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 5);

  return (last | (last & 2 ** 7) * 0x1fffffe) * 2 ** 32 +
    first +
    buf[++offset] * 2 ** 8 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 24;
}

function readInt32LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  return first +
    this[++offset] * 2 ** 8 +
    this[++offset] * 2 ** 16 +
    (last << 24); // Overflow
}

function readInt24LE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 2];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 3);

  const val = first + buf[++offset] * 2 ** 8 + last * 2 ** 16;
  return val | (val & 2 ** 23) * 0x1fe;
}

function readInt16LE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 1];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 2);

  const val = first + last * 2 ** 8;
  return val | (val & 2 ** 15) * 0x1fffe;
}

function readInt8(offset = 0) {
  validateNumber(offset, 'offset');
  const val = this[offset];
  if (val === undefined)
    boundsError(offset, this.length - 1);

  return val | (val & 2 ** 7) * 0x1fffffe;
}

function readIntBE(offset, byteLength) {
  if (offset === undefined)
    throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
  if (byteLength === 6)
    return readInt48BE(this, offset);
  if (byteLength === 5)
    return readInt40BE(this, offset);
  if (byteLength === 3)
    return readInt24BE(this, offset);
  if (byteLength === 4)
    return this.readInt32BE(offset);
  if (byteLength === 2)
    return this.readInt16BE(offset);
  if (byteLength === 1)
    return this.readInt8(offset);

  boundsError(byteLength, 6, 'byteLength');
}

function readInt48BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 5];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 6);

  const val = buf[++offset] + first * 2 ** 8;
  return (val | (val & 2 ** 15) * 0x1fffe) * 2 ** 32 +
    buf[++offset] * 2 ** 24 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 8 +
    last;
}

function readInt40BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 4];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 5);

  return (first | (first & 2 ** 7) * 0x1fffffe) * 2 ** 32 +
    buf[++offset] * 2 ** 24 +
    buf[++offset] * 2 ** 16 +
    buf[++offset] * 2 ** 8 +
    last;
}

function readInt32BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  return (first << 24) + // Overflow
    this[++offset] * 2 ** 16 +
    this[++offset] * 2 ** 8 +
    last;
}

function readInt24BE(buf, offset = 0) {
  validateNumber(offset, 'offset');
  const first = buf[offset];
  const last = buf[offset + 2];
  if (first === undefined || last === undefined)
    boundsError(offset, buf.length - 3);

  const val = first * 2 ** 16 + buf[++offset] * 2 ** 8 + last;
  return val | (val & 2 ** 23) * 0x1fe;
}

function readInt16BE(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 1];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 2);

  const val = first * 2 ** 8 + last;
  return val | (val & 2 ** 15) * 0x1fffe;
}

// Read floats
function readFloatBackwards(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  uInt8Float32Array[3] = first;
  uInt8Float32Array[2] = this[++offset];
  uInt8Float32Array[1] = this[++offset];
  uInt8Float32Array[0] = last;
  return float32Array[0];
}

function readFloatForwards(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 3];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 4);

  uInt8Float32Array[0] = first;
  uInt8Float32Array[1] = this[++offset];
  uInt8Float32Array[2] = this[++offset];
  uInt8Float32Array[3] = last;
  return float32Array[0];
}

function readDoubleBackwards(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  uInt8Float64Array[7] = first;
  uInt8Float64Array[6] = this[++offset];
  uInt8Float64Array[5] = this[++offset];
  uInt8Float64Array[4] = this[++offset];
  uInt8Float64Array[3] = this[++offset];
  uInt8Float64Array[2] = this[++offset];
  uInt8Float64Array[1] = this[++offset];
  uInt8Float64Array[0] = last;
  return float64Array[0];
}

function readDoubleForwards(offset = 0) {
  validateNumber(offset, 'offset');
  const first = this[offset];
  const last = this[offset + 7];
  if (first === undefined || last === undefined)
    boundsError(offset, this.length - 8);

  uInt8Float64Array[0] = first;
  uInt8Float64Array[1] = this[++offset];
  uInt8Float64Array[2] = this[++offset];
  uInt8Float64Array[3] = this[++offset];
  uInt8Float64Array[4] = this[++offset];
  uInt8Float64Array[5] = this[++offset];
  uInt8Float64Array[6] = this[++offset];
  uInt8Float64Array[7] = last;
  return float64Array[0];
}

// Write integers.
function writeBigU_Int64LE(buf, value, offset, min, max) {
  checkInt(value, min, max, buf, offset, 7);

  let lo = Number(value & 0xffffffffn);
  buf[offset++] = lo;
  lo = lo >> 8;
  buf[offset++] = lo;
  lo = lo >> 8;
  buf[offset++] = lo;
  lo = lo >> 8;
  buf[offset++] = lo;
  let hi = Number(value >> 32n & 0xffffffffn);
  buf[offset++] = hi;
  hi = hi >> 8;
  buf[offset++] = hi;
  hi = hi >> 8;
  buf[offset++] = hi;
  hi = hi >> 8;
  buf[offset++] = hi;
  return offset;
}

function writeBigUInt64LE(value, offset = 0) {
  return writeBigU_Int64LE(this, value, offset, 0n, 0xffffffffffffffffn);
}

function writeBigU_Int64BE(buf, value, offset, min, max) {
  checkInt(value, min, max, buf, offset, 7);

  let lo = Number(value & 0xffffffffn);
  buf[offset + 7] = lo;
  lo = lo >> 8;
  buf[offset + 6] = lo;
  lo = lo >> 8;
  buf[offset + 5] = lo;
  lo = lo >> 8;
  buf[offset + 4] = lo;
  let hi = Number(value >> 32n & 0xffffffffn);
  buf[offset + 3] = hi;
  hi = hi >> 8;
  buf[offset + 2] = hi;
  hi = hi >> 8;
  buf[offset + 1] = hi;
  hi = hi >> 8;
  buf[offset] = hi;
  return offset + 8;
}

function writeBigUInt64BE(value, offset = 0) {
  return writeBigU_Int64BE(this, value, offset, 0n, 0xffffffffffffffffn);
}

function writeBigInt64LE(value, offset = 0) {
  return writeBigU_Int64LE(
    this, value, offset, -0x8000000000000000n, 0x7fffffffffffffffn);
}

function writeBigInt64BE(value, offset = 0) {
  return writeBigU_Int64BE(
    this, value, offset, -0x8000000000000000n, 0x7fffffffffffffffn);
}

function writeUIntLE(value, offset, byteLength) {
  if (byteLength === 6)
    return writeU_Int48LE(this, value, offset, 0, 0xffffffffffff);
  if (byteLength === 5)
    return writeU_Int40LE(this, value, offset, 0, 0xffffffffff);
  if (byteLength === 3)
    return writeU_Int24LE(this, value, offset, 0, 0xffffff);
  if (byteLength === 4)
    return writeU_Int32LE(this, value, offset, 0, 0xffffffff);
  if (byteLength === 2)
    return writeU_Int16LE(this, value, offset, 0, 0xffff);
  if (byteLength === 1)
    return writeU_Int8(this, value, offset, 0, 0xff);

  boundsError(byteLength, 6, 'byteLength');
}

function writeU_Int48LE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 5);

  const newVal = MathFloor(value * 2 ** -32);
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  buf[offset++] = newVal;
  buf[offset++] = (newVal >>> 8);
  return offset;
}

function writeU_Int40LE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 4);

  const newVal = value;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  buf[offset++] = MathFloor(newVal * 2 ** -32);
  return offset;
}

function writeU_Int32LE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 3);

  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  return offset;
}

function writeUInt32LE(value, offset = 0) {
  return writeU_Int32LE(this, value, offset, 0, 0xffffffff);
}

function writeU_Int24LE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 2);

  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  value = value >>> 8;
  buf[offset++] = value;
  return offset;
}

function writeU_Int16LE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 1);

  buf[offset++] = value;
  buf[offset++] = (value >>> 8);
  return offset;
}

function writeUInt16LE(value, offset = 0) {
  return writeU_Int16LE(this, value, offset, 0, 0xffff);
}

function writeU_Int8(buf, value, offset, min, max) {
  value = +value;
  // `checkInt()` can not be used here because it checks two entries.
  validateNumber(offset, 'offset');
  if (value > max || value < min) {
    throw new ERR_OUT_OF_RANGE('value', `>= ${min} and <= ${max}`, value);
  }
  if (buf[offset] === undefined)
    boundsError(offset, buf.length - 1);

  buf[offset] = value;
  return offset + 1;
}

function writeUInt8(value, offset = 0) {
  return writeU_Int8(this, value, offset, 0, 0xff);
}

function writeUIntBE(value, offset, byteLength) {
  if (byteLength === 6)
    return writeU_Int48BE(this, value, offset, 0, 0xffffffffffff);
  if (byteLength === 5)
    return writeU_Int40BE(this, value, offset, 0, 0xffffffffff);
  if (byteLength === 3)
    return writeU_Int24BE(this, value, offset, 0, 0xffffff);
  if (byteLength === 4)
    return writeU_Int32BE(this, value, offset, 0, 0xffffffff);
  if (byteLength === 2)
    return writeU_Int16BE(this, value, offset, 0, 0xffff);
  if (byteLength === 1)
    return writeU_Int8(this, value, offset, 0, 0xff);

  boundsError(byteLength, 6, 'byteLength');
}

function writeU_Int48BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 5);

  const newVal = MathFloor(value * 2 ** -32);
  buf[offset++] = (newVal >>> 8);
  buf[offset++] = newVal;
  buf[offset + 3] = value;
  value = value >>> 8;
  buf[offset + 2] = value;
  value = value >>> 8;
  buf[offset + 1] = value;
  value = value >>> 8;
  buf[offset] = value;
  return offset + 4;
}

function writeU_Int40BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 4);

  buf[offset++] = MathFloor(value * 2 ** -32);
  buf[offset + 3] = value;
  value = value >>> 8;
  buf[offset + 2] = value;
  value = value >>> 8;
  buf[offset + 1] = value;
  value = value >>> 8;
  buf[offset] = value;
  return offset + 4;
}

function writeU_Int32BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 3);

  buf[offset + 3] = value;
  value = value >>> 8;
  buf[offset + 2] = value;
  value = value >>> 8;
  buf[offset + 1] = value;
  value = value >>> 8;
  buf[offset] = value;
  return offset + 4;
}

function writeUInt32BE(value, offset = 0) {
  return writeU_Int32BE(this, value, offset, 0, 0xffffffff);
}

function writeU_Int24BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 2);

  buf[offset + 2] = value;
  value = value >>> 8;
  buf[offset + 1] = value;
  value = value >>> 8;
  buf[offset] = value;
  return offset + 3;
}

function writeU_Int16BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 1);

  buf[offset++] = (value >>> 8);
  buf[offset++] = value;
  return offset;
}

function writeUInt16BE(value, offset = 0) {
  return writeU_Int16BE(this, value, offset, 0, 0xffff);
}

function writeIntLE(value, offset, byteLength) {
  if (byteLength === 6)
    return writeU_Int48LE(this, value, offset, -0x800000000000, 0x7fffffffffff);
  if (byteLength === 5)
    return writeU_Int40LE(this, value, offset, -0x8000000000, 0x7fffffffff);
  if (byteLength === 3)
    return writeU_Int24LE(this, value, offset, -0x800000, 0x7fffff);
  if (byteLength === 4)
    return writeU_Int32LE(this, value, offset, -0x80000000, 0x7fffffff);
  if (byteLength === 2)
    return writeU_Int16LE(this, value, offset, -0x8000, 0x7fff);
  if (byteLength === 1)
    return writeU_Int8(this, value, offset, -0x80, 0x7f);

  boundsError(byteLength, 6, 'byteLength');
}

function writeInt32LE(value, offset = 0) {
  return writeU_Int32LE(this, value, offset, -0x80000000, 0x7fffffff);
}

function writeInt16LE(value, offset = 0) {
  return writeU_Int16LE(this, value, offset, -0x8000, 0x7fff);
}

function writeInt8(value, offset = 0) {
  return writeU_Int8(this, value, offset, -0x80, 0x7f);
}

function writeIntBE(value, offset, byteLength) {
  if (byteLength === 6)
    return writeU_Int48BE(this, value, offset, -0x800000000000, 0x7fffffffffff);
  if (byteLength === 5)
    return writeU_Int40BE(this, value, offset, -0x8000000000, 0x7fffffffff);
  if (byteLength === 3)
    return writeU_Int24BE(this, value, offset, -0x800000, 0x7fffff);
  if (byteLength === 4)
    return writeU_Int32BE(this, value, offset, -0x80000000, 0x7fffffff);
  if (byteLength === 2)
    return writeU_Int16BE(this, value, offset, -0x8000, 0x7fff);
  if (byteLength === 1)
    return writeU_Int8(this, value, offset, -0x80, 0x7f);

  boundsError(byteLength, 6, 'byteLength');
}

function writeInt32BE(value, offset = 0) {
  return writeU_Int32BE(this, value, offset, -0x80000000, 0x7fffffff);
}

function writeInt16BE(value, offset = 0) {
  return writeU_Int16BE(this, value, offset, -0x8000, 0x7fff);
}

// Write floats.
function writeDoubleForwards(val, offset = 0) {
  val = +val;
  checkBounds(this, offset, 7);

  float64Array[0] = val;
  this[offset++] = uInt8Float64Array[0];
  this[offset++] = uInt8Float64Array[1];
  this[offset++] = uInt8Float64Array[2];
  this[offset++] = uInt8Float64Array[3];
  this[offset++] = uInt8Float64Array[4];
  this[offset++] = uInt8Float64Array[5];
  this[offset++] = uInt8Float64Array[6];
  this[offset++] = uInt8Float64Array[7];
  return offset;
}

function writeDoubleBackwards(val, offset = 0) {
  val = +val;
  checkBounds(this, offset, 7);

  float64Array[0] = val;
  this[offset++] = uInt8Float64Array[7];
  this[offset++] = uInt8Float64Array[6];
  this[offset++] = uInt8Float64Array[5];
  this[offset++] = uInt8Float64Array[4];
  this[offset++] = uInt8Float64Array[3];
  this[offset++] = uInt8Float64Array[2];
  this[offset++] = uInt8Float64Array[1];
  this[offset++] = uInt8Float64Array[0];
  return offset;
}

function writeFloatForwards(val, offset = 0) {
  val = +val;
  checkBounds(this, offset, 3);

  float32Array[0] = val;
  this[offset++] = uInt8Float32Array[0];
  this[offset++] = uInt8Float32Array[1];
  this[offset++] = uInt8Float32Array[2];
  this[offset++] = uInt8Float32Array[3];
  return offset;
}

function writeFloatBackwards(val, offset = 0) {
  val = +val;
  checkBounds(this, offset, 3);

  float32Array[0] = val;
  this[offset++] = uInt8Float32Array[3];
  this[offset++] = uInt8Float32Array[2];
  this[offset++] = uInt8Float32Array[1];
  this[offset++] = uInt8Float32Array[0];
  return offset;
}

class FastBuffer extends Uint8Array {
  // Using an explicit constructor here is necessary to avoid relying on
  // `Array.prototype[Symbol.iterator]`, which can be mutated by users.
  // eslint-disable-next-line no-useless-constructor
  constructor(bufferOrLength, byteOffset, length) {
    super(bufferOrLength, byteOffset, length);
  }
}

function addBufferPrototypeMethods(proto) {
  proto.readBigUInt64LE = readBigUInt64LE;
  proto.readBigUInt64BE = readBigUInt64BE;
  proto.readBigUint64LE = readBigUInt64LE;
  proto.readBigUint64BE = readBigUInt64BE;
  proto.readBigInt64LE = readBigInt64LE;
  proto.readBigInt64BE = readBigInt64BE;
  proto.writeBigUInt64LE = writeBigUInt64LE;
  proto.writeBigUInt64BE = writeBigUInt64BE;
  proto.writeBigUint64LE = writeBigUInt64LE;
  proto.writeBigUint64BE = writeBigUInt64BE;
  proto.writeBigInt64LE = writeBigInt64LE;
  proto.writeBigInt64BE = writeBigInt64BE;

  proto.readUIntLE = readUIntLE;
  proto.readUInt32LE = readUInt32LE;
  proto.readUInt16LE = readUInt16LE;
  proto.readUInt8 = readUInt8;
  proto.readUIntBE = readUIntBE;
  proto.readUInt32BE = readUInt32BE;
  proto.readUInt16BE = readUInt16BE;
  proto.readUintLE = readUIntLE;
  proto.readUint32LE = readUInt32LE;
  proto.readUint16LE = readUInt16LE;
  proto.readUint8 = readUInt8;
  proto.readUintBE = readUIntBE;
  proto.readUint32BE = readUInt32BE;
  proto.readUint16BE = readUInt16BE;
  proto.readIntLE = readIntLE;
  proto.readInt32LE = readInt32LE;
  proto.readInt16LE = readInt16LE;
  proto.readInt8 = readInt8;
  proto.readIntBE = readIntBE;
  proto.readInt32BE = readInt32BE;
  proto.readInt16BE = readInt16BE;

  proto.writeUIntLE = writeUIntLE;
  proto.writeUInt32LE = writeUInt32LE;
  proto.writeUInt16LE = writeUInt16LE;
  proto.writeUInt8 = writeUInt8;
  proto.writeUIntBE = writeUIntBE;
  proto.writeUInt32BE = writeUInt32BE;
  proto.writeUInt16BE = writeUInt16BE;
  proto.writeUintLE = writeUIntLE;
  proto.writeUint32LE = writeUInt32LE;
  proto.writeUint16LE = writeUInt16LE;
  proto.writeUint8 = writeUInt8;
  proto.writeUintBE = writeUIntBE;
  proto.writeUint32BE = writeUInt32BE;
  proto.writeUint16BE = writeUInt16BE;
  proto.writeIntLE = writeIntLE;
  proto.writeInt32LE = writeInt32LE;
  proto.writeInt16LE = writeInt16LE;
  proto.writeInt8 = writeInt8;
  proto.writeIntBE = writeIntBE;
  proto.writeInt32BE = writeInt32BE;
  proto.writeInt16BE = writeInt16BE;

  proto.readFloatLE = bigEndian ? readFloatBackwards : readFloatForwards;
  proto.readFloatBE = bigEndian ? readFloatForwards : readFloatBackwards;
  proto.readDoubleLE = bigEndian ? readDoubleBackwards : readDoubleForwards;
  proto.readDoubleBE = bigEndian ? readDoubleForwards : readDoubleBackwards;
  proto.writeFloatLE = bigEndian ? writeFloatBackwards : writeFloatForwards;
  proto.writeFloatBE = bigEndian ? writeFloatForwards : writeFloatBackwards;
  proto.writeDoubleLE = bigEndian ? writeDoubleBackwards : writeDoubleForwards;
  proto.writeDoubleBE = bigEndian ? writeDoubleForwards : writeDoubleBackwards;

  proto.asciiSlice = asciiSlice;
  proto.base64Slice = base64Slice;
  proto.base64urlSlice = base64urlSlice;
  proto.latin1Slice = latin1Slice;
  proto.hexSlice = hexSlice;
  proto.ucs2Slice = ucs2Slice;
  proto.utf8Slice = utf8Slice;
  proto.asciiWrite = function(string, offset = 0, length = this.byteLength - offset) {
    if (offset < 0 || offset > this.byteLength) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('offset');
    }
    if (length < 0 || length > this.byteLength - offset) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('length');
    }
    return asciiWriteStatic(this, string, offset, length);
  };
  proto.base64Write = base64Write;
  proto.base64urlWrite = base64urlWrite;
  proto.latin1Write = function(string, offset = 0, length = this.byteLength - offset) {
    if (offset < 0 || offset > this.byteLength) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('offset');
    }
    if (length < 0 || length > this.byteLength - offset) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('length');
    }
    return latin1WriteStatic(this, string, offset, length);
  };
  proto.hexWrite = hexWrite;
  proto.ucs2Write = ucs2Write;
  proto.utf8Write = function(string, offset = 0, length = this.byteLength - offset) {
    if (offset < 0 || offset > this.byteLength) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('offset');
    }
    if (length < 0 || length > this.byteLength - offset) {
      throw new ERR_BUFFER_OUT_OF_BOUNDS('length');
    }
    return utf8WriteStatic(this, string, offset, length);
  };
}

// This would better be placed in internal/worker/io.js, but that doesn't work
// because Buffer needs this and that would introduce a cyclic dependency.
function markAsUntransferable(obj) {
  if ((typeof obj !== 'object' && typeof obj !== 'function') || obj === null)
    return;  // This object is a primitive and therefore already untransferable.
  obj[untransferable_object_private_symbol] = true;
}

// This simply checks if the object is marked as untransferable and doesn't
// check whether we are able to transfer it.
function isMarkedAsUntransferable(obj) {
  if (obj == null)
    return false;
  // Private symbols are not inherited.
  return obj[untransferable_object_private_symbol] !== undefined;
}

// A toggle used to access the zero fill setting of the array buffer allocator
// in C++.
// |zeroFill| can be undefined when running inside an isolate where we
// do not own the ArrayBuffer allocator.  Zero fill is always on in that case.
let zeroFill;
function createUnsafeBuffer(size) {
  if (!zeroFill) {
    zeroFill = getZeroFillToggle();
    if (isBuildingSnapshot()) {
      // Reset the toggle so that after serialization, we'll re-create a real
      // toggle connected to the C++ one via getZeroFillToggle().
      addAfterUserSerializeCallback(() => {
        zeroFill = undefined;
      });
    }
  }
  zeroFill[0] = 0;
  try {
    return new FastBuffer(size);
  } finally {
    zeroFill[0] = 1;
  }
}

FastBuffer.prototype.constructor = Buffer;
Buffer.prototype = FastBuffer.prototype;
addBufferPrototypeMethods(Buffer.prototype);

const constants = ObjectDefineProperties({}, {
  MAX_LENGTH: {
    __proto__: null,
    value: kMaxLength,
    writable: false,
    enumerable: true,
  },
  MAX_STRING_LENGTH: {
    __proto__: null,
    value: kStringMaxLength,
    writable: false,
    enumerable: true,
  },
});

Buffer.poolSize = 8 * 1024;
let poolSize, poolOffset, allocPool, allocBuffer;

function createPool() {
  poolSize = Buffer.poolSize;
  allocBuffer = createUnsafeBuffer(poolSize);
  allocPool = allocBuffer.buffer;
  markAsUntransferable(allocPool);
  poolOffset = 0;
}
createPool();

function alignPool() {
  // Ensure aligned slices
  if (poolOffset & 0x7) {
    poolOffset |= 0x7;
    poolOffset++;
  }
}

let bufferWarningAlreadyEmitted = false;
let nodeModulesCheckCounter = 0;
const bufferWarning = 'Buffer() is deprecated due to security and usability ' +
                      'issues. Please use the Buffer.alloc(), ' +
                      'Buffer.allocUnsafe(), or Buffer.from() methods instead.';

function showFlaggedDeprecation() {
  if (bufferWarningAlreadyEmitted ||
      ++nodeModulesCheckCounter > 10000 ||
      (!require('internal/options').getOptionValue('--pending-deprecation') &&
       isInsideNodeModules(100, true))) {
    // We don't emit a warning, because we either:
    // - Already did so, or
    // - Already checked too many times whether a call is coming
    //   from node_modules and want to stop slowing down things, or
    // - We aren't running with `--pending-deprecation` enabled,
    //   and the code is inside `node_modules`.
    // - We found node_modules in up to the topmost 100 frames, or
    //   there are more than 100 frames and we don't want to search anymore.
    return;
  }

  process.emitWarning(bufferWarning, 'DeprecationWarning', 'DEP0005');
  bufferWarningAlreadyEmitted = true;
}

function toInteger(n, defaultVal) {
  n = +n;
  if (!NumberIsNaN(n) &&
      n >= NumberMIN_SAFE_INTEGER &&
      n <= NumberMAX_SAFE_INTEGER) {
    return ((n % 1) === 0 ? n : MathFloor(n));
  }
  return defaultVal;
}

function copyImpl(source, target, targetStart, sourceStart, sourceEnd) {
  if (!ArrayBufferIsView(source))
    throw new ERR_INVALID_ARG_TYPE('source', ['Buffer', 'Uint8Array'], source);
  if (!ArrayBufferIsView(target))
    throw new ERR_INVALID_ARG_TYPE('target', ['Buffer', 'Uint8Array'], target);

  if (targetStart === undefined) {
    targetStart = 0;
  } else {
    targetStart = NumberIsInteger(targetStart) ? targetStart : toInteger(targetStart, 0);
    if (targetStart < 0)
      throw new ERR_OUT_OF_RANGE('targetStart', '>= 0', targetStart);
  }

  if (sourceStart === undefined) {
    sourceStart = 0;
  } else {
    sourceStart = NumberIsInteger(sourceStart) ? sourceStart : toInteger(sourceStart, 0);
    if (sourceStart < 0 || sourceStart > source.byteLength)
      throw new ERR_OUT_OF_RANGE('sourceStart', `>= 0 && <= ${source.byteLength}`, sourceStart);
  }

  if (sourceEnd === undefined) {
    sourceEnd = source.byteLength;
  } else {
    sourceEnd = NumberIsInteger(sourceEnd) ? sourceEnd : toInteger(sourceEnd, 0);
    if (sourceEnd < 0)
      throw new ERR_OUT_OF_RANGE('sourceEnd', '>= 0', sourceEnd);
  }

  if (targetStart >= target.byteLength || sourceStart >= sourceEnd)
    return 0;

  return _copyActual(source, target, targetStart, sourceStart, sourceEnd);
}

function _copyActual(source, target, targetStart, sourceStart, sourceEnd) {
  if (sourceEnd - sourceStart > target.byteLength - targetStart)
    sourceEnd = sourceStart + target.byteLength - targetStart;

  let nb = sourceEnd - sourceStart;
  const sourceLen = source.byteLength - sourceStart;
  if (nb > sourceLen)
    nb = sourceLen;

  if (nb <= 0)
    return 0;

  _copy(source, target, targetStart, sourceStart, nb);

  return nb;
}

/**
 * The Buffer() constructor is deprecated in documentation and should not be
 * used moving forward. Rather, developers should use one of the three new
 * factory APIs: Buffer.from(), Buffer.allocUnsafe() or Buffer.alloc() based on
 * their specific needs. There is no runtime deprecation because of the extent
 * to which the Buffer constructor is used in the ecosystem currently -- a
 * runtime deprecation would introduce too much breakage at this time. It's not
 * likely that the Buffer constructors would ever actually be removed.
 * Deprecation Code: DEP0005
 */
function Buffer(arg, encodingOrOffset, length) {
  showFlaggedDeprecation();
  // Common case.
  if (typeof arg === 'number') {
    if (typeof encodingOrOffset === 'string') {
      throw new ERR_INVALID_ARG_TYPE('string', 'string', arg);
    }
    return Buffer.alloc(arg);
  }
  return Buffer.from(arg, encodingOrOffset, length);
}

ObjectDefineProperty(Buffer, SymbolSpecies, {
  __proto__: null,
  enumerable: false,
  configurable: true,
  get() { return FastBuffer; },
});

/**
 * Functionally equivalent to Buffer(arg, encoding) but throws a TypeError
 * if value is a number.
 * Buffer.from(str[, encoding])
 * Buffer.from(array)
 * Buffer.from(buffer)
 * Buffer.from(arrayBuffer[, byteOffset[, length]])
 */
Buffer.from = function from(value, encodingOrOffset, length) {
  if (typeof value === 'string')
    return fromString(value, encodingOrOffset);

  if (typeof value === 'object' && value !== null) {
    if (isAnyArrayBuffer(value))
      return fromArrayBuffer(value, encodingOrOffset, length);

    const valueOf = value.valueOf && value.valueOf();
    if (valueOf != null &&
        valueOf !== value &&
        (typeof valueOf === 'string' || typeof valueOf === 'object')) {
      return from(valueOf, encodingOrOffset, length);
    }

    const b = fromObject(value);
    if (b)
      return b;

    if (typeof value[SymbolToPrimitive] === 'function') {
      const primitive = value[SymbolToPrimitive]('string');
      if (typeof primitive === 'string') {
        return fromString(primitive, encodingOrOffset);
      }
    }
  }

  throw new ERR_INVALID_ARG_TYPE(
    'first argument',
    ['string', 'Buffer', 'ArrayBuffer', 'Array', 'Array-like Object'],
    value,
  );
};

/**
 * Creates the Buffer as a copy of the underlying ArrayBuffer of the view
 * rather than the contents of the view.
 * @param {TypedArray} view
 * @param {number} [offset]
 * @param {number} [length]
 * @returns {Buffer}
 */
Buffer.copyBytesFrom = function copyBytesFrom(view, offset, length) {
  if (!isTypedArray(view)) {
    throw new ERR_INVALID_ARG_TYPE('view', [ 'TypedArray' ], view);
  }

  const viewLength = TypedArrayPrototypeGetLength(view);
  if (viewLength === 0) {
    return Buffer.alloc(0);
  }

  if (offset !== undefined || length !== undefined) {
    if (offset !== undefined) {
      validateInteger(offset, 'offset', 0);
      if (offset >= viewLength) return Buffer.alloc(0);
    } else {
      offset = 0;
    }
    let end;
    if (length !== undefined) {
      validateInteger(length, 'length', 0);
      end = offset + length;
    } else {
      end = viewLength;
    }

    view = TypedArrayPrototypeSlice(view, offset, end);
  }

  return fromArrayLike(new Uint8Array(
    TypedArrayPrototypeGetBuffer(view),
    TypedArrayPrototypeGetByteOffset(view),
    TypedArrayPrototypeGetByteLength(view)));
};

// Identical to the built-in %TypedArray%.of(), but avoids using the deprecated
// Buffer() constructor. Must use arrow function syntax to avoid automatically
// adding a `prototype` property and making the function a constructor.
//
// Refs: https://tc39.github.io/ecma262/#sec-%typedarray%.of
// Refs: https://esdiscuss.org/topic/isconstructor#content-11
const of = (...items) => {
  const newObj = createUnsafeBuffer(items.length);
  for (let k = 0; k < items.length; k++)
    newObj[k] = items[k];
  return newObj;
};
Buffer.of = of;

ObjectSetPrototypeOf(Buffer, Uint8Array);

/**
 * Creates a new filled Buffer instance.
 * alloc(size[, fill[, encoding]])
 */
Buffer.alloc = function alloc(size, fill, encoding) {
  validateNumber(size, 'size', 0, kMaxLength);
  if (fill !== undefined && fill !== 0 && size > 0) {
    const buf = createUnsafeBuffer(size);
    return _fill(buf, fill, 0, buf.length, encoding);
  }
  return new FastBuffer(size);
};

/**
 * Equivalent to Buffer(num), by default creates a non-zero-filled Buffer
 * instance. If `--zero-fill-buffers` is set, will zero-fill the buffer.
 */
Buffer.allocUnsafe = function allocUnsafe(size) {
  validateNumber(size, 'size', 0, kMaxLength);
  return allocate(size);
};

/**
 * Equivalent to SlowBuffer(num), by default creates a non-zero-filled
 * Buffer instance that is not allocated off the pre-initialized pool.
 * If `--zero-fill-buffers` is set, will zero-fill the buffer.
 */
Buffer.allocUnsafeSlow = function allocUnsafeSlow(size) {
  validateNumber(size, 'size', 0, kMaxLength);
  return createUnsafeBuffer(size);
};

// If --zero-fill-buffers command line argument is set, a zero-filled
// buffer is returned.
function SlowBuffer(size) {
  validateNumber(size, 'size', 0, kMaxLength);
  return createUnsafeBuffer(size);
}

ObjectSetPrototypeOf(SlowBuffer.prototype, Uint8ArrayPrototype);
ObjectSetPrototypeOf(SlowBuffer, Uint8Array);

function allocate(size) {
  if (size <= 0) {
    return new FastBuffer();
  }
  if (size < (Buffer.poolSize >>> 1)) {
    if (size > (poolSize - poolOffset))
      createPool();
    const b = new FastBuffer(allocPool, poolOffset, size);
    poolOffset += size;
    alignPool();
    return b;
  }
  return createUnsafeBuffer(size);
}

function fromStringFast(string, ops) {
  const maxLength = Buffer.poolSize >>> 1;

  let length = string.length; // Min length

  if (length >= maxLength)
    return createFromString(string, ops);

  length *= 4; // Max length (4 bytes per character)

  if (length >= maxLength)
    length = ops.byteLength(string); // Actual length

  if (length >= maxLength)
    return createFromString(string, ops, length);

  if (length > (poolSize - poolOffset))
    createPool();

  const actual = ops.write(allocBuffer, string, poolOffset, length);
  const b = new FastBuffer(allocPool, poolOffset, actual);

  poolOffset += actual;
  alignPool();
  return b;
}

function createFromString(string, ops, length = ops.byteLength(string)) {
  const buf = Buffer.allocUnsafeSlow(length);
  const actual = ops.write(buf, string, 0, length);
  return actual < length ? new FastBuffer(buf.buffer, 0, actual) : buf;
}

function fromString(string, encoding) {
  let ops;
  if (!encoding || encoding === 'utf8' || typeof encoding !== 'string') {
    ops = encodingOps.utf8;
  } else {
    ops = getEncodingOps(encoding);
    if (ops === undefined)
      throw new ERR_UNKNOWN_ENCODING(encoding);
  }

  return string.length === 0 ? new FastBuffer() : fromStringFast(string, ops);
}

function fromArrayBuffer(obj, byteOffset, length) {
  // Convert byteOffset to integer
  if (byteOffset === undefined) {
    byteOffset = 0;
  } else {
    byteOffset = +byteOffset;
    if (NumberIsNaN(byteOffset))
      byteOffset = 0;
  }

  const maxLength = obj.byteLength - byteOffset;

  if (maxLength < 0)
    throw new ERR_BUFFER_OUT_OF_BOUNDS('offset');

  if (length !== undefined) {
    // Convert length to non-negative integer.
    length = +length;
    if (length > 0) {
      if (length > maxLength)
        throw new ERR_BUFFER_OUT_OF_BOUNDS('length');
    } else {
      length = 0;
    }
  }

  return new FastBuffer(obj, byteOffset, length);
}

function fromArrayLike(obj) {
  if (obj.length <= 0)
    return new FastBuffer();
  if (obj.length < (Buffer.poolSize >>> 1)) {
    if (obj.length > (poolSize - poolOffset))
      createPool();
    const b = new FastBuffer(allocPool, poolOffset, obj.length);
    TypedArrayPrototypeSet(b, obj, 0);
    poolOffset += obj.length;
    alignPool();
    return b;
  }
  return new FastBuffer(obj);
}

function fromObject(obj) {
  if (obj.length !== undefined || isAnyArrayBuffer(obj.buffer)) {
    if (typeof obj.length !== 'number') {
      return new FastBuffer();
    }
    return fromArrayLike(obj);
  }

  if (obj.type === 'Buffer' && ArrayIsArray(obj.data)) {
    return fromArrayLike(obj.data);
  }
}

// Static methods

Buffer.isBuffer = function isBuffer(b) {
  return b instanceof Buffer;
};

Buffer.compare = function compare(buf1, buf2) {
  if (!isUint8Array(buf1)) {
    throw new ERR_INVALID_ARG_TYPE('buf1', ['Buffer', 'Uint8Array'], buf1);
  }

  if (!isUint8Array(buf2)) {
    throw new ERR_INVALID_ARG_TYPE('buf2', ['Buffer', 'Uint8Array'], buf2);
  }

  if (buf1 === buf2) {
    return 0;
  }

  return _compare(buf1, buf2);
};

Buffer.isEncoding = function isEncoding(encoding) {
  return typeof encoding === 'string' && encoding.length !== 0 &&
         normalizeEncoding(encoding) !== undefined;
};
Buffer[kIsEncodingSymbol] = Buffer.isEncoding;

Buffer.concat = function concat(list, length) {
  validateArray(list, 'list');

  if (list.length === 0)
    return new FastBuffer();

  if (length === undefined) {
    length = 0;
    for (let i = 0; i < list.length; i++) {
      if (list[i].length) {
        length += list[i].length;
      }
    }
  } else {
    validateOffset(length, 'length');
  }

  const buffer = Buffer.allocUnsafe(length);
  let pos = 0;
  for (let i = 0; i < list.length; i++) {
    const buf = list[i];
    if (!isUint8Array(buf)) {
      // TODO(BridgeAR): This should not be of type ERR_INVALID_ARG_TYPE.
      // Instead, find the proper error code for this.
      throw new ERR_INVALID_ARG_TYPE(
        `list[${i}]`, ['Buffer', 'Uint8Array'], list[i]);
    }
    pos += _copyActual(buf, buffer, pos, 0, buf.length);
  }

  // Note: `length` is always equal to `buffer.length` at this point
  if (pos < length) {
    // Zero-fill the remaining bytes if the specified `length` was more than
    // the actual total length, i.e. if we have some remaining allocated bytes
    // there were not initialized.
    TypedArrayPrototypeFill(buffer, 0, pos, length);
  }

  return buffer;
};

function base64ByteLength(str, bytes) {
  // Handle padding
  if (StringPrototypeCharCodeAt(str, bytes - 1) === 0x3D)
    bytes--;
  if (bytes > 1 && StringPrototypeCharCodeAt(str, bytes - 1) === 0x3D)
    bytes--;

  // Base64 ratio: 3/4
  return (bytes * 3) >>> 2;
}

const encodingOps = {
  utf8: {
    encoding: 'utf8',
    encodingVal: encodingsMap.utf8,
    byteLength: byteLengthUtf8,
    write: (buf, string, offset, len) => buf.utf8Write(string, offset, len),
    slice: (buf, start, end) => buf.utf8Slice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfString(buf, val, byteOffset, encodingsMap.utf8, dir),
  },
  ucs2: {
    encoding: 'ucs2',
    encodingVal: encodingsMap.utf16le,
    byteLength: (string) => string.length * 2,
    write: (buf, string, offset, len) => buf.ucs2Write(string, offset, len),
    slice: (buf, start, end) => buf.ucs2Slice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfString(buf, val, byteOffset, encodingsMap.utf16le, dir),
  },
  utf16le: {
    encoding: 'utf16le',
    encodingVal: encodingsMap.utf16le,
    byteLength: (string) => string.length * 2,
    write: (buf, string, offset, len) => buf.ucs2Write(string, offset, len),
    slice: (buf, start, end) => buf.ucs2Slice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfString(buf, val, byteOffset, encodingsMap.utf16le, dir),
  },
  latin1: {
    encoding: 'latin1',
    encodingVal: encodingsMap.latin1,
    byteLength: (string) => string.length,
    write: (buf, string, offset, len) => buf.latin1Write(string, offset, len),
    slice: (buf, start, end) => buf.latin1Slice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfString(buf, val, byteOffset, encodingsMap.latin1, dir),
  },
  ascii: {
    encoding: 'ascii',
    encodingVal: encodingsMap.ascii,
    byteLength: (string) => string.length,
    write: (buf, string, offset, len) => buf.asciiWrite(string, offset, len),
    slice: (buf, start, end) => buf.asciiSlice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfBuffer(buf,
                    fromStringFast(val, encodingOps.ascii),
                    byteOffset,
                    encodingsMap.ascii,
                    dir),
  },
  base64: {
    encoding: 'base64',
    encodingVal: encodingsMap.base64,
    byteLength: (string) => base64ByteLength(string, string.length),
    write: (buf, string, offset, len) => buf.base64Write(string, offset, len),
    slice: (buf, start, end) => buf.base64Slice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfBuffer(buf,
                    fromStringFast(val, encodingOps.base64),
                    byteOffset,
                    encodingsMap.base64,
                    dir),
  },
  base64url: {
    encoding: 'base64url',
    encodingVal: encodingsMap.base64url,
    byteLength: (string) => base64ByteLength(string, string.length),
    write: (buf, string, offset, len) =>
      buf.base64urlWrite(string, offset, len),
    slice: (buf, start, end) => buf.base64urlSlice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfBuffer(buf,
                    fromStringFast(val, encodingOps.base64url),
                    byteOffset,
                    encodingsMap.base64url,
                    dir),
  },
  hex: {
    encoding: 'hex',
    encodingVal: encodingsMap.hex,
    byteLength: (string) => string.length >>> 1,
    write: (buf, string, offset, len) => buf.hexWrite(string, offset, len),
    slice: (buf, start, end) => buf.hexSlice(start, end),
    indexOf: (buf, val, byteOffset, dir) =>
      indexOfBuffer(buf,
                    fromStringFast(val, encodingOps.hex),
                    byteOffset,
                    encodingsMap.hex,
                    dir),
  },
};
function getEncodingOps(encoding) {
  encoding += '';
  switch (encoding.length) {
    case 4:
      if (encoding === 'utf8') return encodingOps.utf8;
      if (encoding === 'ucs2') return encodingOps.ucs2;
      encoding = StringPrototypeToLowerCase(encoding);
      if (encoding === 'utf8') return encodingOps.utf8;
      if (encoding === 'ucs2') return encodingOps.ucs2;
      break;
    case 5:
      if (encoding === 'utf-8') return encodingOps.utf8;
      if (encoding === 'ascii') return encodingOps.ascii;
      if (encoding === 'ucs-2') return encodingOps.ucs2;
      encoding = StringPrototypeToLowerCase(encoding);
      if (encoding === 'utf-8') return encodingOps.utf8;
      if (encoding === 'ascii') return encodingOps.ascii;
      if (encoding === 'ucs-2') return encodingOps.ucs2;
      break;
    case 7:
      if (encoding === 'utf16le' ||
          StringPrototypeToLowerCase(encoding) === 'utf16le')
        return encodingOps.utf16le;
      break;
    case 8:
      if (encoding === 'utf-16le' ||
          StringPrototypeToLowerCase(encoding) === 'utf-16le')
        return encodingOps.utf16le;
      break;
    case 6:
      if (encoding === 'latin1' || encoding === 'binary')
        return encodingOps.latin1;
      if (encoding === 'base64') return encodingOps.base64;
      encoding = StringPrototypeToLowerCase(encoding);
      if (encoding === 'latin1' || encoding === 'binary')
        return encodingOps.latin1;
      if (encoding === 'base64') return encodingOps.base64;
      break;
    case 3:
      if (encoding === 'hex' || StringPrototypeToLowerCase(encoding) === 'hex')
        return encodingOps.hex;
      break;
    case 9:
      if (encoding === 'base64url' ||
          StringPrototypeToLowerCase(encoding) === 'base64url')
        return encodingOps.base64url;
      break;
  }
}

function byteLength(string, encoding) {
  if (typeof string !== 'string') {
    if (isArrayBufferView(string) || isAnyArrayBuffer(string)) {
      return string.byteLength;
    }

    throw new ERR_INVALID_ARG_TYPE(
      'string', ['string', 'Buffer', 'ArrayBuffer'], string,
    );
  }

  const len = string.length;
  if (len === 0)
    return 0;

  if (!encoding || encoding === 'utf8') {
    return byteLengthUtf8(string);
  }

  if (encoding === 'ascii') {
    return len;
  }

  const ops = getEncodingOps(encoding);
  if (ops === undefined) {
    // TODO (ronag): Makes more sense to throw here.
    // throw new ERR_UNKNOWN_ENCODING(encoding);
    return byteLengthUtf8(string);
  }

  return ops.byteLength(string);
}

Buffer.byteLength = byteLength;

// For backwards compatibility.
ObjectDefineProperty(Buffer.prototype, 'parent', {
  __proto__: null,
  enumerable: true,
  get() {
    if (!(this instanceof Buffer))
      return undefined;
    return this.buffer;
  },
});
ObjectDefineProperty(Buffer.prototype, 'offset', {
  __proto__: null,
  enumerable: true,
  get() {
    if (!(this instanceof Buffer))
      return undefined;
    return this.byteOffset;
  },
});

Buffer.prototype.copy =
  function copy(target, targetStart, sourceStart, sourceEnd) {
    return copyImpl(this, target, targetStart, sourceStart, sourceEnd);
  };

// No need to verify that "buf.length <= MAX_UINT32" since it's a read-only
// property of a typed array.
// This behaves neither like String nor Uint8Array in that we set start/end
// to their upper/lower bounds if the value passed is out of range.
Buffer.prototype.toString = function toString(encoding, start, end) {
  if (arguments.length === 0) {
    return this.utf8Slice(0, this.length);
  }

  const len = this.length;

  if (start <= 0)
    start = 0;
  else if (start >= len)
    return '';
  else
    start = MathTrunc(start) || 0;

  if (end === undefined || end > len)
    end = len;
  else
    end = MathTrunc(end) || 0;

  if (end <= start)
    return '';

  if (encoding === undefined)
    return this.utf8Slice(start, end);

  const ops = getEncodingOps(encoding);
  if (ops === undefined)
    throw new ERR_UNKNOWN_ENCODING(encoding);

  return ops.slice(this, start, end);
};

Buffer.prototype.equals = function equals(otherBuffer) {
  if (!isUint8Array(otherBuffer)) {
    throw new ERR_INVALID_ARG_TYPE(
      'otherBuffer', ['Buffer', 'Uint8Array'], otherBuffer);
  }

  if (this === otherBuffer)
    return true;
  const len = TypedArrayPrototypeGetByteLength(this);
  if (len !== TypedArrayPrototypeGetByteLength(otherBuffer))
    return false;

  return len === 0 || _compare(this, otherBuffer) === 0;
};

let INSPECT_MAX_BYTES = 50;
// Override how buffers are presented by util.inspect().
Buffer.prototype[customInspectSymbol] = function inspect(recurseTimes, ctx) {
  const max = INSPECT_MAX_BYTES;
  const actualMax = MathMin(max, this.length);
  const remaining = this.length - max;
  let str = StringPrototypeTrim(RegExpPrototypeSymbolReplace(
    /(.{2})/g, this.hexSlice(0, actualMax), '$1 '));
  if (remaining > 0)
    str += ` ... ${remaining} more byte${remaining > 1 ? 's' : ''}`;
  // Inspect special properties as well, if possible.
  if (ctx) {
    let extras = false;
    const filter = ctx.showHidden ? ALL_PROPERTIES : ONLY_ENUMERABLE;
    const obj = { __proto__: null };
    ArrayPrototypeForEach(getOwnNonIndexProperties(this, filter),
                          (key) => {
                            extras = true;
                            obj[key] = this[key];
                          });
    if (extras) {
      if (this.length !== 0)
        str += ', ';
      // '[Object: null prototype] {'.length === 26
      // This is guarded with a test.
      str += StringPrototypeSlice(utilInspect(obj, {
        ...ctx,
        breakLength: Infinity,
        compact: true,
      }), 27, -2);
    }
  }
  let constructorName = 'Buffer';
  try {
    const { constructor } = this;
    if (typeof constructor === 'function' && ObjectPrototypeHasOwnProperty(constructor, 'name')) {
      constructorName = constructor.name;
    }
  } catch { /* Ignore error and use default name */ }
  return `<${constructorName} ${str}>`;
};
Buffer.prototype.inspect = Buffer.prototype[customInspectSymbol];

Buffer.prototype.compare = function compare(target,
                                            targetStart,
                                            targetEnd,
                                            sourceStart,
                                            sourceEnd) {
  if (!isUint8Array(target)) {
    throw new ERR_INVALID_ARG_TYPE('target', ['Buffer', 'Uint8Array'], target);
  }
  if (arguments.length === 1)
    return _compare(this, target);

  if (targetStart === undefined)
    targetStart = 0;
  else
    validateOffset(targetStart, 'targetStart');

  if (targetEnd === undefined)
    targetEnd = target.length;
  else
    validateOffset(targetEnd, 'targetEnd', 0, target.length);

  if (sourceStart === undefined)
    sourceStart = 0;
  else
    validateOffset(sourceStart, 'sourceStart');

  if (sourceEnd === undefined)
    sourceEnd = this.length;
  else
    validateOffset(sourceEnd, 'sourceEnd', 0, this.length);

  if (sourceStart >= sourceEnd)
    return (targetStart >= targetEnd ? 0 : -1);
  if (targetStart >= targetEnd)
    return 1;

  return compareOffset(this, target, targetStart, sourceStart, targetEnd,
                       sourceEnd);
};

// Finds either the first index of `val` in `buffer` at offset >= `byteOffset`,
// OR the last index of `val` in `buffer` at offset <= `byteOffset`.
//
// Arguments:
// - buffer - a Buffer to search
// - val - a string, Buffer, or number
// - byteOffset - an index into `buffer`; will be clamped to an int32
// - encoding - an optional encoding, relevant if val is a string
// - dir - true for indexOf, false for lastIndexOf
function bidirectionalIndexOf(buffer, val, byteOffset, encoding, dir) {
  validateBuffer(buffer);

  if (typeof byteOffset === 'string') {
    encoding = byteOffset;
    byteOffset = undefined;
  } else if (byteOffset > 0x7fffffff) {
    byteOffset = 0x7fffffff;
  } else if (byteOffset < -0x80000000) {
    byteOffset = -0x80000000;
  }
  // Coerce to Number. Values like null and [] become 0.
  byteOffset = +byteOffset;
  // If the offset is undefined, "foo", {}, coerces to NaN, search whole buffer.
  if (NumberIsNaN(byteOffset)) {
    byteOffset = dir ? 0 : (buffer.length || buffer.byteLength);
  }
  dir = !!dir;  // Cast to bool.

  if (typeof val === 'number')
    return indexOfNumber(buffer, val >>> 0, byteOffset, dir);

  let ops;
  if (encoding === undefined)
    ops = encodingOps.utf8;
  else
    ops = getEncodingOps(encoding);

  if (typeof val === 'string') {
    if (ops === undefined)
      throw new ERR_UNKNOWN_ENCODING(encoding);
    return ops.indexOf(buffer, val, byteOffset, dir);
  }

  if (isUint8Array(val)) {
    const encodingVal =
      (ops === undefined ? encodingsMap.utf8 : ops.encodingVal);
    return indexOfBuffer(buffer, val, byteOffset, encodingVal, dir);
  }

  throw new ERR_INVALID_ARG_TYPE(
    'value', ['number', 'string', 'Buffer', 'Uint8Array'], val,
  );
}

Buffer.prototype.indexOf = function indexOf(val, byteOffset, encoding) {
  return bidirectionalIndexOf(this, val, byteOffset, encoding, true);
};

Buffer.prototype.lastIndexOf = function lastIndexOf(val, byteOffset, encoding) {
  return bidirectionalIndexOf(this, val, byteOffset, encoding, false);
};

Buffer.prototype.includes = function includes(val, byteOffset, encoding) {
  return this.indexOf(val, byteOffset, encoding) !== -1;
};

// Usage:
//    buffer.fill(number[, offset[, end]])
//    buffer.fill(buffer[, offset[, end]])
//    buffer.fill(string[, offset[, end]][, encoding])
Buffer.prototype.fill = function fill(value, offset, end, encoding) {
  return _fill(this, value, offset, end, encoding);
};

function _fill(buf, value, offset, end, encoding) {
  if (typeof value === 'string') {
    if (offset === undefined || typeof offset === 'string') {
      encoding = offset;
      offset = 0;
      end = buf.length;
    } else if (typeof end === 'string') {
      encoding = end;
      end = buf.length;
    }

    const normalizedEncoding = normalizeEncoding(encoding);
    if (normalizedEncoding === undefined) {
      validateString(encoding, 'encoding');
      throw new ERR_UNKNOWN_ENCODING(encoding);
    }

    if (value.length === 0) {
      // If value === '' default to zero.
      value = 0;
    } else if (value.length === 1) {
      // Fast path: If `value` fits into a single byte, use that numeric value.
      if (normalizedEncoding === 'utf8') {
        const code = StringPrototypeCharCodeAt(value, 0);
        if (code < 128) {
          value = code;
        }
      } else if (normalizedEncoding === 'latin1') {
        value = StringPrototypeCharCodeAt(value, 0);
      }
    }
  } else {
    encoding = undefined;
  }

  if (offset === undefined) {
    offset = 0;
    end = buf.length;
  } else {
    validateOffset(offset, 'offset');
    // Invalid ranges are not set to a default, so can range check early.
    if (end === undefined) {
      end = buf.length;
    } else {
      validateOffset(end, 'end', 0, buf.length);
    }
    if (offset >= end)
      return buf;
  }


  if (typeof value === 'number') {
    // OOB check
    const byteLen = TypedArrayPrototypeGetByteLength(buf);
    const fillLength = end - offset;
    if (offset > end || fillLength + offset > byteLen)
      throw new ERR_BUFFER_OUT_OF_BOUNDS();

    TypedArrayPrototypeFill(buf, value, offset, end);
  } else {
    const res = bindingFill(buf, value, offset, end, encoding);
    if (res < 0) {
      if (res === -1)
        throw new ERR_INVALID_ARG_VALUE('value', value);
      throw new ERR_BUFFER_OUT_OF_BOUNDS();
    }
  }

  return buf;
}

Buffer.prototype.write = function write(string, offset, length, encoding) {
  // Buffer#write(string);
  if (offset === undefined) {
    return this.utf8Write(string, 0, this.length);
  }
  // Buffer#write(string, encoding)
  if (length === undefined && typeof offset === 'string') {
    encoding = offset;
    length = this.length;
    offset = 0;

  // Buffer#write(string, offset[, length][, encoding])
  } else {
    validateOffset(offset, 'offset', 0, this.length);

    const remaining = this.length - offset;

    if (length === undefined) {
      length = remaining;
    } else if (typeof length === 'string') {
      encoding = length;
      length = remaining;
    } else {
      validateOffset(length, 'length', 0, this.length);
      if (length > remaining)
        length = remaining;
    }
  }

  if (!encoding || encoding === 'utf8')
    return this.utf8Write(string, offset, length);
  if (encoding === 'ascii')
    return this.asciiWrite(string, offset, length);

  const ops = getEncodingOps(encoding);
  if (ops === undefined)
    throw new ERR_UNKNOWN_ENCODING(encoding);
  return ops.write(this, string, offset, length);
};

Buffer.prototype.toJSON = function toJSON() {
  if (this.length > 0) {
    const data = new Array(this.length);
    for (let i = 0; i < this.length; ++i)
      data[i] = this[i];
    return { type: 'Buffer', data };
  }
  return { type: 'Buffer', data: [] };
};

function adjustOffset(offset, length) {
  // Use Math.trunc() to convert offset to an integer value that can be larger
  // than an Int32. Hence, don't use offset | 0 or similar techniques.
  offset = MathTrunc(offset);
  if (offset === 0) {
    return 0;
  }
  if (offset < 0) {
    offset += length;
    return offset > 0 ? offset : 0;
  }
  if (offset < length) {
    return offset;
  }
  return NumberIsNaN(offset) ? 0 : length;
}

Buffer.prototype.subarray = function subarray(start, end) {
  const srcLength = this.length;
  start = adjustOffset(start, srcLength);
  end = end !== undefined ? adjustOffset(end, srcLength) : srcLength;
  const newLength = end > start ? end - start : 0;
  return new FastBuffer(this.buffer, this.byteOffset + start, newLength);
};

Buffer.prototype.slice = function slice(start, end) {
  return this.subarray(start, end);
};

function swap(b, n, m) {
  const i = b[n];
  b[n] = b[m];
  b[m] = i;
}

Buffer.prototype.swap16 = function swap16() {
  // For Buffer.length < 128, it's generally faster to
  // do the swap in javascript. For larger buffers,
  // dropping down to the native code is faster.
  const len = this.length;
  if (len % 2 !== 0)
    throw new ERR_INVALID_BUFFER_SIZE('16-bits');
  if (len < 128) {
    for (let i = 0; i < len; i += 2)
      swap(this, i, i + 1);
    return this;
  }
  return _swap16(this);
};

Buffer.prototype.swap32 = function swap32() {
  // For Buffer.length < 192, it's generally faster to
  // do the swap in javascript. For larger buffers,
  // dropping down to the native code is faster.
  const len = this.length;
  if (len % 4 !== 0)
    throw new ERR_INVALID_BUFFER_SIZE('32-bits');
  if (len < 192) {
    for (let i = 0; i < len; i += 4) {
      swap(this, i, i + 3);
      swap(this, i + 1, i + 2);
    }
    return this;
  }
  return _swap32(this);
};

Buffer.prototype.swap64 = function swap64() {
  // For Buffer.length < 192, it's generally faster to
  // do the swap in javascript. For larger buffers,
  // dropping down to the native code is faster.
  const len = this.length;
  if (len % 8 !== 0)
    throw new ERR_INVALID_BUFFER_SIZE('64-bits');
  if (len < 192) {
    for (let i = 0; i < len; i += 8) {
      swap(this, i, i + 7);
      swap(this, i + 1, i + 6);
      swap(this, i + 2, i + 5);
      swap(this, i + 3, i + 4);
    }
    return this;
  }
  return _swap64(this);
};

Buffer.prototype.toLocaleString = Buffer.prototype.toString;

let transcode;
if (internalBinding('config').hasIntl) {
  const {
    icuErrName,
    transcode: _transcode,
  } = internalBinding('icu');

  // Transcodes the Buffer from one encoding to another, returning a new
  // Buffer instance.
  transcode = function transcode(source, fromEncoding, toEncoding) {
    if (!isUint8Array(source)) {
      throw new ERR_INVALID_ARG_TYPE('source',
                                     ['Buffer', 'Uint8Array'], source);
    }
    if (source.length === 0) return Buffer.alloc(0);

    fromEncoding = normalizeEncoding(fromEncoding) || fromEncoding;
    toEncoding = normalizeEncoding(toEncoding) || toEncoding;
    const result = _transcode(source, fromEncoding, toEncoding);
    if (typeof result !== 'number')
      return result;

    const code = icuErrName(result);
    const err = genericNodeError(
      `Unable to transcode Buffer [${code}]`,
      { code: code, errno: result },
    );
    throw err;
  };
}

function btoa(input) {
  // The implementation here has not been performance optimized in any way and
  // should not be.
  // Refs: https://github.com/nodejs/node/pull/38433#issuecomment-828426932
  if (arguments.length === 0) {
    throw new ERR_MISSING_ARGS('input');
  }
  const result = _btoa(`${input}`);
  if (result === -1) {
    throw lazyDOMException('Invalid character', 'InvalidCharacterError');
  }
  return result;
}

function atob(input) {
  if (arguments.length === 0) {
    throw new ERR_MISSING_ARGS('input');
  }

  const result = _atob(`${input}`);

  switch (result) {
    case -2: // Invalid character
      throw lazyDOMException('Invalid character', 'InvalidCharacterError');
    case -1: // Single character remained
      throw lazyDOMException(
        'The string to be decoded is not correctly encoded.',
        'InvalidCharacterError');
    case -3: // Possible overflow
      // TODO(@anonrig): Throw correct error in here.
      throw lazyDOMException('The input causes overflow.', 'InvalidCharacterError');
    default:
      return result;
  }
}

function isUtf8(input) {
  if (isTypedArray(input) || isAnyArrayBuffer(input)) {
    return bindingIsUtf8(input);
  }

  throw new ERR_INVALID_ARG_TYPE('input', ['ArrayBuffer', 'Buffer', 'TypedArray'], input);
}

function isAscii(input) {
  if (isTypedArray(input) || isAnyArrayBuffer(input)) {
    return bindingIsAscii(input);
  }

  throw new ERR_INVALID_ARG_TYPE('input', ['ArrayBuffer', 'Buffer', 'TypedArray'], input);
}

module.exports = {
  Buffer,
  SlowBuffer: deprecate(
    SlowBuffer,
    'SlowBuffer() is deprecated. Please use Buffer.allocUnsafeSlow()',
    'DEP0030'),
  transcode,
  isUtf8,
  isAscii,

  // Legacy
  kMaxLength,
  kStringMaxLength,
  btoa,
  atob,
};

ObjectDefineProperties(module.exports, {
  constants: {
    __proto__: null,
    configurable: false,
    enumerable: true,
    value: constants,
  },
  INSPECT_MAX_BYTES: {
    __proto__: null,
    configurable: true,
    enumerable: true,
    get() { return INSPECT_MAX_BYTES; },
    set(val) {
      validateNumber(val, 'INSPECT_MAX_BYTES', 0);
      INSPECT_MAX_BYTES = val;
    },
  },
});

defineLazyProperties(
  module.exports,
  'internal/blob',
  ['Blob', 'resolveObjectURL'],
);
defineLazyProperties(
  module.exports,
  'internal/file',
  ['File'],
);


  const buffer =  readFileSync("readFileSync.js")