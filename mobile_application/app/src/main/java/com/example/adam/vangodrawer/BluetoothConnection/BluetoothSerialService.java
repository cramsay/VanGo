package com.example.adam.vangodrawer.BluetoothConnection;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.lang.reflect.Method;
import java.util.Scanner;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.example.adam.vangodrawer.Drawing.Drawing;
import com.example.adam.vangodrawer.Drawing.DrawingReader;
import com.example.adam.vangodrawer.Drawing.LineManager;

import javax.xml.transform.dom.DOMResult;
    /*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Created by ross on 10/01/16.
 */
public class BluetoothSerialService {

        // Debugging
        private static final String TAG = "BluetoothReadService";
        private static final boolean D = true;


        private static final UUID SerialPortServiceClass_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

        // Member fields
        private final BluetoothAdapter mAdapter;
        private ConnectThread mConnectThread;
        private ConnectedThread mConnectedThread;
        private int mState;

        private boolean mAllowInsecureConnections;

        // Constants that indicate the current connection state
        public static final int STATE_NONE = 0;       // we're doing nothing
        public static final int STATE_LISTEN = 1;     // now listening for incoming connections
        public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
        public static final int STATE_CONNECTED = 3;  // now connected to a remote device

        /**
         * Constructor. Prepares a new BluetoothChat session.
         */
        public BluetoothSerialService() {
            mAdapter = BluetoothAdapter.getDefaultAdapter();
            mState = STATE_NONE;
            mAllowInsecureConnections = true;
        }

        /**
         * Set the current state of the chat connection
         * @param state  An integer defining the current connection state
         */
        private synchronized void setState(int state) {
            if (D) Log.d(TAG, "setState() " + mState + " -> " + state);
            mState = state;

            // Give the new state to the Handler so the UI Activity can update
            //mHandler.obtainMessage(BlueTerm.MESSAGE_STATE_CHANGE, state, -1).sendToTarget();
        }

        /**
         * Start the ConnectThread to initiate a connection to a remote device.
         * @param device  The BluetoothDevice to connect
         */
        public synchronized void connect(BluetoothDevice device, DrawingReader drawingReader) {
            if (D) Log.d(TAG, "connect to: " + device);

            // Cancel any thread attempting to make a connection
            if (mState == STATE_CONNECTING) {
                if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}
            }

            // Cancel any thread currently running a connection
            if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

            // Start the thread to connect with the given device
            mConnectThread = new ConnectThread(device, drawingReader);
            mConnectThread.start();
            setState(STATE_CONNECTING);
        }

        /**
         * Start the ConnectedThread to begin managing a Bluetooth connection
         * @param socket  The BluetoothSocket on which the connection was made
         * @param device  The BluetoothDevice that has been connected
         * @param drawingReader The DrawingReader containing the drawing to be sent
         */
        public synchronized void connected(BluetoothSocket socket, BluetoothDevice device, DrawingReader drawingReader) {
            if (D) Log.d(TAG, "connected");

            // Cancel the thread that completed the connection
            if (mConnectThread != null) {
                mConnectThread.cancel();
                mConnectThread = null;
            }

            // Cancel any thread currently running a connection
            if (mConnectedThread != null) {
                mConnectedThread.cancel();
                mConnectedThread = null;
            }

            // Start the thread to manage the connection and perform transmissions
            mConnectedThread = new ConnectedThread(socket, drawingReader);
            mConnectedThread.start();

            setState(STATE_CONNECTED);
        }

        /**
         * Stop all threads
         */
        public synchronized void stop() {
            if (D) Log.d(TAG, "stop");


            if (mConnectThread != null) {
                mConnectThread.cancel();
                mConnectThread = null;
            }

            if (mConnectedThread != null) {
                mConnectedThread.cancel();
                mConnectedThread = null;
            }

            setState(STATE_NONE);
        }

        /**
         * Indicate that the connection attempt failed and notify the UI Activity.
         */
        private void connectionFailed() {
            setState(STATE_NONE);
        }

        /**
         * Indicate that the connection was lost and notify the UI Activity.
         */
        private void connectionLost() {
            setState(STATE_NONE);
        }

        /**
         * This thread runs while attempting to make an outgoing connection
         * with a device. It runs straight through; the connection either
         * succeeds or fails.
         */
        private class ConnectThread extends Thread {
            private final BluetoothSocket mmSocket;
            private final BluetoothDevice mmDevice;
            private DrawingReader drawingReader;

            public ConnectThread(BluetoothDevice device, DrawingReader drawingReader) {
                mmDevice = device;
                this.drawingReader = drawingReader;
                BluetoothSocket tmp = null;

                // Get a BluetoothSocket for a connection with the
                // given BluetoothDevice
                try {
                    if ( mAllowInsecureConnections ) {
                        Method method;

                        method = device.getClass().getMethod("createRfcommSocket", new Class[] { int.class } );
                        tmp = (BluetoothSocket) method.invoke(device, 1);
                    }
                    else {
                        tmp = device.createRfcommSocketToServiceRecord( SerialPortServiceClass_UUID );
                    }
                } catch (Exception e) {
                    Log.e(TAG, "create() failed", e);
                }
                mmSocket = tmp;
            }

            public void run() {
                Log.i(TAG, "BEGIN mConnectThread");
                setName("ConnectThread");

                // Always cancel discovery because it will slow down a connection
                mAdapter.cancelDiscovery();

                // Make a connection to the BluetoothSocket
                try {
                    // This is a blocking call and will only return on a
                    // successful connection or an exception
                    mmSocket.connect();
                } catch (IOException e) {
                    connectionFailed();
                    // Close the socket
                    try {
                        mmSocket.close();
                    } catch (IOException e2) {
                        Log.e(TAG, "unable to close() socket during connection failure", e2);
                    }
                    // Start the service over to restart listening mode
                    //BluetoothSerialService.this.start();
                    return;
                }

                // Reset the ConnectThread because we're done
                synchronized (BluetoothSerialService.this) {
                    mConnectThread = null;
                }

                // Start the connected thread
                connected(mmSocket, mmDevice, drawingReader);
            }

            public void cancel() {
                try {
                    mmSocket.close();
                } catch (IOException e) {
                    Log.e(TAG, "close() of connect socket failed", e);
                }
            }
        }

        /**
         * This thread runs during a connection with a remote device.
         * It handles all incoming and outgoing transmissions.
         */
        private class ConnectedThread extends Thread {
            private final BluetoothSocket mmSocket;
            private final InputStream mmInStream;
            private final OutputStream mmOutStream;
            private DrawingReader drawingReader;


            public ConnectedThread(BluetoothSocket socket, DrawingReader drawingReader) {
                Log.d(TAG, "create ConnectedThread");
                mmSocket = socket;
                InputStream tmpIn = null;
                OutputStream tmpOut = null;
                this.drawingReader = drawingReader;

                // Get the BluetoothSocket input and output streams
                try {
                    tmpIn = socket.getInputStream();
                    tmpOut = socket.getOutputStream();
                } catch (IOException e) {
                    Log.e(TAG, "temp sockets not created", e);
                }

                mmInStream = tmpIn;
                mmOutStream = tmpOut;
            }

            public void run() {
                Log.i(TAG, "BEGIN mConnectedThread");
                byte[] buffer = new byte[1024];
                PrintWriter printWriter = new PrintWriter(mmOutStream);
                Scanner scanner = new Scanner(mmInStream);
                int bytes;
                String moreString = "Meer instructies, alsjeblieft";
                String nextMove = null;
                String test = null;

                // Keep listening to the InputStream while connected
                while (true) {

                    try {
                        Thread.sleep(2000);
                        for(int i = 0; i<128; i++){
                            nextMove = drawingReader.nextMove();
                            if(nextMove!=null){
                                Log.d(TAG, nextMove);
                            }
                            if(nextMove == null) {
                                break;
                            }
                            mmOutStream.write((nextMove + "\n").getBytes());
                        }
                        // Read from the InputStream
                        while(true){
                            if(scanner.hasNextLine()){
                                test =  scanner.nextLine();
                                if(test.equals(moreString)){
                                    Log.d(TAG,test);
                                    break;
                                }
                            }
                        }
                    } catch (Exception e) {
                        try {
                            mmInStream.close();
                            mmOutStream.close();
                        }catch (Exception e1){

                        }
                        Log.e(TAG, "disconnected", e);

                        connectionLost();
                        break;
                    }
                }
            }

            public void cancel() {
                try {
                    mmSocket.close();
                } catch (IOException e) {
                    Log.e(TAG, "close() of connect socket failed", e);
                }
            }
        }

}
