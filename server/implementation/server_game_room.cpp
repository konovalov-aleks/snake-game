#include "server_game_room.h"

#include "engine/color_generator.h"
#include "engine/vector2d.h"
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-rpc/rpc_func.hpp>

namespace sbis
{
namespace game
{

const float SNAKE_SPEED = 20.0f;

ServerGameRoom& ServerGameRoom::Instance()
{
   static ServerGameRoom instance;
   return instance;
}

ServerGameRoom::ServerGameRoom() :
   mGameThread( boost::thread( &ServerGameRoom::GameLoopThreadFunc, this ) )
{
   PrepareBonuses();
}

void ServerGameRoom::PrepareBonuses()
{
   const size_t count = 50;
   const auto& wd = WorldDimensions();
   const int w = static_cast<int>( wd.second.getX() - wd.first.getX() );
   const int h = static_cast<int>( wd.second.getY() - wd.first.getY() );
   for( size_t i = 0; i < count; ++i )
      AddBonus( Vector2D( static_cast<float>( rand() % w - w / 2 ),
                          static_cast<float>( rand() % h - h / 2 ) ) );
}

void ServerGameRoom::GameLoopThreadFunc()
{
   const int frame_period = 10; // ms
   for( ;; )
   {
      Run( frame_period );
      boost::this_thread::sleep( boost::posix_time::milliseconds( frame_period ) );
   }
}

Snake ServerGameRoom::DoEnter()
{
   // змей будем создавать в центре поля (в прямоугольнике со сторонами = половинам размеров поля),
   // чтобы при старте сразу не врезаться в стены
   int world_width = static_cast<int>( WorldDimensions().second.getX() - WorldDimensions().first.getX() );
   int world_height = static_cast<int>( WorldDimensions().second.getY() - WorldDimensions().first.getY() );
   Vector2D initial_pos( ( static_cast<float>( rand() % world_width ) + WorldDimensions().first.getX() ) / 2,
                         ( static_cast<float>( rand() % world_height ) + WorldDimensions().first.getY() ) / 2 );
   Vector2D direction( static_cast<float>( rand() % 100 - 50 ), static_cast<float>( rand() % 100 - 50 ) );
   return Snake( GenerateUUIDRandomDevice(), std::move( initial_pos ), SNAKE_SPEED, std::move( direction ), 5 ,
                 GenerateColor( rand () ) );
}

RPC_FUNC_2( L"GameRoom.SetPlayerDirection", void, GameRoom_SetPlayerDirection,
            boost::uuids::uuid, L"pid", pid,
            Vector2D, L"dir", dir )
{
   (void)result;
   ServerGameRoom::Instance().SetPlayerDirection( pid, dir );
}

RPC_FUNC_0( L"GameRoom.Enter", Snake, GameRoom_Enter )
{
   result = ServerGameRoom::Instance().Enter();
}

RPC_FUNC_2( L"GameRoom.State", GameState, GameRoom_State,
            Vector2D, L"vp_center", vp_center,
            Vector2D, L"vp_size", vp_size )
{
   result = ServerGameRoom::Instance().State();

   // Возвращаем только объекты, которые входят в указанную область
   const float minx = vp_center.getX() - vp_size.getX() / 2.0f - SNAKE_SPEED * 2.0f;
   const float maxx = vp_center.getX() + vp_size.getX() / 2.0f + SNAKE_SPEED * 2.0f;
   const float miny = vp_center.getY() - vp_size.getY() / 2.0f - SNAKE_SPEED * 2.0f;
   const float maxy = vp_center.getY() + vp_size.getY() / 2.0f + SNAKE_SPEED * 2.0f;

   // Для змей расширяем границы на случай, если она приползет на экран в период между синхронизациями
   const float sminx = minx - SNAKE_SPEED * 2;
   const float smaxx = maxx + SNAKE_SPEED * 2;
   const float sminy = miny - SNAKE_SPEED * 2;
   const float smaxy = maxy + SNAKE_SPEED * 2;
   result.players.erase(
      std::remove_if( result.players.begin(), result.players.end(),
         [ sminx, smaxx, sminy, smaxy ]( const Snake& s )
      {
         for( const Vector2D& p : s.Points() )
            if( p.getX() >= sminx && p.getX() <= smaxx && p.getY() >= sminy && p.getY() <= smaxy )
               return false;
         return true;
      } ), result.players.end() );

   result.bonuses.erase(
      std::remove_if( result.bonuses.begin(), result.bonuses.end(),
         [ minx, maxx, miny, maxy ]( const Bonus& b )
      {
         const Vector2D& pos = b.Position();
         return pos.getX() < minx || pos.getX() > maxx || pos.getY() < miny || pos.getY() > maxy;
      } ), result.bonuses.end() );
}

RPC_FUNC_2( L"GameRoom.PartialState", GameState, GameRoom_PartialState,
            Vector2D, L"vp_center", vp_center,
            Vector2D, L"vp_size", vp_size )
{
   GameRoom_State( result, vp_center, vp_size );

   result.bonuses.clear();

   const size_t max_fast_sync_snake_len = 5;

   // возвращаем только начальные точки змей, так как только их координаты могли сильно разойтись
   // за время между синхронизациями
   for( Snake& s : result.players )
   {
      auto& points = s.Points();
      if( points.size() > max_fast_sync_snake_len )
         points.resize( max_fast_sync_snake_len );
   }
}

} // namespace game
} // namespace sbis
