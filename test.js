// This file is part of Tiewrap and is mostly used to test features as they are implemented.

console.log("Welcome to Tiewrap "+process.version+" test suite");
console.log("Command line: "+process.argv.join(" "));
console.log("Title: "+window.title);
console.log("Size: "+window.innerWidth+"x"+window.innerHeight);
window.title = "Hello, world!";
console.log("Setting title to: "+window.title);

//process.quit();
