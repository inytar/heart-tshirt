// Helper function for mapping pixels in a two-dimensional
// XY matrix of pixels.
//
// Lets you program the heart as a plain 13x13 matrix.
// Writing and reading to 'holes' is allowed. 
// It is possible to test if you are in the layout like this:
//   if( XY(x,y) > LAST_VISIBLE_LED ) { ..off the heart.. }
//
// It is safe to call XY(x,y) with arbritary values, any value
// outisde the max will map th the first hidden pixel.

// Params for width and height
const uint8_t kMatrixWidth = 13;
const uint8_t kMatrixHeight = 13;

// Pixel layout
//
//
//  XY    0   1   2   3   4   5   6   7   8   9  10  11  12
//    +----------------------------------------------------
//  0 |   .   .  20  19  18   .   .   .  14  13  12   .   .
//  1 |   .  21  50  49  48  17   .  15  44  43  42  11   .
//  2 |  22  51  76  75  74  47  16  45  70  69  68  41  10
//  3 |  23  52  77  98  97  73  46  71  91  90  67  40   9
//  4 |  24  53  78  99  96  95  72  93  92  89  66  39   8
//  5 |   .  25  54  79 100 108  94 105  88  65  38   7   .
//  6 |   .  26  55  80 101 107 106 104  87  64  37   6   .
//  7 |   .   .  27  56  81 102 103  86  63  36   5   .   .
//  8 |   .   .   .  28  57  82  85  62  35   4   .   .   .
//  9 |   .   .   .  29  58  83  84  61  34   3   .   .   .
// 10 |   .   .   .   .  30  59  60  33   2   .   .   .   .
// 11 |   .   .   .   .   .  31  32   1   .   .   .   .   .
// 12 |   .   .   .   .   .   .   0   .   .   .   .   .   .

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds[ NUM_LEDS ];

#define LAST_VISIBLE_LED 106

// This function will retn the right 'led index number' for
// a given set of X and Y coordinates.
unint8_t XY( uint8_t x, uint8_t y )
{
        if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
                return (LAST_VISIBLE_LED + 1);
        }

        const uint8_t MatrixTable[] = {
                162, 163,  20,  19,  18, 164, 165, 166,  14,  13,  12, 167, 168,
                159,  21,  50,  49,  48,  17, 160,  15,  44,  43,  42,  11, 161,
                 22,  51,  76,  75,  74,  47,  16,  45,  70,  69,  68,  41,  10,
                 23,  52,  77,  98,  97,  73,  46,  71,  91,  90,  67,  40,   9,
                 24,  53,  78,  99,  96,  95,  72,  93,  92,  89,  66,  39,   8,
                157,  25,  54,  79, 100, 108,  94, 105,  88,  65,  38,   7, 158,
                155,  26,  55,  80, 101, 107, 106, 104,  87,  64,  37,   6, 156,
                151, 152,  27,  56,  81, 102, 103,  86,  63,  36,   5, 153, 154,
                145, 146, 147,  28,  57,  82,  85,  62,  35,   4, 148, 149, 150,
                139, 140, 141,  29,  58,  83,  84,  61,  34,   3, 142, 143, 144,
                131, 132, 133, 134,  30,  59,  60,  33,   2, 135, 136, 137, 138,
                121, 122, 123, 124, 125,  31,  32,   1, 126, 127, 128, 129, 130,
                109, 110, 111, 112, 113, 114,   0, 115, 116, 117, 118, 119, 120
        };
        uint8_t i = (y * kMatrixWidth) + x;
        uint8_t j = MatrixTable[i];
        return j;
};
