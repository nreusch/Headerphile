#pragma once

#include <array>
//
//#include "Texture.h"


struct Texture
{
	void SetIsSolid( bool solid )
	{
		isSolid = solid;
	}
	bool GetIsSolid( )
	{
		return isSolid;
	}
	void LoadTexture( SDL_Renderer *renderer, const std::string &str, const SDL_Rect &windowPos )
	{
		SDL_Surface* surface = IMG_Load( str.c_str() );

		texture = SDL_CreateTextureFromSurface( renderer, surface );

		SDL_FreeSurface( surface );

		CalculateSize( );
	}
	void Render( SDL_Renderer* renderer ) const
	{
		SDL_RenderCopyEx( renderer, texture, nullptr, &rect,  angle, &center, SDL_FLIP_NONE );
	}
	void CalculateSize( )
	{
		// Find width and height of texture
		int32_t width = 0;
		int32_t height = 0;
		SDL_QueryTexture( texture, nullptr, nullptr, &width, &height );

		// Set size
		rect.w = width;
		rect.h = height;
	}
	void MoveDown()
	{
		rect.y += rect.h + 10;
	}
	void MoveRight()
	{
		rect.x += rect.h + 10;
	}
	void MoveLeft()
	{
		rect.x -= rect.h + 10;
	}
	void operator=( const Texture &rhs )
	{
		texture = rhs.texture;
		isSolid = rhs.isSolid;
	}
	double angle;
	SDL_Point center;
	SDL_Texture* texture;
	SDL_Rect rect;

	bool isSolid;
};
enum class PieceType
{
	L,
	Inverted_L,
	Stair_Right,
	Stair_Left,
	Roof,
	Square,
	Tetris
};
struct PieceLayout
{
	PieceLayout( )
	{
		pieceTexture = "red_tile.png";
		type = PieceType::L;
	}
	bool operator==( const PieceLayout &other )
	{
		return type == other.type;
	}

	std::array< std::array< bool, 4 > , 4 > layout;
	PieceType type;
	std::string pieceTexture;
};
struct TetrisPiece
{
	public:
		TetrisPiece()
		{
			blockSize = 20;
			blockMargin = 10;
		}
		void Init( SDL_Renderer* renderer, const std::string &str, const PieceLayout &layout, const SDL_Rect &windowPos )
		{
			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
				{
					textures[column][row].SetIsSolid( layout.layout[row][column] );

					if ( layout.layout[row][column] == true )
						textures[column][row].LoadTexture( renderer, layout.pieceTexture, windowPos );
					else
						textures[column][row].LoadTexture( renderer, "empty_tile.png", windowPos );
				}
			}

			PositionPieces();
			UpdateRectPosition();

			AdjustRect();
		}
		void UpdateRectPosition()
		{
			pieceRect.x = textures[0][0].rect.x;
			pieceRect.y = textures[0][0].rect.y;
		}
		void Render( SDL_Renderer* renderer ) const
		{
			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
					textures[column][row].Render( renderer );
			}

			SDL_SetRenderDrawColor( renderer, 255, 255, 0, 255 );
			SDL_RenderDrawRect( renderer, &pieceRect );
			SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		}
		int32_t GetLeftMostX()
		{
			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
				{
					if ( textures[column][row].GetIsSolid() )
						return textures[column][row].rect.x;
				}
			}

			return -1;
		}
		void MoveDown()
		{
			return;
			for ( auto &p : textures )
				for ( auto &q : p )
					q.MoveDown();

			pieceRect.y += ( blockSize + blockMargin );
		}
		void MoveLeft()
		{
			for ( auto &p : textures )
				for ( auto &q : p )
					q.MoveLeft();

			pieceRect.x -= ( blockSize + blockMargin );
		}
		void MoveRight()
		{
			for ( auto &p : textures )
				for ( auto &q : p )
					q.MoveRight();

			pieceRect.x += ( blockSize + blockMargin );
		}
		int32_t GetRightMostX()
		{
			for ( int column = 3 ; column != 0 ; --column )
			{
				for ( int row = 3 ; row != 0 ;  --row )
				{
					if ( textures[column][row].GetIsSolid() )
						return  textures[column][row].rect.x + textures[column][row].rect.w;
				}
			}

			return -1;
		}
		void Rotate( const SDL_Rect &bounds )
		{
			std::array< std::array< Texture, 4 > , 4 > copied;

			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
					copied[column][row] = textures[column][row];
			}

			textures[0][0] = copied[0][3];
			textures[1][0] = copied[0][2];
			textures[2][0] = copied[0][1];
			textures[3][0] = copied[0][0];

			textures[0][1] = copied[1][3];
			textures[0][2] = copied[2][3];
			textures[0][3] = copied[3][3];

			textures[1][3] = copied[3][2];
			textures[2][3] = copied[3][1];
			textures[3][3] = copied[3][0];

			textures[3][1] = copied[1][0];
			textures[3][2] = copied[2][0];
			textures[3][3] = copied[3][0];


			textures[1][1] = copied[1][2];
			textures[2][1] = copied[1][1];

			textures[1][2] = copied[2][2];

			textures[2][2] = copied[2][1];

			textures[2][1] = copied[1][1];

			AdjustRect();

			if ( pieceRect.x < bounds.x || ( pieceRect.x + pieceRect.w ) > ( bounds.x + bounds.w ) )
			{
				std::cout << "Out of bounds!\n";

				for ( int column = 0 ; column < 4 ; ++column )
				{
					for ( int row = 0 ; row < 4 ; ++row )
						textures[column][row] = copied[column][row];
				}
			}
		}
		SDL_Rect GetRect()
		{
			return pieceRect;
		}

	private:
		void PositionPieces( )
		{
			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
				{
					if ( column > 0 )
						textures[column][row].rect.x
							= textures[column- 1][row].rect.x + textures[column - 1][row].rect.w + 10;

					if ( row > 0 )
						textures[column][row].rect.y
							= textures[column][row- 1].rect.y + textures[column][row - 1 ].rect.h + 10;
				}
			}

			pieceRect.w = textures[3][3].rect.x + textures[3][3].rect.w - pieceRect.x;
			pieceRect.h = textures[3][3].rect.y + textures[3][3].rect.h - pieceRect.y;
		}
		void AdjustRect()
		{
			int32_t min = 3;
			int32_t max = 0;
			for ( int column = 0 ; column < 4 ; ++column )
			{
				for ( int row = 0 ; row < 4 ; ++row )
				{
					if ( textures[column][row].GetIsSolid( ) )
					{
						if ( column > max )
							max = column;

						if ( column < min )
							min = column;

						std::cout << column << ", " << row << " is solid\n";
					}
				}
			}

			std::cout << "min/max : " << min << ", " << max << std::endl;

			pieceRect.x = textures[min][0].rect.x;
			pieceRect.w = ( textures[max][0].rect.x + textures[max][0].rect.w ) - pieceRect.x;

		}
		int32_t blockSize;
		int32_t blockMargin;
		SDL_Rect pieceRect;
		std::array< std::array< Texture, 4 > , 4 > textures;
};
