#include "Texture_Text.h"

class TextRect : public Texture_Text
{
	public:
	TextRect()
		:	rect( { 0,0,0,0 } )
		,	emptyItem( true )
	{
	}
	void SetOuterRect( SDL_Rect rect_ )
	{
		rect = rect_;

		SDL_Point p;
		p.x = rect.x + ( rect.w * 0.5 );
		p.y = rect.y + ( rect.h * 0.5 );
		CenterAtPoint( p );
	}
	void CheckRect( )
	{
		SDL_Rect innerRect = GetRect();

		rect.w = std::max( rect.w, innerRect.w );
		rect.h = std::max( rect.h, innerRect.h );

		SDL_Point p;
		p.x = rect.x + ( rect.w * 0.5 );
		p.y = rect.y + ( rect.h * 0.5 );
		CenterAtPoint( p );
	}
	void IncrementX( int32_t x )
	{
		rect.x += x;
		Texture::IncrementX( x );
	}
	void Render( SDL_Renderer* renderer ) const
	{
		if ( !emptyItem )
		{
			SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
			SDL_RenderFillRect( renderer, &rect );
			Texture_Text::Render( renderer );
		}

		SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
		SDL_RenderDrawRect( renderer, &rect );
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	}
	void RenderValue( SDL_Renderer* renderer, int32_t value )
	{
		Texture_Text::RenderValue( renderer, value );
		emptyItem = false;
		CenterAtPoint( { rect.x + static_cast< int32_t >( rect.w * 0.5 ), rect.y + static_cast< int32_t >( rect.h * 0.5 ) } );
	}
	void RenderValue( SDL_Renderer* renderer, const std::string &str  )
	{
		Texture_Text::RenderText_Solid( renderer, str );
		emptyItem = false;
		CenterAtPoint( { rect.x + static_cast< int32_t >( rect.w * 0.5 ), rect.y + static_cast< int32_t >( rect.h * 0.5 ) } );
	}
	void Remove()
	{
		emptyItem = true;
	}
	SDL_Rect GetOuterRect()
	{
		return rect;
	}
	SDL_Texture* GetTexture() 
	{
		return Texture::GetTexture();
	}
	bool IsEmpty() const
	{
		return emptyItem;
	}

	private:
	bool emptyItem;
	SDL_Rect rect;
};
