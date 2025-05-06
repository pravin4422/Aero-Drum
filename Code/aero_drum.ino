#include <Wire.h>

const int MPU1_ADDR = 0x68;
const int MPU2_ADDR = 0x69;

int16_t acc1X, acc1Y, acc1Z;
int16_t acc2X, acc2Y, acc2Z;

int16_t baseline1X, baseline1Y, baseline1Z;
int16_t baseline2X, baseline2Y, baseline2Z;

int sampleCounter = 0;
const int hitThreshold = 5000;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  initMPU(MPU1_ADDR);
  initMPU(MPU2_ADDR);
  calibrateMPU(MPU1_ADDR, &baseline1X, &baseline1Y, &baseline1Z);
  calibrateMPU(MPU2_ADDR, &baseline2X, &baseline2Y, &baseline2Z);
  Serial.println("READY");
}

void initMPU(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Wire.beginTransmission(address);
  Wire.write(0x1C);
  Wire.write(0x08);
  Wire.endTransmission(true);
}

void calibrateMPU(int address, int16_t *baseX, int16_t *baseY, int16_t *baseZ) {
  long sumX = 0, sumY = 0, sumZ = 0;
  int16_t x, y, z;
  for (int i = 0; i < 10; i++) {
    readAcceleration(address, &x, &y, &z);
    sumX += x;
    sumY += y;
    sumZ += z;
    delay(50);
  }
  *baseX = sumX / 10;
  *baseY = sumY / 10;
  *baseZ = sumZ / 10;
}

void readAcceleration(int address, int16_t *x, int16_t *y, int16_t *z) {
  Wire.beginTransmission(address);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(address, 6, true);
  *x = Wire.read() << 8 | Wire.read();
  *y = Wire.read() << 8 | Wire.read();
  *z = Wire.read() << 8 | Wire.read();
}

void loop() {
  readAcceleration(MPU1_ADDR, &acc1X, &acc1Y, &acc1Z);
  readAcceleration(MPU2_ADDR, &acc2X, &acc2Y, &acc2Z);

  int xDiff1 = abs(acc1X - baseline1X);
  int yDiff1 = abs(acc1Y - baseline1Y);
  int zDiff1 = abs(acc1Z - baseline1Z);

  int xDiff2 = abs(acc2X - baseline2X);
  int yDiff2 = abs(acc2Y - baseline2Y);
  int zDiff2 = abs(acc2Z - baseline2Z);

  if (xDiff1 > hitThreshold && xDiff1 >= yDiff1 && xDiff1 >= zDiff1) {
    Serial.println("MPU1_X");
    delay(200);
  } else if (yDiff1 > hitThreshold && yDiff1 >= xDiff1 && yDiff1 >= zDiff1) {
    Serial.println("MPU1_Y");
    delay(200);
  } else if (zDiff1 > hitThreshold && zDiff1 >= xDiff1 && zDiff1 >= yDiff1) {
    Serial.println("MPU1_Z");
    delay(200);
  }

  if (xDiff2 > hitThreshold && xDiff2 >= yDiff2 && xDiff2 >= zDiff2) {
    Serial.println("MPU2_X");
    delay(200);
  } else if (yDiff2 > hitThreshold && yDiff2 >= xDiff2 && yDiff2 >= zDiff2) {
    Serial.println("MPU2_Y");
    delay(200);
  } else if (zDiff2 > hitThreshold && zDiff2 >= xDiff2 && zDiff2 >= yDiff2) {
    Serial.println("MPU2_Z");
    delay(200);
  }

  sampleCounter++;
  if (sampleCounter >= 500) {
    if (xDiff1 < hitThreshold / 2 && yDiff1 < hitThreshold / 2 && zDiff1 < hitThreshold / 2) {
      baseline1X = acc1X * 0.1 + baseline1X * 0.9;
      baseline1Y = acc1Y * 0.1 + baseline1Y * 0.9;
      baseline1Z = acc1Z * 0.1 + baseline1Z * 0.9;
    }
    if (xDiff2 < hitThreshold / 2 && yDiff2 < hitThreshold / 2 && zDiff2 < hitThreshold / 2) {
      baseline2X = acc2X * 0.1 + baseline2X * 0.9;
      baseline2Y = acc2Y * 0.1 + baseline2Y * 0.9;
      baseline2Z = acc2Z * 0.1 + baseline2Z * 0.9;
    }
    sampleCounter = 0;
  }

  delay(10);
}
