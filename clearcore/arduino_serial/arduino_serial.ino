String nom = "Arduino";
String msg;
void setup() {
   Serial.begin(9600);
}
void loop() {
  readSerialPort();
  if (msg != "") {
      if (msg == "DETECT_FILE_CMD" || "DETECT_FILE_CMD\n") {
          int resp = detectInputFile();
          if (resp) {
             Serial.print("HAS_INPUT_MOTOR_CSV");
          }
      } else {
        sendData();
      }
  }
  delay(500);
}

void readSerialPort() {
  msg = "";
  if (Serial.available()) {
      //delay(10);
      while (Serial.available() > 0) {
          //msg += (char)Serial.read();
          msg = Serial.readString();
          //msg = Serial.readStringUntil('#');
      }
      //Serial.flush();
  }
}

int detectInputFile() {
  return 1;
}

void sendData() {
  //write data
  Serial.print(nom);
  Serial.print(" received : ");
  Serial.print(msg);
}
