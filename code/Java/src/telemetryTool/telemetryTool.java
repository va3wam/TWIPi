
/*******************************************************************************************************
 This Java class acts as a simulator for the SegbotSTEP robot and is intended to be used in conjunction
 with the code found in the wifi_socket.ino sketch (or an event driven replacement planned later).	
 *******************************************************************************************************/
import java.io.*;
import java.net.*;
import java.util.Scanner;
 
//public class Client 
public class telemetryTool 
{
	static Scanner br;
    static BufferedReader reader;
    static BufferedWriter writer;
	static Socket socketClient;
	static String serverMsg;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main class driving socket client code logic
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	public static void main(String argv[])
	{
		String msg = "";
		System.out.println("Top of main routine\r\n");
		connect();
//		while(!msg.equals("bye"))
		sendMSG("test");
		while(true)
		{
//			msg=userINPUT();
//			System.out.println("You typed in '" + msg + "' length = " + msg.length() + "\r\n");
//			sendMSG(msg);
			getReponse();
		} //while
		//System.out.println("You terminated the session\r\n");
		//disconnect();
	} //main()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Connect to the socket server
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	private static void connect()
	{
		try
		{
			socketClient= new Socket("192.168.2.70",81);
		    System.out.println("Client: "+"Connection Established\r\n");
		    reader = new BufferedReader(new InputStreamReader(socketClient.getInputStream()));
		    writer= new BufferedWriter(new OutputStreamWriter(socketClient.getOutputStream())); 
		} //try
		catch(Exception e)
		{
			e.printStackTrace();
		} //catch
	} //connect()
		
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Send message to socket server 
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	private static void sendMSG(String clientMsg)
	{
		System.out.println("Sending: " + clientMsg + "\r\n");
		try
		{
			writer.write(clientMsg +"\r\n");
	        writer.flush();
		} //try
		catch(Exception e)
		{
			e.printStackTrace();
		} //catch
		
	} //sendMSG()

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get response from socket server (not used at this time)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	private static void getReponse()
	{
		try
		{
			while((serverMsg = reader.readLine()) != null)
			{
				System.out.println(serverMsg + "\r\n");
			} //while
	 
		} //try
		catch(Exception e)
		{
			e.printStackTrace();
		} //catch
		
	} //getReponse()

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Disconnect from socket server and console 
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	private static void disconnect()
	{
		try
		{
			socketClient.close();
		    reader.close();
		    writer.close();

		} //try
		catch(Exception e)
		{
			e.printStackTrace();
		} //catch
		
	} //disconnect()

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get user input from the console
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	private static String userINPUT()
	{

		String uinput="";
		try
		{
			System.out.println("Please enter message (typing 'bye' ends session): ");
			br = new Scanner(System.in);
			uinput = br.nextLine();
		} //try
		catch(Exception e)
		{
			e.printStackTrace();
		} //catch
		
		return uinput;
				
	} //userINPUT()
	
} //Class Client()
