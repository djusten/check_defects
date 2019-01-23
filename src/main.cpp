// Copyright (C) 2019  Diogo Justen. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>
#include <memory>
#include <fstream>

#include "camera.h"

int main(int argc, char** argv)
{
  int index = 0;

  arv_enable_interface("Fake");

  auto cam = std::make_unique<Camera>();

  if (!cam->open(index)) {
    printf("Error\n");
  } else {

    std::vector<char> data = cam->getFramebuffer();

    cam->save_to_file("output.pgm", data);
  }

  arv_shutdown();

  return 0;
}
