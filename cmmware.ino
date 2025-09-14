//Written for the Teensy 3.5

//Allow optimized interrupts, do not use attachIntterrupt to avoid conflict
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <usb_keyboard.h>

#define encoder_0a 18
#define encoder_0b 19
#define encoder_0z 17
#define encoder_1a 20
#define encoder_1b 21
#define encoder_2a 22
#define encoder_2b 23

#define probe 38
#define probe_led 37
#define beep 24

Encoder enc0( encoder_0a, encoder_0b );
Encoder enc1( encoder_1a, encoder_1b );
Encoder enc2( encoder_2a, encoder_2b );
Encoder * enc_array[]{ &enc0, &enc1, &enc2 };

long axis_current[3];
long axis_new[3];
long axis_probe[3];
bool position_updated;
unsigned long interval = 0;
unsigned long hb = 0;
bool hb_state = 1;

unsigned long last_probe = 0;
unsigned long last_release = 0;

bool dro_mode = 0;
unsigned long last_dro = 0;
enum keyboard_modes {
  rhino_mode,
  excel_mode,
  none
};
enum keyboard_modes keyboard_mode = none;

void updatePoll(){
  if( millis() - last_dro > 100 ){
    position_updated = 0;
    for( int i=0; i<3; i++ ){
      axis_new[i] = enc_array[i]->read();
      if( axis_new[i] != axis_current[i] ){
        axis_current[i] = axis_new[i];
        position_updated = 1;
      }
    }
    if( position_updated ){ print_dro(); }
    last_dro = millis();
  }
}

void print_dro(){
  Serial.print( "\r\33[2K\r" );
  Serial.printf( "%f,%f,%f\r\n", axis_current[0]/1000.0, axis_current[1]/1000.0, axis_current[2]/100.0 );
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
      case 63:
        printCommands();
      break;
      case 100:         //char d
        dro_mode = !dro_mode;
      break;
      case 101:         //char e
        keyboard_mode = excel_mode;
        Serial.println( "Mode: Excel");
      break;
      case 110:         //char n
        keyboard_mode = none;
        Serial.println( "Mode: None" );
      break;
      case 112:         //char p
        print_dro();
      break;
      case 114:         //char r
        keyboard_mode = rhino_mode;
        Serial.println( "Mode: Rhino" );
      break;
      case 120:         // char x
        enc0.write( enc0.read() - axis_probe[0] );
        Serial.println( "X Axis Zeroed" );
      break;
      case 121:         // char y
        enc1.write( enc1.read() - axis_probe[1] );
        Serial.println( "Y Axis Zeroed" );
      break;
      case 122:         // char z
        enc2.write( enc2.read() - axis_probe[2] );
        Serial.println( "Z Axis Zeroed" );
      break;
      default:
      break;
    }
  }
}

void probe_interrupt(){
  for( int i=0; i<3; i++ ){
    axis_probe[i] = enc_array[i]->read();
  }
}

void handle_probe(){  
  //Debounce input and handle printing
  if( !digitalRead(probe) && millis() - last_release > 100 ){ 
    for( int i=0; i<3; i++ ){
      axis_probe[i] = enc_array[i]->read();
    }
    tone( beep, 3000, 100 );
    Serial.printf( "* %f,%f,%f\r\n", axis_probe[0]/1000.0, axis_probe[1]/1000.0, axis_probe[2]/100.0 );
    if( keyboard_mode == rhino_mode ){
      Keyboard.printf( "point %f,%f,%f ", axis_probe[0]/1000.0, axis_probe[1]/1000.0, axis_probe[2]/100.0 );
    }
    if( keyboard_mode == excel_mode ){
        Keyboard.printf( "%f\t%f\t%f\r\n", axis_probe[0]/1000.0, axis_probe[1]/1000.0, axis_probe[2]/100.0 );
    }
  }
  if( digitalRead(probe) && millis() - last_release > 100 ){ 
    tone( beep, 2000, 100 );
    last_touch = millis();
  }
  if( !digitalRead(probe) ){ 
    last_release = millis();
    digitalWrite( probe_led, 1 );
  }
  else { 
    digitalWrite( probe_led, 0 ); 
    last_touch = millis();
  }

}

void printCommands(){
  Serial.println( "Commands:");
  Serial.println( "p - Print current position");
  Serial.println( "d - Toggle DRO Mode");
  Serial.println( "x / y / z - Zero named axis" );
  Serial.println( "r - Rhino mode" );
  Serial.println( "e - Excel mode" );
  Serial.println( "n - Cancel keyboard mode" );
}

void setup(){
  Serial.begin( 115200 );
  Serial.println( "ONLINE" );
  printCommands();  
  pinMode( 13, OUTPUT );
  pinMode( probe, INPUT );
  pinMode( probe_led, OUTPUT );
  pinMode( beep, OUTPUT);
}

void loop(){
  if( dro_mode ){ updatePoll(); }
  handle_serial();
  handle_probe();

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
