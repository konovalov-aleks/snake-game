#pragma once

#include <boost/unordered_map.hpp>
#include <boost/uuid/uuid.hpp>
#include "bonus.h"
#include "snake.h"
#include <sbis-lib/serialization/serialization.hpp>

namespace sbis
{
namespace game
{

struct GameState
{
   boost::unordered_map<boost::uuids::uuid, Snake> players;
   std::vector<Bonus> bonuses;
};

} // namespace game

template<>
class Serialization<game::GameState>
{
public:
   static void Read( IObjectReader& reader, game::GameState& state );
   static void Write( IObjectWriter& writer, const game::GameState& state );
};

} // namespace sbis
