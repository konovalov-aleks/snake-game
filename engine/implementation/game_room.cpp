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

GameRoom::GameRoom() : mWorldDimensions{ { -128, -128 }, { 128, 128 } }
{}

const Snake& GameRoom::Enter()
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   Snake snake = DoEnter();
   return mPlayers.emplace( snake.ID(), snake ).first->second;
}

void GameRoom::SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   auto player = mPlayers.find( boost::cref( pid ) );
   if( SBIS_UNLIKELY( player == mPlayers.end() ) )
      Error<Exception>( L"Unknown user <" + ToString( pid ) + L'>' );
   player->second.SetDirection( direction );
}

void GameRoom::AddBonus( const Vector2D& position )
{
   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   mBonuses.insert( position );
}

void GameRoom::Run( int dt )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   std::vector<decltype(mPlayers)::iterator> killed_snakes;
   for( auto it = mPlayers.begin(); it != mPlayers.end(); ++it )
   {
      Snake& snake = it->second;
      Vector2D old_head_pos = snake.Points().front();
      snake.Move( dt );
      Vector2D new_head_pos = snake.Points().front();
      if( SBIS_UNLIKELY(
            new_head_pos.getX() < WorldDimensions().first.getX() ||
            new_head_pos.getY() < WorldDimensions().first.getY() ||
            new_head_pos.getX() > WorldDimensions().second.getX() ||
            new_head_pos.getY() > WorldDimensions().second.getY() ||
            CheckCollisions( snake, old_head_pos ) ) )
      {
         killed_snakes.push_back( it );
         LogMsg( L"snake killed" );
      }
      else
         EatBonuses( snake, old_head_pos );
   }
   for( auto& it : killed_snakes )
   {
      OnSnakeKilled( it->second );
      mPlayers.erase( it );
   }
}

namespace
{

inline float area(const Vector2D& a, const Vector2D& b, const Vector2D& c)
{
   return ( b.getX() - a.getX() ) * ( c.getY() - a.getY() ) -
          ( b.getY() - a.getY() ) * ( c.getX() - a.getX() );
}

inline bool intersect_1( float a, float b, float c, float d)
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

void GameRoom::OnSnakeKilled( const Snake& snake )
{
   for( const Vector2D& p : snake.Points() )
      AddBonus( p );
}

void GameRoom::EatBonuses( const Snake& /*snake*/, const Vector2D& /*old_head_pos*/ )
{
   // TODO найти бонусы, которые съела змея за данный такт, убрать их с поля и увеличить змею
}

std::vector<Snake> GameRoom::Players() const
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   std::vector<Snake> res;
   res.reserve( mPlayers.size() );
   for( auto& it : mPlayers )
      res.push_back( it.second );
   return res;
}

void GameRoom::SetPlayers( std::vector<Snake> players )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   mPlayers.clear();
   for( Snake& s : players )
   {
      boost::uuids::uuid id = s.ID();
      mPlayers.emplace( std::move( id ), std::move( s ) );
   }
}

std::vector<Bonus> GameRoom::Bonuses() const
{
   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   return std::vector<Bonus>( mBonuses.begin(), mBonuses.end() );
}

void GameRoom::SetBonuses( std::vector<Bonus> bonuses )
{
   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   mBonuses = std::set<Bonus>( bonuses.begin(), bonuses.end() );
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

void GameRoom::ApplyStateDelta( GameState state )
{
   {
      boost::unique_lock<boost::mutex> lock( mBonusesMtx );
      for( Bonus& b : state.bonuses )
         mBonuses.insert( std::move( b ) );
   }
   {
      boost::unique_lock<boost::mutex> lock( mPlayersMtx );
      for( Snake& s : state.players )
      {
         auto iter = mPlayers.find( s.ID() );
         if( iter != mPlayers.end() )
         {
            auto& local_points = iter->second.Points();
            auto& remote_points = s.Points();
            for( size_t i = 0; i < std::min( local_points.size(), remote_points.size() ); ++i )
               local_points[ i ] = remote_points[ i ];
         }
      }
   }
}

} // namespace game
} // namespace sbis
