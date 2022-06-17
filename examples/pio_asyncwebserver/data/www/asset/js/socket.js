var socket_keep_alive_period  = 500;
var socket_keep_alive_enabled = false;
var socket_connect_display    = true;
var socket_display            = true;
var socket_connected          = false;
var web_socket                = null;
var myTimeout;

function socket_status_display() {
}

function socket_keep_alive() {
    if (!web_socket) return;
    if (web_socket.readyState !== 1) return;
    setTimeout(socket_keep_alive, socket_keep_alive_period);
}
function socket_receive(msg) {
    if (socket_display) {console.log("<<<[socket_receive]>>\n"+msg);console.log(JSON.parse(msg));console.log("---");}
    const [err, result] = safeJsonParse(msg);
    if (err) {
        console.log('\tFailed to parse JSON: ' + err.message);
        return;
    }
    if (msg == "null") {
        console.log('\tFailed to parse JSON(null)');
        return;     
    }   
    // document.getElementById("test").appendChild(renderjson(JSON.parse(msg)));
    ListBasePtr().socketParse(msg);
    ProgramPtr().socketParse(msg);
    PlaylistPtr().socketParse(msg);
    playlisItemsPtr().socketParse(msg);
    playlisItemsDispPtr().socketParse(msg);

    setData();
}
function socket_send(msg) {
    // if (! socket_connected ) socket_initialize();
    web_socket.send(msg);
    if (socket_display) {console.log("<<<[socket_send]>>>\n"+msg);console.log(JSON.parse(msg));console.log("---");}
}
function socket_initialize(){
    console.log("\n[socket_initialize]\n");
    // web_socket = new WebSocket('ws://' + document.location.host + ':81/',['arduino']);
    web_socket = new WebSocket('ws://' + document.location.host + '/ws',['arduino']);
    // web_socket = new WebSocket('ws://' + window.location.hostname + ':2000/');
    // web_socket = new ReconnectingWebSocket('ws://' + window.location.hostname + ':81/', ['arduino']);
    web_socket.binaryType = "arraybuffer";
    web_socket.debug = true;
    console.log('ws://'+document.location.host+'/ws\n');

    web_socket.onopen = function(e) { 
        if (socket_connect_display) console.log("[socket onopen] ", e);
        socket_connected=true;
        socket_status_display();

        setTimeout(function() {  socket_send(api_request(0, "SOKCET", [],["loop"])); },     250);
        setTimeout(function() {  socket_send(api_request(0, "SOKCET", [],["listc"])); },    1000);
        setTimeout(function() {  socket_send(api_request(0, "SOKCET", [],["js_init"])); },  1500);

        if (socket_keep_alive_enabled) keep_alive_time_out=setTimeout(socket_keep_alive, socket_keep_alive_period);
    };
     
    web_socket.onclose = function(e){
        if (socket_connect_display) console.log("[socket onclose] ", e);
        socket_connected=false;
        socket_status_display();
        // setTimeout(function() {  socket_init()  }, 1000);
    };
     
    web_socket.onerror = function(e){
        if (socket_connect_display) console.log("[socket onerror] ", e);
        socket_connected=false;
        // setTimeout(function() {  socket_init()  }, 1000);
        socket_status_display();
    };
     
    web_socket.onmessage = function(e){
        var msg = "";
        if (e.data instanceof ArrayBuffer){
            msg = "BIN:";
            var bytes = new Uint8Array(e.data);
            for (var i = 0; i < bytes.length; i++) {
                msg += String.fromCharCode(bytes[i]);
            }
        } else {
            msg = e.data;
        }
        socket_receive(msg);
    };
  

}