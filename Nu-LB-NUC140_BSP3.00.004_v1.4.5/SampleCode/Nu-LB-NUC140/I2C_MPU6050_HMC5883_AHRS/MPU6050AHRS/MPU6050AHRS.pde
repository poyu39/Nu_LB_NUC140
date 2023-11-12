//
// Processign 3.5.4 + toxi library
//
import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;  
import toxi.processing.*;

ToxiclibsSupport gfx;

Serial port;                         // The serial port
char[] teapotPacket = new char[14];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int aligned = 0;
int interval = 0;

float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);

float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];

void setup() {
    // 300px square viewport using P3D rendering
    size(300, 300, P3D); 
    noStroke(); 
    colorMode(RGB, 1);
    
    gfx = new ToxiclibsSupport(this);
  
    // display serial port list for debugging/clarity
    println(Serial.list());

    String portName = Serial.list()[0];
    //String portName = "COM4";
    //String portName = "/dev/ttyUSB1"; // for Linux	
    println("connect to ",portName);

    port = new Serial(this, portName, 115200);
    
    //port.write('r'); // send a character to trigger MPU6050
}

void draw() {
/*
    // resend a character to trigger MPU6050 in the case it halts
    if (millis() - interval > 1000) {
        port.write('r');
        interval = millis();
    }
*/    
    // grey background
    background(0.5); //0 = black
    
    // translate everything to the middle of the viewport
    pushMatrix();
    translate(width / 2, height / 2);

    // 3-step rotation from yaw/pitch/roll angles
    //rotateY(-ypr[0]); // yaw
    //rotateZ(-ypr[1]); // pitch
    //rotateX(-ypr[2]); // roll

    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
    // different coordinate system orientation assumptions between Processing
    // and InvenSense DMP)
    float[] axis = quat.toAxisAngle();
    rotate(axis[0], -axis[1], axis[3], axis[2]);

    scale(50);
    beginShape(QUADS);
    fill(0, 1, 1); vertex(-1,  1,  1);
    fill(1, 1, 1); vertex( 1,  1,  1);
    fill(1, 0, 1); vertex( 1, -1,  1);
    fill(0, 0, 1); vertex(-1, -1,  1);

    fill(1, 1, 1); vertex( 1,  1,  1);
    fill(1, 1, 0); vertex( 1,  1, -1);
    fill(1, 0, 0); vertex( 1, -1, -1);
    fill(1, 0, 1); vertex( 1, -1,  1);

    fill(1, 1, 0); vertex( 1,  1, -1);
    fill(0, 1, 0); vertex(-1,  1, -1);
    fill(0, 0, 0); vertex(-1, -1, -1);
    fill(1, 0, 0); vertex( 1, -1, -1);

    fill(0, 1, 0); vertex(-1,  1, -1);
    fill(0, 1, 1); vertex(-1,  1,  1);
    fill(0, 0, 1); vertex(-1, -1,  1);
    fill(0, 0, 0); vertex(-1, -1, -1);

    fill(0, 1, 0); vertex(-1,  1, -1);
    fill(1, 1, 0); vertex( 1,  1, -1);
    fill(1, 1, 1); vertex( 1,  1,  1);
    fill(0, 1, 1); vertex(-1,  1,  1);

    fill(0, 0, 0); vertex(-1, -1, -1);
    fill(1, 0, 0); vertex( 1, -1, -1);
    fill(1, 0, 1); vertex( 1, -1,  1);
    fill(0, 0, 1); vertex(-1, -1,  1);
    endShape();

    popMatrix();
}

void serialEvent(Serial port) {
    interval = millis();
    while (port.available() > 0) {
        int ch = port.read();
        print((char)ch);
        if (ch == '$') {serialCount = 0;} // this will help with alignment
        if (aligned < 4) {
            // make sure we are properly aligned on a 14-byte packet
            if (serialCount == 0) {
                if (ch == '$') aligned++; else aligned = 0;
            } else if (serialCount == 1) {
                if (ch == 2) aligned++; else aligned = 0;
            } else if (serialCount == 12) {
                if (ch == '\r') aligned++; else aligned = 0;
            } else if (serialCount == 13) {
                if (ch == '\n') aligned++; else aligned = 0;
            }
            //println(ch + " " + aligned + " " + serialCount);
            serialCount++;
            if (serialCount == 14) serialCount = 0;
        } else {
            if (serialCount > 0 || ch == '$') {
                teapotPacket[serialCount++] = (char)ch;
                if (serialCount == 14) {
                    serialCount = 0; // restart packet byte position
                    
                    // get quaternion from data packet
                    q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
                    q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
                    q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
                    q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
                    for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
                    
                    // set our toxilibs quaternion to new data
                    quat.set(q[0], q[1], q[2], q[3]);

                    /*
                    // below calculations unnecessary for orientation only using toxilibs
                    
                    // calculate gravity vector
                    gravity[0] = 2 * (q[1]*q[3] - q[0]*q[2]);
                    gravity[1] = 2 * (q[0]*q[1] + q[2]*q[3]);
                    gravity[2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
        
                    // calculate Euler angles
                    euler[0] = atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1);
                    euler[1] = -asin(2*q[1]*q[3] + 2*q[0]*q[2]);
                    euler[2] = atan2(2*q[2]*q[3] - 2*q[0]*q[1], 2*q[0]*q[0] + 2*q[3]*q[3] - 1);
        
                    // calculate yaw/pitch/roll angles
                    ypr[0] = atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1);
                    ypr[1] = atan(gravity[0] / sqrt(gravity[1]*gravity[1] + gravity[2]*gravity[2]));
                    ypr[2] = atan(gravity[1] / sqrt(gravity[0]*gravity[0] + gravity[2]*gravity[2]));
                    
                    // output various components for debugging
                    //println("q:\t" + round(q[0]*100.0f)/100.0f + "\t" + round(q[1]*100.0f)/100.0f + "\t" + round(q[2]*100.0f)/100.0f + "\t" + round(q[3]*100.0f)/100.0f);
                    //println("euler:\t" + euler[0]*180.0f/PI + "\t" + euler[1]*180.0f/PI + "\t" + euler[2]*180.0f/PI);
                    //println("ypr:\t" + ypr[0]*180.0f/PI + "\t" + ypr[1]*180.0f/PI + "\t" + ypr[2]*180.0f/PI);
                    */
                }
            }
        }
    }
}
