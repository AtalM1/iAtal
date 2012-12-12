// -*- c-basic-offset: 2; -*-
#ifndef MAP_H
#define MAP_H

#include <vector>

#include "layer.h"

/*!
  \brief A Map is made of zero or more Layer.

*/
class Map
{
  
public:
  
  /*!
    \brief Initialize an empty Map of the given size.

    \param width the width of the map.
    \param height the height of the map.
  */
  Map(unsigned int width,
      unsigned int height);

  /*!
    \brief Returns the Layer at the given level.

    \param layer An integer which denotes the chosen level.
    \return The selected Layer.
  */
  Layer& getLayer(Layer::Level level);  
  
  /*!
    \brief Default destructor.

  */
  virtual ~Map();

protected:
  
private:
  std::vector< Layer > layers_; //!< The set of Layer
};

#endif // MAP_H
