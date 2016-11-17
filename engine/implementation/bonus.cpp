#include <engine/bonus.h>

namespace sbis
{
namespace game
{

bool Bonus::operator< ( const Bonus& another ) const
{
   if( mPosition.getX() == another.mPosition.getX() )
      return mPosition.getY() < another.mPosition.getY();
   return mPosition.getX() < another.mPosition.getX();
}

} // namespace game

void Serialization<game::Bonus>::Read( IObjectReader& reader, game::Bonus& bonus )
{
   game::Vector2D p;
   ReadValue( reader, p );
   bonus = game::Bonus( p );
}

void Serialization<game::Bonus>::Write( IObjectWriter& writer, const game::Bonus& bonus )
{
   WriteValue( writer, bonus.Position() );
}

} // namespace sbis
