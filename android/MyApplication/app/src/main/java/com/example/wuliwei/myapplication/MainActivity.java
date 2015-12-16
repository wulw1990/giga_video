package com.example.wuliwei.myapplication;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.FloatMath;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.URL;

public class MainActivity extends Activity {

    private static final String TAG = "GIGA_VIDEO";

    private Button m_buttonConnect;
    private EditText m_editTextIp;
    private EditText m_editTextPort;
    private TextView m_textViewState;
    private ImageView m_imageView;

    private Bitmap m_bitmap;

    private static final int MESSAGE_TYPE_DOWNLOAD_IMG = 1;
    private static final int MESSAGE_TYPE_STATE = 2;

    private static int touch_mode;
    private static float touch_x;
    private static float touch_y;
    private static float touch_dis;
    private static final int TOUCH_MODE_DRAG = 1;
    private static final int TOUCH_MODE_ZOOM = 2;

    private Socket socket = null;
    InputStream socket_input_stream = null;
    OutputStream socket_output_stream = null;

    private Thread socketThead;
    private String m_state = "";

    private Protocal m_protocol = new Protocal();

    class Info{
        public int x = 0;
        public int y = 0;
        public int z = 0;
        public boolean update = false;
        public void reset(){
            x = 0;
            y = 0;
            z = 0;
            update = false;
        }
    }
    private Info m_info = new Info();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_buttonConnect = (Button)findViewById(R.id.buttonConnect);
        m_editTextIp = (EditText)findViewById(R.id.editTextIp);
        m_editTextPort = (EditText)findViewById(R.id.editTextPort);
        m_textViewState = (TextView)findViewById(R.id.textViewState);
        m_imageView = (ImageView)findViewById(R.id.imageView);

        m_buttonConnect.setOnClickListener(new ButtonListener());

//        Log.d(TAG, "init ok");

//      for test internet
//      DownloadImageFromInternet downloadThread = new DownloadImageFromInternet();
//      downloadThread.start();
    }
    class ButtonListener implements View.OnClickListener {
        public void onClick(View v) {
//            m_state = "login exception";
//            Message message = Message.obtain();
//            message.what = MESSAGE_TYPE_STATE;
//            m_message_handler.sendMessage(message);
            InitSocketThread socketThead = new InitSocketThread();
            socketThead.start();
        }
    }
    public Handler m_message_handler = new Handler() {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what==MESSAGE_TYPE_STATE){
                m_textViewState.setText(m_state);
            }
            else if(msg.what==MESSAGE_TYPE_DOWNLOAD_IMG){
                if(m_bitmap==null){
                    m_state = "bitmap null";
                    m_textViewState.setText(m_state);
                }else {
                    //m_imageView.setImageBitmap(m_bitmap);
                    m_imageView.setImageBitmap( Bitmap.createScaledBitmap( m_bitmap, m_imageView.getWidth(), m_imageView.getHeight(), false) );
                    m_imageView.setOnTouchListener(new MyOnTouchListener());

                }
            }
        }
    };
    private String readCmdFromSocket(){
        byte[] buf = readByteFromSocket(m_protocol.getCmdLen());
        return m_protocol.decodeCmd(buf);
    }
    private int readIntFromSocket(){
        byte[] buf = readByteFromSocket(m_protocol.getIntLen());
        return m_protocol.decodeInt(buf);
    }
    private byte[] readByteFromSocket(int len){
        byte[] buf = new byte[len];
        int offset = 0;
        while(offset!=len){
            int count = 0;
            try {
                count = socket_input_stream.read(buf, offset, len - offset);
            }catch (IOException ex) {
                ex.printStackTrace();
                ex.printStackTrace();
                m_state += "socket_input_stream exception " + ex.getMessage();
                Message message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
            }
            offset += count;
        }
        return buf;
    }
    private void writeByteIntoSocket(byte[] buf){
        try{
            socket_output_stream.write(buf, 0, buf.length);
        }catch (IOException ex) {
            ex.printStackTrace();
            ex.printStackTrace();
            m_state += "socket_output_stream exception " + ex.getMessage();
            Message message = Message.obtain();
            message.what = MESSAGE_TYPE_STATE;
            m_message_handler.sendMessage(message);
        }
    }
    private void writeXYZInfoSocket(int x, int y, int z){
        writeByteIntoSocket(m_protocol.encodeCmd("xyz"));
        writeByteIntoSocket(m_protocol.encodeInt(3*m_protocol.getIntLen()));
        writeByteIntoSocket(m_protocol.encodeInt(x));
        writeByteIntoSocket(m_protocol.encodeInt(y));
        writeByteIntoSocket(m_protocol.encodeInt(z));
    }

    public static Bitmap rotateBitmap(Bitmap source, float angle)
    {
        Matrix matrix = new Matrix();
        matrix.postRotate(angle);
        return Bitmap.createBitmap(source, 0, 0, source.getWidth(), source.getHeight(), matrix, true);
    }

    class InitSocketThread extends Thread implements Runnable{
        public void run(){
            m_state += "try login ";
            Message message = Message.obtain();
            message.what = MESSAGE_TYPE_STATE;
            m_message_handler.sendMessage(message);
            try {
                m_state = "login...";
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);

                String ip = m_editTextIp.getText().toString();
                int port = Integer.parseInt(m_editTextPort.getText().toString());
                socket = new Socket(ip, port);
                socket_input_stream = socket.getInputStream();
                socket_output_stream = socket.getOutputStream();

                m_state = "login ok";
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
            } catch (IOException ex) {
                ex.printStackTrace();
                ex.printStackTrace();
                m_state += "login exception1 " + ex.getMessage();
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
            } catch (Exception ex){
                ex.printStackTrace();
                m_state += "login exception2 " + ex.getMessage();
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
            }
            if(socket==null){
                m_state = "socket init failed ";
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
//                return;
            }else{
                m_state = "socket init ok ";
                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);
            }

            //write window size
            writeXYZInfoSocket(m_imageView.getHeight()/4, m_imageView.getWidth()/4, 0);


            while(true) {
                String cmd = readCmdFromSocket();
                int len = readIntFromSocket();

                m_state = "";
                m_state += cmd;
                m_state += " " + len;

                byte[] jpg = readByteFromSocket(len);
                m_bitmap = BitmapFactory.decodeByteArray(jpg, 0, jpg.length);
                m_bitmap = rotateBitmap(m_bitmap, 90);

                message = Message.obtain();
                message.what = MESSAGE_TYPE_STATE;
                m_message_handler.sendMessage(message);

                Message message1 = Message.obtain();
                message1.what = MESSAGE_TYPE_DOWNLOAD_IMG;
                m_message_handler.sendMessage(message1);

                if(m_info.update){
                    writeXYZInfoSocket(m_info.x, m_info.y, m_info.z);
                    m_info.reset();
                }else{
                    writeXYZInfoSocket(0, 0, 0);
                }
            }
        }
    }


    class MyOnTouchListener  implements View.OnTouchListener {
        public boolean onTouch(View v, MotionEvent event) {
//            m_state = "touch ";

            switch (event.getAction() & event.ACTION_MASK) {
                case MotionEvent.ACTION_DOWN:
                    touch_mode = TOUCH_MODE_DRAG;
                    touch_x = event.getX();
                    touch_y = event.getY();
//                    m_state += " ACTION_DOWN";
                    break;
                case MotionEvent.ACTION_POINTER_DOWN:
                    touch_mode = TOUCH_MODE_ZOOM;
                    float x = event.getX(0) - event.getX(1);
                    float y = event.getY(0) - event.getY(1);
                    touch_dis = FloatMath.sqrt(x*x + y*y);
                    m_state = "ACTION_POINTER_DOWN: " + touch_dis;
                    break;
                case MotionEvent.ACTION_UP:
                    if(m_info.update==false) {
                        m_info.y = (int) (touch_x - event.getX()) / 4 * -1;
                        m_info.x = (int) (touch_y - event.getY()) / 4;
                        m_info.update = true;
//                    m_state += " ACTION_UP";
                    }
                    break;
                case MotionEvent.ACTION_POINTER_UP:
                    if(m_info.update==false) {
                        x = event.getX(0) - event.getX(1);
                        y = event.getY(0) - event.getY(1);
//                    touch_dis = FloatMath.sqrt(x*x + y*y);
                        m_state = "ACTION_POINTER_UP: " + touch_dis;
                        if (FloatMath.sqrt(x * x + y * y) > touch_dis) {
                            m_info.z = 1;
                        } else {
                            m_info.z = -1;
                        }
                        m_info.update = true;
                    }
                    break;
                case MotionEvent.ACTION_MOVE:
//                    m_state = " ACTION_MOVE " + event.getX() + " " + event.getY();
                    break;
            }

//            m_state += " " + event.getAction() + " " + event.getX() + " " + event.getY();
            Message message = Message.obtain();
            message.what = MESSAGE_TYPE_STATE;
            m_message_handler.sendMessage(message);

            return true;
        }
    }


//    class DownloadImageFromInternet extends Thread implements Runnable{
//        public void run(){
//            try {
//                URL url = new URL("http://pic20.nipic.com/20120427/9550075_110812726000_2.jpg");
//                m_bitmap = BitmapFactory.decodeStream(url.openConnection().getInputStream());
////                m_imageView.setImageBitmap(bmp);
//            } catch (Exception e) {
//                Log.e("Error", e.getMessage());
//                e.printStackTrace();
//                Message message = Message.obtain();
//                message.what = MESSAGE_TYPE_STATE;
//                m_state = "download failed";
//                m_message_handler.sendMessage(message);
//            }
//            Message message = Message.obtain();
//            message.what = MESSAGE_TYPE_DOWNLOAD_IMG;
//            m_state = "download ok";
//            m_message_handler.sendMessage(message);
//        }
//    }


//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        // Inflate the menu; this adds items to the action bar if it is present.
//        getMenuInflater().inflate(R.menu.menu_main, menu);
//        return true;
//    }
//
//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//        // Handle action bar item clicks here. The action bar will
//        // automatically handle clicks on the Home/Up button, so long
//        // as you specify a parent activity in AndroidManifest.xml.
//        int id = item.getItemId();
//
//        //noinspection SimplifiableIfStatement
//        if (id == R.id.action_settings) {
//            return true;
//        }
//
//        return super.onOptionsItemSelected(item);
//    }
}
