package sbsRemote;
import java.awt.EventQueue;
import javax.swing.JFrame;

public class SBSremote 
{

	private JFrame frame;

	/**
	 * Launch the application.
	 */	
	public static void main(String[] args) 
	{
		EventQueue.invokeLater(new Runnable() 
		{
			public void run() 
			{
				try 
				{
		            PieChart demo = new PieChart("Comparison", "Which operating system are you using?");
		            demo.pack();
		            demo.setVisible(true);
//					SBSremote window = new SBSremote();
//					window.frame.setVisible(true);
				} //try
				catch (Exception e) 
				{
					e.printStackTrace();
				} //catch
			} //run()
		}); //EventQueue
	} //main()

	/**
	 * Create the application.
	 */
	public SBSremote() 
	{
		initialize();
	} //SBSremote()

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() 
	{
		frame = new JFrame();
		frame.setBounds(100, 100, 450, 300);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	} //initialize()

} //SBSremote
