#include <SPI.h>
#include <nRF24L01.h>                                     
#include <RF24.h>        
                                 
RF24 radio(9,10);                               
int       data[3],remote_data[3],server_data[3];
uint8_t   pipe;  
int in0=3,in1=4,in2=5,in3=6,in4=7,in5=8,s1=0,s2=0,onv,offv,ksv,sv1,sv2,sv3,sv4;

void setup(){
    server_data[0]=0;
    server_data[1]=0;
    server_data[2]=0;
    remote_data[0]=0;
    remote_data[1]=0;
    remote_data[2]=0;
    pinMode(in0, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(in5, OUTPUT);
    digitalWrite(in0, HIGH);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(in5, LOW);
    Serial.begin(9600);
    radio.begin();                                        
    radio.setChannel(5);                                  
    radio.setDataRate     (RF24_250KBPS);                
    radio.setPALevel      (RF24_PA_MAX);                 
    radio.openReadingPipe (1,0xF0F1F2F3F4LL);            
    radio.openReadingPipe (2,0xF0F1F2F311LL); //badroomSensor
    radio.openReadingPipe (3,0xF0F1F2F322LL); //remoteController
    radio.startListening  ();                                                          
}

void remote_signal(){
  radio.stopListening();
  radio.openWritingPipe(111156789);
  radio.write(&server_data, sizeof(server_data));
  radio.startListening();
  delay(500);
}

void data_all_control(){
  while (s1==0 || s2==0){
    Serial.print("s1="+String(s1));
    Serial.println("s2="+String(s2));
    if(radio.available(&pipe)){                                
      radio.read(&data, sizeof(data));                  
      if(pipe==1){s1=data[0];}
      if(pipe==2){s2=data[0];}
      if(pipe==3){remote_data[0]=data[0];remote_data[1]=data[1];server_data[2]=data[2];}
    }
    delay(500);
    if (remote_data[0]==1){user_mode();}
    remote_signal();
  }
}

void user_mode(){
  while (remote_data[0]==1){
    Serial.print("1");
    if(radio.available(&pipe)){                                
      radio.read(&data, sizeof(data));                  
      if(pipe==3){remote_data[0]=data[0];remote_data[1]=data[1];remote_data[2]=data[2];}
      
      if (remote_data[1]==0){
        digitalWrite(in0, HIGH);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        digitalWrite(in5, LOW);
        server_data[1]=0;
      }

      if (remote_data[1]==5){
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        digitalWrite(in5, LOW);
        digitalWrite(in0, LOW);
        delay(500);
        digitalWrite(in1, HIGH);
        server_data[1]=5;
      }

      if (remote_data[1]==4){
        digitalWrite(in1, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        digitalWrite(in5, LOW);
        delay(500);
        digitalWrite(in0, LOW);
        digitalWrite(in2, HIGH);
        server_data[1]=4;
      }

      if (remote_data[1]==3){
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in4, LOW);
        digitalWrite(in5, LOW);
        delay(500);
        digitalWrite(in0, LOW);
        digitalWrite(in3, HIGH);
        server_data[1]=3;
      }

      if (remote_data[1]==2){
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in5, LOW);
        delay(500);
        digitalWrite(in0, LOW);
        digitalWrite(in4, HIGH);
        server_data[1]=2;
      }

      if (remote_data[1]==1){
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        delay(500);
        digitalWrite(in0, LOW);
        digitalWrite(in5, HIGH);
        server_data[1]=1;
      }
      server_data[0]=remote_data[0];
      delay(1000);
      remote_signal();
    } 
  }
  digitalWrite(in0, HIGH);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in5, LOW);
}

void loop(){
    data_all_control();
    onv=s1+10;
    offv=s1+5;
    ksv=int((100-onv)/4);
    sv1=int(onv+ksv);
    sv2=int(onv+ksv*2);
    sv3=int(onv+ksv*3);

// Off ventelator
    if (s2<offv){
      digitalWrite(in0, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      digitalWrite(in5, LOW);
      server_data[1]=0;
    }

// 1(2) speed ventelator
    if (s2>onv and s2<sv1){
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      delay(500);
      digitalWrite(in0, LOW);
      digitalWrite(in4, HIGH);
      server_data[1]=2;
    }

// 3 speed ventelstor
      
    if (s2>sv1 and s2<sv2){
      server_data[1]=3;
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in4, LOW);
      delay(500);
      digitalWrite(in0, LOW);
      digitalWrite(in3,HIGH);
    }

// 4 speed ventelator
      
    if (s2>sv2 and s2<sv3){
      server_data[1]=4;
      digitalWrite(in1, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      delay(500);
      digitalWrite(in0, LOW);
      digitalWrite(in2,HIGH);
    }

// max speed ventelator
      
    if (s2>sv3){
      server_data[1]=5;
      digitalWrite(in0, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      delay(500);
      digitalWrite(in1,HIGH);
   }
  Serial.print("Speed server mod:");
  Serial.println(server_data[1]);   
  s1=0;
  s2=0;
  delay(1000);
}
