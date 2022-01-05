# 1 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
# 2 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino" 2
# 3 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino" 2
# 4 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino" 2

volatile bool state;



void timerCallback() {
    state = !state;
    digitalWrite(12, state);
    timer1_write(5*1000 * 500);
}


void setup()
{
    Serial.begin(115200);
    pinMode(12, 0x01);

    timer1_attachInterrupt(timerCallback);
    timer1_enable(TIM_DIV16, 0, 0 /*on interrupt routine you need to write a new value to start the timer again*/);
    timer1_write(5*500); //500 us
}



void loop()
{
}
