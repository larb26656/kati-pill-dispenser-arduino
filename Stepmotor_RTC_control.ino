#include <ArduinoJson.h>
#include <DS1302.h>

String savetxt = "";
int controlpin[8][4] {
    {11,10,9,8},
    {7,6,5,4},
    {3,2,1,0},
    {14,15,16,17},
    {26,27,28,29},
    {30,31,32,33},
    {34,35,36,37},
    {38,39,40,41}
};
int controlhalfstep[][4] = {
    {HIGH,LOW,LOW,HIGH},
    {HIGH,LOW,LOW,LOW},
    {HIGH,HIGH,LOW,LOW},
    {LOW,HIGH,LOW,LOW},
    {LOW,HIGH,HIGH,LOW},
    {LOW,LOW,HIGH,LOW},
    {LOW,LOW,HIGH,HIGH},
    {LOW,LOW,LOW,HIGH}
  };

namespace {
const int kCePin   = 18;
const int kIoPin   = 19;
const int kSclkPin = 20; 

DS1302 rtc(kCePin, kIoPin, kSclkPin);
String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

String printTime() {
  Time t = rtc.time();
  const String day = dayAsString(t.day);
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  return buf;
}
}

void setup() {
  Serial.begin(9600);
  rtc.writeProtect(true);
  rtc.halt(false);
  for(int num = 0; num < 8; num++) {
    for(int pin = 0; pin < 4; pin++) {
      pinMode(controlpin[num][pin], OUTPUT);
      //digitalWrite(controlpin[num][pin], LOW);
      //Serial.println(controlpin[num][pin]);
    }
  }
}

void loop() {
  if(Serial.available())
   {
     savetxt = Serial.readString();  
     if (get_json_root(String(savetxt))["type"]=="pill_dispenser")
        {
         int slot_num = get_json_root(String(savetxt))["slot_num"];
         int num_of_dispenser = get_json_root(String(savetxt))["num_of_dispenser"];
         pill_dispenser(slot_num,num_of_dispenser);
         Serial.println("success");
        }
     else if (get_json_root(String(savetxt))["type"]=="current_time")
        {
         Serial.println(printTime());
        }
   }
}

JsonObject& get_json_root(String json){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    return;
  }
  return root;
}

void pill_dispenser(int slot_num,int num_of_dispenser){
  for (int num = 0 ; num < num_of_dispenser; num++)
  {
    for (int i = 0; i < 32; i++)
    {
      for(int halfstep = 0; halfstep < 8; halfstep++) 
      {
        for(int pin = 0; pin < 4; pin++) 
        {
          digitalWrite(controlpin[slot_num-1][pin],controlhalfstep[halfstep][pin]);
        }
        delay(10);
      }
    }
     delay(500);
  }
}





