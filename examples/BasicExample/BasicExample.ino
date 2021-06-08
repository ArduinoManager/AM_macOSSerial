/*
* Test Arduino Manager for iPad / iPhone / Mac
 *
 * A simple test program to show the Arduino Manager
 * features.
 *
 * Author: Fabrizio Boco - fabboco@gmail.com
 *
 * Version: 1.0
 *
 * 06/07/2021
 *
 * All rights reserved
 *
 */

/*
*
 * AMController library, example sketches (“The Software”) and the related documentation (“The Documentation”) are supplied to you 
 * by the Author in consideration of your agreement to the following terms, and your use or installation of The Software and the use of The Documentation 
 * constitutes acceptance of these terms.  
 * If you do not agree with these terms, please do not use or install The Software.
 * The Author grants you a personal, non-exclusive license, under author's copyrights in this original software, to use The Software. 
 * Except as expressly stated in this notice, no other rights or licenses, express or implied, are granted by the Author, including but not limited to any 
 * patent rights that may be infringed by your derivative works or by other works in which The Software may be incorporated.
 * The Software and the Documentation are provided by the Author on an "AS IS" basis.  THE AUTHOR MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE SOFTWARE OR ITS USE AND OPERATION 
 * ALONE OR IN COMBINATION WITH YOUR PRODUCTS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, 
 * REPRODUCTION AND MODIFICATION OF THE SOFTWARE AND OR OF THE DOCUMENTATION, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), 
 * STRICT LIABILITY OR OTHERWISE, EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <SPI.h>
#include <Servo.h>
#include <SD.h>
#include <AM_macOSSerial.h>

/**
 *
 * Other initializations
 *
 */
#define YELLOWLEDPIN 8
int yellowLed = HIGH;

#define TEMPERATUREPIN 0
float temperature;

#define LIGHTPIN 1
int light;

#define SERVOPIN 9
Servo servo;
int servoPos;


#define REDLEDPIN 6
int connectionLed = LOW;

#define POTENTIOMETERPIN 2
int pot;

/*
 *
 * Prototypes of IOSController’s callbacks
 *
 *
 */
void doWork();
void doSync();
void processIncomingMessages(char *variable, char *value);
void processOutgoingMessages();
void processAlarms(char *variable);


/*
 *
 * IOSController Library initialization
 *
 */
#ifdef ALARMS_SUPPORT 
  AMController amController(&doWork,&doSync,&processIncomingMessages,&processOutgoingMessages,&processAlarms);
#else
  AMController amController(&doWork,&doSync,&processIncomingMessages,&processOutgoingMessages);
#endif

void setup() {
  Serial.begin(9600);
  
  /**
   *
   * Other initializations
   *
   */
   
  // Yellow LED on
  pinMode(YELLOWLEDPIN,OUTPUT);
  digitalWrite(YELLOWLEDPIN,yellowLed);


  // Servo position at 90 degrees 
  servo.attach(SERVOPIN);
  servoPos = 90;
  servo.write(servoPos);

  // Red LED OFF
  pinMode(REDLEDPIN,OUTPUT);
  digitalWrite(REDLEDPIN,connectionLed);
}


/**
 * 
 * Standard loop function
 *
 */
void loop() {
  amController.loop(500);
}

/**
*
*
* This function is called periodically and its equivalent to the standard loop() function
*
*/
void doWork() {

  temperature = getVoltage(TEMPERATUREPIN);  //getting the voltage reading from the temperature sensor
  temperature = (temperature - 0.5) * 100;  // converting from 10 mv per degree with 500 mV offset
                                            // to degrees ((voltage – 500mV) times 100  
  digitalWrite(YELLOWLEDPIN,yellowLed);                                            

  servo.write(servoPos);

  light = analogRead(LIGHTPIN);
  
  pot = analogRead(POTENTIOMETERPIN);
}

/**
*
*
* This function is called when the ios device connects and needs to initialize the position of switches and knobs
*
*/
void doSync () {
	amController.writeMessage("Knob1",map(servo.read(),0,180,0,1023));
  amController.writeMessage("S1",digitalRead(YELLOWLEDPIN));
  amController.writeTxtMessage("Msg","Hello, I'm your Arduino board");
}

/**
*
*
* This function is called when a new message is received from the iOS device
*
*/
void processIncomingMessages(char *variable, char *value) {
    
  if (strcmp(variable,"S1")==0) {

    yellowLed = atoi(value);
  }

  if (strcmp(variable,"Knob1")==0) {

      servoPos = atoi(value);  
      servoPos = map(servoPos, 0, 1023, 0, 180);      
  }

  if (strcmp(variable,"Push1")==0) {
        
    amController.logLn("Push1");
    amController.temporaryDigitalWrite(REDLEDPIN,HIGH,500);
  }

  if (strcmp(variable,"Cmd_01")==0) {
    
    amController.log("Command: ");
    amController.logLn(value);    
  } 
  
  if (strcmp(variable,"Cmd_02")==0) {
    
    amController.log("Command: ");
    amController.logLn(value);    
  } 

}

/**
*
*
* This function is called periodically and messages can be sent to the iOS device
*
*/
void processOutgoingMessages() {
  
  amController.writeMessage("T",temperature);

  amController.writeMessage("L",light);

  amController.writeMessage("Led13",yellowLed);
  
  amController.writeMessage("Pot",pot);
}

/**
*
*
* This function is called when an Alarm is fired
*
*/
void processAlarms(char *alarm) {

  amController.log("Alarm ");
  amController.log(alarm);
  amController.logLn(" fired");
  servoPos = 0;
}

/**
*  
* Auxiliary functions
*
*/

/*
 * getVoltage() – returns the voltage on the analog input defined by pin
 * 
 */
float getVoltage(int pin) {

  return (analogRead(pin) * .004882814);  // converting from a 0 to 1023 digital range
                                          // to 0 to 5 volts (each 1 reading equals ~ 5 millivolts
}