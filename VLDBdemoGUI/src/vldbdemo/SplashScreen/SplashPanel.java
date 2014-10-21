/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.SplashScreen;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import javax.swing.ImageIcon;
import javax.swing.JPanel;


/**
 *
 * @author shuhaozhang
 */
public class SplashPanel extends JPanel {

    public ImageIcon bg;
    public int bgwidth = -1;
    public int bgheight = -1;
    static String Img = "./pictures/splash.gif";
    public static Image bgimage;

    public SplashPanel() {
        bgimage = Toolkit.getDefaultToolkit().getImage(Img);
        while (bgwidth == -1) {
            bgwidth = bgimage.getWidth(this);
        }
        while (bgheight == -1) {
            bgheight = bgimage.getHeight(this);
        }
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(bgimage, 0, 0, bgwidth, bgheight ,this);
    }
}
