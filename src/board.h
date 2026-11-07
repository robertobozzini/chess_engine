#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "main.h"
#include <cstdint>



enum Square : int { 
A1, A2, A3, A4, A5, A6, A7, A8, 
B1, B2, B3, B4, B5, B6, B7, B8, 
C1, C2, C3, C4, C5, C6, C7, C8, 
D1, D2, D3, D4, D5, D6, D7, D8, 
E1, E2, E3, E4, E5, E6, E7, E8, 
F1, F2, F3, F4, F5, F6, F7, F8, 
G1, G2, G3, G4, G5, G6, G7, G8, 
H1, H2, H3, H4, H5, H6, H7, H8,
	NO_SQ };



struct Position {
    uint64_t pieces[2][6];   //[color][type] -> in un solo numero mi salvo tutte le posizioni in cui un pezzo di un colore si trova: se il bit è 1 se ne trova 1
    uint64_t occ[2]; //occupied: fai gli or tra i 6 pezzi di un colore
    uint64_t all; //or di occ[0] e occ[1]
    uint8_t  type_on[64]; 
    bool w_to_move; //white to move?
    //uint8_t castling; 
    //int8_t enpas_sq; //casa di en passant: -1 se nessuna
    //uint8_t halfmove; 
};
extern Position startingp;
extern Position currentp;
extern Position emptyp;

extern std::vector<Position>record;



extern Vector2 corner;
extern int squaresize, chessboardsize;


//BIT OPERATIONS ON BOARD
inline int lsb(uint64_t bb) { 
	if(bb == 0)
		return NO_SQ;//thecnically its -1 but i cant
	return __builtin_ctzll(bb); 
}//HAVE TO CHECK ITS NOT 64 when i use it: almost impossible but who knows -> possible if no pieces of a type
inline int msb(uint64_t bb) { 
	if(bb == 0)
		return NO_SQ;//this time its correct
	return 63 - __builtin_clzll(bb); //63 - leading 0s
}
inline int pop_lsb(uint64_t& bb) { 
	int s = lsb(bb); 
	bb &= bb - 1; 
	return s; //remind to check if 64
} 
inline bool is_occ(uint64_t bb, int sq) { return bb >> sq & 1; }//checks if square number sq is occupied in board bb
inline void set_bit(uint64_t& bb, int sq)   { bb |= 1ULL << sq; }
inline void clear_bit(uint64_t& bb, int sq) { bb &= ~(1ULL << sq); }

void clear_board_piece(Position& board, bool color, int sq, int piece);
void set_board_piece(Position& board, bool color, int sq, int piece);

bool on_board(Vector2 pos);
int invert_onb(int sq);
Vector2 invert_coord(Vector2 coord);

int coord_to_sq(Vector2 coord);
Vector2 sq_to_coord(int sq);

Vector2 chessboard_pos(Vector2 abs_pos);

void board_reset(void);
void board_init(void);

void change_turn(void);

void board_draw(void);

#endif // CHESSBOARD_H