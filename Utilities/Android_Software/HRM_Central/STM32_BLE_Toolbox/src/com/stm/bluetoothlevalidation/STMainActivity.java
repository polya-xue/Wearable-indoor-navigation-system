package com.stm.bluetoothlevalidation;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;


public class STMainActivity extends Activity {
	
	static final String TAG = "STmainActivity";
	/** Called when the activity is first created. */
    @Override
    
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mainpage);
        
        ImageView img = (ImageView) findViewById(R.id.stm32_picture);
      
        img.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v) {
               Log.v(TAG, "BLE : STM32 clicked ");
            	//Intent intent = new Intent(ST_HRMActivity.this, AboutActivity.class);
               Intent intent = new Intent(STMainActivity.this, DeviceScanActivity.class);
            	
				startActivity(intent);
				finish();
        		
            }
        });  
        
        
    }
    
}