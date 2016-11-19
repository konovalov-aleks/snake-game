#pragma once

#include <boost/intrusive/unordered_set_hook.hpp>
#include <boost/uuid/uuid.hpp>

#include <vector>

#include <sbis-lib/serialization/serialization.hpp>
#include "vector2d.h"

namespace sbis
{
namespace game
{

class Snake
{
public:
   Snake();
   Snake( boost::uuids::uuid id, const Vector2D& pos, float speed,
          const Vector2D& direction, UInt32 length, UInt32 color );

   const std::vector<Vector2D>& Points() const { return mPoints; }
   std::vector<Vector2D>& Points() { return mPoints; }
   const Vector2D& Speed() const { return mSpeed; }
   UInt32 Color() const { return mColor; }
   void SetDirection( Vector2D direction );
   void Move( int dt );

   void Grow( size_t size );

   const boost::uuids::uuid& ID() const { return mId; }

private:
   Vector2D mSpeed;
   float mLinearSpeed;
   UInt32 mLength;
   boost::uuids::uuid mId;
   UInt32 mColor;
   std::vector<Vector2D> mPoints;

   friend class sbis::Serialization<game::Snake>;
};

} // namespace game

template<>
class Serialization<game::Snake>
{
public:
   static void Read( IObjectReader& reader, game::Snake& snake );
   static void Write( IObjectWriter& writer, const game::Snake& snake );
};

} // namespace sbis
