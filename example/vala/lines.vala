/*
 * lines.vala
 *
 * Copyright (c) 2017 David Lechner <david@lechnology.com>
 * This file is part of the GRX graphics library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

using Grx;

const int BOUNDS = 200; // How far out of bounds to draw lines
const int COUNT = 1000; // number of lines to draw

static void activate () {
    unowned Color[] colors = Color.get_ega_colors ();
    var w = get_width ();
    var h = get_height ();
    for (var n = 0; n < COUNT; n++) {
        var x1 = Random.int_range (-BOUNDS, w + BOUNDS);
        var y1 = Random.int_range (-BOUNDS, h + BOUNDS);
        var x2 = Random.int_range (-BOUNDS, w + BOUNDS);
        var y2 = Random.int_range (-BOUNDS, h + BOUNDS);
        var c = colors[Random.int_range (0, 16)];
        draw_line (x1, y1, x2, y2, c);
    }
}

static int main (string [] args) {
    Environment.set_application_name ("GRX3 Lines Demo");
    try {
        var app = new SimpleDemoApp ();
        app.activate.connect (activate);
        return app.run (args);
    } catch (GLib.Error err) {
        critical ("%s", err.message);
        return 1;
    }
}
