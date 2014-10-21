/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.mp;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.MouseEvent;
import javax.swing.GroupLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingConstants;
import javax.swing.event.MouseInputAdapter;
import javax.swing.plaf.basic.BasicSplitPaneUI;
import javax.swing.plaf.basic.BasicTreeUI;
import sun.awt.HorizBagLayout;
import vldbdemo.VLDBdemo;
import vldbdemo.mp.TP.TPanel;

/**
 *
 * @author shuhaozhang
 */
public class MainPannel extends JPanel {

    public Log lg;
    public TPanel tp;

    public MainPannel(VLDBdemo app) {
        tp = new TPanel(app);
        lg = new Log(app);
        JScrollPane jScrollPane = new JScrollPane(lg, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

      /*  jScrollPane.addMouseListener(new MouseInputAdapter() {
            public void mouseClicked(MouseEvent e) {
                System.out.print("mouseClicked\n");
            }

            public void mousePressed(MouseEvent e) {
                System.out.print("mousePressed\n");
            }

            public void mouseEntered(MouseEvent e) {
                System.out.print("mouseEntered\n");
                lg.setAutoscrolls(true);
            }
        });*/


        this.setLayout(new GridLayout(2, 1, 0, 20));
        this.add(jScrollPane);
        this.add(tp);
        //setOpaque(false); 
    }
}
