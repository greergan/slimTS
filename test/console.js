slim.load("console");
console.clear();
console.time();
console.assert();
console.count();
console.countReset();
console.debug("debug works");
console.dir("dir works");
console.dirxml("dirxml works");
console.error("error works");
console.group();
console.groupCollapsed();
console.groupEnd();
console.info("info works");
console.log("log works");
console.print("PRINTLEVEL","print works");
console.table("table works");
console.todo("todo works");
console.trace("trace works");
console.warn("warn works");
console.timeEnd();
console.timeLog();


console.info({});
console.info(()=>{});
console.info(function iamafunction() {});
console.info(false,true);
console.info(100,500);
console.info([0,1,2,3]);
console.info(["a","b","c","d"]);
console.info({"message":"message","value":"value"});
console.info({"true": true, "false": false})
console.info("strings are fun", "they are very very fun");
