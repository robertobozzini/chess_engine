

#include "chessboard.h"
#include "features.h"
//suoni, frecce e boh altro
float scalethickness = 0.15f;


vector<pair<Vector2, Vector2>>arrows;

bool holdingarrow = 0;
Vector2 last;


void reset_arrows(Status *status){
	arrows.clear();
	holdingarrow = 0;
}

void handle_arrows(Status *status, Vector2 mousepos){
	if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
		if(isinboard(mousepos)){
			Vector2 pos = chessboardpos(mousepos);
			holdingarrow = 1;
			last = pos;
		}
		else{
			reset_arrows(status);
		}

	}
	else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
		if(isinboard(mousepos) && holdingarrow){
			
			Vector2 pos = chessboardpos(mousepos);
			if(pos != last)
				arrows.push_back({last, pos});
			
		}
		holdingarrow = 0;
	}
}

void drawArrow(Vector2 start, Vector2 end){
	
	start = { corner.x + squaresize*start.x + squaresize/2, corner.y + squaresize*start.y + squaresize/2 };
	
	end = { corner.x + squaresize*end.x + squaresize/2, corner.y + squaresize*end.y + squaresize/2 };
	
	Color color = Fade(ORANGE, 0.5f);
	
	Vector2 dir = Vector2Subtract(end, start);
	float len = Vector2Length(dir);
	dir = Vector2Scale(dir, 1.0f/len);
	
	Vector2 perp = Vector2{-dir.y, dir.x};
	
	float HeadLen = scalethickness * squaresize * 3.5f;
	float HeadWid = scalethickness * squaresize * 3.0f;
	
	Vector2 shaftEnd = Vector2Subtract(end, Vector2Scale(dir, HeadLen));
	
	DrawLineEx(start, shaftEnd, scalethickness * squaresize, color);
	
	Vector2 left  = Vector2Add(shaftEnd, Vector2Scale(perp, HeadWid/2));
	Vector2 right = Vector2Subtract(shaftEnd, Vector2Scale(perp, HeadWid/2));
	
	DrawTriangle(end, right, left, color);
	
}

void drawArrows(Status *status){
	for(auto arrow:arrows){
		//cout<<arrow.first.x<<" "<<arrow.first.y<<" - "<<arrow.second.x<<" "<<arrow.second.y<<endl;
		drawArrow(arrow.first, arrow.second);
	}
}


