#define CONTROLLER_NAME "Timer's Reef" //change this to your controller name
#define NTPSERVER 193,193,193,107  //pool.ntp.org
#define LOCAL_IP 192,168,1,208 //change this to a local fixed ip address
#define ROUTER_IP 192,168,1,1 //change this to your router ip address
#define ROUTER_PORT 80 //usually port 80 for the ip configuration web page

#define WEBSERVERPORT 8000
//go to http://base64-encoder-online.waraxe.us
//to create your encoded string
//BASICAUTH = username:password,  sample below is admin:password
//SMTPUSER = your email login, sample below is admin
//SMTPPASSWORD = your email password, sample below is password
#define BASICAUTH "YWRtaW46cGFzc3dvcmQ="
#define SMTPSERVER "my.smtp.net"
#define SMTPPORT 587
#define SMTPUSER "YWRtaW4="
#define SMTPPASSWORD "cGFzc3dvcmQ="
#define EMAILFROM "user@host.com"
#define EMAILTO "8005551212@txt.att.net"
#define SD_CS 4
#define ETHER_CS 10
//时钟芯片DS3231
#define RTC_ADDR 104

#define STDTZOFFSET -8L
