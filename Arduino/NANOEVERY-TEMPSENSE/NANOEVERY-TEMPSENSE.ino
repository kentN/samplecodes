/*
  Arduino Nano Every Internal temperature sensor test

  Board:   Arduino Nano Every (Arduino MegaAVR boards)   
  config:  Registers emulation: none (4809 mode)
  Visualiser : Serial plotter 

  code Refelence1: megaAVR-0-series-Family-Data-Sheet 28.3.2.6
  code Refelence2: wiring_analog.c (Arduino Core-megaAVR)

  Kent Nakazawa (kent_n@outlook.com)
*/

//#define DEBUG  //show kelvin temperature & sigrow values

void setup() {
  analogReference(INTERNAL1V1);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("new AVR Internal Temperature Sensor readout");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);  
  int16_t temperature = (internal_temperature_K() - 273);
  Serial.print(temperature);
  Serial.println(" C");
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

uint16_t internal_temperature_K()
{
  int8_t sigrow_offset = SIGROW.TEMPSENSE1;
  uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
  uint16_t adcread = 0;
  
  adcread = ADCRead(0x1E);
  uint32_t temp = adcread;
  temp -= sigrow_offset;
  temp *= sigrow_gain;
  temp += 0x80;
  temp >>= 8;
  uint16_t temperature_K = temp;

  #ifdef DEBUG
  Serial.print("DEBUG: ");
  Serial.print(temperature_K); 
  Serial.print("k, offset :");
  Serial.print(sigrow_offset);
  Serial.print(" gain :");
  Serial.println(sigrow_gain);
  #endif

  return (temperature_K);
}

uint16_t ADCRead(uint8_t mux)
{
  uint8_t low, high;
  /* Reference should be already set up */
  /* Select channel */
  ADC0.MUXPOS = (mux << ADC_MUXPOS_gp);
  /* Start conversion */
  ADC0.COMMAND = ADC_STCONV_bm;
  /* Wait for result ready */
  while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
  /* Save state */
  uint8_t status = SREG;
  cli();
  /* Read result */
  low = ADC0.RESL;
  high = ADC0.RESH;
  /* Restore state */
  SREG = status;
  /* Combine two bytes */
  return (high << 8) | low;
}
