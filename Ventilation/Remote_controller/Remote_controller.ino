#define CLK 2
#define DT 3
#define SW 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1


//Biblio oled display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Biblio encoder
#include <GyverEncoder.h>



Encoder enc(CLK, DT, SW);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(9, 10);

uint8_t pipe;
int server_data[3], remote_data[3];
int sp = 0, connect_in_server=0, cs=0;
String current_sstatus = "None",current_rstatus = "", choose_speed = "OFF", mode;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  remote_data[0]=0;
  remote_data[1]=0;
  remote_data[2]=0;
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.display();
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, 111156789);
  radio.startListening();
  enc.setType(TYPE2);
  attachInterrupt(0, isr, CHANGE);
  attachInterrupt(1, isr, CHANGE);
  connect_server();
}

void isr(){
  enc.tick();
  if (enc.isTurn() and remote_data[0] == 1) {
    if (enc.isRight()) sp++;
    if (enc.isLeft()) sp--;
    check_sp();
  }
}

void connect_server() {
  while (connect_in_server == 0) {
    display.clearDisplay();
    display.display();
    delay(500);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,12);
    display.println("Connect to server");
    display.display();
    delay(500);
    if (radio.available(&pipe)) {
        radio.read(&server_data, sizeof(server_data));
        remote_data[2]=1;
        remote_data[0]=server_data[0];
        remote_data[1]=server_data[1];
        connect_in_server=1;
        cs=0;      
    }
  }
  print_display();
}

void print_display() {
  enc.tick();
//---------------------------------  
  switch (server_data[0]){
    case 0:
      mode="Automatic";
      break;
    case 1:
      mode="User";
      break;
  }
//---------------------------------
  switch (server_data[1]){
    case 0:
      current_sstatus="OFF";
      break;
    case 1:
      current_sstatus="Speed 1";
      break;
    case 2:
      current_sstatus="Speed 2";
      break;    
    case 3:
      current_sstatus="Speed 3";
      break;
    case 4:
      current_sstatus="Speed 4";
      break;
    case 5:
      current_sstatus="Speed MAX";
      break;
    case 6:
      current_sstatus="ERROR";
      break;
    case 7:
      current_sstatus="Waiting server";
      break;
  }
//---------------------------------
  switch (sp){
    case 0:
      current_rstatus="OFF";
      break;
    case 1:
      current_rstatus="Speed 1";
      break;
    case 2:
      current_rstatus="Speed 2";
      break;    
    case 3:
      current_rstatus="Speed 3";
      break;
    case 4:
      current_rstatus="Speed 4";
      break;
    case 5:
      current_rstatus="Speed MAX";
      break;
  }
//---------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Mode:" + mode);
  if (connect_in_server == 1) {
    display.setCursor(92, 0);
    display.print("online");
  } else {
    display.setCursor(92, 0);
    display.print("ofline");
  }
  display.setCursor(1, 13);
  display.print("Status:" + current_sstatus);
  if (remote_data[0]==1){
    display.setCursor(1, 25);
    display.print("Chose mode:" + current_rstatus);
  }
  display.display();
//  delay(100);
//  enc.tick();
}

void check_sp() {
  if (sp <= 0) {
    sp = 0;
    return;
  }
  if (sp >= 5) {
    sp = 5;
    return;
  }
}

void user_mode(){
  cs=0;
  while (remote_data[0]==1 or remote_data[0]!=server_data[0]) {
    enc.tick();
    delay(300);
    if (remote_data[0]==1 or remote_data[0]!=server_data[0] or remote_data[2]!=server_data[2]){
      radio.stopListening();
      radio.openWritingPipe(0xF0F1F2F322LL);
      radio.write(&remote_data, sizeof(remote_data));
      radio.startListening();
    }
//    if (enc.isTurn() and remote_data[0] == 1) {
//      if (enc.isRight()) sp++;
//      if (enc.isLeft()) sp--;
//      check_sp();
//    }
//    
    if (enc.isClick() and remote_data[0] == 1) {remote_data[1]=sp;}
    
    if (enc.isHolded()) {
      if (remote_data[0]==0){remote_data[0]=1;}
      else {remote_data[0]=0;}
      delay(300);
    }
    if (radio.available(&pipe)) {
      radio.read(&server_data, sizeof(server_data));
      cs=0;
      }
    print_display();
    cs++;
    if (cs>100) {break;}
  }
  connect_in_server=0;
  connect_server();
  cs=0;
  return;
}


void loop() {
  enc.tick();
  if (remote_data[0]==1 and remote_data[2]==server_data[2]){user_mode();}
  if (remote_data[2]!=server_data[2]){
    Serial.println("se");
    Serial.println(remote_data[0]);
    Serial.println(remote_data[1]);
    Serial.println(remote_data[2]);
    radio.stopListening();
    radio.openWritingPipe(0xF0F1F2F322LL);
    radio.write(&remote_data, sizeof(remote_data));
    radio.startListening();
  }


    
  if (enc.isHolded()) {
    if (remote_data[0]==0){remote_data[0]=1; user_mode();}
    else {remote_data[0]=0;
    }
  }


  if (radio.available(&pipe)) {
    radio.read(&server_data, sizeof(server_data));
    cs=0;
//    Serial.println("ls");
//    Serial.println(server_data[0]);
//    Serial.println(server_data[1]);
//    Serial.println(server_data[2]);}
  }
  print_display();
  cs++;
  if (cs>300) {connect_in_server=0; connect_server();}
  
}
