// Create a union to easily convert float to byte
typedef union{
  float number;
  uint8_t bytes[4];
} FLOATUNION_t;

// PID parameters
float Kp = 7.57390076801148;
float Ki = 9.07873843489081;
float Kd = 1.5685268606042;

// Variables for PID calculations
float previousError = 0;
float integral = 0;
unsigned long previousTime = 0;

// Create the variable you want to send
FLOATUNION_t myValue;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // Initialize timing
  previousTime = millis();
}

void loop() {
  // Check if data is available to read
  if (Serial.available() >= 4) { // Ensure there are enough bytes available for a float
    // Read the error value sent from Simulink
    float error = getFloat();

    // Calculate time difference
    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0; // in seconds

    // PID calculations
    integral += error * elapsedTime;
    float derivative = (error - previousError) / elapsedTime;
    float output = Kp * error + Ki * integral + Kd * derivative;

    // Send the PID output back to Simulink
    sendFloat(output);

    // Update previous values for next iteration
    previousError = error;
    previousTime = currentTime;
  }
}

// Function to get float from serial
float getFloat(){
  int cont = 0;
  FLOATUNION_t f;
  while (cont < 4 && Serial.available() > 0){
    f.bytes[cont] = Serial.read();
    cont++;
  }
  return f.number;
}

// Function to send float to serial
void sendFloat(float value){
  FLOATUNION_t f;
  f.number = value;

  // Print header: Important to avoid sync errors!
  Serial.write('A');

  // Print float data
  for (int i = 0; i < 4; i++) {
    Serial.write(f.bytes[i]);
  }

  // Print terminator
  Serial.print('\n');
}
