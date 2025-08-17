//Written for the Teensy 3.5

//Allow optimized interrupts, do not use attachIntterrupt to avoid conflict
//#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define encoder_0a 18
#define encoder_0b 19
#define encoder_0z 17
#define encoder_1a 20
#define encoder_1b 21
#define encoder_2a 22
#define encoder_2b 23

Encoder enc0( encoder_0a, encoder_0b );
Encoder enc1( encoder_1a, encoder_1b );
Encoder enc2( encoder_2a, encoder_2b );
Encoder * enc_array[]{ &enc0, &enc1, &enc2 };

long axis_current[3];
long axis_new[3];
bool position_updated;
unsigned long interval = 0;
unsigned long hb = 0;
bool hb_state = 1;

void updatePoll(){
  position_updated = 0;
  for( int i=0; i<3; i++ ){
    axis_new[i] = enc_array[i]->read();
    if( axis_new[i] != axis_current[i] ){
      axis_current[i] = axis_new[i];
      position_updated = 1;
    }
  }
  if( position_updated ){ printAxes(); }
  if( digitalRead(encoder_0z) ){ 
      Serial.print( "Z" );
      Serial.println( millis() ); 
  }
}

void printAxes(){
  //Serial.print( "\r\33[2K\r" );
  for( int i=0; i<3; i++ ){
    Serial.print( axis_current[i] );
    Serial.print( ", " );
  }
  Serial.print("\n");
}

void heartbeat(){
  if( millis() - hb > 500 ){
    digitalWrite( 13, hb_state );
    hb_state = !hb_state;
    hb = millis();
  }
}

void handle_serial(){
  if( Serial.available() ){
    char recvd = Serial.read();
    switch( recvd ){
      case 120:         // char x
        enc0.write( 0 );
        Serial.println( "X Axis Zeroed" );
      break;
      case 121:         // char y
        enc1.write( 0 );
        Serial.println( "Y Axis Zeroed" );
      break;
      case 122:         // char z
        enc2.write( 0 );
        Serial.println( "X Axis Zeroed" );
      break;
      default:
      break;
    }
  }
}

void setup(){
  Serial.begin( 115200 );
  Serial.println( "ONLINE" );
  pinMode( 13, OUTPUT );
}

void loop(){
  updatePoll();
  handle_serial();
  
  if( millis() - interval > 100 ){
    //printAxes();
    //Serial.println( enc1.read() );
    //Serial.print( digitalRead(encoder_1a) );
    //Serial.print(", ");
    //Serial.println( digitalRead(encoder_1b) );
    interval = millis();
  } 

  heartbeat();
}
