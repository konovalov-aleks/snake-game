#include <engine/snake.h>

#include <sbis-lib/serialization/advanced_serialization.hpp>

namespace sbis
{
namespace game
{

Snake::Snake()
   : mLinearSpeed(), mColor()
{}

Snake::Snake( boost::uuids::uuid id, const Vector2D& pos, float speed, const Vector2D& direction,
              UInt32 length, UInt32 color)
   : mLinearSpeed( speed ), mId( std::move( id ) ), mColor( color ), mPoints( length, pos )
{
   SetDirection( direction );
}

void Snake::Move( int dt )
{
   for( auto iter = mPoints.rbegin(); iter != mPoints.rend(); ++iter )
   {
      auto next = std::next( iter );
      if( SBIS_UNLIKELY( next == mPoints.rend() ) )
         *iter += mSpeed * static_cast<float>( dt ) / 1000.0f;
      else
      {
         Vector2D delta( next->getX() - iter->getX(), next->getY() - iter->getY() );
         delta *= static_cast<float>( dt ) / 100.0f;
         *iter += delta;
      }
   }
}

void Snake::SetDirection( Vector2D direction )
{
   // TODO не давать повернуть слишком резко
   direction.normalize();
   mSpeed = direction * mLinearSpeed;
}

void Snake::Grow( size_t size )
{
   mPoints.insert( mPoints.end(), size, mPoints.back() );
}

} // namespace game

void Serialization<game::Snake>::Read( IObjectReader& reader, game::Snake& snake )
{
   ReadValue( *reader[ L"id" ], snake.mId );
   ReadValue( *reader[ L"lspeed" ], snake.mLinearSpeed );
   ReadValue( *reader[ L"speed" ], snake.mSpeed );
   ReadValue( *reader[ L"points" ], snake.mPoints );
   ReadValue( *reader[ L"color" ], snake.mColor );
}

void Serialization<game::Snake>::Write( IObjectWriter& writer, const game::Snake& snake )
{
   writer.BeginWriteObject();
   writer.BeginWriteObjectElem( L"id" );
   WriteValue( writer, snake.mId );
   writer.EndWriteObjectElem( L"id" );
   writer.BeginWriteObjectElem( L"lspeed" );
   WriteValue( writer, snake.mLinearSpeed );
   writer.EndWriteObjectElem( L"lspeed" );
   writer.BeginWriteObjectElem( L"speed" );
   WriteValue( writer, snake.mSpeed );
   writer.EndWriteObjectElem( L"speed" );
   writer.BeginWriteObjectElem( L"points" );
   WriteValue( writer, snake.mPoints );
   writer.EndWriteObjectElem( L"points" );
   writer.BeginWriteObjectElem( L"color" );
   WriteValue( writer, snake.mColor );
   writer.EndWriteObjectElem( L"color" );
   writer.EndWriteObject();
}

} // namespace sbis

