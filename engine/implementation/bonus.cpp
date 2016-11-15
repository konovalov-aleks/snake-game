#include <engine/bonus.h>

namespace sbis
{

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
