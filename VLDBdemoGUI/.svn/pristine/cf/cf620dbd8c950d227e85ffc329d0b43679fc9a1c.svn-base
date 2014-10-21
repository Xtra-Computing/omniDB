/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import vldbdemo.mp.MainPannel;
import vldbdemo.bp.ButtonPannel;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import vldbdemo.ID.IDPannel;
import vldbdemo.status.StatusPanel;

/**
 *
 * @author tony
 */
public class Background extends JPanel {

    public VLDBdemo app;     //the Bus Simulation frame
    public int cSpeed = 10;
    public int states = 0;
    //additional component.
    private JSlider slider = new JSlider(JSlider.VERTICAL, 1, 15, 5);
    public StatusPanel sp;
    private IDPannel ip = new IDPannel();
    public ButtonPannel bp;
    public MainPannel mp;

    public Background(VLDBdemo app) {
        this.app = app;
    }

    public void addComponent() {
        slider.setMajorTickSpacing(2);
        slider.setMinorTickSpacing(1);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                cSpeed = slider.getValue();
            }
        });
        //set layout.
        bp = new ButtonPannel(app);
        sp = new StatusPanel(app);
        mp = new MainPannel(app);

        this.setBackground(Color.yellow);
        this.setLayout(new BorderLayout());
        this.add(ip, BorderLayout.NORTH);
        this.add(sp, BorderLayout.SOUTH);

        this.add(slider, BorderLayout.EAST);
        this.add(bp, BorderLayout.WEST);
        this.add(mp, BorderLayout.CENTER);
        this.setSize(1200, 1200);
    }
}
