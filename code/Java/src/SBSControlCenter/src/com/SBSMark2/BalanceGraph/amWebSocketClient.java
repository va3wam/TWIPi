package com.SBSMark2.BalanceGraph;
//Classes for WebSocket
import java.net.URI;

//Classes for WebSocket
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.drafts.Draft;
import org.java_websocket.handshake.ServerHandshake;
//import org.java_websocket.drafts.Draft_6455;
//import org.java_websocket.WebSocketImpl;

// Classes for JSON processing. Note we are using JSON Simple. Its JSONObject is derived from HashMap 
// and unfortunately doesn't use generic parameters and is causing Eclipse warning: “Type safety: The 
// method put(Object, Object) belongs to the raw type HashMap.”. Comments in chat groups suggest
// switching to another JSON library like GSON or Jackson.
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/*******************************************************************************************************
This Java class creates an event driven Websocket client used to process incoming JSON formatted
messages.
*******************************************************************************************************/
public class amWebSocketClient extends WebSocketClient 
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Definition override to launch the application.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	public amWebSocketClient( URI serverUri , Draft draft ) 
	{
		super( serverUri, draft );
	} //amWebSocketClient()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Definition override to launch the application.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	public amWebSocketClient( URI serverURI ) 
	{
		super( serverURI );
	} //amWebSocketClient()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New websocket connection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void onOpen( ServerHandshake handshakedata ) 
	{
		
		System.out.println("[onOpen] Opened Websocket connection to server");        
		// Request current balance telemetry flag setting on server
		JSONObject tmp = new JSONObject();
        tmp.put("item","BAL-TEL");
        tmp.put("action","get");
        tmp.put("value",BalanceGraph.telemetryFlag);
		BalanceGraph.wsClient.send(tmp.toString()); // Convert JSON object to string and send to server 
		
	} //onOpen()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New Websocket message received
	// JSON code samples found here: https://www.tutorialspoint.com/json/json_java_example.htm
	// Note that the JSON library we are using cannot handle floating values so we pass everything as Strings 
	// with a lower case 'f' added on the end to denote that they represent floating point values. When
	// working with PID values we need to drop the 'f' off the end of the String using this function:
	// <var>.substring(0, <var>.length()-1), where <var> is the variable being truncated. Next we must 
	// convert the PID Strings back to float variables using the Float.parseFloat() function.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void onMessage( String message ) 
	{
		System.out.println( "[onMessage] received: " + message );

        // Parse of JSON name/value pair
        try 
        {

            JSONParser tmpParser = new JSONParser();
            JSONObject json = (JSONObject) tmpParser.parse(message);
            String item = (String) json.get("item"); // Item element is in all messages, determines JSON format
            System.out.println("[onMessage] item = " + item);
            
            // process incoming JSON message
            if(item.equals("balGraph")) // If this is balance message
            {
                // Drop lower case f off each balance value element passed to use via JSON messages from the server
                String strAngle = (String) json.get("angle"); // line1 element contains message on LCD line 1
            	String strPID = (String) json.get("pid"); // line1 element contains message on LCD line 1
            	String strGspeed = (String) json.get("gspeed"); // line1 element contains message on LCD line 1
            	String strPmem = (String) json.get("pmem"); // line1 element contains message on LCD line 1
                
            	// Convert the String variables above to float variables
            	float fAngle = Float.parseFloat(strAngle.substring(0, strAngle.length()-1)); // Drop 'f' and convert for float
                System.out.println("[onMessage] angle = " + fAngle);
            	float fPID = Float.parseFloat(strPID.substring(0, strPID.length()-1)); // Drop 'f' and convert for float
                System.out.println("[onMessage] pid = " + fPID);
            	float fGspeed = Float.parseFloat(strGspeed.substring(0, strGspeed.length()-1)); // Drop 'f' and convert for float
                System.out.println("[onMessage] gspeed = " + fGspeed);
            	float fPmem = Float.parseFloat(strGspeed.substring(0, strPmem.length()-1)); // Drop 'f' and convert for float
                System.out.println("[onMessage] mem = " + fPmem);

                // Plot new data to each of the series in the graph
                BalanceGraph.xPlot++;
                BalanceGraph.series1.add(BalanceGraph.xPlot,fAngle*100); //Angle plotted at x100
                BalanceGraph.series2.add(BalanceGraph.xPlot,fPID);
                BalanceGraph.series3.add(BalanceGraph.xPlot,fGspeed);
                BalanceGraph.series4.add(BalanceGraph.xPlot,fPmem);
                
 		        // Capture teletry in a buffer to write to file at end of capture. Max rows in buffer is 1000
                if(BalanceGraph.xPlot <= 1000)
                {	
                   BalanceGraph.fCapture[BalanceGraph.xPlot][0] = fAngle; //Angle captured as is
                   BalanceGraph.fCapture[BalanceGraph.xPlot][1] = fPID;
                   BalanceGraph.fCapture[BalanceGraph.xPlot][2] = fGspeed;
                   BalanceGraph.fCapture[BalanceGraph.xPlot][3] = fPmem;
                } //if
                else
                {
                	System.out.println("[onMessage] Capture exceeds 1000. Buffer not updated");
                	
                } //else

            } //if
            else if(item.equals("LED")) // If this is an LCD message
            {
            	System.out.println("[onMessage] Got message with LED value");
            	// Note that we do not care about the action parameter so we ignore it
            	String value = (String) json.get("value"); // Value element tells state of LED
            	System.out.println("[onMessage] value = " + value); 
            	if(value.equals("ledon")) //LED set to "ledgon" 
            	{
                   System.out.println("[onMessage] server reports that the LED is ON. Setting onboardLEDstate = true"); 
                   BalanceGraph.onboardLEDstate = true;
            	} //if
            	else // Otherwise assume the LED set to "ledoff"
            	{
                   System.out.println("[onMessage] server reports that the LED is OFF. Setting onboardLEDstate = false"); 
                   BalanceGraph.onboardLEDstate = false;
             	} //else            			            	
            } // else if
            else if(item.equals("LCD")) // If this is an LCD message
            {
            	System.out.println("[onMessage] Got message with LCD values"); 
            	// Note that we do not care about the action parameter so we ignore it
            	String l1 = (String) json.get("line1"); // line1 element contains message on LCD line 1
            	String l2 = (String) json.get("line2"); // line2 element contains message on LCD line 2
            	System.out.println("[onMessage] line1 = " + l1); 
            	System.out.println("[onMessage] line2 = " + l2); 
                BalanceGraph.updateLCDline(1,l1);
                BalanceGraph.updateLCDline(2,l2);
            } // else if
            else if(item.equals("BAL-TEL")) // If this is balance telemetry flag message
            {
            	// Note that we do not care about the action parameter so we ignore it
            	String value = (String) json.get("value"); // Value element determines set or get message
            	System.out.println("[onMessage] value = " + value); 
            	if(value == "flagon") //Server flag controlling sending of telemetry data is set to "flagon" 
            	{
                   System.out.println("[onMessage] server reports that telemetry flag = flagon. Setting telemetry = true"); 
                   BalanceGraph.telemetryFlag = true;
            	} //if
            	else // Otherwise assume the flag controlling telemetry at the server is set to "flagoff"
            	{
                   System.out.println("[onMessage] server reports that telemetry flag = flagoff. Setting telemetry = false"); 
                   BalanceGraph.telemetryFlag = false;
             	} //else            			
            } //else if
            else if(item.equals("PID")) // If this is PID variable data
            {
            	// Note that we do not care about the action parameter so we ignore it
            	String pGain =  (String) json.get("pGain"); // Grab p Gain value from message
            	System.out.println("[onMessage] pGain = " + pGain); 
            	String iGain = (String) json.get("iGain"); // Grab i Gain value from message
            	System.out.println("[onMessage] iGain = " + iGain); 
            	String dGain = (String) json.get("dGain"); // Grab d Gain value from message
            	System.out.println("[onMessage] dGain = " + dGain); 
            	BalanceGraph.updatePIDdata(pGain, iGain, dGain);
            } //else if
            else  // If message type not one of the ones in this if/else construct
            {
                System.out.println("[onMessage] item = " + item + " is unknown, message ignored");            	
            } //else
            
        } //try
        catch (ParseException e) 
        {
            e.printStackTrace();
        } //catch
        
	} //onMessage()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Websocket connection closing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void onClose( int code, String reason, boolean remote ) 
	{
		// The codecodes are documented in class org.java_websocket.framing.CloseFrame
		System.out.println( "[onClose] Connection closed by " + ( remote ? "remote peer" : "us" ) + " Code: " + code + " Reason: " + reason );
	} //onClose()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Error occurred with Websocket connection. Note that if the error is fatal then onClose will be called 
	// additionally
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void onError( Exception ex ) 
	{
		ex.printStackTrace();
	} //onError()

} //Class SegbotRemote()
