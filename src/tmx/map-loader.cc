// -*- c-basic-offset: 2; c-indentation-style: ellemtel; -*-

//  Copyright (C) 2012

// This file is part of iAtal.

// iAtal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// iAtal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with iAtal.  If not, see <http://www.gnu.org/licenses/>.

#include "tmx/map-loader.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <gdkmm/pixbuf.h>
#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>

#include "model/tileset.h"

#include "tmx/map-loader.h"
#include "tmx/tmx-tileset.h"

using namespace tinyxml2;

void
MapLoader::loadTMXFile(XMLDocument & tmxDoc,
                       const Glib::ustring & filePath)
{
  tmxDoc.LoadFile(filePath.c_str());
  if (tmxDoc.ErrorID() != XML_SUCCESS)
  {
    if (tmxDoc.ErrorID() == XML_ERROR_FILE_NOT_FOUND)
	{
	  throw FileNotFoundException(
	    "TinyXML2 hasn't been able to find the TMX file "
	    + filePath + ".");
	}
    else
	{
	  std::ostringstream error;
	  error << "TinyXML2 hasn't been able to load the TMX"
            << "file. The TMX error number is: "
            << static_cast<int>(tmxDoc.ErrorID())
            << ".";
	  throw TmxException(error.str());
	}
  }
}


std::shared_ptr< iatal::Map > MapLoader::loadTmx(std::string tmxPath) {
  
  std::string dir = Glib::path_get_dirname(tmxPath);
  // Charger le fichier TMX	
  XMLDocument tmxDoc;
  try
  {
    loadTMXFile(tmxDoc, tmxPath);
  }
  catch(Exception e)
  {
    throw e;
  }

  // Récupération des informations générales
  XMLElement* tmx = tmxDoc.FirstChildElement("map");
  unsigned int width = tmx->IntAttribute("width"),
    height = tmx->IntAttribute("height"),
    tileWidth = tmx->IntAttribute("tilewidth"),
    tileHeight = tmx->IntAttribute("tileheight");

  if(!width || !height || !tileWidth || !tileHeight)
  {
    std::ostringstream oss;
    oss << "One of the map height, width, "
        << "tileHeight or tileWidth parsed was zero. "
        << "The map creation can go smoothly only if "
        << "this doesn't happen.";
    throw iatal::BadParametersException(oss.str());
  }

  // Vérification que la map est de type orthogonal
  if (std::string(tmx->Attribute("orientation")) != "orthogonal")
  {
    throw TmxException(
      "The TMX orientation must be orthogonal.");
  }

  // Création de l'objet Map
  auto map = std::make_shared< iatal::Map >(width,
                                            height,
                                            tileWidth,
                                            tileHeight);

  TmxTileset groundTmxTileset;
  TmxTileset obstaclesTmxTileset;
  TmxTileset objectsTmxTileset;

  // Récupération des tilesets
  XMLElement* xmlElement = tmx->FirstChildElement("tileset");
  while (xmlElement)
  {
    bool ok = true;
    XMLElement* xmlTilesetTmp;
    unsigned int firstGid = xmlElement->IntAttribute("firstgid");
    std::string source = xmlElement->Attribute("source");

    // Récupération du type de tileset (externe ou interne)
    XMLDocument tsxDoc;
    if (source == "")
	{
	  xmlTilesetTmp = xmlElement;
	}
    else
	{
	  try
      {
        loadTMXFile(tsxDoc,
                    Glib::build_filename(dir,
                                         source));
      }
	  catch(Exception e)
      {
        throw e;
      }
	  xmlTilesetTmp = tsxDoc.FirstChildElement("tileset");
	}
    // Une fois sortie, que le tileset soit interne ou externe,
    // on manipule xmlTilesetTmp pareil

    // On vérifie le name du tileset pour savoir à quel layer
    // il correspond
    TmxTileset * tmxTileset = NULL;
    Glib::ustring name =
      Glib::ustring(xmlTilesetTmp->Attribute("name"));
    if(name == "ground")
	{
	  tmxTileset = &groundTmxTileset;
	}
    else if(name == "obstacles")
	{
	  tmxTileset = &obstaclesTmxTileset;
	}
    else if(name == "objects")
	{
	  tmxTileset = &objectsTmxTileset;
	}
    else
	{
	  ok = false;
	}
    if (ok)
	{ // Si le tileset appartient bien à un layer
	  tmxTileset->setFirstGid(firstGid);
	  tmxTileset->setSpacing(
	    xmlTilesetTmp->IntAttribute("spacing"));
	  tmxTileset->setTileHeight(
	    xmlTilesetTmp->IntAttribute("tileheight"));
	  tmxTileset->setTileWidth(
	    xmlTilesetTmp->IntAttribute("tilewidth"));
	  XMLElement* element =
	    xmlTilesetTmp->FirstChildElement("image");
	  tmxTileset->setImage(
	    Glib::build_filename(dir,
                             element->Attribute("source")));
	  tmxTileset->setHeight(
	    element->IntAttribute("height"));
	  tmxTileset->setWidth(
	    element->IntAttribute("width"));
	  element = xmlTilesetTmp->FirstChildElement("tile");
	  // Parcourt de la liste des tiles définies dans le tileset
	  while (element)
      {
        std::string id = element->Attribute("id");
        unsigned int idInt =
          boost::lexical_cast<unsigned int>(id) + 1;
        XMLElement* xmlTile =
          element
          ->FirstChildElement("properties")
          ->FirstChildElement("property");
        tmxTileset
          ->setProperty(idInt,
                        std::string(xmlTile->Attribute("name"))
                        + ":"
                        + std::string(
                          xmlTile->Attribute("value")));
        element =
          element->NextSiblingElement("tile");
      }
	}
    xmlElement = xmlElement->NextSiblingElement("tileset");
  }
  
  // Récupération des layers
  xmlElement = tmx->FirstChildElement("layer");
  while (xmlElement)
  {
    TmxTileset* tmxTileset = NULL;
    iatal::Layer* layer = NULL;
    bool ok = true;
    // unsigned int firstgid = 0;
    // On vérifie le name du layer
    std::string name(xmlElement->Attribute("name"));
    if(name == "ground")
	{
	  layer = &map->getLayer(iatal::Layer::Underground);
	  tmxTileset = &groundTmxTileset;
	}
    else if(name == "obstacles")
	{
	  layer = &map->getLayer(iatal::Layer::Ground);
	  tmxTileset = &obstaclesTmxTileset;
	}
    else if(name == "objects")
	{
	  layer = &map->getLayer(iatal::Layer::Object);
	  tmxTileset = &objectsTmxTileset;
	}
    else
	{
	  ok = false;
	}
    if (ok)
	{ // Si le name correspond
	  // On vérifie que la taille est cohérente
	  if ((unsigned int) xmlElement->IntAttribute("height")
	      != map->height
	      || (unsigned int) xmlElement->IntAttribute("width")
	      != map->width)
      {
        throw TmxException(
          "layer width and map width aren't "
          + std::string("equal. Aborting loading"));
      }
	  XMLElement* xmlData = xmlElement->FirstChildElement("data");
	  // On vérifie que l'encodage est supporté
	  if (std::string(xmlData->Attribute("encoding")) != "csv")
      {
        throw TmxException(
          "The encoding "
          + std::string(xmlData->Attribute("encoding"))
          + " of one of the layers isn't supported yet."
          + " Aborting map loading.");
      }
	  std::string data = xmlData->GetText();
	  
	  // Suppression des \n et des espaces
	  std::string temp;
	  temp.resize(data.size());
	  
	  std::remove_copy(data.begin(),
                       data.end(),
                       temp.begin(),
                       '\n');

	  std::remove_copy(temp.begin(),
                       temp.end(),
                       data.begin(),
                       ' ');
	  
	  std::vector< std::string > dataVector;
	  boost::split(dataVector, data, boost::is_any_of(","));
	  
	  
	  for(size_t i = 0, size = dataVector.size(); i < size; i++)
      {
        unsigned int y = i / map->width;
        unsigned int x = i % map->width;
        std::string id(dataVector.at(i));
        unsigned int idInt =
          static_cast<unsigned int>(std::stoi(id));
        idInt = idInt
          ? idInt - tmxTileset->getFirstGid() + 1
          : idInt;
        std::string property(tmxTileset->getProperty(idInt));
        layer->setTile(x, y, property);
      }
	}
    xmlElement = xmlElement->NextSiblingElement("layer");
  }
  
  handleTileset(map->getLayer(iatal::Layer::Level::Underground)
                .getTileset(),
                groundTmxTileset);
  handleTileset(map->getLayer(iatal::Layer::Level::Ground)
                .getTileset(),
                obstaclesTmxTileset);
  handleTileset(map->getLayer(iatal::Layer::Level::Object)
                .getTileset(),
                objectsTmxTileset);
  
  return map;
}

void
MapLoader::handleTileset(iatal::Tileset & ts,
                         const TmxTileset & tmxTs)
{
  unsigned int tileHeight = tmxTs.getTileHeight(),
    tileWidth = tmxTs.getTileWidth(),
    spacing = tmxTs.getSpacing(),
    offsetX = tileWidth + spacing,
    offsetY = tileHeight + spacing,
    widthPix = tmxTs.getWidth(),
    width = (widthPix == tileWidth
             ? 1
             : (widthPix + spacing) / offsetX);
  
  
  Glib::RefPtr<Gdk::Pixbuf> image;
  try
  {
    image = Gdk::Pixbuf::create_from_file(tmxTs.getImage());
  }
  catch (const Glib::FileError & ex)
  {
    throw ex;
  }
  catch (const Gdk::PixbufError & ex)
  {
    throw ex;
  }

  
  TmxTileset::map props =
    tmxTs.getProperties();
  std::for_each(
    props.begin(),
    props.end(),
    [& image, & ts, offsetX, offsetY, width, tileWidth, tileHeight]
    (std::pair< unsigned int, std::string > p)
    {
      unsigned int index = p.first;
      Glib::ustring property = p.second;
      Glib::RefPtr< const Gdk::Pixbuf > tile =
        Gdk::Pixbuf::create_subpixbuf(image,
                                      ((index - 1) % width) * offsetX,
                                      ((index - 1) / width) * offsetY,
                                      tileWidth,
                                      tileHeight);
      ts.setImage(property, tile);
    });
  
}
