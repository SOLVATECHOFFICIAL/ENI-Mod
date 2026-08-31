package com.enimod.bloodstrike;

public class NativeBridge {
    static {
        System.loadLibrary("enimod");
    }
    
    public static native void initHooks();
    public static native void setAimbotEnabled(boolean enabled);
    public static native void setESPEnabled(boolean enabled);
    public static native void setNoRecoilEnabled(boolean enabled);
    public static native void setAimSmooth(float smooth);
    public static native void setHeadshotChance(float chance);
    public static native void setStealthMode(boolean enabled);
}
