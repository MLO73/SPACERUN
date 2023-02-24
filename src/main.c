#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "draw.c"
#include "game.c"
#include "math.c"

#define BACKGROUND "./resources/background.jpg"
#define BACKGROUND2 "./resources/background2.png"

#define LOGO "./resources/logo.png"
#define BACK "./resources/hole.png"
#define CONF "./resources/conf.txt"

#define MAX_LINES 999
#define MAX_WORD_LENGTH 999

bool quit = false;
bool gameOver = false;

Uint64 now = 0;
Uint64 last = 0;
double deltaTime = 0;

int cubesLength = 0;
Cube cubes[1000];

////////

int BUTTON_WIDTH = 350;
int BUTTON_HEIGHT = 50;

int WINDOW_WIDTH = 1200;
int WINDOW_HEIGHT = 600;

char players[9999][9999];

int rrr = 0;
int bbb= 0;

int seen = 0;

char score[10];

SDL_Color white = { 255, 255, 255, 255 };
SDL_Color black = { 0, 0, 0, 255 };

int menu();

void drawSpeedText(SDL_Renderer *renderer) {
  if (Sans == NULL) {
    Sans = TTF_OpenFont("./resources/Mono.ttf", 42);
  }
  sprintf(score, "%d", (int)playerSpeed);
  SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, score, TEXT_COLOR);
  SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  SDL_Rect Message_rect;
  Message_rect.x = 0;
  Message_rect.y = -10;
  Message_rect.w = 24 * 3;
  Message_rect.h = 50;
  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
}

void drawGameOverText(SDL_Renderer *renderer) {
  SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, "GAME OVER", TEXT_COLOR);
  SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  SDL_Rect Message_rect;
  Message_rect.w = WINDOW_WIDTH / 2;
  Message_rect.h = WINDOW_HEIGHT / 2;
  Message_rect.x = /*WIDTH / 2 -*/ Message_rect.w / 2;
  Message_rect.y = /*HEIGHT / 2 -*/ Message_rect.h / 2 - 10;
  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
}

void drawPressEscText(SDL_Renderer *renderer) {
  SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, "Press Esc", TEXT_COLOR);
  SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  SDL_Rect Message_rect;
  Message_rect.w = WINDOW_WIDTH / 3;
  Message_rect.h = WINDOW_HEIGHT / 4;
  Message_rect.x = /*WIDTH / 2 -*/ Message_rect.w ;
  Message_rect.y = /*HEIGHT / 2 -*/ Message_rect.h + 300;
  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
}

void init(SDL_Renderer* renderer) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  srand(time(NULL));
  TTF_Init();
  gameInit(cubes, &cubesLength);
  SDL_Texture* backgroundTexture = NULL;
  SDL_Surface* backgroundSurface = IMG_Load("./resources/background2.png");
  if(backgroundSurface == NULL) {
      printf("Unable to load background image! SDL Error: %s\n", SDL_GetError());
  } else {
      backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
      SDL_FreeSurface(backgroundSurface);
  }

  while (!quit) {
    last = now;
    now = SDL_GetTicks();

    SDL_Event e;
    SDL_PollEvent(&e);
    if (!gameOver) {
    	gameOver = gameFrame(deltaTime, cubes, &cubesLength);
    }
    if (e.type == SDL_QUIT) {
        quit = true;
    }

    draw(renderer, backgroundTexture);

    drawCubes(renderer, cubes, cubesLength);

    drawSpeedText(renderer);
    if (gameOver) {
        drawGameOverText(renderer);
        drawPressEscText(renderer);

        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        if (keyState[SDL_SCANCODE_ESCAPE]) {
            break;
        }
    }

    SDL_RenderPresent(renderer);

    deltaTime = (double)((now - last)) / 12000;


	    while (SDL_PollEvent(&e)) {
		switch (e.type) {
		    case SDL_QUIT:
		        quit = true;
		        break;
		    case SDL_KEYDOWN:
		        if (e.key.keysym.sym == SDLK_ESCAPE) {
		            quit = true;
		        }
		        break;
		}
	    }
    }

       
	
}

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Texture* texture;
    SDL_Rect text_rect;
} Button;

void create_button(Button* button, SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color, SDL_Color color2) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color2);
    button->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_QueryTexture(button->texture, NULL, NULL, &button->text_rect.w, &button->text_rect.h);
    button->text_rect.x = x + (BUTTON_WIDTH - button->text_rect.w) / 2;
    button->text_rect.y = y + (BUTTON_HEIGHT - button->text_rect.h) / 2;

    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = BUTTON_WIDTH;
    button->rect.h = BUTTON_HEIGHT;
    button->color = color;
}

void background(SDL_Renderer* renderer){
	SDL_Surface* imageSurface = IMG_Load(BACKGROUND);
	SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
	SDL_FreeSurface(imageSurface);

	SDL_Rect destRect = {0, 0, imageSurface->w, imageSurface->h};
	SDL_RenderCopy(renderer, imageTexture, NULL, &destRect);

	SDL_DestroyTexture(imageTexture);
}

void logo(SDL_Renderer* renderer, int h){
	// Load the image
	SDL_Surface* imageSurface = IMG_Load(LOGO);

	// Get the dimensions of the image
	int imageWidth = imageSurface->w;
	int imageHeight = imageSurface->h;

	// Calculate the position of the image
	int imageX = (WINDOW_WIDTH - imageWidth) / 2;

	// Create a texture from the image
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);

	// Render the texture in the center of the window
	SDL_Rect destination = { imageX, h, imageWidth, imageHeight };
	SDL_RenderCopy(renderer, texture, NULL, &destination);
	
	// Free the surface and texture
	SDL_FreeSurface(imageSurface);
	SDL_DestroyTexture(texture);
}

void back(SDL_Renderer* renderer, int h){
	// Load the image
	SDL_Surface* imageSurface = IMG_Load(BACK);

	// Get the dimensions of the image
	int imageWidth = imageSurface->w;
	int imageHeight = imageSurface->h;

	// Calculate the position of the image
	int imageX = (WINDOW_WIDTH - imageWidth) / 2;

	// Create a texture from the image
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);

	// Render the texture in the center of the window
	SDL_Rect destination = { imageX, h, imageWidth, imageHeight };
	SDL_RenderCopy(renderer, texture, NULL, &destination);
	
	// Free the surface and texture
	SDL_FreeSurface(imageSurface);
	SDL_DestroyTexture(texture);
}

int contains_space(char *str) {
    while (*str != '\0') {
        if (isspace(*str)) {
            return 1; // строка содержит пробелы
        }
        str++;
    }
    return 0; // строка не содержит пробелы
}

void start_game(SDL_Renderer* renderer) {
    SDL_Texture* texture = NULL;
    AVFormatContext* formatCtx = NULL;
    AVCodecContext* codecCtx = NULL;
    const AVCodec* codec = NULL;
    AVFrame* frame = NULL;
    SDL_Event event;
    int quit = 0;
    int ret = 0;

    // Initialize SDL2
    ret = SDL_Init(SDL_INIT_VIDEO);

    // Create window and renderer

    // Load MP4 file
    ret = avformat_open_input(&formatCtx, "./resources/video.mp4", NULL, NULL);

    // Find video stream
    ret = avformat_find_stream_info(formatCtx, NULL);

    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    // Find video codec
    codecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codecCtx, formatCtx->streams[videoStreamIndex]->codecpar);
    codec = avcodec_find_decoder(codecCtx->codec_id);

    // Open codec
    ret = avcodec_open2(codecCtx, codec, NULL);

    // Allocate video frame
    frame = av_frame_alloc();

    // Create texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, codecCtx->width, codecCtx->height);

    // Calculate the time to wait between frames
    double frame_time = av_q2d(formatCtx->streams[videoStreamIndex]->avg_frame_rate);
    int delay_time = (int) round(1000.0 / frame_time);

    // Launch MP3 player in the background
    //system("mpg123 -q audio.mp3 &");

    while (!quit) {
        SDL_PollEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
        }

        // Read frame
        AVPacket packet;
        ret = av_read_frame(formatCtx, &packet);
        if (ret < 0) {
            break;
        }

        if (packet.stream_index == videoStreamIndex) {
            // Decode frame
            ret = avcodec_send_packet(codecCtx, &packet);

            while (ret >= 0) {
                ret = avcodec_receive_frame(codecCtx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }

                // Update texture
                SDL_UpdateYUVTexture(texture, NULL, frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);

                // Delay to synchronize with display refresh rate
                SDL_Delay(delay_time);
            }
            av_packet_unref(&packet);
        }

        const Uint8 *keyState = SDL_GetKeyboardState(NULL);

        if (keyState[SDL_SCANCODE_SPACE]) {
            break;
        }
    }

    // Clean up resources
    av_frame_free(&frame);
    avcodec_close(codecCtx);
    avformat_close_input(&formatCtx);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}


void start_menu(SDL_Renderer* renderer, TTF_Font* font, SDL_Window* window) {
     // Load the button image
     
    SDL_Surface* button_surface = IMG_Load("./resources/back.png");
    SDL_Texture* button_texture = SDL_CreateTextureFromSurface(renderer, button_surface);
    SDL_FreeSurface(button_surface);

    // Create the button rectangle
    SDL_Rect button_rect = { 10, 10, button_surface->w, button_surface->h };

	int padding_x = 10; // задаем отступ по оси X
	int padding_y = 10; // задаем отступ по оси Y
	int bg_width = button_surface->w - 2 * padding_x - 9; // ширина фона кнопки
	int bg_height = button_surface->h - 2 * padding_y - 9; // высота фона кнопки
	int bg_x = button_rect.x + (button_rect.w - bg_width) / 2; // координата X фона кнопки
	int bg_y = button_rect.y + (button_rect.h - bg_height) / 2; // координата Y фона кнопки

	SDL_Rect button_bg_rect = { bg_x, bg_y, bg_width, bg_height };

    
    
    // Create a text input field
    SDL_Rect input_rect = { (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 240+30+rrr, BUTTON_WIDTH, BUTTON_HEIGHT };
    SDL_Surface* input_surface = SDL_CreateRGBSurfaceWithFormat(0, BUTTON_WIDTH, BUTTON_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_Texture* input_texture = SDL_CreateTextureFromSurface(renderer, input_surface);
    SDL_FreeSurface(input_surface);
    char input_text[11] = "Player";
    SDL_StartTextInput();

    // Create a text display area above the "START GAME" button
    SDL_Surface* inscription_surface = TTF_RenderText_Blended(font, "ENTER PLAYER NAME WITHOUT SPACES:", white);
    SDL_Texture* inscription_texture = SDL_CreateTextureFromSurface(renderer, inscription_surface);
    SDL_FreeSurface(inscription_surface);
    SDL_Rect inscription_rect = { (WINDOW_WIDTH - inscription_surface->w) / 2, 200+10+rrr, inscription_surface->w, inscription_surface->h };


    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 0, 255 };

    // Create a "START GAME" button
    Button start_game_button;
    create_button(&start_game_button, renderer, font, "START GAME", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 300+30+10+rrr, white, black);

    // Create a "BACK" button
    Button back_button;
    create_button(&back_button, renderer, font, "BACK", 10, 10, white, black);
   

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;

                        if (SDL_PointInRect(& (SDL_Point) { x, y }, &start_game_button.rect)) {
			    if (strlen(input_text) > 0) {

                    

                    system("mpg123 -q ./resources/audio.mp3 &");
                    if (seen != 1) {
                    seen = 1;
                    start_game(renderer);
                if (SDL_Init(SDL_INIT_VIDEO) != 0) {
                    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
                    //return 1;
                }

                if (TTF_Init() != 0) {
                    SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
                    SDL_Quit();
                    //return 1;
                }

                SDL_Window* window = SDL_CreateWindow("SPACERUN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
                if (!window) {
                    SDL_Log("Failed to create window: %s", SDL_GetError());
                    //return 1;
                }

                SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                if (!renderer) {
                    SDL_Log("Failed to create renderer: %s", SDL_GetError());
                    // 1;
                }
				init(renderer);

                FILE *file = fopen("./resources/conf.txt", "a");
				    fprintf(file, "%s %s\n", input_text, score);
				    fclose(file);


				gameOver = false;
				now = 0;
				last = 0;
				deltaTime = 0;
				cubesLength = 0;
                //const Uint8 *keyState = SDL_GetKeyboardState(NULL);
                //if (keyState[SDL_SCANCODE_S]) {
                //    init(renderer);
                //}
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
                system("pkill mpg123");
				menu();
			    } else {
                    init(renderer);

                FILE *file = fopen("./resources/conf.txt", "a");
				    fprintf(file, "%s %s\n", input_text, score);
				    fclose(file);


				gameOver = false;
				now = 0;
				last = 0;
				deltaTime = 0;
				cubesLength = 0;
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
                system("pkill mpg123");
				menu();
                }
                    }
                
			} else if (SDL_PointInRect(& (SDL_Point) { x, y }, &back_button.rect)) {
			    running = 0;
			}
                    }
                    break;
                case SDL_TEXTINPUT:
		    if (strlen(input_text) < 13) { // limit to 31 characters
			if (event.text.text[0] != ' ') {
				    strcat(input_text, event.text.text);
			}
		    }
		    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(input_text) > 0) {
                        input_text[strlen(input_text)-1] = '\0';
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                    }
                    break;
            }
        }

        // Render the background
        background(renderer);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &button_bg_rect);

	// Render the button image on top of the white background
	SDL_RenderCopy(renderer, button_texture, NULL, &button_rect);
        
        // Draw the button
    	SDL_RenderCopy(renderer, button_texture, NULL, &button_rect);

        // Render the input field
        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
        SDL_RenderFillRect(renderer, &input_rect);

        // Render the text display area
        SDL_RenderCopy(renderer, inscription_texture, NULL, &inscription_rect);

        // Render the "START GAME" button and the "BACK" button
        SDL_SetRenderDrawColor(renderer, start_game_button.color.r, start_game_button.color.g, start_game_button.color.b, start_game_button.color.a);
        SDL_RenderFillRect(renderer, &start_game_button.rect);
        SDL_RenderCopy(renderer, start_game_button.texture, NULL, &start_game_button.text_rect);
        
        SDL_Rect input_text_rect = { input_rect.x + 10, input_rect.y + 10, input_rect.w - 20, input_rect.h - 20 };


	// Render the text in the input field using the fixed font size
	SDL_Surface* input_text_surface = TTF_RenderText_Blended(font, input_text, black);
	SDL_Texture* input_text_texture = SDL_CreateTextureFromSurface(renderer, input_text_surface);
	SDL_FreeSurface(input_text_surface);

	// Center the input text in the input field
	int text_width, text_height;
	SDL_QueryTexture(input_text_texture, NULL, NULL, &text_width, &text_height);
	SDL_Rect text_rect = { input_text_rect.x + (input_text_rect.w - text_width) / 2, input_text_rect.y + (input_text_rect.h - text_height) / 2, text_width, text_height };

	SDL_RenderCopy(renderer, input_text_texture, NULL, &text_rect);
	SDL_DestroyTexture(input_text_texture);

	// Update the screen
	SDL_RenderPresent(renderer);
	}

	// Clean up
	SDL_DestroyTexture(input_texture);
	SDL_DestroyTexture(start_game_button.texture);
	SDL_DestroyTexture(back_button.texture);
}

void options_menu(SDL_Renderer* renderer, TTF_Font* font, SDL_Window* window) {
    
    SDL_Surface* button_surface = IMG_Load("./resources/back.png");
    SDL_Texture* button_texture = SDL_CreateTextureFromSurface(renderer, button_surface);
    SDL_FreeSurface(button_surface);

    // Create the button rectangle
    SDL_Rect button_rect = { 10, 10, button_surface->w, button_surface->h };

	int padding_x = 10; // задаем отступ по оси X
	int padding_y = 10; // задаем отступ по оси Y
	int bg_width = button_surface->w - 2 * padding_x - 9; // ширина фона кнопки
	int bg_height = button_surface->h - 2 * padding_y - 9; // высота фона кнопки
	int bg_x = button_rect.x + (button_rect.w - bg_width) / 2; // координата X фона кнопки
	int bg_y = button_rect.y + (button_rect.h - bg_height) / 2; // координата Y фона кнопки

	SDL_Rect button_bg_rect = { bg_x, bg_y, bg_width, bg_height };
	
	    // Create a text display area above the "START GAME" button
    SDL_Surface* inscription_surface = TTF_RenderText_Blended(font, "CHOOSE SCREEN EXTENSION:", white);
    SDL_Texture* inscription_texture = SDL_CreateTextureFromSurface(renderer, inscription_surface);
    SDL_FreeSurface(inscription_surface);
    SDL_Rect inscription_rect = { (WINDOW_WIDTH - inscription_surface->w) / 2, 200+5+rrr, inscription_surface->w, inscription_surface->h };
    
    Button back_button;
    create_button(&back_button, renderer, font, "BACK", 10, 10, white, black);
    

    Button expand_button0;
    create_button(&expand_button0, renderer, font, "800 X 600", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 240+30+rrr, white, black);
    Button expand_button1;
    create_button(&expand_button1, renderer, font, "1024 X 728", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 300+30+rrr, white, black);
    Button expand_button3;
    create_button(&expand_button3, renderer, font, "1600 X 900", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 360+30+rrr, white, black);


    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;

                        if (SDL_PointInRect(& (SDL_Point) { x, y }, &expand_button0.rect)) {
                        	SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				TTF_Quit();
				SDL_Quit();
				WINDOW_WIDTH = 800;
				WINDOW_HEIGHT = 600;
				
				FILE *file = fopen("./resources/conf.txt", "r+");
				fprintf(file, "%d %d", WINDOW_WIDTH, WINDOW_HEIGHT);
				fclose(file);
				
				menu();
                        } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &expand_button1.rect)) {
                        	SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				TTF_Quit();
				SDL_Quit();
				WINDOW_WIDTH = 1024;
				WINDOW_HEIGHT = 728;
				
				FILE *file = fopen("./resources/conf.txt", "r+");
				fprintf(file, "%d %d", WINDOW_WIDTH, WINDOW_HEIGHT);
				fclose(file);
				
				menu();
                        } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &expand_button3.rect)) {
                        	SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				TTF_Quit();
				SDL_Quit();
				WINDOW_WIDTH = 1600;
				WINDOW_HEIGHT = 900;
				
				FILE *file = fopen("./resources/conf.txt", "r+");
				fprintf(file, "%d %d", WINDOW_WIDTH, WINDOW_HEIGHT);
				fclose(file);
				
				menu();
                        } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &back_button.rect)) {
                            running = 0;
                        }
                        
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        background(renderer);
        
         SDL_RenderCopy(renderer, inscription_texture, NULL, &inscription_rect);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &button_bg_rect);

	// Render the button image on top of the white background
	SDL_RenderCopy(renderer, button_texture, NULL, &button_rect);
        
        // Draw the button
    	SDL_RenderCopy(renderer, button_texture, NULL, &button_rect);
    	
        SDL_SetRenderDrawColor(renderer, expand_button0.color.r, expand_button0.color.g, expand_button0.color.b, expand_button0.color.a);
        SDL_RenderFillRect(renderer, &expand_button0.rect);
        SDL_RenderCopy(renderer, expand_button0.texture, NULL, &expand_button0.text_rect);

        SDL_SetRenderDrawColor(renderer, expand_button1.color.r, expand_button1.color.g, expand_button1.color.b, expand_button1.color.a);
        SDL_RenderFillRect(renderer, &expand_button1.rect);
        SDL_RenderCopy(renderer, expand_button1.texture, NULL, &expand_button1.text_rect);
       
        
        SDL_SetRenderDrawColor(renderer, expand_button3.color.r, expand_button3.color.g, expand_button3.color.b, expand_button3.color.a);
        SDL_RenderFillRect(renderer, &expand_button3.rect);
        SDL_RenderCopy(renderer, expand_button3.texture, NULL, &expand_button3.text_rect);
        

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(back_button.texture);
    SDL_DestroyTexture(expand_button0.texture);
    SDL_DestroyTexture(expand_button1.texture);
    SDL_DestroyTexture(expand_button3.texture);
}



void score_menu(SDL_Renderer* renderer, TTF_Font* font) {

    char filename[] = "./resources/conf.txt";
    char words[MAX_LINES][2][MAX_WORD_LENGTH];

    FILE* fp = fopen(filename, "r");


    // Прочитать и пропустить первую строку
    char temp[MAX_WORD_LENGTH];
    fgets(temp, MAX_WORD_LENGTH, fp);

    int line_count = 0;
    while (fgets(words[line_count][0], MAX_WORD_LENGTH, fp) != NULL && line_count < MAX_LINES) {
        // Убедитесь, что каждая строка содержит два слова
        char* newline_char = strchr(words[line_count][0], '\n');
        if (newline_char != NULL) {
            *newline_char = '\0';
        }
        char* token = strtok(words[line_count][0], " ");
        if (token == NULL) {
            continue;
        }
        strcpy(words[line_count][1], token);
        token = strtok(NULL, " ");
        if (token == NULL) {
            continue;
        }
        strcpy(words[line_count][0], token);

        line_count++;
    }

    fclose(fp);

    // Сортировка массива по первому слову в порядке убывания
    for (int i = 0; i < line_count - 1; i++) {
        for (int j = i + 1; j < line_count; j++) {
            int num1 = atoi(words[i][0]);
            int num2 = atoi(words[j][0]);
            if (num1 < num2) {
                // Поменять местами i-ю и j-ю строки
                char temp[MAX_WORD_LENGTH];
                strcpy(temp, words[i][0]);
                strcpy(words[i][0], words[j][0]);
                strcpy(words[j][0], temp);

                strcpy(temp, words[i][1]);
                strcpy(words[i][1], words[j][1]);
                strcpy(words[j][1], temp);
            }
        }
    }

    // Удаление повторяющихся строк с одинаковыми числами
    int new_line_count = 1;
    for (int i = 1; i < line_count; i++) {
        if (strcmp(words[i][0], words[i-1][0]) != 0) {
            strcpy(words[new_line_count][0], words[i][0]);
            strcpy(words[new_line_count][1], words[i][1]);
            new_line_count++;
        }
    }
    
    char play[MAX_LINES][2][MAX_WORD_LENGTH];
    
    for (int i = 0; i < 5; i++) {
        strcpy(play[i][0], "-");
        strcpy(play[i][1], "-");
    }
    
	int play_index = 0;

	// Копирование отсортированного массива с удаленными повторяющимися строками в массив "play"
	for (int i = 0; i < new_line_count; i++) {
	    // Копирование первого слова в массив "play"
	    strcpy(play[play_index][0], words[i][0]);
	    
	    // Копирование второго слова в массив "play"
	    strcpy(play[play_index][1], words[i][1]);
	    
	    play_index++;
	}

    

    SDL_Surface* button_surface = IMG_Load("./resources/back.png");
    SDL_Texture* button_texture = SDL_CreateTextureFromSurface(renderer, button_surface);
    SDL_FreeSurface(button_surface);

    // Create the button rectangle
    SDL_Rect button_rect = { 10, 10, button_surface->w, button_surface->h };

	int padding_x = 10; // задаем отступ по оси X
	int padding_y = 10; // задаем отступ по оси Y
	int bg_width = button_surface->w - 2 * padding_x - 9; // ширина фона кнопки
	int bg_height = button_surface->h - 2 * padding_y - 9; // высота фона кнопки
	int bg_x = button_rect.x + (button_rect.w - bg_width) / 2; // координата X фона кнопки
	int bg_y = button_rect.y + (button_rect.h - bg_height) / 2; // координата Y фона кнопки

	SDL_Rect button_bg_rect = { bg_x, bg_y, bg_width, bg_height };
	
	    // Create a text display area above the "START GAME" button
    SDL_Surface* inscription_surface = TTF_RenderText_Blended(font, "TOP PLAYERS:", white);
    SDL_Texture* inscription_texture = SDL_CreateTextureFromSurface(renderer, inscription_surface);
    SDL_FreeSurface(inscription_surface);
    SDL_Rect inscription_rect = { (WINDOW_WIDTH - inscription_surface->w) / 2, 60+10+40+20+rrr, inscription_surface->w, inscription_surface->h };
    
    
    SDL_Surface* inscription_surface1 = TTF_RenderText_Blended(font, play[0][1], white);
    SDL_Texture* inscription_texture1 = SDL_CreateTextureFromSurface(renderer, inscription_surface1);
    SDL_FreeSurface(inscription_surface1);
    SDL_Rect inscription_rect1 = { (WINDOW_WIDTH) / 2 - 200 - 30, 120+10+50+20+rrr, inscription_surface1->w, inscription_surface1->h };
    
    SDL_Surface* inscription_surface2 = TTF_RenderText_Blended(font, play[1][1], white);
    SDL_Texture* inscription_texture2 = SDL_CreateTextureFromSurface(renderer, inscription_surface2);
    SDL_FreeSurface(inscription_surface2);
    SDL_Rect inscription_rect2 = { (WINDOW_WIDTH) / 2 - 200 - 30, 180+10+50+20+rrr, inscription_surface2->w, inscription_surface2->h };
    
    SDL_Surface* inscription_surface3 = TTF_RenderText_Blended(font, play[2][1], white);
    SDL_Texture* inscription_texture3 = SDL_CreateTextureFromSurface(renderer, inscription_surface3);
    SDL_FreeSurface(inscription_surface3);
    SDL_Rect inscription_rect3 = { (WINDOW_WIDTH) / 2 - 200 - 30, 240+10+50+20+rrr, inscription_surface3->w, inscription_surface3->h };
    
    SDL_Surface* inscription_surface4 = TTF_RenderText_Blended(font, play[3][1], white);
    SDL_Texture* inscription_texture4 = SDL_CreateTextureFromSurface(renderer, inscription_surface4);
    SDL_FreeSurface(inscription_surface4);
    SDL_Rect inscription_rect4 = { (WINDOW_WIDTH) / 2 - 200 - 30, 300+10+50+20+rrr, inscription_surface4->w, inscription_surface4->h };
    
    SDL_Surface* inscription_surface5 = TTF_RenderText_Blended(font, play[4][1], white);
    SDL_Texture* inscription_texture5 = SDL_CreateTextureFromSurface(renderer, inscription_surface5);
    SDL_FreeSurface(inscription_surface5);
    SDL_Rect inscription_rect5 = { (WINDOW_WIDTH) / 2 - 200 - 30, 360+10+50+20+rrr, inscription_surface5->w, inscription_surface5->h };
    
    
    ////////////////////
    
    SDL_Surface* inscription_surface11 = TTF_RenderText_Blended(font, play[0][0], white);
    SDL_Texture* inscription_texture11 = SDL_CreateTextureFromSurface(renderer, inscription_surface11);
    SDL_FreeSurface(inscription_surface11);
    SDL_Rect inscription_rect11 = { (WINDOW_WIDTH) / 2 + 100 - 30, 120+10+50+20+rrr, inscription_surface11->w, inscription_surface11->h };
    
    SDL_Surface* inscription_surface22 = TTF_RenderText_Blended(font, play[1][0], white);
    SDL_Texture* inscription_texture22 = SDL_CreateTextureFromSurface(renderer, inscription_surface22);
    SDL_FreeSurface(inscription_surface22);
    SDL_Rect inscription_rect22 = { (WINDOW_WIDTH) / 2 + 100 - 30, 180+10+50+20+rrr, inscription_surface22->w, inscription_surface22->h };
    
    SDL_Surface* inscription_surface33 = TTF_RenderText_Blended(font, play[2][0], white);
    SDL_Texture* inscription_texture33 = SDL_CreateTextureFromSurface(renderer, inscription_surface33);
    SDL_FreeSurface(inscription_surface33);
    SDL_Rect inscription_rect33 = { (WINDOW_WIDTH) / 2 + 100 - 30, 240+10+50+20+rrr, inscription_surface33->w, inscription_surface33->h };
    
    SDL_Surface* inscription_surface44 = TTF_RenderText_Blended(font, play[3][0], white);
    SDL_Texture* inscription_texture44 = SDL_CreateTextureFromSurface(renderer, inscription_surface44);
    SDL_FreeSurface(inscription_surface44);
    SDL_Rect inscription_rect44 = { (WINDOW_WIDTH) / 2 + 100 - 30, 300+10+50+20+rrr, inscription_surface44->w, inscription_surface44->h };
    
    SDL_Surface* inscription_surface55 = TTF_RenderText_Blended(font, play[4][0], white);
    SDL_Texture* inscription_texture55 = SDL_CreateTextureFromSurface(renderer, inscription_surface55);
    SDL_FreeSurface(inscription_surface55);
    SDL_Rect inscription_rect55 = { (WINDOW_WIDTH) / 2 + 100 - 30, 360+10+50+20+rrr, inscription_surface55->w, inscription_surface55->h };
    
    
    Button back_button;
    create_button(&back_button, renderer, font, "BACK", 10, 10, white, black);
    


    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;
                    if (SDL_PointInRect(& (SDL_Point) { x, y }, &back_button.rect)) {
                    	running = 0;
                    }
                }
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        background(renderer);
        
        //back(renderer, -270+b);
        
        SDL_RenderCopy(renderer, inscription_texture, NULL, &inscription_rect);
        
        SDL_RenderCopy(renderer, inscription_texture1, NULL, &inscription_rect1);
        SDL_RenderCopy(renderer, inscription_texture2, NULL, &inscription_rect2);
        SDL_RenderCopy(renderer, inscription_texture3, NULL, &inscription_rect3);
        SDL_RenderCopy(renderer, inscription_texture4, NULL, &inscription_rect4);
        SDL_RenderCopy(renderer, inscription_texture5, NULL, &inscription_rect5);
        
        ///////////
        
        SDL_RenderCopy(renderer, inscription_texture11, NULL, &inscription_rect11);
        SDL_RenderCopy(renderer, inscription_texture22, NULL, &inscription_rect22);
        SDL_RenderCopy(renderer, inscription_texture33, NULL, &inscription_rect33);
        SDL_RenderCopy(renderer, inscription_texture44, NULL, &inscription_rect44);
        SDL_RenderCopy(renderer, inscription_texture55, NULL, &inscription_rect55);

        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &button_bg_rect);

	// Render the button image on top of the white background
	SDL_RenderCopy(renderer, button_texture, NULL, &button_rect);
    
        SDL_RenderPresent(renderer);
        
    }

    SDL_DestroyTexture(back_button.texture);

}



int menu(){
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SPACERUN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return 1;
    }
    
    SDL_Surface* icon = IMG_Load("./resources/icon.png");
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    TTF_Font* font = TTF_OpenFont("./resources/DejaVuSans.ttf", 28);
    
    if(WINDOW_HEIGHT > 750) { rrr = 110; font = TTF_OpenFont("./resources/DejaVuSans.ttf", 28+8-2); BUTTON_WIDTH = 350+30;
    	    WIDTH = 1600;
    	    HEIGHT = 1000;
    }
    else if (WINDOW_HEIGHT == 600) {rrr = 0;  font = TTF_OpenFont("./resources/DejaVuSans.ttf", 28+6-2); BUTTON_WIDTH = 350+0;
            WIDTH = WINDOW_WIDTH;
    	    HEIGHT = WINDOW_WIDTH;
    }
    else if (WINDOW_HEIGHT == 728) {rrr = 50; bbb = 50; font = TTF_OpenFont("./resources/DejaVuSans.ttf", 28+5-2); BUTTON_WIDTH = 350+20;
    	    WIDTH = 1000;
    	    HEIGHT = 1000;
    }
    
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return 1;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 0, 255 };
    
    if(quit == true){
    	quit = false;
    	start_menu(renderer, font, window);
    }

    Button start_button, options_button, score_button, exit_button;
    create_button(&start_button, renderer, font, "START", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 240+30+rrr, white, black);
    create_button(&options_button, renderer, font, "OPTIONS", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 300+30+rrr, white, black);
    create_button(&score_button, renderer, font, "SCORE", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 360+30+rrr, white, black);
    create_button(&exit_button, renderer, font, "EXIT", (WINDOW_WIDTH - BUTTON_WIDTH) / 2, 420+30+rrr, white, black);
    
    SDL_Event event;
    int running = 1;

    while (running) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (SDL_PointInRect(& (SDL_Point) { x, y }, &start_button.rect)) {
                        start_menu(renderer, font, window);
                    } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &options_button.rect)) {
                        options_menu(renderer, font, window);
                    } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &score_button.rect)) {
                        score_menu(renderer, font);
                    } else if (SDL_PointInRect(& (SDL_Point) { x, y }, &exit_button.rect)) {
                        running = 0;
                        exit(1);
                    }
                }
                break;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    background(renderer);
    logo(renderer, -190+rrr);

    SDL_SetRenderDrawColor(renderer, start_button.color.r, start_button.color.g, start_button.color.b, start_button.color.a);
    SDL_RenderFillRect(renderer, &start_button.rect);
    SDL_RenderCopy(renderer, start_button.texture, NULL, &start_button.text_rect);

    SDL_SetRenderDrawColor(renderer, options_button.color.r, options_button.color.g, options_button.color.b, options_button.color.a);
    SDL_RenderFillRect(renderer, &options_button.rect);
    SDL_RenderCopy(renderer, options_button.texture, NULL, &options_button.text_rect);

    SDL_SetRenderDrawColor(renderer, score_button.color.r, score_button.color.g, score_button.color.b, score_button.color.a);
    SDL_RenderFillRect(renderer, &score_button.rect);
    SDL_RenderCopy(renderer, score_button.texture, NULL, &score_button.text_rect);

    SDL_SetRenderDrawColor(renderer, exit_button.color.r, exit_button.color.g, exit_button.color.b, exit_button.color.a);
    SDL_RenderFillRect(renderer, &exit_button.rect);
    SDL_RenderCopy(renderer, exit_button.texture, NULL, &exit_button.text_rect);

    SDL_RenderPresent(renderer);
    }

	SDL_DestroyTexture(start_button.texture);
	SDL_DestroyTexture(options_button.texture);
	SDL_DestroyTexture(score_button.texture);
	SDL_DestroyTexture(exit_button.texture);

	TTF_CloseFont(font);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();
	return 0;
}

int main(){
    FILE *file = fopen("./resources/conf.txt", "r");
    if (file != NULL) {
    	char line[256];
        if (fgets(line, sizeof(line), file) != NULL) {
            char *width_str = strtok(line, " ");
            char *height_str = strtok(NULL, " ");
            if (width_str != NULL && height_str != NULL) {
                int width = atoi(width_str);
                int height = atoi(height_str);
                if (width > 0 && height > 0) {
              	    if (height > 2000) height = 600;
                    WINDOW_WIDTH = width;
                    WINDOW_HEIGHT = height;
                }
            }
        }
        
        
        fclose(file);

        
    }
    menu();
}
