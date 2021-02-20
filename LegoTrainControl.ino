#include <PowerFunctions.h>
 
#define IR_TRANS_IN   2  //IR Trans PIN
#define IR_DEBUG_OFF  0  //IR Debug Mode Off
#define IR_DEBUG_ON   1  //IR Debug Mode On
 
//IR Channels
#define CH1 0x0
#define CH2 0x1
#define CH3 0x2
#define CH4 0x3
 
#define TRIGGER_PIN   8  // Trig pin
#define ECHO_PIN      9  // Echo pin
#define NEAREST_VALUE 50 // Maximun Speed Distance(mm)
#define OFFSET        60 // Distance(mm) Between Speed
 
long lDistance     = 0;  // Distance(mm)
long lBeforeValue  = 0;  // Before measurement distance value
long lCurrentValue = 1;  // Before measurement distance value
int forceStopValue = 0;  // Value for periodically sending a stop signal
 
PowerFunctions pf(IR_TRANS_IN, CH1, IR_DEBUG_ON);
 
void setup() {
  pinMode(TRIGGER_PIN, OUTPUT); // Trigger is an output pin
  pinMode(ECHO_PIN,     INPUT); // Echo is an input pin
  Serial.begin(9600); // Serial Output
}
 
void loop() {
  lDistance = getDistance();
  Serial.print("Distance = "); // Output to serial
  Serial.print(lDistance);
  Serial.print("mm");
 
  operateTrain(RED, lDistance);
  Serial.println();
 
  delay(100); // Wait to do next measurement
}
 
// get distance using ultrasonic sensor(HC-SR04)
long getDistance()
{
  long distance  = 0;
  long duration = 0;
 
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH); // Trigger pin to HIGH
  delayMicroseconds(10); // 10us high
  digitalWrite(TRIGGER_PIN, LOW); // Trigger pin to HIGH
 
  duration = pulseIn(ECHO_PIN, HIGH); // Waits for the echo pin to get high
  // returns the dration in microseconds
 
  // Calculates the distance in mm
  // ((time)*(Speed of sound))/ toward and backward of object) * 10
  distance = ((duration / 2.9) / 2); // Actual calculation in mm
  //distance = duration / 74 / 2; // Actual calculation in inches
 
  return distance;
}
 
 
// operat train using IR transmitter(Keyes KY-005)
void operateTrain(uint8_t output, long lDistance) {
 
  if (lDistance > 0 && lDistance < NEAREST_VALUE) {
    lCurrentValue = PWM_FWD7;
  }
  else if (lDistance >= NEAREST_VALUE && lDistance < NEAREST_VALUE + OFFSET) {
    lCurrentValue = PWM_FWD6;
  }
  else if (lDistance >= NEAREST_VALUE + OFFSET && lDistance < NEAREST_VALUE + OFFSET * 2) {
    lCurrentValue = PWM_FWD5;
  }
  else if (lDistance >= NEAREST_VALUE + OFFSET * 2 && lDistance < NEAREST_VALUE + OFFSET * 3) {
    lCurrentValue = PWM_FWD4;
  }
  else if (lDistance >= NEAREST_VALUE + OFFSET * 3 && lDistance < NEAREST_VALUE + OFFSET * 4) {
    lCurrentValue = PWM_FWD3;
  }
  else if (lDistance >= NEAREST_VALUE + OFFSET * 4 && lDistance < NEAREST_VALUE + OFFSET * 5) {
    lCurrentValue = PWM_FWD2;
  }
  else if (lDistance >= NEAREST_VALUE + OFFSET * 5 && lDistance < NEAREST_VALUE + OFFSET * 6) {
    lCurrentValue = PWM_FWD1;
  }
  else if ( lDistance >= NEAREST_VALUE + OFFSET * 6)
  {
    lCurrentValue = PWM_BRK;
  }
 
  Serial.print(" PWM_CODE : ");
  Serial.print(lCurrentValue);
  if (lCurrentValue != lBeforeValue)
  {
    pf.single_pwm(output, lCurrentValue);
    Serial.print(" IR_SIGNAL_SENT");
  }
  lBeforeValue = lCurrentValue;
 
  forceStopValue++;
 
  if (forceStopValue > 100) {
    if ( lCurrentValue == PWM_BRK )
    {
      pf.single_pwm(output, lCurrentValue);
      Serial.print(" PERIODICALLY_PWM_BRK_SENT");
    }
    forceStopValue = 0;
  }
}
