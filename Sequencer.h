#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <avr/io.h>
#include "defines.h"

#define PARAMS 2
#define PARAM_CONTROL_MASK 0x80
#define PARAM_CONTROL_BIT 7
#define PARAM_VALUE_MASK 0x7F
#define PATTERN_LENGTH MAX_BEATS*PPQN
#define PATTERN_TAB_SIZE PATTERN_LENGTH*PARAMS

#define POTCHANGEMAX 40

struct SequencerState
{
	uint8_t RUNNING:1;
	uint8_t PLAYING:1; // the sequencer can be running without playing automation
	uint8_t RECORDING:1;
};

struct SequencerSignals
{
	uint8_t START_PLAYING:1;
	uint8_t STOP_PLAYING:1;
	uint8_t START_RECORD:1;
	uint8_t STOP_RECORD:1;
	uint8_t PLAYING_START_STOP:1;
	uint8_t RECORDING_START_STOP:1;
	uint8_t ERASE:1;
};

struct SequencerParam
{
	uint8_t control:1;
	uint8_t value:7;
};

extern struct SequencerSignals sequencerSignals;

void SequencerInit(void);
void Sequencer_Erase(void);
void Sequencer_SignalsProcessing(void);
void SequencerStart(void);
void SequencerStop(void);
void SequencerTick(void);

#endif