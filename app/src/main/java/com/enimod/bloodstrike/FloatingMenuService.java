package com.enimod.bloodstrike;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

public class FloatingMenuService extends Service {
    private WindowManager windowManager;
    private View menuView;
    private boolean isMenuOpen = false;
    
    // Feature toggles
    private boolean aimbotEnabled = true;
    private boolean espEnabled = true;
    private boolean noRecoilEnabled = true;
    private float aimSmooth = 0.15f;
    private float headshotChance = 0.15f;
    
    @Override
    public void onCreate() {
        super.onCreate();
        windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        createFloatingMenu();
    }
    
    private void createFloatingMenu() {
        // Invisible trigger button (small, semi-transparent)
        LayoutInflater inflater = LayoutInflater.from(this);
        View triggerView = inflater.inflate(R.layout.floating_trigger, null);
        
        WindowManager.LayoutParams triggerParams = new WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
            PixelFormat.TRANSLUCENT
        );
        triggerParams.gravity = Gravity.TOP | Gravity.START;
        triggerParams.x = 50;
        triggerParams.y = 100;
        
        windowManager.addView(triggerView, triggerParams);
        
        // Menu panel (hidden by default)
        menuView = inflater.inflate(R.layout.mod_menu, null);
        setupMenuControls();
        
        // Toggle menu on trigger click
        triggerView.setOnClickListener(v -> toggleMenu());
        
        // Drag functionality
        triggerView.setOnTouchListener(new View.OnTouchListener() {
            private int initialX, initialY;
            private float touchX, touchY;
            
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = triggerParams.x;
                        initialY = triggerParams.y;
                        touchX = event.getRawX();
                        touchY = event.getRawY();
                        return true;
                    case MotionEvent.ACTION_MOVE:
                        triggerParams.x = initialX + (int)(event.getRawX() - touchX);
                        triggerParams.y = initialY + (int)(event.getRawY() - touchY);
                        windowManager.updateViewLayout(triggerView, triggerParams);
                        return true;
                }
                return false;
            }
        });
    }
    
    private void setupMenuControls() {
        // Aimbot toggle
        Switch aimSwitch = menuView.findViewById(R.id.aimbot_switch);
        aimSwitch.setChecked(aimbotEnabled);
        aimSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            aimbotEnabled = isChecked;
            NativeBridge.setAimbotEnabled(isChecked);
        });
        
        // ESP toggle
        Switch espSwitch = menuView.findViewById(R.id.esp_switch);
        espSwitch.setChecked(espEnabled);
        espSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            espEnabled = isChecked;
            NativeBridge.setESPEnabled(isChecked);
        });
        
        // No recoil toggle
        Switch recoilSwitch = menuView.findViewById(R.id.recoil_switch);
        recoilSwitch.setChecked(noRecoilEnabled);
        recoilSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            noRecoilEnabled = isChecked;
            NativeBridge.setNoRecoilEnabled(isChecked);
        });
        
        // Smooth aim slider
        SeekBar smoothSlider = menuView.findViewById(R.id.smooth_slider);
        smoothSlider.setProgress((int)(aimSmooth * 100));
        smoothSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                aimSmooth = progress / 100.0f;
                NativeBridge.setAimSmooth(aimSmooth);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        
        // Headshot chance slider
        SeekBar hsSlider = menuView.findViewById(R.id.headshot_slider);
        hsSlider.setProgress((int)(headshotChance * 100));
        hsSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                headshotChance = progress / 100.0f;
                NativeBridge.setHeadshotChance(headshotChance);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        
        // Stealth mode button (all features to human-like defaults)
        Button stealthBtn = menuView.findViewById(R.id.stealth_button);
        stealthBtn.setOnClickListener(v -> {
            aimSmooth = 0.25f;
            headshotChance = 0.10f;
            NativeBridge.setStealthMode(true);
            // Update UI
            smoothSlider.setProgress(25);
            hsSlider.setProgress(10);
        });
    }
    
    private void toggleMenu() {
        if (isMenuOpen) {
            windowManager.removeView(menuView);
        } else {
            WindowManager.LayoutParams menuParams = new WindowManager.LayoutParams(
                600, 800,
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT
            );
            menuParams.gravity = Gravity.CENTER;
            windowManager.addView(menuView, menuParams);
        }
        isMenuOpen = !isMenuOpen;
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        if (menuView != null) windowManager.removeView(menuView);
    }
    
    @Override
    public IBinder onBind(Intent intent) { return null; }
}
