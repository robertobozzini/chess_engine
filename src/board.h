#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "main.h"
#include <cstdint>
#include <bit>
#include<vector>

typedef uint64_t U64;

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

enum Piece : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY = 255 };
struct Position {
    U64 pieces[2][6];   //[color][type] -> in un solo numero mi salvo tutte le posizioni in cui un pezzo di un colore si trova: se il bit è 1 se ne trova 1
    U64 occ[2]; //occupied: fai gli or tra i 6 pezzi di un colore
    U64 all; //or di occ[0] e occ[1]
    uint8_t  type_on[64]; 
    bool w_to_move; //white to move?
    //uint8_t castling; 
    //int8_t enpas_sq; //casa di en passant: -1 se nessuna
    //uint8_t halfmove; 
};

extern vector<Position>record;

extern Texture2D texall[2][6];

extern Vector2 corner;
extern int squaresize, chessboardsize;

void board_reset(void);
void board_init(void);

void board_draw(void);

#endif // CHESSBOARD_H