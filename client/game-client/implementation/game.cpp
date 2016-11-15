#include "game.hpp"

#include <boost/uuid/uuid.hpp>
#include <sbis-bl-core/end_point.hpp>
#include <sbis-bl-core/object.hpp>
#include <sbis-lib/serialization/advanced_serialization.hpp>
#include <sbis-lib/utils/config.hpp>

#include "client_game_room.h"
#include "field.hpp"
#include "engine/vector2d.h"

namespace sbis
{
namespace game
{
namespace
{

class GameImpl : public Game
{
public:
   GameImpl();

   virtual void Enter() override;
   virtual void Run() override;
   virtual void SetDirection(int32_t dx, int32_t dy) override;
   virtual Field GetField() override;

private:
   boost::uuids::uuid mId;
   Point mLastHeadPos;
};

GameImpl::GameImpl() : mLastHeadPos{ 30, 30 }
{
   Config::Instance().Set( L"gameserver.Адрес", L"http://10.76.174.14:20082" );
   Config::Instance().Set( L"Ядро.Логирование.Уровень", L"Отладочный" );
}

void GameImpl::Enter()
{
   mId = ClientGameRoom::Instance().Enter();
}

void GameImpl::Run()
{
   ClientGameRoom::Instance().Run();
}

void GameImpl::SetDirection(int32_t dx, int32_t dy)
{
   ClientGameRoom::Instance().SetPlayerDirection(mId, Vector2D(dx, dy));
}

Field GameImpl::GetField()
{
   auto players = ClientGameRoom::Instance().Players();
   std::vector<SnakeModel> res_snakes;
   res_snakes.reserve( players.size() );
   SnakeModel cur_snake( ( std::vector<Point>() ) );
   for( const auto& player : players )
   {
      const auto& player_points = player.second.Points();
      std::vector<Point> points;
      points.reserve( player_points.size() );
      for( const auto& p : player_points )
         points.push_back( Point( static_cast<int32_t>( p.getX() ),
                                  static_cast<int32_t>( p.getY() ) ) );
      SnakeModel snake( std::move( points ) );
      if( player.first == mId )
         cur_snake = std::move( snake );
      else
         res_snakes.push_back( std::move( snake ) );
   }
   Point& head = cur_snake.points.front();
   head.x = (head.x + mLastHeadPos.x * 2) / 3;
   head.y = (head.y + mLastHeadPos.y * 2) / 3;
   mLastHeadPos = head;
   return Field( Walls( 0, 0, 0, 0 ), std::move( res_snakes ), std::move( cur_snake ) );
}

} // namespace
} // namespace game
} // namespace sbis

std::shared_ptr<Game> Game::Instance()
{
   static std::shared_ptr<Game> instance = std::make_shared<sbis::game::GameImpl>();
   return instance;
}
