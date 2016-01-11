package com.example.adam.vangodrawer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;
import android.provider.MediaStore;
import android.app.Dialog;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.Toast;

import com.example.adam.vangodrawer.BluetoothConnection.BluetoothSerialService;
import com.example.adam.vangodrawer.Drawing.Drawing;
import com.example.adam.vangodrawer.Drawing.DrawingReader;

public class MainActivity extends AppCompatActivity implements OnClickListener{

    private ImageButton newPageBtn, scaleBtn, saveBtn, drawBtn;
    private Drawing drawing;
    private String vangoAddress = "ABC2";
    private UUID appUUID;
    protected static ArrayList<BluetoothDevice> devices;
    BroadcastReceiver mReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        appUUID = UUID.fromString("a11b73f5-9458-401b-819e-a458b9753615");
        setContentView(R.layout.activity_main);
        newPageBtn = (ImageButton)findViewById(R.id.new_btn);
        saveBtn = (ImageButton)findViewById(R.id.save_btn);
        drawBtn = (ImageButton)findViewById(R.id.draw_btn);
        scaleBtn = (ImageButton)findViewById(R.id.scale_btn);
        newPageBtn.setOnClickListener(this);
        saveBtn.setOnClickListener(this);
        drawBtn.setOnClickListener(this);
        scaleBtn.setOnClickListener(this);
        devices = new ArrayList<BluetoothDevice>();
        mReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                // When discovery finds a device
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    // Get the BluetoothDevice object from the Intent
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    // Add the name and address to an array adapter to show in a ListView
                    devices.add(device);
                }
            }
        };
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver, filter);
        drawing = (Drawing)findViewById(R.id.drawing);
    }

    @Override
    protected void onDestroy(){
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }
    @Override
    public void onClick(View view){
        //respond to clicks
        if(view.getId()==R.id.new_btn) {
            //new page button clicked
            drawing.startNew();
        } else if (view.getId() == R.id.scale_btn){
            changePaperSize();
        } else if(view.getId() == R.id.save_btn){
            //save button clicked
            saveImage();
        } else if(view.getId()==R.id.draw_btn){
            //ToDo: SEND FINALISED IMAGE TO ROBOT (ASK TO SAVE?)
            BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            BluetoothDevice foundDevice = null;
            if (mBluetoothAdapter == null) {
                // Device does not support Bluetooth
                Toast noBluetoothToast = Toast.makeText(getApplicationContext(),
                        "Device does not support bluetooth, use another device", Toast.LENGTH_SHORT);
                noBluetoothToast.show();
            } else {
                if (!mBluetoothAdapter.isEnabled()) {
                    //If bluetooth isn't enabled, enable it
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, 9);
                }
                Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
                // for each paired device
                for(BluetoothDevice device : pairedDevices){
                    //Check if paired devices matches the address of it
                    Log.d("Name", device.getName());
                    if(device.getName().equals("japanboxz")){
                        Log.d("Address", device.getAddress());
                        foundDevice = mBluetoothAdapter.getRemoteDevice(device.getAddress());
                        showMessage("Device paired , connecting...");
                    }
                }
                if(foundDevice!=null){
                    Log.d(foundDevice.getName(), foundDevice.getAddress());
                    Log.d("UUID", foundDevice.getUuids()[0].getUuid().toString());

                    BluetoothSerialService bs = new BluetoothSerialService(getApplicationContext());
                    bs.connect(foundDevice, new DrawingReader(drawing.getLineManager()));

                }else{
                    showMessage("VanGoBot was not found");
                }
            }
        }
    }

    private void changePaperSize(){

        final Dialog paperDialog = new Dialog(this);
        paperDialog.setTitle("Paper size:");
        paperDialog.setContentView(R.layout.paper_chooser);
        ImageButton a0Btn = (ImageButton)paperDialog.findViewById(R.id.a0_btn);
        a0Btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                drawing.setPageSize(getResources().getInteger(R.integer.a1_h), getResources().getInteger(R.integer.a0_h));
                scaleBtn.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.a0,null));
                paperDialog.dismiss();
            }
        });
        ImageButton a1Btn = (ImageButton)paperDialog.findViewById(R.id.a1_btn);
        a1Btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                drawing.setPageSize(getResources().getInteger(R.integer.a2_h), getResources().getInteger(R.integer.a1_h));
                scaleBtn.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.a1, null));
                paperDialog.dismiss();
            }
        });
        ImageButton a2Btn = (ImageButton)paperDialog.findViewById(R.id.a2_btn);
        a2Btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                drawing.setPageSize(getResources().getInteger(R.integer.a3_h), getResources().getInteger(R.integer.a2_h));
                scaleBtn.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.a2, null));
                paperDialog.dismiss();
            }
        });
        ImageButton a3Btn = (ImageButton)paperDialog.findViewById(R.id.a3_btn);
        a3Btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                drawing.setPageSize(getResources().getInteger(R.integer.a4_h), getResources().getInteger(R.integer.a3_h));
                scaleBtn.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.a3, null));
                paperDialog.dismiss();
            }
        });
        ImageButton a4Btn = (ImageButton)paperDialog.findViewById(R.id.a4_btn);
        a4Btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                drawing.setPageSize(getResources().getInteger(R.integer.a4_w), getResources().getInteger(R.integer.a4_h));
                scaleBtn.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.a4, null));
                drawing.startNew();
                paperDialog.dismiss();
            }
        });
        paperDialog.show();
    }

    private void saveImage() {
        drawing.setDrawingCacheEnabled(true);
        String imgSaved = MediaStore.Images.Media.insertImage(
                getContentResolver(), drawing.getDrawingCache(),
                UUID.randomUUID().toString()+".png", "drawing");
        if(imgSaved!=null){
            showMessage("Drawing saved to Gallery!");
        }
        else{
            showMessage("Oops! Image could not be saved.");
        }
        drawing.destroyDrawingCache();
    }

    private void showMessage(String s){
        Toast savedToast = Toast.makeText(getApplicationContext(),s, Toast.LENGTH_SHORT);
        savedToast.show();
    }
}
