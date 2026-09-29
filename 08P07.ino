// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10

#define _DIST_MIN 100.0
#define _DIST_MID 200.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2.0) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time = 0;

void setup() {

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // Active LOW
  // 0 = brightest, 255 = OFF
  analogWrite(PIN_LED, 255);

  Serial.begin(57600);
}

void loop() {

  float distance;
  int led_value;

  // wait until next sampling time
  if (millis() < (last_sampling_time + INTERVAL)) {
    return;
  }

  // measure distance
  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // outside measurement range
  if ((distance == 0.0) ||
      (distance < _DIST_MIN) ||
      (distance > _DIST_MAX)) {

    led_value = 255;
  }

  // 100mm ~ 200mm
  else if (distance <= _DIST_MID) {

    float ratio =
      (distance - _DIST_MIN) /
      (_DIST_MID - _DIST_MIN);

    led_value = (int)(255.0 * (1.0 - ratio) + 0.5);
  }

  // 200mm ~ 300mm
  else {

    float ratio =
      (distance - _DIST_MID) /
      (_DIST_MAX - _DIST_MID);

    led_value = (int)(255.0 * ratio + 0.5);
  }

  led_value = constrain(led_value, 0, 255);

  analogWrite(PIN_LED, led_value);

  // output values to serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",distance:");
  Serial.print(distance);

  Serial.print(",LED:");
  Serial.print(led_value);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println();

  // update next sampling time
  last_sampling_time += INTERVAL;
}


// get a distance reading from USS
// return value is in millimeter
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  unsigned long duration =
    pulseIn(ECHO, HIGH, TIMEOUT);

  return duration * SCALE;
}ㄴ
