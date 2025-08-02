
// include libraries

#include <SPI.h>              
#include <LoRa.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
// i change this <Arduino_JSON.h> to this #include <ArduinoJson.h>
#include <ArduinoJson.h>


// define the pins used by the transceiver module
#define ss 4
#define rst 5
#define dio0 2

// define the WIFI 
// whifi configuration 
// const char *ssid = "Redmi 9";
// const char *password = "0557211669";
// const char *ssid = "TP-LINK_B29EDB";
// const char *password = "Dz47+@+@+@47";
const char *ssid = "NABTAKIR ASSOCIATION";
const char *password = "//**$$Nabtakir2023$$**//";

// declare endpoints
const char *apiSENDEndpoint = "http://agritech-dz.com/api/store";  // declare endpoint of sending data
const char *apiRECIVEEndpoint = "http://agritech-dz.com/api/send";  // declare endpoint of asking for data



int MAX_NODES = 2;  // Nodes Number
const int NumberGreenhouses = 2;

// const int numRows = 5 * NumberGreenhouses;
// const int numCols = 3;
// Create an array of node addresses
byte nodes_addresses[] = {0xBB, 0xCC /*, Add more addresses as needed */};
byte MasterNode = 0xFF;  // declare MasterNode Adress

// Structure to represent a node
struct Node {
  byte address;
  String name;
  String message;
};

// Create an array of nodes and initialize it with addresses
std::vector<Node> nodes;


String SenderNode = "";
String outgoing;              // outgoing message
byte Node2 = 0xCC;
byte Node1 = 0xBB;
byte msgCount = 0;            // count of outgoing messages

// Tracks the time since last event fired
unsigned long previousMillis=0;
unsigned long int previoussecs = 0; 
unsigned long int currentsecs = 0; 
unsigned long currentMillis = 0;
int interval= 1 ; // updated every 1 second
int Secs = 0; 
int stop = 0;
static bool wait = false;
int node_duration = 5 ;
int max_sescond = node_duration * MAX_NODES;
int node_index = 0;
int POSE_PERIOD = 10;
int NumberOfGreenhouses = 2;
// DynamicJsonDocument doc(1024);
StaticJsonDocument<1024> doc;


JsonArray previousData;
JsonArray updatedData;

static bool begin = true;

// Sending Data Function

// typedef struct {
//   int greenhouse_id;
//   char actuator[10]; // Adjust the size according to your actual requirements
//   double is_active;
// } RowData;
// // Declare and initialize a 1D array of RowData
// RowData AllActuators[numRows] = {
//   {1, "heater", 0.0},
//   {1, "water", 0.0},
//   {1, "fan", 1.0},
//   {1, "pump", 1.0},
//   {1, "window", 0.0},
//   {2, "heater", 0.0},
//   {2, "water", 0.0},
//   {2, "fan", 1.0},
//   {2, "pump", 0.0},
//   {2, "window", 0.0}
// };
// RowData MyActuators[numRows] = {
//   {1, "heater", 0.0},
//   {1, "water", 0.0},
//   {1, "fan", 1.0},
//   {1, "pump", 1.0},
//   {1, "window", 0.0}
// };


void sendToPlatform(String var1 = "", String var2 = "", String var3 = "", String var4 = "", String var5 = "" , String greenhouseName = "") {
  // Create JSON payload
  String payload = "{\"temperature\": " + var1 +
                   ", \"humidity\": " + var2 +
                   ", \"co2\": " + var3 +
                   ", \"soil_humidity\": " + var4 +
                   ", \"wind_velocity\": " +var5 +
                   ", \"greenhouse_name\": \"" + greenhouseName + "\"}";

Serial.println("payload = "  + payload);
    // Make HTTP POST request to the Laravel API
    HTTPClient http;
    http.begin(apiSENDEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-SECRET", "sOkd5mW8cs5Xw7eRbyrUcG0yi6k2nzMghdNbfCHpiXALeApU6LDAx4kn9iR9Lid6");
    http.addHeader("X-API-KEY", "8Ft7AVb17uKFcv252jHpQYwtBOZsnS9a");

    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        Serial.println(httpCode);
        String response = http.getString();

          Serial.println(response);
          Serial.println("-------------------------");
        
    } else {
        Serial.println("HTTP request failed");
    }

    http.end();

    return;
}

void printJsonArray(DynamicJsonDocument& doc) {
    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        Serial.print("Greenhouse ID: ");
        Serial.println(obj["greenhouse_id"].as<String>());
        Serial.print("Actuator: ");
        Serial.println(obj["actuator"].as<String>());
        Serial.print("Is Active: ");
        Serial.println(obj["is_active"].as<String>());
        Serial.println("-------------------------");
    }
}




void setup() {

initializeNodes();    // initialize Nodes


Serial.begin(9600);                   // initialize serial

while (!Serial);
 // Connect to Wi-Fi
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
}

Serial.println("Connected to WiFi");

LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  // LoRa.setSpreadingFactor(12);
  // LoRa.setSignalBandwidth(125E3);
  
  Serial.println("LoRa init succeeded.");
// get actutaors status 

// updatedData = getFromPlatform();
// previousData = updatedData;


}



void loop() {

currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    
    if(!wait){
      Secs = Secs + 1;
    
     //Serial.println(Secs);
     if ( Secs >= max_sescond )
    {
      Secs = 0; 
      wait = true;
      Serial.println("We are Waiting ....");


    }

  for (int node_index = 0; node_index <  MAX_NODES ; node_index++){

    if ( (Secs >= node_index * node_duration + 1 ) && (Secs <= node_duration * (node_index + 1)) )
    {
     
    String message = nodes[node_index].message; 
    Serial.println("message");
    Serial.println(message);

    sendMessage(message,MasterNode, nodes[node_index].address);// Get data from the platform

    
    Serial.println("Ask the data from " + nodes[node_index].name + " with Address = ");
    Serial.print(nodes[node_index].address,HEX);

    }

  }
    

    }else{
      stop = stop + 1 ;

      // if(stop % 5 == 0 ){

      //     // Get data from the platform
      //     String update = getFromPlatform();

      //     // Print the current update
      //     Serial.println("Current Update:");
      //     if(update != "1"){

      //     String messageA = cleanString(update);
      //     Serial.println(messageA);
      //     all_actuators(messageA);
      //     for (int Greenhouse_Id = 1;Greenhouse_Id<3;Greenhouse_Id++){
      //                   int j=0;
                            
      //                     RowData MyActuators[numRows] = {}; // Initialize to zeros
      //                     String actuator_states= "";

      //                 for (int i = 0; i < numRows; i++) {
      //                       Serial.print("Row ");
      //                       Serial.print(i + 1);
      //                       Serial.print(": ");
      //                       Serial.print("ID: ");
      //                       Serial.print(AllActuators[i].greenhouse_id);
      //                       Serial.print(", Actuator: ");
      //                       Serial.print(AllActuators[i].actuator);
      //                       Serial.print(", Status: ");
      //                       Serial.println(AllActuators[i].is_active);
            
      //                       if(Greenhouse_Id == AllActuators[i].greenhouse_id){
      //                             MyActuators[j].greenhouse_id =  AllActuators[i].greenhouse_id;
      //                             strncpy(MyActuators[j].actuator, AllActuators[i].actuator, sizeof(MyActuators[j].actuator));
      //                             MyActuators[j].is_active =    AllActuators[i].is_active;
      //                             actuator_states =  actuator_states + String(MyActuators[j].greenhouse_id)  + "," + strncpy(MyActuators[j].actuator, AllActuators[i].actuator, sizeof(MyActuators[j].actuator)) +","+ String(MyActuators[j].is_active);     
      //                             j++;
      //                       }
      //                 }
      //                 Serial.println("actuator_states = ");
      //                 Serial.println(actuator_states);
      //                 sendMessage(actuator_states,MasterNode,nodes_addresses[Greenhouse_Id]);
      //     }

      //     }else if(update == "automatic"){
      //       sendMessage(update,MasterNode,0xFF);///////////ALL GREENHOUSE HAS THE SAME STATE AUTO/MANUEL

      //     }else{
      //       Serial.println("No changes");

      //     }

      // }

      // if(stop % 5 == 0 ){

      //     // Get data from the platform
      //     String update = getFromPlatform();

      //     // Print the current update
      //     Serial.println("Current Update:");
      //     if(update != "1"){

      //     String messageA = cleanString(update);
      //     Serial.println(messageA);
      //     sendMessage(messageA,MasterNode,0xBB);

      //     }else if(update == "automatic"){
      //       sendMessage(update,MasterNode,0xBB);

      //     }else{
      //       Serial.println("No changes");

      //     }

      // }
    }


    
    if(stop >= POSE_PERIOD){
      wait = false;
      stop = 0;
    }

    
    previoussecs = currentsecs;


    }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
    
  }


void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address


  String SenderNode = "";               // Initialize SenderNode

  // Check the sender address against dynamically created nodes
  for (int i = 0; i < MAX_NODES; i++) {
    if (sender == nodes[i].address) {
      SenderNode = "Node" + String(i + 1) + ":";
      Serial.println("SenderNode" + SenderNode);
      break;  // Exit the loop once the matching node is found
    }
  }


  if (SenderNode == "") {
    // Handle the case when the sender is not found in the nodes array
    Serial.println("Unknown sender address");
    return;
  }

  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();

  }

  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   ;
  //   return;                             // skip rest of function
  // }

  // if the recipient isn't this device or broadcast,
  // if (recipient != Node1 && recipient != MasterNode) {
  //  Serial.println("This message is not for me.");
  //   ;
  //   return;                             // skip rest of function
  // }

  // if message is for this device, or broadcast, print details:
//   Serial.println("Received from: 0x" + String(sender, HEX));
//   Serial.println("Sent to: 0x" + String(recipient, HEX));
//   Serial.println("Message ID: " + String(incomingMsgId));
//  Serial.println("Message length: " + String(incomingLength));
//  Serial.println("Message: " + incoming);
//   Serial.println("RSSI: " + String(LoRa.packetRssi()));
//  Serial.println("Snr: " + String(LoRa.packetSnr()));
//  Serial.println();


  // Serial.println(SenderNode);
  Serial.println("incoming = " +incoming);
 
 // get the data
 String parameters[8];  // Assuming there are 7 parameters
  int startIndex = 0;
  int endIndex = incoming.indexOf(',');
  for (int i = 0; i < 8; i++) {
    parameters[i] = incoming.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
    endIndex = incoming.indexOf(',', startIndex);
    if (endIndex == -1) {
      endIndex = incoming.length();
    }
    Serial.println(parameters[i]);
  }

String var1 , var2 , var3,  var4 , var5 , var6 ;
String var_help1,var_help2;
var1 = parameters[0];
var2 = parameters[1];
var3 = parameters[2];
var4 = parameters[3];
var5 = parameters[4];
String  greenhouseName = "";
greenhouseName = parameters[7];

Serial.println("var1 : " + var1);
Serial.println("var2 : " + var2);
Serial.println("var3 : " + var3);
Serial.println("var4 : " + var4);
Serial.println("var5 : " + var5);

Serial.println("greenhouseName : " + greenhouseName);
sendToPlatform(var1 , var2 , var3,  var4 , var5, greenhouseName) ;

}


String getFromPlatform() {
    HTTPClient http;
    http.begin(apiRECIVEEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-SECRET", "sOkd5mW8cs5Xw7eRbyrUcG0yi6k2nzMghdNbfCHpiXALeApU6LDAx4kn9iR9Lid6");
    http.addHeader("X-API-KEY", "8Ft7AVb17uKFcv252jHpQYwtBOZsnS9a");

    int httpCode = http.POST("{}"); // Empty JSON payload


    DynamicJsonDocument doc(1024);

    if (httpCode > 0) {
       String response = http.getString();
        Serial.println(response);
        if (response.indexOf("automatic") != -1) {
          return "automatic";
        }
        // Find the position of the opening brace '{' of the inner JSON array
        size_t startPos = response.indexOf("[");
        // Find the position of the closing brace '}' of the inner JSON array
        size_t endPos = response.lastIndexOf("]") + 1;


        if(startPos >= 4290002295){
          return "1";
        }


        // Extract the inner JSON array from the response string
        String jsonData = response.substring(startPos, endPos);
      
       
        return jsonData;
    } else {
        Serial.println("HTTP request failed");
        getFromPlatform();
    }

    http.end();
    return "error";
}



// Initialize Nodes Function

void initializeNodes() {

  nodes.resize(MAX_NODES);

  //   // Initialize nodes with addresses
  for (int i = 0; i < MAX_NODES; i++) {
    nodes[i].address = nodes_addresses[i];
    nodes[i].name = "Node"+ String(i+1);
    nodes[i].message = String(i+1);
    // Add other initialization for the nodes as needed
  }

return;

}

String cleanString(String data){
              data.replace("\\\"", "\""); // Remove escaped double quotes

        data.replace("\"greenhouse_id\":\"", "");
                  // Serial.println(update);

        data.replace("\",\"actuator\":\"", ",");
          // Serial.println(update);

        data.replace("\",\"is_active\":\"", ",");
          // Serial.println(update);

        data.replace("\"},", ";");
        data.replace("\"}", "");
        data.replace("{", "");
        data.replace("[", "");
        data.replace("]", "");
        return data;
}
// void all_actuators( String jsonString){
//    // Split the string into individual rows
//         String rows[numRows];
//         int startIndex = 0;
//         for (int i = 0; i < numRows; i++) {
//           int endIndex = jsonString.indexOf(';', startIndex);
//           if (endIndex == -1) {
//             rows[i] = jsonString.substring(startIndex);
//           } else {
//             rows[i] = jsonString.substring(startIndex, endIndex);
//           }
//           startIndex = endIndex + 1;
//         }

//         // Populate the AllActuators array
//         for (int i = 0; i < numRows; i++) {
//           int id = rows[i].substring(0, rows[i].indexOf(',')).toInt();
//           rows[i] = rows[i].substring(rows[i].indexOf(',') + 1);
//           String actuator = rows[i].substring(0, rows[i].indexOf(','));
//           double status = rows[i].substring(rows[i].indexOf(',') + 1).toDouble();

//           AllActuators[i].greenhouse_id = id;
//           strncpy(AllActuators[i].actuator, actuator.c_str(), sizeof(AllActuators[i].actuator));
//           AllActuators[i].is_active = status; 
//         }

//         // // Print the populated myTable array for verification
//         // Serial.println("Populated AllActuators array:");
       
       
// }



