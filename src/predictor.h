//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h> // for memset()

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken

// Definitions for 3-bit counters
#define NT_0  0			// predict NT, very strong not taken
#define NT_1  1			// predict NT, strong not taken
#define NT_2  2			// predict NT, mildy not taken
#define NT_3  3			// predict NT, weakly not taken
#define NT_4  4         // predict NT, very weakly not taken 
#define T_0  5			// predict T, very weakly taken
#define T_1  6			// predict T, weakly taken
#define T_2  7			// predict T, mildly taken
#define T_3  8			// predict T, strong taken
#define T_4  9			// predict T, very strong taken

/*
#define T_0  4			// predict T, weakly taken
#define T_1  5			// predict T, mildly taken
#define T_2  6			// predict T, strong taken
#define T_3  7			// predict T, very strong taken
*/

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

// void init_gshare();

// uint8_t pred_gshare(uint32_t pc) ;

// void train_gshare(uint32_t pc, uint8_t outcome);

#endif
