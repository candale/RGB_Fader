

class Fader {

private:
	// Output pins for PWM
	int R_PIN; // Red LED
	int G_PIN; // Green LED
	int B_PIN; // Blue LED

	// Used to adjust the limits for the LED, especially if it has a lower ON threshold
	int MIN_RGB_VALUE = 10; // no smaller than 0. 
	int MAX_RGB_VALUE = 255; // no bigger than 255.
	
	/*
	 A list of vertex numbers encoded 2 per char.
	 Hex digits are used as vertices 0-7 fit nicely (3 bits 000-111) and have the same visual
	 representation as decimal, so chars 0x12, 0x34 ... should be interpreted as vertex 1 to 
	 v2 to v3 to v4 (ie, one continuous path B to C to D to E).
	*/

	int PATH_SIZE = 18;
	const char path[18] =
	{
		0x01, 0x23, 0x76, 0x54, 0x03, 0x21, 0x56, 0x74,  // trace the edges
		0x13, 0x64, 0x16, 0x02, 0x75, 0x24, 0x35, 0x17, 0x25, 0x70 // do the diagonals
	};


	typedef struct
	{
		int  x, y, z;
	} coord;
	coord currentRGB; // current RGB color used for continous transition
	coord seqCurrentRGB = {MIN_RGB_VALUE, MIN_RGB_VALUE, MIN_RGB_VALUE}; // current RGB color used for sequential transition

	/*
	 Vertices of a cube
				
		C+----------+G
		/|        / |
	  B+---------+F |
	   | |       |  |    y   
	   |D+-------|--+H   ^  7 z
	   |/        | /     | /
	  A+---------+E      +--->x

	*/
	const coord vertex[8] = 
	{
	   //x  y  z      name
		{0, 0, 0}, // A or 0
		{0, 1, 0}, // B or 1
		{0, 1, 1}, // C or 2
		{0, 0, 1}, // D or 3
		{1, 0, 0}, // E or 4
		{1, 1, 0}, // F or 5
		{1, 1, 1}, // G or 6
		{1, 0, 1}  // H or 7
	};

	int currentVertexA = 0, currentVertexB = 1, currentPosInPath = 0, currentColorPos = 0, globalIncrement = 1;
protected:
	void writeValues(coord rgb);

public:
	Fader(int, int, int);
	void traverse(int delay = 80); // default transition delay is 80 milliseconds
	void setNextColor();
};