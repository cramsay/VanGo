package com.example.adam.vangodrawer;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.UUID;

/**
 * Created by Adam on 09/01/2016.
 */
public class AcceptThread extends Thread {
        private final BluetoothSocket mmServerSocket;
        private DrawingReader drawingReader;

        public AcceptThread(BluetoothAdapter mBluetoothAdapter, BluetoothDevice device, DrawingReader dr) {
            // Use a temporary object that is later assigned to mmServerSocket,
            // because mmServerSocket is final
            //BluetoothServerSocket tmp = null;
            BluetoothSocket tmp = null;
            drawingReader = dr;
            try {
                // MY_UUID is the app's UUID string, also used by the client code
                //tmp = mBluetoothAdapter.listenUsingRfcommWithServiceRecord(device.getName(), UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"));
                tmp = device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"));
            } catch (IOException e) {    }
            mmServerSocket = tmp;
        }

        public void run() {
            //BluetoothSocket socket = null;
            // Keep listening until exception occurs or a socket is returned
            while (true) {
                try {
                    mmServerSocket.connect();
                } catch (IOException e) {
                    Log.d("Bluetooth", "Connecting failed");
                    break;
                }
                // If a connection was accepted
                if (mmServerSocket != null) {
                    // Do work to manage the connection (in a separate thread)
                    new ConnectedThread(mmServerSocket, drawingReader).start();
                    try {
                        mmServerSocket.close();
                    } catch (IOException e) { }
                    break;
                }
            }
        }

        /** Will cancel the listening socket, and cause the thread to finish */
        public void cancel() {
            try {
                mmServerSocket.close();
            } catch (IOException e) { }
        }
}
