/*sous-vide versao 0.1 - Antonio Celso atractorz@gmail.com
*/

#include  <TimerOne.h>  

#define DEBUG 1

// Mude aqui a temperatura que se queira
const int temp_final = 200;

//coeficiente de calibracao
const float coef_lin=1.3;
; 
const float coef_ang=9.1;



//Config dos pinos
int tempPin = 0;
int AC_pin = 3; 


// variaveis da temperatura

int dif_temp=0;

//variaves do Dimmer
volatile boolean zero_cross=0; 
volatile int i=0;
int dim = 0; // Dimming level (0-128)  0 = on, 128 = 0ff
int inc=1;
int freqStep = 120;  // (120 Hz=8333uS) / 128 brightness steps = 65 uS / brightness step
                    // (100Hz=10000uS) / 128 steps = 75uS/step



void setup()
{
  analogReference(INTERNAL); // tensao de ref = 1.1V
  Serial.begin(9600);
  
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  attachInterrupt(0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep); 

}

void loop(){
  dim=128;
  
  dif_temp=temp_final-temp()+1; //+1 para atingir a temperatura teto
  
  
  if(dif_temp<=20){
    dim=map(dif_temp,0,20,126,0);
  }
  else dim=0;

  if(dif_temp<=0)dim=128;


  
  #if DEBUG
  Serial.print("Dif_temp");
  Serial.println(dif_temp);
  Serial.print("dim");
  Serial.println(dim);
  #endif
  
  delay(10000);
}

int temp(){
  float tempC=0.0;
  int reading=0;
  int read_aux=0;
  int cont=0;
  
  while(cont<10){
    read_aux= analogRead(tempPin);
    
    #if DEBUG
    Serial.println(read_aux);
    #endif
    
    reading+=read_aux;
    cont++;
    delay(100);
  }


  tempC = (reading / (coef_ang*10.0))-coef_lin;

  Serial.print("Temperatura(celsius) - ");
  Serial.println(tempC);
  return int(tempC);

  delay(100);
}

void dim_check() {                   
  if(zero_cross == true) {              
    
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else {
      i++; // increment time step counter                     
    }                                
  }                                  
} 

void zero_cross_detect() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}  

