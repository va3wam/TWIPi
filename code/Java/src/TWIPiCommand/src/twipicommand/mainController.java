/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package twipicommand;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;

/**
 *
 * @author andrewmitchell
 */
public class mainController implements Initializable 
{
    
    
    ///////////////////////////////////////////////////////////////////////////
    // This method closes down the Command Console application.
    ///////////////////////////////////////////////////////////////////////////
    @FXML
    private void exitConsole()
    {
        System.exit(1);
    }    
    
    ///////////////////////////////////////////////////////////////////////////
    // This method displays Console Settings window. 
    ///////////////////////////////////////////////////////////////////////////    
    @FXML
    private void showConsoleSettingsScene(ActionEvent event) throws IOException 
    {
        Parent consoleSettingsParent = FXMLLoader.load(getClass().getResource("ConsoleSettings.fxml"));
        Scene consoleScene = new Scene(consoleSettingsParent);
        Stage window = (Stage)((Node)event.getSource()).getScene().getWindow();
        window.setScene(consoleScene);
        window.show();
       
    }
    
    @Override
    public void initialize(URL url, ResourceBundle rb) 
    {
        // TODO
    }    
    
}
