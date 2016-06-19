 #include <Encoder.h>

/* find maximum of a and b */
#define MAX(a,b) (((a)>(b))?(a):(b))

/* absolute value of a */
#define ABS(a) (((a)<0) ? -(a) : (a))

/* take sign of a, either -1, 0, or 1 */   
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

/* colors are equal  */
#define EQUAL(a, b) (a.x == b.x && a.y == b.y && a.z == b.z)

/* no of points we travel on */
#define NO_POINTS 9

/* start position */
#define START_POINT 0

struct color { 
    int x;
    int y;
    int z;
};

struct color current_color = (struct color) {0, 0, 0};
int target = START_POINT + 1;
int last_direction = 1;

struct color points[] = {
    (struct color) {255, 255, 255},  
    (struct color) {  0,   0,   0}, 
    (struct color) {  0, 255,   0}, 
    (struct color) {255, 255,   0}, 
    (struct color) {255,   0,   0}, 
    (struct color) {255,   0, 255}, 
    (struct color) {  0,   0, 255}, 
    (struct color) {  0, 255, 255},  
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



struct color next_color(int dir, int no_times) {
  for(int i = 0; i < no_times; i ++) {
      // if we reached or current target
      if(EQUAL(current_color, points[target])) {
          // if one of the ends of the color ponints array was reached
          if(target == NO_POINTS || target == 0) {
              // check if the new direction does not go out of bounds
              if(target + dir < NO_POINTS && target + dir > 0) {
                  target += dir;
                  current_color = line3d(current_color, points[target]);
              } else {
                  // trying to get out of bounds. return same color
                  return current_color;
              }
          } else {
              // get to the next target, generate next color
              target += dir;
              current_color = line3d(current_color, points[target]);
          }
      // haven't reached out target yet
      } else {
          // check if we need to change direction
          if(dir != last_direction) {
              target += dir;
              // not sure why i did this
              if(EQUAL(current_color, points[target])) {
                target+=dir;
              }
          }
          current_color = line3d(current_color, points[target]);
      }

    last_direction = dir;
    return current_color;
  }
}

// !!!!!!!!!!!!!!!!!!!!!!!in Encode.h need to change a plus2 to minus2!!!!!!!!!!!!!!!!!!
// if it does not workk, of course

#define ENCODER_PINA 6
#define ENCODER_PINB 7

#define MANUAL_MODE_PIN 2
#define AUTO_MODE_PIN 3

#define RED_PIN   9
#define GREEN_PIN 10
#define BLUE_PIN  11

// point bounds for automatic mode
#define AUTO_LOWER_BOUND 1
#define AUTO_UPPER_BOUND 7

/* COLORS */
#define RED   (struct color) {255,   0,   0}
#define GREEN (struct color) {0,   255,   0}
#define BLUE  (struct color) {0,     0, 255}
#define WHITE (struct color) {255, 255, 255}
#define BLACK (struct color) {0,     0,   0}

/*  ERRORS   */
// manual/auto button, both wires HIGH or both wire LOW
#define MA_BUTTON_LOW_ERR  0
#define MA_BUTTON_HIGH_ERR 1

/* ENCODER FUNCTIONS */
#define NONE                   -1
#define CHANGE_COLOR            0
#define MAKE_TRANSITION         1
#define CHANGE_INTENSITY        2
#define CHANGE_TRANSITION_SPEED 3

// time after encoder action expires in milliseconds
#define TIMER_TIME_OUT 500

// time between each color
#define COLOR_TIMEOUT 60

#define MAX_TRANSITION_DELAY 300
#define MIN_TRANSITION_DELAY 1

Encoder myEnc(ENCODER_PINA, ENCODER_PINB);
long old_pos;
long new_pos;
int button_auto;
int button_manual;
int auto_direction;

void write_rgb(struct color c) {
//  print_color(c);
  analogWrite(RED_PIN, c.x);
  analogWrite(GREEN_PIN, c.y);
  analogWrite(BLUE_PIN, c.z);
}

void test() {
  struct color red = {255, 0, 0};
  struct color green = {0, 255, 0};
  struct color blue = {0, 0, 255};
  struct color s  = {0, 255, 255};
  struct color ss = {255, 0, 255};
  struct color sss = {255, 255, 0};
  struct color white = {255, 255, 255};
  struct color black = {0, 0, 0};
  
  write_rgb(red);
  delay(100);
  write_rgb(black);
  write_rgb(green);
  delay(100);
  write_rgb(black);
  write_rgb(blue);
  delay(100);
  write_rgb(black);
}

void flash_color(struct color err_color) {
  for(int i = 0; i < 3; i++) {
     write_rgb(err_color);
     delay(400);
  } 
  
  write_rgb(BLACK);
}

// 0 blue led flashing -> auto/manual button both low
// 1 red led flashing  -> auto/manual button borh high
void flash_error(int error) {
  if(error == MA_BUTTON_LOW_ERR) {
    flash_color(BLUE);
  }
  
  if(error == MA_BUTTON_HIGH_ERR) {
    flash_color(RED); 
  }
}

void print_color(struct color a)
{
    char text[50];
    sprintf(text, "x:%d  y:%d  z:%d\n", a.x, a.y, a.z);
    Serial.println(text);
}

void startup() {
   for(int i = 0; i < 255; i++) {
     analogWrite(RED_PIN, i);
     analogWrite(BLUE_PIN, i);
     analogWrite(GREEN_PIN, i);
     delay(20);
   }
   current_color = WHITE;
}

void setup() {
  Serial.begin(9600);
  pinMode(MANUAL_MODE_PIN, INPUT);
  pinMode(AUTO_MODE_PIN, INPUT);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  new_pos = 0;
  old_pos = 0;
  
  button_auto = 0;
  button_manual = 0;
  
  auto_direction = 1;
  
  test();
  startup();
}

void loop() {
  new_pos = myEnc.read();
  int encoder_changed = new_pos - old_pos;
  int dir = ZSGN(encoder_changed);
  
  button_manual = digitalRead(MANUAL_MODE_PIN);
  button_auto = digitalRead(AUTO_MODE_PIN);
  
  //delay(10);

  if(button_manual && dir != 0) {
    next_color(ZSGN(encoder_changed), 7);
    write_rgb(current_color);
    old_pos = new_pos;
  }
  
  if(button_auto) {
    // color change logic
    if(target == AUTO_LOWER_BOUND) {
      auto_direction = 1;
    } else if(target == AUTO_UPPER_BOUND) {
      auto_direction = -1;
    } else if(target > AUTO_UPPER_BOUND) {
    	auto_direction = -1;
    } else if(target < AUTO_LOWER_BOUND) {
    	auto_direction = 1;
    }
    
    next_color(auto_direction, 3);
    write_rgb(current_color);
    
    delay(COLOR_TIMEOUT);
  }
}

