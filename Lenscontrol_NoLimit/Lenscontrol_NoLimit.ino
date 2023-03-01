#include <Stepper.h>
#include <Encoder.h>
#include <Ethernet.h>

Encoder myEnc(14, 15);
long oldPosition  = -999;

// stepper1
#define SPIN1 22
#define SPIN2 24
#define SPIN3 26
#define SPIN4 28

// stepper
#define SPIN5 30
#define SPIN6 32
#define SPIN7 34
#define SPIN8 36

// stepper
#define SPIN9 38
#define SPIN10 40
#define SPIN11 42
#define SPIN12 44

// Steps
#define STEP 2048

//Objects
Stepper stepper1(STEP, SPIN1, SPIN3, SPIN2, SPIN4);
Stepper stepper2(STEP, SPIN5, SPIN7, SPIN6, SPIN8);
Stepper stepper3(STEP, SPIN9, SPIN11, SPIN10, SPIN12);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 192, 168, 0, 3 }; // ip in lan
byte gateway[] = { 192, 168, 0, 1 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port

String readString;


void setup() {

  // configure steps by rpm
  stepper1.setSpeed( 15 );
  stepper2.setSpeed( 15 );
  stepper3.setSpeed( 15 );

  pinMode(SPIN1, OUTPUT);
  pinMode(SPIN2, OUTPUT);
  pinMode(SPIN3, OUTPUT);
  pinMode(SPIN4, OUTPUT);
  pinMode(SPIN5, OUTPUT);
  pinMode(SPIN6, OUTPUT);
  pinMode(SPIN7, OUTPUT);
  pinMode(SPIN8, OUTPUT);
  pinMode(SPIN9, OUTPUT);
  pinMode(SPIN10, OUTPUT);
  pinMode(SPIN11, OUTPUT);
  pinMode(SPIN12, OUTPUT);

  pinMode(5, OUTPUT); //pin selected to control
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  server.begin();

  //enable serial data print
  Serial.begin(9600);
  Serial.println("LensServo"); // so I can keep track of what is loaded


}


void Pinoff(int servoNo) {
  switch (servoNo) {
    case 1:
      digitalWrite(SPIN1, LOW);
      digitalWrite(SPIN2, LOW);
      digitalWrite(SPIN3, LOW);
      digitalWrite(SPIN4, LOW);
      //      Serial.println("Pinoff Complete");
      break;
    case 2:
      digitalWrite(SPIN5, LOW);
      digitalWrite(SPIN6, LOW);
      digitalWrite(SPIN7, LOW);
      digitalWrite(SPIN8, LOW);
      //      Serial.println("Pinoff Complete");
      break;
    case 3:
      digitalWrite(SPIN9, LOW);
      digitalWrite(SPIN10, LOW);
      digitalWrite(SPIN11, LOW);
      digitalWrite(SPIN12, LOW);
      //      Serial.println("Pinoff Complete");
      break;
    default:
      Serial.println("Undef Pinoff");
      break;
  }
}

void Stepwork(int servoNo, int steps) {
  Serial.print("Stepwork: servoNo:");
  Serial.print(servoNo);
  Serial.print(", steps; ");
  Serial.print(steps);

  switch (servoNo) {
    case 1:
      stepper1.step(steps);
      Pinoff(servoNo);
      Serial.println(" -> Complete");
      break;
    case 2:
      stepper2.step(steps);
      Pinoff(servoNo);
      Serial.println(" -> Complete");
      break;
    case 3:
      stepper3.step(steps);
      Pinoff(servoNo);
      Serial.println(" -> Complete");
      break;
    default:
      Serial.println(" -> Pinoff");
      break;
  }

  Serial.println("---------------------------------------------");
}


void loop() {
  // Create a client connection
  EthernetClient client = server.available();



  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string
          readString += c;
        }

        //if HTTP request has ended
        if (c == '\n') {
          if (readString.indexOf('?') >= 0) {
            client.println("HTTP/1.1 204 OK");
            client.println();
            client.println();
          }
          else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");

            char* longString[] = { "",
                                   "<HTML>",
                                   "  <HEAD>",
                                   "  <TITLE>LensServo</TITLE>",
                                   "  <BODY>",
                                   "    <H1>LensServo</H1>",
                                   "    <FORM ACTION='/' method=get>",
                                   "      Zoom Gear Control(-1024 ~ 1024): <INPUT TYPE=TEXT NAME='zc' VALUE='' SIZE='10' MAXLENGTH='10'>",
                                   "     </FORM>",
                                   "    <FORM ACTION='/' method=get>",
                                   "      Focus Control(-1024 ~ 1024): <INPUT TYPE=TEXT NAME='fc' VALUE='' SIZE='10' MAXLENGTH='10'>",
                                   "     </FORM>",
                                   "    <FORM ACTION='/' method=get>",
                                   "      IRIS Gear Control(-1024 ~ 1024): <INPUT TYPE=TEXT NAME='ic' VALUE='' SIZE='10' MAXLENGTH='10'>",
                                   "     </FORM>",
                                   "  </BODY>",
                                   "</HTML>"
                                 };

            for (int i = 0; i < (sizeof(longString) / sizeof(int)); i++) {
              client.println(longString[i]);
            }
          }

          delay(1);
          //stopping client
          client.stop();

          if (readString.indexOf("zc") > 0) {
            String readString_trimmed = (String)readString;
            readString_trimmed.trim();
            Serial.println("readString: \"" + readString_trimmed + "\"");

            int respValEnd = readString.indexOf(" HTTP");
            String cmdVal = readString.substring(9, respValEnd);
            Stepwork(1, cmdVal.toInt());
            delay(1000);
          }

          if (readString.indexOf("fc") > 0) {
            Serial.println("readString\"" + (String)readString + "\"");

            int respValEnd = readString.indexOf(" HTTP");
            String cmdVal = readString.substring(9, respValEnd);
            Serial.println("cmdVal\"" + cmdVal + "\"");
            Stepwork(2, cmdVal.toInt());
            delay(1000);
          }

          if (readString.indexOf("ic") > 0) {
            String readString_trimmed = (String)readString;
            readString_trimmed.trim();
            Serial.println("readString\"" + readString_trimmed + "\"");

            int respValEnd = readString.indexOf(" HTTP");
            String cmdVal = readString.substring(9, respValEnd);
            Serial.println("cmdVal\"" + cmdVal + "\"");
            Stepwork(3, cmdVal.toInt());
            delay(1000);
          }

          readString = "";

        }
      }
    }
  }
}
