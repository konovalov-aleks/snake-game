#include <engine/vector2d.h>

#include <math.h>

namespace sbis
{
namespace game
{

Vector2D::ValueT Vector2D::length() const
{
   return sqrt( mX * mX + mY * mY );
}

void Vector2D::normalize()
{
   ValueT len = length();
   if( len != 0 )
   {
      mX /= len;
      mY /= len;
   }
}

} // namespace game
} // namespace sbis
