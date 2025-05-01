require("console")
console.clear();
console.configuration.info.time_stamp.text_color = "yellow";
console.info("beginning time_stamp test");
console.configuration.info.time_stamp.show = true;
console.info("time_stamp is on");
console.info("time_stamp.time_format=" + console.configuration.info.time_stamp.time_format);
console.configuration.info.time_stamp.show = false;
console.info("time_stamp is off");
console.info("ending time_stamp test");
