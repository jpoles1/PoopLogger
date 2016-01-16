Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app ready and running!');
  if(Pebble.getActiveWatchInfo) {
    var watch = Pebble.getActiveWatchInfo();
  } else {
    console.log("Skreeeee")
  }
});
Pebble.addEventListener("appmessage", function(data){
  console.log(data.payload[5])
});
