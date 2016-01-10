package com.example.adam.vangodrawer.BluetoothConnection;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import com.example.adam.vangodrawer.Drawing.DrawingReader;

import java.io.IOException;
import java.io.InputStream;
import java.util.Set;
import java.util.UUID;

/**
 * Created by Adam on 09/01/2016.
 */
public class AcceptThread extends Thread {
        private final BluetoothServerSocket mmServerSocket;
        private DrawingReader drawingReader;

        public AcceptThread(BluetoothAdapter mBluetoothAdapter, BluetoothDevice device, DrawingReader dr) {
            // Use a temporary object that is later assigned to mmServerSocket,
            // because mmServerSocket is final
            BluetoothServerSocket tmp = null;
            //BluetoothSocket tmp = null;
            drawingReader = dr;

            Set<BluetoothDevice> pairedDevices =  BluetoothAdapter.getDefaultAdapter().getBondedDevices();

            BluetoothSocket mmSocket;
            BluetoothDevice mmDevice;

            mmDevice = device;

            InputStream mmInputStream;

            for (BluetoothDevice d : pairedDevices) {
                String name = d.getName();
                Log.d("tag",d.getName());
                if (name.contains("japanboxz")) {
                    mmDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(device.getAddress());
                    break;
                }
            }

            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); // Standard SerialPortService ID
            try {
                mBluetoothAdapter.cancelDiscovery();
                mmSocket = mmDevice.createInsecureRfcommSocketToServiceRecord(uuid);





                try {
                    //mmSocket.connect();

                    //mmSocket =(BluetoothSocket) device.getClass().getMethod("createRfcommSocket", new Class[] {int.class}).invoke(device,1);
                    mmSocket.connect();
                    Log.i("Tag", "bluetooth socket connected");
                    try {
                        mmInputStream = mmSocket.getInputStream();
                        Log.i("Tag", "getInputStream succeeded");
                    } catch (IOException e_getin) {
                        Log.i("Tag", "getInputStream failed", e_getin);
                    }
                } catch (IOException econnect) {
                    Log.i("Tag", "connect socket failed", econnect);
                }
            } catch (IOException ecreate) {
                Log.i("Tag", "create socket failed", ecreate);
            }





            try {
                // MY_UUID is the app's UUID string, also used by the client code
                //tmp = mBluetoothAdapter.listenUsingRfcommWithServiceRecord(device.getName(), UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"));
                tmp = mBluetoothAdapter.listenUsingInsecureRfcommWithServiceRecord(device.getName(), UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"));
                //mmSocket = mmDevice.createInsecureRfcommSocketToServiceRecord(uuid);
                //tmp.connect();
                Log.d("wooooo", "bluetooth socket connected");
            } catch (IOException e) {
                Log.d("error","error");
            }
            mmServerSocket = tmp;
        }

        public void run() {

            BluetoothSocket socket = null;
            // Keep listening until exception occurs or a socket is returned
            while (true) {
                try {
                    socket = mmServerSocket.accept(10000);
                } catch (IOException e) {
                    Log.d("Bluetooth", "Connecting failed");
                    break;
                }
                // If a connection was accepted
                if (socket != null) {
                    // Do work to manage the connection (in a separate thread)
                    new ConnectedThread(socket, drawingReader).start();
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
