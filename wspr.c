#include <stdlib.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdbool.h> 
#include <inttypes.h>
#include <pigpio.h>
#include <sys/time.h>
#include<signal.h>
#include "wspr.h"
#include "si5351a.c"

struct sigaction old_action;

uint8_t WSPRsymbolSequence[]  = {3,1,2,2,0,0,2,0,1,0,2,2,3,3,1,0,2,2,1,0,0,1,2,1,1,3,3,0,2,2,0,0,0,0,1,0,0,3,0,3,0,2,2,2,0,2,1,2,1,1,2,2,3,1,2,3,2,0,2,1,3,0,1,0,2,0,2,1,1,0,1,2,3,0,3,0,1,0,2,3,2,0,1,2,1,3,2,2,0,1,1,2,3,0,3,2,2,0,3,0,0,0,2,2,3,0,0,1,2,0,3,1,1,0,3,1,0,2,3,1,0,3,2,0,2,1,1,1,2,2,0,2,2,1,2,1,2,0,3,3,0,0,0,2,2,2,0,1,3,2,1,2,1,3,0,2,0,1,1,2,2,2};
uint8_t FST4WsymbolSequence[] = {0,1,3,2,1,0,2,3,0,0,3,0,2,1,2,1,1,3,1,1,2,0,3,0,3,1,0,0,1,0,3,1,0,0,2,0,2,0,2,3,1,0,3,2,0,1,3,2,1,0,1,3,1,2,0,0,1,0,0,2,0,2,2,0,3,1,0,2,3,2,1,0,0,0,2,3,0,1,3,2,1,0,2,3,2,0,1,2,2,3,2,3,2,0,2,1,1,1,2,2,0,0,3,2,0,0,0,3,0,2,1,0,3,2,2,3,1,0,3,2,0,1,2,3,1,3,3,0,2,3,0,3,3,2,2,2,1,3,1,1,1,0,2,0,3,1,1,0,3,3,1,0,0,1,3,2,1,0,2,3};

 uint8_t  mult[]		={24,		24,			24,			24		};
 uint32_t num[]			={993881,	993883,		993885,		993887	};
 uint32_t denom[]		={1048575,	1048575,	1048575,	1048575	};
 uint32_t divider[]		={32,		32,			32,			32		};
 uint8_t  rDiv[]		={0,		0,			0,			0		};

unsigned long long millis() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    unsigned long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

// Wait for the system clock's minute to reach one second past 'minute'
void waitWindows(int minute){
  time_t t;
  struct tm* ptm;
  char ttime[5];
  for(;;){
    time(&t);
    ptm = gmtime(&t);
    if((ptm->tm_min % minute) == 0 && ptm->tm_sec == 0) break;
	int remaining_seconds = 120 - ((ptm->tm_min % minute) * 60 + ptm->tm_sec);
	snprintf(ttime, sizeof(ttime), "%3ds", remaining_seconds);
	printf("\b\b\b\b%s",ttime);
	fflush(stdout);
    usleep(1000);
  }
  usleep(1000000); // wait another second
}

void sigint_handler(int sig_no __attribute__((unused)))
{
	I2C_BUS=0;
	si5351aOutputOffAll();
	I2C_BUS=1;
	si5351aOutputOffAll();
    printf("CTRL-C pressed\n");
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}

int main(int argc, char **argv) {
	
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
	
	uint8_t wrc = 2;
	bool directsending = 0;
	
	int c;
	while ((c = getopt (argc, argv, "hw:d")) != -1)
			switch (c)
			{
			case 'h':
					printf ("wspr -d plughw:CARD=PCH,DEV=0 -r 192000 -i 60 -a 10 -x 10 -w 1024 -m 10 -o outputfile.bmp -v\n"); //$dsnoop:lp2220mXRP10,1
					printf ("-d send without wait\n-w number cycle waiting before sending\n");
					printf ("format I/Q f16_le only\n");
					exit(0);
					break;
			case 'w':
					wrc = atoi(optarg);
					break;	
			case 'd':
					directsending = 1;
					break;					
			default:
					abort ();
			}
			
	
	I2C_BUS=0;
	si5351ainit();
	si5351aOutputOffAll();
	
	I2C_BUS=1;
	si5351ainit();
	si5351aSetsetupPLLandsetupMultisynth(30, 0, 1048575, 6, 0); //12500000000
	si5351aOutputOffAll();
	
	usleep(2000000);
	
	unsigned long long startmillis;
	unsigned long long endmillis;
	
	uint8_t SYMBOL_COUNT = sizeof(WSPRsymbolSequence) / sizeof(WSPRsymbolSequence[0]);
	uint8_t actsymb = 0;
	
	while(true){

		if(!directsending){
			for(uint8_t i=0;i<=wrc;i++)
			{
				printf("Waiting for next transmition window in %d cycles and ",wrc);
				fflush(stdout);
				waitWindows(2);
				printf("\n");
			}
		}
		
		I2C_BUS=1;
		SetPower(1,SI_CLK0_CONTROL);
		
		I2C_BUS=0;
		
		startmillis = millis();
		
		for(uint8_t loop = 0; loop < SYMBOL_COUNT; loop++) 
		{
			endmillis = startmillis + ((loop + 1) * (unsigned long) 683) ;
			actsymb = WSPRsymbolSequence[loop];
			printf("%d ",actsymb);
			fflush(stdout);
			si5351aSetsetupPLLandsetupMultisynth(mult[actsymb],num[actsymb],denom[actsymb],divider[actsymb],rDiv[actsymb]); //1949050000
			while (millis() < endmillis){asm("nop");};
		}
		
		I2C_BUS=0;
		si5351aOutputOffAll();
		I2C_BUS=1;
		si5351aOutputOffAll();
		
	}
	
	I2C_BUS=0;
	si5351aOutputOffAll();
	
	I2C_BUS=1;
	si5351aOutputOffAll();
	
}

   
