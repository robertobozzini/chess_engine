
#include "main.h"
#include "lobby.h"
#include "chessboard.h"
using namespace std;

Texture2D background;
Texture2D logo;

float fmaxx, scaleX, scaleY;


Vector2 logop;
float logoscale=0.5f;

int fontsize = 32;
Font uiFont;

float spacing = 1.0f;

int stage = 0;


float factor = -0.15f;

struct Button{
	
	Rectangle rect;
	
	Color col;
	
	Color col_hov;
	
	const char* text;
	
	Color textcolor;
	
	Color textcolor_hov;
	
	Vector2 textsize;
	
	Vector2 textpos;
	
	bool hovering = 0;
	
	bool ison;
	void init(Status * status,  Rectangle _rect, Color _col,  const char* _text, Color _textcolor){
		
		rect = _rect;
		col = _col;
		col_hov = ColorBrightness(col, factor);
		
		text = _text;
		textcolor = _textcolor;
		textcolor_hov = ColorBrightness(textcolor, factor);
		
		textsize = MeasureTextEx(uiFont, text, fontsize, spacing);
		
		//spacing = 1.0f;
		
		textpos.x = rect.x + rect.width/2 - textsize.x/2;
		textpos.y = rect.y + rect.height/2 - textsize.y/2;
		
		ison = 0;
		
	}
	bool ispressed(Status * status){
		Vector2 mousepos = GetMousePosition();
		//cout<<hovering<<endl;
		if(mousepos.x >= rect.x && mousepos.x <= rect.x+rect.width && mousepos.y >= rect.y && mousepos.y <= rect.y+rect.height){
			//dentro
			
			hovering = 1;
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
				ison = !ison;
				return 1;
			}
		}
		else{
			hovering = 0;
		}
		
		return 0;
	}
	void draw(Status * status){
	
		Color color = (hovering)?col_hov: col;
		Color textcol = (hovering)?textcolor_hov: textcolor;
		
		int mulfactor = (ison)?-2:1;
		
		DrawRectangleRec(rect, ColorBrightness(color, mulfactor*factor));
		
		DrawTextEx(uiFont, text, textpos, fontsize, spacing, ColorBrightness(textcol, mulfactor*factor));
		//DrawTextEx(uiFont, text, )
	}
};

vector<vector<Button>>buttons;

void lobby_init(Status *status){
	background = LoadTexture("../images/background.png");
	logo = LoadTexture("../images/LOGO.png");
	//printf("%d %d\n", background.width, background.height);
	scaleX = (float)GetScreenWidth() / background.width;
	scaleY = (float)GetScreenHeight() / background.height;
	fmaxx=fmaxf(scaleX,scaleY);
	
	
	logop.x=GetScreenWidth()/2 - logo.width*logoscale/2;
	logop.y=0;
	
	
	uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-SemiBold.ttf", fontsize, NULL, 0);
	
	stage = 0;
	buttons.clear();
	buttons.resize(2);
	
	//bottone iniziale
	Button button;
	const char* label = "PLAY";
	float rectwidth = ScreenWidth/4.0f;
	float rectheight = ScreenHeight/10.0f;
	
	Rectangle rect = {ScreenWidth/2.0f - rectwidth/2.0f, ScreenHeight/2.0f, rectwidth, rectheight};
	
	button.init(status, rect, BEIGE, label, DARKBROWN );
	buttons[stage].push_back(button);
	
	
	rect = {ScreenWidth/4.0f - rectwidth/2.0f, ScreenHeight/4.0f , rectwidth, rectheight};
	button.init(status, rect, BEIGE, label, DARKBROWN );
	buttons[1].push_back(button);
	
	label = "playing with white";
	rect = {ScreenWidth/4.0f - rectwidth/2.0f, ScreenHeight/4.0f + 2*rectheight , rectwidth, rectheight};
	button.init(status, rect, BEIGE, label, DARKBROWN );
	buttons[1].push_back(button);
	
	label = "play against the bot";
	rect = {ScreenWidth/4.0f - rectwidth/2.0f, ScreenHeight/4.0f + 4*rectheight , rectwidth, rectheight};
	button.init(status, rect, BEIGE, label, DARKBROWN );
	buttons[1].push_back(button);
	
	label = "game audio";
	rect = {ScreenWidth/4.0f - rectwidth/2.0f, ScreenHeight/4.0f + 6*rectheight , rectwidth, rectheight};
	button.init(status, rect, BEIGE, label, DARKBROWN );
	buttons[1].push_back(button);
}


void buttons_handler(Status *status){
	

	
	for(int i = 0; i < buttons[stage].size(); i++){
		
		if(buttons[stage][i].ispressed(status)){
			
			if(i==0){
				buttons[stage][0].ison = 0;
				if(stage == 0)
				{
					//buttons[0][0].ison = 0;
					stage = 1;
					return;
				}
				
				chessboard_reset(status);
				event_handler=chessboard_handler;
				screen_drawer=chessboard_drawer;
				
				//chessboard_reset(status);
				
				status->ingame=1;
				status->isholding = 0;
			}
			else if(i==1){
				if(buttons[stage][i].ison){
					buttons[stage][i].text = "playing with black";
				}
				else{
					buttons[stage][i].text = "playing with white";
				}
				buttons[stage][i].textsize = MeasureTextEx(uiFont, buttons[stage][i].text, fontsize, spacing);
				
				//spacing = 1.0f;
				
				buttons[stage][i].textpos.x = buttons[stage][i].rect.x + buttons[stage][i].rect.width/2 - buttons[stage][i].textsize.x/2;
				buttons[stage][i].textpos.y = buttons[stage][i].rect.y + buttons[stage][i].rect.height/2 - buttons[stage][i].textsize.y/2;
				status->inverted = !status->inverted;
			}
			else if(i==2){
				status->bot = !status->bot;
			}
			else if(i==3){
				status->audio = !status->audio;
			}
		}
	}

}


void lobby_handler(Status *status){
	
	status->isholding = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	
	if(IsKeyPressed(KEY_ESCAPE)){
		if(stage == 0)
			CloseWindow();
		stage = 0;
	}
	
	buttons_handler(status);
	
}

void lobby_drawer(Status *status){
	
	ClearBackground(BLACK);
	
	DrawTextureEx(
		background,
		(Vector2){0, 0},
		0,
		fmaxx,
		LIGHTGRAY
	);
	DrawTextureEx(
		logo,
		logop,
		0,
		logoscale,
		WHITE
	);
	//DrawTexture(logo,logop.x, logop.y, BLACK);
	//DrawTexture(background, 0 , 0, WHITE);
	for(auto &button:buttons[stage]){
		button.draw(status);
	}
}



