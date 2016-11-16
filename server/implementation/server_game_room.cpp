#include "server_game_room.h"

#include "engine/vector2d.h"
#include <sbis-lib/types/uuid_generator.hpp>
#include <sbis-rpc/rpc_func.hpp>

namespace sbis
{
namespace game
{

ServerGameRoom& ServerGameRoom::Instance()
{
   static ServerGameRoom instance;
   return instance;
}

ServerGameRoom::ServerGameRoom() :
   mGameThread( boost::thread( &ServerGameRoom::GameLoopThreadFunc, this ) )
{}

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
   Vector2D initial_pos( ( rand() % world_width + WorldDimensions().first.getX() ) / 2,
                         ( rand() % world_height + WorldDimensions().first.getY() ) / 2 );
   Vector2D direction( rand() % 100 - 50, rand() % 100 - 50 );
   return Snake( GenerateUUIDRandomDevice(), std::move( initial_pos ), 20, std::move( direction ), 5 );
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

RPC_FUNC_0( L"GameRoom.State", GameState, GameRoom_State )
{
   result = ServerGameRoom::Instance().State();
}

} // namespace game
} // namespace sbis
