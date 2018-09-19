/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tcommand;

import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
//import javafx.scene.control.Label;
import javafx.scene.control.SingleSelectionModel;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;

/**
 *
 * @author andrewmitchell
 */
public class FXMLDocumentController implements Initializable 
{
    ///////////////////////////////////////////////////////////////////////////
    // Link FXMLDocumentfxml elements to this Java controller
    // These IDs are defined in Scene Builder under the Code
    //tab in the FX:ID filed.
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private TabPane tabPane;
    @FXML
    private Tab tabMissions;
    @FXML
    private Tab tabPeripherals;
    @FXML
    private Tab tabBalance;
    @FXML
    private Tab tabNavigation;
    @FXML
    private Tab tabRobotSettings;
    @FXML
    private Tab tabConsoleSettings;
    @FXML
    private SingleSelectionModel<Tab> selectionModel;
  
    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Balance tab of the Command Console
    // UI. The call for this function is defined in the FXMLDocument.fxml file
    // for the MenuItem text="Balance Telemetry" 
    // onAction="#handlemnuOpenBalanceTelemetry"
    // Scene Builder note: 
    // Linked in Scene Builder for Menu Item "Balance Telemetry" in the code 
    // tab in the OnAction field. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuOpenBalanceTelemetry(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabBalance);
        
    }

    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Navigation tab of the Command 
    // Console UI. The call for this function is defined in the 
    // FXMLDocument.fxml file for the MenuItem text="Navigate Telemetry" 
    // onAction="#handlemnuOpenNavigate Telemetry"
    // Scene Builder note: 
    // Linked in Scene Builder for Menu Item "Navigate Telemetry" in the code 
    // tab in the OnAction field. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuOpenNavigateTelemetry(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabNavigation);
        
    }

    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Peripherals tab of the Command 
    // Console UI. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuEditPeripherals(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabPeripherals);
        
    }

    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Robot Settings tab of the Command 
    // Console UI. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuEditSettingsRobot(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabRobotSettings);
        
    }

    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Missions tab of the Command 
    // Console UI. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuEditMissions(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabMissions);
        
    }

    ///////////////////////////////////////////////////////////////////////////
    // This event handler sets focus on the Console Settings tab of the Command 
    // Console UI. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void handlemnuEditSettingsConsole(ActionEvent event) {

        selectionModel = tabPane.getSelectionModel();
        selectionModel.select(tabConsoleSettings);
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // This event handler terminates the COmmand Console application. The call 
    // for this function is defined in the FXMLDocument.fxml file for the 
    // MenuItem text="Close" onAction="#exitConsole"
    // Scene Builder note: 
    // Linked in Scene Builder for Menu Item "Close" in the code 
    // tab in the OnAction field. 
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void exitConsole()
    {
        System.exit(1);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // I have no idea what this is for at this time.
    ///////////////////////////////////////////////////////////////////////////
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        // TODO
    }    
    
}
