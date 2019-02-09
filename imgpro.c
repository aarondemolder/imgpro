#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

//folder header
#include <sys/stat.h>

//counter declaration for batch editing
int a;

//input values declared
int brightness;
int contrast;
int threshold;
int highlight;
int glare;
double gammaVal;
int blurSize;
int sharpenVal;

//init SDL elements
SDL_Window * window = NULL;
SDL_Window * window1 = NULL;
SDL_Surface * image = NULL;
SDL_Texture * texture = NULL;
SDL_Renderer * renderer = NULL;
SDL_Renderer * renderer1 = NULL;
SDL_Surface * image2 = NULL;
SDL_Texture * texture2 = NULL;
SDL_Surface * original = NULL;
SDL_Texture * texture3 = NULL;

//struct folders for saving process
struct stat st = {0};

//init voids
void windowInit(SDL_Surface * image);
void saveImage(SDL_Surface * image, int a);
//init filter modes
void blackWhite(SDL_Surface * image);
void inversion(SDL_Surface * image);
void redFilter(SDL_Surface * image);
void greenFilter(SDL_Surface * image);
void blueFilter(SDL_Surface * image);
//init editing functions
void brightnessFun(SDL_Surface * image, int brightness);
void contrastFilter(SDL_Surface * image, int contrast);
void thresholdFilter(SDL_Surface * image, int threshold);
void highlightFilter(SDL_Surface * image, int highlight);
void glareFilter(SDL_Surface * image, int glare);
void gammaFilter(SDL_Surface * image, double gammaVal);
void blurFilter(SDL_Surface * image);
void unsharpMask(SDL_Surface * image);
//init image transfer
void RGBtoLAB(Uint8, Uint8, Uint8, double *LABvaluesArray);
void LABtoRGB(double lValue, double aValue, double labBValue, int *RGBArray);
void imgTransfer(SDL_Surface * image, SDL_Surface * image2, SDL_Surface * original);


int main(int argc, char ** argv)
{
	bool quit = false;
	SDL_Event event;

	//init (SDL)
	SDL_Init(SDL_INIT_VIDEO);
	
	//if no args input, run through steps
	if(argc==1)
	{
		//definitions for user inputs
		char imageSource[1000]; //really long file locations?
		char filterMode[3];
		
		int brightnessVal =0;
		int* brightness = &brightnessVal;
		
		int contrastVal =0;
		int* contrast = &contrastVal;
		
		int blurVal =0;
		int* blurSize = &blurVal;
		
		int sharpVal =0;
		int* sharpenVal = &sharpVal;
		
		double gammavalue =0;
		double* gammaVal = &gammavalue;
		
		int highlightVal =0;
		int* highlight = &highlightVal;
		
		int thresholdVal =0;
		int* threshold = &thresholdVal;
		
		int glareVal =0;
		int* glare = &glareVal;
		
		//if no filename defined in commandline
		printf("\nBasic Editing Enabled, for Image Transfer & Batch Edit, please read the documentation.\n");
		printf("Frame Input - ");
		scanf("%s", imageSource);
		fflush(stdin);
		
		//loads image
		image = IMG_Load( imageSource );
		if (image!=NULL) printf("Image '%s' Loaded\n", imageSource);
		else
		{
			printf("\nImage '%s' could not load, please check your spelling.\n", imageSource);
			printf("Quitting Image Processor.\n");
			
			SDL_Quit();
			exit(0);
		}
		
		//image mode
		printf("Filter Mode - ");
		scanf("%s", filterMode);
		fflush(stdin);
		
		//values input
		printf("Brightness Value (-255 to 255) - ");
		scanf("%d", brightness);
		fflush(stdin);
		
		printf("Contrast Value (-255 to 255) - ");
		scanf("%d", contrast);
		fflush(stdin);

		printf("Blur Value (0 to 500) - ");
		scanf("%d", blurSize);
		fflush(stdin);
		
		printf("Sharpen Value (0 to 500) - ");
		scanf("%d", sharpenVal);
		fflush(stdin);
		
		printf("Highlight Value (0 to 255) - ");
		scanf("%d", highlight);
		fflush(stdin);
		
		printf("Glare Value (0 to 255) - ");
		scanf("%d", glare);
		fflush(stdin);
		
		printf("Threshold Value (0 to 255) - ");
		scanf("%d", threshold);
		fflush(stdin);
		
		printf("Gamma Value (No change = 1.0, otherwise 0.2 to 5.0) - ");
		scanf("%lf", gammaVal);
		fflush(stdin);
		
		//prepares window for preview
		windowInit(image);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
	
		//converts the surface into pixel format (ARGB) A is never used so ignored later
		SDL_Surface * originalImage = image;
		image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_FreeSurface(originalImage);

		//typecasts pixels from void to Uint32 to allow individual pixel editing/access
		Uint32 * pixels = (Uint32 *)image->pixels;

		//applies filters
		if(!strncmp(filterMode,"bw",2))
		{
			printf("Greyscale Filter Selected\n");
			blackWhite(image);
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);

		}
		
		else if (!strncmp(filterMode,"rf",2))
		{
			printf("Red Filter Selected\n");
			redFilter(image);
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);
		}
		
		else if (!strncmp(filterMode,"gf",2))
		{
			printf("Green Filter Selected\n");
			greenFilter(image);
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);
		}
		
		else if (!strncmp(filterMode,"bf",2))
		{
			printf("Blue Filter Selected\n");
			blueFilter(image);
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);
		}
		
		else if (!strncmp(filterMode,"in",2))
		{
			printf("Inversion Filter Selected\n");
			inversion(image);
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);
		}
		
		else if (!strncmp(filterMode,"st",2))
		{
			printf("Standard Editing Selected\n");
			if(*brightness!=0) brightnessFun(image, *brightness);
			if(*contrast!=0) contrastFilter(image, *contrast);
			
			int blurLoop = *blurSize/5;
			for (int bV = 1; bV < blurLoop; bV=bV+1)
			{
				blurFilter(image);
			}
			
			int sharpLoop = *sharpenVal/5;
			for (int bV = 1; bV < sharpLoop; bV=bV+1)
			{
				unsharpMask(image);
			}
			
			if(*highlight!=0) highlightFilter(image, *highlight);
			if(*glare!=0) glareFilter(image, *glare);
			gammaFilter(image, *gammaVal);
			if(*threshold!=0) thresholdFilter(image, *threshold);
		}
		else 
		{
			printf("Please refer to the documentation for correct input usage.\n");
			printf("Quitting Image Processor.\n");
			
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
			SDL_DestroyRenderer(renderer);

			SDL_Quit();
			exit(0);
		}
	}
	//end of argc==1


	//image transfer filter mode
	else if(argc==14)
	{
		//IMAGE TRANSFER
		printf("\nAdvanced Editing Enabled\n");
		if(!strncmp(argv[2],"tr",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[13], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[13]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[12], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[12], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image 
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);

				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				//LOADS SOURCE IMAGE
				image2 = IMG_Load( argv[3] );
				if (a==1)
				{
					if (image2!=NULL) printf("Image '%s' Loaded\n", argv[3]);
					else 
					{
						printf("\nImage '%s' could not load, please check your spelling.\n", argv[2]);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
				
				//loads target image again to a new surface so values do not have to be stored meanwhile for image transfer
				if (frameMaxInt==0) original = IMG_Load( argv[1] );
				else original = IMG_Load( strC );
				original = SDL_ConvertSurfaceFormat(original, SDL_PIXELFORMAT_ARGB8888, 0);

				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				SDL_Surface * originalImage2 = image2;
				SDL_Surface * originalImage3 = original;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				image2 = SDL_ConvertSurfaceFormat(image2, SDL_PIXELFORMAT_ARGB8888, 0);
				original = SDL_ConvertSurfaceFormat(original, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				SDL_FreeSurface(originalImage2);
				SDL_FreeSurface(originalImage3);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				Uint32 * pixels2 = (Uint32 *)image2->pixels;
				Uint32 * pixels3 = (Uint32 *)original->pixels;
				
				//declaring argv numbers for each input
				double brightness = atoi(argv[4]);
				double contrast = atoi(argv[5]);
				double blurSize = atoi(argv[6]);
				double sharpenVal = atoi(argv[7]);
				double gammaVal = atof(argv[8]);
				double highlight = atoi(argv[9]);
				double glare = atoi(argv[10]);
				double threshold = atoi(argv[11]);
				
				//creates window but doesn't display so can process before a preview
				window1 = SDL_CreateWindow("Grade Preview", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_HIDDEN);
				renderer1 = SDL_CreateRenderer(window1, -1, 0);
				
				//image transfer begin (longest process)
				imgTransfer(image, image2, original);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);

				
				if(!strncmp(argv[12],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[12],"sr",2))
				{
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_FreeSurface(image2);
					SDL_FreeSurface(original);
					SDL_DestroyWindow(window);
					SDL_DestroyWindow(window1);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
				
			}
			//end of for loop for multiple frames
		}
		else
		{
			printf("Please refer to the documentation for correct input usage.\n");
			printf("Quitting Image Processor.\n");
			
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
			SDL_DestroyRenderer(renderer);
	
			SDL_Quit();
			exit(0);
		}
	}
	//end argc[13] (image transfer)
	

	//start argc[12] (black and white, red, green, blue, and inversion.)
	else if(argc==13)
	{
		//black and white + process
		if(!strncmp(argv[2],"bw",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//makes image black and white
				blackWhite(image);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);

				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
			}
		}
		
		//red filter + process
		else if(!strncmp(argv[2],"rf",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//makes image red
				redFilter(image);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input (move to end?)
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);
				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
					
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work#
			}
		}
		
		//green filter + process
		else if(!strncmp(argv[2],"gf",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//makes image green
				greenFilter(image);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);
				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
					
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
			}
		}
		
		//blue filter + process
		else if(!strncmp(argv[2],"bf",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//makes image blue
				blueFilter(image);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);
				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
					
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
			}
		}
		
		//inversion filter + process
		else if(!strncmp(argv[2],"in",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//makes image inverted
				inversion(image);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input (move to end?)
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);
				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
					
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
				else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
			}
		}

		//standard process
		else if(!strncmp(argv[2],"st",2))
		{
			//file counter for multiple frames
			char * strA = argv[1], * frameMaxS = argv[12], strC[50];
			int filenameLength = strlen(argv[1]);
			int x = filenameLength-4;
			int frameMaxInt = atoi(argv[12]);
			char countStr[10];
			
			//if preview mode enabled, disable image sequences
			if(!strncmp(argv[11], "pr", 2))
			{
				frameMaxInt=1;
				printf("Preview mode only displays the first image of a sequence.\n");
			}
			
			//if save mode enabled, ensure frames start on
			if(!strncmp(argv[11], "sr", 2))
			{
				if (frameMaxInt==0) 
				{
					frameMaxInt=1;
					printf("Cannot start on frame 0, setting to 1.\n");
				}
			}
			
			//loop for image sequences - set to run once if preview is enabled
			for (a = 1; a<frameMaxInt+1; ++a)
			{
				//joins strings and frame number so to read image
				sprintf(countStr, "%d", a);
				strncpy(strC,strA,x);
				strC[x] = '\0';
				strcat(strC,countStr);
				strcat(strC,strA+x);
			
				//ARGV1 LOADS TARGET IMAGE
				image = IMG_Load( strC );
				if (a==1)
				{
					//on only first run, print if loading images was a success
					if (image!=NULL) printf("Image '%s' Loaded\n", argv[1]);
					else 
					{
						//if loading images was not successful, exit
						printf("\nImage '%s' could not load, please check your spelling.\n", strC);
						printf("Exiting.\n");
						SDL_Quit();
						exit(0);
					}
				}
				
				//if couldn't load frame after frame1
				if (image==NULL) 
				{
					printf("Could not load %s\n", strC);
					printf("Exiting.\n");
					SDL_Quit();
					exit(0);
				}
				
				printf("Processing Frame: %s of %d\n", countStr, frameMaxInt);
			
				//converts the surfaces into pixel format (ARGB) A is never used so ignored later
				SDL_Surface * originalImage = image;
				image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
				SDL_FreeSurface(originalImage);
				
				//typecasts pixels from void to Uint32 to allow individual pixel editing/access
				Uint32 * pixels = (Uint32 *)image->pixels;
				
				double brightness = atoi(argv[3]);
				double contrast = atoi(argv[4]);
				double blurSize = atoi(argv[5]);
				double sharpenVal = atoi(argv[6]);
				double gammaVal = atof(argv[7]);
				double highlight = atoi(argv[8]);
				double glare = atoi(argv[9]);
				double threshold = atoi(argv[10]);
				
				//brightness input
				if(brightness!=0) brightnessFun(image, brightness);
				
				//contrast input
				if(contrast!=0) contrastFilter(image, contrast);
				
				//blur input
				if(blurSize!=0)
				{
					int blurLoop = blurSize/5;
					for (int bV = 1; bV < blurLoop; bV=bV+1)
					{
						blurFilter(image);
					}
				}
				
				//sharpen input
				if(sharpenVal!=0)
				{
					int sharpLoop = sharpenVal/5;
					for (int bV = 1; bV < sharpLoop; bV=bV+1)
					{
						unsharpMask(image);
					}
				}
				
				//highlights input
				if(highlight!=0) highlightFilter(image, highlight);
				
				//glare input
				if(glare!=0) glareFilter(image, glare);
				
				//gamma input
				if(gammaVal!=1) gammaFilter(image, gammaVal);
				
				//threshold input
				if(threshold!=0) thresholdFilter(image, threshold);
				
				if(!strncmp(argv[11],"pr",2))
				{
					printf("Preview Window Opened.\n");
					//close hidden window so the exit button works on preview window
					SDL_DestroyRenderer(renderer1);
					SDL_DestroyWindow(window1);
					
					//open preview window
					windowInit(image);
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
				}
				
				else if (!strncmp(argv[11],"sr",2))
				{
					texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);
					
					//saves image to process folder
					saveImage(image, a);
					
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
		
					SDL_Quit();
					if (frameMaxInt==a) 
					{
						printf("Frame process complete.\n");
						exit(0);
					}
				}
			else printf("Please use 'pr' or 'sr' to preview or save the result respectively"); //this doesn't work
			}
		}

		else
		{
			printf("Please refer to the documentation for correct input usage.\n");
			printf("Quitting Image Processor.\n");
			
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
			SDL_DestroyRenderer(renderer);
	
			SDL_Quit();
			exit(0);
		}
	}
	//end argc[12] (all other filters)

	else
	{
		printf("Please refer to the documentation for correct input usage.\n");
		printf("Quitting Image Processor.\n");
		
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(image);
		SDL_DestroyRenderer(renderer);

		SDL_Quit();
		exit(0);
	}

	//while window is open so only for preview mode
	while (!quit)
	{
		//includes texture edit in view loop for button presses
		SDL_UpdateTexture(texture, NULL, image->pixels, image->w * sizeof(Uint32));
		SDL_WaitEvent(&event);
 
		switch (event.type)
		{
			
			case SDL_QUIT:
			{
				quit = true;
				break;
			}
			
			//key cases for whilst preview window is open!
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					//save with s key
					case SDLK_s:
					{
						printf("Saving Image.\n");
						saveImage(image, a);
						break;
					}
					//quit with q key
					case SDLK_q:
					{
						printf("Exiting Image Processsor.\n");
						quit = true;
						break;
					}
				}
			}
		}
		
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
	}

	//exit procedure (SDL)
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture2);
	SDL_FreeSurface(image2);
	SDL_DestroyTexture(texture3);
	SDL_FreeSurface(original);
	SDL_DestroyWindow(window);
	
	SDL_Quit();
 
	return 0;
}

//window initialisation for standard procedures
void windowInit(SDL_Surface * image)
{
	//creates window after image input
	window = SDL_CreateWindow("Grade Preview", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
}

//checks for processed folder and saves images into it //linux only due to mkdir
void saveImage(SDL_Surface * image, int a)
{
	if (stat("processed", &st) == -1) {
		system("mkdir processed");
	}
	
	char countStr[10000]; ///someone might want to edit a lot of images
	char savePath[500]= "processed/image";
	
	//merges frame number with image name and adds .ext
	sprintf(countStr, "%d", a);
	strcat(savePath,countStr);
	strcat(savePath, ".bmp");
	SDL_SaveBMP(image, savePath);
}

///////////////////////
/*FILTERS BEYOND HERE*/
///////////////////////

//Black and White Mode
void blackWhite(SDL_Surface * image)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
		
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;

			//natural conversion (better!)
			Uint8 v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
								
			pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
			pixels[y * image->w + x] = pixel;
		}
	}
}

//Inversion Mode
void inversion(SDL_Surface * image)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			//splits channels
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//inverts each channel
			Uint8 vr = 255-r;
			Uint8 vg = 255-g;
			Uint8 vb = 255-b;
			
			//merges channels
			pixel = (0xFF << 24) | (vr << 16) | (vg << 8) | vb;
			pixels[y * image->w + x] = pixel;
		}
	}
}

//Red Filter
void redFilter(SDL_Surface * image)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			//splits channels
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//sets all but red to 0
			Uint8 cfr = r;
			Uint8 cfg = 0;
			Uint8 cfb = 0;
			
			pixel = (0xFF << 24) | (cfr << 16) | (cfg << 8) | cfb; 
			pixels[y * image->w + x] = pixel;
		}
	}	
}

//Green Filter
void greenFilter(SDL_Surface * image)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			//splits channels
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//sets all but green to 0
			Uint8 cfr = 0;
			Uint8 cfg = g;
			Uint8 cfb = 0;
			
			pixel = (0xFF << 24) | (cfr << 16) | (cfg << 8) | cfb; 
			pixels[y * image->w + x] = pixel;
		}
	}	
}

//Blue Filter
void blueFilter(SDL_Surface * image)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			//splits channels
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//sets all but blue to 0
			Uint8 cfr = 0;
			Uint8 cfg = 0;
			Uint8 cfb = b;
			
			pixel = (0xFF << 24) | (cfr << 16) | (cfg << 8) | cfb; 
			pixels[y * image->w + x] = pixel;
		}
	}	
}

//Brightness
void brightnessFun(SDL_Surface * image, int brightness)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	//input clamps
	if (brightness<-255) brightness = -255;
	if (brightness>255) brightness = 255;

	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//adds brightnes value to image value
			int bR = r+brightness;
			int bG = g+brightness;
			int bB = b+brightness;
			
			//clamps
			if (bR < 0) bR = 1;
			if (bR >255) bR = 255;
			if (bG < 0) bG = 1;
			if (bG >255) bG = 255;
			if (bB < 0) bB = 1;
			if (bB >255) bB = 255;	
			
			pixel = (0xFF << 24) | (bR << 16) | (bG << 8) | bB;
			pixels[y * image->w + x] = pixel;
		}
	}	
}	

//Contrast
void contrastFilter(SDL_Surface * image, int contrast) 
{
	//input clamps
	if (contrast < -100) contrast = -100;
	if (contrast > 100) contrast = 100;
	
	contrast = (100.0 + contrast) / 100.0;
	contrast *= contrast;
	int y, x;
	
	Uint32 * pixels = (Uint32 *)image->pixels;
	for (y=0; y< image ->h; y++)
	{
		for (x = 0; x< image->w; x++)
		{
			Uint8 r,g,b; 
			SDL_GetRGB(pixels[x+y*image->w],image->format,&r,&g,&b);
			
			double redContrast = r / 255.0;
			redContrast -= 0.5;
			redContrast *= contrast;
			redContrast += 0.5;
			redContrast *= 255;
			if (redContrast < 0) redContrast = 0;
			if (redContrast > 255) redContrast = 255;

			double greenContrast = g / 255.0;
			greenContrast -= 0.5;
			greenContrast *= contrast;
			greenContrast += 0.5;
			greenContrast *= 255;
			if (greenContrast < 0) greenContrast = 0;
			if (greenContrast > 255) greenContrast = 255;

			double blueContrast = b / 255.0;
			blueContrast -= 0.5;
			blueContrast *= contrast;
			blueContrast += 0.5;
			blueContrast *= 255;
			if (blueContrast < 0) blueContrast = 0;
			if (blueContrast > 255) blueContrast = 255;

			Uint32 pixel=SDL_MapRGB(image->format,redContrast,greenContrast,blueContrast);
			pixels[x+y*image->w]=pixel;
		}
	}
}

//thresholding
void thresholdFilter(SDL_Surface * image, int threshold)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	for (y=0; y< image ->h; y++)
	{
		for (x = 0; x< image->w; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			//clamps
			if (r > threshold) r = 255;
			if (r < threshold) r = 0;
			if (g > threshold) g = 255;
			if (g < threshold) g = 0;
			if (b > threshold) b = 255;
			if (b < threshold) b = 0;
			
			//merge clamped values
			Uint8 v = r + g + b;
			
			pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;
			pixels[y * image->w + x] = pixel;
			
		}
	}
}

//highlights
void highlightFilter(SDL_Surface * image, int highlight)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	//input clamps
	if (highlight>255) highlight = 255;
	if (highlight<0) highlight = 0;

	for (y=0; y< image ->h; y++)
	{
		for (x = 0; x< image->w; x++)
		{
			//different pixel seperation method using SDL!
			Uint8 r,g,b; 
			SDL_GetRGB(pixels[x+y*image->w],image->format,&r,&g,&b);
			
			//minuses the highlight limit value from the rgb values
			int redHighlight = r - highlight;
			if (redHighlight<0) redHighlight =0;

			int greenHighlight = g - highlight;
			if (greenHighlight<0) greenHighlight =0;
			
			int blueHighlight = b - highlight;
			if (blueHighlight<0) blueHighlight =0;
			
			Uint32 pixel=SDL_MapRGB(image->format, redHighlight, greenHighlight, blueHighlight);
			pixels[x+y*image->w]=pixel;
			
		}
	}
}

//glare
void glareFilter(SDL_Surface * image, int glare)
{
	int y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	//input clamps
	if (glare>255) glare = 255;
	if (glare<0) glare = 0;

	for (y=0; y< image ->h; y++)
	{
		for (x = 0; x< image->w; x++)
		{
			Uint8 r,g,b;
			SDL_GetRGB(pixels[x+y*image->w],image->format,&r,&g,&b);
			
			//copy of original values
			int r1 = r;
			int g1 = g;
			int b1 = b;
			
			//same method as highlight but adds to original image
			int rRed = (r - glare);
			if (rRed<0) rRed=0;
			int resultRed = rRed +r1;
			
			int rGreen = (g - glare);
			if (rGreen<0) rGreen=0;
			int resultGreen = rGreen +g1;
			
			int rBlue = (b - glare);
			if (rBlue<0) rBlue=0;
			int resultBlue = rBlue +b1;
			
			//extra clamps
			if (resultRed<0) resultRed =0;
			if (resultGreen<0) resultGreen =0;
			if (resultBlue<0) resultBlue =0;
			
			if (resultRed>255) resultRed =255;
			if (resultGreen>255) resultGreen =255;
			if (resultBlue>255) resultBlue =255;
			
			Uint32 pixel=SDL_MapRGB(image->format, resultRed, resultGreen, resultBlue);
			pixels[x+y*image->w]=pixel;
			
		}
	}
}

//gamma, based on http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-6-gamma-correction/
void gammaFilter(SDL_Surface * image, double gammaVal)
{
	int y, x;
	double gammaCorrection;
	
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	gammaCorrection = 1 / gammaVal;
	
	for (y=0; y< image ->h; y++)
	{
		for (x = 0; x< image->w; x++)
		{
			Uint8 r,g,b;
			SDL_GetRGB(pixels[x+y*image->w],image->format,&r,&g,&b);

			//doubles didn't work, floats did?
			float newRed = (float)255 * pow(((float)r / (float)255), (float)gammaCorrection);
			if (newRed>255) newRed = 255;
			
			float newGreen = (float)255 * pow(((float)g / (float)255), (float)gammaCorrection);
			if (newGreen>255) newGreen = 255;
			
			float newBlue = (float)255 * pow(((float)b / (float)255), (float)gammaCorrection);
			if (newBlue>255) newBlue = 255;

			
			Uint32 pixel=SDL_MapRGB(image->format, newRed, newGreen, newBlue);
			pixels[x+y*image->w]=pixel;
		}
	}
}

//blur
void blurFilter(SDL_Surface * image) //blurs radius of 5, reruns to increase by 5 value
{
	int rNewVal, gNewVal, bNewVal, y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	//blur kernel
	float blurMat[5][5] = 
	{
		{1.0, 4.0, 7.0, 4.0, 1.0},
		{4.0, 16.0, 26.0, 16.0, 4.0},
		{7.0, 26.0, 41.0, 26.0, 7.0},
		{4.0, 16.0, 26.0, 16.0, 4.0},
		{1.0, 4.0, 7.0, 4.0, 1.0},
	};
	
	for (int y = 2; y < image ->h-2; y++)
	{
		for (int x = 2; x < image ->w-2; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			float sumR = 0;
			float sumG = 0;
			float sumB = 0;
			
			for (int i = -2; i <= 2; i++) //runs through kernel matrix (left-right)
			{
				for (int j = -2; j <= 2; j++) //runs through up-down
				{ 
					SDL_GetRGB(pixels[(x+j)+(y+i)*image->w],image->format,&r,&g,&b); //reload pixel values
					
					//multiple values by matrix
					float red=(float)(r*blurMat[i+2][j+2]);
					float green=(float)(g*blurMat[i+2][j+2]);
					float blue=(float)(b*blurMat[i+2][j+2]);
					
					//sum results
					sumR+=red;
					sumG+=green;
					sumB+=blue;
				}
			}
			
			//divide by matrix weighting
			rNewVal = sumR/273;
			gNewVal = sumG/273;
			bNewVal = sumB/273;

			pixel = (0xFF << 24) | (rNewVal << 16) | (gNewVal << 8) | bNewVal;
			pixels[x+y*image->w]=pixel;
		}
	}
}

//sharpen, based on blur filter to create an unsharp mask
void unsharpMask(SDL_Surface * image) //blurs radius of 5, reruns to increase by 5 value, creates mask using original image and then adds to original
{
	int rNewVal, gNewVal, bNewVal, y, x;
	Uint32 * pixels = (Uint32 *)image->pixels;
	
	float blurMat[5][5] = 
	{
		{1.0, 4.0, 7.0, 4.0, 1.0},
		{4.0, 16.0, 26.0, 16.0, 4.0},
		{7.0, 26.0, 41.0, 26.0, 7.0},
		{4.0, 16.0, 26.0, 16.0, 4.0},
		{1.0, 4.0, 7.0, 4.0, 1.0},
	};
	
	for (int y = 2; y < image ->h-2; y++)
	{
		for (int x = 2; x < image ->w-2; x++)
		{
			Uint32 pixel = pixels[y * image->w + x];
			Uint8 r = pixel >> 16 & 0xFF;
			Uint8 g = pixel >> 8 & 0xFF;
			Uint8 b = pixel & 0xFF;
			
			int r1= r;
			int g1= g;
			int b1= b;
			
			float sumR = 0;
			float sumG = 0;
			float sumB = 0;
			
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{ 
					SDL_GetRGB(pixels[(x+j)+(y+i)*image->w],image->format,&r,&g,&b);
					
					float red=(float)(r*blurMat[i+2][j+2]);
					float green=(float)(g*blurMat[i+2][j+2]);
					float blue=(float)(b*blurMat[i+2][j+2]);
					
					sumR+=red;
					sumG+=green;
					sumB+=blue;
				}
			}
			
			rNewVal = sumR/273;
			gNewVal = sumG/273;
			bNewVal = sumB/273;
			
			//creates mask
			int subtractionR = r1 - rNewVal;
			int subtractionG = g1 - gNewVal;
			int subtractionB = b1 - bNewVal;
			
			//adds to original image
			int addR = subtractionR + r1;
			int addG = subtractionG + g1;
			int addB = subtractionB + b1;
			
			//more clamps
			if (addR > 255) addR = 255;
			if (addG > 255) addG = 255;
			if (addB > 255) addB = 255;
			if (addR < 0) addR = 0;
			if (addG < 0) addG = 0;
			if (addB < 0) addB = 0;
			
			pixel = (0xFF << 24) | (addR << 16) | (addG << 8) | addB;
			pixels[x+y*image->w]=pixel;
		}
	}
}

//Image Transfer
void imgTransfer(SDL_Surface * image, SDL_Surface * image2, SDL_Surface * original)
{
	int x,y;
	Uint8 targetR, targetG, targetB;
   
	Uint32 *pixels = (Uint32 *)image->pixels;
	Uint32 pixel;
	
	double targetLsum, targetAsum, targetBsum, sourceLsum, sourceAsum, sourceBsum = 0.0;
	
	//defining total pixel count
	double targetWidth = image->w;
	double targetHeight = image->h;
	double aSizeTarget = targetWidth * targetHeight;
	
	double LABvaluesArray[3];
	int RGBArray[3];
	
	//RGB to LAB conversion for target image, also performs sum for the means values
	for (y=0; y<image->h; y++)
	{
		for (x = 0; x<image->w; x++)
		{
			pixel = pixels[y * image-> w + x];
			
			SDL_GetRGB(pixels[x+y*image->w],image->format,&targetR,&targetG,&targetB);
			
			RGBtoLAB(targetR, targetG, targetB, LABvaluesArray);
			
			targetLsum += LABvaluesArray[0];
			targetAsum += LABvaluesArray[1];
			targetBsum += LABvaluesArray[2];
		}
	}
	
	//mean values of each target channel
	double targetLmean = targetLsum/aSizeTarget;
	double targetAmean = targetAsum/aSizeTarget;
	double targetBmean = targetBsum/aSizeTarget;
	
	//resets sums to zero 
	targetLsum = 0.0;
	targetAsum = 0.0;
	targetBsum = 0.0;
	
	//Creates standard deviation values
	for(y=0;y<image->h;y++)
	{
		for(x=0;x<image->w;x++)
		{
			pixel = pixels[y * image->w + x];
			SDL_GetRGB(pixels[x+y*image->w],image->format, &targetR, &targetG, &targetB);
			
			//converts RGB to LAB
			RGBtoLAB(targetR, targetG, targetB, LABvaluesArray);
			
			//means LAB values
			double targetmeanfromL = LABvaluesArray[0] - targetLmean;
			double targetmeanfromA = LABvaluesArray[1] - targetAmean;
			double targetmeanfromB = LABvaluesArray[2] - targetBmean;
			
			targetLsum += pow(targetmeanfromL,2.0);
			targetAsum += pow(targetmeanfromA,2.0);
			targetBsum += pow(targetmeanfromB,2.0);
		}
	}
	
	//Standard deviation values
	double TargetDeviationL = targetLsum/aSizeTarget;
	double standardTargetDeviationL = sqrt(TargetDeviationL);
	
	double TargetDeviationA = targetAsum/aSizeTarget;
	double standardTargetDeviationA = sqrt(TargetDeviationA);
	
	double TargetDeviationB = targetBsum/aSizeTarget;
	double standardTargetDeviationB = sqrt(TargetDeviationB);
	
	//commence source image calculations
	Uint8 sourceR, sourceG, sourceB;
	Uint32 *sourcepixels = (Uint32 *)image2->pixels;
	Uint32 sourcepixel;
	
	//source pixel count
	double sourceWidth = image2->w;
	double sourceHeight = image2->h;
	double aSizeSource = sourceWidth * sourceHeight;
	
	//source image sum and LAb conversion
	for (y=0; y<image2->h; y++)
	{
		for (x = 0; x<image2->w; x++)
		{
			sourcepixel = sourcepixels[y * image2-> w + x];
			SDL_GetRGB(sourcepixels[x+y*image2->w],image2->format,&sourceR,&sourceG,&sourceB);

			RGBtoLAB(sourceR, sourceG, sourceB, LABvaluesArray);
			
			sourceLsum += LABvaluesArray[0];
			sourceAsum += LABvaluesArray[1];
			sourceBsum += LABvaluesArray[2];
		}
	}
	
	//source image mean
	double sourceLmean = sourceLsum/aSizeSource;
	double sourceAmean = sourceAsum/aSizeSource;
	double sourceBmean = sourceBsum/aSizeSource;
	
	//reset sums
	sourceLsum = 0;
	sourceAsum = 0;
	sourceBsum = 0;
	
	//source standard deviation for all pixels
	for(y=0;y<image2->h;y++)
	{
		for(x=0;x<image2->w;x++)
		{
			sourcepixel = sourcepixels[y * image2->w + x];
			SDL_GetRGB(sourcepixels[x+y*image2->w],image2->format, &sourceR, &sourceG, &sourceB);
			
			RGBtoLAB(sourceR, sourceG, sourceB, LABvaluesArray);
			
			double sourcemeanfromL = LABvaluesArray[0] - sourceLmean;
			double sourcemeanfromA = LABvaluesArray[1] - sourceAmean;
			double sourcemeanfromB = LABvaluesArray[2] - sourceBmean;
			
			sourceLsum += pow(sourcemeanfromL,2.0);
			sourceAsum += pow(sourcemeanfromA,2.0);
			sourceBsum += pow(sourcemeanfromB,2.0);
		}
	}
	
	//std dev calculations
	double SourceDeviationL = sourceLsum/aSizeSource;
	double standardSourceDeviationL = sqrt(SourceDeviationL);
	
	double SourceDeviationA = sourceAsum/aSizeSource;
	double standardSourceDeviationA = sqrt(SourceDeviationA);
	
	double SourceDeviationB = sourceBsum/aSizeSource;
	double standardSourceDeviationB = sqrt(SourceDeviationB);
	
	//reset sums
	double originalLsum = 0.0;
	double originalAsum = 0.0;
	double originalBsum = 0.0;
	
	//target image loaded again so to merge with calculated values
	Uint32 *originalpixels = (Uint32 *)original->pixels;
	Uint32 originalpixel;
	Uint8 originalR, originalG, originalB;
	
	//defining maximum values for conversions (clamps)
	double maxL=10000, maxA=10000, maxlabB=10000;
	Uint8 maxR=255, maxG=255, maxB=255;
	double minL=0, minA=0, minlabB=0;
	Uint8 minR=0, minG=0, minB=0;
	
	//run through original image and apply calculated results
	for (y=0; y<original->h; y++)
	{
		for (x =0; x<original->w; x++)
		{
			//load original image
			originalpixel = originalpixels[y * original-> w + x];
			SDL_GetRGB(originalpixels[x+y*original->w],original->format,&originalR,&originalG,&originalB);
			
			//convert RGB to LAB
			RGBtoLAB(originalR, originalG, originalB, LABvaluesArray);
			
			//clamp LAB values returned in arrays
			if(LABvaluesArray[0] > maxL) maxL = LABvaluesArray[0];
			if(LABvaluesArray[1] > maxA) maxA = LABvaluesArray[1];
			if(LABvaluesArray[2] > maxB) maxlabB = LABvaluesArray[2];
			
			if(LABvaluesArray[0] < minL) minL = LABvaluesArray[0];
			if(LABvaluesArray[1] < minA) minA = LABvaluesArray[1];
			if(LABvaluesArray[2] < minB) minlabB = LABvaluesArray[2];
			
			//Take away the mean of the target image channels
			originalLsum = LABvaluesArray[0] - targetLmean;
			originalAsum = LABvaluesArray[1] - targetAmean;
			originalBsum = LABvaluesArray[2] - targetBmean;
			
			//Divide standard deviations
			double targetLbySourceL = standardTargetDeviationL/standardSourceDeviationL;
			double targetAbySourceA = standardTargetDeviationA/standardSourceDeviationA;
			double targetBbySourceB = standardTargetDeviationB/standardSourceDeviationB;
			
			//Multiply together
			double optionL = targetLbySourceL * originalLsum;
			double optionA = targetAbySourceA * originalAsum;
			double optionB = targetBbySourceB * originalBsum;
			
			//Add in the mean of the source image
			optionL += sourceLmean;
			optionA += sourceAmean;
			optionB += sourceBmean;
			
			//Convert back to RGB
			LABtoRGB(optionL, optionA, optionB, RGBArray);
			
			int newRed = RGBArray[0];
			int newGreen = RGBArray[1];
			int newBlue = RGBArray[2];
			
			//final RGB clamps (uses : ? method)
			newRed = newRed <= 0 ? 0:newRed;
			newRed = newRed >= 255 ? 255:newRed;
			
			newGreen = newGreen <= 0 ? 0:newGreen;
			newGreen = newGreen >= 255 ? 255:newGreen;
			
			newBlue = newBlue <= 0 ? 0:newBlue;
			newBlue = newBlue >= 255 ? 255:newBlue;
			
			newRed = (Uint8)newRed;
			newGreen = (Uint8)newGreen;
			newBlue = (Uint8)newBlue;

			//return to image output
			pixel = SDL_MapRGB(original->format,newRed,newGreen,newBlue);
			pixels[y * image->w + x] = pixel;
		}
	}
}


//RGB TO LAB conversion function
void RGBtoLAB(Uint8 red, Uint8 green, Uint8 blue, double *LABarray)
{
	//normalising RGB values
	double redValue = red/255.0;
	double greenValue = green/255.0;
	double blueValue = blue/255.0;
	
	//apply conversion to sRGB colourspace in preperation for XYZ
	if(redValue>0.04045) redValue = pow(((redValue + 0.055) / 1.055),2.4);
	else redValue = redValue / 12.92;
	
	if(greenValue>0.04045) greenValue = pow(((greenValue + 0.055) / 1.055),2.4);
	else greenValue = greenValue / 12.92;
	
	if(blueValue>0.04045) blueValue = pow(((blueValue + 0.055) / 1.055),2.4);
	else blueValue = blueValue / 12.92;
	
	//normalise
	double newRedValue = redValue * 100.0;
	double newGreenValue = greenValue * 100.0;
	double newBlueValue = blueValue * 100.0;
	
	//initilaise array for XYZ
	double XYZvaluesArray[3];
	
	//XYZ conversions to sRGB values stores in array
	XYZvaluesArray[0] = ((newRedValue * 0.4124) + (newGreenValue * 0.3576) + (newBlueValue * 0.1805));
	XYZvaluesArray[1] = ((newRedValue * 0.2126) + (newGreenValue * 0.7152) + (newBlueValue * 0.0722));
	XYZvaluesArray[2] = ((newRedValue * 0.0193) + (newGreenValue * 0.1192) + (newBlueValue * 0.9505));
	
	//declaring XYZ weighting references
	double ref_X = 95.047;
	double ref_Y = 100.0;
	double ref_Z = 108.883;
	
	//divide against weights
	double variableX = XYZvaluesArray[0] / ref_X;
	double variableY = XYZvaluesArray[1] / ref_Y;
	double variableZ = XYZvaluesArray[2] / ref_Z;
	
	//declare division for power function
	double power1 = 1.0 / 3.0;
	
	//XYZ to LAB conversion
	if(variableX>0.008856) variableX = pow((double)variableX, (float)power1);
	else variableX = (7.787 * variableX) + 16.0/116.0;
	
	if(variableY>0.008856) variableY = pow((double)variableY, (float)power1);
	else variableY = (7.787 * variableY) + 16.0/116.0;
	
	if(variableZ>0.008856) variableZ = pow((double)variableZ, (float)power1);
	else variableZ = (7.787 * variableZ) + 16.0/116.0;
	
	//applying LAB weighting
	LABarray[0] = (116.0 * variableY) - 16.0;
	LABarray[1] = 500.0 * (variableX - variableY);
	LABarray[2] = 200.0 * (variableY - variableZ);
}

//LAB TO RGB conversion, like the one before but backwards!
void LABtoRGB(double lValue, double aValue, double labBValue, int *RGBArray)
{
	//remove LAB weighting
	double yValue = (lValue + 16.0) / 116.0;
	double xValue = aValue / 500.0 + yValue;
	double zValue = yValue - labBValue / 200.0;
	
	//declare division for power function
	double division = 16.0/166.0;
	
	//LAB to XYZ conversion
	if((pow(yValue,3))>0.008856) yValue = pow((double)yValue,3.0);
	else yValue = ((double)yValue - division) / 7.787;
	
	if((pow(xValue,3))>0.008856) xValue = pow(xValue,3.0);
	else xValue = (xValue - division) / 7.787;
	
	if((pow(zValue,3))>0.008856) zValue = pow((double)zValue,3.0);
	else zValue = (zValue - division) / 7.787;
	
	//declare arrays
	double xyzArray[3];
	
	//remove array weighting
	xyzArray[0] = 95.047 * xValue;
	xyzArray[1] = 100.0 * yValue;
	xyzArray[2] = 108.883 * zValue;    
	
	//normalise
	double convertX = xyzArray[0] / 100.0;
	double convertY = xyzArray[1] / 100.0;
	double convertZ = xyzArray[2] / 100.0;
	
	//XYZ to sRGB
	double rValue = (convertX * 3.2406) + (convertY * (-1.5372)) + (convertZ * (-0.4986));
	double gValue = (convertX * (-0.9689)) + (convertY * 1.8758) + (convertZ * 0.0415);
	double bValue = (convertX * 0.0557) + (convertY * (-0.2040)) + (convertZ * 1.0570);
	
	//power declaration
	double power = 1.0/2.4;
	
	//sRGB to RGB
	if(rValue>0.0031308) rValue = 1.055 * (pow(rValue, power)) - 0.055;
	else rValue = 12.92 * rValue;
	
	if(gValue>0.0031308) gValue = 1.055 * (pow(gValue, power)) - 0.055;
	else gValue = 12.92 * gValue;
	
	if(bValue>0.0031308) bValue = 1.055 * (pow(bValue, power)) - 0.055;
	else bValue = 12.92 * bValue;
	
	//return RGB values to main transfer function to write to pixels
	RGBArray[0] = rValue * 255;
	RGBArray[1] = gValue * 255;
	RGBArray[2] = bValue * 255;
}

//end of file
