#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

String command;
int speedCar = 1023; // 400 - 1023.
int speed_Coeff = 3;

const char *ssid = "POCOX3";			 
const char *password = "12345677"; 
WebServer server(80);

#define ENA 23	// Enable/speed motors Right        GPIO14(D5)
#define ENB 22	// Enable/speed motors Left         GPIO12(D6)
#define IN_1 16 // L298N in1 motors Right           GPIO15(D8)
#define IN_2 17 // L298N in2 motors Right           GPIO13(D7)
#define IN_3 18 // L298N in3 motors Left            GPIO2(D4)
#define IN_4 19 // L298N in4 motors Left            GPIO0(D3)

#define PUMP_PIN 2

#define DHTPIN 21
#define DHTTYPE DHT11
#define smokepin 34
#define soil_sensor 33

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);
	pinMode(IN_1, OUTPUT);
	pinMode(IN_2, OUTPUT);
	pinMode(IN_3, OUTPUT);
	pinMode(IN_4, OUTPUT);
	pinMode(PUMP_PIN, OUTPUT);
	pinMode(smokepin, INPUT);
	pinMode(soil_sensor, INPUT);

	digitalWrite(PUMP_PIN, LOW);

	Serial.begin(115200);
	dht.begin();

	// Connecting to existing WiFi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi....");
	}

	Serial.println("Connected to WiFi");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	// Starting WEB-server
	server.on("/", HTTP_handleRoot);

	server.on("/sensorData", HTTP_handleSensorData); 

	server.onNotFound(HTTP_handleRoot);
	server.begin();
	delay(4000);
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

void startPump()
{
	digitalWrite(PUMP_PIN, HIGH);
}

void stopPump()
{
	digitalWrite(PUMP_PIN, LOW);
}

float readDHTTemperature()
{
	float t = dht.readTemperature();
	if (isnan(t))
	{
		Serial.println("Failed to read from DHT sensor!");
		return -1;
	}
	else
	{
		Serial.println(t);
		return t;
	}
}

float readDHTHumidity()
{
	float h = dht.readHumidity();
	if (isnan(h))
	{
		Serial.println("Failed to read from DHT sensor!");
		return -1;
	}
	else
	{
		Serial.println(h);
		return h;
	}
}

float readSmokeSensor()
{
	int t = analogRead(smokepin);
	if (isnan(t))
	{
		Serial.println("Failed to read from Smoke sensor!");
		return -1;
	}
	else
	{
		Serial.println(t);
		return t;
	}
}

float readSoilSensor()
{
	float t = analogRead(soil_sensor);
	if (isnan(t))
	{
		Serial.println("Failed to read from DHT sensor!");
		return -1;
	}
	else
	{
		Serial.println(t);
		return t;
	}
}

const char *htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>DSLD Project Agriculture Bot</title>
		<style>
			body {
				font-family: Arial, sans-serif;
				display: flex;
				flex-direction: column;
				align-items: center;
				justify-content: center;
				height: 100vh;
				margin: 0;
			}

			h1 {
				text-align: center;
			}

			table {
				margin: 0 auto;
			}

			td {
				text-align: center;
			}

			button {
				width: 80px;
				height: 80px;
				font-size: 18px;
				margin: 5px;
			}

			@media (max-width: 600px) {
				button {
					width: 60px;
					height: 60px;
					font-size: 14px;
				}
			}
		</style>
	</head>
	<body>
		<h1>DSLD Project Agriculture Bot</h1>
		<table>
			<tr>
				<td><button id="forwardLeftBtn" ontouchstart="sendCommand('G')" ontouchend="sendCommand('S')">↖️</button></td>
				<td><button id="forwardBtn" ontouchstart="sendCommand('F')" ontouchend="sendCommand('S')">⬆️</button></td>
				<td><button id="forwardRightBtn" ontouchstart="sendCommand('I')" ontouchend="sendCommand('S')">↗️</button></td>
			</tr>
			<tr>
				<td><button id="leftBtn" ontouchstart="sendCommand('L')" ontouchend="sendCommand('S')">⬅️</button></td>
				<td><button id="stopBtn" ontouchstart="sendCommand('S')" ontouchend="sendCommand('S')">⏹️</button></td>
				<td><button id="rightBtn" ontouchstart="sendCommand('R')" ontouchend="sendCommand('S')">➡️</button></td>
			</tr>
			<tr>
				<td><button id="backwardLeftBtn" ontouchstart="sendCommand('H')" ontouchend="sendCommand('S')">↙️</button></td>
				<td><button id="backwardBtn" ontouchstart="sendCommand('B')" ontouchend="sendCommand('S')">⬇️</button></td>
				<td><button id="backwardRightBtn" ontouchstart="sendCommand('J')" ontouchend="sendCommand('S')">↘️</button></td>
			</tr>
		</table>
				<button id="startPumpBtn" onclick="sendCommand('P')"">Start Pump</button>
        <button id="stopPumpBtn" onclick="sendCommand('Q')"">Stop Pump</button>
		
		<p>Temperature: <span id="temperature">--</span> °C</p>
    <p>Humidity: <span id="humidity">--</span>%</p>
		<p>Smoke Sensor: <span id="smoke">--</span></p>
    <p>Soil Moisture: <span id="soil">--</span></p>
		<script>
			function sendCommand(command) {
				var xhr = new XMLHttpRequest();
				xhr.open("GET", "/?State=" + command, true);
				xhr.send();
			}

			function updateSensorData() {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var data = JSON.parse(xhr.responseText);
                    document.getElementById("temperature").innerText = data.temperature.toFixed(2);
                    document.getElementById("humidity").innerText = data.humidity.toFixed(2);
										document.getElementById("smoke").innerText = data.smoke.toFixed(2);
										document.getElementById("soil").innerText = data.soil.toFixed(2);
                }
            };
            xhr.open("GET", "/sensorData", true);
            xhr.send();
        }

        setInterval(updateSensorData, 5000);
		</script>
	</body>
</html>


)HTMLHOMEPAGE";

void HTTP_handleRoot(void)
{
	server.send(200, "text/html", htmlHomePage);
}

void HTTP_handleSensorData(void)
{
	String sensorData = "{\"temperature\":" + String(readDHTTemperature()) + ",\"humidity\":" + String(readDHTHumidity()) + ",\"smoke\":" + String(readSmokeSensor()) + ",\"soil\":" + String(readSoilSensor()) + "}";
	server.send(200, "application/json", sensorData);
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
	else if (command == "S")
		stopRobot();
	else if (command == "Q")
		startPump();
	else if (command == "P")
		stopPump();
}
