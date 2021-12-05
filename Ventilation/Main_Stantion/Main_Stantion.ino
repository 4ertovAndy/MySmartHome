#include <SPI.h>
#include <nRF24L01.h>                                     
#include <RF24.h>        
                                 
RF24 radio(9,10);                               
int       data[3],remote_data[3],server_data[3];
uint8_t   pipe;  
int in0=3,in1=8,in2=7,in3=6,in4=5,in5=4,s1=0,s2=0,onv,offv,ksv,sv1,sv2,sv3,sv4;

void setup(){
    server_data[0]=0;
    server_data[1]=0;
    server_data[2]=0;
    remote_data[0]=0;
    remote_data[1]=0;
    remote_data[2]=0;
    pinMode(3, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(in5, OUTPUT);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(in5, LOW);
    //Serial.begin(9600);
    radio.begin();                                        
    radio.setChannel(5);                                  
    radio.setDataRate     (RF24_250KBPS);                
    radio.setPALevel      (RF24_PA_MAX);                 
    radio.openReadingPipe (1,0xF0F1F2F3F4LL);            
    radio.openReadingPipe (2,0xF0F1F2F311LL); //badroomSensor
    radio.openReadingPipe (3,0xF0F1F2F322LL); //remoteController
    radio.startListening  ();                                                          
}

void blink_diod_server(){
    delay(150);
    digitalWrite(3, HIGH);
    delay(150);
    digitalWrite(3, LOW);
}

void blink_diod_user(){
    delay(50);
    digitalWrite(3, HIGH);
    delay(50);
    digitalWrite(3, LOW);
}

void remote_signal(){
    radio.stopListening();
    radio.openWritingPipe(111156789);
    radio.write(&server_data, sizeof(server_data));
    radio.startListening();
    //delay(500);
}

void data_all_control(){
    while (s1==0 || s2==0){
        blink_diod_server();
        //Serial.print("s1="+String(s1));
        //Serial.println("s2="+String(s2));
        if(radio.available(&pipe)){                                
            radio.read(&data, sizeof(data));                  
            if(pipe==1){s1=data[0];}
            if(pipe==2){s2=data[0];}
            if(pipe==3){
                remote_data[0]=data[0];
                remote_data[1]=data[1];
                server_data[2]=data[2];
            }
        }
        if (remote_data[0]==1){user_mode();}
        //remote_signal();
    }
}

void set_speed(int speed){
    switch(speed){
        case 0: // Off vent
            digitalWrite(in1, LOW);
            digitalWrite(in2, LOW);
            digitalWrite(in3, LOW);
            digitalWrite(in4, LOW);
            digitalWrite(in5, LOW);
            break;
        case 1: // min speed
            digitalWrite(in2, LOW);
            digitalWrite(in3, LOW);
            digitalWrite(in4, LOW);
            digitalWrite(in5, LOW);
            delay(500);
            digitalWrite(in1, HIGH);
            break;
        case 2: // 1 speed
            digitalWrite(in1, LOW);
            digitalWrite(in3, LOW);
            digitalWrite(in4, LOW);
            digitalWrite(in5, LOW);
            delay(500);
            digitalWrite(in2, HIGH);
            break;    
        case 3: // 2 speed
            digitalWrite(in1, LOW);
            digitalWrite(in2, LOW);
            digitalWrite(in4, LOW);
            digitalWrite(in5, LOW);
            delay(500);
            digitalWrite(in3, HIGH);
            break;
        case 4: // 3 speed
            digitalWrite(in1, LOW);
            digitalWrite(in2, LOW);
            digitalWrite(in3, LOW);
            digitalWrite(in5, LOW);
            delay(500);
            digitalWrite(in4, HIGH);
            break;
        case 5: // max speed
            digitalWrite(in1, LOW);
            digitalWrite(in2, LOW);
            digitalWrite(in3, LOW);
            digitalWrite(in4, LOW);
            delay(500);
            digitalWrite(in5, HIGH);
            break;
    }
}

void user_mode(){
    while (remote_data[0]==1){
        blink_diod_user();
        //Serial.print("1");
        if(radio.available(&pipe)){                                
            radio.read(&data, sizeof(data));                  
            if(pipe==3){
                remote_data[0]=data[0];
                remote_data[1]=data[1];
                remote_data[2]=data[2];
                server_data[0]=remote_data[0];
                if (remote_data[1]==0){
                    set_speed(0);
                    server_data[1]=0;
                }
                if (remote_data[1]==1){
                    set_speed(1);
                    server_data[1]=1;
                }
                if (remote_data[1]==2){
                    set_speed(2);
                    server_data[1]=2;
                }
                if (remote_data[1]==3){
                    set_speed(3);
                    server_data[1]=3;
                }
                if (remote_data[1]==4){
                    set_speed(4);
                    server_data[1]=4;
                }
                if (remote_data[1]==5){
                    set_speed(5);
                    server_data[1]=5;
                }
                delay(500);
                
            }
        }
        remote_signal();
    }
    set_speed(0);
    server_data[1]=0;
}

void loop(){
    data_all_control();
    onv=s1+10;
    offv=s1+5;
    ksv=int((100-onv)/4);
    sv1=int(onv+ksv);
    sv2=int(onv+ksv*2);
    sv3=int(onv+ksv*3);
    if (s2<offv){
        set_speed(0);
        server_data[1]=0;
    }
    if (s2>onv and s2<sv1){
        set_speed(2);
        server_data[1]=2;
    }
    if (s2>sv1 and s2<sv2){
        set_speed(3);
        server_data[1]=3;
    }
    if (s2>sv2 and s2<sv3){
        set_speed(4);
        server_data[1]=4;
    }
    if (s2>sv3){
        set_speed(5);
        server_data[1]=5;
    }
    //Serial.print("Speed server mod:");
    //Serial.println(server_data[1]);   
    delay(500);
    remote_signal();
    s1=0;
    s2=0;
    
}
