package com.SBSMark2.BalanceGraph;

// Classes for graphing
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Frame;
import java.awt.Panel;
import java.io.BufferedReader;
import java.io.BufferedWriter;

// Classes for file I/O and date/time formatting (used for file names)
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
 
// Classes for Websockets
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.charset.Charset;

//Classes for GUI
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import org.eclipse.swt.SWT;
import org.eclipse.swt.awt.SWT_AWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.wb.swt.SWTResourceManager;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.Marker;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.ValueMarker;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYSplineRenderer;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleAnchor;
import org.jfree.ui.TextAnchor;

//Classes or JSON message handling
import org.json.simple.JSONObject;

/**********************************************************************************************************
 * @author andrewmitchell
 * This class plots balancing data sent from the robot via JSON messages onto a WindowsBuilder GUI using
 * using the JFreechart library for graphing and the json-simple-1.1.1.jar library for parsing the JSON 
 * messages. JSON compliance specification (RFC4627) details are here: http://www.ietf.org/rfc/rfc4627.txt.
 * JSON code samples found here: https://www.tutorialspoint.com/json/json_java_example.htm
 * 
 * Newbie Developer Notes:
 * This is a Java GUI application. It was written using the Eclipse IDE and the WindowBuilder WYSIWYG 
 * design plugin. A re-working of this code to make each window its own class is a possible future project. 
 * There is an excellent tutorial on proper class work can be found here: 
 * https://github.com/jfree/jfreechart-swt/blob/master/src/main/java/org/jfree/chart/swt/demo/SWTTimeSeriesDemo1.java
 * User Interface (UI) elements are rendered using SWT. More on each of these concepts follows: 
 * 
 * SWT
 * The Standard Widget Toolkit (SWT) is a graphical widget toolkit for use with the Java platform. It was originally 
 * developed by Stephen Northover at IBM and is now maintained by the Eclipse Foundation in tandem with the Eclipse IDE. 
 * It is an alternative to the Abstract Window Toolkit (AWT) and Swing Java graphical user interface (GUI) toolkits provided 
 * by Sun Microsystems as part of the Java Platform, Standard Edition (J2SE).
 * One practical note here is that button colours etc. cannot be set in SWT because the native theme colours in Windows and 
 * Mac OS control this.
 *
 * Eclipse
 * The Integrated Development Environment (IDE) used to write this code is called Eclipse. Version is called Oxygen. Other IDEs
 * like Netbeans could have been used.
 *
 * Windowbuilder
 * SWT Designer (a.k.a. WindowBuilder)  is a visual editor plugin for Eclipse that is used to create graphical user interfaces. 
 * It is a two way parser, e.g., you can edit the source code or use a graphical editor to modify the user interface. SWT 
 * Designer synchronizes between both representations. SWT Designer is part of the WindowBuilder project. Other WYSIWYG 
 * plug-ins like JBuilder could have been used s well. 
 * 
 * History
 * Version MM-DD-YYYY Description
 * ------- ---------- -------------------------------------------------------------------------------------
 * 2.1     03-03-2018 Fixed bad websocket library. Now using Java-WebSocket-1.3.7.jar. Also added an 
 *                    arrow and text showing which direction the robot is moving with the arrow growing
 *                    as speed increases. This will help to improve the PID tuning process. 
 * 2.0     03-01-2018 All the stuff from 1.9 had to be re-added and project rebuilt after adding eGIT to
 *                    Eclipse. Quite the mess but its all sorted now. Checking in from Eclipse.
 * 1.9     03-01-2018 Added robot balance profile drawing with text showing current angle 
 * 1.8     03-01-2018 Fixed line focus issue as well as graph scaling. Now need to add robot orientation.
 * 1.7     02-28-2018 Analysis table and graph now populate based on file data. User can now scroll through
 *                    Loaded data and the table and graph stay synchronized. Need to fix time line focus
 *                    logic as well as make all the graph plot sizes the same.
 * 1.6     02-27-2018 Added data table and headings. Started on logic to populate table
 * 1.5     02-26-2018 Added Telemetry analyzer window
 * 1.4     02-26-2018 Telemetry is now captured (up to 1000 readings per capture) and put in a time stamped
 *                    file. Capture button label changes between start and stop text as required.
 * 1.3     02-26-2018 JSON messages now handle float variables as String with an 'f' appended to the end.
 *                    Also use real data for the graph now.
 * 1.2     02-25-2018 Changed interface. Added LCD control. Added PID tuning data from server (robot)
 * 1.1     02-23-2018 Changed how we connect to the server. No longer needs user input, use DNS name 
 * 1.0     02-20-2018 Project created. Driven by BalanceGraph and amWebSocketClient classes 
 **********************************************************************************************************/
public class BalanceGraph 
{

	static Shell shlSbsMarkControl;                                     // Main application GUI container
	static Display analyzeDisplay;                                      // Analyzer display
	static Shell analyzeShell;                                          // Analyzer shell

	// Define objects and variables for the Main window line graph
	static XYSeriesCollection dataset;                                  // jframe dataset
    static XYSeries series1;                                            // jframe dataset series 1
    static XYSeries series2;                                            // jframe dataset series 2
    static XYSeries series3;                                            // jframe dataset series 3
    static XYSeries series4;                                            // jframe dataset series 4
    static int xPlot=0;                                                 // jframe XY line graph x plot value
    static float fCapture[][] = new float[1000][4];                     // Float array 100 rows & 4 columns. Is a telemetry buffer 
    static boolean telemetryFlag;                                       // Flag if we want balance data from
                                                                        // robot or not
	// Define objects and variables for the Analysis window line graph
	static XYSeriesCollection daDataset;                                // jframe dataset
    static XYSeries daSeries1;                                          // jframe dataset series 1
    static XYSeries daSeries2;                                          // jframe dataset series 2
    static XYSeries daSeries3;                                          // jframe dataset series 3
    static XYSeries daSeries4;                                          // jframe dataset series 4
    static Button btnLED;                                               // Button for LED control
    static Button btnTeleControl;                                       // Button for telemetry flag
    static amWebSocketClient wsClient;                                  // Handle for Websocket connection
    static URI svr;                                                     // DNS address of server (robot)
    static boolean onboardLEDstate;                                     // Flag to track status of onboard LED            
    public static Label lblLCDline1;                                    // Contains value of LCD line 1
    public static Label lblLCDline2;                                    // Contains value of LCD line 2
    public static Label lblLEDimage;                                    // Image for robot on-board LED status
	public boolean LEDstat = false;                                     // Flag that tracks robot on-board LED status
	public static Text txtLCDline1;                                     // Text box that shows/sets line 1 of LCD on robot
	public static Text txtLCDline2;                                     // Text box that shows/sets line 2 of LCD on robot
	public static File imgFileLEDOFF;                                   // File pointer used to retrieve LED-OFF image from disk
	public static File imgFileLEDON;                                    // File pointer used to retrieve LED-ON image from disk
    public static Label lblPGain;                                       // Label for PID parameter PGain
    public static Label lblIGain;                                       // Label for PID parameter IGain
    public static Label lblDGain;                                       // Label for PID parameter DGain
	public static Text txtPGain;                                        // Text box for PID parameter PGain
	public static Text txtIGain;                                        // Text box for PID parameter IGain
	public static Text txtDGain;                                        // Text box for PID parameter DGain
    public static String sTelemetryData[][] = new String[1000][4];      // Array holding telemetry data loaded from file
    public static Table analysisTable;                                  // Table that lists file content for analysis
    public static Marker mkrPlotLine;                                   // Vertical plot line for graph analysis
    public static XYPlot plot;                                          // Plot handle for plot line on analysis graph   
    public static Group grpTable;                                       // Top group containing SWT widgets for analysis window 
    public static Canvas canvas;                                        // Canvas that robot is drawn on
    public static float robotAngle = 0;                                 // Angle of robot
    public static float robotSpeed = 0;                                 // Speed of robot
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Launch the application.
	// @param args
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static void main(String[] args)
	{

        // Cheater notes for Andrew, that can be removed once we get data telemetry working
    	// Example of how to convert String to float
    	// String numberAsString = "123.4567";
        // float number = Float.parseFloat(numberAsString);
        // System.out.println("Converted string to float. The number is: " + number);
        
        // Example of how to convert float to String
        // String str = String.valueOf(number);
        // System.out.println("Converted float to string. The string is: " + str);
    	
    	// Put running directory in Console 
    	String p = Paths.get(".").toAbsolutePath().normalize().toString();
    	System.out.println("[main] Path is: " + p);
    	
    	try 
		{
    		// Build and display GUI
			BalanceGraph myApp = new BalanceGraph();
			myApp.openConnectingWindow(); // Call function to open and control connecting window
			myApp.open();
		} //try 
		catch (Exception e) 
		{
			e.printStackTrace();
		} //catch
	
	} //main()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Open the connecting window
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	public void openConnectingWindow()
	{

	    int numColumns = 3; // Specifies number of columns in grid used to place widgets on the window
		boolean makeColumnsEqualWidth = false; // Specifies that each column can have a different width
		Display display = Display.getDefault();
		Shell dialogShell = new Shell(display, SWT.APPLICATION_MODAL);

		// Define dialogShell size etc
		dialogShell.setSize(210, 50); // Set size of Shell
		dialogShell.setText("Connecting to SegbotSTEP Mark2"); // Set text in heading bar of shell
		GridLayout gridLayout = new GridLayout(numColumns, makeColumnsEqualWidth);
		dialogShell.setLayout(gridLayout);

		// Define label for line 1 of LCD
		Label lblConnectMessage = new Label(dialogShell, SWT.NONE);
		lblConnectMessage.setBounds(0, 40, 59, 14);
		lblConnectMessage.setText("Connecting to SegbotSTEP Mark2");
		dialogShell.layout(); // Implement the layout of the GUI composites defined in createContents()

		// Open dialogShell 
		dialogShell.open();

		// Connect to server (robot) using Websockets
    	try
    	{
     	   svr = new URI( "ws://espWebSock.local:81"); 
//    	   svr = new URI( "ws://192.168.2.47:81" ); 
    	   wsClient = new amWebSocketClient(svr); 
           wsClient.connect();
    	}
	    catch (URISyntaxException e1) 
    	{
           e1.printStackTrace();
        }

    	//Delay a bit to allow connection to settle before user can interact with it
    	try        
	    {
	        Thread.sleep(2000);
	    } 
	    catch(InterruptedException ex) 
	    {
	        Thread.currentThread().interrupt();
	    }		    		
        
	    // Close out connecting dialog box
        dialogShell.dispose();
    	display.dispose();
		
	} //openConnectingWindow()
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Open the window.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void open() 
	{
		Display display = Display.getDefault(); // Manages the connection between SWT and the underlying operating system
		createContents(); // Method that constructs the GUI for the main window
		shlSbsMarkControl.open(); // Open method of shell called to display main window
		shlSbsMarkControl.layout();	// Implement the layout of the GUI composites defined in createContents()	
		while (!shlSbsMarkControl.isDisposed()) 
		{
			if (!display.readAndDispatch()) 
			{
				display.sleep();
			} //if
		} //while
		display.dispose();
	} //open()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create contents of the window.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected void createContents() 
	{
	    int numColumns = 2; // Specifies number of columns in grid used to place widgets on the window
		boolean makeColumnsEqualWidth = false; // Specifies that each column can have a different width
		imgFileLEDOFF = new File("img/LED-OFF.jpg"); // File pointer used to retrieve LED-OFF image from disk
		imgFileLEDON = new File("img/LED-ON.jpg"); // File pointer used to retrieve LED-ON image from disk

        // Define grid layout for main shell
		GridLayout gridLayout = new GridLayout(numColumns, makeColumnsEqualWidth);

    	// Set up main window container
    	shlSbsMarkControl = new Shell();
		shlSbsMarkControl.setSize(893, 600);
		shlSbsMarkControl.setText("SBS Mark2 Control Center");
		shlSbsMarkControl.setLayout(gridLayout);
		
        // Define an SWT group that will contain robot connection information
		Group grpRobotConnectionStatus = new Group(shlSbsMarkControl, SWT.SHADOW_ETCHED_IN);
		GridData gd_RobotConnectionStatus = new GridData(SWT.LEFT, SWT.CENTER, false, false, 1, 1);
		gd_RobotConnectionStatus.heightHint = 60;
		gd_RobotConnectionStatus.widthHint = 300;
		grpRobotConnectionStatus.setLayoutData(gd_RobotConnectionStatus);
		grpRobotConnectionStatus.setText("Robot Peripheral Readout");		
		grpRobotConnectionStatus.pack();

		// Define label text showing that the LED image indicates the status of the ESP8266 on-board LED status
		Label lblLEDtext = new Label(grpRobotConnectionStatus, SWT.NONE);
		lblLEDtext.setText("Onboard LED: ");
		lblLEDtext.setBounds(0, 0, 64, 19);
		lblLEDtext.pack();

		// Define image that shows the status of the on-board LED. Also add MouseDown event to act as a button
		// so the user can control the state of the LED
		lblLEDimage = new Label(grpRobotConnectionStatus, SWT.NONE);
		lblLEDimage.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e) 
			{
				System.out.println("[createContents] User clicked LED button. LEDstat = " + LEDstat);
				JSONObject tmp = new JSONObject();
				tmp.put("item", "LED");
				tmp.put("action", "set");

				if(LEDstat == true)
				{
					LEDstat = false;
					lblLEDimage.setImage(SWTResourceManager.getImage(imgFileLEDOFF.getPath()));	
				    System.out.println("[createContents] LED currently ON. Sending ledoff message to server");
				    tmp.put("value", "ledoff"); // Tell server to toggle it OFF

				} //if
				else
				{
					LEDstat = true;
					lblLEDimage.setImage(SWTResourceManager.getImage(imgFileLEDON.getPath()));	
			        System.out.println("[createContents] LED currently ON. Sending ledon message to server");
			    	tmp.put("value", "ledon"); // Tell server to toggle it ON
				} //else
	            System.out.println("[createContents] Sending this to server: " + tmp.toString());
			    wsClient.send(tmp.toString()); // Convert JSON object to string and send to server  		
			} //mouseDown()
		}); //lblLEDimage.addMouseListener()
		lblLEDimage.setImage(SWTResourceManager.getImage(imgFileLEDOFF.getPath()));	
		lblLEDimage.setBounds(95, 0, 64, 19);
		lblLEDimage.pack(); // forces label to be visible

		// Define label for line 1 of LCD
		lblLCDline1 = new Label(grpRobotConnectionStatus, SWT.NONE);
		lblLCDline1.setBounds(0, 20, 59, 14);
		lblLCDline1.setText("LCD line 1");

		// Define text box where value of LCD is shown and can be over-written by the user
		txtLCDline1 = new Text(grpRobotConnectionStatus, SWT.BORDER);
		txtLCDline1.setBounds(95, 20, 125, 19);
		txtLCDline1.setTextLimit(16);

		// Define label for line 1 of LCD
		lblLCDline2 = new Label(grpRobotConnectionStatus, SWT.NONE);
		lblLCDline2.setBounds(0, 40, 59, 14);
		lblLCDline2.setText("LCD line 2");

		// Define text box where value of LCD is shown and can be over-written by the user
		txtLCDline2 = new Text(grpRobotConnectionStatus, SWT.BORDER);
		txtLCDline2.setBounds(95, 40, 125, 19);
		txtLCDline2.setTextLimit(16);
		
		//Define button to GET LCD values from robot
		Button btnGETLCD = new Button(grpRobotConnectionStatus, SWT.NONE);
		btnGETLCD.addSelectionListener(new SelectionAdapter() 
		{
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
		       System.out.println("[createContents] User clicked GET LCD button");
		       JSONObject tmp = new JSONObject();
		       tmp.put("item", "LCD");
		       tmp.put("action", "get");
		       tmp.put("line1", "null");
		       tmp.put("line2", "null");
               System.out.println("[createContents] Sending this to server: " + tmp.toString());
		       wsClient.send(tmp.toString()); // Convert JSON object to string and send to server 
			} //widgetSelected()
		}); //btnGETLCD.addSelectionListener()		
		btnGETLCD.setBounds(220, 16, 94, 28);
		btnGETLCD.setText("Get LCD");

		//Define button to SET LCD values from robot
		Button btnSETLCD = new Button(grpRobotConnectionStatus, SWT.NONE);
		btnSETLCD.addSelectionListener(new SelectionAdapter() 
		{
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
		       System.out.println("[createContents] User clicked GET LCD button");
		       JSONObject tmp = new JSONObject();
		       tmp.put("item", "LCD");
		       tmp.put("action", "set");
		       tmp.put("line1", txtLCDline1.getText());
		       tmp.put("line2", txtLCDline2.getText());
               System.out.println("[createContents] Sending this to server: " + tmp.toString());
		       wsClient.send(tmp.toString()); // Convert JSON object to string and send to server 
			} //widgetSelected()
		}); //btnSETLCD.addSelectionListener()				
		btnSETLCD.setBounds(220, 37, 94, 28);
		btnSETLCD.setText("Set LCD");
			
        // Define an SWT group that will contain robot balance telemetry information and controls
		Group grpRobotTelemetry = new Group(shlSbsMarkControl, SWT.SHADOW_ETCHED_IN);
		GridData gd_RobotTelemetry = new GridData(SWT.RIGHT, SWT.TOP, false, false, 1, 1);
		gd_RobotTelemetry.heightHint = 60;
		gd_RobotTelemetry.widthHint = 300;
		grpRobotTelemetry.setLayoutData(gd_RobotTelemetry);
		grpRobotTelemetry.setText("Robot Balance Tuning");		
		grpRobotTelemetry.pack();

		// Define label for P Gain PID tuning varible
		lblPGain = new Label(grpRobotTelemetry, SWT.NONE);
		lblPGain.setBounds(0, 0, 59, 14);
		lblPGain.setText("P Gain:");

		// Define text box where value of PGain is shown and can be over-written by the user
		txtPGain = new Text(grpRobotTelemetry, SWT.BORDER);
		txtPGain.setBounds(95, 0, 125, 19);
		txtPGain.setTextLimit(16);

		// Define label for I Gain PID tuning varible
		lblIGain = new Label(grpRobotTelemetry, SWT.NONE);
		lblIGain.setBounds(0, 20, 59, 14);
		lblIGain.setText("I Gain:");

		// Define text box where value of IGain is shown and can be over-written by the user
		txtIGain = new Text(grpRobotTelemetry, SWT.BORDER);
		txtIGain.setBounds(95, 20, 125, 19);
		txtIGain.setTextLimit(16);

		// Define label for D Gain PID tuning varible
		lblDGain = new Label(grpRobotTelemetry, SWT.NONE);
		lblDGain.setBounds(0, 40, 59, 14);
		lblDGain.setText("D Gain:");

		// Define text box where value of DGain is shown and can be over-written by the user
		txtDGain = new Text(grpRobotTelemetry, SWT.BORDER);
		txtDGain.setBounds(95, 40, 125, 19);
		txtDGain.setTextLimit(16);

		// Define label that tells user what Start/Stop button is for
		lblDGain = new Label(grpRobotTelemetry, SWT.NONE);
		lblDGain.setBounds(225, 0, 94, 28);
		lblDGain.setText("Data Capture");
		
		// Create button that toggles balance telemetry messaging from server
		btnTeleControl = new Button(grpRobotTelemetry, SWT.NONE);
		btnTeleControl.addSelectionListener(new SelectionAdapter() 
		{
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
		       System.out.println("[createContents] User clicked Start/Stop Capture button");
		       JSONObject tmp = new JSONObject();
		       tmp.put("item", "BAL-TEL");
		       tmp.put("action", "set");
		       if(telemetryFlag == true) // If flag is currently TRUE
		       {
		          System.out.println("[createContents] telemetryFlag currently true, setting false and updating server with flagoff");
		    	  tmp.put("value", "flagoff"); // Tell server to toggle it OFF
		    	  telemetryFlag = false;
		    	  btnTeleControl.setText("Start");
		    	  amNewFile(); // Call method that will dump buffer to file
		       } //if
		       else //otherwise assume it is set to FALSE
		       {
		          System.out.println("[createContents] telemetryFlag currently false, setting true and updating server with flagon");
		    	  tmp.put("value", "flagon"); // Tell server to toggle it ON
		    	  telemetryFlag = true;
		    	  btnTeleControl.setText("Stop");
		    	  xPlot=0; //New capture request. Reset pointer to 0 readings 
		       } //if
               System.out.println("[createContents] Sending this to server: " + tmp.toString());
		       wsClient.send(tmp.toString()); // Convert JSON object to string and send to server 
			} //widgetSelected()
		}); //btnTeleControl.addSelectionListener()
		btnTeleControl.setText("Start");
		btnTeleControl.setBounds(220, 15, 94, 28);
		btnTeleControl.pack(); // forces button to be visible

		// Create button that launches data analysis process
		Button btnAnalysis = new Button(grpRobotTelemetry, SWT.NONE);
		btnAnalysis.addSelectionListener(new SelectionAdapter() 
		{
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
		       System.out.println("[createContents] User clicked Review button");
		       amReviewTelemetry(); // Call method to drive logic for reviewing telemetry data
			} //widgetSelected()
		}); //btnAnalysis.addSelectionListener()
		btnAnalysis.setText("Review");
		btnAnalysis.setBounds(220, 35, 94, 28);
		btnAnalysis.pack(); // forces button to be visible

		//------------------------------------------------------------------------------------------------------
        // Draw embedded charts
		//------------------------------------------------------------------------------------------------------
		drawXYChart();                            // Draw embedded XY chart
		shlSbsMarkControl.pack();
		
	} //createContents()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This java class draws an embedded XY chart
    // Examples:
    // http://www.codejava.net/java-se/graphics/using-jfreechart-to-draw-line-chart-with-categorydataset
    // http://www.codejava.net/java-se/graphics/using-jfreechart-to-draw-xy-line-chart-with-xydataset
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    private void drawXYChart() 
	{

    	dataset = new XYSeriesCollection();
        series1 = new XYSeries("Angle x100");
        series1.setMaximumItemCount(20);
        series2 = new XYSeries("PID now");
        series2.setMaximumItemCount(20);
        series3 = new XYSeries("Ground Speed");
        series3.setMaximumItemCount(20);
        series4 = new XYSeries("PID History");
        series4.setMaximumItemCount(20);
        dataset.addSeries(series1);
        dataset.addSeries(series2);
        dataset.addSeries(series3);
        dataset.addSeries(series4);

        // Define JFreechart XY chart look 
	    JFreeChart XYchart = ChartFactory.createXYLineChart
	    		(
	    		   "SBS Mark2 Balancing Graph",        // Title of graph 
	    		   "Period (4 mico seconds)",          // Label X axis
	    		   "Values",                           // Label Y axis
	               dataset,                            // Data set to plot (can have multiple series)
	               PlotOrientation.VERTICAL,           // Orientation of lines
	               true,                               // Display Legend
	               true,                               // Include tool tips
	               false                               // Include URLs
	            );
    
        // Define an SWT group that will contain robot balance telemetry information and controls
	    Group grpGraph = new Group(shlSbsMarkControl, SWT.EMBEDDED);
	    GridData gd_Graph = new GridData(SWT.LEFT, SWT.CENTER, false, false, 1, 1);
	    gd_Graph.horizontalAlignment = GridData.FILL;
	    gd_Graph.horizontalSpan = 2;
	    gd_Graph.heightHint = 391;
	    gd_Graph.widthHint = 799;
	    grpGraph.setLayoutData(gd_Graph);		
	    Frame frame2 = SWT_AWT.new_Frame(grpGraph);
	    
	    //Create panel to put XY JFreeChart in
	    Panel XYPanel = new Panel();
	    frame2.add(XYPanel);
	    XYPanel.setLayout(new BorderLayout(0, 0));
	    
	    // Draw XY chart embedded in its panel
	    ChartPanel XYchartPanel = new ChartPanel(XYchart); //creating the chart panel, which extends JPanel
	    XYPanel.add(XYchartPanel);
	    new Label(shlSbsMarkControl, SWT.NONE);

	} //drawXYChart()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This java class updates the text in the LCD text boxes
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static void updateLCDline(int line, String txt) 
	{
       Display.getDefault().asyncExec(new Runnable() 
       {
    
          public void run() 
          {
             // change/modify components here
    	     if(line==1)
             {
    	    	 txtLCDline1.setText(txt);
             } //if
             else
             {
            	 txtLCDline2.setText(txt);
             } //else
          } //run()  
       }); //Display.getDefault()    	
	} //updateLCDline()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This java class updates the PID tuning data from the server (robot) in text boxes. Note that the
    // JSON library we are using cannot handle floating values so we pass everything as Strings with a lower 
    // case 'f' added on the end to denote that they represent floating point values. Since we are simply 
    // updating text fields that show the PID values we do not need to convert these Strings back to float
    // but we do need to drop the lower case f off the end of the strings. For future reference, here is how 
    // we can convert a String to a float:
	//    String numberAsString = "123.4567";
    //    float number = Float.parseFloat(numberAsString);
    //    System.out.println("Converted string to float. The number is: " + number);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static void updatePIDdata(String P, String I, String D) 
	{
       Display.getDefault().asyncExec(new Runnable() 
       {
    
          public void run() 
          {

        	  txtPGain.setText(P.substring(0, P.length()-1));            // Update P gain text box
        	  txtIGain.setText(I.substring(0, I.length()-1));            // Update I gain text box
        	  txtDGain.setText(D.substring(0, D.length()-1));            // Update D gain text box

          } //run()  
       }); //Display.getDefault()    	
	} //updatePIDdata()
 
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This method generates a date/time stamp file name then dumps the content of the fCapture[][] buffer
    // to the file. The variable xPlot is used to track the number of rows data captured. The max number
    // of lines for a single capture is 1000.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    private void amNewFile() 
	{
    	String tmp;
    	String p = Paths.get(".").toAbsolutePath().normalize().toString();
    	    	
    	// Create new file name called amFileName
    	Date now = new Date();
//        SimpleDateFormat dateFormatter = new SimpleDateFormat("E yyyy:MM:dd 'at' hh.mm.ss a zzz"); // day of week in front
        SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy:MM:dd 'at' hh.mm.ss a zzz"); // no day of week
        String amFileName=dateFormatter.format(now);
        amFileName="telemetry/" + amFileName + ".txt";
        System.out.println("[amNewFile] Open new file called " + amFileName);
        
        // Open file and dump array of values captured in comma-delimited format 
        Path path = Paths.get(amFileName);
    	try(BufferedWriter writer = Files.newBufferedWriter(path, Charset.forName("UTF-8")))
    	{
        	for(int i=1; i<xPlot; i++)
            {
            	tmp=BalanceGraph.fCapture[i][0] + ",";
            	tmp=tmp+BalanceGraph.fCapture[i][1] + ",";
            	tmp=tmp+BalanceGraph.fCapture[i][2] + ",";
            	tmp=tmp+BalanceGraph.fCapture[i][3];
            	tmp=tmp + "\r\n";
            	writer.write(tmp);
            } //for
            System.out.println("[amNewFile] File created. Full path is: " + p + amFileName);
    	}
    	catch(IOException ex)
    	{
    		ex.printStackTrace();
        	System.out.println("[amNewFile] Path is: " + p + ". Ensure there is a directry called telemetry in it.");
    	}
    	       	
	} //amNewFile()
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This method drives the process of analyzing previously captured telemetry data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    private void amReviewTelemetry()
    {
		System.out.println("[amReviewTelemetry] Not sure why this cannot be merged with AnalyzingWindow()");
		// Build and display GUI
		BalanceGraph myApp = new BalanceGraph();
		myApp.AnalyzingWindow(); // Call function to open and control connecting window
   	
    } //amReviewTelemetry()
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Open the analyzing window
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void AnalyzingWindow()
    {

		System.out.println("[AnalyzingWindow] Drive analysis GUI creation");
    	try 
		{
    		// Build and display GUI
			BalanceGraph myApp = new BalanceGraph();
			myApp.createAnalyzingContent();
			myApp.openAnalyzingWindow();
		} //try 
		catch (Exception e) 
		{
			e.printStackTrace();
		} //catch
    	
           		
    } //AnalyzingWindow()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This function draws the analyzing window
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void createAnalyzingContent()
    {

		System.out.println("[createAnalyzingContent] Create conent for Analysis window");
    	int numColumns = 2; // Specifies number of columns in grid used to place widgets on the window
    	boolean makeColumnsEqualWidth = false; // Specifies that each column can have a different width

    	// Define window layout --------------------------------------------------------------------------------
    	GridLayout gridLayout = new GridLayout(numColumns, makeColumnsEqualWidth);
    	analyzeShell = new Shell();
    	analyzeShell.setSize(820, 660); // Set size of Shell
    	analyzeShell.setText("Balance Telemetry Data Analysis"); // Set text in heading bar of shell
    	analyzeShell.setLayout(gridLayout);

        // Define an SWT group that will contain file list, table of file data and a canvas with robot image
 	    grpTable = new Group(analyzeShell, SWT.EMBEDDED);
 	    GridData gd_Table = new GridData(SWT.LEFT, SWT.CENTER, false, false, 1, 1);
 	    gd_Table.horizontalAlignment = GridData.FILL;
 	    gd_Table.horizontalSpan = 2;
 	    gd_Table.heightHint = 200;
 	    gd_Table.widthHint = 300;
 	    grpTable.setLayoutData(gd_Table);		
    
    	// Create a Combo Box ----------------------------------------------------------------------------------
        Combo combo = new Combo(grpTable, SWT.DROP_DOWN);
        combo.setBounds(5, 0, 300, 28);
        // Get list of files ending in .txt and add them to the combo box drop down list
        File my_dir = new File("telemetry/");  // Set directory to use (relative to running directory)
    	String[] filenames_in_dir = my_dir.list(); //Put a list of files in directory into a String array
    	Arrays.sort(filenames_in_dir, Collections.reverseOrder()); // Reverse order sort so latest is at top
    	
    	if(my_dir.exists()) // Does the directory exist
    	{
    	   System.out.println("[createAnalyzingContent] directory " + my_dir + " exists"); 
       	
    	   if(my_dir.isDirectory()) // Is actually a directory
    	   {
        	   System.out.println("[createAnalyzingContent] confirmed that is is indeed a directory"); 
    		   
       	       //Clean out any non .txt files
       	       System.out.println("[createAnalyzingContent] Populate combo box from directory list array");	
       	       for(int i=0; i<filenames_in_dir.length; i++)
               {
       		      System.out.println("[createAnalyzingContent] check if this should be added to the combo box: " + filenames_in_dir[i]);
       		      if(filenames_in_dir[i].contains(".txt"))
       		      {
           		     System.out.println("[createAnalyzingContent] Yes it ends in .txt and should be added. Adding it"); 
           		     combo.add(filenames_in_dir[i]); // Add file to combo box drop down list
                  } //if
       		      else
       		      {
           		     System.out.println("[createAnalyzingContent] No it does not end in .txt and should not be added. Ignoring it"); 
       		      } //else
               } //for
    	   } //if is directory
    	   else
    	   {
        	   System.out.println("[createAnalyzingContent] file spcified is not a directory. Cannot populate combo box");     		   
    	   } //else
    	} //if exists
    	else
    	{
     	   System.out.println("[createAnalyzingContent] directory does not exist. Cannot populate combo box.");     		
    	} //else
    	combo.setText("<pick a log to look at>");
        combo.addSelectionListener(new SelectionListener() 
        {
            // Process valid selection from combo box
        	public void widgetSelected(SelectionEvent e) 
            {
            	System.out.println("[createAnalyzingContent] User selected an item from the combo box:");
            	System.out.println("[createAnalyzingContent] Selected index: " + combo.getSelectionIndex());
            	System.out.println("[createAnalyzingContent] Selected item: " + combo.getItem(combo.getSelectionIndex()));
            	System.out.println("[createAnalyzingContent] Text content in the text field: " + combo.getText());
            	System.out.println("[createAnalyzingContent] Time to load up file and populate table and graph...");
            	amLoadAnalysisArray(combo.getText()); // Call method that will load up the file data
            } //widgetSelected()

            // Process default selection from combo box
        	public void widgetDefaultSelected(SelectionEvent e) 
            {
            	System.out.println("[createAnalyzingContent] User selected default from combo box:");
        		System.out.println("[createAnalyzingContent] Default selected index: " + combo.getSelectionIndex());
        		System.out.println("[createAnalyzingContent] Default selected item: " +
                                   (combo.getSelectionIndex() == -1 ? "<null>" : 
                                	combo.getItem(combo.getSelectionIndex())));
        		System.out.println("[createAnalyzingContent] Text content in the text field: " + combo.getText());
            	System.out.println("[createAnalyzingContent] Will do nothing. User must not select default");
            } // widgetDefaultSelected()
        }); //combo.addSelectionListener()
        combo.pack();
    	
		// Draw table ---------------------------------------------------------------------------------------
        drawAnalysisTable(grpTable); // Draw table with values logged from past data capture

        // Draw XY line graph and canvas with robot image ---------------------------------------------------
        drawAnalysisChart(); // Draw XY line chart
        
		// Draw canvas with robot image ---------------------------------------------------------------------
        Label label = new Label(grpTable, SWT.CENTER);
        label.setText("Robot Balance Profile");	    
        label.setBounds(500, 35, 340, 180);
    	canvas = new Canvas(grpTable,SWT.NONE); // Create a canvas widget to draw on
        canvas.setBounds(550, 25, 248, 180); // Put canvas beside file selection combo box       
        canvas.addPaintListener(new PaintListener() 
        {
           public void paintControl(PaintEvent e) 
            {
                // Draw background of canvas area
                e.gc.setForeground(analyzeDisplay.getSystemColor(SWT.COLOR_BLACK));
                e.gc.setBackground(analyzeDisplay.getSystemColor(SWT.COLOR_WHITE));
                e.gc.fillRectangle(0, 0, 500, 500); //.fillOval(0,0,clientArea.width,clientArea.height);
         	    drawRobot(e, robotAngle, robotSpeed); // Draw image of robot that shows its orientation based on current data slice                 
            } //paintControl()
        }); // addPaintListener()

    } //createAnalyzingContent()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Open the window.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void openAnalyzingWindow() 
	{

		System.out.println("[openAnalyzingWindow] Create new window for balance telemetry analysis");
    	analyzeDisplay = Display.getDefault();
    	analyzeShell.open();
    	analyzeShell.layout();
    	
		while (!analyzeShell.isDisposed()) 
		{
			if (!analyzeDisplay.readAndDispatch()) 
			{
				analyzeDisplay.sleep();
			} //if
		} //while
		analyzeDisplay.dispose();

	} //openAnalyzingWindow()
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This java class draws an embedded XY chart for the analysis shell
    // Examples:
    // http://www.codejava.net/java-se/graphics/using-jfreechart-to-draw-line-chart-with-categorydataset
    // http://www.codejava.net/java-se/graphics/using-jfreechart-to-draw-xy-line-chart-with-xydataset
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    private void drawAnalysisChart() 
	{

		System.out.println("[drawAnalysisChart] Create chart");
    	daDataset = new XYSeriesCollection();
        daSeries1 = new XYSeries("Angle x100");
        daSeries1.setMaximumItemCount(20);
        daSeries2 = new XYSeries("PID now");
        daSeries2.setMaximumItemCount(20);
        daSeries3 = new XYSeries("Ground Speed");
        daSeries3.setMaximumItemCount(20);
        daSeries4 = new XYSeries("PID Integral");
        daSeries4.setMaximumItemCount(20);
        daDataset.addSeries(daSeries1);
        daDataset.addSeries(daSeries2);
        daDataset.addSeries(daSeries3);
        daDataset.addSeries(daSeries4);
        
        // Define JFreechart XY chart look 
	    JFreeChart analysisLineChart = ChartFactory.createXYLineChart
	    		(
	    		   "SBS Mark2 Balancing Graph",        // Title of graph 
	    		   "Period (4 mico seconds)",          // Label X axis
	    		   "Values",                           // Label Y axis
	    		   daDataset,                          // Data set to plot (can have multiple series)
	               PlotOrientation.VERTICAL,           // Orientation of lines
	               true,                               // Display Legend
	               true,                               // Include tool tips
	               false                               // Include URLs
	            );
	    plot = (XYPlot) analysisLineChart.getPlot();   // Graphic rendering handleNot sure exactly what this does
	    plot.setRenderer(new XYSplineRenderer()); // Change lines so that they show plots points and not just the line
	    plot.setDomainCrosshairVisible(true);
	    plot.setRangeCrosshairVisible(true);
        mkrPlotLine = new ValueMarker(10);
        mkrPlotLine.setPaint(Color.red);
        mkrPlotLine.setLabel("Time slice");
        mkrPlotLine.setLabelAnchor(RectangleAnchor.TOP_RIGHT);
        mkrPlotLine.setLabelTextAnchor(TextAnchor.TOP_LEFT);
        plot.addDomainMarker(mkrPlotLine);
                
	    // Define an SWT group that will contain robot balance telemetry information and controls
	    Group grpGraph = new Group(analyzeShell, SWT.EMBEDDED);
	    GridData gd_Graph = new GridData(SWT.LEFT, SWT.CENTER, false, false, 1, 1);
	    gd_Graph.horizontalAlignment = GridData.FILL;
	    gd_Graph.horizontalSpan = 2;
	    gd_Graph.heightHint = 391;
	    gd_Graph.widthHint = 799;
	    grpGraph.setLayoutData(gd_Graph);		
	    Frame frameGraph = SWT_AWT.new_Frame(grpGraph);
	    
	    //Create panel to put XY JFreeChart in
	    Panel analysisLinePanel = new Panel();
	    frameGraph.add(analysisLinePanel);
	    analysisLinePanel.setLayout(new BorderLayout(0, 0));
	    
	    // Draw XY chart embedded in its panel
	    ChartPanel analysisChartPanel = new ChartPanel(analysisLineChart); //creating the chart panel, which extends JPanel
	    analysisLinePanel.add(analysisChartPanel);
	    new Label(analyzeShell, SWT.NONE);
	    
	} //drawAnalysisChart()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw table of telemetry data loaded from disk
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    private void drawAnalysisTable(Group grpTable) 
	{
    
		System.out.println("[drawAnalysisTable] Create table");
//		analysisTable = new Table(grpTable, SWT.MULTI | SWT.BORDER | SWT.FULL_SELECTION); // Can select multiple rows
		analysisTable = new Table(grpTable, SWT.SINGLE| SWT.BORDER | SWT.FULL_SELECTION); // Can only select one row at a time
	    analysisTable.setBounds(5, 25, 340, 180);	    
        String[] titles = { "Angle", "PID", "Ground Speed", "PID Integral"};
        for (int i = 0; i < titles.length; i++) 
        {
          TableColumn column = new TableColumn(analysisTable, SWT.CENTER);
          column.setText(titles[i]);
          column.setWidth(80);
        } //for
        analysisTable.setHeaderVisible(true);

        analysisTable.addSelectionListener(new SelectionAdapter() 
		{
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
		       System.out.println("[drawAnalysisTable] User selected new item in table");
           	   System.out.println("[drawAnalysisTable] Selected index: " + analysisTable.getSelectionIndex());
           	   System.out.println("[drawAnalysisTable] Selected item: " + analysisTable.getItem(analysisTable.getSelectionIndex()));
               amDynamicGraph(analysisTable.getSelectionIndex());
               robotAngle = Float.parseFloat(sTelemetryData[analysisTable.getSelectionIndex()][0]); 
    		   System.out.println("[drawAnalysisTable] Draw robot at an angle of " + robotAngle);
               robotSpeed = Float.parseFloat(sTelemetryData[analysisTable.getSelectionIndex()][2]); 
    		   System.out.println("[drawAnalysisTable] Draw robot speed arrow line using " + robotSpeed);
    		   canvas.redraw(); // Triggers canvas repaint event which in turns calls robotDraw() method

			} //widgetSelected()
		}); //analysisTable.addSelectionListener()

	} //drawAnalysisTable() 
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This method loads up the content of the file selected from the analysis combo box. Columns contain
    // the following: Angle x100, PID now, Ground Speed, PID Integral);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void amLoadAnalysisArray(String fileName)
    {

		System.out.println("[amLoadAnalysisArray] Create table");
    	String fullFileName = "telemetry/" + fileName;
    	int iTmp = 0;
    	BufferedReader br = null;
		FileReader fr = null;
		System.out.println("[amLoadAnalysisArray] Grab content of file " + fullFileName);
		
		try 
		{
			fr = new FileReader(fullFileName);
			br = new BufferedReader(fr);
			String sCurrentLine;
			String splitted[];

			while ((sCurrentLine = br.readLine()) != null) 
			{
				System.out.println("[amLoadAnalysisArray] line " + iTmp + " = " + sCurrentLine);
				splitted = sCurrentLine.split(",",4);
				System.out.println(splitted[0]); 
				sTelemetryData[iTmp][0] = splitted[0];
				System.out.println(splitted[1]);
				sTelemetryData[iTmp][1] = splitted[1];
				System.out.println(splitted[2]);
				sTelemetryData[iTmp][2] = splitted[2];
				System.out.println(splitted[3]);
				sTelemetryData[iTmp][3] = splitted[3];
				iTmp++;
			} //while
			
		} //try 
		catch (IOException e) 
		{
			e.printStackTrace();
		} //catch 
		finally 
		{
			try 
			{
				if (br != null) br.close();
				if (fr != null) fr.close();
			} //try
			catch (IOException ex) 
			{
				ex.printStackTrace();
			} //catch
		} //finally
		int nRows = iTmp-1; // Number of rows read from file and put into array

		// Populate data table
		System.out.println("[amLoadAnalysisArray] Populate table. Number of rows = " + nRows);
		analysisTable.removeAll(); // Clear table of old data
        for (int i = 0; i < nRows; i++) 
        {
        	TableItem item = new TableItem(analysisTable, SWT.NONE);
        	String tmp1 = sTelemetryData[i][0]; System.out.println(tmp1);
        	String tmp2 = sTelemetryData[i][1]; System.out.println(tmp2);
        	String tmp3 = sTelemetryData[i][2]; System.out.println(tmp3);
        	String tmp4 = sTelemetryData[i][3]; System.out.println(tmp4);
        	item.setText(new String[] { tmp1, tmp2, tmp3, tmp4 });
        } //for
        analysisTable.select(0); // Make sure the first item in the table is selected
        analysisTable.showSelection(); // Cause the selection bar to appear so it is clear what item is selected 
        analysisTable.setFocus(); //Make the table the object that is active for keyboard input etc.

        // Populate graph
		System.out.println("[amLoadAnalysisArray] Populate line graph. Number of rows = " + nRows);
		amDynamicGraph(0);
        robotAngle = Float.parseFloat(sTelemetryData[0][0]); 
        robotSpeed = Float.parseFloat(sTelemetryData[0][2]); 
		System.out.println("[drawAnalysisTable] Draw robot at an angle of " + robotAngle);
		System.out.println("[drawAnalysisTable] Draw robot at a speed of " + robotSpeed);
	    canvas.redraw(); // Triggers canvas repaint event which in turns calls robotDraw() method

    } //amLoadAnalysisArray()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This method draws the analysis graph keeping the plot line on the graph synchronized with the line
    // selected on the table of values. Note: This is a great reference for working with plot lines: 
    // http://www.java2s.com/Code/Java/Chart/JFreeChartMarkerDemo1.htm
    // Sample code for moving plot line on analysis graph:
    //    mkrPlotLine = new ValueMarker(selectionIndex);
    //    mkrPlotLine.setLabel("Time slice");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void amDynamicGraph(int selectionIndex)
    {
		int nColumns = 20; // Number of visible plot point in graph
		int sampleSize = analysisTable.getItemCount(); // Number of plot points in table
		int startPoint = selectionIndex - (nColumns/2); // Start by trying to plot half the plots before focus plot
		int endPoint = selectionIndex + (nColumns/2); // Start by trying to put half the plots after focus plot
		if(startPoint < 0) // Check to see if the start plot would end up before the start of the table data
		{
			endPoint += (startPoint *-1); // Add plot points would go before focus to after
			startPoint = 0; // Make start plot = 0 to avoid error of pointing to non-existent data
		} //if
		if(endPoint > sampleSize) // Check to see if the end plot would end up after the last available data
		{
			startPoint += (endPoint - sampleSize); // Add plot points that would go after focus to before
			endPoint = sampleSize; // Make end plot = end of sample to avoid error of pointing to non-existent data 
		} //if
		if(startPoint < 0) // Check once again to see if that start point goes before 0. 
		{	               // This will happen if the data set is smaller then the number of columns to be viewed 
		   startPoint = 0; // Make start plot = 0 to avoid error of pointing to non-existent data
		} //if
		
    	System.out.println("[amDynamicGraph] Data index to draw graph off is " + selectionIndex);
		System.out.println("[amDynamicGraph] Populate line graph. Number of plot points is set to " + nColumns);
        daSeries1.clear(); // Clear series 1 of old data
        daSeries2.clear(); // Clear series 2 of old data
        daSeries3.clear(); // Clear series 3 of old data
        daSeries4.clear(); // Clear series 4 of old data
        for (int i = startPoint; i < endPoint; i++) 
        {
        	float tmp1 = Float.parseFloat(sTelemetryData[i][0]); System.out.println("[amLoadAnalysisArray] " + tmp1);
        	float tmp2 = Float.parseFloat(sTelemetryData[i][1]); System.out.println("[amLoadAnalysisArray] " + tmp2);
        	float tmp3 = Float.parseFloat(sTelemetryData[i][2]); System.out.println("[amLoadAnalysisArray] " + tmp3);
        	float tmp4 = Float.parseFloat(sTelemetryData[i][3]); System.out.println("[amLoadAnalysisArray] " + tmp4);
        	daSeries1.add(i,tmp1*100); //Plot angle as x100
        	daSeries2.add(i,tmp2);
        	daSeries3.add(i,tmp3);
        	daSeries4.add(i,tmp4);
        } //for
        
        // Place plot line marker in graph that shows which set of data points we are focused on
        plot.clearDomainMarkers();
        mkrPlotLine = new ValueMarker(selectionIndex);
        mkrPlotLine.setPaint(Color.red);
        mkrPlotLine.setLabel("Time slice");
        mkrPlotLine.setLabelAnchor(RectangleAnchor.TOP_RIGHT);
        mkrPlotLine.setLabelTextAnchor(TextAnchor.TOP_LEFT);
        plot.addDomainMarker(mkrPlotLine);
    	
    } //amDynamicGraph()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This method draws a simple orientation diagram of the robot to help visualize the angle the robot
    // was at during the current data set reading selected in the table.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public void drawRobot(PaintEvent e, float fAngle, float fSpeed)
    {

    	// Calculate angle to draw line at
        int startX = 120;
        int startY = 150;
        int length = -50;
        int angle = Math.round(fAngle);
        int speed = Math.round(fSpeed);
        System.out.println("[drawRobot] Angle = " + angle);

        // Calculate end points of line
        double endX = startX + Math.cos(Math.toRadians(angle)) * length;
        double endY = startY + Math.sin(Math.toRadians(angle)) * length;
        System.out.println("[drawRobot] endX = " + endX);
        System.out.println("[drawRobot] int endX = " + (int) endX);
        System.out.println("[drawRobot] endY = " + endX);
        System.out.println("[drawRobot] int endY = " + (int) endX);
        
        // Display angle of robot
        Font font = new Font(analyzeDisplay,"Arial",14,SWT.BOLD | SWT.ITALIC);
        e.gc.setForeground(analyzeDisplay.getSystemColor(SWT.COLOR_BLUE));
        e.gc.setFont(font);
        e.gc.drawText(angle + " Degrees",90,40);

        // Display direction of robot
        e.gc.drawText(speed + " cm/sec",90,70);
        font.dispose();
        
        // Draw line which is the body of the robot
        e.gc.setForeground(analyzeDisplay.getSystemColor(SWT.COLOR_DARK_GRAY));
        e.gc.setLineWidth(8);
        e.gc.drawLine(startX, startY, (int)endX, (int)endY);
        e.gc.setLineWidth(1);
        
        // Draw circle which is the wheel of the robot
        e.gc.setBackground(analyzeDisplay.getSystemColor(SWT.COLOR_DARK_GRAY));
        e.gc.fillOval(110, 140, 20, 20);
        
        // Calculate the start and end points of the line
        int sline_col = 120; // start line column
        int sline_row = 90; // start line row 
        int eline_col = sline_col+speed/10; // start line column
        int eline_row = sline_row; // start line row 

        // Display shaft of arrow 
        int len = sline_col - eline_col;
        sline_col=sline_col+(len/2);
        eline_col=eline_col+(len/2);
        e.gc.setForeground(analyzeDisplay.getSystemColor(SWT.COLOR_RED));
        e.gc.setLineWidth(2);
        e.gc.drawLine(sline_col, sline_row, eline_col, eline_row); // use robotSpeed for line length            

    	// Show arrow (or arrows if motors are stopped) at appropriate end of shaft
        e.gc.setBackground(analyzeDisplay.getSystemColor(SWT.COLOR_RED));
        if(speed<=0) // if motors are running backwards or stopped show left arrow
        {
            //Display left arrow at the end of the line        	
        	e.gc.fillPolygon(new int[] { sline_col+6,sline_row,sline_col,sline_row-3,sline_col,sline_row+3 });
            len=sline_col-eline_col;
        } //if

        if(speed>=0) // if motors are running forwards or stopped show right arrow
        {
        	//Display right arrow at the end of the line 
        	e.gc.fillPolygon(new int[] { sline_col-6,sline_row,sline_col,sline_row-3,sline_col,sline_row+3 });
        } //if
        
    } //drawRobot()
 
} //class BalanceGraph
