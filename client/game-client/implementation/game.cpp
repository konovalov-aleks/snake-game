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
   virtual void Run( float disp_width, float disp_height ) override;
   virtual void SetDirection(int32_t dx, int32_t dy) override;
   virtual Field GetField() override;

private:
   boost::optional<boost::uuids::uuid> mId;
   Vector2D mLastHeadPos;
   boost::posix_time::ptime mLastActionTime;

   static const int MIN_ACTION_PERIOD = 50; // мс
};

GameImpl::GameImpl() : mLastHeadPos{ 0, 0 }
{
   Config::Instance().Set( L"gameserver.Адрес", L"http://91.232.92.46" );
   Config::Instance().Set( L"Ядро.Логирование.Уровень", L"Отладочный" );
}

void GameImpl::Enter()
{
   const Snake& snake = ClientGameRoom::Instance().Enter();
   mLastHeadPos = snake.Points()[ 0 ];
   mId = snake.ID();
}

void GameImpl::Run( float disp_width, float disp_height )
{
   ClientGameRoom::Instance().Run( mLastHeadPos, Vector2D( disp_width, disp_height ) );
}

void GameImpl::SetDirection(int32_t dx, int32_t dy)
{
   boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
   if( mId && ( now - mLastActionTime ).total_milliseconds() >= MIN_ACTION_PERIOD )
   {
      mLastActionTime = now;
      ClientGameRoom::Instance().SetPlayerDirection( *mId, Vector2D( dx, dy ) );
   }
}

Field GameImpl::GetField()
{
   auto players = ClientGameRoom::Instance().Players();
   std::vector<SnakeModel> res_snakes;
   res_snakes.reserve( players.size() );
   boost::optional< SnakeModel > cur_snake;

   for( const Snake& player : players )
   {
      const auto& player_points = player.Points();
      if( player_points.empty() )
      {
         ErrorMsg( L"snake without body!" );
         continue;
      }
      std::vector<VectorModel> points;
      points.reserve( player_points.size() );
      for( const auto& p : player_points )
         points.push_back( VectorModel( p.getX(), p.getY() ) );

      Vector2D head = player_points[ 0 ];

      const bool is_my_snake = player.ID() == mId;
      if( is_my_snake )
      {
         // сглаживаем траекторию движения головы для своей змеи, чтобы не дергался экран
         head = Vector2D( ( head.getX() + mLastHeadPos.getX() * 2.0 ) / 3.0,
                          ( head.getY() + mLastHeadPos.getY() * 2.0 ) / 3.0 );
         mLastHeadPos = head;
         points[0].x = head.getX();
         points[0].y = head.getY();
      }

      Vector2D dir( 1, 0 );
      if( player_points.size() > 1 )
         dir = Vector2D( head.getX() - player_points[1].getX(),
                         head.getY() - player_points[1].getY() );
      dir.normalize();
      dir *= 0.7;

      VectorModel left_eye( head.getX() - dir.getY(), head.getY() + dir.getX() );
      VectorModel right_eye( head.getX() + dir.getY(), head.getY() - dir.getX() );
      SnakeModel snake( std::move( points ), std::move( left_eye ), std::move( right_eye ) );
      if( is_my_snake )
         cur_snake = std::move( snake );
      else
         res_snakes.push_back( std::move( snake ) );
   }

   const auto& bonuses = ClientGameRoom::Instance().Bonuses();
   std::vector<VectorModel> field_bonuses;
   field_bonuses.reserve( bonuses.size() );
   for( const auto& b : bonuses )
      field_bonuses.push_back( VectorModel( b.Position().getX(), b.Position().getY() ) );

   const auto& dimensions = ClientGameRoom::Instance().WorldDimensions();
   return Field( Walls( dimensions.first.getX(), dimensions.first.getY(),
                        dimensions.second.getX(), dimensions.second.getY() ),
                 std::move( res_snakes ), std::move( cur_snake ), std::move( field_bonuses ) );
}

} // namespace
} // namespace game
} // namespace sbis

std::shared_ptr<Game> Game::Instance()
{
   static std::shared_ptr<Game> instance = std::make_shared<sbis::game::GameImpl>();
   return instance;
}
