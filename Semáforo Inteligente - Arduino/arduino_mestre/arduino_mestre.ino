#include <avr/io.h>

uint16_t cont_overflow;
uint8_t cont = 0;
int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;
long int semaforo1 = 0;
long int semaforo2 = 0;
long int historico1 = 0;
long int historico2 = 0;
long int x = 0;

void troca_setup(){
  TCCR0B |= 0b00000101; // prescale do registrador contador_overflow (1024)
  TIMSK0 |= 0b00000001;
  TCCR3B |= 0b00000001; // prescale do registrador contador_overflow (1024)
  TIMSK3 |= 0b00000001;
}

void fluxo(){
  if(ADMUX == 0b01000000){
    aux1 = aux2;
    aux2 = ADC;
    if((aux2 - aux1) > 300){
      semaforo1++;
      historico1++;
    } 
  }
  if(ADMUX == 0b01000011){
    aux3 = aux4;
    aux4 = ADC;
    if((aux4 - aux3) > 300){
      semaforo2++;
      historico2++;
    }
  }
  if(cont_overflow < 1838/6 && (cont != 1 || cont != 3))   
    x = (semaforo1 - semaforo2)*100;     
}

ISR (TIMER0_OVF_vect){
  cont_overflow++;
  if(cont_overflow > 1838/6 && cont == 0){
    if(x > 0){
      x--;  
    }
    else{    
      cont++;
      cont_overflow = 0;
    }
  }
  else if(cont_overflow > 1838/6 && cont == 2){
    if(x < 0){
      x++;
    }
    else{
      cont++;
      cont_overflow = 0;
    }   
  }
  else if(cont_overflow > 1838/30 && (cont == 1 || cont == 3)){
    cont++;
    cont_overflow = 0;  
    //semaforo1 = 0;
    //semaforo2 = 0;
  }
  else if(cont == 4)
    cont = 0;
}

ISR (TIMER3_OVF_vect){
  switch(ADMUX){
  case 0b01000000:
    ADMUX = 0b01000011;
    ADCSRA |= 0b11000111;
    break;
  case 0b01000011:
    ADMUX = 0b01000000;
    ADCSRA |= 0b11000111;
    break;
  } 
}

int main(){
  Serial.begin(9600);
  Serial1.begin(9600);
  troca_setup();  
  sei();
  ADMUX = 0b01000000;
  ADCSRA = 0b00010111;
  while(1){
    ADCSRA |= (1<<ADSC); //iniciar conversor de luminosidade, seta diretamento o ADC
    while (!(ADCSRA & 0b00010111));
    fluxo();
    Serial.print("(semaforo1 - semaforo2) = ");
    Serial.print(x);
    Serial1.print(x);
    Serial.print("\t semaforo1 = ");
    Serial.print(semaforo1);
    Serial.print("\t semaforo2 = ");
    Serial.print(semaforo2);
    Serial.print("\t ADC = ");
    Serial.println(ADC);
  }
}










