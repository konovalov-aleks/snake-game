#pragma once

#include "engine/game_room.h"

namespace sbis
{
namespace game
{

class ServerGameRoom : public GameRoom
{
public:
   static ServerGameRoom& Instance();

private:
   ServerGameRoom();

   virtual boost::uuids::uuid DoEnter() override;
   void GameLoopThreadFunc();

   boost::thread mGameThread;
};

} // namespace game
} // namespace sbis
