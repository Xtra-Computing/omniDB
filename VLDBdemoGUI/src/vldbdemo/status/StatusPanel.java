/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.status;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.SwingConstants;

/**
 *
 * @author shuhaozhang
 */
public class StatusPanel extends JPanel {

    public JLabel state = new JLabel("Stop");               // display ???   
    public JProgressBar pb;
    public vldbdemo.VLDBdemo app;
    private int totalNum;

    public StatusPanel(vldbdemo.VLDBdemo app) {
        this.app = app;
        pb = new JProgressBar();
        pb.setValue(0);
        pb.setStringPainted(true);
        //pb.setOpaque(true);
        // pb.setBorder(BorderFactory.createEmptyBorder(1, 1, 1, 1));
        this.add(pb);
        this.setLayout(new GridLayout(2, 0));
        this.add(state);
        state.setHorizontalAlignment(SwingConstants.CENTER);

    }

    public void setPBMax(int totalNum) {
        this.totalNum = totalNum;
        pb.setMaximum(totalNum);
    }
}
