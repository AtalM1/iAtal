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

#include "iatal-about-dialog.h"

#include "config.h"

using namespace iatal;

IAtalAboutDialog::IAtalAboutDialog()
  : authors_({
      "Rémi Bois",
        "Grégoire Jadi",
        "Agathe Mollé",
        "Joseph Lark",
        "Noémi Salaün",
        "Loïc Jankowiak",
        "Hugo Mougard"
        }),
    documenters_({
        "Rémi Bois",
          "Grégoire Jadi",
          "Agathe Mollé",
          "Joseph Lark",
          "Noémi Salaün",
          "Loïc Jankowiak",
          "Hugo Mougard"
          }),
    artists_({
        "Agathe Mollé"
          })
{
  set_program_name(PACKAGE_NAME);
  set_version(PACKAGE_VERSION);
  set_copyright(PACKAGE_COPYRIGHT);
  set_license_type(Gtk::License::LICENSE_GPL_3_0);
  set_website(PACKAGE_URL);
  set_website_label(PACKAGE_NAME + Glib::ustring(" homepage"));
  set_authors(authors_);
  set_documenters(documenters_);
  set_artists(artists_);
  
  run();
}

IAtalAboutDialog::~IAtalAboutDialog()
{
  
}
