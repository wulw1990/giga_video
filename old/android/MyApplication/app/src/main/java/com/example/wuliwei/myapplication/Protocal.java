package com.example.wuliwei.myapplication;

import java.util.zip.InflaterInputStream;

/**
 * Created by wuliwei on 15-12-11.
 */
public class Protocal {
    public int getCmdLen(){
        return CMD_LEN;
    }
    public int getIntLen() { return INT_LEN; }

    private static final int CMD_LEN = 10;
    private static final int INT_LEN = 10;

    public String decodeCmd(byte[] buf){
        String cmd = "";
        for(int i=0; i<CMD_LEN; ++i){
            if(buf[i]!=0){
                cmd += (char)buf[i];
            }
        }
        return cmd;
    }

    public byte[] encodeCmd(String cmd){
        byte[] buf = new byte[CMD_LEN];
        for(int i=0; i<cmd.length(); ++i){
            buf[i] = (byte)cmd.charAt(i);
        }
        return buf;
    }

    public int decodeInt(byte[] buf){
        int len = 0;
        for(int i=1; i<INT_LEN; ++i){
            len *= 10;
            len += buf[i];
        }
        if(buf[0]==1){
            len = -len;
        }
        return len;
    }

    public byte[] encodeInt(int value){
        byte[] buf = new byte[INT_LEN];
        if (value < 0) {
            buf[0] = 1;
            value = -value;
        }
        for (int i = INT_LEN - 1; i >= 1 && value!=0; --i) {
            buf[i] = (byte)(value % 10);
            value /= 10;
        }
        return buf;
    }
}
