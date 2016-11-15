#include <engine/game_state.h>

#include <sbis-lib/serialization/advanced_serialization.hpp>
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-lib/utils/stringify.hpp>

namespace sbis
{

void Serialization<game::GameState>::Read( IObjectReader& reader, game::GameState& state )
{
   IObjectReaderPtr p = reader[ L"players" ];
   auto& iter = p->CreateIterator();
   state.players.clear();
   for( iter.First(); !iter.IsDone(); iter.Next() )
   {
      game::Snake s;
      ReadValue( *iter.Get(), s );
      state.players.emplace( FromString<Uuid>( iter.MemberName() ), s );
   }
}

void Serialization<game::GameState>::Write( IObjectWriter& writer, const game::GameState& state )
{
   writer.BeginWriteObject();
   writer.BeginWriteObjectElem( L"players" );
   writer.BeginWriteObject();
   for( const auto& elem : state.players )
   {
      String key = UUIDToStringWithoutHyphen( elem.first );
      writer.BeginWriteObjectElem( key.c_str() );
      WriteValue( writer, elem.second );
      writer.EndWriteObjectElem( key.c_str() );
   }
   writer.EndWriteObject();
   writer.EndWriteObjectElem( L"players" );
   writer.EndWriteObject();
}

} // namespace sbis
