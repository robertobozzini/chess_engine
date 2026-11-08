#include "pieces.h"
#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
Texture2D texall[2][6];

int piece_value[7] = {1,3,3,5,9,1000,0};
float scalepiece = 0.5f;

void load_pieces_textures(void){
    texall[0][0] = LoadTexture("../images/bp.png");
	texall[0][1] = LoadTexture("../images/bn.png");
	texall[0][2] = LoadTexture("../images/bb.png");
	texall[0][3] = LoadTexture("../images/br.png");
	texall[0][4] = LoadTexture("../images/bq.png");
	texall[0][5] = LoadTexture("../images/bk.png");
	
	texall[1][0] = LoadTexture("../images/wp.png");
	texall[1][1] = LoadTexture("../images/wn.png");
	texall[1][2] = LoadTexture("../images/wb.png");
	texall[1][3] = LoadTexture("../images/wr.png");
	texall[1][4] = LoadTexture("../images/wq.png");
	texall[1][5] = LoadTexture("../images/wk.png");
}

uint64_t pawn_att[2][64], knight_att[64], king_att[64], ray[8][64]; //attack tables
//{E, W, N, S, NE, NW, SE, SW}; 
Vector2 directions[8]={{-1,0},{+1,0},{0,-1},{0,+1},{-1,-1},{+1,-1},{-1,+1},{+1,+1}};

uint64_t poss_moves;

static void add_if_in(uint64_t& bb, int col, int row){
	//if(row<0 || row>=8 || col<0 || col>=8)return;
	Vector2 pos = (Vector2){(float)col,(float)row};
	if(!in_board(pos))return;

	int sq = coord_to_sq(pos);
	set_bit(bb, sq);
}

void att_tables_init(void){
	//ps for the pawns i need to add captures
	static const int KN[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    static const int KG[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
	for(int sq=0; sq<64;sq++){
		auto [col,row] = sq_to_coord(sq);

		//pawns
		add_if_in(pawn_att[0][sq], col, row+1);
		add_if_in(pawn_att[1][sq], col, row-1);
		if(row == 1)add_if_in(pawn_att[0][sq], col, row+2);//2 square step if you are at a certain sqare with a certain color
		else if (row == 6)add_if_in(pawn_att[1][sq], col, row-2);

		for(auto &dir:KN)//knight
			add_if_in(knight_att[sq], col+dir[0], row + dir[1]);

		for(auto &dir:KG)//king
			add_if_in(king_att[sq], col+dir[0], row + dir[1]);

		for(int dir=E;dir<=SW;dir++){
			ray[dir][sq] = 0;//devo mettere roba
			Vector2 pos = sq_to_coord(sq);
			pos = Vector2Add(pos,directions[dir]);

			while(in_board(pos)){
				add_if_in(ray[dir][sq], pos.x, pos.y);
				pos = Vector2Add(pos,directions[dir]);
			}
			
		}
	}
}
uint64_t find_illegal(Position state){//for a certain position 
	//probably will change -> checks if position is illegal? my checking if king is attacked? (old approach) -> probably not optimal 
	//mmmmm
	uint64_t illegal_moves = 0UL;

	return illegal_moves;
}

uint64_t ray_attacks(u_int8_t dir, int sq, uint64_t occ){
	uint64_t table = ray[dir][sq];
	uint64_t blockers = table & occ;
	int bit;
	bool increasesq = (dir == N || dir == E || dir == NE || dir == NW);
	bit = (increasesq)? lsb(blockers) : msb(blockers);
	return table & ~ray[dir][bit];
}

uint64_t find_moves(Position state, u_int8_t piece, int sq){
	uint64_t moves_table = 0UL;
	bool turn = state.w_to_move;

	switch(piece){
		case PAWN:
			moves_table = pawn_att[turn][sq]&~state.occ[turn];
			break;
		case KNIGHT:
			moves_table = knight_att[sq]&~state.occ[turn];
			break;
		case KING:
			moves_table = king_att[sq]&~state.occ[turn];
			break;
		case BISHOP:
			for(int dir=NE;dir<=SW;dir++){
				moves_table &= ray_attacks(dir, sq, state.all);
			}
			moves_table &=~state.occ[turn];
			break;
		case ROOK:
			for(int dir=E;dir<=S;dir++){
				moves_table &=ray_attacks(dir, sq, state.all);
			}
			moves_table &=~state.occ[turn];
			break;
		case QUEEN:
			for(int dir=E;dir<=SW;dir++){
				moves_table &= ray_attacks(dir, sq, state.all); 
			}
			moves_table &=~state.occ[turn];
			break;
	}

	//moves_table = moves_table & ~find_illegal(state);//ITS NOT ACTUALLY LIKE THIS
	return moves_table;
}

void draw_piece(bool turn, u_int8_t piece, Vector2 pos){

    DrawTextureEx(
        texall[turn][piece],
        pos,
        0,
        scalepiece,
        WHITE
    );
}

void pieces_draw(bool turn){
	Position lastp = currentp;
	for (int i=0;i<6;i++){//type of piece currently looking at
		while(lastp.pieces[turn][i]){
			int sq = pop_lsb(lastp.pieces[turn][i]);
			//draw
			Vector2 coords = sq_to_coord(sq);

			if(!settings[TURN]){//se è nero devo invertire
				coords = invert_coord(coords);
			}
			int col = coords.x;
			int row = coords.y;


			Vector2 pos;
			pos.x = corner.x + squaresize*col + (float)squaresize/2.0f - texall[turn][i].width*scalepiece/2.0f;
			pos.y = corner.y + squaresize*row + (float)squaresize/2.0f - texall[turn][i].height*scalepiece/2.0f;
			draw_piece(turn,i,pos);
		}
	}
}
