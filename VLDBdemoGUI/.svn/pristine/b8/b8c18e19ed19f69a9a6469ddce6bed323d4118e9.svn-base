/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.SplashScreen;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;
import static vldbdemo.mp.Log.bgimage;

/**
 *
 * @author shuhaozhang
 */
public class MySplashScreen extends JFrame implements MouseListener {

    private vldbdemo.VLDBdemo app;

    public MySplashScreen(vldbdemo.VLDBdemo app) {
        this.app = app;
        SplashPanel sp = new SplashPanel();
        JLabel label2 = new JLabel("Click to continue!");
        getContentPane().add(sp, BorderLayout.CENTER);
        getContentPane().add(label2, BorderLayout.SOUTH);
        // setup window correct
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setResizable(false);
        setUndecorated(true);
        pack();
        this.setSize(300, 200);
        // place it at the right position
        Dimension windowDimension = Toolkit.getDefaultToolkit().getScreenSize();

        int x = (windowDimension.width - getWidth()) / 2;
        int y = (windowDimension.height - getHeight()) / 3;

        setLocation(x, y);

        // add the mouse listener
        addMouseListener(this);
    }

    @Override
    public void mouseClicked(MouseEvent me) {
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        dispose();
        app.LoadProgram(app);
    }

    @Override
    public void mousePressed(MouseEvent me) {
        //       throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void mouseReleased(MouseEvent me) {
//        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void mouseEntered(MouseEvent me) {
//        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void mouseExited(MouseEvent me) {
//        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
