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
   GameRoom::SetPlayerDirection( pid, direction );
   blcore::EndPoint endpoint( L"gameserver" );
   endpoint.SetTimeout( 2000 ); // 2c
   blcore::Object( L"GameRoom", endpoint ).Invoke<void>( L"SetPlayerDirection", pid, direction ); 
}

void ClientGameRoom::Run( const Vector2D& viewport_center, const Vector2D& viewport_size )
{
   boost::unique_lock<boost::mutex> run_lock( mRunMtx );

   boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
   const int time_delta = static_cast<int>( ( now - mLastFrameTime ).total_milliseconds() );
   if( time_delta > 5 )
   {
      GameRoom::Run( static_cast<int>( ( now - mLastFrameTime ).total_milliseconds() ) );
      mLastFrameTime = now;
   }

   boost::unique_lock<boost::mutex> lock( mViewportMtx );
   mViewportCenter = viewport_center;
   mViewportSize = viewport_size;
}

Snake ClientGameRoom::DoEnter()
{
   blcore::EndPoint endpoint( L"gameserver" );
   return blcore::Object( L"GameRoom", endpoint ).Invoke<Snake>( L"Enter" ); 
}

void ClientGameRoom::FastSync()
{
   try
   {
      Vector2D vpcenter, vpsize;
      {
        boost::unique_lock<boost::mutex> lock( mViewportMtx );
        vpcenter = mViewportCenter;
        vpsize = mViewportSize;
      }
      Run( vpcenter, vpsize );
      blcore::EndPoint endpoint( L"gameserver" );
      ApplyStateDelta( blcore::Object( L"GameRoom", endpoint ).Invoke<GameState>( L"PartialState", vpcenter, vpsize ) );
   }
   catch( const Exception& ex )
   {
      ErrorMsg( L"Synchronization failed: " + ex.ErrorMessage() );
   }
}

void ClientGameRoom::FullSync()
{
   try
   {
      Vector2D vpcenter, vpsize;
      {
         boost::unique_lock<boost::mutex> lock( mViewportMtx );
         vpcenter = mViewportCenter;
         vpsize = mViewportSize;
      }
      blcore::EndPoint endpoint( L"gameserver" );
      SetState( blcore::Object( L"GameRoom", endpoint ).Invoke<GameState>( L"State", vpcenter, vpsize ) );
      mLastFrameTime = boost::posix_time::microsec_clock::universal_time();
   }
   catch( const Exception& ex )
   {
      ErrorMsg( L"Synchronization failed: " + ex.ErrorMessage() );
   }
}

void ClientGameRoom::SyncThreadFunc()
{
   for( ;; )
   {
      for( int i = 0; i < 10; ++i )
      {
         FastSync();
         boost::this_thread::sleep( boost::posix_time::milliseconds( 50 ) );
      }
      FullSync();
      boost::this_thread::sleep( boost::posix_time::milliseconds( 50 ) );
   }
}

} // namespace game
} // namespace sbis
