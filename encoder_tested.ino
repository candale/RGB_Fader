 #include <Encoder.h>

/* find maximum of a and b */
#define MAX(a,b) (((a)>(b))?(a):(b))

/* absolute value of a */
#define ABS(a) (((a)<0) ? -(a) : (a))

/* take sign of a, either -1, 0, or 1 */   
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

/* colors are equal  */
#define EQUAL(a, b) (a.x == b.x && a.y == b.y && a.z == b.z)

struct color { 
    int x;
    int y;
    int z;
};

struct color current_color = (struct color) {0, 0, 0};
int target = 1;
int last_dir = 1;

struct color points[] = {
    (struct color) {  0,   0,   0}, 
    (struct color) {  0, 255,   0}, 
    (struct color) {255, 255,   0}, 
    (struct color) {255,   0,   0}, 
    (struct color) {  0,   0, 255}, 
    (struct color) {255, 255, 255}  
};

struct color line3d(struct color a, struct color b)
{
    int x1, y1, z1;
    int x2, y2, z2;
    x1 = a.x; y1 = a.y; z1 = a.z;
    x2 = b.x; y2 = b.y; z2 = b.z;
    int xd, yd, zd;
    int x, y, z;
    int ax, ay, az;
    int sx, sy, sz;
    int dx, dy, dz;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    ax = ABS(dx) << 1;
    ay = ABS(dy) << 1;
    az = ABS(dz) << 1;

    sx = ZSGN(dx);
    sy = ZSGN(dy);
    sz = ZSGN(dz);

    x = x1;
    y = y1;
    z = z1;

    if (ax >= MAX(ay, az))            /* x dominant */
    {
        yd = ay - (ax >> 1);
        zd = az - (ax >> 1);
        for (;;)
        {
            if (x == x2)
            {
                return (struct color) {x, y, z};
            }

            if (yd >= 0)
            {
                y += sy;
                yd -= ax;
            }

            if (zd >= 0)
            {
                z += sz;
                zd -= ax;
            }


            x += sx;
            yd += ay;
            zd += az;

            return (struct color) {x, y, z};
        }
    }
    else if (ay >= MAX(ax, az))            /* y dominant */
    {
        xd = ax - (ay >> 1);
        zd = az - (ay >> 1);
        for (;;)
        {
            if (y == y2)
            {
                return (struct color) {x, y, z};
            }

            if (xd >= 0)
            {
                x += sx;
                xd -= ay;
            }

            if (zd >= 0)
            {
                z += sz;
                zd -= ay;
            }

            y += sy;
            xd += ax;
            zd += az;

            return (struct color) {x, y, z};
        }
    }
    else if (az >= MAX(ax, ay))            /* z dominant */
    {
        xd = ax - (az >> 1);
        yd = ay - (az >> 1);
        for (;;)
        {
            if (z == z2)
            {
                return (struct color) {x, y, z};
            }

            if (xd >= 0)
            {
                x += sx;
                xd -= az;
            }

            if (yd >= 0)
            {
                y += sy;
                yd -= az;
            }

            z += sz;
            xd += ax;
            yd += ay;

            return (struct color) {x, y, z};
        }
    }

    return (struct color) {x, y, z};
}



struct color next_color(int dir) {
  for(int i = 0; i < 10; i ++) {
      if(EQUAL(current_color, points[target])) {
          if(target == 5 || target == 0) {
              if(target + dir < 5 && target + dir > 0) {
                  target += dir;
                  current_color = line3d(current_color, points[target]);
              } else {
                  return current_color;
              }
          } else {
              target += dir;
              current_color = line3d(current_color, points[target]);
          }
      } else {
          if(dir != last_dir) {
              target += dir;
              if(EQUAL(current_color, points[target])) {
                target+=dir;
              }
          }
          current_color = line3d(current_color, points[target]);
      }

    last_dir = dir;
    return current_color;
  }
}

// !!!!!!!!!!!!!!!!!!!!!!!in Encode.h need to change a plus2 to minus2!!!!!!!!!!!!!!!!!!
// if it does not workk, of course

#define ENCODER_PINA 6
#define ENCODER_PINB 7

#define MANUAL_MODE_PIN 2
#define AUTO_MODE_PIN 3

#define RED   11
#define GREEN 10
#define BLUE  9


Encoder myEnc(ENCODER_PINA, ENCODER_PINB);
long old_pos = -100;
long new_pos = -1;
int mode = 1; // 1 - manual, 0 auto

void write_rgb(struct color c) {
  //Serial.println("Wrote rgb");
  analogWrite(RED, c.x);
  analogWrite(GREEN, c.y);
  analogWrite(BLUE, c.z);
}

void flash_led() {
  struct color red = {255, 0, 0};
  struct color green = {0, 255, 0};
  struct color blue = {0, 0, 255};
  struct color s  = {0, 255, 255};
  struct color ss = {255, 0, 255};
  struct color sss = {255, 255, 0};
  struct color white = {255, 255, 255};
  struct color black = {0, 0, 0};
  
  write_rgb(red);
  delay(700);
  write_rgb(black);
  write_rgb(green);
  delay(700);
  write_rgb(black);
  write_rgb(blue);
  delay(700);
  write_rgb(black);
  
  
}

void print_color(struct color a)
{
    char text[50];
    sprintf(text, "x:%d  y:%d  z:%d\n", a.x, a.y, a.z);
    Serial.println(text);
}

int i;
int foo = 0;
void setup() {
  Serial.begin(9600);
  pinMode(MANUAL_MODE_PIN, INPUT);
  pinMode(AUTO_MODE_PIN, INPUT);
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  pinMode(12, OUTPUT);
  
  new_pos = 0;
  old_pos = 0;
  i = 0;
}

void loop() {
  foo++;
  new_pos = myEnc.read();
  if (new_pos != old_pos) {
    next_color(ZSGN(new_pos - old_pos));
    write_rgb(current_color);
    old_pos = new_pos;
  }

//  for(int i = 0; i < 1000; i ++) {
//    next_color(1);
//    write_rgb(current_color);
//    print_color(current_color);
//    delay(30);
//  }

//  for(int i = 0; i < 256; i++) {
//     analogWrite(RED, i);
//     analogWrite(GREEN, i);
//     analogWrite(BLUE, i);
//     analogWrite(12, i);
//    delay(50); 
//  }
    //flash_led();
}
