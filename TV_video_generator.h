/*
 * TV_video_generator.h
 *
 * Created: 24.07.2017 23:56:57
 *  Author: integrator.sb@gmail.com
 */ 


#ifndef TV_VIDEO_GENERATOR_H_
#define TV_VIDEO_GENERATOR_H_

#define LEVEL_SYNC 0;
#define LEVEL_BLACK 256*0.3/5
#define LEVEL_WHITE 256*1/5
#define LEVEL_GRAY 256*0.6/5
#define LEVEL_GRAY1 256*0.4/5
#define LEVEL_GRAY2 256*0.8/5

enum{
	TV_VBARS,
	TV_LATTICE,
	TV_CHESSBOARD
};

//extern uint8_t buf[256];

//void tv_gen(void(*f)(void));
void tv_gen(uint8_t t);



#endif /* TV_VIDEO_GENERATOR_H_ */