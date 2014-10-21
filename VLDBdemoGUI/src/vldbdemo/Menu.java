/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vldbdemo;

import java.awt.Component;
import java.awt.Dialog;
import java.awt.PopupMenu;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;
import javax.swing.text.Style;
import java.io.File;
import java.io.IOException;

/**
 *
 * @author shuhaozhang
 */
public class Menu extends JMenuBar {

    vldbdemo.VLDBdemo app;
    Scanner ScCB;
    Scanner ScGB;
    Scanner ScTime;
    private int select = 0;

    private void update(String msg, Style style) {
        app.bg.mp.lg.update(msg, style);
        app.bg.mp.lg.setVisible(true);
    }

    private vldbdemo.VLDBdemo getApp() {
        return this.app;
    }

    private void skipvoid() {
        while (ScCB.hasNext()) {
            double a = ScCB.nextDouble();
            double b = ScGB.nextDouble();
            if (!(a == 0 && b == 0)) {
                return;
            }
//            voidFront++;
        }
    }

    private void loadData() {
        skipvoid();
        int i = 0;
        while (ScCB.hasNext()) {
            this.app.cpu_burden[i++] = ScCB.nextDouble();
        }
        this.app.totalIndex_Burden = i;
        app.update("this.app.totalIndex_Burden" + this.app.totalIndex_Burden, null);
        this.app.bg.sp.setPBMax(i);
        i = 0;
        while (ScGB.hasNext()) {
            this.app.gpu_burden[i++] = ScGB.nextDouble();
        }
        // app.update("this.app.totalIndex_Burden" + i, null);
        i = 0;
        while (ScTime.hasNext()) {
            this.app.spendTime = ScTime.nextDouble();
        }
        this.app.totalIndex_Schedule = i;
        this.app.loaded = true;
    }

    private void setvisible() {
        app.frame.setVisible(true);
    }

    public Menu(vldbdemo.VLDBdemo app) {
        this.app = app;

        /*
         * Menu 1
         */
        JMenu Menu1 = new JMenu("Load");
        Menu1.setMnemonic('F');
        JMenuItem QueryScheduler = new JMenuItem("QueryScheduler");
        QueryScheduler.setMnemonic('Q');
        Menu1.add(QueryScheduler);
        JMenuItem OperatorScheduler = new JMenuItem("OperatorScheduler");
        OperatorScheduler.setMnemonic('O');
        Menu1.add(OperatorScheduler);
        JMenuItem KernelScheduler = new JMenuItem("KernelScheduler");
        KernelScheduler.setMnemonic('K');
        Menu1.add(KernelScheduler);
        Menu1.addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent me) {
                setvisible();
                //setVisible(true);
            }

            @Override
            public void menuDeselected(MenuEvent me) {
//                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
                // setVisible(false);
                // setvisible();
            }

            @Override
            public void menuCanceled(MenuEvent me) {
//                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
                //setVisible(false);
            }
        });


        QueryScheduler.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    ScCB = new Scanner(new File("./Experiment/QueryScheduler/ExpOut_cpuBurden.tony"));
                    ScGB = new Scanner(new File("./Experiment/QueryScheduler/ExpOut_gpuBurden.tony"));
                    ScTime = new Scanner(new File("./Experiment/QueryScheduler/Q_level_Time.tony"));

                    loadData();
                    select = 0;
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    update("File read done\n", null);
                    System.out.print("File read done\n");
                }
            }
        });

        KernelScheduler.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    ScCB = new Scanner(new File("./Experiment/KernelScheduler/ExpOut_cpuBurden.tony"));
                    ScGB = new Scanner(new File("./Experiment/KernelScheduler/ExpOut_gpuBurden.tony"));
                    ScTime = new Scanner(new File("./Experiment/KernelScheduler/K_level_Time.tony"));
                    loadData();
                    select = 2;
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    update("File read done\n", null);
                    System.out.print("File read done\n");
                }
            }
        });
        OperatorScheduler.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    ScCB = new Scanner(new File("./Experiment/OperatorScheduler/ExpOut_cpuBurden.tony"));
                    ScGB = new Scanner(new File("./Experiment/OperatorScheduler/ExpOut_gpuBurden.tony"));
                    ScTime = new Scanner(new File("./Experiment/OperatorScheduler/O_level_Time.tony"));
                    loadData();
                    select = 1;
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    update("File read done\n", null);
                    System.out.print("File read done\n");
                }
            }
        });
        /*
         * Menu 4
         */
        JMenu Menu4 = new JMenu("Run");
        Menu4.setMnemonic('R');
        JMenuItem QueryScheduler2 = new JMenuItem("QueryScheduler");
        QueryScheduler2.setMnemonic('Q');
        Menu4.add(QueryScheduler2);
        JMenuItem OperatorScheduler2 = new JMenuItem("OperatorScheduler");
        OperatorScheduler2.setMnemonic('O');
        Menu4.add(OperatorScheduler2);
        JMenuItem KernelScheduler2 = new JMenuItem("KernelScheduler");
        KernelScheduler2.setMnemonic('K');
        Menu4.add(KernelScheduler2);
        Menu4.addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent me) {
                setvisible();
            }

            @Override
            public void menuDeselected(MenuEvent me) {
//                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }

            @Override
            public void menuCanceled(MenuEvent me) {
//                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }
        });


        QueryScheduler2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    ProcessBuilder b = new ProcessBuilder("../Experiment/Q-30-8/CoProcessor.exe", "", "");
                    Process p = b.start();
                    InputStream is = p.getInputStream();
                    InputStreamReader isr = new InputStreamReader(is);
                    BufferedReader br = new BufferedReader(isr);
                    String line;
                    while ((line = br.readLine()) != null) {
                        System.out.println(line);
                        update(line, null);
                    }
                    System.out.println("Program terminated!");
                } catch (Exception ex) {
                    // Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    System.out.print("File read done\n");
                }
            }
        });

        KernelScheduler2.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    ScCB = new Scanner(new File("./Experiment/KernelScheduler/ExpOut_cpuBurden.txt"));
                    ScGB = new Scanner(new File("./Experiment/KernelScheduler/ExpOut_gpuBurden.txt"));
                    ScTime = new Scanner(new File("./Experiment/KernelScheduler/ExpOut_Scheduler.txt"));
                    loadData();
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    System.out.print("File read done\n");
                }
            }
        });
        OperatorScheduler2.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    ScCB = new Scanner(new File("./Experiment/OperatorScheduler/ExpOut_cpuBurden.txt"));
                    ScGB = new Scanner(new File("./Experiment/OperatorScheduler/ExpOut_gpuBurden.txt"));
                    ScTime = new Scanner(new File("./Experiment/OperatorScheduler/ExpOut_Scheduler.txt"));
                    loadData();
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(VLDBdemo.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    System.out.print("File read done\n");
                }
            }
        });



        /*
         * Menu 2
         */
        JMenu Menu2 = new JMenu("About");
        Menu2.addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent e) {
                //throw new UnsupportedOperationException("Not supported yet.");
                //System.out.println("menuSelected");
                JOptionPane.showMessageDialog(null, "Hello,I'm Zhang Shu Hao");
            }

            @Override
            public void menuDeselected(MenuEvent e) {
                //throw new UnsupportedOperationException("Not supported yet.");
            }

            @Override
            public void menuCanceled(MenuEvent e) {
                //throw new UnsupportedOperationException("Not supported yet.");
            }
        });


        /*
         * Menu 3
         */
        JMenu Menu3 = new JMenu("HandShaking");
        Menu3.addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent e) {
                //default icon, custom title
                int ans = JOptionPane.showConfirmDialog(
                        null,
                        "Would you like to run handshaking?",
                        "Caution",
                        JOptionPane.YES_NO_OPTION);

                if (ans == JOptionPane.YES_OPTION) {
                    JOptionPane.showMessageDialog(null, "Please wait until handshaking finish execution");
                    try {
                        //new ProcessBuilder("c:/Users/f11301bs/Dropbox/TONY/FYP/VLDBDemo13/Experiment/Handshaking/DLL_OpenCL.exe", null, null).start();
                        update("Start to run handshaking program!Please wait!", null);

                        ProcessBuilder b = new ProcessBuilder("../Experiment/Handshaking/DLL_OpenCL.exe", "../Experiment/Handshaking/", "");
                        Process p = b.start();
                        InputStream is = p.getInputStream();
                        InputStreamReader isr = new InputStreamReader(is);
                        BufferedReader br = new BufferedReader(isr);
                        String line;
                        while ((line = br.readLine()) != null) {
                            System.out.println(line);
                            update(line, null);
                        }
                        System.out.println("Program terminated!Copy data to destination\n");
                        InputStream inStream = null;
                        OutputStream outStream = null;


                        File afile = new File("../Experiment/Handshaking/KernelTimeSpecification.list");
                        File bfile = new File("../Experiment/K-30-8/KernelTimeSpecification.list");
                        File cfile = new File("../Experiment/O-30-8/KernelTimeSpecification.list");
                        File dfile = new File("../Experiment/Q-30-8/KernelTimeSpecification.list");
                        inStream = new FileInputStream(afile);


                        outStream = new FileOutputStream(bfile);
                        byte[] buffer = new byte[1024];

                        int length;
                        //copy the file content in bytes 
                        while ((length = inStream.read(buffer)) > 0) {
                            outStream.write(buffer, 0, length);
                        }
                        inStream = new FileInputStream(afile);
                        outStream = new FileOutputStream(cfile);
                        buffer = new byte[1024];


                        //copy the file content in bytes 
                        while ((length = inStream.read(buffer)) > 0) {
                            outStream.write(buffer, 0, length);
                        }
                        inStream = new FileInputStream(afile);
                        outStream = new FileOutputStream(dfile);
                        buffer = new byte[1024];


                        //copy the file content in bytes 
                        while ((length = inStream.read(buffer)) > 0) {
                            outStream.write(buffer, 0, length);
                        }

                        inStream.close();
                        outStream.close();

                        System.out.println("File is copied successful!\n");


                    } catch (IOException ex) {
                    }
                } else {
                    //JOptionPane.showMessageDialog(null, "GOODBYE");
                }
            }

            @Override
            public void menuDeselected(MenuEvent e) {
                //throw new UnsupportedOperationException("Not supported yet.");
            }

            @Override
            public void menuCanceled(MenuEvent e) {
                //throw new UnsupportedOperationException("Not supported yet.");
            }
        });

        /*
         * Menu 5
         */
        JMenu Menu5 = new JMenu("Show result");
        Menu5.addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent me) {
                // throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.

                if (getApp().loaded) {
                    switch (select) {
                        case 0: {
                            JOptionPane.showMessageDialog(null, "Query level schedule time spend: " + getApp().spendTime);
                            break;
                        }
                        case 1: {
                            JOptionPane.showMessageDialog(null, "Operator level schedule time spend: " + getApp().spendTime);
                            break;
                        }
                        case 2: {
                            JOptionPane.showMessageDialog(null, "Kernel level schedule time spend: " + getApp().spendTime);
                            break;
                        }
                    }
                } else {
                    JOptionPane.showMessageDialog(null, "Please load data first!!");
                }

            }

            @Override
            public void menuDeselected(MenuEvent me) {
                //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }

            @Override
            public void menuCanceled(MenuEvent me) {
                // throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }
        });
        // this.add(Menu3);
        // this.add(Menu4);

        this.add(Menu1);
        this.add(Menu5);
        this.add(Menu2);
    }
}
