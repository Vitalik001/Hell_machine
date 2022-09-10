#include <SoftwareSerial.h>

SoftwareSerial bluetooth_module(51, 49); //51 49 RX | TX pins for bluetooth module.

const int BLUETOOTH_BAUDRATE = 9600;
const int MASTER_DEVICE_BAUDRATE = 9600;

/*
Initialising constants responsible for engine speed and direction control pins 
*/

const int ENGINE1_SPEED_CONTROL_PIN=A1;
const int ENGINE2_SPEED_CONTROL_PIN=A2;
//const int ENGINE3_SPEED_CONTROL_PIN=A3;
//const int ENGINE4_SPEED_CONTROL_PIN=A4;

const int TURNING_PIN_2=12;
const int TURNING_PIN_1=13;
const int V_5=11;
const int V_5_1=53;


const int ENGINE1_DIRECTION_CONTROL_PIN=6;
const int ENGINE2_DIRECTION_CONTROL_PIN=5;
//const int ENGINE3_DIRECTION_CONTROL_PIN=4;
//const int ENGINE4_DIRECTION_CONTROL_PIN=3;

const int MOTOR_SPEED_0 = 0;
const int MOTOR_SPEED_1 = 130;
const int MOTOR_SPEED_2 = 150;
const int MOTOR_SPEED_3 = 170;

char current_command = ' '; // Contains a command symbol received with bluetooth.

int current_motor_speed = MOTOR_SPEED_1;

/* 
Initialising max_loops_without_command which contains a maximum number of loops with unavailable bluetooth module.
If it is exceeded - we assume that connection has been lost, so we should stop a car for safety.
*/
int max_loops_without_command = 0;
int loops_without_command = 0;
int total_commands_received = 0;
int time_without_command = 0;

long long int last_received_signal_timestamp = 0; // Another way to ensure we have an established connection.

bool connection_lost = false; // If connection is lost, we stop a car.

void setup() {

  pinMode(ENGINE1_SPEED_CONTROL_PIN, OUTPUT);
  pinMode(ENGINE2_SPEED_CONTROL_PIN, OUTPUT);
//  pinMode(ENGINE3_SPEED_CONTROL_PIN, OUTPUT);
//  pinMode(ENGINE4_SPEED_CONTROL_PIN, OUTPUT);

  pinMode(TURNING_PIN_1, OUTPUT);
  pinMode(TURNING_PIN_2, OUTPUT);
  
  pinMode(V_5, OUTPUT);
  pinMode(V_5_1, OUTPUT);
  digitalWrite(V_5, HIGH);
  digitalWrite(V_5_1, HIGH);

  pinMode(ENGINE1_DIRECTION_CONTROL_PIN, OUTPUT);
  pinMode(ENGINE2_DIRECTION_CONTROL_PIN, OUTPUT);
//  pinMode(ENGINE3_DIRECTION_CONTROL_PIN, OUTPUT);
//  pinMode(ENGINE4_DIRECTION_CONTROL_PIN, OUTPUT);
  
  Serial.begin(MASTER_DEVICE_BAUDRATE);
  bluetooth_module.begin(BLUETOOTH_BAUDRATE);

  begin_bluetooth_connection(); // Waits until bluetooth connection is established.
}


void loop()
{
  if (!connection_lost){
    loops_without_command+=1;
    default_mode();
  }
  else{
    stop();
    Serial.print("Connection has been lost!\n");
    delay(3000);
  }
}

void default_mode(){
  receive_command();
  check_connection();
  execute_command();
}


void begin_bluetooth_connection(){
  while (!(bluetooth_module.available() > 0)){
    delay(100);
  }
  Serial.print("Connection established.\n");
}


void receive_command(){
  if (bluetooth_module.available()>0){
    current_command = bluetooth_module.read();

    last_received_signal_timestamp = millis();

    max_loops_without_command=max(max_loops_without_command, loops_without_command*30);
    loops_without_command=0;

    total_commands_received++;


    /*Serial.print(current_command);
    Serial.print("\nLoops before command: ");
    Serial.print(loops_without_command);
    Serial.print("\n");*/
  }
}


void check_connection(){
  time_without_command = millis() - last_received_signal_timestamp;

  if (time_without_command > 300){
    current_command=' ';
    connection_lost = true;
    stop();

    Serial.print("Disbanding connection: more than 0.3 second without new commands.\n");
  }
}


void execute_command(){
  Serial.print(current_command);
  switch (current_command)
  {
    case 'F' : {
      move_forward();
      digitalWrite(TURNING_PIN_1, LOW);
      digitalWrite(TURNING_PIN_2, LOW);
    }
    break;
    case 'B' : {
      move_backward();
      digitalWrite(TURNING_PIN_1, LOW);
      digitalWrite(TURNING_PIN_2, LOW);
    }
    break;
    case 'L' : {
      turn_left();
    }
    break;
    case 'R' : {
      turn_right();
    }
    break;
    case 'I' : {
      forward_right();
    }
    break;
    case 'J' : {
      backward_right();
    }
    break;
    case 'G' : {
      forward_left();
    }
    break;
    case 'H' : {
      backward_left();
    }
    break;
    default: {
      stop();
    }
    break;
  }

  }


  void move_forward(){
      analogWrite(ENGINE1_SPEED_CONTROL_PIN, current_motor_speed);
      analogWrite(ENGINE2_SPEED_CONTROL_PIN, current_motor_speed);

      digitalWrite(ENGINE1_DIRECTION_CONTROL_PIN, LOW);
      digitalWrite(ENGINE2_DIRECTION_CONTROL_PIN, HIGH);
  }


  void move_backward(){
      analogWrite(ENGINE1_SPEED_CONTROL_PIN, current_motor_speed);
      analogWrite(ENGINE2_SPEED_CONTROL_PIN, current_motor_speed);

      digitalWrite(ENGINE1_DIRECTION_CONTROL_PIN, HIGH);
      digitalWrite(ENGINE2_DIRECTION_CONTROL_PIN, LOW);
  }


  void turn_right(){
      digitalWrite(TURNING_PIN_1, HIGH);
      digitalWrite(TURNING_PIN_2, LOW);
  }


  void turn_left(){
      digitalWrite(TURNING_PIN_2, HIGH);
      digitalWrite(TURNING_PIN_1, LOW);
  }

  void forward_right()
    {
      turn_right();
      move_forward();
    }

    
 void backward_right()
  {
    turn_right();
    move_backward();
  }

  void forward_left()
    {
      turn_left();
      move_forward();
    }

   void backward_left()
    {
      turn_left();
      move_backward();
    }


  void stop(){
      analogWrite(ENGINE1_SPEED_CONTROL_PIN, MOTOR_SPEED_0);
      analogWrite(ENGINE2_SPEED_CONTROL_PIN, MOTOR_SPEED_0);
      digitalWrite(TURNING_PIN_1, LOW);
      digitalWrite(TURNING_PIN_2, LOW);
  }
