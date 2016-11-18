#include "engine/game_room.h"

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace sbis
{
namespace game
{

class ClientGameRoom : public GameRoom
{
public:
   static ClientGameRoom& Instance();
   virtual void SetPlayerDirection( const boost::uuids::uuid& pid, const Vector2D& direction ) override;
   void Run( const Vector2D& viewport_center, const Vector2D& viewport_size );

protected:
   virtual Snake DoEnter() override;

private:
   ClientGameRoom();

   void FastSync();
   void FullSync();

   void SyncThreadFunc();

   boost::mutex mViewportMtx, mRunMtx;
   Vector2D mViewportCenter;
   Vector2D mViewportSize;
    
   boost::thread mSyncThread;
   boost::posix_time::ptime mLastFrameTime;
};

} // namespace game
} // namespace sbis
