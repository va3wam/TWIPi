///////////////////////////////////////////////////////////////////////////////
// This script is used to record telemetry data sent by the TWIPi robot.   
// Script inspired by: https://www.npmjs.com/package/websocket (Websockets)
// and https://stackabuse.com/writing-to-files-in-node-js/ (file IO).
///////////////////////////////////////////////////////////////////////////////
var currentdate = new Date();
var fileName = "NULL"; 
var path = __dirname;
var directory = path.substring(path.indexOf('/'), path.lastIndexOf('/'));            
var WebSocketClient = require('websocket').client;
var client = new WebSocketClient();
var numLines=0;
const fs = require('fs'); // Telemetry log file handle

////////////////////////////////////////////////////////////////////////////
// This event runs when an attempt to connect to the websocket server fails
////////////////////////////////////////////////////////////////////////////
client.on('connectFailed', function(error) 
{
    console.log('[connectFailed] Connect Error: ' + error.toString());
});
 
////////////////////////////////////////////////////////////////////////////
// This event runs when an attempt to connect to the websocket server is
// successful
////////////////////////////////////////////////////////////////////////////
client.on('connect', function(connection) 
{
    console.log('[connect] WebSocket client connected');
    fileName = "tl-"
    + currentdate.getFullYear()  // Get the year as 4 digit number
    + (currentdate.getMonth()+1) // Get the month as a number (0-11)
    + currentdate.getDate()      // Get the day as a number (1-31)
    + currentdate.getHours()     // Get the hour (0-23)
    + currentdate.getMinutes()   // Get the minute (0-59) 
    + currentdate.getSeconds()   // Get the second (0-59)
    + ".csv";                    // append .csv to file name 
    console.log('[connect] Write to file: ' + path + '/' + fileName);
    let writeStream = fs.createWriteStream(fileName); // Open log file
    ////////////////////////////////////////////////////////////////////////
    // This event runs when the client has sent all of the data that is has
    // to the server.
    ////////////////////////////////////////////////////////////////////////
    writeStream.on('finish', () => 
    {  
        console.log('[finish] wrote all data to file');
    });    
    ////////////////////////////////////////////////////////////////////////
    // This event runs when the current connection experiences an error 
    ////////////////////////////////////////////////////////////////////////
    connection.on('error', function(error)
    {
        console.log("[connect] Connection Error: " + error.toString());
    });
    ////////////////////////////////////////////////////////////////////////
    // This event runs when the current connection gets closed by the server 
    ////////////////////////////////////////////////////////////////////////
    connection.on('close', function() 
    {
        console.log('[connect] echo-protocol Connection Closed');
        writeStream.end();  // Close telemetry log file
    });
    ////////////////////////////////////////////////////////////////////////
    // This event runs when a new message is receieved from the server 
    ////////////////////////////////////////////////////////////////////////
    connection.on('message', function(message) 
    {
        if (message.type === 'utf8') 
        {
            console.log("Received: '" + message.utf8Data + "'");
            writeStream.write(message.utf8Data+'\n'); // Write to log file
        }
    });
    
    ////////////////////////////////////////////////////////////////////////
    // This function sends a random number to the server 
    ////////////////////////////////////////////////////////////////////////
    function sendNumber() 
    {
        if (connection.connected) 
        {
            var number = Math.round(Math.random() * 0xFFFFFF);
            connection.sendUTF(number.toString());
            setTimeout(sendNumber, 1000);
        }
    }
    //sendNumber();
});
 
client.connect('ws://192.168.2.70:81/');