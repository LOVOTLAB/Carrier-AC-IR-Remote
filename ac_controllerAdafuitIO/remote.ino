/*
 * 
Carrier A/C IR Remote Code Gen Ver 0.1 Beta
media artist collective LOVOT LAB
Auth : Hyuns Hong
Lisence : CC0

이 코드는 한국 캐이러 에어컨 CPV-Q0908D0 모델에서 테스트되었습니다.
한국 케리어 에어컨이며 다른 에어컨에서는 테스트되지 않았습니다.
이 코드가 다른 모델에서 동작한다면 info@lovot.co 로 알려주세요.

This code tested on Carrier A/C which model no. is CPV-Q0908D0
The A/C is korean ver of carrier A/C.
This code is not tested any other A/C.
Please email to info@lovot.co when this code works for your Carrier A/C
All temperature is in Celius

*/

void setRawData(int a, int b, int c) {
  rawData[0] = 8400;
  rawData[1] = 4150;

  for (int i = 0; i < 16; i++) {
    rawData[2 + i * 2] = 550;
    if (a & (1 << (15 - i))) rawData[3 + i * 2] = 1500;
    else rawData[3 + i * 2] = 500;
  }

  for (int i = 0; i < 16; i++) {
    rawData[2 + 32 + i * 2] = 550;
    if (b & (1 << (15 - i))) rawData[3 + 32 + i * 2] = 1500;
    else rawData[3 + 32 + i * 2] = 500;
  }

  for (int i = 0; i < 16; i++) {
    rawData[2 + 64 + i * 2] = 550;
    if (c & (1 << (15 - i))) rawData[3 + 64 + i * 2] = 1500;
    else rawData[3 + 64 + i * 2] = 500;
  }
}

void makeRemoteCode(int o, int m, int w, int t, int h, int v) {
  // o = 0:꺼짐 off 1:켜짐 on;
  // m = 0:쾌적 Comfort, 1:냉방 A/C, 2:난방 Heat, 3:송풍 Fan, 4:제습 Dehumidification;
  // w = 1~18 바람세기 Fan Speed
  // t = 17~32; 온도 Temperature
  // h = 0:좌우정지 Stop 1:좌우회전 Horizontal Turn 
  // v = 0:상하정지 Stop 1:상하회전 Vertical Turn

  int code1, code2, code3;
  int countOffset = 0;
  int windCheck = 0;

  // 쾌적인경우 온도 23~27 바람 최고
  // In mode 0, temp limits 23~27
  if (m == 0) { 
    if (t < 23) t = 23;
    else if (t > 27) t = 27;
    w = 0b11111;
  }
  // 냉난방인경우 바람 5 이상
  // In mode 1 or 2, Fan speed greater than 5  
  else if (m < 3) { 
    if (w < 5) w = 5;
  }
  // 송풍인경우 온도 24
  // In mode 3, temp set 24
  else if (m == 3) { 
    t = 24;
  }
  // 제습인경우
  // In mode 4
  else if ( m == 4) { 
    w = 0b11111;
  }
  // 나머지 17~32도 고정
  // else, temp limited to between 17 and 32 
  t = constrain(t, 17, 32);

  windCheck = ( w + t + h );
  countOffset = m * 2 - 2 + (v * 4) + 8 - (o * 8);

  code1 = ((count + 12) << 12) + ((m + 1) << 5) + h;
  code2 = 0x8000 + (w << 8) + t;
  code3 = (o << 15) | (v << 14) | ((((count + countOffset) << 4) + windCheck) & 0xFF);

  setRawData(code1, code2, code3);

  digitalWrite(D4, LOW);
  delay(50);
  digitalWrite(D4, HIGH);
  irsend.sendRaw(rawData, 99, 38);  // Send a raw data capture at 38kHz.
  delay(134);
  irsend.sendRaw(rawData, 99, 38);  // Send a raw data capture at 38kHz.
  delay(134);
  irsend.sendRaw(rawData, 99, 38);  // Send a raw data capture at 38kHz.

  count++;
  if (count >= 4) count = 0;
}
