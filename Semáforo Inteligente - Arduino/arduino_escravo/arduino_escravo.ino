#include <avr/io.h>

uint16_t cont_overflow;
uint8_t cont = 0;
long int x = 0;

void conter_setup(){
  TCCR0B |= 0b00000101; // prescale do registrador contador_overflow (1024)
  TIMSK0 |= 0b00000001;
}

ISR (TIMER0_OVF_vect){
  Serial.begin(9600);
  cont_overflow++;
  if(cont_overflow > 1838/6 && cont == 0){
    if(Serial.available() > 0){
      x = Serial.read();
      if(x == 0){
        cont++;
        cont_overflow = 0;  
      }
    }
  }
  else if(cont_overflow > 1838/6 && cont == 2){
    if(Serial.available() > 0){
      x = Serial.read();
      if(x == 0){
        cont++;
        cont_overflow = 0;  
      } 
    }
  }
  else if(cont_overflow > 1838/30 && (cont == 1 || cont == 3)){
    cont++;
    cont_overflow = 0;  
  }
  else if(cont == 4)
    cont = 0;
}

int main(){
  conter_setup();
  DDRD = 0b11111100;
  sei();
  while(1){
    if(cont == 0){
      PORTD = 0b10000100;
    }
    else if(cont == 1){
      PORTD = 0b01000100;
    }
    else if(cont == 2){
      PORTD = 0b00110000;
    }
    else if(cont == 3){
      PORTD = 0b00101000;
    }
    else
      cont = 0;
  }
}


