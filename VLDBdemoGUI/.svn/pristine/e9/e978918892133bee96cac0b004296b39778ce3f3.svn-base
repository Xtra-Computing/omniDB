/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.mp.TP;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import javax.swing.*;
import vldbdemo.VLDBdemo;

public class techPanel extends JPanel implements ActionListener, ComponentListener {

    private Color c1 = new Color(79, 129, 189);
    private Image meter;
    private Image needle;
    private Image centerwheel;
    //define fixed variables
    private final static int maxRPM = 580;
    private final static double zeroSpeedDeg = -120.5;
    private final static double maxSpeedDeg = 130.5;
    
    /**
     * referenceGraph[x/10] where x is the rpm, the values inside the array
     * gives the deg to turn from zeroSpeedDeg
     */
    private double[] referenceGraph = {0, 5, 10, 15, 20, 25, 30, 33, 36, 40, 45, 50, 55, 60, 63, 66, 70, 75, 78, 80, 85, 90,
        93, 96, 100, 105, 110, 115, 118, 120.5, 123, 126, 131, 136, 141, 145, 148, 151, 156, 161,
        163, 166, 171, 175, 178, 181, 186, 191, 196, 201, 205, 208, 211, 216, 221, 226, 231,
        236, 241};
    private double targetDeg;
    private int targetRPM;
    private int currentRPM, rpm;
    private Timer timer;
  

    public techPanel(VLDBdemo app, int CPU_GPU) {
        if (CPU_GPU == 0) {
            meter = Toolkit.getDefaultToolkit().getImage("./pictures/speedMeter/CPUmeter.png");
        } else {
            meter = Toolkit.getDefaultToolkit().getImage("./pictures/speedMeter/GPUmeter.png");
        }
        needle = Toolkit.getDefaultToolkit().getImage("./pictures/speedMeter/needle.png");
        centerwheel = Toolkit.getDefaultToolkit().getImage("./pictures/speedMeter/centerwheel.png");
        setBackground(c1);
        targetDeg = zeroSpeedDeg;
        //targetRPM = 500;
        currentRPM = 0;
        timer = new Timer(10, this);
        timer.start();
        setTargetRPM(0);
        //rotateToRPM(600);
        //this.setBackground(Color.red);
    }

    //rotating the needle to the correct RPM
    public void rotateToRPM(int rpm) {
        this.rpm = rpm;

        //if RPM overshot set maxRPM
        if (rpm >= maxRPM) {
            targetDeg = maxSpeedDeg;
        } else if (rpm <= 0) {
            //if RPM below 0 set minRPM
            targetDeg = zeroSpeedDeg;
        } else {
            //set deg to turn to desired rpm
            targetDeg = zeroSpeedDeg + referenceGraph[rpm / 10];
        }
    }

    @Override
    public void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g.create();
        super.paintComponent(g2d);
        this.getWidth();

        int mxco = this.getWidth() / 2 - meter.getWidth(this) / 2;
        int myco = this.getHeight() / 2 - meter.getHeight(this) / 2;
        int rxco = this.getWidth() / 2 + 10;
        int ryco = this.getHeight() / 2 + 15;
        int nxco = this.getWidth() / 2;
        int nyco = this.getHeight() / 2 - needle.getHeight(this);
        int cxco = this.getWidth() / 2 - centerwheel.getWidth(this) / 2;
        int cyco = this.getHeight() / 2 - centerwheel.getHeight(this) / 2 + 15;


        //draw meter
        g2d.drawImage(meter, mxco, myco, this);
        g2d.dispose();
        g2d = (Graphics2D) g.create();
        //rotate abt the rotatePt - which is the center of the centerwheel
        g2d.rotate(Math.toRadians(targetDeg), rxco, ryco);
        //draw the needle
        g2d.drawImage(needle, nxco, nyco, this);
        g2d.dispose();
        g2d = (Graphics2D) g.create();
        //draw centerwheel
        g2d.drawImage(centerwheel, cxco, cyco, this);
        g2d.dispose();
    }

    public void actionPerformed(ActionEvent ae) {
        if (currentRPM < targetRPM) {
            currentRPM += 20;
        } else {
            currentRPM -= 20;
        }

        rotateToRPM(currentRPM);
        repaint();

    }

    public int getTargetRPM() {
        return targetRPM;
    }

    public void setTargetRPM(int targetRPM) {
        this.targetRPM = targetRPM;
    }

    @Override
    public void componentResized(ComponentEvent ce) {
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        System.out.print("Resized!");
        repaint();
    }

    @Override
    public void componentMoved(ComponentEvent ce) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void componentShown(ComponentEvent ce) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void componentHidden(ComponentEvent ce) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
