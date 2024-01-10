#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

String command;			 // String to store app command state.
int speedCar = 1023; // 400 - 1023.
int speed_Coeff = 3;

const char *ssid = "NodeMCU Car";
WebServer server(80);

#define ENA 23	// Enable/speed motors Right        GPIO14(D5)
#define ENB 22	// Enable/speed motors Left         GPIO12(D6)
#define IN_1 16 // L298N in1 motors Right           GPIO15(D8)
#define IN_2 17 // L298N in2 motors Right           GPIO13(D7)
#define IN_3 18 // L298N in3 motors Left            GPIO2(D4)
#define IN_4 19 // L298N in4 motors Left            GPIO0(D3)

void setup()
{
	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);
	pinMode(IN_1, OUTPUT);
	pinMode(IN_2, OUTPUT);
	pinMode(IN_3, OUTPUT);
	pinMode(IN_4, OUTPUT);

	Serial.begin(115200);

	// Connecting WiFi
	WiFi.mode(WIFI_AP);
	WiFi.softAP(ssid);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

	// Starting WEB-server
	server.on("/", HTTP_handleRoot);
	server.onNotFound(HTTP_handleRoot);
	server.begin();
}

void goAhead()
{
	digitalWrite(IN_1, LOW);
	digitalWrite(IN_2, HIGH);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, LOW);
	digitalWrite(IN_4, HIGH);
	analogWrite(ENB, speedCar);
}

void goBack()
{
	digitalWrite(IN_1, HIGH);
	digitalWrite(IN_2, LOW);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, HIGH);
	digitalWrite(IN_4, LOW);
	analogWrite(ENB, speedCar);
}

void goRight()
{

	digitalWrite(IN_1, HIGH);
	digitalWrite(IN_2, LOW);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, LOW);
	digitalWrite(IN_4, HIGH);
	analogWrite(ENB, speedCar);
}

void goLeft()
{

	digitalWrite(IN_1, LOW);
	digitalWrite(IN_2, HIGH);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, HIGH);
	digitalWrite(IN_4, LOW);
	analogWrite(ENB, speedCar);
}

void goAheadRight()
{

	digitalWrite(IN_1, LOW);
	digitalWrite(IN_2, HIGH);
	analogWrite(ENA, speedCar / speed_Coeff);

	digitalWrite(IN_3, LOW);
	digitalWrite(IN_4, HIGH);
	analogWrite(ENB, speedCar);
}

void goAheadLeft()
{

	digitalWrite(IN_1, LOW);
	digitalWrite(IN_2, HIGH);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, LOW);
	digitalWrite(IN_4, HIGH);
	analogWrite(ENB, speedCar / speed_Coeff);
}

void goBackRight()
{

	digitalWrite(IN_1, HIGH);
	digitalWrite(IN_2, LOW);
	analogWrite(ENA, speedCar / speed_Coeff);

	digitalWrite(IN_3, HIGH);
	digitalWrite(IN_4, LOW);
	analogWrite(ENB, speedCar);
}

void goBackLeft()
{

	digitalWrite(IN_1, HIGH);
	digitalWrite(IN_2, LOW);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, HIGH);
	digitalWrite(IN_4, LOW);
	analogWrite(ENB, speedCar / speed_Coeff);
}

void stopRobot()
{

	digitalWrite(IN_1, LOW);
	digitalWrite(IN_2, LOW);
	analogWrite(ENA, speedCar);

	digitalWrite(IN_3, LOW);
	digitalWrite(IN_4, LOW);
	analogWrite(ENB, speedCar);
}
const char *htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
  <html>
<head>
  <title>ESP32 Motor Control</title>
</head>
<body>
  <h1>ESP32 Motor Control</h1>
  <button id="forwardBtn">Forward</button>
  <button id="backwardBtn">Backward</button>
  <button id="leftBtn">Left</button>
  <button id="rightBtn">Right</button>
  <button id="forwardRightBtn">Forward Right</button>
  <button id="forwardLeftBtn">Forward Left</button>
  <button id="backwardRightBtn">Backward Right</button>
  <button id="backwardLeftBtn">Backward Left</button>
  <br><br>
  <button id="stopBtn">Stop</button>

  <script>
    function sendCommand(command) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', '/?State=' + command, true);
      xhr.send();
    }

    // Add event listeners to each button
    document.getElementById('forwardBtn').addEventListener('click', function() {
      sendCommand('F');
    });

    document.getElementById('backwardBtn').addEventListener('click', function() {
      sendCommand('B');
    });

    document.getElementById('leftBtn').addEventListener('click', function() {
      sendCommand('L');
    });

    document.getElementById('rightBtn').addEventListener('click', function() {
      sendCommand('R');
    });

    document.getElementById('forwardRightBtn').addEventListener('click', function() {
      sendCommand('I');
    });

    document.getElementById('forwardLeftBtn').addEventListener('click', function() {
      sendCommand('G');
    });

    document.getElementById('backwardRightBtn').addEventListener('click', function() {
      sendCommand('J');
    });

    document.getElementById('backwardLeftBtn').addEventListener('click', function() {
      sendCommand('H');
    });

    document.getElementById('stopBtn').addEventListener('click', function() {
      sendCommand('S');
    });
  </script>
</body>
</html>
)HTMLHOMEPAGE";

void HTTP_handleRoot(void)
{
	server.send(200, "text/html", htmlHomePage);
}

void loop()
{
	server.handleClient();

	command = server.arg("State");
	if (command == "F")
		goAhead();
	else if (command == "B")
		goBack();
	else if (command == "L")
		goLeft();
	else if (command == "R")
		goRight();
	else if (command == "I")
		goAheadRight();
	else if (command == "G")
		goAheadLeft();
	else if (command == "J")
		goBackRight();
	else if (command == "H")
		goBackLeft();
	else if (command == "0")
		speedCar = 1023;
	else if (command == "1")
		speedCar = 1023;
	else if (command == "2")
		speedCar = 1023;
	else if (command == "3")
		speedCar = 1023;
	else if (command == "4")
		speedCar = 1023;
	else if (command == "5")
		speedCar = 1023;
	else if (command == "6")
		speedCar = 1023;
	else if (command == "7")
		speedCar = 1023;
	else if (command == "8")
		speedCar = 1023;
	else if (command == "9")
		speedCar = 1023;
	else if (command == "S")
		stopRobot();
}
