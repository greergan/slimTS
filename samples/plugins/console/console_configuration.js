slim.load("console");
console.clear();
console.configuration.info.time_stamp.show = true;
console.info("printing some info");
console.configuration.info.time_stamp.show = false;
console.info(console.configuration.info.time_stamp);
//console.info(console.configuration.info);
