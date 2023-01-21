//Written for the Teensy 3.5

//Allow optimized interrupts, do not use attachIntterrupt to avoid conflict
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define encoder_0a 18
#define encoder_0b 19
#define encoder_1a 20
#define encoder_1b 21
#define encoder_2a 22
#define encoder_2b 23

Encoder enc0( encoder_0a, encoder_0b );
Encoder enc1( encoder_1a, encoder_1b );
Encoder enc2( encoder_2a, encoder_2b );
Encoder enc_array[]{ enc0, enc1, enc2 };

long axis_current[3];
long axis_new[3];
bool position_updated;

void updatePoll(){
  position_updated = 0;
  for( int i=0; i<3; i++ ){
    axis_new[i] = enc_array[i].read();
    if( axis_new[i] != axis_current[i] ){
      axis_current[i] = axis_new[i];
      position_updated = 1;
    }
  }
  if( position_updated ){ printAxes(); }
}

void printAxes(){
  for( int i=0; i<3; i++ ){
    Serial.print( axis_current[i] );
    Serial.print( ", " );
  }
  Serial.print("\n");
}

void setup(){
  Serial.begin( 115200 );
}

void loop(){
  updatePoll();
  
}
