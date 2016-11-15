#include <engine/snake.h>

#include <sbis-lib/serialization/advanced_serialization.hpp>

namespace sbis
{
namespace game
{

Snake::Snake()
   : mLinearSpeed()
{}

Snake::Snake( const Vector2D& pos, double speed, const Vector2D& direction, UInt32 length )
   : mLinearSpeed( speed ), mPoints( length, pos )
{
   SetDirection( direction );
}

void Snake::Move( int dt )
{
   for( auto iter = mPoints.rbegin(); iter != mPoints.rend(); ++iter )
   {
      auto next = std::next( iter );
      if( SBIS_UNLIKELY( next == mPoints.rend() ) )
         *iter += mSpeed * dt / 1000;
      else
      {
         Vector2D delta( next->getX() - iter->getX(), next->getY() - iter->getY() );
         delta *= 2 * dt / 1000.0;
         *iter += delta;
      }
   }
}

void Snake::SetDirection( Vector2D direction )
{
   direction.normalize();
   mSpeed = direction * mLinearSpeed;
}

} // namespace game

void Serialization<game::Snake>::Read( IObjectReader& reader, game::Snake& snake )
{
   ReadValue( *reader[ L"lspeed" ], snake.mLinearSpeed );
   ReadValue( *reader[ L"speed" ], snake.mSpeed );
   ReadValue( *reader[ L"points" ], snake.mPoints );
}

void Serialization<game::Snake>::Write( IObjectWriter& writer, const game::Snake& snake )
{
   writer.BeginWriteObject();
   writer.BeginWriteObjectElem( L"lspeed" );
   WriteValue( writer, snake.mLinearSpeed );
   writer.EndWriteObjectElem( L"lspeed" );
   writer.BeginWriteObjectElem( L"speed" );
   WriteValue( writer, snake.mSpeed );
   writer.EndWriteObjectElem( L"speed" );
   writer.BeginWriteObjectElem( L"points" );
   WriteValue( writer, snake.mPoints );
   writer.EndWriteObjectElem( L"points" );
   writer.EndWriteObject();
}

} // namespace sbis

