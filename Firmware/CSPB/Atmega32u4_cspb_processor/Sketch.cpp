/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

// -------------------------------------------------------------------------------------------
// cspb_processor
// -------------------------------------------------------------------------------------------
//
// This creates an I2C Slave device that processes Cluster System Power Board commands.
//
// -------------------------------------------------------------------------------------------

//#include <i2c_t3.h>
#include <Wire.h>
#include <EEPROM.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio



// Function prototypes
void receiveEvent(int count);
void requestEvent(void);
void processCommand();
void setSlotPower(unsigned int value);
void ShutdownSlot(unsigned int value);
void signalShutdownOnSlot(int slot);
void waitForShutdownState(int slot);
void powerDownSlot(int slot);
void setPinState(int pinNumber, int state);
//int get_monitor_line_states();
//int get_power_states();
int get_port_state(const int port_list[4]);

// Memory
#define MEM_LEN 256 // Bytes
#define STATUS_LED A4
#define TX_RX_LED A5
uint8_t databuf[MEM_LEN];

// Variables
volatile uint8_t received;
int register_address = 0;
int count_test = 0;
bool is_in_shutdown = false;

// EEPROM Byte number location for programmable values.
const int EEPROM_SIZE = 128;         // in Bytes
const int EEPROM_I2C_ADDR = 0;       // i2c power up address
const int EEPROM_CSPB_BOOT_DATA = 1; // cspb boot data, which slots are powered up on boot cspb boot up
const int EEPROM_APP_DATA = 2;       // cspb application data
const int EEPROM_POWER_UP_DELAY = 3; // power up delay value (in 20 millisecond units, 50x20 - 1000 milliseconds or 1 second.)
const int EEPROM_SHUTDOWN_SIGNAL_HOLD_TIME = 4; // shutdown hold time value (in 20 millisecond units, 50x20 - 1000 milliseconds or 1 second.)
const int EEPROM_SHUTDOWN_TIME_OUT = 5; // shutdown time out value.

// IO Definitions
// Pin 18 = i2c bus - SDA0
// PIN 19 = i2c bus - SCL0
const int POWER_PIN[] = {30, 6, 10, 99};  // Pin assignments for each slots power relay.
const int SHUTDOWN_PIN[] = {0, 4, 8, 5};  // Pin assignments for each slots shutdown signal.
const int MONITOR_PIN[] = {1, 12, 9, 13}; // Pin assignments for each slots shutdown monitor.
const int DEFAULT_ADDRESS = 7;            // Use i2c address 0x30 - Jumper
const int ALT_ADDRESS = 11;               // Use i2c address 0x15 - Jumper

// Application
const int NUMBER_OR_SLOTS = sizeof(POWER_PIN) / sizeof(POWER_PIN[0]);
unsigned long previousMillis = 0; //  Store last time LED was updated

//
// Setup
//
void setup() {

  pinMode(STATUS_LED, OUTPUT);     // Analog Pin - Test LED
  pinMode(TX_RX_LED, OUTPUT);      // Analog Pin - Board Status.
  pinMode(ALT_ADDRESS, INPUT);     // Alternate address header
  pinMode(DEFAULT_ADDRESS, INPUT); // Default address header
  DDRE |= (1 << 2);                // Set HWB pin as output. This is used for power relay 4 know as pin 99.

  setPinState(STATUS_LED, HIGH);  // Power on, processor running!
  setPinState(TX_RX_LED, HIGH);   // In setup phase.

  // Set pin modes for power, shutdown and monitor functions.
  // Default the power relays to low (off). No power to the slot.
  // Default shutdown signals to low. Transition to high is a shutdown request.
  for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++) {
    pinMode(POWER_PIN[slot], OUTPUT);
    pinMode(SHUTDOWN_PIN[slot], OUTPUT);
    pinMode(MONITOR_PIN[slot], INPUT);
    setPinState(POWER_PIN[slot], LOW);
    setPinState(SHUTDOWN_PIN[slot], LOW);
  }

  // Setup i2c bus address.
  int addr;
  if (digitalRead(DEFAULT_ADDRESS) == HIGH) // Header installed to Vcc.
    addr = 48; // 0x30
  else if (digitalRead(ALT_ADDRESS) == HIGH) // Header installed to Vcc.
    addr = 21; // 0x15
  else
    addr = EEPROM.read(EEPROM_I2C_ADDR); // Note - address 0x00 through 0x07 are reserved addresses. Do not use.

  Wire.begin(addr);

  // Read default power up config and set ports accordingly.
  unsigned int pwr_config = EEPROM.read(EEPROM_CSPB_BOOT_DATA);
  unsigned int pwr_delay = EEPROM.read(EEPROM_POWER_UP_DELAY) * 20;
  int power_on = 0;
  for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++ ) {
    power_on = (pwr_config >> slot) & 1;
    if (power_on == 1) {
		setPinState(POWER_PIN[slot], HIGH);
		delay(pwr_delay);
	}
  }

  // Initialize receive data signal and buffer.
  received = 0;
  memset(databuf, 0, sizeof(databuf));

  // register events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // start serial monitor
  setPinState(TX_RX_LED, LOW);  // Setup finished.
}


/**
  / Main program loop
**/
void loop()
{
	const long interval = 20;  // interval at which to blink (milliseconds)

	if (received) {
		setPinState(TX_RX_LED, HIGH);
		previousMillis = millis();
		processCommand();
		received = 0;
	}

	if (millis() - previousMillis >= interval) {
		setPinState(TX_RX_LED, LOW);
	}

}


/**
  / handle Rx Event (incoming I2C data)
**/
void receiveEvent(int count)
{
  int idx = 0;
  memset(databuf, 0, sizeof(databuf)); // clears current buffer.
  while (idx < count) {
    if (idx < MEM_LEN) {            // drop data beyond memory boundary
      databuf[idx] = Wire.read();   // copy data to memory
      
      char str_buffer [100]; // Typical formated message is 56 characters long.
      int number_of_chars;
      number_of_chars = sprintf(str_buffer, "CSPB received byte--> Char: '%s'  \t Hex: %#x \t Dec: %d \n", &databuf[idx], databuf[idx], databuf[idx]);
      //Serial.println(number_of_chars);
      Serial.print(str_buffer);
      idx++;
    }
    else
      Wire.read();                  // drop data if memory full
  }
  //Serial.print("CSPB received count: %d\n", count);
  received = count; // set received flag to count, this triggers print in main loop
}

/**
   handle Tx Event (outgoing I2C data)
   The incoming read command will contain the EEPROM register number in the cmd field.
   Only EEPROM registers 0-127 are available and can be queried. 128 to 255 could be used
   for other data return functions.
**/
void requestEvent(void)
{
	setPinState(TX_RX_LED, HIGH);
	if (register_address < EEPROM_SIZE)
		Wire.write(EEPROM.read(register_address));
	else if (register_address == 128)
		Wire.write(get_port_state(MONITOR_PIN));
	else if (register_address == 129)
		Wire.write(get_port_state(POWER_PIN));
	else if (register_address == 130)
		Wire.write(is_in_shutdown);
	else
		Wire.write(0);
	delay(50);
	setPinState(TX_RX_LED, LOW);
	//Serial.print("CSPB sent data. \n");
}


/**
  / Process the single character command.
**/
void processCommand()
{
  unsigned int cmd = databuf[1];

  if (cmd == '!') { // Power on/off slot.
    unsigned int value = databuf[2];
    setSlotPower(value);
  }
  else if (cmd == '#') { // SBC to shutdown.
    unsigned int value = databuf[2];
    ShutdownSlot(value);
  }
  else if (cmd == '$') { // Signal shutdown line.
    unsigned int value = databuf[2];
    int signalShutdown = 0;
    for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++ ) {
      signalShutdown = (value >> slot) & 1;
      if (signalShutdown == 1) {
        signalShutdownOnSlot(slot);
      }
    }
  }
  else if (cmd == 'W') { // Write EEPROM.
    unsigned int address = databuf[2];
    unsigned int data = databuf[3];
    //Serial.print("CSPB write byte address: '%d'\n", address);
    //Serial.print("CSPB write byte data: '%d'\n", data);
	if (address == EEPROM_I2C_ADDR) {
		if (data < 8) {
			// Reserved i2c bus address, ignore command.
		}
		else {
			EEPROM.write(address, data);	
		}
	}
	else {
		EEPROM.write(address, data);
	}
  }
  else if (cmd == 'R') { // Set the register read address for next register data request.
	  register_address = databuf[2];
  }
} // end processCommand


/**
  / setSlotPower
**/
void setSlotPower(unsigned int value)
{
	int state = LOW;
	int power_on = 0;
	char *endptr;

	//value = strtol(value, &endptr, 16)- 48;
	value = value - 48;
	Serial.println(value);
	
	for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++ ) {
		power_on = (value >> slot) & 1;
		Serial.println(power_on);
		if (power_on == HIGH)
			state = HIGH;
		else
			state = LOW;
		
		setPinState( POWER_PIN[slot],  state);
	}

} // end setSlotPower


/**
  / signalShutdownOnSlot
**/
void signalShutdownOnSlot(int slot)
{
  unsigned int signal_hold = EEPROM.read(EEPROM_SHUTDOWN_SIGNAL_HOLD_TIME) * 20;

  setPinState(SHUTDOWN_PIN[slot], HIGH);
  delay(signal_hold);
  setPinState(SHUTDOWN_PIN[slot], LOW);
}


/**
  / ShutdownSlot
**/
void ShutdownSlot(unsigned int value)
{
  is_in_shutdown = true;
  int signalShutdown = 0;
  for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++ ) {
    signalShutdown = (value >> slot) & 1;
    if (signalShutdown == 1) {
      signalShutdownOnSlot(slot);
      waitForShutdownState(slot);
      powerDownSlot(slot);
    }
  }
  is_in_shutdown = false;
}


/**
/ waitForShutdownState
**/
void waitForShutdownState(int slot)
{
  unsigned long timeOut = 1000 * EEPROM.read(EEPROM_SHUTDOWN_TIME_OUT); // Convert to seconds.
  unsigned long StartTime = millis();
  unsigned long CurrentTime = 0;
  unsigned long ElapsedTime = 0;

  do {
	  CurrentTime = millis();
	  ElapsedTime = CurrentTime - StartTime;
    if (digitalRead(MONITOR_PIN[slot]) == LOW) {
      ElapsedTime = timeOut; // SBC has shutdown, break out of loop.
    }
    else {
      delay(50); //50 milliseconds
    }
  } while (ElapsedTime < timeOut);  // in milliseconds
}


/**
  / powerDownSlot
**/
void powerDownSlot(int slot)
{
  setPinState(POWER_PIN[slot], LOW);
}


/**
/ get_monitor_line_states
**/
//int get_monitor_line_states()
//{
//  int port_states = 0;
//  int value = 0;
//  for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++) {
//    value = digitalRead(MONITOR_PIN[slot]);
//    port_states = port_states | (value << slot);
//  }
//  return port_states;
//}


/**
/ get_power_states
**/
//int get_power_states()
//{
//  int port_states = 0;
//  int value = 0;
//  for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++) {
//    value = digitalRead(POWER_PIN[slot]);
//    port_states = port_states | (value << slot);
//  }
//  return port_states;
//}

/************************************************************************/
/* get_port_state                                                                     */
/************************************************************************/
int get_port_state(const int port_list[4])
{
	 int port_states = 0;
	 int value = 0;
	 for (int slot = 0; slot < NUMBER_OR_SLOTS; slot++) {
		 if (port_list[slot] != 99) { // This is a kludge to access the HBW pin on the ATMEGA32U4 which is not defined in the arduino pins file.
			value = digitalRead(port_list[slot]);
			port_states = port_states | (value << slot);
		 }
		 else {
			 value = (PINE & (1 << PINE2)) >> PINE2;
			 port_states = port_states | (value << slot);
		 }
	 }
	 return port_states;
}

/**
/ setPinState
/
/ Sets the specified pin the the given state.
**/
void setPinState(int pinNumber, int state){
	if (pinNumber != 99) // This is a kludge to access the HBW pin on the ATMEGA32U4 which is not defined in the arduino pins file.
		digitalWrite(pinNumber, state);
	else {
		int port_status = PINE;
		// TODO: OR current pin status then write new status.
		if (state == LOW)
			PORTE &= ~(1 << 2); // low
		else
			PORTE |= (1 << 2); // high
	}
}
