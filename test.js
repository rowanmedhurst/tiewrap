// This file is part of Tiewrap and is mostly used to test features as they are implemented.

console.log("Welcome to Tiewrap "+process.version+" test suite");
console.log("Using Duktape "+process.versions.duktape+" and SDL "+process.versions.sdl+" on "+process.platform+"/"+window.subsystem);
console.log("Command line: "+process.argv.join(" "));
console.log("Title: "+window.title);
console.log("Size: "+window.innerWidth+"x"+window.innerHeight);
console.log("Position: "+window.screenX+", "+window.screenY);
console.log("Opacity: "+window.opacity);
console.log("Screen brightness: "+window.brightness);

alert("Let's change things around!");

window.title = "Hello, world!";
console.log("Setting title to: "+window.title);
window.innerWidth = 800;
window.innerHeight = 600;
console.log("Setting size to: "+window.innerWidth+"x"+window.innerHeight);
window.screenX = 0;
window.screenY = 0;
console.log("Setting position to: "+window.screenX+", "+window.screenY);
window.opacity = 0.75;
console.log("Setting opacity to: "+window.opacity);
window.brightness = 0.5;
console.log("Setting brightness to: "+window.brightness);

process.exitCode = 42;
console.log("Process should now quit with return value "+process.exitCode);

//process.exit();