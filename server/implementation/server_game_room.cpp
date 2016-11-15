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

boost::uuids::uuid ServerGameRoom::DoEnter()
{
   return GenerateUUIDRandomDevice();
}

RPC_FUNC_2( L"GameRoom.SetPlayerDirection", void, GameRoom_SetPlayerDirection,
            boost::uuids::uuid, L"pid", pid,
            Vector2D, L"dir", dir )
{
   (void)result;
   ServerGameRoom::Instance().SetPlayerDirection( pid, dir );
}

RPC_FUNC_0( L"GameRoom.Enter", boost::uuids::uuid, GameRoom_Enter )
{
   result = ServerGameRoom::Instance().Enter();
}

RPC_FUNC_0( L"GameRoom.State", GameState, GameRoom_State )
{
   result = ServerGameRoom::Instance().State();
}

} // namespace game
} // namespace sbis
