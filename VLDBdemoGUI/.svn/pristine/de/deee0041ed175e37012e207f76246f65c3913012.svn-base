/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo.mp;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.event.MouseInputAdapter;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

/**
 *
 * @author shuhaozhang
 */
public class Log extends JTextPane {
    public StyledDocument document;
    public ImageIcon bg;
    public int bgwidth = -1;
    public int bgheight = -1;
    static String Img = "./pictures/ntu_logo.jpg";
    public static Image bgimage;
    private vldbdemo.VLDBdemo app;

   
private void setscroll(){
    this.setAutoscrolls(true);
}
    public Log(vldbdemo.VLDBdemo app) {
        this.app = app;
        bgimage = Toolkit.getDefaultToolkit().getImage(Img);
        while (bgwidth == -1) {
            bgwidth = bgimage.getWidth(this);
        }
        while (bgheight == -1) {
            bgheight = bgimage.getHeight(this);
        }
        
        this.addMouseListener(new MouseInputAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                //setVisible(true);
               // setvi();
                System.out.print("mouse click\n");
                setscroll();
            }
        });
        setEditable(false);
        setPreferredSize(new Dimension(300, 300));
        // And remove default (white) margin
        setBorder(BorderFactory.createEmptyBorder());
        app.style = addStyle(null, null);
        StyleConstants.setBackground(app.style, Color.black);
        StyleConstants.setFontSize(app.style, 20);
        document = getStyledDocument();
        //JScrollPane jScrollPane=new JScrollPane(ja);     
        setVisible(true);
        setAutoscrolls(true);      
        //this.setVisible(true);
        //this.setOpaque(false);
        //setEnabled(false);
        //setFocusable(false);
       
    }

    public void update(String msg, Style style) {
        try {
            document.insertString(document.getLength(), msg, style);
            document.insertString(document.getLength(), "\n", style);
            revalidate();
            repaint();
        } catch (BadLocationException e) {
            System.out.println(e.getMessage());
        }

    }
}
