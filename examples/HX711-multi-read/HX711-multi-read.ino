//==========================================================
//Shmulik Edelman
//shmulike@post.bgu.ac.il
// This code reading form MULTI HX711 Load-cells and sending there value as ROS-message
//==========================================================

//=====[ INCULDE ]==========================================

#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include "HX711-multi.h"

//=====[ Constants ]========================================
#define PRINT 1
#define N_joints 4
#define CLK 11      // clock pin to the load cell amp
#define DOUT1 A0    // data pin to the first lca
#define DOUT2 A1    // data pin to the second lca
#define DOUT3 A2    // data pin to the third lca
#define DOUT4 A3    // data pin to the third lca
#define DOUT5 A4    // data pin to the third lca
#define DOUT6 A5    // data pin to the third lca
#define DOUT7 A6    // data pin to the third lca
#define DOUT8 A7    // data pin to the third lca
#define BOOT_MESSAGE "MIT_ML_SCALE V0.8"
#define d_count 500
//#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

#define N_tensions 8
long count = 0;
unsigned long startTime, curTime, startTime2;
float curTimeSec, curTimeMin;

//=====[ VARIABLES ]=========================================
ros::NodeHandle nh;
byte DOUTS[N_tensions] = {DOUT1, DOUT2, DOUT3, DOUT4, DOUT5, DOUT6, DOUT7, DOUT8};
int tension_order[N_tensions] = {1, 2, 3, 4, 5, 6, 7, 8};

long int result[N_tensions];
float arr[N_tensions]={0};
HX711MULTI scales(N_joints*2, DOUTS, CLK);

std_msgs::Float32MultiArray tension_val;
ros::Publisher pub_tensions("/robot_snake_1/tension_val", &tension_val);

   double P[N_tensions][2] = {{0.00000000, 0.00002348},
                              {0.00000000, 0.00002385},
                              {0.00000000, 0.00002414},
                              {0.00000000, 0.00002028},
                              {0.00000000, 0.00002322},
                              {0.00000000, 0.00002559},
                              {0.00000000, 0.00002408},
                              {0.00000000, 0.00002436}};
                     
//=====[ SETUP ]=============================================
void setup() {
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.advertise(pub_tensions);
  tension_val.data_length = N_tensions;
  if (PRINT){
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Serial Communication started");
  }
  scales.setTimeOut(300);
  
  startTime = millis();
  startTime2 = millis();
}

//=====[ LOOP ]==============================================
void loop() {
  
  
  scales.readRaw(result);                // Read raw data from all the tension-meters
    
  // ---------- Convert RAW data to Kg
  for (int i=0; i<(N_joints*2); i++){
    arr[i] = P[i][0]*pow(result[i],2) + P[i][1]*result[i];
    //Serial.print(arr[i],3);
    //Serial.print("\t");
  }

  
  // PRINT ALL VALUES
  if (PRINT){
    count++;
    //Serial.print(count);
    
    if (count%d_count==0){
        curTime = (millis()-startTime);
    curTimeSec = (curTime/1000)%60;
    curTimeMin = (int)(curTime/60000);
  
  
    Serial.print(curTimeMin,0);
    Serial.print(":");
    Serial.print(curTimeSec,0);
    Serial.print("\t");
    Serial.print(d_count*1000.0/(millis()-startTime2));
    Serial.print("Hz");
    
    startTime2=millis();
    
    Serial.print("\t");
    
    for (int i=0; i<(N_joints*2); i++){
      //Serial.print(result[k]);
      //Serial.print("Joint ");
      Serial.print(arr[i],4);
      Serial.print(" ; ");
    }
    Serial.print('\n');
    }
    
  }

  // Send ROS message
  tension_val.data = arr;
  pub_tensions.publish(&tension_val);
  nh.spinOnce();
  delay(1);
}
