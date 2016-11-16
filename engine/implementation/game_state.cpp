#include <engine/game_state.h>

#include <sbis-lib/serialization/advanced_serialization.hpp>
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-lib/utils/stringify.hpp>

namespace sbis
{

void Serialization<game::GameState>::Read( IObjectReader& reader, game::GameState& state )
{
   IObjectReaderPtr p = reader[ L"players" ];
   if( p )
      ReadValue( *p, state.players );
   p = reader[ L"bonuses" ];
   if( p )
      ReadValue( *p, state.bonuses );
}

void Serialization<game::GameState>::Write( IObjectWriter& writer, const game::GameState& state )
{
   writer.BeginWriteObject();
   writer.BeginWriteObjectElem( L"players" );
   WriteValue( writer, state.players );
   writer.EndWriteObjectElem( L"players" );
   writer.BeginWriteObjectElem( L"bonuses" );
   WriteValue( writer, state.bonuses );
   writer.EndWriteObjectElem( L"bonuses" );
   writer.EndWriteObject();
}

} // namespace sbis
