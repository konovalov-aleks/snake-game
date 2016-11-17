#pragma once

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/uuid/uuid.hpp>

#include "bonus.h"
#include "game_state.h"
#include "snake.h"
#include "vector2d.h"

namespace sbis
{
namespace game
{

class GameRoom : boost::noncopyable
{
public:
   const Snake& Enter();
   virtual void SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction );

   void AddBonus( const Vector2D& position );

   GameState State();
   void SetState( GameState state );
   void ApplyStateDelta( GameState state );

   std::vector<Snake> Players() const;
   void SetPlayers( std::vector<Snake> players );
   std::vector<Bonus> Bonuses() const;
   void SetBonuses( std::vector<Bonus> bonuses );

   const std::pair<Vector2D, Vector2D>& WorldDimensions() const { return mWorldDimensions; }

protected:
   GameRoom();

   void Run( int dt );
   virtual Snake DoEnter() = 0;

private:
   bool CheckCollisions( const Snake& snake, const Vector2D& old_head_pos );
   void OnSnakeKilled( const Snake& snake );
   void EatBonuses( const Snake& snake, const Vector2D& old_head_pos );

   boost::unordered_map<boost::uuids::uuid, Snake> mPlayers;
   mutable boost::mutex mPlayersMtx;

   std::set<Bonus> mBonuses;
   mutable boost::mutex mBonusesMtx;

   const std::pair<Vector2D, Vector2D> mWorldDimensions;
};

} // namespace game
} // namespace sbis
