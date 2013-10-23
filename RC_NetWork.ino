byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED    };
IPAddress ip(LOCAL_IP);

boolean netCheck() {
  static IPAddress router(ROUTER_IP); //change the ip to your router ip address
  EthernetClient client;
  if (client.connect(router,ROUTER_PORT)) {
    client.stop();
    return true;
  } else {
    resetNetwork();
    return false;
  }
}

void resetNetwork() {
  Ethernet.begin(mac,ip);
  //web.begin();
  //chirp();
  Serial.println("Ethernet and Webserver reset.");
}

void initNetwork() {
  //init ethernet shield
  Ethernet.begin(mac,ip);
  initClock();
  Serial.println("System initializing...");
  if (netCheck()) {
    Serial.println("Network OK.     ");
    //initWebServer();
    Serial.println("Webserver Up.   ");
    Serial.println("Network initialized.");
  } else {
     Serial.println("Network Down.   "); 
  }
}



////////////////////////////////////
//  时钟
///////////////////////////////////
boolean isDst = false;
IPAddress timeServer(NTPSERVER);
EthernetUDP Udp;

time_t getntp(){
  long start = millis();
  time_t ntptime = getNtpTime(Udp, timeServer);
  char buf[20];
  Serial.println("Time NTP sync ");
  long elapsed = millis() - start;
  Serial.println("Time to execute sync ");
  return ntptime;
}

void initClock() {
  unsigned int localPort = 7777;      // local port to listen for UDP packets
  Udp.begin(localPort);
  unsigned long ntptime = getNtpTime(Udp, timeServer); //get standard time
  if (ntptime>0) {
    setTime(ntptime);
    if (IsDST(ntptime)) 
    {
      //dstoffset = STDTZOFFSET+1L; //dst
      isDst = true;
      ntptime += SECS_PER_HOUR;
      setTime(ntptime);  //adjust to dst
    }
    Serial.println("Got NTP time ");
    Serial.println("NTP sync OK.    ");
    char buf[20];
    Serial.print("NTP time is ");
    Serial.println(getdatestring(ntptime,buf));
    //check if RTC is present
    Wire.beginTransmission(RTC_ADDR);
    uint8_t error = Wire.endTransmission();
    if (!error) {
      RTC.set(ntptime);
      unsigned long rtctime = RTC.get();
      Serial.print("RTC time");
      Serial.println(getdatestring(rtctime,buf));
      Serial.println("Using RTC as sync provider.");
      setSyncProvider(RTC.get);
      Serial.println("RTC time OK.    ");
    } else {
      Serial.println("RTC is not present.");
    }
  } else {
    Wire.beginTransmission(RTC_ADDR);
    uint8_t error = Wire.endTransmission();
    if (!error) {
      char buf[20];
      unsigned long rtctime = RTC.get();
      setTime(rtctime);
      Serial.println("Get ntp failed.");
      Serial.println("Using RTC as sync provider.");
      Serial.print("RTC time");
      Serial.println(getdatestring(rtctime,buf));
      setSyncProvider(RTC.get);
      Serial.println("Using RTC time. ");
    } else {
     // beepFail();  
      for(;;);
    }
  }
  tz = IsDST(now())?-7:-8;
}

void autoDST(time_t t) {
  if (IsDST(t)==isDst) return;
  isDst = !isDst;
  if (isDst)
    RTC.set(t+SECS_PER_HOUR);
  else
    RTC.set(t-SECS_PER_HOUR);
  setTime(RTC.get());
  Serial.println("Auto adjusted DST time.");
}

boolean IsDST(time_t t)
{
  tmElements_t te;
  te.Year = year(t)-1970;
  te.Month =3;
  te.Day =1;
  te.Hour = 0;
  te.Minute = 0;
  te.Second = 0;
  time_t dstStart,dstEnd, current;
  dstStart = makeTime(te);
  //dstStart = nextSunday(dstStart);
  dstStart = dstStart + 7;//nextSunday(dstStart); //second sunday in march
  dstStart += 2*SECS_PER_HOUR;//2AM
  te.Month=11;
  dstEnd = makeTime(te);
  dstEnd = dstEnd + 7; //nextSunday(dstEnd); //first sunday in november
  dstEnd += SECS_PER_HOUR; //1AM
  return (t>=dstStart && t<dstEnd);
}

unsigned long getNtpTime(EthernetUDP Udp, IPAddress timeServer)
{

  const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 


  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  //    Udp.beginPacket(timeServer, 123); //NTP requests are to port 123
  //    Udp.write(packetBuffer,NTP_PACKET_SIZE);
  //    Udp.endPacket(); 

  // wait to see if a reply is available
  int retries = 3;
  while (retries>0) {
    Udp.beginPacket(timeServer, 123); //NTP requests are to port 123
    Udp.write(packetBuffer,NTP_PACKET_SIZE);
    Udp.endPacket(); 
    delay(500);  
    if ( Udp.parsePacket() ) {  
      // We've received a packet, read the data from it
      Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
      unsigned long seventy_years = 2208988800UL - (STDTZOFFSET * 60 * 60);        
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;  
      return secsSince1900 -  seventy_years;    
    }
    //      Serial.print("Retries ");Serial.println(retries);
    retries--;
  }
  return 0; // return 0 if unable to get the time
}
