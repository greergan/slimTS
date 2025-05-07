"use strict";
import { debug, log, warn, error, info } from 'console'
debug("debug is working")
log("log is working")
warn("warn is working")
error("error is working")
info("info is working")

import a from './D.js'
info(a)

import { D } from './D.js'
warn("The real D",D)

error("testing")

import{HelloWorld}from'./lib/hello_world.mjs'
error("HelloWorld is an import of 2 other imports which together return => ",HelloWorld());
