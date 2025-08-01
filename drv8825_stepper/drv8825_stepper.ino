void setup() {
    // put your setup code here, to run once:




    pinMode(9,OUTPUT);//M0
    pinMode(10,OUTPUT);//M1
    pinMode(11,OUTPUT);//M2

    pinMode(2,OUTPUT);//PUL
    pinMode(3,OUTPUT);//DIR
    pinMode(4,OUTPUT);//ENA
    //32 div
    digitalWrite(9,HIGH);
    digitalWrite(10,LOW);
    digitalWrite(11,HIGH);
    digitalWrite(4,LOW);

}
int x;
void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(3,LOW);
    /*
       for(x = 0; x < 200; x++) // Loop 200 times  
       {  
       digitalWrite(13,HIGH); // Output high  
       delayMicroseconds(800); // Wait 1/2 a ms  
       digitalWrite(13,LOW); // Output low  
       delayMicroseconds(800); // Wait 1/2 a ms  
       }  
       delay(1000); // pause one second  

       for(x = 0; x < 200; x++) // Loop 2000 times  
       {  
       digitalWrite(13,HIGH); // Output high  
       delayMicroseconds(800); // Wait 1/2 a ms  
       digitalWrite(13,LOW); // Output low  
       delayMicroseconds(800); // Wait 1/2 a ms  
       }  
       delay(1000); // pause one second  
     */
    if(x == 6400)
    {
        x = 0;
        delay(1000);
    }
    else
    {
        digitalWrite(2,HIGH); // Output high  
        delayMicroseconds(100); // Wait 1/2 a ms  
        digitalWrite(2,LOW); // Output low  
        delayMicroseconds(100); // Wait 1/2 a ms  
        x+=1;
    }

}
