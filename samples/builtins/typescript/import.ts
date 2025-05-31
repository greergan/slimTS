"use strict";
import { debug, log, warn, error, info } from 'console'
debug("debug is working")
log("log is working")
warn("warn is working")
error("error is working")
info("info is working")

import{HelloWorld}from'./lib/hello_world'
error("HelloWorld is an import of 2 other imports which together return => ",HelloWorld());
