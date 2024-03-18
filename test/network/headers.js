slim.load("console");
console.info("begin Headers() testing");
const headers = new Headers();
console.debug("result of new Headers() =>", headers);
headers.append('test', 'test');
console.debug(headers);
console.info("end Headers() testing");