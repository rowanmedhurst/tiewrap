// This file is part of Tiewrap and is mostly used to test features as they are implemented.

function eventHandler(e)
{
  console.log("Caught event "+e.type);
  if(e.type == "keydown")
  {
    console.log("keydown "+e.key);
  }
  if(e.type == "keyup")
  {
    console.log("keyup "+e.key);
  }
  if(e.type == "mousedown")
  {
    console.log("mousedown "+e.buttons+" at ("+e.clientX+","+e.clientY+") ("+e.screenX+","+e.screenY+")");
  }
  if(e.type == "mouseup")
  {
    console.log("mouseup "+e.buttons+" at ("+e.clientX+","+e.clientY+") ("+e.screenX+","+e.screenY+")");
  }
  if(e.type == "unload")
  {
    alert("Bye!");
    console.log("This script ran for "+e.timeStamp+" ticks");
    return true;
  }
}

var prevStamp = 0;

function draw(e)
{
  window.title = "Hello, world! FPS: "+Math.round(1000/(e.timeStamp-prevStamp));
  prevStamp = e.timeStamp;
  process.sleep(14);
}

console.log("Welcome to Tiewrap "+process.version+" test suite");
var today = new Date();
console.log(today);
console.log("Using Duktape "+process.versions.duktape+" and SDL "+process.versions.sdl+" on "+process.platform+"/"+window.subsystem);
console.log("Command line: "+process.argv.join(" "));
console.log("Title: "+window.title);
console.log("Size: "+window.innerWidth+"x"+window.innerHeight);
console.log("Position: "+window.screenX+", "+window.screenY);
console.log("Opacity: "+window.opacity);
console.log("Screen brightness: "+window.brightness);

window.on('keydown', eventHandler);
window.on('keyup', eventHandler);
window.on('mousedown', eventHandler);
window.on('mouseup', eventHandler);
window.on('unload', eventHandler);
window.on('draw', draw);

alert("Let's change things around!");
console.log("Process should sleep for 2s...");
process.sleep(2000);
console.log("done.");

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
console.log("Process should now quit with return value "+process.exitCode);  //yeah guys stop trying to fukc juju she is my missus
