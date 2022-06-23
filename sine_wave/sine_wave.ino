#include <MCP47CXBXX.h>

MCP47CXBXX_DAC dac;
#define DAC_RES 10
//I2C address based on pin 2 (A0) and pin 8 (A1)
#define DAC_ADR_A0  0
#define DAC_ADR_A1  0
#define DAC_ADR (0b01100000)| DAC_ADR_A1<<1 | DAC_ADR_A0 

uint16_t bits_returned = 0;
uint16_t dac0_value = 0;
uint16_t dac1_value = 0;

//sine wave array
//uint16_t sine_wave[] = {0, 1, 2, 3, 4};
uint16_t sine_wave[] = {512,524,537,550,563,576,588,601,614,626,639,651,663,676,688,700,
712,723,735,747,758,769,780,791,802,812,822,833,842,852,862,871,880,889,897,906,914,922,
929,936,943,950,957,963,969,974,980,985,989,994,998,1002,1005,1008,1011,1014,1016,1018,
1020,1021,1022,1023,1023,1023,1023,1022,1021,1020,1018,1016,1014,1011,1008,1005,1002,998,
994,989,985,980,974,969,963,957,950,943,936,929,922,914,906,897,889,880,871,862,852,842,
833,822,812,802,791,780,769,758,747,735,723,712,700,688,676,663,651,639,626,614,601,588,
576,563,550,537,524,512,499,486,473,460,447,435,422,409,397,384,372,360,347,335,323,311,
300,288,276,265,254,243,232,221,211,201,190,181,171,161,152,143,134,126,117,109,101,94,
87,80,73,66,60,54,49,43,38,34,29,25,21,18,15,12,9,7,5,3,2,1,0,0,0,0,1,2,3,5,7,9,12,15,
18,21,25,29,34,38,43,49,54,60,66,73,80,87,94,101,109,117,126,134,143,152,161,171,181,190,
201,211,221,232,243,254,265,276,288,300,311,323,335,347,360,372,384,397,409,422,435,447,
460,473,486,499,512};

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A0));
  Serial.begin(115200);
  Serial.print("DAC Adrs: 0x");
  Serial.println(DAC_ADR, HEX);

  if(dac.begin(10, 2, DAC_ADR) == 1)
    Serial.println("DAC initiated sucessfully");
  else
    Serial.println("DAc not initialized");

  //dac.Reset();
  dac.setVref(DAC_VREF_INTERNAL, DAC_VREF_INTERNAL);
  //dac.setGain(DAC_GAIN_1X, DAC_GAIN_1X);    //Not needed since the DAC defaults to gain of 1
  Serial.print("DAC resolution is ");
  Serial.println(DAC_RES);

  dac0_value = 30;
  dac1_value = 1000;

  Serial.println(dac.setOutput(0, dac0_value));
  dac.setOutput(1, dac1_value);

  dac.readOutput(0, &bits_returned);
  Serial.println("");
  Serial.println(random(300));
  Serial.print("Sent dac0 value: ");
  Serial.println(dac0_value);
  Serial.print("Returned dac0 value: ");
  Serial.println(bits_returned);

  dac.readOutput(1, &bits_returned);
  Serial.print("Sent dac1 value: ");
  Serial.println(dac1_value);
  Serial.print("Returned dac1 value: ");
  Serial.println(bits_returned);

  Serial.print("size of sine array: ");
  Serial.println(sizeof(sine_wave ));
  Serial.print("size of a uint16_t: ");
  Serial.println(sizeof(uint16_t));

  //while(1){  }
}

void loop() {
  // put your main code here, to run repeatedly:

  for(uint16_t i = 0; i < (sizeof(sine_wave)/sizeof(uint16_t)); i++)
  {
    dac.setOutput(0, sine_wave[i]);
  }
}
