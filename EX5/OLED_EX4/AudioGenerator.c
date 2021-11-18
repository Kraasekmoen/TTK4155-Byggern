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
		
		FCPU = 4915200Hz	= clk_i/o (?)			4915200/256 = 19200Hz
		
		Step length is determined by Beats per Minutes (BPM) value. Standard is 120. This equates to 2Hz. 
		
		

	Registers of note:
		Timer/Counter Control Registers A [TCCR1A] (and TCCR1B, which is also needed for the settings of Channel A)
			Mode: CTC	
			Set
				COM1A1:0 COM1A0:1	Toggle OC1A on Compare Match
				FOC1A: Force Output Compare; trigger match. Don't care about this one
				WGM1 1:0 in TCCR1A and WGM1 3:2 in TCCR1B: Together decides mode and source of counter-reset-value. Set WGM1[3]=0, WGM1[2]=1, WGM1[1]=0, WGM1[0]=0 for Mode:CTC, Source:OCR1A
				CS1[2:0] in TCCR1B: Decides Counter/Timer's clock source. Set CS1[2:0] to 100 will set clock to 19200Hz - sufficient for all notes
					! Speaker can only drive up to 10KHz anyway
		TCNT1H and TCNT1L: The two 8-bit registers that make up the 16-bit counter.
		OCR1AH and OCR1AL: The registers with the value the counter is compared against. Write your value to these!

*/
//		--		--		--		//
void AG_init(int bpm){
	
}

void AG_osc_init(){
	
	
}
void AG_timer_init(){
	
}

void AG_set_freq(int freq){
	
}