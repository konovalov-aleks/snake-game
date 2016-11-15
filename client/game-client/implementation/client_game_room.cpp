#include "client_game_room.h"

#include <sbis-bl-core/object.hpp>

namespace sbis
{
namespace game
{

ClientGameRoom& ClientGameRoom::Instance()
{
   static ClientGameRoom instance;
   return instance;
}

ClientGameRoom::ClientGameRoom() :
   mSyncThread( &ClientGameRoom::SyncThreadFunc, this ),
   mLastFrameTime( boost::posix_time::microsec_clock::universal_time() )
{}

void ClientGameRoom::SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction )
{
   blcore::EndPoint endpoint( L"gameserver" );
   GameRoom::SetPlayerDirection( pid, direction );
   blcore::Object( L"GameRoom", endpoint ).Invoke<void>( L"SetPlayerDirection", pid, direction ); 
}

void ClientGameRoom::Run()
{
   boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
   GameRoom::Run( static_cast<int>( ( now - mLastFrameTime ).total_milliseconds() ) );
   mLastFrameTime = now;
}

boost::uuids::uuid ClientGameRoom::DoEnter()
{
   blcore::EndPoint endpoint( L"gameserver" );
   return blcore::Object( L"GameRoom", endpoint ).Invoke<boost::uuids::uuid>( L"Enter" ); 
}

void ClientGameRoom::FastSync()
{
}

void ClientGameRoom::FullSync()
{
   blcore::EndPoint endpoint( L"gameserver" );
   SetState( blcore::Object( L"GameRoom", endpoint ).Invoke<GameState>( L"State" ) );
   mLastFrameTime = boost::posix_time::microsec_clock::universal_time();
}

void ClientGameRoom::SyncThreadFunc()
{
   for( ;; )
   {
      FullSync();
      boost::this_thread::sleep( boost::posix_time::milliseconds( 50 ) );
   }
}

} // namespace game
} // namespace sbis
