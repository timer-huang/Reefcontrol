
void initOutlets(){
  pinMode(jdqPin1,OUTPUT);//设定数字接口22、23、24、25为输出接口   
  pinMode(jdqPin2,OUTPUT);
  pinMode(jdqPin3,OUTPUT);
  pinMode(jdqPin4,OUTPUT);

  digitalWrite(jdqPin1,LOW); 
  digitalWrite(jdqPin2,LOW); 
  digitalWrite(jdqPin3,LOW); 
  digitalWrite(jdqPin4,LOW); 
}
