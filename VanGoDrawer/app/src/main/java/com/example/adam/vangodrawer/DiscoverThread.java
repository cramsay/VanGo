package com.example.adam.vangodrawer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import java.util.ArrayList;

/**
 * Created by Adam on 09/01/2016.
 */
public class DiscoverThread extends Thread {

    private BluetoothAdapter mBluetoothAdapter;
    private ArrayList<BluetoothDevice> devices;
    public DiscoverThread(BluetoothAdapter bluetoothAdapter, ArrayList<BluetoothDevice> devices){
        mBluetoothAdapter = bluetoothAdapter;

    }
    public void run() {

        mBluetoothAdapter.startDiscovery();

        for (BluetoothDevice d : devices) {
            if (d.getName().equals("japanboxz")) {
                //foundDevice = d;
            }
        }
    }
}
