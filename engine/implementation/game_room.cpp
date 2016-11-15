#include <engine/game_room.h>

#include <sbis-lib/errors/error.hpp>
#include <sbis-lib/serialization/advanced_serialization.hpp>
#include <sbis-lib/serialization/serialization.hpp>
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-rpc/rpc_func.hpp>

#include <sbis-lib/utils/log_msg.hpp>

namespace sbis
{
namespace game
{

GameRoom::GameRoom()
{}

boost::uuids::uuid GameRoom::Enter()
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   return mPlayers.emplace( DoEnter(), Snake( Vector2D( 30, 30 ), 150.0, Vector2D( 0, 1 ), 30 ) ).first->first;
}

void GameRoom::SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   auto player = mPlayers.find( pid );
   if( SBIS_UNLIKELY( player == mPlayers.end() ) )
      Error<Exception>( L"Unknown user <" + ToString( pid ) + L'>' );
   player->second.SetDirection( direction );
}

void GameRoom::Run( int dt )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   std::vector<boost::unordered_map<boost::uuids::uuid, Snake>::iterator> killed_snakes;
   for( auto it = mPlayers.begin(); it != mPlayers.end(); ++it )
   {
      Vector2D head_pos = it->second.Points().front();
      it->second.Move( dt );
      if( CheckCollisions( it->second, head_pos ) )
      {
         killed_snakes.push_back( it );
         LogMsg( L"snake killed" );
      }
      else
         EatBonuses( it->second, head_pos );
   }
   for( auto& it : killed_snakes )
   {
      OnSnakeKilled( it->second );
      mPlayers.erase( it );
   }
}

namespace
{

inline double area(const Vector2D& a, const Vector2D& b, const Vector2D& c)
{
   return ( b.getX() - a.getX() ) * ( c.getY() - a.getY() ) -
          ( b.getY() - a.getY() ) * ( c.getX() - a.getX() );
}

inline bool intersect_1( double a, double b, double c, double d)
{
   if( a > b )
      std::swap( a, b );
   if( c > d )
      std::swap( c, d );
   return std::max( a, c ) <= std::min( b, d );
}
 
bool intersect( const Vector2D& a, const Vector2D& b, const Vector2D& c, const Vector2D& d )
{
   return intersect_1( a.getX(), b.getX(), c.getX(), d.getX() )
       && intersect_1( a.getY(), b.getY(), c.getY(), d.getY() )
       && area( a, b, c ) * area( a, b, d ) <= 0
       && area( c, d, a ) * area( c, d, b ) <= 0;
}

} // namespace

bool GameRoom::CheckCollisions( const Snake& snake, const Vector2D& old_head_pos )
{
   const Vector2D& head_pos = snake.Points().front();
   for( const auto& s : mPlayers )
   {
      if( &s.second == &snake )
         continue;
      const auto& points = s.second.Points();
      for( size_t i = 1; i < points.size(); ++i )
         if( intersect( points[i - 1], points[i], head_pos, old_head_pos ) )
            return true;
   }
   return false;
}

void GameRoom::OnSnakeKilled( const Snake& /*snake*/ )
{
   // TODO создать бонусы
}

void GameRoom::EatBonuses( const Snake& /*snake*/, const Vector2D& /*old_head_pos*/ )
{
   // TODO найти бонусы, которые съела змея за данный такт, убрать их с поля и увеличить змею
}

boost::unordered_map<boost::uuids::uuid, Snake> GameRoom::Players() const
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   return mPlayers;
}

void GameRoom::SetPlayers( boost::unordered_map<boost::uuids::uuid, Snake> players )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   mPlayers = std::move( players );
}

std::vector<Bonus> GameRoom::Bonuses() const
{
   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   return mBonuses;
}

void GameRoom::SetBonuses( std::vector<Bonus> bonuses )
{
   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   mBonuses = std::move( bonuses );
}

GameState GameRoom::State()
{
   GameState res;
   res.players = Players();
   res.bonuses = Bonuses();
   return res;
}

void GameRoom::SetState( GameState state )
{
   SetPlayers( std::move( state.players ) );
   SetBonuses( std::move( state.bonuses ) );
}

} // namespace game
} // namespace sbis
