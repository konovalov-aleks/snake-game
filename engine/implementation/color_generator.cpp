#include "engine/color_generator.h"

#include <algorithm>

namespace sbis
{
namespace game
{

UInt32 GenerateColor( int seed )
{
   const UInt32 BIG_PRIME_NUMBER = 23581;
   UInt32 x = seed * BIG_PRIME_NUMBER;
   UInt32 r = x & 0xff;
   UInt32 g = ( x >> 8 ) & 0xff;
   UInt32 b = ( x >> 16 ) & 0xff;

   if( r + g + b < 200 )
   {
      r = std::min( 255u, r + 100 );
      g = std::min( 255u, g + 100 );
      b = std::min( 255u, b + 100 );
   }

   return r | ( g << 8 ) | ( b << 16 );
}

} // namespace game
} // namespace sbis
