#include <PAC1720.h>

PAC1720 Sense; //Instatiate sensor

unsigned long Period = 1000; //Wait 1 second between prints
// uint8_t ResVals[2] = {10, 5}; //Mapping of resistor values [mOhms]

void setup() {
	Serial.begin(38400); //Begin serial
	Sense.begin(); //Basic setup
	while(!Serial); //Wait for serial to start
	Serial.print("Fight the POWER!\n\n"); //Genaric Naughty By Nature 
}

void loop() {
	float Bus[2] = {0}; //Setup array for bus voltages
	float Current[2] = {0}; //Setup array for current values

	// String Data;
	char Data[100]; //String to concatonate data into for single print

	for(int i = 0; i < 2; i++) {
		Bus[i] = Sense.GetBusVoltage(i); //Get bus voltage
		Current[i] = Sense.GetSenseVoltage(i); //Get sensing voltage
		// Current[i] = Sense.GetCurrent(i, ResVals[i]); //Get calculated current
	}
	sprintf(Data, "Vbus1 = %dmV\tVsense1 = %+duV\tVbus2 = %dmV\tVsense2 = %+duV\n", int(Bus[0]*1000.0), int(Current[0]*1000.0), int(Bus[1]*1000.0), int(Current[1]*1000.0)); //Format string for sufficient resolution for each value and +/- indication for clarity on current 
	// sprintf(Data, "V1 = %.4fV\tI1 = %+.4fmA\tV2 = %.4fV\tI2 = %+.4fmA\n", Bus[0], Current[0], Bus[1], Current[1]); //Format string for sufficient resolution for each value and +/- indication for clarity on current 
	Serial.print(Data); //Print out preformatted data string
	// Serial.println(Current[0], 8); //DEBUG!
	delay(Period); //Wait a bit
}

