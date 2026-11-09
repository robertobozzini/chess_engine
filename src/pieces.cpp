#include "pieces.h"
#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
Texture2D texall[2][6];

float piece_value[7] = {1.0f,3.0f,3.2f,5.0f,9.0f,1000.0f,0.0f};
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

uint64_t pawn_mv[2][64], pawn_att[2][64], knight_att[64], king_att[64], ray[8][64]; //attack tables
//{E, W, N, S, NE, NW, SE, SW}; 
Vector2 directions[8]={{+1,0},{-1,0},{0,-1},{0,+1},{+1,-1},{-1,-1},{+1,+1},{-1,+1}};//to check
static const int opp[8] = {1,0,3,2,7,6,5,4};//opp[E] = W

uint64_t between[64][64], line[64][64];//between: squares within the line a-b, line: the whole line

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
		add_if_in(pawn_mv[0][sq], col, row+1);
		add_if_in(pawn_att[0][sq], col-1, row +1);
		add_if_in(pawn_att[0][sq], col+1, row +1);

		add_if_in(pawn_mv[1][sq], col, row-1);
		add_if_in(pawn_att[1][sq], col-1, row -1);
		add_if_in(pawn_att[1][sq], col+1, row -1);

		if(row == 1)add_if_in(pawn_mv[0][sq], col, row+2);//2 square step if you are at a certain sqare with a certain color
		else if (row == 6)add_if_in(pawn_mv[1][sq], col, row-2);

		for(auto &dir:KN)//knight
			add_if_in(knight_att[sq], col+dir[0], row + dir[1]);

		for(auto &dir:KG)//king
			add_if_in(king_att[sq], col+dir[0], row + dir[1]);

		for(uint8_t dir=E;dir<=SW;dir++){
			ray[dir][sq] = 0;//devo mettere roba
			Vector2 pos = sq_to_coord(sq);
			pos = Vector2Add(pos,directions[dir]);

			while(in_board(pos)){
				add_if_in(ray[dir][sq], pos.x, pos.y);
				pos = Vector2Add(pos,directions[dir]);
			}
			
		}
	}
	for(int a=0;a<64;a++){
		auto [ac,ar] = sq_to_coord(a);
		for(int dir=E; dir<=SW; dir++){
			uint64_t acc = 0;
			Vector2 pos = Vector2Add((Vector2){ac,ar}, directions[dir]);
			while(in_board(pos)){
				int b = coord_to_sq(pos);
				between[a][b] = acc;
				line[a][b] = ray[dir][a] | ray[opp[dir]][a] | (1ULL<<a);
				acc |= 1ULL<<b;
				pos = Vector2Add(pos, directions[dir]);
			}
		}
	}
}
uint64_t find_illegal(Position state){//for a certain position 
	//probably will change -> checks if position is illegal? my checking if king is attacked? (old approach) -> probably not optimal 
	//mmmmm
	uint64_t illegal_moves = 0ULL;

	return illegal_moves;
}

uint64_t ray_attacks(uint8_t dir, int sq, const uint64_t &occ){
	uint64_t table = ray[dir][sq];
	uint64_t blockers = table & occ;

	if(!blockers) return table;

	int bit;
	bool increasesq = (dir == N || dir == E || dir == NE || dir == NW);
	bit = (increasesq)? lsb(blockers) : msb(blockers);
	return table & ~ray[dir][bit];
}

uint64_t bishop_attacks(const uint64_t &occ, int sq){
	uint64_t table = 0ULL;
	for(uint8_t dir=NE;dir<=SW;dir++){
		table |= ray_attacks(dir, sq, occ);
	}
	return table;
}

uint64_t rook_attacks(const uint64_t& occ, int sq){
	uint64_t table = 0ULL;
	for(uint8_t dir=E;dir<=S;dir++){
		table |= ray_attacks(dir, sq, occ);
	}
	return table;
}

uint64_t attackers(const Position &state, int sq, bool by, uint64_t occ){
	    return (pawn_att[!by][sq] & state.pieces[by][PAWN])      // tabella del colore OPPOSTO
         | (knight_att[sq] & state.pieces[by][KNIGHT]) | (king_att[sq] & state.pieces[by][KING])
         | (bishop_attacks(occ, sq) & (state.pieces[by][BISHOP]|state.pieces[by][QUEEN]))
         | (rook_attacks(occ, sq)   & (state.pieces[by][ROOK]  |state.pieces[by][QUEEN]));
}

uint64_t check_en_pas(const Position &state, int sq){//returns whether and where a pawn from square sq can enpassant in a certain position
	uint64_t table = 0ULL;
	//probably not the best: i have to also check if this move is an en passant later on (or could just check a "Move" type element??)
	if(state.lastmove.flag != EN_PASSANT) return 0ULL;
	//Move lastmove = state.lastmove;
	Vector2 topos = sq_to_coord(state.lastmove.to);
	Vector2 pawnpos = sq_to_coord(sq);

	if(pawnpos.y == topos.y){
		if(pawnpos.x +1 == topos.x){//if true pawnpos.x+1 is inside the board

		}
		else if(pawnpos.x -1 == topos.x){

		}
	}
	return table;
}

uint64_t find_moves(const Position &state, uint8_t piece, int sq){
	uint64_t moves_table = 0ULL; //pseudo legal
	bool turn = state.w_to_move;

	switch(piece){
		case PAWN:{
			moves_table =pawn_mv[turn][sq]&~state.all;//non deve essere occupato da nessuno
			
			Vector2 tmp = sq_to_coord(sq);//check per evitare che pedina salti pezzo in mezzo
			float col,row;
			col =tmp.x; row = tmp.y;
			int nsq1, nsq2; 
			row = (turn)? row-1:row+1;
			nsq1 = coord_to_sq((Vector2){col,row});
			row = (turn)? row-1:row+1;
			nsq2 = coord_to_sq((Vector2){col,row});

			if(is_occ(state.all, nsq1) && is_occ(moves_table, nsq2)){
				//devo rimediare
				moves_table &= ~(1ULL<<nsq2);
			}
			moves_table |= pawn_att[turn][sq] & state.occ[!turn];
			// + en passant
			//moves_table |= check_en_pas(state, sq);
			break;
		}
		case KNIGHT:
			moves_table = knight_att[sq]&~state.occ[turn];
			break;
		case KING:
			moves_table = king_att[sq]&~state.occ[turn];
			//+ short && long castle
			break;
		case BISHOP:
			moves_table = bishop_attacks(state.all, sq);
			moves_table &=~state.occ[turn];
			break;
		case ROOK:
			moves_table = rook_attacks(state.all, sq);
			moves_table &=~state.occ[turn];
			break;
		case QUEEN:
			moves_table = bishop_attacks(state.all, sq) | rook_attacks(state.all, sq);
			moves_table &=~state.occ[turn];
			break;
	}
	//gotta filter out illegal moves
	/*
	i could check IF the piece can move (on same diagonal with king?)
	*/

	//moves_table = moves_table & ~find_illegal(state);//ITS NOT ACTUALLY LIKE THIS
	return moves_table;
}



void draw_piece(bool turn, uint8_t piece, Vector2 pos){

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
