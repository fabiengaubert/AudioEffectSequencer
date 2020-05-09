#include "Sequencer.h"
#include "PatternLights.h"
#include "ADC.h"
#include "AudioEffects.h"
#include "Pin.h"

//==============================================================================================
//======================================  Local functions  =====================================
//==============================================================================================
// Beat variables
static uint8_t currentBeat=0;
static uint8_t currentBeatTick=0;

// Sequence variables
struct SequencerState sequencerState;
struct SequencerSignals sequencerSignals;
static uint16_t currentSequenceTick=0;

// Pot variables
struct SequencerParam savedParams[PARAMS];
struct SequencerParam currentParam[PARAMS];
uint8_t potValueChangeCount[PARAMS];
struct SequencerParam sequence[PATTERN_TAB_SIZE];

static uint8_t Sequencer_ParamChange(uint8_t paramNb);
static uint8_t Sequencer_ParamADCChange(uint8_t paramNb);
static uint8_t Sequencer_ParamGet(uint8_t paramNb);
static void Sequencer_ParamLoad(void);
static void Sequencer_ParamUnload(void);
static void Sequencer_ParamSave(uint8_t paramId);

static uint8_t Sequencer_ParamChange(uint8_t paramNb)
{
	// Whatever the sequencer state, if the pot value change we will refresh the current value
	if (currentParam[paramNb].value != savedParams[paramNb].value) {
		potValueChangeCount[paramNb] = POTCHANGEMAX;
		return 1;
	}
	struct SequencerParam * tabParam = & sequence[currentSequenceTick+paramNb];
	return (*tabParam).control;
}

static uint8_t Sequencer_ParamADCChange(uint8_t paramNb)
{
	// Whatever the sequencer state, if the pot value change we will refresh the current value
	if (currentParam[paramNb].value != savedParams[paramNb].value) {
		return 1;
	}
	return 0;
}

static uint8_t Sequencer_ParamGet(uint8_t paramNb)
{
	//char buffer[20];
	// Pot change physicaly
	if (currentParam[paramNb].value != savedParams[paramNb].value) {
		//sprintf(buffer, "Phy%d: %d\n", paramNb, currentParam[paramNb].value);
		//USART_TransmitString(buffer);
		return currentParam[paramNb].value;
	}
	if (potValueChangeCount[paramNb] > 0) {
		//sprintf(buffer, "Last%d: %d\n", paramNb, currentParam[paramNb].value);
		//USART_TransmitString(buffer);
		return currentParam[paramNb].value;
	}
	struct SequencerParam * tabParam = & sequence[currentSequenceTick+paramNb];
	//sprintf(buffer, "Rec%d: %d\n", paramNb, (*tabParam).value);
	//USART_TransmitString(buffer);
	return (*tabParam).value;
}

static void Sequencer_ParamLoad() {
	uint8_t paramId;
	for (paramId = 0; paramId < PARAMS; paramId++) {
		ADCChangeChannel(paramId);
		currentParam[paramId].value = ADCRead7Bits();
		if (potValueChangeCount[paramId] > 0) {
			potValueChangeCount[paramId]--;
		}
	}
}
static void Sequencer_ParamUnload() {
	uint8_t paramId;
	for (paramId = 0; paramId < PARAMS; paramId++) {
		savedParams[paramId].value = currentParam[paramId].value;
	}	
}
static void Sequencer_ParamSave(uint8_t paramId) {
	struct SequencerParam *tabParam = & sequence[currentSequenceTick+paramId];
	if (savedParams[paramId].value != currentParam[paramId].value) {
		(*tabParam).control = 1;
		(*tabParam).value = currentParam[paramId].value;
	} else if (potValueChangeCount[paramId] > 0) {
		(*tabParam).control = 1;
		(*tabParam).value = savedParams[paramId].value;	
	} else {
		(*tabParam).control = 0;
		(*tabParam).value = 0;
	}
}
			
			
/*
static void SequencerStoreParamAt(uint16_t beat, uint8_t ppqn, uint8_t paramNb, uint8_t paramValue)
{
	paramValue &= PARAM_VALUE_MASK;
	paramValue |= PARAM_CONTROL_MASK;
	sequence[(beat*PPQN*PARAMS)+PPQN*ppqn+paramNb] = paramValue;
}
*/
//==============================================================================================
//=========================================  Functions  ========================================
//==============================================================================================

void SequencerInit()
{
	sequencerSignals.RECORDING_START_STOP = 0;
	sequencerSignals.PLAYING_START_STOP = 0;
	sequencerSignals.START_PLAYING = 0;
	sequencerSignals.STOP_PLAYING = 0;
	sequencerSignals.START_RECORD = 0;
	sequencerSignals.STOP_RECORD = 0;
	
	sequencerState.RUNNING=0;
	sequencerState.PLAYING=0;
	sequencerState.RECORDING=0;
}

void Sequencer_Erase() {
	uint16_t i;
	for (i = 0; i < PATTERN_TAB_SIZE; i++) {
		sequence[i].value = 0;
		sequence[i].control = 0;
	}
	SetParam1(127);
	SetParam2(127);
}

void Sequencer_SignalsProcessing() {
	//char buffer[30];
	// What do we do when several flags of same type are raised
	// -> we order signal by priority
	
	if (sequencerSignals.ERASE == 1) {
		sequencerSignals.ERASE = 0;
		Sequencer_Erase();
		//sprintf(buffer, "ERASE\n");
	}
	
	if (sequencerSignals.RECORDING_START_STOP) {
		sequencerSignals.RECORDING_START_STOP = 0;
		if (sequencerState.RECORDING) {
			sequencerSignals.STOP_RECORD = 1;
		} else {
			sequencerSignals.START_RECORD = 1;
		}
	}
	if (sequencerSignals.PLAYING_START_STOP) {
		sequencerSignals.PLAYING_START_STOP = 0;
		//sprintf(buffer, "START/STOP PLAYING\n");
		//USART_TransmitString(buffer);
		if (sequencerState.RUNNING) {
			sequencerSignals.STOP_PLAYING = 1;
		} else {
			sequencerSignals.START_PLAYING = 1;
		}
	}
	
	if (sequencerSignals.STOP_PLAYING) {
		sequencerSignals.STOP_PLAYING = 0;
		sequencerState.RUNNING=0;
		sequencerSignals.STOP_RECORD = 1;
		//sprintf(buffer, "STOP PLAYING\n");
		//USART_TransmitString(buffer);
		
		PLBeat = 0;
		currentSequenceTick=0;
		currentBeatTick=0;
		currentBeat=0;
		
		PatternLights_Reset(0);
		PIN_Off(LIGHT_PS);
	} else if (sequencerSignals.START_PLAYING) {
		sequencerSignals.START_PLAYING = 0;
		currentSequenceTick=0; 
		sequencerState.RUNNING=1;
		//sprintf(buffer, "START PLAYING\n");
		//USART_TransmitString(buffer);
		PIN_On(LIGHT_PS);
	}
	
	if (sequencerSignals.STOP_RECORD) {
		sequencerSignals.STOP_RECORD = 0;
		sequencerState.RECORDING=0;
		//sprintf(buffer, "STOP RECORDING\n");
		//USART_TransmitString(buffer);
		PIN_Off(LIGHT_REC);
	} else if (sequencerSignals.START_RECORD) {
		sequencerSignals.START_RECORD = 0;
		sequencerState.RECORDING=1;
		//sprintf(buffer, "START RECORDING\n");
		//USART_TransmitString(buffer);
		PIN_On(LIGHT_REC);
	}
}

void SequencerStart()
{
	sequencerSignals.START_PLAYING = 1;
}

void SequencerStop()
{
	sequencerSignals.STOP_PLAYING = 1;
}

// called by timer
void SequencerTick()
{
	Sequencer_SignalsProcessing();
	
	Sequencer_ParamLoad();
	if(sequencerState.RECORDING)
	{
		uint8_t paramId;
		for (paramId = 0; paramId < PARAMS; paramId++) {
			if (Sequencer_ParamADCChange(paramId)) {
				Sequencer_ParamSave(paramId);
			}
		}
	}
	
	if(sequencerState.RUNNING)
	{
		if (currentBeatTick == 0)
		{
			PatternLights_NextStep();
		}
		currentSequenceTick+=PARAMS;
		currentBeatTick++;
		if (currentBeatTick>=PPQN)
		{
			currentBeatTick=0;
			currentBeat++;
		}
		if (currentSequenceTick>=PATTERN_TAB_SIZE)
		{
			currentSequenceTick=0;
			currentBeat=0;
		}
		for (int paramId = 0; paramId<PARAMS; paramId++)
		{
			if (Sequencer_ParamChange(paramId))
			{
				// Midi?
				// MIDISendCC(1, 49, SequencerGetParam(currentParam));
				// Setting?
				if (paramId == 0) {
					SetParam1(Sequencer_ParamGet(paramId));
				} else if (paramId == 1) {
					SetParam2(Sequencer_ParamGet(paramId));
				}
			}
		}
	}
	Sequencer_ParamUnload();
}
