#pragma once

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
   Snake( const Vector2D& pos, double speed, const Vector2D& direction,
          UInt32 length );

   const std::vector<Vector2D>& Points() const { return mPoints; }
   const Vector2D& Speed() const { return mSpeed; }

   void SetDirection( Vector2D direction );
   void Move( int dt );

private:
   Vector2D mSpeed;
   double mLinearSpeed;
   UInt32 mLength;

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
