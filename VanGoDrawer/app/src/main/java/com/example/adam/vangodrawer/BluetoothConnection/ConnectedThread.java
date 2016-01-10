package com.example.adam.vangodrawer.BluetoothConnection;

import android.bluetooth.BluetoothSocket;

import com.example.adam.vangodrawer.Drawing.DrawingReader;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;

/**
 * Created by Adam on 09/01/2016.
 */
public class ConnectedThread extends Thread{
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private DrawingReader drawingReader;

    public ConnectedThread(BluetoothSocket socket, DrawingReader dr) {
        drawingReader = dr;
        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    public void run() {
        byte[] buffer = new byte[1024];  // buffer store for the stream
        int bytes; // bytes returned from read()
        PrintWriter writer = new PrintWriter(mmOutStream);
        String next;
        // Keep listening to the InputStream until an exception occurs
        while (true) {
            for (int i = 0; i<128; i++){
                next = drawingReader.nextMove();
                writer.write(next + "|");
            }
            //ToDo: Block while listening for message to say arduino is out of instructions
        }
    }

    /* Call this from the main activity to send data to the remote device */
    public void write(byte[] bytes) {
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) { }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

}
