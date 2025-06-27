#pragma once
#include <windows.h>

//Setting Value ===========================================
#define MapLocation "D:\\Map.adofai"
#define Pitch 1.0

/*Speed & BPM*/
#define OneHandBPM 200
#define MinPressBPM 500

/*Fit Beat Mode*/
#define FitBeat false
#define BeginBeat 0.5
#define StandardBPM (220.0)

/*Auto Restart*/
#define AutoRestart false

/*Clear*/
#define Clear true

/*Input Style*/
#define StyleInOut			InToOut, InToOut
#define StyleOutIn			OutToIn, OutToIn
#define StyleStep			InToOut, OutToIn
#define StyleReversedStep	OutToIn, InToOut
#define StyleTrans			Trans, Trans
#define Style { -1, {InToOut} }


/*Sentence Input Macro*/
#define SentenceInput false
#define Sentence "SENTENCE"

#if SentenceInput
#define KeyTable {'A'}, {'B'}, {'C'}, {'D'}, {'E'}, {'F'}, {'G'}, {'H'}, {'I'}, {'J'}, {'K'}, {'L'}, {'M'}, {'N'}, {'O'}, {'P'}, {'Q'}, {'R'}, {'S'}, {'T'}, {'U'}, {'V'}, {'W'}, {'X'}, {'Y'}, {'Z'}, {' '}
//#define KeyTable {'0'}, {'1'}, {'2'}, {'3'}, {'4'}, {'5'}, {'6'}, {'7'}, {'8'}, {'9'}
#else
/*Key Table*/
//#define KeyTable {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
//#define KeyTable { 'L', 'R' } // Mouse
//#define KeyTable { 'L', 'K', 'G', 'H', 'I', 'J' }, { 'A', 'B', 'F', 'E', 'D', 'C' } // 12keys
#define KeyTable { 'K', 'G', 'H', 'I', 'J' }, { 'B', 'F', 'E', 'D', 'C' } // 10keys
//#define KeyTable { 'P', 'Q', 'R', 'S', 'T' }, { 'F', 'G', 'H', 'I', 'J' }, { 'E', 'D', 'C', 'B', 'A' }, { 'O', 'N', 'M', 'L', 'K' }
//#define KeyTable { 'B', 'C', 'D', 'E', 'A' } // LEFT
//#define KeyTable { 'D', 'C', 'B', 'A', 'E' } // RIGHT
//#define KeyTable { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' } // LEFT
//#define KeyTable { 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A' } // RIGHT

//#define KeyTable {'A'}, {'B'}, {'C'}, {'D'}, {'E'}, {'F'}, {'G'}, {'H'}, {'I'}, {'J'}, {'K'}, {'L'}, {'M'}, {'N'}, {'O'}, {'P'}
//#define KeyTable {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6'}, {'P', 'O', 'N', 'M', 'L', 'K', 'J', 'I', 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}
//#define KeyTable {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'}, {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}
//*64*/	#define KeyTable {'A'}, {'B'}, {'C'}, {'D'}, {'E'}, {'F'}, {'G'}, {'H'}, {'I'}, {'J'}, {'K'}, {'L'}, {'M'}, {'N'}, {'O'}, {'P'}, {'Q'}, {'R'}, {'S'}, {'T'}, {'U'}, {'V'}, {'W'}, {'X'}, {'Y'}, {'Z'}, {'1'}, {'2'}, {'3'}, {'4'}, {'5'}, {'6'}, {'7'}, {'8'}, {'9'}, {'0'}, {VK_NUMPAD1}, {VK_NUMPAD2}, {VK_NUMPAD3}, {VK_NUMPAD4}, {VK_NUMPAD5}, {VK_NUMPAD6}, {VK_NUMPAD7}, {VK_NUMPAD8}, {VK_NUMPAD9}, {VK_NUMPAD0}, {VK_F1}, {VK_F2}, {VK_F3}, {VK_F4}, {VK_F5}, {VK_F6}, {VK_F7}, {VK_F8}, {VK_F9}, {VK_F10}, {VK_LEFT}, {VK_UP}, {VK_RIGHT}, {VK_DOWN}, {VK_INSERT}, {VK_HOME}, {VK_DELETE}, {VK_END}
// 
//*64*/	#define KeyTable {'7', '8', '9', '0', VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_NUMPAD0, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_INSERT, VK_HOME, VK_DELETE, VK_END}, {'6', '5', '4', '3', '2', '1', 'Z', 'Y', 'X', 'W', 'V', 'U', 'T', 'S', 'R', 'Q', 'P', 'O', 'N', 'M', 'L', 'K', 'J', 'I', 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}
//*32*/	#define KeyTable {'7', '8', '9', '0', VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_NUMPAD0, VK_F1, VK_F2}, {'6', '5', '4', '3', '2', '1', 'Z', 'Y', 'X', 'W', 'V', 'U', 'T', 'S', 'R', 'Q'}
//*16*/	#define KeyTable {'7', '8', '9', '0', VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4}, {'6', '5', '4', '3', '2', '1', 'Z', 'Y'}
//*8*/	#define KeyTable {'7', '8', '9', '0'}, {'6', '5', '4', '3'}
//*4*/	#define KeyTable {'7'}, {'6'}
#endif

/*Hand*/
#define MaxHandNumber 2
#define HandNumber {-1, 2, 0.6}	// {Tile, Num, Offset (Pressing Duration)}
#define FingerNumber 5
#define ThumbNumber 1

#define MouseMode false

/*Accuracy Error*/
#define Accurate true
#define StandardDeviation(x) ((x / 3) ? (x / 3) : 1)

#if Accurate
//DO NOT TOUCH
#define MissPercent 0.0
#define ErrorRange 0
#define PressRange 0
#else

#define MissPercent 0.001
#define ErrorRange 30
#define PressRange 10

#endif

/*Sound Effect*/
#define UseSoundEffect true

/*Random*/
#define Random false

/*Hold Activation*/
#define HoldActive true

/*Infinity Click*/
#define Infinity false

/*Sound Offset*/
#define SoundOffset 133

/*Key Input*/
#define KeyPressing true

/*Start Immediately*/
#define StartImmediately false

/*Hand Cam*/
#define UseHandCam true
#define HandSize 1.0

/*Increase Hand Speed*/
#define IncreaseHandSpeed true

//=========================================================

/*Testing*/
#define Test false

//=========================================================

enum InputStyle { InToOut = 0, OutToIn = 1, Trans = 2 };
const int  key_table[MaxHandNumber][FingerNumber] = { KeyTable };
struct InputStylePair { int num; InputStyle style[MaxHandNumber]; };
const InputStylePair style[] = {Style};
struct HandPair { int tile; int num; double key_offset; };
const HandPair hand_number_list[] = { HandNumber };
#define BPM_to_Time(x) (60000.0 / (x))
#define HandSpeed(x) BPM_to_Time(x)
#define MaxPressSpeed BPM_to_Time(MinPressBPM)