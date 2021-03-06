#!/usr/bin/env gjs

// fonts.js - Font browser
//
// Copyright (c) 2017 David Lechner <david@lechnology.com>
// This file is part of the GRX graphics library.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

"use strict";

const GLib = imports.gi.GLib;
const Grx = imports.gi.Grx;
const Format = imports.format;
const Lang = imports.lang;

const DemoApp = new Lang.Class({
    Name: "DemoApp",
    Extends: Grx.Application,

    font_index: 0,
    next_index: 0,
    prev_index: [],
    font_files: undefined,
    v_offset: undefined,

    _init: function() {
        this.parent();
        this.init(null);
        this.hold();

        this.WHITE = Grx.color_get_white();
        this.BLACK = Grx.color_get_black();

        let [ok, stdout, stderr, status] = GLib.spawn_sync(null,
            ['fc-list', '--format=%{file}\n', ':scalable=False'], null,
            GLib.SpawnFlags.SEARCH_PATH, null);
        this.font_files = stdout.toString().trim().split(/[\r\n]+/);
        this.font_files.sort();
        this.default_text_opt = Grx.TextOptions.new_full(
            // Don't want to use the dpi-aware font here so we can cram info on to small screens
            Grx.Font.load_full('lucida', 10, -1, Grx.FontWeight.REGULAR,
                Grx.FontSlant.REGULAR, Grx.FontWidth.REGULAR, false, null, null),
            this.BLACK, this.WHITE, Grx.TextHAlign.LEFT, Grx.TextVAlign.TOP);
        this.v_offset = this.default_text_opt.get_font().get_height();
    },

    vfunc_activate: function() {
        this._show_next_font(0, 0);
    },

    vfunc_event: function(event) {
        if (this.parent(event)) {
            return true;
        }

        const event_type = event.get_event_type();
        switch (event_type) {
        case Grx.EventType.KEY_DOWN:
            switch (event.get_keysym()) {
                case Grx.Key.LCASE_Q:
                case Grx.Key.BACK_SPACE:
                case Grx.Key.ESCAPE:
                    this.quit();
                    break;
                case Grx.Key.DOWN:
                    if (this.next_index != -1) {
                        this._show_next_font(0, this.next_index);
                    }
                    break;
                case Grx.Key.UP:
                    if (this.prev_index.length >= 2 || (this.prev_index.length == 1 && this.next_index == -1)) {
                        if (this.next_index != -1) {
                            this.prev_index.pop();
                        }
                        this._show_next_font(0, this.prev_index.pop());
                    }
                    break;
                case Grx.Key.HOME:
                    this._show_next_font(-this.font_files.length, 0);
                    break;
                case Grx.Key.END:
                    this._show_next_font(this.font_files.length, 0);
                    break;
                case Grx.Key.PAGE_UP:
                    this._show_next_font(-10, 0);
                    break;
                case Grx.Key.PAGE_DOWN:
                    this._show_next_font(10, 0);
                    break;
                case Grx.Key.LEFT:
                    this._show_next_font(-1, 0);
                    break;
                default:
                    this._show_next_font(1, 0);
                    break;
            }
            return true;
        }

        return false;
    },

    _show_next_font: function(step, start_index) {
        if (step) {
            this.prev_index.length = 0;
            this.font_index += step;
            if (this.font_index < 0) {
                this.font_index = 0;
            }
            else if (this.font_index >= this.font_files.length) {
                this.font_index = this.font_files.length - 1;
            }
        }
        Grx.clear_screen(this.WHITE);
        let file_name = this.font_files[this.font_index];
        Grx.draw_text('file: ' + file_name, 10, 10, this.default_text_opt);
        try {
            let font = Grx.font_load_from_file(file_name);
            let font_info = Format.vprintf('family: %s, style: %s, width: %d, height %d',
                [font.get_family(), font.get_style(), font.get_width(), font.get_height()]);
            Grx.draw_text(font_info, 10, 10 + this.v_offset, this.default_text_opt);
            let dump_context = Grx.Context.new_subcontext(10, 10 + this.v_offset * 2,
                Grx.get_width() - 10, Grx.get_height() - 10, null, null);
            this.next_index = font.dump(dump_context, start_index, this.BLACK, this.WHITE);
            if (this.next_index != -1 && this.next_index != start_index) {
                this.prev_index.push(start_index);
            }
        } catch (e) {
            Grx.draw_text(e.message, 10, 30 + this.v_offset, this.default_text_opt);
            logError(e);
        }
    }
});

GLib.set_prgname('fonts.js');
GLib.set_application_name('GRX3 Font Demo');

// Run the application
let app = new DemoApp();
app.run(ARGV);
