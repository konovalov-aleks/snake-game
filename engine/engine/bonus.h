#pragma once

#include "vector2d.h"

namespace sbis
{
namespace game
{

class Bonus
{
public:
   Bonus() {}
   Bonus( const Vector2D& position ) : mPosition( position ) {}
   const Vector2D& Position() const { return mPosition; }

   bool operator< ( const Bonus& another ) const;

private:
   Vector2D mPosition;
};

} // namespace game

template<>
class Serialization<game::Bonus>
{
public:
   static void Read( IObjectReader& reader, game::Bonus& bonus );
   static void Write( IObjectWriter& writer, const game::Bonus& bonus );
};

} // namespace sbis
