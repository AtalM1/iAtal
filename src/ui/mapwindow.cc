#include "mapwindow.h"
#include "tmx/utils.h"

MapWindow::MapWindow()
  : button("Go"),
    dxl("dx : "),
    dyl("dy : "),
    area("src/ui/img/red.png",
	 "src/ui/img/black.png",
	 "src/ui/img/white.png")
{
  set_title("Map experimentations");
  set_icon_from_file("src/ui/img/icon.png");
  
  // Button stuff
  button
    .signal_clicked()
    .connect(sigc::mem_fun(*this,
			   &MapWindow::on_button_clicked));
  
  // Layout things
  hbox.pack_start(dxl);
  hbox.pack_start(dx);
  hbox.pack_start(dyl);
  hbox.pack_start(dy);
  hbox.pack_start(button);
  layout.put(area, 0, 0);
  layout.put(hbox, 0, 500);
  add(layout);
  
  show_all_children();
}

MapWindow::~MapWindow()
{
}

void
MapWindow::on_button_clicked()
{
  int dx_int = 0;
  int dy_int = 0;
  try
    {
        dx_int = Utils::stringToInt(dx.get_text());
      dy_int = Utils::stringToInt(dy.get_text());
    }
  catch ( std::exception ex )
    {
      std::cout << "Mauvaises valeurs pour les deltas !" << std::endl;
    }
  area.move(dx_int, dy_int);
}
