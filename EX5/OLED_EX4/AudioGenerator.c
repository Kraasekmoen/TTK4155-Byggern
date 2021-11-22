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
		
		// Force compare will NOT reset timer in CTC mode (p 129)
		TCCR1A |= (1<<FOC1A);
		
		Interrupts:
			TIFR:
			TIMSK:

*/
//		--		--		--		//

#include <AudioGenerator.h>

const int TONE_A = 440;
const int TONE_As = 466;
const int TONE_B = 494;
const int TONE_C = 523;
const int TONE_D = 587;
const int TONE_E = 659;
const int TONE_F = 698;
const int TONE_G = 784;



void AG_init(){

	DDRD |= (1<<PD5);					// Enable output on osc pin
	
	TCCR1B |= (1<<WGM12);				// Set mode to CTC and TOP source to OCR1A
	TCCR1B |= (1<<CS11) | (1<<CS10);	// Set counter clock to clk_i/o / 64 = 76800Hz
	
	//TIMSK |= (1<<OCIE1A);				// Enable interrupts when Output Compare Match occurs in Timer/Counter1

	AG_enable_osc();

}

void AG_majorizer(int freq, int durtn){
	int freqs[] = {freq, 1.25*freq, 1.5*freq};
	AG_set_chord(freqs, 3, durtn);
}

void AG_octavizer(int freq, int durtn){
	int freqs[] = {0.5*freq, freq, 2*freq};
	AG_set_chord(freqs, 3, durtn);
}

void AG_set_freq(int freq){
	if (!(freq>0)){ printf("Invalid freq %d\n", freq); return;	}

	// Stop counter
	TCCR1B &= ~(1<<CS10 | 1<<CS11 | 1<<CS12);
	// Reset counter
	TCNT1L = 0;
	TCNT1H = 0;	
	
	//printf("freq:%d\n", freq);
	int trigger_step	= 76800/(2*freq);
	//printf("trg_stp:%d\n",trigger_step);
	
	uint8_t step_upper	= (uint8_t) (trigger_step >> 8);
	uint8_t step_lower	= (uint8_t) (trigger_step & 0b0000000011111111);
	//printf("sp_up, sp_lw: %d %d\n", step_upper, step_lower);
	
	OCR1AH	= step_upper;
	OCR1AL	= step_lower;

	// Start counter
	TCCR1B |= (1<<CS11) | (1<<CS10);	// Set counter clock to clk_i/o / 64 = 76800Hz
	
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

void AG_set_chord(int *freqs, int num_tones, int durtn){
	int v = 0;
	for (int j=0; j<durtn; j++){
		for (int k=0; k<num_tones; k++){
			AG_set_freq(freqs[k]);
			printf("Freq: %d\n",freqs[k]);
		}
	}
	AG_disable_osc();
}

void AG_sequencer_16(int *notes, int legato, int step_length, int majordize){

	AG_init();
	AG_disable_osc();
	int note = 0;


	if (majordize == 1)
	{
		for (uint8_t i = 0; i<16; i++){
			note = notes[i];
			if (note==0){
				AG_disable_osc();
			}
			else{
				AG_enable_osc();
				AG_majorizer(note,step_length/300);
			}
		}
	} 
	else
	{
		
		if (legato >= 1){
			for (uint8_t i = 0; i<16; i++){
				note = notes[i];
				if (note==0){
					AG_disable_osc();
				}
				else{
					AG_set_freq(note);
					AG_enable_osc();
				}
				printf("Note: %d\n",notes[i]);
				
				for (int j = 0; j<100; j++)	{ for (int k = 0; k<step_length; k++){} }
			}
		}
		else{
			for (uint8_t i = 0; i<16; i++){
				note = notes[i];
				if (note==0){
					AG_disable_osc();
				}
				else{
					AG_set_freq(note);
					AG_enable_osc();
				}
				printf("Note: %d\n",notes[i]);
				
				for (int j = 0; j<100; j++)	{ for (int k = 0; k<(step_length/2); k++){} }
				AG_disable_osc();
				for (int j = 0; j<100; j++)	{ for (int k = 0; k<(step_length/2); k++){} }
			}
		}
		
	}

	
	
	AG_disable_osc();
}

void AG_adv_sequencer_16(int *notes, int *legato, int *majordize, int step_length){

	AG_init();
	//AG_disable_osc();
	
	int note = 0;
	int chord = 0;
	int leg = 0;

	for (uint8_t i = 0; i<16; i++){
			note = notes[i];
			chord = majordize[i];
			leg = legato[i];
			
			
			// Legato on
			if (leg >= 1){
				// Majordize on
				if (chord >= 1){
					
					if (note==0){
						AG_disable_osc();
					}
					else{
						AG_enable_osc();
						AG_octavizer(note,step_length/300);
					}					
				}
				else{	// Majordize off
					
					if (note==0){
						AG_disable_osc();
					}
					else{
						AG_set_freq(note);
						AG_enable_osc();
					}
					for (int j = 0; j<100; j++)	{ for (int k = 0; k<step_length; k++){} }

					
				}
			}
			//Legato off
			else{
				// Majordize on
				if (chord >= 1){
					
					if (note==0){
						AG_disable_osc();
					}
					else{
						AG_enable_osc();
						AG_octavizer(note,step_length/300);
					}
				}
				else{	// Majordize off
					
					if (note==0){
						AG_disable_osc();
					}
					else{
						AG_set_freq(note);
						AG_enable_osc();
					}
					for (int j = 0; j<100; j++)	{ for (int k = 0; k<(step_length/2); k++){} }
					AG_disable_osc();
					for (int j = 0; j<100; j++)	{ for (int k = 0; k<(step_length/2); k++){} }
					
				}
				
				
			}
		}
	
	//AG_disable_osc();
}

/*
void AG_adv_sequencer_16(int *notes, int *majordize, int step_length){

	AG_init();
	AG_disable_osc();
	
	int note = 0;
	int chord = 0;
	int leg = 0;

	for (uint8_t i = 0; i<16; i++){
		note = notes[i];
		chord = majordize[i];		

		// Majordize on
		if (chord >= 1){
			
			if (note==0){
				AG_disable_osc();
			}
			else{
				AG_enable_osc();
				AG_majordizer(note,step_length/300);
			}
		}
		else{	// Majordize off
			
			if (note==0){
				AG_disable_osc();
			}
			else{
				AG_set_freq(note);
				AG_enable_osc();
			}
			for (int j = 0; j<100; j++)	{ for (int k = 0; k<step_length; k++){} }
		}
	}
	
	AG_disable_osc();
}
*/