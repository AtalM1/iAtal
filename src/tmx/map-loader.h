// -*- c-basic-offset: 2; -*-
#ifndef MAP_LOADER_H
#define	MAP_LOADER_H

#include <string>

#include "exceptions/tmx/file-not-found-exception.h"
#include "exceptions/tmx/tmx-exception.h"

#include "model/map.h"

#include "tmx/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

class MapLoader {
public:

  /*!
    \brief Permet de créer et de renvoyer une Map générée à partir d'un
    fichier TMX

    \param tmxPath Le chemin du fichier TMX.
    \return La Map générée.
  */
  static Map loadTmx(string tmxPath) throw(string);

private:
  static void loadTMXFile(XMLDocument & tmxDoc,
		   const Glib::ustring & filepath)
    throw (FileNotFoundException, TmxException);
  static XMLElement* loadTsx(XMLElement* tsxElement) throw(string);

};

#endif	/* MAP_LOADER_H */
