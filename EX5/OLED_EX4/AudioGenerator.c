/*
 * AudioGenerator.c
 *
 * Created: 17.11.2021 14:47:51
 *  Author: sindrbov
 */ 
/*
GOAL:	Configure Pin PD5 (OC1A) to act as a variable oscillator, capable of driving the audio amplifier circuit
		This oscillator can then be built upon to produce a 16-step sequencer, capable of playing short jingles
		
	Requirements:
		- Generate signal of constant frequency between 10Hz and 10kHz
		- Keep track of how long a tone has been produced
		- Programmable by feeding an array of the 16 steps as tones
		
	Operation:
		A timer will count in increments at a constant, known rate. When the counter has reached a certain value, 
		! Set DDR register of OC1A pin to enable output (DDRD |= (1<<PD5))
		
		FCPU = 4915200Hz	= clk_i/o (?)			4915200/64 = 76800Hz
		
		Step length is determined by Beats per Minutes (BPM) value. Standard is 120. This equates to 2Hz. Thus the step length would be 0.5 sec.
			SL = 60/BPM. 
			For a given osc_freq, it will take 2*osc_freq*(60/BPM) oscillations to complete one step (2 toggles per wave).
			A counter must keep track of the number of oscillations, and determine when it is time to advance to next step.
			
			Example: 
			The counter increments with 76800Hz per second. The oscillator is tasked with playing an A4 at 440Hz. The osc pin output must thusly toggle at each 76800/(2*440) ~= 87th step.
			At 120bpm, this means that after (2*440)*(60/120)=440 toggles, it's time to load the next step.
		
		

	Registers of note:
		Timer/Counter Control Registers A [TCCR1A] (and TCCR1B, which is also needed for the settings of Channel A)
			Mode: CTC	
			Set
				COM1A1:0 COM1A0:1	Toggle OC1A on Compare Match
				FOC1A: Force Output Compare; trigger match. Don't care about this one
				WGM1 1:0 in TCCR1A and WGM1 3:2 in TCCR1B: Together decides mode and source of counter-reset-value. Set WGM1[3]=0, WGM1[2]=1, WGM1[1]=0, WGM1[0]=0 for Mode:CTC, Source:OCR1A
				CS1[2:0] in TCCR1B: Decides Counter/Timer's clock source. Set CS1[2:0] to 011 will set clock to 76800Hz - sufficient for all notes
					! Speaker can only drive up to 10KHz anyway
		TCNT1H and TCNT1L: The two 8-bit registers that make up the 16-bit counter.
		OCR1AH and OCR1AL: The registers with the value the counter is compared against. Write your value to these!
		
		Interrupts:
			TIFR:
			TIMSK:

*/
//		--		--		--		//

#include <AudioGenerator.h>

const int TONE_A = 440;
const int TONE_B = 494;
const int TONE_C = 523;
const int TONE_D = 587;
const int TONE_E = 659;
const int TONE_F = 698;
const int TONE_G = 784;



void AG_init(){

	DDRD |= (1<<PD5);					// Enable output on osc pin
	
	//TCCR1A |= (1<<COM1A0);				// Set 'Toggle OC1A on Compare Match'
	TCCR1B |= (1<<WGM12);				// Set mode to CTC and TOP source to OCR1A
	TCCR1B |= (1<<CS11) | (1<<CS10);	// Set counter clock to clk_i/o / 64 = 76800Hz
	
	//TIMSK |= (1<<OCIE1A);				// Enable interrupts when Output Compare Match occurs in Timer/Counter1

	AG_enable_osc();

}



void AG_set_freq(int freq){
	printf("freq:%d\n", freq);
	int trigger_step	= 76800/(2*freq);
	printf("trg_stp:%d\n",trigger_step);
	
	uint8_t step_upper	= (uint8_t) (trigger_step >> 8);
	uint8_t step_lower	= (uint8_t) (trigger_step & 0b0000000011111111);
	printf("sp_up, sp_lw: %d %d\n", step_upper, step_lower);
	
	OCR1AH	= step_upper;
	OCR1AL	= step_lower;
}

void AG_enable_osc(){
	TCCR1A |= (1<<COM1A0);				// Set 'Toggle OC1A on Compare Match'
}
void AG_disable_osc(){
	TCCR1A &= ~(1<<COM1A0);				// Clear 'Toggle OC1A on Compare Match'
}

int OC1A_COMP_INT = 0;

ISR(TIMER1_COMPA_vect){
	OC1A_COMP_INT = 1;
	printf("\nComp1A!\n");
}

void AG_sequencer_16(){
	int notes[] = {440, 494, 523, 587, 659, 698, 784, 698, 659, 587, 523, 494, 440, 494, 523, 494};

	AG_init();
	int step = 0;
	int step_switch = 0;
	//TIMSK |= (1<<OCIE1A);
	
	for (uint8_t i = 0; i<16; i++){
		AG_set_freq(notes[i]);
		printf("Note: %d\n",notes[i]);
		step_switch = notes[i];
		
		for (int j = 0; j<100; j++)	{ for (int k = 0; k<7000; k++){} }
		/*
		while(step<step_switch){
			if (OC1A_COMP_INT == 1){				// Problem: Interrupts are waaay too frequent - system freezes. Consider changing clck
				OC1A_COMP_INT = 0;
				step++;
			}
		}
		*/
	}

	//TIMSK &= ~(1<<OCIE1A);
	AG_disable_osc();
}