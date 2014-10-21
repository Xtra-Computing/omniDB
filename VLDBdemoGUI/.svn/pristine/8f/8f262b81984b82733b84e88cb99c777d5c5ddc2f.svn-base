/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.bp;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JPanel;

public class ButtonPannel extends JPanel implements ActionListener {

    public JButton b[] = new JButton[3];             // 3 Buttons
    public boolean bp[] = new boolean[3];            // the state of each button, pressed or not
    private Color c3 = new Color(255, 255, 255);
    private vldbdemo.VLDBdemo app;
    private int index;

    public ButtonPannel(vldbdemo.VLDBdemo app) {                          //create GUI
        this.app = app;
        this.setBackground(c3);
        this.setLayout(new GridLayout(3, 1));
        b[0] = new JButton("STOP");
        b[1] = new JButton("PAUSE");
        b[2] = new JButton("START");

        for (int i = 2; i >= 1; i--) {
            this.add(b[i]);
            b[i].setBackground(Color.CYAN);
            b[i].addActionListener(this);
        }
        bp[0] = true;
        b[0].setBackground(Color.red);
        b[0].addActionListener(this);
        this.add(b[0]);

    }

    public void setStop() {
        index = 0;
        this.app.bg.states = 0;
        for (int i = 2; i >= 1; i--) {
            b[i].setBackground(Color.CYAN);
            bp[i] = false;
        }
        bp[index] = true;
        b[index].setBackground(Color.red);
    }

    @Override
    public void actionPerformed(ActionEvent e) {           //handle the button pressing events

        String str = e.getActionCommand();
        if (str.equals("STOP")) {
            index = 0;
            this.app.bg.states = 0;
        } else if (str.equals("PAUSE")) {
            index = 1;
            this.app.bg.states = 1;
        } else if (str.equals("START")) {
            index = 2;
            this.app.bg.states = 2;
            if (app.loaded == false) {
                System.out.print("Sorry, file not loaded yet!!");
                app.update("Sorry, file not loaded yet!!", null);
                index = 0;
                this.app.bg.states = 0;
            }
        } else {
            index = 0;
            this.app.bg.states = 0;
            System.out.print("Error");
        }
        for (int i = 2; i >= 0; i--) {
            b[i].setBackground(Color.CYAN);
            bp[i] = false;
        }
        bp[index] = true;
        b[index].setBackground(Color.red);
    }
} //the end of ButtonPannel class