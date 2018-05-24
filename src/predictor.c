//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Chen Chen | Sha Liu";
const char *studentID = "A53245506 | ";
const char *email = "chc507@eng.ucsd.edu | ";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t *globalBHT;
uint8_t *localBHT;
uint8_t *localPHT;
uint8_t *chooserTable;
uint8_t globalOutcome;
uint8_t localOutcome;

uint32_t ghistoryReg;
uint32_t globalBHTIdx;
uint32_t localBHTIdx;
uint32_t localPHTIdx;
/*my code*/
uint8_t weakNotTaken = WN;
uint8_t strongTaken = ST;
uint8_t weakTaken = WT;
uint8_t strongNotTaken = SN;
uint8_t initBias = WN;

void init_gshare()
{
    globalBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
    //memset(globalBHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));  // Initiating as weakly not taken
    /*My code*/
    memset(globalBHT, initBias, (1 << ghistoryBits) * sizeof(uint8_t)); // Initiating as weakly not taken
    ghistoryReg = 0;
}

uint8_t pred_gshare(uint32_t pc)
{
    uint32_t temp = pc ^ ghistoryReg;
    uint32_t mask = (1 << ghistoryBits) - 1;
    globalBHTIdx = temp & mask;

    /*My code*/
    if (globalBHT[globalBHTIdx] <= weakNotTaken && globalBHT[globalBHTIdx] >= strongNotTaken)
        return NOTTAKEN;
    else
        return TAKEN;
}

void train_gshare(uint32_t pc, uint8_t outcome)
{
    /*My code*/
    if (outcome)
    {
        if (globalBHT[globalBHTIdx] != strongTaken)
            globalBHT[globalBHTIdx]++;
    }
    else
    {
        if (globalBHT[globalBHTIdx] != strongNotTaken)
            globalBHT[globalBHTIdx]--;
    }

    //ghistoryReg = ghistoryReg << 1;
    uint32_t mask = (1 << ghistoryBits) - 1;
    ghistoryReg &= mask;
    ghistoryReg = ghistoryReg << 1;
    ghistoryReg += outcome;
}

void init_tournament()
{
    globalBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
    localBHT = malloc((1 << lhistoryBits) * sizeof(uint8_t));
    localPHT = malloc((1 << pcIndexBits) * sizeof(uint32_t));
    chooserTable = malloc((1 << ghistoryBits) * sizeof(uint8_t));

    /*my code*/
    memset(globalBHT, initBias, (1 << ghistoryBits) * sizeof(uint8_t));
    memset(localBHT, initBias, (1 << lhistoryBits) * sizeof(uint8_t));
    memset(localPHT, 0, (1 << pcIndexBits) * sizeof(uint32_t));
    memset(chooserTable, initBias, (1 << ghistoryBits) * sizeof(uint8_t));
    ghistoryReg = 0;
}

uint8_t pred_local(uint32_t pc)
{

    uint32_t localPHTIndex = pc & ((1 << pcIndexBits) - 1);
    uint32_t localBHTIndex = localPHT[localPHTIndex];
    uint8_t localPrediction = localBHT[localBHTIndex];
    //localOutcome = ((localPrediction == WN || localPrediction == SN) ? NOTTAKEN : TAKEN);
    /*My code*/
    localOutcome = ((localPrediction <= weakNotTaken && localPrediction>= strongNotTaken) ? NOTTAKEN : TAKEN);
    return localOutcome;
}

uint8_t pred_global(uint32_t pc)
{
    uint32_t gBHTIndex = (ghistoryReg) & ((1 << ghistoryBits) - 1);
    uint8_t gPrediction = globalBHT[gBHTIndex];
    /*my code*/
    globalOutcome = ((gPrediction <= weakNotTaken && gPrediction>= strongNotTaken) ? NOTTAKEN : TAKEN);
    //globalOutcome = ((gPrediction == WN || gPrediction == SN) ? NOTTAKEN : TAKEN);
    return globalOutcome;
}

uint8_t pred_tournament(uint32_t pc)
{
    uint32_t mask = (1 << ghistoryBits) - 1;
    globalBHTIdx = ghistoryReg & mask;
    pred_global(pc);
    pred_local(pc);

    /*My code*/

    if (chooserTable[globalBHTIdx] <= weakNotTaken && chooserTable[globalBHTIdx] >= strongNotTaken)
        return globalOutcome;
    else
        return localOutcome;

    /*
    if(chooserTable[globalBHTIdx] == SN || chooserTable[globalBHTIdx] == WN)
        return globalOutcome;
    else
        return localOutcome;
    */
}

void init_custom()
{
    weakNotTaken = NT_4;
    strongNotTaken = NT_0;
    strongTaken = T_4;
    initBias = NT_4;
    ghistoryBits = 9;
    lhistoryBits = 10;
    pcIndexBits = 10;
    init_tournament();
}

uint8_t pred_custom(uint32_t pc)
{
    return pred_tournament(pc);
}

void train_tournament(uint32_t pc, uint8_t outcome)
{
   
    /*My code*/
    //Get index of the local branch table index
    uint32_t pcMask = ((1 << pcIndexBits) - 1);
    uint32_t localPHTIdx = pc & pcMask;
    uint32_t localBHTIdx = localPHT[localPHTIdx];

    if (outcome)
    {
        if (localBHT[localBHTIdx] != strongTaken)
            localBHT[localBHTIdx]++;
    }
    else
    {
        if (localBHT[localBHTIdx] != strongNotTaken)
            localBHT[localBHTIdx]--;
    }

    localPHT[localPHTIdx] = localPHT[localPHTIdx] << 1;
    localPHTIdx = pc & ((1 << pcIndexBits) - 1);
    localPHT[localPHTIdx] += outcome;


    //Update global history
    /*My code*/
    if (outcome)
    {
        if (globalBHT[globalBHTIdx] != strongTaken)
            globalBHT[globalBHTIdx]++;
    }
    else
    {
        if (globalBHT[globalBHTIdx] != strongNotTaken)
            globalBHT[globalBHTIdx]--;
    }

    
    uint32_t mask = (1 << ghistoryBits) - 1;
    ghistoryReg = ghistoryReg << 1;
    ghistoryReg &= mask;
    ghistoryReg += outcome;
    

    // Updating the chooser table
    if (globalOutcome == outcome && localOutcome != outcome)
    {
        if (chooserTable[globalBHTIdx] != strongNotTaken)
            chooserTable[globalBHTIdx]--;
    }
    if (globalOutcome != outcome && localOutcome == outcome)
    {
        if (chooserTable[globalBHTIdx] != strongTaken)
            chooserTable[globalBHTIdx]++;
    }
}

void train_custom(uint32_t pc, uint8_t outcome)
{
    train_tournament(pc, outcome);
}

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor()
{
    //
    //TODO: Initialize Branch Predictor Data Structures
    //
    switch (bpType)
    {
    case STATIC:
        break;
    case GSHARE:
        init_gshare();
        break;
    case TOURNAMENT:
        init_tournament();
        break;
    case CUSTOM:
        init_custom();
        break;
    default:
        break;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{
    //
    //TODO: Implement prediction scheme
    //

    // Make a prediction based on the bpType
    switch (bpType)
    {
    case STATIC:
        return TAKEN;
    case GSHARE:
        return pred_gshare(pc);
    case TOURNAMENT:
        return pred_tournament(pc);
        break;
    case CUSTOM:
        //return pred_bimode(pc);
        return pred_custom(pc);
        break;
    default:
        break;
    }

    // If there is not a compatable bpType then return NOTTAKEN
    return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome)
{
    //
    //TODO: Implement Predictor training
    //
    switch (bpType)
    {
    case STATIC:
        break;
    case GSHARE:
        train_gshare(pc, outcome);
        break;
    case TOURNAMENT:
        train_tournament(pc, outcome);
        break;
    case CUSTOM:
        train_custom(pc, outcome);
        break;
    default:
        break;
    }
}
