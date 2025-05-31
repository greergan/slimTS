'use strict'
slim.load('console')
console.debug(typeof primordials)
for(const key of Object.keys(primordials)) {
	console.debug("key =>", key)
}
//console.debug(primordials)