#pragma once

#include <sbis-lib/serialization/serialization.hpp>

namespace sbis
{
namespace game
{

class Vector2D
{
public:
   typedef double ValueT;

   Vector2D() : mX(), mY() {}
   Vector2D( ValueT x, ValueT y ) : mX( x ), mY( y ) {}

   ValueT getX() const { return mX; }
   ValueT getY() const { return mY; }

   void setX( ValueT x ) { mX = x; }
   void setY( ValueT y ) { mY = y; }

   void normalize();
   ValueT length() const;

   Vector2D& operator+= ( const Vector2D& other )
   {
      mX += other.mX;
      mY += other.mY;
      return *this;
   }	

   Vector2D& operator*= ( ValueT s )
   {
      mX *= s;
      mY *= s;
      return *this;
   }	

   Vector2D operator * ( ValueT s ) const { return Vector2D( mX * s, mY * s ); }
   Vector2D operator / ( ValueT s ) const { return Vector2D( mX / s, mY / s ); }

private:
   ValueT mX, mY;
};

} // namespace game

template<>
class Serialization<game::Vector2D>
{
public:
   static void Read( IObjectReader& reader, game::Vector2D& elem )
   {
      IObjectReader::IIterator& iter( reader.CreateIterator() );
      game::Vector2D::ValueT v;
      ReadValue( *iter.Get(), v );
      elem.setX( v );
      iter.Next();
      ReadValue( *iter.Get(), v );
      elem.setY( v );
   }

   static void Write( IObjectWriter& writer, const game::Vector2D& elem )
   {
      writer.BeginWriteArray();
      writer.WriteElem( elem.getX() );
      writer.WriteElem( elem.getY() );
      writer.EndWriteArray();
   }

};

} // namespace sbis
