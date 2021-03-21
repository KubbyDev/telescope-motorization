#define POT_PIN A0
#define AVCC 5
#define ADC_PRECISION 10
#define VMIN 1
#define VMAX 4
#define MIN_SPEED 0 // In step/s
#define MAX_SPEED 1000 // In step/s

#define BUT_PIN 2

#define DIR_PIN 4
#define STP_PIN 6

void setup() {
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
  pinMode(BUT_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STP_PIN, OUTPUT);
  while(!Serial);
}

void pulse(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

float clamp(float x, float down, float up) {
  return max(down, min(x, up));
}

unsigned long get_period(int adc_reading) {
  float voltage = adc_reading * (AVCC / (float)(1 << ADC_PRECISION));
  float input = (clamp(voltage, VMIN, VMAX) - VMIN)/(VMAX - VMIN); // input is in [0; 1]
  float step_per_sec = input * (MAX_SPEED - MIN_SPEED) + MIN_SPEED;
  Serial.println(step_per_sec);
  if(step_per_sec <= 0)
    return 1e9;
  return ceil(1e6/step_per_sec); // in microseconds
}

unsigned long last_step = 0;
bool dir = true;

void loop() {
  
  // Direction change
  if(digitalRead(BUT_PIN)) {
    delay(1000);
    dir = !dir;
    digitalWrite(DIR_PIN, dir);
    while(digitalRead(BUT_PIN));
  }

  // Speed change
  unsigned long period = get_period(analogRead(POT_PIN));
    
  // Sends a step pulse if necessary
  unsigned long now = micros();
  if(now >= last_step + period) {
    pulse(STP_PIN);
    last_step = now;
  }
}
