#include<stdlib.h>
#include<stdio.h>
#include <stdio.h>
 
// Video resolution
#define W 1920
#define H 1279
 
// Allocate a buffer to store one frame
unsigned char frame[H][W][3] = {0};
 
int main()
{
    int x, y, count;
     
	//ffmpeg -loop 1 -i after_10_mins_nnet_1L.jpg -c:v libx264 -t 10 -pix_fmt yuv420p -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" output3.mp4  
    // Open an input pipe from ffmpeg and an output pipe to a second instance of ffmpeg
    FILE *pipein = popen("ffmpeg -i after_10_mins_nnet_1L.jpg -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -", "r");
    
	FILE *pipeout = popen("ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s 1280x720 -r 60 -i - -f mp4 -q:v 5 -an -vcodec mpeg4 combined.mp4", "w");
	
	int img_h = 1279;
	int img_w = 1920;
    count = fread(frame, 1, img_h*img_w*3, pipein);
	
    // If we didn't get a frame of video, we're probably at the end
    if (count != img_h*img_w*3) {
		fprintf(stderr, "Problem!!!!!!!!!!\n"); 
		exit(1);
	}
	
    // Process video frames
	int i = 0;
    while(i < 510)
    {
        // Read a frame from the input pipe into the buffer
        //count = fread(frame, 1, H*W*3, pipein);
		
        // If we didn't get a frame of video, we're probably at the end
        //if (count != H*W*3) break;
		
        // Process this frame
		int video_h = 720;
		int video_w = 1280;
		unsigned char temp_frame[720][1280][3] = {0};
        for (y=0 ; y<video_h ; ++y){
			for (x=0 ; x<video_w ; ++x){
				// Invert each colour component in every pixel
				if(i<=255){
					temp_frame[y][x][0] = frame[y][x][0] + (char)(i - frame[y][x][0]*(i/255.0)); // red
					temp_frame[y][x][1] = frame[y][x][1] + (char)(i - frame[y][x][1]*(i/255.0)); // green
					temp_frame[y][x][2] = frame[y][x][2] + (char)(i - frame[y][x][2]*(i/255.0));  // blue
				}
				else{
					temp_frame[y][x][0] = frame[y][x][0] + (char)((510-i) - frame[y][x][0]*((510-i)/255.0)); // red
					temp_frame[y][x][1] = frame[y][x][1] + (char)((510-i) - frame[y][x][1]*((510-i)/255.0)); // green
					temp_frame[y][x][2] = frame[y][x][2] + (char)((510-i) - frame[y][x][2]*((510-i)/255.0)); // blue
				}
			}
		}
         
        // Write this frame to the output pipe
        fwrite(temp_frame, 1, video_h*video_w*3, pipeout);
		i++;
    }
     
    // Flush and close input and output pipes
    fflush(pipein);
    pclose(pipein);
    fflush(pipeout);
    pclose(pipeout);
	
	return 0;
}