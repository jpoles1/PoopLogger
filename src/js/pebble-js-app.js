var ajax = {};
ajax.x=function(){if("undefined"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(var e,t=["MSXML2.XmlHttp.6.0","MSXML2.XmlHttp.5.0","MSXML2.XmlHttp.4.0","MSXML2.XmlHttp.3.0","MSXML2.XmlHttp.2.0","Microsoft.XmlHttp"],n=0;n<t.length;n++)try{e=new ActiveXObject(t[n]);break}catch(o){}return e},ajax.send=function(e,t,n,o,a){var r=ajax.x();r.open(n,e,a),r.onreadystatechange=function(){4==r.readyState&&t(r.responseText)},"POST"==n&&r.setRequestHeader("Content-type","application/x-www-form-urlencoded"),r.send(o)},ajax.get=function(e,t,n,o){var a=[];for(var r in t)a.push(encodeURIComponent(r)+"="+encodeURIComponent(t[r]));ajax.send(e+(a.length?"?"+a.join("&"):""),n,"GET",null,o)},ajax.post=function(e,t,n,o){var a=[];for(var r in t)a.push(encodeURIComponent(r)+"="+encodeURIComponent(t[r]));ajax.send(e,n,"POST",a.join("&"),o)};

var url = "http://192.168.1.109:8000"
//var url = "http://poopguru.herokuapp.com";
Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app ready and running!');
  if(Pebble.getActiveWatchInfo) {
    var watch = Pebble.getActiveWatchInfo();
  } else {
    console.log("Skreeeee")
  }
});
Pebble.addEventListener("appmessage", function(data){
  bristol = data.payload[5];
  console.log("Bristol val is:", bristol)
  var http = new XMLHttpRequest();
  var d = new Date();
  var idvar = Pebble.getAccountToken();
  if(localStorage.getItem("uname")){
    idvar=localStorage.getItem("uname");
  }
  ajax.get(url+"/tracker", {"bristol": bristol, "hour": d.getHours(), "dow": d.getDay(), "id": idvar}, function(){console.log("SENT")});
});
Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL(url+"/newUserForm");
});
