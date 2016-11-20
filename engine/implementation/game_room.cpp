﻿#include <engine/game_room.h>

#include <sbis-lib/errors/error.hpp>
#include <sbis-lib/serialization/advanced_serialization.hpp>
#include <sbis-lib/serialization/serialization.hpp>
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-rpc/rpc_func.hpp>

#include <sbis-lib/utils/log_msg.hpp>

#include <cmath>

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

static const float MAX_ROTATE_ANGLE = 0.2f;
static const float MAX_ROTATE_ANGLE_SIN = std::sin( MAX_ROTATE_ANGLE );
static const float MAX_ROTATE_ANGLE_COS = std::cos( MAX_ROTATE_ANGLE );

void GameRoom::SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction )
{
   boost::unique_lock<boost::mutex> lock( mPlayersMtx );
   auto player = mPlayers.find( boost::cref( pid ) );
   if( SBIS_UNLIKELY( player == mPlayers.end() ) )
      Error<Exception>( L"Unknown user <" + ToString( pid ) + L'>' );

   Vector2D cur_direction = player->second.Speed();

   const float p1 = direction.getX() * cur_direction.getY() - direction.getY() * cur_direction.getX();
   const float p2 = direction.getX() * cur_direction.getX() + direction.getY() * cur_direction.getY();
   const float angle = p2 == 0.0f ? ( p1 > 0 ? M_PI / 2.0f : -M_PI / 2.0f ) : std::atan2( p1, p2 );

   Vector2D new_direction = direction;
   if( fabs( angle ) > MAX_ROTATE_ANGLE )
   {
      float angle_sin = angle > 0 ? -MAX_ROTATE_ANGLE_SIN : MAX_ROTATE_ANGLE_SIN;
      new_direction = Vector2D( cur_direction.getX() * MAX_ROTATE_ANGLE_COS - cur_direction.getY() * angle_sin,
                                cur_direction.getX() * angle_sin + cur_direction.getY() * MAX_ROTATE_ANGLE_COS );
   }

   player->second.SetDirection( new_direction );
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

Vector2D RandomizeBonusPosition( const Vector2D& p )
{
   // детерминированный алгоритм, псевдорандомизирующий позиции бонусов,
   // выпадающих из умерших змей
   int rounded_x = static_cast<int>( p.getX() / 3 );
   int rounded_y = static_cast<int>( p.getY() / 3 );
   float dx = static_cast<float>( static_cast<UInt16>( rounded_x * 997 ) % 5 +
                                  static_cast<UInt16>( rounded_y * 619 ) % 5 - 5 ) / 2.0f;
   float dy = static_cast<float>( static_cast<UInt16>( rounded_x * 787 ) % 5 +
                                  static_cast<UInt16>( rounded_y * 937 ) % 5 - 5 ) / 2.0f;
   return Vector2D( p.getX() + dx, p.getY() + dy );
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
         if( intersect( points[ i - 1 ], points[ i ], head_pos, old_head_pos ) )
            return true;
   }
   return false;
}

void GameRoom::OnSnakeKilled( const Snake& snake )
{
   for( const Vector2D& p : snake.Points() )
      AddBonus( RandomizeBonusPosition( p ) );
}

void GameRoom::EatBonuses( Snake& snake, const Vector2D& /*old_head_pos*/ )
{
   const float eat_radius = 3.0f;
   const Vector2D& head_pos = snake.Points()[0];

   boost::unique_lock<boost::mutex> lock( mBonusesMtx );
   std::vector<decltype(mBonuses)::iterator> eated_bonuses;
   for( auto iter = mBonuses.begin(); iter != mBonuses.end(); ++iter )
   {
      const float dx = head_pos.getX() - iter->Position().getX();
      const float dy = head_pos.getY() - iter->Position().getY();
      if( dx * dx + dy * dy <= eat_radius * eat_radius )
         eated_bonuses.push_back( iter );
   }
   snake.Grow( eated_bonuses.size() );
   for( auto& it : eated_bonuses )
      mBonuses.erase( it );
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
            iter->second.SetDirection( s.Speed() );
         }
      }
   }
}

} // namespace game
} // namespace sbis
