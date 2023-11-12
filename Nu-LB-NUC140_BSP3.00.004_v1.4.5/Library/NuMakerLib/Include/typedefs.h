
typedef struct {
  int x;
  int y;
  int z;
} tAxisB;

typedef struct {
  int x;
  int y;
  int z;
} tAxis;

typedef struct {
  float x;
  float y;
  float z;
} tAxisF;

typedef struct {
  int Heading;
  int Roll;
  int Pitch;
} eAxis;



typedef struct {
   tAxis accel;
   tAxis gyro;
   tAxis mag;
   eAxis euler;
   short temp;
   float gdt;            //gyro sample time (sec)
} tSensor;
