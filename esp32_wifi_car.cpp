#include "Arduino.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

constexpr size_t MOTOR_PIN_COUNT{4};
constexpr size_t SPEED_CONTROL_PIN_COUNT{2};

enum speedSettings
{
	SLOW = 100,
	NORMAL = 180,
	FAST = 255
};

class WifiCar
{
private:
	int motorPins[4] = {16, 17, 32, 33};
	int speedControlpins[2] = {21, 22};
	const int freq = 2000;
	const int channel_0 = 1;
	const int channel_1 = 2;
	const int resolution = 8;
	speedSettings currentSpeedSettings;
	AsyncWebServer server{80};
	AsyncWebSocket ws{"/ws"};

public:
	WifiCar();
	WifiCar(const int *newMotorPins, const int *newSpeedControlPins);
	~WifiCar();
	void init();
	void setMotorPinStatus(uint8_t *pinStatusPointer);
	void turnLeft();
	void setMotorSpeed();
	void turnRight();
	void moveForward();
	void moveBackward();
	void stop();
	void setCurrentSpeed(speedSettings newSpeedSettings);
	speedSettings getCurrentSpeed();
	void sendCarCommand(const char *command);
	void start(std::function<void(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType,
																void *, uint8_t *, size_t)>
								 webSocketCallbackFunc);
};

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}

WifiCar::WifiCar()
{
	init();
}

WifiCar::WifiCar(const int *newMotorPins, const int *newSpeedControlPins)
{
	for (size_t i = 0; i < MOTOR_PIN_COUNT; i++)
	{
		motorPins[i] = *(newMotorPins + i);
	}

	for (size_t i = 0; i < SPEED_CONTROL_PIN_COUNT; i++)
	{
		speedControlpins[i] = *(newSpeedControlPins + i);
	}
}

WifiCar::~WifiCar()
{
}

void WifiCar::init()
{
	for (size_t iCtr = 0; iCtr < MOTOR_PIN_COUNT; iCtr++)
	{
		pinMode(motorPins[iCtr], OUTPUT);
		digitalWrite(motorPins[iCtr], LOW);
	}

	for (size_t iCtr = 0; iCtr < SPEED_CONTROL_PIN_COUNT; iCtr++)
	{
		pinMode(speedControlpins[iCtr], OUTPUT);
	}

	ledcSetup(channel_0, freq, resolution);
	ledcSetup(channel_1, freq, resolution);

	ledcAttachPin(speedControlpins[0], channel_0);
	ledcAttachPin(speedControlpins[1], channel_1);

	setCurrentSpeed(speedSettings::NORMAL);
}

void WifiCar::setMotorSpeed()
{
	ledcWrite(channel_0, currentSpeedSettings);
	ledcWrite(channel_1, currentSpeedSettings);
}

void WifiCar::setMotorPinStatus(uint8_t *pinStatusPointer)
{
	for (size_t iCtr = 0; iCtr < MOTOR_PIN_COUNT; iCtr++)
	{
		digitalWrite(motorPins[iCtr], *(pinStatusPointer + iCtr));
	}
}

void WifiCar::turnLeft()
{
	setMotorSpeed();
	uint8_t status[MOTOR_PIN_COUNT] = {LOW, HIGH, LOW, LOW};
	setMotorPinStatus(status);
}

void WifiCar::turnRight()
{
	setMotorSpeed();
	uint8_t status[MOTOR_PIN_COUNT] = {LOW, LOW, LOW, HIGH};
	setMotorPinStatus(status);
}

void WifiCar::moveForward()
{
	setMotorSpeed();
	uint8_t status[MOTOR_PIN_COUNT] = {LOW, HIGH, LOW, HIGH};
	setMotorPinStatus(status);
}

void WifiCar::moveBackward()
{
	setMotorSpeed();
	uint8_t status[MOTOR_PIN_COUNT] = {HIGH, LOW, HIGH, LOW};
	setMotorPinStatus(status);
}

void WifiCar::stop()
{
	ledcWrite(channel_0, 0);
	ledcWrite(channel_1, 0);

	uint8_t status[MOTOR_PIN_COUNT] = {LOW, LOW, LOW, LOW};
	setMotorPinStatus(status);
}

void WifiCar::setCurrentSpeed(speedSettings newSpeedSettings)
{
	currentSpeedSettings = newSpeedSettings;
}

speedSettings WifiCar::getCurrentSpeed()
{
	return currentSpeedSettings;
}

void WifiCar::start(std::function<void(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType,
																			 void *, uint8_t *, size_t)>
												webSocketCallbackFunc)
{
	ws.onEvent(webSocketCallbackFunc);
	server.addHandler(&ws);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
						{ request->send(SPIFFS, "C:\\Users\\mcb76\\DSLD\\index.html", "text/html", false); });

	server.on("/css/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
						{ request->send(SPIFFS, "C:\\Users\\mcb76\\DSLD\\framework.css", "text/css"); });

	server.on("/css/custom.css", HTTP_GET, [](AsyncWebServerRequest *request)
						{ request->send(SPIFFS, "C:\\Users\\mcb76\\DSLD\\custom.css", "text/css"); });

	server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request)
						{ request->send(SPIFFS, "C:\\Users\\mcb76\\DSLD\\custom.js", "text/javascript"); });

	server.onNotFound(notFound);

	server.begin();
}

void WifiCar::sendCarCommand(const char *command)
{
	if (strcmp(command, "left") == 0)
	{
		this->turnLeft();
	}
	else if (strcmp(command, "right") == 0)
	{
		this->turnRight();
	}
	else if (strcmp(command, "up") == 0)
	{
		this->moveForward();
	}
	else if (strcmp(command, "down") == 0)
	{
		this->moveBackward();
	}
	else if (strcmp(command, "stop") == 0)
	{
		this->stop();
	}
	else if (strcmp(command, "slow-speed") == 0)
	{
		this->setCurrentSpeed(speedSettings::SLOW);
	}
	else if (strcmp(command, "normal-speed") == 0)
	{
		this->setCurrentSpeed(speedSettings::NORMAL);
	}
	else if (strcmp(command, "fast-speed") == 0)
	{
		this->setCurrentSpeed(speedSettings::FAST);
	}
}

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}
