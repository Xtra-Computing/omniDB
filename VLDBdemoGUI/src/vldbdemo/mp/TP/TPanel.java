/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.mp.TP;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.LayoutManager;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.Timer;
import javax.swing.text.StyleConstants;

/**
 *
 * @author shuhaozhang
 */
public class TPanel extends JPanel implements ActionListener {

    private techPanel CPUTP;
    private techPanel GPUTP;
    private Timer timer;
    private vldbdemo.VLDBdemo app;
    private int count = 10;
    private int index = 0;//this is the main control point
    private int update = 0;
    private int precmd = 0;

    public TPanel(vldbdemo.VLDBdemo app) {

        this.app = app;
        //this.setBackground(Color.red);
        CPUTP = new techPanel(app, 0);
        GPUTP = new techPanel(app, 1);
        this.setLayout(new GridLayout(0, 2));
        this.add(CPUTP);
        this.add(GPUTP);
        timer = new Timer(1, this);//basic unit 1ms.
        timer.start();
        pbupdate();
        //repaint();
    }

    private int checkCMD() {
        int cmd = 0;
        if (app.bg.bp.bp[0]) {//stop being pressed
            cmd = 0;
        } else if (app.bg.bp.bp[1]) {//pause being pressed
            cmd = 1;
        } else {
            cmd = 2;
        }
        return cmd;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        int cmd = checkCMD();
        if (app.loaded == false) {
            cmd = precmd;
        }
        if (cmd != precmd) {
            update = 0;
            precmd = cmd;
        }
        switch (cmd) {
            case 0: {
                //System.out.println("System Stop!");
                app.bg.sp.state.setText("Stop");
                if (update == 0) {
                    StyleConstants.setForeground(app.style, Color.yellow);
                    StyleConstants.setFontSize(app.style, 20);
                    app.update("=====Stop====", app.style);
                    update = 1;
                }
                index = 0;
                CPUTP.setTargetRPM(0);
                GPUTP.setTargetRPM(0);
                break;
            }
            case 1: {
                //System.out.print("System Pause!");
                app.bg.sp.state.setText("Pause");
                if (update == 0) {
                    StyleConstants.setForeground(app.style, Color.yellow);
                    StyleConstants.setFontSize(app.style, 20);
                    app.update("=====Pause====", app.style);
                    update = 1;
                }
                break;
            }
            case 2: {
                app.bg.sp.state.setText("Start");
                if (update == 0) {
                    StyleConstants.setForeground(app.style, Color.yellow);
                    StyleConstants.setFontSize(app.style, 20);
                    app.update("=====Start====", app.style);
                    update = 1;
                }
                if (count == 0) {
                    count = app.bg.cSpeed * 1;
                    int cpuB = ((int) (app.cpu_burden[index] * 0.1));
                    int gpuB = ((int) (app.gpu_burden[index] * 0.1));
                    CPUTP.setTargetRPM(cpuB);
                    GPUTP.setTargetRPM(gpuB);
                    app.update("CPU burden:" + cpuB + "               " + "GPU burden:" + gpuB + "\n", null);
                    index++;
                    if (index == app.totalIndex_Burden) {
                        app.bg.bp.setStop();
                    }
                } else {
                    count--;
                }
                break;
            }
        }
    }

    public void pbupdate() {
        Runnable runner = new Runnable() {
            @Override
            public void run() {
                while (true) {
                    app.bg.sp.pb.setValue(index);
                    if (index == app.totalIndex_Burden && app.loaded) {
                        app.update("Query execution finished: spend time:" + app.spendTime, null);
                    }
                    app.setVisible(true);
                }
            }
        };
        Thread t = new Thread(runner, "Code Executer");
        t.start();
    }
}
